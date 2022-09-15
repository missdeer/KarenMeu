#include <algorithm>
#include <vtextedit/vtextedit.h>

#include <QApplication>
#include <QClipboard>
#include <QCryptographicHash>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QResizeEvent>
#include <QScrollBar>
#include <QSplitter>
#include <QStackedWidget>
#include <QTimer>
#include <QVBoxLayout>
#include <QWebChannel>
#include <QWebEngineScript>
#include <QWebEngineScriptCollection>
#include <QWebEngineView>
#include <QtCore>

#include "clientview.h"
#include "clientutils.h"
#include "filecache.h"
#include "listimagesdialog.h"
#include "markdowneditor.h"
#include "networkreplyhelper.h"
#include "plantumlrunner.h"
#include "plantumlsourceeditor.h"
#include "plantumlurlcodec.h"
#include "previewpage.h"
#include "previewthemeeditor.h"
#include "settings.h"
#include "utils.h"

static const auto g_loadingGif = QStringLiteral("https://miro.medium.com/max/1000/1*CsJ05WEGfunYMLGfsT2sXA.gif");

ClientView::ClientView(QNetworkAccessManager *nam, FileCache *fileCache, QWidget *parent)
    : QWidget(parent),
      m_editorStackedWidget(new QStackedWidget(this)),
      m_splitter(new QSplitter(this)),
      m_markdownEditor(new MarkdownEditor(g_settings->markdownEditorConfig(), g_settings->textEditorParameters(), this)),
      m_plantUMLEditor(new PlantUMLSourceEditor(this)),
      m_preview(new QWebEngineView(this)),
      m_convertTimer(new QTimer),
      m_nam(nam),
      m_fileCache(fileCache)
{
    m_editorStackedWidget->addWidget(m_markdownEditor);
    m_editorStackedWidget->addWidget(m_plantUMLEditor);
    m_plantUMLEditor->setReadOnly(false);

    m_splitter->addWidget(m_editorStackedWidget);
    m_splitter->addWidget(m_preview);
    m_splitter->setStyleSheet("QSplitter:handle { border: 0 }"
                              "QSplitter { border: 0; margin: 0; padding: 0 }");
    QLayout *layout = nullptr;
    if (g_settings->markdownViewArrange() == 0)
    {
        layout = new QVBoxLayout;
        m_splitter->setOrientation(Qt::Vertical);
    }
    else
    {
        layout = new QHBoxLayout;
        m_splitter->setOrientation(Qt::Horizontal);
    }
    layout->addWidget(m_splitter);
    layout->setMargin(0);
    layout->setSpacing(0);

    setLayout(layout);
    m_splitter->setSizes(QList<int>() << width() / 2 << width() / 2);
    m_plantUMLEditor->initialize();
    connect(m_plantUMLEditor, &PlantUMLSourceEditor::contentModified, this, &ClientView::documentModified);
    m_markdownEditor->initialize();
    connect(m_markdownEditor, &MarkdownEditor::contentModified, this, &ClientView::documentModified);
    connect(this, &ClientView::formatStrong, m_markdownEditor, &MarkdownEditor::formatStrong);
    connect(this, &ClientView::formatEmphasize, m_markdownEditor, &MarkdownEditor::formatEmphasize);
    connect(this, &ClientView::formatStrikethrough, m_markdownEditor, &MarkdownEditor::formatStrikethrough);
    connect(this, &ClientView::formatInlineCode, m_markdownEditor, &MarkdownEditor::formatInlineCode);
    connect(this, &ClientView::formatCodeBlock, m_markdownEditor, &MarkdownEditor::formatCodeBlock);
    connect(this, &ClientView::formatComment, m_markdownEditor, &MarkdownEditor::formatComment);
    connect(this, &ClientView::formatOrderedList, m_markdownEditor, &MarkdownEditor::formatOrderedList);
    connect(this, &ClientView::formatUnorderedList, m_markdownEditor, &MarkdownEditor::formatUnorderedList);
    connect(this, &ClientView::formatBlockquote, m_markdownEditor, &MarkdownEditor::formatBlockquote);
    connect(this, &ClientView::formatHyperlink, m_markdownEditor, &MarkdownEditor::formatHyperlink);
    connect(this, &ClientView::formatImage, m_markdownEditor, &MarkdownEditor::formatImage);
    connect(this, &ClientView::formatNewParagraph, m_markdownEditor, &MarkdownEditor::formatNewParagraph);
    connect(this, &ClientView::formatHorizontalRule, m_markdownEditor, &MarkdownEditor::formatHorizontalRule);
    connect(this, &ClientView::formatHeader1, m_markdownEditor, &MarkdownEditor::formatHeader1);
    connect(this, &ClientView::formatHeader2, m_markdownEditor, &MarkdownEditor::formatHeader2);
    connect(this, &ClientView::formatHeader3, m_markdownEditor, &MarkdownEditor::formatHeader3);
    connect(this, &ClientView::formatHeader4, m_markdownEditor, &MarkdownEditor::formatHeader4);
    connect(this, &ClientView::formatHeader5, m_markdownEditor, &MarkdownEditor::formatHeader5);
    connect(this, &ClientView::formatHeader6, m_markdownEditor, &MarkdownEditor::formatHeader6);
    connect(this, &ClientView::formatShiftRight, m_markdownEditor, &MarkdownEditor::formatShiftRight);
    connect(this, &ClientView::formatShiftLeft, m_markdownEditor, &MarkdownEditor::formatShiftLeft);
    connect(this, &ClientView::insertText, m_markdownEditor, &MarkdownEditor::insertText);

    auto *page = new PreviewPage(nam, this);
    m_preview->setPage(page);
    connect(page, &PreviewPage::allImagesEmbeded, this, &ClientView::onAllImagesEmbeded);
    connect(page, &PreviewPage::gotAllImages, this, &ClientView::onGotAllImages);
    connect(m_markdownEditor, &MarkdownEditor::scrollValueChanged, this, &ClientView::updatePreviewScrollBar);
    Q_ASSERT(m_markdownEditor->document());
    connect(m_markdownEditor->document(), &QTextDocument::cursorPositionChanged, this, &ClientView::updatePreviewScrollBar);

    auto *channel = new QWebChannel(this);
    channel->registerObject(QStringLiteral("content"), &m_renderedContent);
    channel->registerObject(QStringLiteral("theme"), &m_themeStyle);
    channel->registerObject(QStringLiteral("wxboxwidth"), &m_wxboxWidth);
    channel->registerObject(QStringLiteral("macStyleCodeBlock"), &m_macStyleCodeBlock);
    page->setWebChannel(channel);

    m_preview->setContextMenuPolicy(Qt::NoContextMenu);
    m_preview->load(QUrl("qrc:/rc/html/index.html"));
    connect(m_preview, &QWebEngineView::loadFinished, this, &ClientView::previewLoadFinished);
    connect(page, &QWebEnginePage::pdfPrintingFinished, this, &ClientView::pdfPrintingFinished);

    page->setDevToolsPage(new QWebEnginePage);

    connect(m_convertTimer, &QTimer::timeout, this, &ClientView::convertTimeout);
    m_convertTimer->start(g_settings->autoRefreshInterval());
}

