#include <QApplication>
#include <QClipboard>
#include <QCryptographicHash>
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
#include <algorithm>

#include "markdownview.h"

#include "clientutils.h"
#include "markdowneditor4.h"
#include "plantumlurlcodec.h"
#include "previewpage.h"
#include "previewthemeeditor.h"
#include "settings.h"
#include "utils.h"

using pFMarkdownEngine          = char *(*)(GoString, GoString, GoUint8);
pFMarkdownEngine markdownEngine = nullptr;

MarkdownView::MarkdownView(QWidget *parent)
    : QWidget(parent)
    , m_splitter(new QSplitter(this))
    , m_editor(new MarkdownEditor4(g_settings->markdownEditorConfig(), this))
    , m_preview(new QWebEngineView(this))
    , m_convertTimer(new QTimer)
{
    m_splitter->addWidget(m_editor);
    m_splitter->addWidget(m_preview);
    m_splitter->setStyleSheet("QSplitter:handle { border: 0 }"
                              "QSplitter { border: 0; margin: 0; padding: 0 }");
    auto *layout = new QVBoxLayout;
    layout->addWidget(m_splitter);
    layout->setMargin(0);
    layout->setSpacing(0);

    setLayout(layout);
    m_splitter->setSizes(QList<int>() << width() / 2 << width() / 2);
    m_editor->initialize();
    connect(m_editor, &MarkdownEditor4::contentModified, this, &MarkdownView::documentModified);
    connect(this, &MarkdownView::formatStrong, m_editor, &MarkdownEditor4::formatStrong);
    connect(this, &MarkdownView::formatEmphasize, m_editor, &MarkdownEditor4::formatEmphasize);
    connect(this, &MarkdownView::formatStrikethrough, m_editor, &MarkdownEditor4::formatStrikethrough);
    connect(this, &MarkdownView::formatInlineCode, m_editor, &MarkdownEditor4::formatInlineCode);
    connect(this, &MarkdownView::formatCodeBlock, m_editor, &MarkdownEditor4::formatCodeBlock);
    connect(this, &MarkdownView::formatComment, m_editor, &MarkdownEditor4::formatComment);
    connect(this, &MarkdownView::formatOrderedList, m_editor, &MarkdownEditor4::formatOrderedList);
    connect(this, &MarkdownView::formatUnorderedList, m_editor, &MarkdownEditor4::formatUnorderedList);
    connect(this, &MarkdownView::formatBlockquote, m_editor, &MarkdownEditor4::formatBlockquote);
    connect(this, &MarkdownView::formatHyperlink, m_editor, &MarkdownEditor4::formatHyperlink);
    connect(this, &MarkdownView::formatImage, m_editor, &MarkdownEditor4::formatImage);
    connect(this, &MarkdownView::formatNewParagraph, m_editor, &MarkdownEditor4::formatNewParagraph);
    connect(this, &MarkdownView::formatHorizontalRule, m_editor, &MarkdownEditor4::formatHorizontalRule);
    connect(this, &MarkdownView::formatHeader1, m_editor, &MarkdownEditor4::formatHeader1);
    connect(this, &MarkdownView::formatHeader2, m_editor, &MarkdownEditor4::formatHeader2);
    connect(this, &MarkdownView::formatHeader3, m_editor, &MarkdownEditor4::formatHeader3);
    connect(this, &MarkdownView::formatHeader4, m_editor, &MarkdownEditor4::formatHeader4);
    connect(this, &MarkdownView::formatHeader5, m_editor, &MarkdownEditor4::formatHeader5);
    connect(this, &MarkdownView::formatHeader6, m_editor, &MarkdownEditor4::formatHeader6);
    connect(this, &MarkdownView::formatShiftRight, m_editor, &MarkdownEditor4::formatShiftRight);
    connect(this, &MarkdownView::formatShiftLeft, m_editor, &MarkdownEditor4::formatShiftLeft);

    auto *page = new PreviewPage(this);
    m_preview->setPage(page);
    connect(m_editor, &MarkdownEditor4::scrollValueChanged, page, &PreviewPage::onEditorScrollMoved);

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
    if (m_editor->modify())
    {
        // prompt user to save document first
        int res = QMessageBox::question(this,
                                        tr("Confirm"),
                                        tr("Modified document has not been saved, do you want to save it?"),
                                        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
                                        QMessageBox::Yes);
        if (res == QMessageBox::Yes)
            saveDocument();
        if (res == QMessageBox::Cancel)
            return;
    }
    QString fileName =
        QFileDialog::getOpenFileName(this, tr("Open Markdown file"), "", tr("Markdown files (*.md *.markdown *.mdown);;All files (*.*)"));
    if (!QFile::exists(fileName))
    {
        return;
    }
    m_editor->clear();
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
    Q_ASSERT(m_editor);
    if (m_editor->modify())
    {
        // prompt user to save document first
        int res = QMessageBox::question(this,
                                        tr("Confirm"),
                                        tr("Modified document has not been saved, do you want to save it?"),
                                        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
                                        QMessageBox::Yes);
        if (res == QMessageBox::Yes)
            saveDocument();
        if (res == QMessageBox::Cancel)
            return;
    }
    m_editor->clear();
    m_editor->setSavePoint();
    m_editor->emptyUndoBuffer();
    static int untitledCount = 0;
    untitledCount++;
    emit setCurrentFile(tr("Untitled%1").arg(untitledCount));
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
    emit contentModified();
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
    ClientUtils::InitializeWidgetFont(m_editor);
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
            m_editor->setBasePath(QFileInfo(fileName).absolutePath());
            f.close();
            m_savePath = fileName;
        }
        QApplication::restoreOverrideCursor();

        emit setCurrentFile(fileName);
    }
}

