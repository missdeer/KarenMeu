#include <QtCore>
#include <QApplication>
#include <QClipboard>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QSplitter>
#include <QWebEngineView>
#include <QWebEngineScript>
#include <QWebEngineScriptCollection>
#include <QWebChannel>
#include <QTimer>
#include "settings.h"
#include "markdowneditor.h"
#include "markdownview.h"
#include "previewpage.h"
#include "renderer.h"

std::function<char*(GoString, GoString, GoUint8)> markdownEngine;

MarkdownView::MarkdownView(QWidget *parent) 
    : QWidget(parent)
    , m_splitter(new QSplitter(this))
    , m_editor(new MarkdownEditor(this))
    , m_preview(new QWebEngineView(this))
    , m_convertTimer(new QTimer)
{
    m_splitter->addWidget(m_editor);
    m_splitter->addWidget(m_preview);
    auto *layout = new QVBoxLayout;
    layout->addWidget(m_splitter);
    
    setLayout(layout);
    m_splitter->setSizes(QList<int>() << width()/2 << width() /2);
    m_editor->initialize();
    connect(m_editor, &MarkdownEditor::contentModified, this, &MarkdownView::documentModified);
    connect(this, &MarkdownView::formatStrong, m_editor, &MarkdownEditor::formatStrong);
    connect(this, &MarkdownView::formatEmphasize, m_editor, &MarkdownEditor::formatEmphasize);
    connect(this, &MarkdownView::formatInlineCode, m_editor, &MarkdownEditor::formatInlineCode);
    connect(this, &MarkdownView::formatComment, m_editor, &MarkdownEditor::formatComment);
    connect(this, &MarkdownView::formatOrderedList, m_editor, &MarkdownEditor::formatOrderedList);
    connect(this, &MarkdownView::formatUnorderedList, m_editor, &MarkdownEditor::formatUnorderedList);
    connect(this, &MarkdownView::formatBlockquote, m_editor, &MarkdownEditor::formatBlockquote);
    connect(this, &MarkdownView::formatHyperlink, m_editor, &MarkdownEditor::formatHyperlink);
    connect(this, &MarkdownView::formatImage, m_editor, &MarkdownEditor::formatImage);
    connect(this, &MarkdownView::formatNewParagraph, m_editor, &MarkdownEditor::formatNewParagraph);
    connect(this, &MarkdownView::formatHorizontalRule, m_editor, &MarkdownEditor::formatHorizontalRule);
    connect(this, &MarkdownView::formatHeader1, m_editor, &MarkdownEditor::formatHeader1);
    connect(this, &MarkdownView::formatHeader2, m_editor, &MarkdownEditor::formatHeader2);
    connect(this, &MarkdownView::formatHeader3, m_editor, &MarkdownEditor::formatHeader3);
    connect(this, &MarkdownView::formatHeader4, m_editor, &MarkdownEditor::formatHeader4);
    connect(this, &MarkdownView::formatHeader5, m_editor, &MarkdownEditor::formatHeader5);
    connect(this, &MarkdownView::formatHeader6, m_editor, &MarkdownEditor::formatHeader6);
    connect(this, &MarkdownView::formatShiftRight, m_editor, &MarkdownEditor::formatShiftRight);
    connect(this, &MarkdownView::formatShiftLeft, m_editor, &MarkdownEditor::formatShiftLeft);
    
    PreviewPage *page = new PreviewPage(this);
    m_preview->setPage(page);
    
    auto *channel = new QWebChannel(this);
    channel->registerObject(QStringLiteral("content"), &m_renderedContent);
    m_preview->page()->setWebChannel(channel);
    
    m_preview->setContextMenuPolicy(Qt::NoContextMenu);
    m_preview->load(QUrl("qrc:/rc/html/index.html"));
    connect(m_preview, &QWebEngineView::loadFinished, this, &MarkdownView::previewLoadFinished);
    connect(m_preview->page(), &QWebEnginePage::pdfPrintingFinished, this, &MarkdownView::pdfPrintingFinished);
    
    connect(m_convertTimer, &QTimer::timeout, this, &MarkdownView::convertTimeout);
    m_convertTimer->start(g_settings->autoRefreshInterval());
}

void MarkdownView::forceConvert()
{
    convert();
}

bool MarkdownView::maybeSave()
{
    if (m_editor->modify())
    {
        int res = QMessageBox::question(this, tr("Confirm"), 
                                        tr("Modified document has not been saved, do you want to save it?"),
                                        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
                                        QMessageBox::Yes);
        if (res == QMessageBox::Cancel)
            return false;
        if (res == QMessageBox::Yes)
            saveDocument();
    }
    return true;
}

