#include <algorithm>

#include <QApplication>
#include <QClipboard>
#include <QFileDialog>
#include <QMessageBox>
#include <QSplitter>
#include <QTimer>
#include <QVBoxLayout>
#include <QWebChannel>
#include <QWebEngineScript>
#include <QWebEngineScriptCollection>
#include <QWebEngineView>
#include <QtCore>

#include "markdownview.h"
#include "markdowneditor2.h"
#include "previewpage.h"
#include "previewthemeeditor.h"
#include "settings.h"
#include "utils.h"

using pFMarkdownEngine          = char *(*)(GoString, GoString, GoUint8);
pFMarkdownEngine markdownEngine = nullptr;

MarkdownView::MarkdownView(QWidget *parent)
    : QWidget(parent),
      m_splitter(new QSplitter(this)),
      m_editor(new MarkdownEditor2(this)),
      m_preview(new QWebEngineView(this)),
      m_convertTimer(new QTimer)
{
    m_splitter->addWidget(m_editor);
    m_splitter->addWidget(m_preview);
    m_splitter->setStyleSheet("QSplitter:handle { border: 0 }"
                              "QSplitter { border: 0; margin: 0; padding: 0 }");
    auto *layout = new QVBoxLayout;
    layout->addWidget(m_splitter);

    setLayout(layout);
    m_splitter->setSizes(QList<int>() << width() / 2 << width() / 2);
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
    channel->registerObject(QStringLiteral("macStyleCodeBlock"), &m_macStyleCodeBlock);
    page->setWebChannel(channel);

    m_preview->setContextMenuPolicy(Qt::NoContextMenu);
    m_preview->load(QUrl("qrc:/rc/html/index.html"));
    connect(m_preview, &QWebEngineView::loadFinished, this, &MarkdownView::previewLoadFinished);
    connect(page, &QWebEnginePage::pdfPrintingFinished, this, &MarkdownView::pdfPrintingFinished);

    auto *devToolPage = new QWebEnginePage;
    page->setDevToolsPage(devToolPage);

    updateMarkdownEngine();

    connect(m_convertTimer, &QTimer::timeout, this, &MarkdownView::convertTimeout);
    m_convertTimer->start(g_settings->autoRefreshInterval());
}

MarkdownView::~MarkdownView()
{
    Q_ASSERT(m_preview && m_preview->page() && m_preview->page()->devToolsPage());
    delete m_preview->page()->devToolsPage();
}

void MarkdownView::forceConvert()
{
    renderMarkdownToHTML();
}

