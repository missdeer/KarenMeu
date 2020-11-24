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
#include "markdowneditor2.h"
#include "markdownview.h"
#include "previewpage.h"
#include "renderer.h"

using pFMarkdownEngine = char *(*)(GoString, GoString, GoUint8); 
pFMarkdownEngine markdownEngine = nullptr;

MarkdownView::MarkdownView(QWidget *parent) 
    : QWidget(parent)
    , m_splitter(new QSplitter(this))
    , m_editor(new MarkdownEditor2(this))
    , m_preview(new QWebEngineView(this))
    , m_convertTimer(new QTimer)
{
    m_splitter->addWidget(m_editor);
    m_splitter->addWidget(m_preview);
    m_splitter->setStyleSheet("QSplitter:handle { border: 0 }"
                              "QSplitter { border: 0; margin: 0; padding: 0 }");
    auto *layout = new QVBoxLayout;
    layout->addWidget(m_splitter);
    
    setLayout(layout);
    m_splitter->setSizes(QList<int>() << width()/2 << width() /2);
    m_editor->initialize();
    connect(m_editor, &MarkdownEditor2::contentModified, this, &MarkdownView::documentModified);
    connect(this, &MarkdownView::formatStrong, m_editor, &MarkdownEditor2::formatStrong);
    connect(this, &MarkdownView::formatEmphasize, m_editor, &MarkdownEditor2::formatEmphasize);
    connect(this, &MarkdownView::formatStrikethrough, m_editor, &MarkdownEditor2::formatStrikethrough);
    connect(this, &MarkdownView::formatInlineCode, m_editor, &MarkdownEditor2::formatInlineCode);
    connect(this, &MarkdownView::formatCodeBlock, m_editor, &MarkdownEditor2::formatCodeBlock);
    connect(this, &MarkdownView::formatComment, m_editor, &MarkdownEditor2::formatComment);
    connect(this, &MarkdownView::formatOrderedList, m_editor, &MarkdownEditor2::formatOrderedList);
    connect(this, &MarkdownView::formatUnorderedList, m_editor, &MarkdownEditor2::formatUnorderedList);
    connect(this, &MarkdownView::formatBlockquote, m_editor, &MarkdownEditor2::formatBlockquote);
    connect(this, &MarkdownView::formatHyperlink, m_editor, &MarkdownEditor2::formatHyperlink);
    connect(this, &MarkdownView::formatImage, m_editor, &MarkdownEditor2::formatImage);
    connect(this, &MarkdownView::formatNewParagraph, m_editor, &MarkdownEditor2::formatNewParagraph);
    connect(this, &MarkdownView::formatHorizontalRule, m_editor, &MarkdownEditor2::formatHorizontalRule);
    connect(this, &MarkdownView::formatHeader1, m_editor, &MarkdownEditor2::formatHeader1);
    connect(this, &MarkdownView::formatHeader2, m_editor, &MarkdownEditor2::formatHeader2);
    connect(this, &MarkdownView::formatHeader3, m_editor, &MarkdownEditor2::formatHeader3);
    connect(this, &MarkdownView::formatHeader4, m_editor, &MarkdownEditor2::formatHeader4);
    connect(this, &MarkdownView::formatHeader5, m_editor, &MarkdownEditor2::formatHeader5);
    connect(this, &MarkdownView::formatHeader6, m_editor, &MarkdownEditor2::formatHeader6);
    connect(this, &MarkdownView::formatShiftRight, m_editor, &MarkdownEditor2::formatShiftRight);
    connect(this, &MarkdownView::formatShiftLeft, m_editor, &MarkdownEditor2::formatShiftLeft);
    
    auto *page = new PreviewPage(this);
    m_preview->setPage(page);
    connect(m_editor, &MarkdownEditor2::scrollValueChanged, page, &PreviewPage::onEditorScrollMoved);
    
    auto *channel = new QWebChannel(this);
    channel->registerObject(QStringLiteral("content"), &m_renderedContent);
    channel->registerObject(QStringLiteral("theme"), &m_themeStyle);
    channel->registerObject(QStringLiteral("wxboxwidth"), &m_wxboxWidth);
    page->setWebChannel(channel);
    
    m_preview->setContextMenuPolicy(Qt::NoContextMenu);
    m_preview->load(QUrl("qrc:/rc/html/index.html"));
    connect(m_preview, &QWebEngineView::loadFinished, this, &MarkdownView::previewLoadFinished);
    connect(page, &QWebEnginePage::pdfPrintingFinished, this, &MarkdownView::pdfPrintingFinished);
    
    updateMarkdownEngine();

    connect(m_convertTimer, &QTimer::timeout, this, &MarkdownView::convertTimeout);
    m_convertTimer->start(g_settings->autoRefreshInterval());
}