void MarkdownView::openDocument()
{
    newDocument();
    
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Markdown file"),
                                                    "",
                                                    tr("Markdown files (*.md *.markdown);;All files (*.*)"));
    if (QFile::exists(fileName))
    {
        QFile f(fileName);
        if (f.open(QIODevice::ReadOnly))
        {
            QByteArray ba = f.readAll();
            m_editor->setContent(ba);
            m_editor->setSavePoint();
            f.close();
        }
    }
}

void MarkdownView::saveDocument()
{
    if (m_savePath.isEmpty())
        saveAsDocument();
    else
        saveToFile(m_savePath);
}

void MarkdownView::saveAsDocument()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Markdown file"),
                                                    "",
                                                    tr("Markdown file (*.md);;All files (*.*)"));
    
    if (fileName.isEmpty())
        return;
    
    m_savePath = fileName;    
    saveToFile(m_savePath);
}

void MarkdownView::newDocument()
{
    if (m_editor->modify())
    {
        // prompt user to save document first
        int res = QMessageBox::question(this, tr("Confirm"), 
                                        tr("Modified document has not been saved, do you want to save it?"),
                                        QMessageBox::Yes | QMessageBox::No,
                                        QMessageBox::Yes);
        if (res == QMessageBox::Yes)
            saveDocument();
    }
    m_editor->clear();
    m_editor->setSavePoint();
}

void MarkdownView::copy()
{
    if (m_editor->hasFocus())
        m_editor->copy();
    else {
        m_preview->triggerPageAction(QWebEnginePage::Copy);
    }
}

void MarkdownView::cut()
{
    if (m_editor->hasFocus())
        m_editor->cut();
    else {
        m_preview->triggerPageAction(QWebEnginePage::Cut);
    }
}

void MarkdownView::paste()
{
    if (m_editor->hasFocus())
        m_editor->paste();
    else {
        m_preview->triggerPageAction(QWebEnginePage::Paste);
    }
}

void MarkdownView::selectAll()
{
    if (m_editor->hasFocus())
        m_editor->selectAll();
    else {
        m_preview->triggerPageAction(QWebEnginePage::SelectAll);
    }
}

void MarkdownView::undo()
{
    if (m_editor->hasFocus())
        m_editor->undo();
    else {
        m_preview->triggerPageAction(QWebEnginePage::Undo);
    }
}

void MarkdownView::redo()
{
    if (m_editor->hasFocus())
        m_editor->redo();
    else {
        m_preview->triggerPageAction(QWebEnginePage::Redo);
    }
}

void MarkdownView::copyAsHTML()
{
    // get whole origin html
    m_preview->page()->toHtml([](const QString& result) mutable {
        // inline css
        QByteArray ba = result.toUtf8();
        GoString content { (const char *)ba.data(), (ptrdiff_t)ba.size()};
        auto res = Inliner(content);
        // set back to webengine page
        QString r = QString::fromUtf8(res);
        
        auto* md = new QMimeData;
        md->setText(r);
        md->setHtml(r);
        QApplication::clipboard()->setMimeData(md, QClipboard::Clipboard);
    });
}

void MarkdownView::exportAsHTML()
{
    QString filePath = QFileDialog::getSaveFileName(this, tr("Export As HTML"),
                                                    "",
                                                    tr("HTML file (*.html)"));
    if (filePath.isEmpty())
        return;
    m_preview->page()->toHtml([filePath, this](const QString& result) mutable {
        QFile f(filePath);
        if (f.open(QIODevice::Truncate | QIODevice::WriteOnly))
        {
            QByteArray ba = result.toUtf8();
            f.write(ba);
            f.close();
            QMessageBox::information(this, tr("HTML exported"),
                                     tr("HTML file has been exported to %1 successfully.").arg(QDir::toNativeSeparators(filePath)), QMessageBox::Ok);
        }
    });
}

void MarkdownView::exportAsPDF()
{
    QString filePath = QFileDialog::getSaveFileName(this, tr("Export As PDF"),
                                                    "",
                                                    tr("Adobe PDF file (*.pdf)"));
    if (filePath.isEmpty())
        return;
    m_preview->page()->printToPdf(filePath);
}

void MarkdownView::documentModified()
{
    m_modified = true;
}