ClientView::~ClientView()
{
    Q_ASSERT(m_preview && m_preview->page() && m_preview->page()->devToolsPage());
    delete m_preview->page()->devToolsPage();
}

void ClientView::forceConvert()
{
    renderToHTML();
}

bool ClientView::maybeSave()
{
    Q_ASSERT(m_markdownEditor);
    Q_ASSERT(m_plantUMLEditor);
    if ((isCurrentMarkdownEditor() && m_markdownEditor->modify()) || (!isCurrentMarkdownEditor() && m_plantUMLEditor->modify()))
    {
        int res = QMessageBox::question(this,
                                        tr("Confirm"),
                                        tr("Modified document has not been saved, do you want to save it?"),
                                        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
                                        QMessageBox::Yes);
        if (res == QMessageBox::Cancel)
        {
            return false;
        }
        if (res == QMessageBox::Yes)
        {
            saveDocument();
        }
    }
    return true;
}

void ClientView::openDocument()
{
    if (!maybeSave())
    {
        return;
    }
    QString fileName =
        QFileDialog::getOpenFileName(this,
                                     tr("Open file"),
                                     ClientUtils::getDefaultFileSaveOpenDirectory(),
                                     tr("All supproted files (*.md *.markdown *.mdown *.puml *.plantuml *.dot *.gv);; Markdown files (*.md "
                                        "*.markdown *.mdown);; PlantUML files (*.puml *.plantuml);; Graphviz files (*.dot *.gv);; All files (*.*)"));
    if (!QFile::exists(fileName))
    {
        return;
    }
    m_markdownEditor->clear();
    m_plantUMLEditor->clear();
    openFromFile(fileName);
}

void ClientView::saveDocument()
{
    if (!QFile::exists(m_savePath) || (!m_savePath.endsWith(".md", Qt::CaseInsensitive) && !m_savePath.endsWith(".mdown", Qt::CaseInsensitive) &&
                                       !m_savePath.endsWith(".markdown", Qt::CaseInsensitive) && !m_savePath.endsWith(".puml", Qt::CaseInsensitive) &&
                                       !m_savePath.endsWith(".plantuml", Qt::CaseInsensitive) && !m_savePath.endsWith(".gv", Qt::CaseInsensitive) &&
                                       !m_savePath.endsWith(".dot", Qt::CaseInsensitive)))
    {
        saveAsDocument();
    }
    else
    {
        saveToFile(m_savePath);
    }
}