void MarkdownView::setInitialDocument(const QString &content)
{
    m_editor->setContent(content.toUtf8());
    m_editor->setSavePoint();
    m_editor->emptyUndoBuffer();
}

MarkdownEditor4 *MarkdownView::editor()
{
    return m_editor;
}

QString MarkdownView::selectedText() const
{
    Q_ASSERT(m_editor);
    if (m_editor->hasFocus())
    {
        QTextCursor c = m_editor->textCursor();
        return c.selectedText();
    }
    Q_ASSERT(m_preview);
    return m_preview->selectedText();
}

QString MarkdownView::fullText() const
{
    Q_ASSERT(m_editor);
    return m_editor->content();
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
        m_editor->setBasePath(QFileInfo(savePath).absolutePath());
    }
    QApplication::restoreOverrideCursor();

    emit setCurrentFile(savePath);
}

void MarkdownView::setRenderedHTML(const QString &html)
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

    // extract leading YAML header for some kind of markdown document, suck as Jekyll document
    QByteArray temp = ba;
    temp.replace('\r', ' ');
    QList<QByteArray> lines = temp.split('\n');
    QList<QByteArray> metaDataLines;
    QRegularExpression reMetadataSeparator("^\\-{3,}[\\s\\t]*$");
    int                startLineIndex = 0;
    while (lines[startLineIndex].trimmed().isEmpty())
        startLineIndex++;
    auto match = reMetadataSeparator.match(QString(lines[startLineIndex]));
    if (match.hasMatch())
    {
        auto beginLine = match.captured(0).toUtf8();
        // find the end separator line
        auto it = std::find_if(lines.begin() + startLineIndex + 1, lines.end(), [&reMetadataSeparator](const auto &l) {
            return reMetadataSeparator.match(QString(l)).hasMatch();
        });
        if (lines.end() != it)
        {
            // extract meta data lines
            std::copy(lines.begin() + startLineIndex + 1, it, std::back_inserter(metaDataLines));

            // remove meta data lines
            lines.erase(lines.begin(), it + 1);
        }
    }

    QRegularExpression rePlantUMLBegin("^```(plantuml|puml|uml|ditaa|dot|mindmap|gantt|math|latex|salt|json)[\\s\\t]*$");
    QRegularExpression reCodeBlockEnd("^```[\\s\\t]*$");
    auto               findBeginLine = [&rePlantUMLBegin](const auto &l) { return rePlantUMLBegin.match(QString(l)).hasMatch(); };
    struct PlantUMLEngine
    {
        QString engine;
        QString outputFormat;
    };
    std::map<QString, QString> engineOutputFormatMap = {
        {"uml", "svg"},
        {"dot", "svg"},
        {"ditaa", "png"},
        {"gantt", "svg"},
        {"mindmap", "svg"},
        {"math", "png"},
        {"latex", "png"},
        {"json", "png"},
        {"salt", "png"},
    };

    for (auto it = std::find_if(lines.begin(), lines.end(), findBeginLine); lines.end() != it;
         it      = std::find_if(lines.begin(), lines.end(), findBeginLine))
    {
        auto itEnd = std::find_if(it + 1, lines.end(), [&reCodeBlockEnd](const auto &l) { return reCodeBlockEnd.match(QString(l)).hasMatch(); });
        if (lines.end() == itEnd)
        {
            break;
        }
        if (std::distance(it, itEnd) < 2)
        {
            lines.erase(it, itEnd + 1);
            continue;
        }
        QList<QByteArray> plantumlLines;
        // extract PlantUML source lines
        std::copy(it + 1, itEnd, std::back_inserter(plantumlLines));

        auto match = rePlantUMLBegin.match(QString(*it));
        auto mark  = match.captured(1);
        if (mark.endsWith("uml"))
        {
            mark = "uml";
        }
        if (!plantumlLines[0].startsWith("@start"))
            plantumlLines.insert(0, "@start" + mark.toUtf8());
        if (!plantumlLines[plantumlLines.length() - 1].startsWith("@end"))
            plantumlLines.append("@end" + mark.toUtf8());
        qDebug() << mark;
        QByteArray plantumlContent = plantumlLines.join("\n");
        auto       md5sum          = QCryptographicHash::hash(plantumlContent, QCryptographicHash::Md5).toHex();
        // generate final image async
        if (!m_plantUMLUrlCodec)
            m_plantUMLUrlCodec = new PlantUMLUrlCodec;
        auto    encodedStr = m_plantUMLUrlCodec->Encode(plantumlContent.toStdString());
        QString u          = QString("https://yii.li/plantuml/%1/").arg(engineOutputFormatMap[mark]) + QString::fromStdString(encodedStr);
        // insert img tag sync
        QByteArray tag = QString("![%1](%1)").arg(u).toUtf8();
        *it            = tag;
        lines.erase(it + 1, itEnd + 1);
        // \TODO use md5 and local cache
    }

    // compose new content block
    ba = lines.join('\n');
    GoString content {(const char *)ba.data(), (ptrdiff_t)ba.size()};

    QByteArray style = g_settings->codeBlockStyle().toUtf8();
    GoString   styleContent {(const char *)style.data(), (ptrdiff_t)style.size()};

    auto    res  = markdownEngine(content, styleContent, g_settings->enableLineNumbers());
    QString html = QString::fromUtf8(res);

    // fix h1/h2/h3 tag for style
    html = html.replace(QRegularExpression("<h1(\\s+id=\".*\")?>"), "<h1\\1><span>")
               .replace("</h1>", "</span></h1>")
               .replace(QRegularExpression("<h2(\\s+id=\".*\")?>"), "<h2\\1><span>")
               .replace("</h2>", "</span></h2>")
               .replace(QRegularExpression("<h3(\\s+id=\".*\")?>"), "<h3\\1><span>")
               .replace("</h3>", "</span></h3>");

    if (g_settings->macTerminalStyleCodeBlock())
    {
        updateMacStyleCodeBlock();
        html = html.replace("<pre", "<pre class=\"macpre\"");
    }
    // add back leading YAML header as detail/summary
    if (!metaDataLines.isEmpty())
    {
        std::transform(metaDataLines.begin(), metaDataLines.end(), metaDataLines.begin(), [](const auto &line) {
            return QString(line).toHtmlEscaped().toUtf8();
        });
        QString metaDataHTML = QString("<details><summary>%1</summary>%2<hr></details>").arg(tr("Metadata"), QString(metaDataLines.join("<br>")));
        html                 = metaDataHTML + html;
    }
    setRenderedHTML(html);

    Free(res);
}