void MarkdownView::forceConvert()
{
    renderMarkdownToHTML();
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
                                                    tr("Markdown files (*.md *.markdown *.mdown);;All files (*.*)"));
    openFromFile(fileName);
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
    m_editor->emptyUndoBuffer();
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
        renderMarkdownToHTML();
    }
}

void MarkdownView::setThemeStyle()
{
    const QString& previewTheme = g_settings->previewTheme();
    QByteArray ba;
    if (previewTheme == tr("Custom"))
    {
        ba = g_settings->customPreviewThemeStyle();
    }
    else
    {
        QFile f(":/rc/theme/" + previewTheme + ".css");
        if (f.open(QIODevice::ReadOnly))
        {
            ba = f.readAll();
            f.close();
        }
    }
    m_wxboxWidth.setText(g_settings->previewMode() == tr("Blog Post") ? "95%" : "70%");
    m_themeStyle.setText(QString::fromUtf8(ba));
    m_editor->updateCodeEditorFont();
}

void MarkdownView::updateMarkdownEngine()
{
    QMap<QString, pFMarkdownEngine> m = {
        { "Goldmark", &Goldmark},
        { "Lute", &Lute},
    };
    
    markdownEngine = m[g_settings->markdownEngine()];
}

void MarkdownView::openFromFile(const QString &fileName)
{
    if (QFile::exists(fileName))
    {
        QFile f(fileName);
        QApplication::setOverrideCursor(Qt::WaitCursor);
        if (f.open(QIODevice::ReadOnly))
        {
            QByteArray ba = f.readAll();
            m_editor->setContent(ba);
            m_editor->setSavePoint();
            m_editor->emptyUndoBuffer();
            f.close();
            m_savePath = fileName;
        }
        QApplication::restoreOverrideCursor();
        
        emit setCurrentFile(fileName);
    }
}

MarkdownEditor2 *MarkdownView::editor() 
{ 
    return m_editor; 
}

QSplitter *MarkdownView::splitter()
{
    return m_splitter;
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
    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (f.open(QIODevice::Truncate | QIODevice::WriteOnly))
    {
        f.write(m_editor->content());
        f.close();
        m_editor->setSavePoint();
    }
    QApplication::restoreOverrideCursor();
    
    emit setCurrentFile(savePath);
}

void MarkdownView::setContent(const QString& html)
{
    m_renderedContent.setText(html);
}

void MarkdownView::renderMarkdownToHTML()
{
    QByteArray ba = m_editor->content();
    if (ba.isEmpty())
        return;
    GoString content{ (const char *)ba.data(), (ptrdiff_t)ba.size()};
    
    QByteArray style = g_settings->codeBlockStyle().toUtf8();
    GoString styleContent { (const char *)style.data(), (ptrdiff_t)style.size()};
    
    auto res = markdownEngine(content, styleContent, g_settings->enableLineNumbers());
    QString html = QString::fromUtf8(res);

    // fix h1/h2/h3 tag for style
    html = html.replace("<h1>", "<h1><span>").replace("</h1>", "</span></h1>")
               .replace("<h2>", "<h2><span>").replace("</h2>", "</span></h2>")
               .replace("<h3>", "<h3><span>").replace("</h3>", "</span></h3>");
    
    setContent(html);
    
    Free(res);
}