void ClientView::saveAsDocument()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("Save file"),
        QStandardPaths::writableLocation(QStandardPaths::DataLocation),
        tr("Markdown files (*.md *.markdown *.mdown);; PlantUML files (*.puml *.plantuml);; Graphviz files (*.dot *.gv);; "
           "All supproted files (*.md *.markdown *.mdown *.puml *.plantuml *.dot *.gv);; All files (*.*)"));

    if (fileName.isEmpty())
    {
        return;
    }

    auto newDocType       = guessDocumentType(fileName);
    bool differentDocType = !(guessDocumentType(m_savePath) == newDocType);
    m_savePath = fileName;
    saveToFile(m_savePath);
    if (differentDocType)
    {
        if (newDocType == MARKDOWN || newDocType == UNKNOWN)
        {
            switchToMarkdownEditor();
            m_markdownEditor->clear();
            m_markdownEditor->setContent(m_plantUMLEditor->content());
            m_markdownEditor->setSavePoint();
            m_markdownEditor->emptyUndoBuffer();
        }
        else
        {
            switchToPlantUMLEditor();
            m_plantUMLEditor->clear();
            m_plantUMLEditor->setContent(m_markdownEditor->content());
            m_plantUMLEditor->setSavePoint();
            m_plantUMLEditor->emptyUndoBuffer();
        }
    }
}

void ClientView::newDocument()
{
    if (!maybeSave())
    {
        return;
    }
    switchToMarkdownEditor();
    m_markdownEditor->clear();
    m_markdownEditor->setSavePoint();
    m_markdownEditor->emptyUndoBuffer();
    m_savePath.clear();
    static int untitledCount = 0;
    untitledCount++;
    emit setCurrentFile(tr("Untitled%1").arg(untitledCount));

    m_preview->load(QUrl("qrc:/rc/html/index.html"));
}

void ClientView::copy()
{
    if (isCurrentMarkdownEditor() && m_markdownEditor->hasFocus())
    {
        m_markdownEditor->copy();
    }
    else if (!isCurrentMarkdownEditor() && m_plantUMLEditor->hasFocus())
    {
        m_plantUMLEditor->copy();
    }
    else
    {
        m_preview->triggerPageAction(QWebEnginePage::Copy);
    }
}

void ClientView::cut()
{
    if (isCurrentMarkdownEditor() && m_markdownEditor->hasFocus())
    {
        m_markdownEditor->cut();
    }
    else if (!isCurrentMarkdownEditor() && m_plantUMLEditor->hasFocus())
    {
        m_plantUMLEditor->cut();
    }
    else
    {
        m_preview->triggerPageAction(QWebEnginePage::Cut);
    }
}

void ClientView::paste()
{
    if (isCurrentMarkdownEditor() && m_markdownEditor->hasFocus())
    {
        m_markdownEditor->paste();
    }
    else if (!isCurrentMarkdownEditor() && m_plantUMLEditor->hasFocus())
    {
        m_plantUMLEditor->paste();
    }
    else
    {
        m_preview->triggerPageAction(QWebEnginePage::Paste);
    }
}

void ClientView::selectAll()
{
    if (isCurrentMarkdownEditor() && m_markdownEditor->hasFocus())
    {
        m_markdownEditor->selectAll();
    }
    else if (!isCurrentMarkdownEditor() && m_plantUMLEditor->hasFocus())
    {
        m_plantUMLEditor->selectAll();
    }
    else
    {
        m_preview->triggerPageAction(QWebEnginePage::SelectAll);
    }
}

void ClientView::undo()
{
    if (isCurrentMarkdownEditor() && m_markdownEditor->hasFocus())
    {
        m_markdownEditor->undo();
    }
    else if (!isCurrentMarkdownEditor() && m_plantUMLEditor->hasFocus())
    {
        m_plantUMLEditor->undo();
    }
    else
    {
        m_preview->triggerPageAction(QWebEnginePage::Undo);
    }
}

void ClientView::redo()
{
    if (isCurrentMarkdownEditor() && m_markdownEditor->hasFocus())
    {
        m_markdownEditor->redo();
    }
    else if (!isCurrentMarkdownEditor() && m_plantUMLEditor->hasFocus())
    {
        m_plantUMLEditor->redo();
    }
    else
    {
        m_preview->triggerPageAction(QWebEnginePage::Redo);
    }
}

void ClientView::copyAsHTML()
{
    Q_ASSERT(m_preview);
    auto *page = dynamic_cast<PreviewPage *>(m_preview->page());
    Q_ASSERT(page);
    page->inlineImages();
}

void ClientView::copyTheFirstImage()
{
    Q_ASSERT(m_preview);
    auto *page = dynamic_cast<PreviewPage *>(m_preview->page());
    Q_ASSERT(page);
    page->copy1stImage();
}