void MarkdownView::convertTimeout()
{
    if (m_modified)
    {
        m_modified = false;
        convert();
    }
}

void MarkdownView::setThemeStyle()
{
    QByteArray ba = g_settings->previewThemeContent();
    if (g_settings->markdownEngine() == "Lute")
    {
        const QString& style = g_settings->codeBlockStyle();
        QFile f(":/rc/styles/" + style + ".css");
        if (f.open(QIODevice::ReadOnly))
        {
            ba.append(f.readAll());
            f.close();
        }    
    }
    removeStyleSheet("theme");
    insertStyleSheet("theme", QString::fromUtf8(ba));
}

void MarkdownView::updateMarkdownEngine()
{
    if (g_settings->markdownEngine() == "Goldmark")
    {
        markdownEngine = [](GoString p0, GoString p1, GoUint8 p2)->char*{ return Goldmark(p0, p1, p2);};
    }
    else {
        markdownEngine = [](GoString p0, GoString p1, GoUint8 p2)->char*{ 
            Q_UNUSED(p1);
            Q_UNUSED(p2);
            return Lute(p0);
        };
    }
}

void MarkdownView::previewLoadFinished(bool)
{
    updateMarkdownEngine();
    setThemeStyle();
}

void MarkdownView::pdfPrintingFinished(const QString &filePath, bool success)
{
    if (success)
        QMessageBox::information(this, tr("PDF exported"),
                                 tr("PDF file has been exported to %1 successfully.").arg(QDir::toNativeSeparators(filePath)), QMessageBox::Ok);
    else
        QMessageBox::information(this, tr("PDF exporting failed"),
                                 tr("PDF file has not been exported to %1.").arg(QDir::toNativeSeparators(filePath)), QMessageBox::Ok);
}

void MarkdownView::resizeEvent(QResizeEvent *event)
{
    m_splitter->setSizes(QList<int>() << width()/2 << width() /2);
    QWidget::resizeEvent(event);
}

void MarkdownView::saveToFile(const QString &savePath)
{
    QFile f(savePath);
    if (f.open(QIODevice::Truncate | QIODevice::WriteOnly))
    {
        f.write(m_editor->content());
        f.close();
    }
}

void MarkdownView::setContent(const QString& html)
{
    m_renderedContent.setText(html);

    QString s = QString::fromLatin1("(function() {" \
                                    "    var code = document.getElementsByTagName('code');" \
                                    "    for (i = 0; i < code.length; i++) {" \
                                    "      code[i].classList.add(\"highlight-chroma\");" \
                                    "    }" \
                                    "})()");
    m_preview->page()->runJavaScript(s, QWebEngineScript::MainWorld);
}

void MarkdownView::convert()
{
    QByteArray ba = m_editor->content();
    GoString content{ (const char *)ba.data(), (ptrdiff_t)ba.size()};
    
    QByteArray style = g_settings->codeBlockStyle().toUtf8();
    GoString styleContent { (const char *)style.data(), (ptrdiff_t)style.size()};
    
    auto res = markdownEngine(content, styleContent, true);
    QString html = QString::fromUtf8(res);
    
    setContent(html);
    
    Free(res);
}

void MarkdownView::insertStyleSheet(const QString &name, const QString &source)
{
    QWebEngineScript script;
    QString s = QString::fromLatin1("(function() {"\
                                    "    css = document.createElement('style');"\
                                    "    css.type = 'text/css';"\
                                    "    css.id = '%1';"\
                                    "    document.head.appendChild(css);"\
                                    "    css.innerText = '%2';"\
                                    "})()").arg(name, source.simplified());
    m_preview->page()->runJavaScript(s, QWebEngineScript::MainWorld);
    
    script.setName(name);
    script.setSourceCode(s);
    script.setInjectionPoint(QWebEngineScript::DocumentReady);
    script.setRunsOnSubFrames(true);
    script.setWorldId(QWebEngineScript::MainWorld);
    m_preview->page()->scripts().insert(script);
}

void MarkdownView::removeStyleSheet(const QString &name)
{
    QString s = QString::fromLatin1("(function() {"\
                                    "    var element = document.getElementById('%1');"\
                                    "    element.outerHTML = '';"\
                                    "    delete element;"\
                                    "})()").arg(name);

    m_preview->page()->runJavaScript(s, QWebEngineScript::MainWorld);
    
    QWebEngineScript script = m_preview->page()->scripts().findScript(name);
    m_preview->page()->scripts().remove(script);
}