bool MarkdownView::maybeSave()
{
    if (m_editor->modify())
    {
        int res = QMessageBox::question(this,
                                        tr("Confirm"),
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

    QString fileName =
        QFileDialog::getOpenFileName(this, tr("Open Markdown file"), "", tr("Markdown files (*.md *.markdown *.mdown);;All files (*.*)"));
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
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Markdown file"), "", tr("Markdown file (*.md);;All files (*.*)"));

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
        int res = QMessageBox::question(this,
                                        tr("Confirm"),
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
    else
    {
        m_preview->triggerPageAction(QWebEnginePage::Copy);
    }
}

void MarkdownView::cut()
{
    if (m_editor->hasFocus())
        m_editor->cut();
    else
    {
        m_preview->triggerPageAction(QWebEnginePage::Cut);
    }
}

void MarkdownView::paste()
{
    if (m_editor->hasFocus())
        m_editor->paste();
    else
    {
        m_preview->triggerPageAction(QWebEnginePage::Paste);
    }
}

void MarkdownView::selectAll()
{
    if (m_editor->hasFocus())
        m_editor->selectAll();
    else
    {
        m_preview->triggerPageAction(QWebEnginePage::SelectAll);
    }
}

void MarkdownView::undo()
{
    if (m_editor->hasFocus())
        m_editor->undo();
    else
    {
        m_preview->triggerPageAction(QWebEnginePage::Undo);
    }
}

void MarkdownView::redo()
{
    if (m_editor->hasFocus())
        m_editor->redo();
    else
    {
        m_preview->triggerPageAction(QWebEnginePage::Redo);
    }
}

void MarkdownView::copyAsHTML()
{
    // get whole origin html
    m_preview->page()->toHtml([](const QString &result) mutable {
        // inline css
        QByteArray ba = result.toUtf8();
        GoString   content {(const char *)ba.data(), (ptrdiff_t)ba.size()};
        auto       res = Inliner(content);
        // set back to webengine page
        QString r = QString::fromUtf8(res);

        auto *md = new QMimeData;
        md->setText(r);
        md->setHtml(r);
        QApplication::clipboard()->setMimeData(md, QClipboard::Clipboard);
    });
}

void MarkdownView::exportAsHTML()
{
    QString filePath = QFileDialog::getSaveFileName(this, tr("Export As HTML"), "", tr("HTML file (*.html)"));
    if (filePath.isEmpty())
        return;
    m_preview->page()->toHtml([filePath, this](const QString &result) mutable {
        QFile f(filePath);
        if (f.open(QIODevice::Truncate | QIODevice::WriteOnly))
        {
            QByteArray ba = result.toUtf8();
            f.write(ba);
            f.close();
            QMessageBox::information(this,
                                     tr("HTML exported"),
                                     tr("HTML file has been exported to %1 successfully.").arg(QDir::toNativeSeparators(filePath)),
                                     QMessageBox::Ok);
        }
    });
}

void MarkdownView::exportAsPDF()
{
    QString filePath = QFileDialog::getSaveFileName(this, tr("Export As PDF"), "", tr("Adobe PDF file (*.pdf)"));
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

void MarkdownView::updatePreviewTheme()
{
    const auto &   previewTheme = g_settings->previewTheme();
    QByteArray     ba;
    if (previewTheme == tr("Custom"))
    {
        ba = g_settings->customPreviewThemeStyle();
        m_customPreivewThemeEditor->setContent(ba);
    }
    else
    {
        QFile f(":/rc/theme/" + previewTheme + ".css");
        if (f.open(QIODevice::ReadOnly))
        {
            ba = f.readAll();
            f.close();
        }
        m_customPreivewThemeEditor->clearAll();
    }
    m_themeStyle.setText(QString::fromUtf8(ba));
    m_editor->updateCodeEditorFont();
}

void MarkdownView::updatePreviewMode()
{
    m_wxboxWidth.setText(g_settings->previewMode() == tr("Blog Post") ? "95%" : "70%");
}

void MarkdownView::updateMarkdownEngine()
{
    QMap<QString, pFMarkdownEngine> m = {
        {"Goldmark", &Goldmark},
        {"Lute", &Lute},
    };

    markdownEngine = m[g_settings->markdownEngine()];
}

void MarkdownView::updateMacStyleCodeBlock()
{
    std::map<QString, QString> styleBackground = {
        {"abap", "#ffffff"},
        {"algol", "#ffffff"},
        {"algol_nu", "#ffffff"},
        {"arduino", "#ffffff"},
        {"autumn", "#ffffff"},
        {"api", "#000000"},
        {"borland", "#ffffff"},
        {"bw", "#ffffff"},
        {"colorful", "#ffffff"},
        {"dracula", "#282a36"},
        {"emacs", "#f8f8f8"},
        {"friendly", "#f0f0f0"},
        {"fruity", "#111111"},
        {"github", "#ffffff"},
        {"igor", "#ffffff"},
        {"lovelace", "#ffffff"},
        {"manni", "#f0f3f3"},
        {"monokai", "#272822"},
        {"monokailight", "#fafafa"},
        {"murphy", "#ffffff"},
        {"native", "#202020"},
        {"paraiso-dark", "#2f1e2e"},
        {"paraiso-light", "#e7e9db"},
        {"pastie", "#ffffff"},
        {"perldoc", "#eeeedd"},
        {"pygments", "#ffffff"},
        {"rainbow_dash", "#ffffff"},
        {"rrt", "#000000"},
        {"solarized-dark", "#002B36"},
        {"solarized-dark256", "#1c1c1c"},
        {"solarized-light", "#eee8d5"},
        {"swapoff", "#black"},
        {"tango", "#f8f8f8"},
        {"trac", "#ffffff"},
        {"vim", "#000000"},
        {"vs", "#ffffff"},
        {"xcode", "#ffffff"},
    };
    QString backgroundColor = "#000000";
    if (styleBackground.find(g_settings->codeBlockStyle()) != styleBackground.end())
        backgroundColor = styleBackground[g_settings->codeBlockStyle()];
    QString macStyle =
        QString("#nice .macpre {                                                                                                       \n"
                "  margin: 12px auto;                                                                                                  \n"
                "  box-shadow: 0 1px 2px -2px rgba(0,0,0,.16), 0 3px 6px 0 rgba(0,0,0,.12), 0 5px 12px 4px rgba(0,0,0,.09) !important; \n"
                "  border-radius: 5px;                                                                                                 \n"
                "}                                                                                                                     \n"
                "#nice .macpre:before {                                                                                                \n"
                "  content: '';                                                                                                        \n"
                "  display:%1;                                                                                                         \n"
                "  background: url(./point.png);                                                                                       \n"
                "  height: 30px;                                                                                                       \n"
                "  width: 100%;                                                                                                        \n"
                "  background-size:40px;                                                                                               \n"
                "  background-repeat: no-repeat;                                                                                       \n"
                "  background-color: %2;                                                                                               \n"
                "  margin-bottom: -7px;                                                                                                \n"
                "  border-radius: 5px;                                                                                                 \n"
                "  background-position: 10px 10px;                                                                                     \n"
                "}                                                                                                                     ")
            .arg(g_settings->macTerminalStyleCodeBlock() ? "block" : "none", backgroundColor);
    m_macStyleCodeBlock.setText(macStyle);
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
    updatePreviewMode();
    updateMacStyleCodeBlock();
    updatePreviewTheme();
}

void MarkdownView::pdfPrintingFinished(const QString &filePath, bool success)
{
    if (success)
        QMessageBox::information(
            this, tr("PDF exported"), tr("PDF file has been exported to %1 successfully.").arg(QDir::toNativeSeparators(filePath)), QMessageBox::Ok);
    else
        QMessageBox::information(
            this, tr("PDF exporting failed"), tr("PDF file has not been exported to %1.").arg(QDir::toNativeSeparators(filePath)), QMessageBox::Ok);
}

void MarkdownView::resizeEvent(QResizeEvent *event)
{
    m_splitter->setSizes(QList<int>() << width() / 2 << width() / 2);
    QWidget::resizeEvent(event);
}

void MarkdownView::setCustomPreivewThemeEditor(PreviewThemeEditor *customPreivewThemeEditor)
{
    m_customPreivewThemeEditor = customPreivewThemeEditor;
}

QWebEnginePage *MarkdownView::devToolPage()
{
    return m_preview->page()->devToolsPage();
}

void MarkdownView::setPreviewHTMLEditor(PreviewThemeEditor *previewHTMLEditor)
{
    m_previewHTMLEditor = previewHTMLEditor;
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

void MarkdownView::setContent(const QString &html)
{
    m_renderedContent.setText(html);
    Q_ASSERT(m_previewHTMLEditor);
    m_previewHTMLEditor->setContent(html.toUtf8());
}

void MarkdownView::renderMarkdownToHTML()
{
    QByteArray ba = m_editor->content();
    if (ba.isEmpty())
        return;

    // remove leading YAML header for Jekyll files
    QByteArray temp = ba;
    temp.replace('\r', ' ');
    QList<QByteArray> lines = temp.split('\n');

    QRegularExpression reBegin("\\s*\\-{3,}");
    auto               match = reBegin.match(QString(lines[0]));
    if (match.hasMatch())
    {
        QRegularExpression reEnd("^\\s*\\-{3,}$");
        int                endLine = 0;
        for (int i = 1; i < lines.length(); i++)
        {
            match = reEnd.match(QString(lines[i]));
            if (match.hasMatch())
            {
                endLine = i;
                break;
            }
        }
        if (endLine)
        {
            lines = lines.mid(endLine + 1);
        }
        ba = lines.join('\n');
    }
    // TODO: add YAML header back to final HTML

    GoString content {(const char *)ba.data(), (ptrdiff_t)ba.size()};

    QByteArray style = g_settings->codeBlockStyle().toUtf8();
    GoString   styleContent {(const char *)style.data(), (ptrdiff_t)style.size()};

    auto    res  = markdownEngine(content, styleContent, g_settings->enableLineNumbers());
    QString html = QString::fromUtf8(res);

    // fix h1/h2/h3 tag for style
    if (g_settings->markdownEngine() == "Lute")
    {
        html = html.replace(QRegularExpression("<h1 id=\".*\">"), "<h1><span>")
                   .replace("</h1>", "</span></h1>")
                   .replace(QRegularExpression("<h2 id=\".*\">"), "<h2><span>")
                   .replace("</h2>", "</span></h2>")
                   .replace(QRegularExpression("<h3 id=\".*\">"), "<h3><span>")
                   .replace("</h3>", "</span></h3>");
    }

    if (g_settings->macTerminalStyleCodeBlock())
    {
        updateMacStyleCodeBlock();
        html = html.replace("<pre", "<pre class=\"macpre\"");
    }
    setContent(html);

    Free(res);
}
