#include <QtCore>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QSplitter>
#include <QWebEngineView>
#include <QWebEngineScript>
#include <QWebEngineScriptCollection>
#include <QTimer>
#include "settings.h"
#include "markdowneditor.h"
#include "markdownview.h"
#include "renderer.h"

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

    m_preview->setContextMenuPolicy(Qt::NoContextMenu);
    m_preview->load(QUrl("qrc:/rc/html/index.html"));
    connect(m_preview, &QWebEngineView::loadFinished, this, &MarkdownView::previewLoadFinished);
    
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
    m_preview->page()->toHtml([this](const QString& result) mutable {
        // inline css
        QByteArray ba = result.toUtf8();
        GoString content { (const char *)ba.data(), (ptrdiff_t)ba.size()};
        auto res = Inliner(content);
        // set back to webengine page
        QString r = QString::fromUtf8(res);
        m_preview->page()->setHtml(r);
        // copy it
        m_preview->page()->runJavaScript(R"(function copyToClip(str) {
                                         function listener(e) {
                                           e.clipboardData.setData("text/html", str);
                                           e.clipboardData.setData("text/plain", str);
                                           e.preventDefault();
                                         }
                                         document.addEventListener("copy", listener);
                                         document.execCommand("copy");
                                         document.removeEventListener("copy", listener);
                                         };
                                         copyToClip(document.getElementById('wx-box').innerHTML);)");
    });
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
    const QString& style = g_settings->codeBlockStyle();
    QFile f(":/rc/styles/" + style + ".css");
    if (f.open(QIODevice::ReadOnly))
    {
        auto ba = f.readAll();
        QByteArray themeContent = g_settings->previewThemeContent();
        removeStyleSheet("theme", true);
        insertStyleSheet("theme", QString::fromUtf8(themeContent + ba), true);
        f.close();
    }    
}

void MarkdownView::previewLoadFinished(bool)
{
    setThemeStyle();
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
    QString s = QString::fromLatin1("(function() {"\
                                    "    document.getElementById('content-container').innerHTML = '%1';"\
                                    "})()").arg(html.simplified());
    m_preview->page()->runJavaScript(s, QWebEngineScript::ApplicationWorld);    
}

void MarkdownView::convert()
{
    QByteArray ba = m_editor->content();
    GoString content{ (const char *)ba.data(), (ptrdiff_t)ba.size()};
    
    QByteArray style = g_settings->codeBlockStyle().toUtf8();
    GoString styleContent { (const char *)style.data(), (ptrdiff_t)style.size()};
    
    //auto res = Goldmark(content, themeContent, styleContent, true);
    auto res = Lute(content);
    QString html = QString::fromUtf8(res);
    
    setContent(html);
    
    Free(res);
}

void MarkdownView::insertStyleSheet(const QString &name, const QString &source, bool immediately)
{
    QWebEngineScript script;
    QString s = QString::fromLatin1("(function() {"\
                                    "    css = document.createElement('style');"\
                                    "    css.type = 'text/css';"\
                                    "    css.id = '%1';"\
                                    "    document.head.appendChild(css);"\
                                    "    css.innerText = '%2';"\
                                    "})()").arg(name, source.simplified());
    if (immediately)
        m_preview->page()->runJavaScript(s, QWebEngineScript::ApplicationWorld);
    
    script.setName(name);
    script.setSourceCode(s);
    script.setInjectionPoint(QWebEngineScript::DocumentReady);
    script.setRunsOnSubFrames(true);
    script.setWorldId(QWebEngineScript::ApplicationWorld);
    m_preview->page()->scripts().insert(script);
}

void MarkdownView::removeStyleSheet(const QString &name, bool immediately)
{
    QString s = QString::fromLatin1("(function() {"\
                                    "    var element = document.getElementById('%1');"\
                                    "    element.outerHTML = '';"\
                                    "    delete element;"\
                                    "})()").arg(name);
    if (immediately)
        m_preview->page()->runJavaScript(s, QWebEngineScript::ApplicationWorld);
    
    QWebEngineScript script = m_preview->page()->scripts().findScript(name);
    m_preview->page()->scripts().remove(script);
}