void ClientView::listImages()
{
    Q_ASSERT(m_preview);
    auto *page = (PreviewPage *)m_preview->page();
    Q_ASSERT(page);
    page->getImages();
}

void ClientView::exportAsHTML()
{
    QString filePath =
        QFileDialog::getSaveFileName(this, tr("Export As HTML"), ClientUtils::getDefaultFileSaveOpenDirectory(), tr("HTML file (*.html)"));
    if (filePath.isEmpty())
    {
        return;
    }
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

void ClientView::exportAsPDF()
{
    QString filePath =
        QFileDialog::getSaveFileName(this, tr("Export As PDF"), ClientUtils::getDefaultFileSaveOpenDirectory(), tr("Adobe PDF file (*.pdf)"));
    if (filePath.isEmpty())
    {
        return;
    }
    m_preview->page()->printToPdf(filePath);
}

void ClientView::documentModified()
{
    m_modified = true;
    emit contentModified();
}

void ClientView::convertTimeout()
{
    if (m_modified)
    {
        m_modified = false;
        renderToHTML();
    }
}

void ClientView::updatePreviewTheme()
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
    ClientUtils::InitializeWidgetFont(m_markdownEditor);
}

void ClientView::updatePreviewMode()
{
    m_wxboxWidth.setText(g_settings->previewMode() == tr("Blog Post") ? "95%" : "70%");
}

void ClientView::updateMacStyleCodeBlock()
{
    static std::map<QString, QString> styleBackground = {
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

void ClientView::openFromFile(const QString &fileName)
{
    if (QFile::exists(fileName))
    {
        QFile f(fileName);
        QApplication::setOverrideCursor(Qt::WaitCursor);
        if (f.open(QIODevice::ReadOnly))
        {
            QByteArray ba = f.readAll();
            if (auto type = guessDocumentType(fileName); type == GRAPHVIZ || type == PLANTUML)
            {
                m_plantUMLEditor->setContent(ba);
                m_plantUMLEditor->setSavePoint();
                m_plantUMLEditor->emptyUndoBuffer();
                switchToPlantUMLEditor();
            }
            else
            {
                m_markdownEditor->setContent(ba);
                m_markdownEditor->setSavePoint();
                m_markdownEditor->emptyUndoBuffer();
                m_markdownEditor->setBasePath(QFileInfo(fileName).absolutePath());
                switchToMarkdownEditor();
            }
            f.close();
            m_savePath = fileName;
        }
        QApplication::restoreOverrideCursor();

        emit setCurrentFile(fileName);
    }
}

void ClientView::setInitialDocument(const QString &content)
{
    m_markdownEditor->setContent(content.toUtf8());
    m_markdownEditor->setSavePoint();
    m_markdownEditor->emptyUndoBuffer();
}

MarkdownEditor *ClientView::editor()
{
    return m_markdownEditor;
}

QString ClientView::selectedText()
{
    if (isCurrentMarkdownEditor())
    {
        Q_ASSERT(m_markdownEditor);
        if (m_markdownEditor->hasFocus() || !m_preview || !m_preview->hasFocus() || m_preview->selectedText().isEmpty())
        {
            QTextCursor c = m_markdownEditor->textCursor();
            return c.selectedText();
        }
    }
    if (!isCurrentMarkdownEditor())
    {
        Q_ASSERT(m_plantUMLEditor);
        if (m_plantUMLEditor->hasFocus() || !m_preview || !m_preview->hasFocus() || m_preview->selectedText().isEmpty())
        {
            return m_plantUMLEditor->getSelText();
        }
    }
    Q_ASSERT(m_preview);
    return m_preview->selectedText();
}

QString ClientView::fullText()
{
    if (isCurrentMarkdownEditor())
    {
        Q_ASSERT(m_markdownEditor);
        return m_markdownEditor->content();
    }
    Q_ASSERT(m_plantUMLEditor);
    return m_plantUMLEditor->content();
}

QSplitter *ClientView::splitter()
{
    return m_splitter;
}

void ClientView::previewLoadFinished(bool /*unused*/)
{
    updatePreviewMode();
    updateMacStyleCodeBlock();
    updatePreviewTheme();
}

void ClientView::pdfPrintingFinished(const QString &filePath, bool success)
{
    if (success)
    {
        QMessageBox::information(
            this, tr("PDF exported"), tr("PDF file has been exported to %1 successfully.").arg(QDir::toNativeSeparators(filePath)), QMessageBox::Ok);
    }
    else
    {
        QMessageBox::information(
            this, tr("PDF exporting failed"), tr("PDF file has not been exported to %1.").arg(QDir::toNativeSeparators(filePath)), QMessageBox::Ok);
    }
}

void ClientView::onRequestRemoteImageDone()
{
    auto *helper = qobject_cast<NetworkReplyHelper *>(sender());
    Q_ASSERT(helper);
    helper->deleteLater();
    auto *reply = helper->reply();
    Q_ASSERT(reply);
    auto request  = reply->request();
    auto cacheKey = request.attribute(QNetworkRequest::Attribute(QNetworkRequest::User + 1)).toString();
    auto content  = helper->content();
#if !defined(QT_NO_DEBUG)
    qDebug() << "save to" << cacheKey << "with" << content.length() << "bytes";
#endif
    if (content.isEmpty())
    {
        return;
    }
    Q_ASSERT(m_fileCache);
    m_fileCache->addItem(content, cacheKey);

    Q_ASSERT(m_preview);
    auto *page = (PreviewPage *)m_preview->page();
    Q_ASSERT(page);
    page->refreshImage(cacheKey, QString("file://%1").arg(cachePathFromPathAndKey(m_fileCache->path(), cacheKey)));
    updatePreviewScrollBar();
}

void ClientView::onRequestLocalImageDone()
{
    qDebug() << __FUNCTION__;
    auto *runner = qobject_cast<PlantUMLRunner *>(sender());
    Q_ASSERT(runner);
    runner->deleteLater();
    const auto &output   = runner->output();
    const auto &cacheKey = runner->cacheKey();
    Q_ASSERT(m_fileCache);
    m_fileCache->addItem(output, cacheKey);

    Q_ASSERT(m_preview);
    auto *page = dynamic_cast<PreviewPage *>(m_preview->page());
    Q_ASSERT(page);
    page->refreshImage(cacheKey, QString("file://%1").arg(cachePathFromPathAndKey(m_fileCache->path(), cacheKey)));
    updatePreviewScrollBar();
}

void ClientView::onAllImagesEmbeded()
{
    Q_ASSERT(m_preview);
    // get whole origin html
    m_preview->page()->toHtml([this](const QString &result) mutable {
        // inline css
        QByteArray ba = result.toUtf8();
        GoString   content {(const char *)ba.data(), static_cast<ptrdiff_t>(ba.size())};
        auto      *res = Inliner(content);
        // set back to webengine page
        QString r = QString::fromUtf8(res);

        auto *md = new QMimeData;
        md->setText(r);
        md->setHtml(r);
        QApplication::clipboard()->setMimeData(md, QClipboard::Clipboard);
        QMessageBox::information(this, tr("HTML copied"), tr("HTML content has been copied into clipboard."), QMessageBox::Ok);
    });
}

void ClientView::onGotAllImages(QStringList images)
{
    ListImagesDialog dlg(images, m_nam, this);
    dlg.exec();
}

void ClientView::setCustomPreivewThemeEditor(PreviewThemeEditor *customPreivewThemeEditor)
{
    m_customPreivewThemeEditor = customPreivewThemeEditor;
}

QWebEnginePage *ClientView::devToolPage()
{
    return m_preview->page()->devToolsPage();
}

bool ClientView::isModified()
{
    return m_modified;
}

void ClientView::setPreviewHTMLEditor(PreviewThemeEditor *previewHTMLEditor)
{
    m_previewHTMLEditor = previewHTMLEditor;
}

void ClientView::saveToFile(const QString &savePath)
{
    QFile f(savePath);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (f.open(QIODevice::Truncate | QIODevice::WriteOnly))
    {
        if (isCurrentMarkdownEditor())
        {
            f.write(m_markdownEditor->content());
            m_markdownEditor->setSavePoint();
            m_markdownEditor->setBasePath(QFileInfo(savePath).absolutePath());
        }
        else
        {
            f.write(m_plantUMLEditor->content());
            m_plantUMLEditor->setSavePoint();
        }
        f.close();
    }
    QApplication::restoreOverrideCursor();

    emit setCurrentFile(savePath);
}

void ClientView::setRenderedHTML(const QString &html)
{
    m_renderedContent.setText(html);
    Q_ASSERT(m_previewHTMLEditor);
    m_previewHTMLEditor->setContent(html.toUtf8());
}

void ClientView::downloadImages(const std::map<QString, QString> &images)
{
    for (const auto &[cacheKey, u] : qAsConst(images))
    {
        QUrl            url(u);
        QNetworkRequest req(url);
        req.setAttribute(QNetworkRequest::Http2AllowedAttribute, true);
        req.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
        req.setAttribute(static_cast<QNetworkRequest::Attribute>(QNetworkRequest::User + 1), cacheKey);
        req.setRawHeader("Referer", QString("%1://%2").arg(url.scheme(), url.host()).toUtf8());
        Q_ASSERT(m_nam);
#if !defined(QT_NO_DEBUG)
        qDebug() << "request" << u;
#endif
        auto *reply  = m_nam->get(req);
        auto *helper = new NetworkReplyHelper(reply);
        helper->setTimeout(10000);
        connect(helper, &NetworkReplyHelper::done, this, &ClientView::onRequestRemoteImageDone);
    }
}

void ClientView::preprocessMarkdown(QList<QByteArray>          &lines,
                                      QList<QByteArray>          &metaDataLines,
                                      std::map<QString, QString> &images,
                                      std::map<QString, QString> &imagesToDownload)
{
    // extract leading YAML header for some kind of markdown document, suck as Jekyll document
    QRegularExpression reMetadataSeparator(R"(^\-{3,}[\s\t]*$)");
    int                startLineIndex = 0;
    while (lines[startLineIndex].trimmed().isEmpty())
    {
        startLineIndex++;
    }
    auto match = reMetadataSeparator.match(QString(lines[startLineIndex]));
    if (match.hasMatch())
    {
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

    QRegularExpression reEmbedGraphRenderBegin(
        "^```(plantuml|puml|uml|ditaa|dot|mindmap|wbs|gantt|math|latex|salt|json|yaml|neato|circo|fdp|sfdp|osage|twopi|patchwork)[\\s\\t]*$");
    QRegularExpression reCodeBlockEnd("^```[\\s\\t]*$");
    auto               findBeginLine   = [&reEmbedGraphRenderBegin](const auto &l) { return reEmbedGraphRenderBegin.match(QString(l)).hasMatch(); };
    QStringList        graphvizEngines = {"dot", "neato", "circo", "fdp", "sfdp", "osage", "twopi", "patchwork"};
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
        QList<QByteArray> embedGraphCodeLines;
        // extract PlantUML source lines
        std::copy(it + 1, itEnd, std::back_inserter(embedGraphCodeLines));

        auto match = reEmbedGraphRenderBegin.match(QString(*it));
        auto mark  = match.captured(1);
        if (mark.endsWith("uml"))
        {
            mark = "uml";
        }
        if (!graphvizEngines.contains(mark))
        {
            if (!embedGraphCodeLines[0].startsWith("@start"))
            {
                embedGraphCodeLines.insert(0, "@start" + mark.toUtf8());
            }
            if (!embedGraphCodeLines[embedGraphCodeLines.length() - 1].startsWith("@end"))
            {
                embedGraphCodeLines.append("@end" + mark.toUtf8());
            }
        }
#if !defined(QT_NO_DEBUG)
        qDebug() << mark;
#endif
        QByteArray embedGraphCode = embedGraphCodeLines.join("\n");
        auto       md5sum         = QCryptographicHash::hash(embedGraphCode, QCryptographicHash::Md5).toHex();
        QString    cacheKey       = QString("%1-%2.png").arg(md5sum, mark);

        // insert img tag sync
        bool       hasCached   = m_fileCache->hasItem(cacheKey);
        QString    imgFilePath = hasCached ? QUrl::fromLocalFile(cachePathFromPathAndKey(m_fileCache->path(), cacheKey)).toString() : g_loadingGif;
        QByteArray tag         = QString("![%1](%2)").arg(cacheKey, imgFilePath).toUtf8();
        images.insert(std::make_pair(cacheKey, imgFilePath));
        *it = tag;
        lines.erase(it + 1, itEnd + 1);

        if (!hasCached)
        {
            // generate final image async
            if (g_settings->plantUMLRemoteServiceEnabled())
            {
                if (!m_plantUMLUrlCodec)
                {
                    m_plantUMLUrlCodec = new PlantUMLUrlCodec;
                }
                auto    encodedStr = m_plantUMLUrlCodec->Encode(embedGraphCode.toStdString());
                QString engine     = graphvizEngines.contains(mark) ? mark : "plantuml";
                QString header     = graphvizEngines.contains(mark) ? "" : "~1";
                QString embedImageUrl =
                    QString("%1%2/png/%3%4")
                        .arg(graphvizEngines.contains(mark) ? "https://plantuml.ismisv.com/" : g_settings->plantUMLRemoteServiceAddress(),
                             engine,
                             header,
                             QString::fromStdString(encodedStr));
                imagesToDownload.insert(std::make_pair(cacheKey, embedImageUrl));
            }
            else
            {
                auto *runner = new PlantUMLRunner;
                connect(runner, &PlantUMLRunner::result, this, &ClientView::onRequestLocalImageDone);
                runner->searchDefaultExecutablePaths();
                runner->setGraphvizPath(g_settings->dotPath());
                runner->setJavaPath(g_settings->javaPath());
                runner->setPlantUmlPath(g_settings->plantUMLJarPath());
                runner->setCacheKey(cacheKey);
                if (graphvizEngines.contains(mark))
                {
                    runner->runGraphviz(embedGraphCode, "png", mark);
                }
                else
                {
                    runner->runPlantUML(embedGraphCode, "png");
                }
            }
        }
    }
}

void ClientView::preprocessPlantUML(QList<QByteArray> &lines, std::map<QString, QString> &images, std::map<QString, QString> &imagesToDownload)
{
    lines.insert(0, QByteArrayLiteral("```plantuml"));
    lines.append(QByteArrayLiteral("```"));

    QList<QByteArray> dummy;
    preprocessMarkdown(lines, dummy, images, imagesToDownload);
}

void ClientView::preprocessGraphviz(QList<QByteArray> &lines, std::map<QString, QString> &images, std::map<QString, QString> &imagesToDownload)
{
    lines.insert(0, QByteArrayLiteral("@startdot"));
    lines.insert(0, QByteArrayLiteral("```plantuml"));
    lines.append(QByteArrayLiteral("@enddot"));
    lines.append(QByteArrayLiteral("```"));

    QList<QByteArray> dummy;
    preprocessMarkdown(lines, dummy, images, imagesToDownload);
}

ClientView::DocumentType ClientView::guessDocumentType(QList<QByteArray> &lines)
{
    if (auto type = guessDocumentType(m_savePath); type != UNKNOWN)
    {
        return type;
    }

    int startLineIndex = 0;
    while (lines[startLineIndex].trimmed().isEmpty())
    {
        startLineIndex++;
    }
    QRegularExpression regPlantUML(R"(^\@start(uml|ditaa|dot|mindmap|wbs|gantt|math|latex|salt|json|yaml)[\s\t]*$)");
    auto               match = regPlantUML.match(QString(lines[startLineIndex]));
    if (match.hasMatch())
    {
        return PLANTUML;
    }
    QRegularExpression regGraphviz(R"(^(strict\s+)?(graph|digraph)\s[a-zA-Z0-9_]+)");
    match = regGraphviz.match(QString(lines[startLineIndex]));
    if (match.hasMatch())
    {
        return GRAPHVIZ;
    }
    return MARKDOWN;
}

ClientView::DocumentType ClientView::guessDocumentType(const QString &fileName)
{
    if (fileName.endsWith(".md", Qt::CaseInsensitive) || fileName.endsWith(".markdown", Qt::CaseInsensitive) ||
        fileName.endsWith(".mdown", Qt::CaseInsensitive))
    {
        return MARKDOWN;
    }
    if (fileName.endsWith(".puml", Qt::CaseInsensitive) || fileName.endsWith(".plantuml", Qt::CaseInsensitive))
    {
        return PLANTUML;
    }
    if (fileName.endsWith(".dot", Qt::CaseInsensitive) || fileName.endsWith(".gv", Qt::CaseInsensitive))
    {
        return GRAPHVIZ;
    }
    return UNKNOWN;
}

void ClientView::switchToMarkdownEditor()
{
    if (isCurrentMarkdownEditor())
    {
        return;
    }
    Q_ASSERT(m_editorStackedWidget);
    m_editorStackedWidget->setCurrentIndex(0);
}

void ClientView::switchToPlantUMLEditor()
{
    if (!isCurrentMarkdownEditor())
    {
        return;
    }
    Q_ASSERT(m_editorStackedWidget);
    m_editorStackedWidget->setCurrentIndex(1);
}

bool ClientView::isCurrentMarkdownEditor()
{
    Q_ASSERT(m_editorStackedWidget);
    return m_editorStackedWidget->currentIndex() == 0;
}

void ClientView::updatePreviewScrollBar()
{
    Q_ASSERT(m_preview);
    auto *page = dynamic_cast<PreviewPage *>(m_preview->page());
    Q_ASSERT(page);

    int currentDocumentLine      = m_markdownEditor->currentDocumentLineNumber();
    int firstVisibleDocumentLine = m_markdownEditor->firstVisibleDocumentLineNumber();
    int lastVisibleDocumentLine  = m_markdownEditor->lastVisibleDocumentLineNumber();
    int documentLineCount        = m_markdownEditor->documentLineCount();
#if !defined(QT_NO_DEBUG)
    int currentEditorLine      = m_editor->currentEditorLineNumber();
    int firstVisibleEditorLine = m_editor->firstVisibleEditorLineNumber();
    int lastVisibleEditorLine  = m_editor->lastVisibleEditorLineNumber();
    int editorLineCount        = m_editor->editorLineCount();
    int visibleLineCount       = m_editor->visibleLineCount();
    qDebug() << "current document line:" << currentDocumentLine << ", first visible document line:" << firstVisibleDocumentLine
             << ", last visible document line:" << lastVisibleDocumentLine << ", document line count:" << documentLineCount
             << "current editor line:" << currentEditorLine << ", first visible editor line:" << firstVisibleEditorLine
             << ", last visible editor line:" << lastVisibleEditorLine << ", editor line count:" << editorLineCount
             << ", visible line count:" << visibleLineCount;
#endif
    if (currentDocumentLine >= firstVisibleDocumentLine && currentDocumentLine <= lastVisibleDocumentLine)
    {
        page->onEditorScrollMoved(currentDocumentLine, documentLineCount);
    }
    else
    {
        Q_ASSERT(m_markdownEditor);
        Q_ASSERT(m_markdownEditor->getTextEdit());
        auto *scrollBar = m_markdownEditor->getTextEdit()->verticalScrollBar();
        page->onEditorScrollMoved(scrollBar->value(), scrollBar->maximum());
    }
}

void ClientView::resizeEvent(QResizeEvent *event)
{
    auto sizes = m_splitter->sizes();
    if (!sizes.contains(0))
    {
        m_splitter->setSizes(QList<int>() << width() / 2 << width() / 2);
    }
    QWidget::resizeEvent(event);
}

void ClientView::renderToHTML()
{
    QByteArray ba = isCurrentMarkdownEditor() ? m_markdownEditor->content() : m_plantUMLEditor->content();
    if (ba.isEmpty())
    {
        return;
    }

    QList<QByteArray>          metaDataLines;
    std::map<QString, QString> images;
    std::map<QString, QString> imagesToDownload;
    QByteArray                 temp = ba;
    temp.replace('\r', ' ');
    QList<QByteArray> lines = temp.split('\n');

    auto docType = isCurrentMarkdownEditor() ? guessDocumentType(lines) : guessDocumentType(m_savePath);
    switch (docType)
    {
    case UNKNOWN:
        [[fallthrough]];
    case MARKDOWN:
        preprocessMarkdown(lines, metaDataLines, images, imagesToDownload);
        break;
    case PLANTUML:
        preprocessPlantUML(lines, images, imagesToDownload);
        break;
    case GRAPHVIZ:
        preprocessGraphviz(lines, images, imagesToDownload);
        break;
    }

    // compose new content block
    ba = lines.join('\n');
    doRendering(ba, metaDataLines, images);

    updatePreviewScrollBar();
    downloadImages(imagesToDownload);
}

void ClientView::doRendering(const QByteArray &ba, QList<QByteArray> &metaDataLines, std::map<QString, QString> &images)
{
    GoString content {ba.data(), static_cast<ptrdiff_t>(ba.size())};

    QByteArray style = g_settings->codeBlockStyle().toUtf8();
    GoString   styleContent {(const char *)style.data(), static_cast<ptrdiff_t>(style.size())};

    using pFMarkdownEngine                                   = char *(*)(GoString, GoString, GoUint8);
    static QMap<QString, pFMarkdownEngine> markdownEngineMap = {
        {"Goldmark", &Goldmark},
        {"Lute", &Lute},
    };

    auto  markdownEngine = markdownEngineMap[g_settings->markdownEngine()];
    auto *res            = markdownEngine(content, styleContent, g_settings->enableLineNumbers());
    auto  renderedHTML   = QString::fromUtf8(res);

    // fix h1/h2/h3 tag for style
    static auto regexpH1 = QRegularExpression(R"(<h1(\s+id=".*")?>)");
    static auto regexpH2 = QRegularExpression(R"(<h2(\s+id=".*")?>)");
    static auto regexpH3 = QRegularExpression(R"(<h3(\s+id=".*")?>)");

    renderedHTML = renderedHTML.replace(regexpH1, "<h1\\1><span>")
                       .replace("</h1>", "</span></h1>")
                       .replace(regexpH2, "<h2\\1><span>")
                       .replace("</h2>", "</span></h2>")
                       .replace(regexpH3, "<h3\\1><span>")
                       .replace("</h3>", "</span></h3>");

    if (g_settings->macTerminalStyleCodeBlock())
    {
        updateMacStyleCodeBlock();
        renderedHTML = renderedHTML.replace("<pre", "<pre class=\"macpre\"");
    }
    // add back leading YAML header as detail/summary
    if (!metaDataLines.isEmpty())
    {
        std::transform(metaDataLines.begin(), metaDataLines.end(), metaDataLines.begin(), [](const auto &line) {
            return QString(line).toHtmlEscaped().toUtf8();
        });
        QString metaDataHTML = QString("<details><summary>%1</summary>%2<hr></details>").arg(tr("Metadata"), QString(metaDataLines.join("<br>")));
        renderedHTML         = metaDataHTML + renderedHTML;
    }

    if (g_settings->markdownEngine() == "Goldmark")
    {
        // Goldmark's bug?
        for (const auto &[cacheKey, path] : images)
        {
            QString imgFilePath = QFile::exists(QUrl(path).toLocalFile()) ? path : g_loadingGif;

            renderedHTML = renderedHTML.replace(QString(R"(<img src="" alt="%1")").arg(cacheKey),
                                                QString(R"(<img src="%2" alt="%1")").arg(cacheKey, imgFilePath));
        }
    }
    setRenderedHTML(renderedHTML);

    Free(res);
}
