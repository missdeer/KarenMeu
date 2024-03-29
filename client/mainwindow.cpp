#include <QAction>
#include <QClipboard>
#include <QCloseEvent>
#include <QComboBox>
#include <QCompleter>
#include <QCoreApplication>
#include <QDesktopServices>
#include <QDockWidget>
#include <QFileDialog>
#include <QFileInfo>
#include <QFileSystemModel>
#include <QGraphicsColorizeEffect>
#include <QHash>
#include <QHeaderView>
#include <QInputDialog>
#include <QLabel>
#include <QListWidget>
#include <QMap>
#include <QMessageBox>
#include <QNetworkDiskCache>
#include <QPainter>
#include <QPlainTextEdit>
#include <QShortcut>
#include <QSignalMapper>
#include <QSplashScreen>
#include <QSplitter>
#include <QSvgRenderer>
#include <QToolBox>
#include <QTreeView>
#include <QUrl>
#include <QVBoxLayout>
#include <QWebEngineProfile>
#include <QWebEngineSettings>
#include <QWebEngineView>
#include <QWindowStateChangeEvent>
#include <QtCore>

#include "mainwindow.h"
#include "baidutranslator.h"
#include "clientutils.h"
#include "clientview.h"
#include "custompreviewthemeeditwidget.h"
#include "deepltranslator.h"
#include "filecache.h"
#include "findreplacedialog.h"
#include "googletranslator.h"
#include "markdowneditor.h"
#include "plantumlsourceeditor.h"
#include "preferencedialog.h"
#include "previewthemeeditor.h"
#include "samplexmlreader.h"
#include "settings.h"
#include "sogoutranslator.h"
#include "templatemanager.h"
#include "templatemanagerdialog.h"
#include "translateoutputwidget.h"
#include "ui_mainwindow.h"
#include "utils.h"
#include "webbrowser.h"
#include "webbrowseraddressbar.h"
#include "xmlSettings.h"
#include "youdaodict.h"
#include "youdaotranslator.h"

namespace
{
    const int   FILE_CACHE_SIZE        = 50 * 1024 * 1024;
    const int   SAMPLE_ITEM_DATA_ROLE  = Qt::UserRole;
    const int   SAMPLE_ITEM_NOTES_ROLE = Qt::UserRole + 1;
    const int   SAMPLE_ITEM_PATH_ROLE  = Qt::UserRole + 2;
    const QSize SAMPLE_ICON_SIZE(128, 128);
} // namespace

using LabelActionMap = QMap<QString, QAction *>;
using ActionLabelMap = QHash<QAction *, QString>;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      m_fileCache(new FileCache(FILE_CACHE_SIZE, this)),
      m_clientView(new ClientView(&m_nam, m_fileCache, this)),
      m_youdaoDict(new YoudaoDict(m_nam)),
      m_templateManager(new TemplateManager)
{
    auto *diskCache = new QNetworkDiskCache(this);
    diskCache->setCacheDirectory(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    m_nam.setCache(diskCache);

    ui->setupUi(this);
    statusBar()->hide();
    setCentralWidget(m_clientView);

    if (g_settings->markdownViewArrange() == 0)
    {
        ui->actionLeftRightOrTopBottomViews->setText(tr("Switch to Left/Right View"));
    }
    else
    {
        ui->actionLeftRightOrTopBottomViews->setText(tr("Switch to Top/Bottom View"));
    }

    connect(ui->actionNewDocument, &QAction::triggered, m_clientView, &ClientView::newDocument);
    connect(ui->actionOpenDocument, &QAction::triggered, m_clientView, &ClientView::openDocument);
    connect(ui->actionSaveDocument, &QAction::triggered, m_clientView, &ClientView::saveDocument);
    connect(ui->actionSaveDocumentAs, &QAction::triggered, m_clientView, &ClientView::saveAsDocument);
    connect(ui->actionCut, &QAction::triggered, m_clientView, &ClientView::cut);
    connect(ui->actionCopy, &QAction::triggered, m_clientView, &ClientView::copy);
    connect(ui->actionPaste, &QAction::triggered, m_clientView, &ClientView::paste);
    connect(ui->actionUndo, &QAction::triggered, m_clientView, &ClientView::undo);
    connect(ui->actionRedo, &QAction::triggered, m_clientView, &ClientView::redo);
    connect(ui->actionSelectAll, &QAction::triggered, m_clientView, &ClientView::selectAll);
    connect(ui->actionCopyAsHTML, &QAction::triggered, m_clientView, &ClientView::copyAsHTML);
    connect(ui->actionCopyTheFirstImage, &QAction::triggered, m_clientView, &ClientView::copyTheFirstImage);
    connect(ui->actionListImages, &QAction::triggered, m_clientView, &ClientView::listImages);
    connect(ui->actionExportAsPDF, &QAction::triggered, m_clientView, &ClientView::exportAsPDF);
    connect(ui->actionExportAsHTML, &QAction::triggered, m_clientView, &ClientView::exportAsHTML);
    connect(ui->actionStrong, &QAction::triggered, m_clientView, &ClientView::formatStrong);
    connect(ui->actionEmphasize, &QAction::triggered, m_clientView, &ClientView::formatEmphasize);
    connect(ui->actionStrikethrough, &QAction::triggered, m_clientView, &ClientView::formatStrikethrough);
    connect(ui->actionInlineCode, &QAction::triggered, m_clientView, &ClientView::formatInlineCode);
    connect(ui->actionCodeBlock, &QAction::triggered, m_clientView, &ClientView::formatCodeBlock);
    connect(ui->actionComment, &QAction::triggered, m_clientView, &ClientView::formatComment);
    connect(ui->actionOrderedList, &QAction::triggered, m_clientView, &ClientView::formatOrderedList);
    connect(ui->actionUnorderedList, &QAction::triggered, m_clientView, &ClientView::formatUnorderedList);
    connect(ui->actionBlockquote, &QAction::triggered, m_clientView, &ClientView::formatBlockquote);
    connect(ui->actionHyperlink, &QAction::triggered, m_clientView, &ClientView::formatHyperlink);
    connect(ui->actionImage, &QAction::triggered, m_clientView, &ClientView::formatImage);
    connect(ui->actionNewParagraph, &QAction::triggered, m_clientView, &ClientView::formatNewParagraph);
    connect(ui->actionHorizontalRule, &QAction::triggered, m_clientView, &ClientView::formatHorizontalRule);
    connect(ui->actionHeader1, &QAction::triggered, m_clientView, &ClientView::formatHeader1);
    connect(ui->actionHeader2, &QAction::triggered, m_clientView, &ClientView::formatHeader2);
    connect(ui->actionHeader3, &QAction::triggered, m_clientView, &ClientView::formatHeader3);
    connect(ui->actionHeader4, &QAction::triggered, m_clientView, &ClientView::formatHeader4);
    connect(ui->actionHeader5, &QAction::triggered, m_clientView, &ClientView::formatHeader5);
    connect(ui->actionHeader6, &QAction::triggered, m_clientView, &ClientView::formatHeader6);
    connect(ui->actionShiftRight, &QAction::triggered, m_clientView, &ClientView::formatShiftRight);
    connect(ui->actionShiftLeft, &QAction::triggered, m_clientView, &ClientView::formatShiftLeft);
    connect(m_clientView, &ClientView::setCurrentFile, this, &MainWindow::onSetCurrentDocument);
    connect(m_clientView, &ClientView::contentModified, this, &MainWindow::onDocumentModified);
    connect(m_youdaoDict, &YoudaoDict::result, this, &MainWindow::onYoudaoDictResult);

    ui->menuView->addAction(ui->fileToolbar->toggleViewAction());
    ui->menuView->addAction(ui->editToolbar->toggleViewAction());
    ui->menuView->addAction(ui->formatToolbar->toggleViewAction());
    ui->menuView->addAction(ui->shortcutToolbar->toggleViewAction());
    ui->menuView->addAction(ui->translateToolBar->toggleViewAction());

    for (int i = 0; i < MaxRecentFiles; ++i)
    {
        recentFileActs.at(i) = new QAction(this);
        recentFileActs.at(i)->setVisible(false);
        connect(recentFileActs.at(i), &QAction::triggered, this, &MainWindow::openRecentFile);
        ui->menuRecentFiles->addAction(recentFileActs.at(i));

        recentWorkspaceActs.at(i) = new QAction(this);
        recentWorkspaceActs.at(i)->setVisible(false);
        connect(recentWorkspaceActs.at(i), &QAction::triggered, this, &MainWindow::openRecentWorkspace);
        ui->menuRecentWorkspaces->addAction(recentWorkspaceActs.at(i));
    }

    updateNewFromTemplateMenus();

    updateTranslationActions();

    setupShortcutToolbar();

    setupDockPanels();

    applyMarkdownEditorTheme();

    m_sampleInsertSignalMapper = new QSignalMapper(this);
    connect(m_sampleInsertSignalMapper, &QSignalMapper::mappedObject, this, &MainWindow::onSampleItemInsert);

    for (auto *sampleResult : std::as_const(m_sampleResults))
    {
        reloadSamples(sampleResult);
    }

    auto mainWindowState = g_settings->mainWindowState();
    restoreState(mainWindowState);

    auto mainWindowGeometry = g_settings->mainWindowGeometry();
    restoreGeometry(mainWindowGeometry);

    auto cachePath = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    m_fileCache->setPath(cachePath);
}

void MainWindow::openFile(const QString &fileName)
{
    if (fileName.endsWith(".krm", Qt::CaseInsensitive))
    {
        openWorkspace(fileName);
    }
    if (fileName.endsWith(".md", Qt::CaseInsensitive) || fileName.endsWith(".markdown", Qt::CaseInsensitive) ||
        fileName.endsWith(".mdown", Qt::CaseInsensitive) || fileName.endsWith(".htm", Qt::CaseInsensitive) ||
        fileName.endsWith(".html", Qt::CaseInsensitive))
    {
        openDocument(fileName);
    }
}

MainWindow::~MainWindow()
{
    auto mainWindowState = saveState();
    g_settings->setMainWindowState(mainWindowState);
    auto mainWindowGeometry = saveGeometry();
    g_settings->setMainWindowGeometry(mainWindowGeometry);

    g_settings->save();

    delete m_fileCache;

    delete ui;
}

void MainWindow::openDocument(const QString &fileName)
{
    Q_ASSERT(m_clientView);

    m_clientView->newDocument();
    m_clientView->openFromFile(fileName);
}

void MainWindow::openWorkspace(const QString &fileName)
{
    Q_ASSERT(m_clientView);
    QSettings settings(fileName, xmlFormat);
    auto      content = settings.value("content").toString();
    newDocumentWithContent(content);
    auto mainWindowState = settings.value("windowState").toByteArray();
    if (!mainWindowState.isEmpty())
    {
        restoreState(mainWindowState);
    }
    auto mainWindowGeometry = settings.value("windowGeometry").toByteArray();
    if (!mainWindowGeometry.isEmpty())
    {
        restoreGeometry(mainWindowGeometry);
    }

    auto html = settings.value("youdaoDictionaryResult").toString();
    onYoudaoDictResult(html);

    html = settings.value("googleTranslateResult").toString();
    if (!html.isEmpty())
    {
        Q_ASSERT(m_googleTranslateEditor);
        m_googleTranslateEditor->onTranslated(html);
    }
    html = settings.value("baiduTranslateResult").toString();
    if (!html.isEmpty())
    {
        Q_ASSERT(m_baiduTranslateEditor);
        m_baiduTranslateEditor->onTranslated(html);
    }
    html = settings.value("youdaoTranslateResult").toString();
    if (!html.isEmpty())
    {
        Q_ASSERT(m_youdaoTranslateEditor);
        m_youdaoTranslateEditor->onTranslated(html);
    }
    html = settings.value("sogouTranslateResult").toString();
    if (!html.isEmpty())
    {
        Q_ASSERT(m_sogouTranslateEditor);
        m_sogouTranslateEditor->onTranslated(html);
    }
    html = settings.value("deepLTranslateResult").toString();
    if (!html.isEmpty())
    {
        Q_ASSERT(m_deepLTranslateEditor);
        m_deepLTranslateEditor->onTranslated(html);
    }
    onSetCurrentWorkspace(fileName);
}

void MainWindow::saveWorkspace(const QString &fileName)
{
    Q_ASSERT(m_clientView);
    QSettings settings(fileName, xmlFormat);
    settings.setValue("content", m_clientView->fullText());
    auto mainWindowState = saveState();
    settings.setValue("windowState", mainWindowState);
    auto mainWindowGeometry = saveGeometry();
    settings.setValue("windowGeometry", mainWindowGeometry);

    Q_ASSERT(m_youdaoDictionaryEditor);
    if (!m_youdaoDictionaryEditor->toPlainText().isEmpty())
    {
        auto html = m_youdaoDictionaryEditor->document()->toHtml();
        settings.setValue("youdaoDictionaryResult", html);
    }

    Q_ASSERT(m_googleTranslateEditor);
    auto html = m_googleTranslateEditor->content();
    if (!html.isEmpty())
    {
        settings.setValue("googleTranslateResult", html);
    }

    Q_ASSERT(m_baiduTranslateEditor);
    html = m_baiduTranslateEditor->content();
    if (!html.isEmpty())
    {
        settings.setValue("baiduTranslateResult", html);
    }

    Q_ASSERT(m_youdaoTranslateEditor);
    html = m_youdaoTranslateEditor->content();
    if (!html.isEmpty())
    {
        settings.setValue("youdaoTranslateResult", html);
    }

    Q_ASSERT(m_sogouTranslateEditor);
    html = m_sogouTranslateEditor->content();
    if (!html.isEmpty())
    {
        settings.setValue("sogouTranslateResult", html);
    }

    Q_ASSERT(m_deepLTranslateEditor);
    html = m_deepLTranslateEditor->content();
    if (!html.isEmpty())
    {
        settings.setValue("deepLTranslateResult", html);
    }

    settings.setValue("googleTranslate", g_settings->enableGoogleTranslate());
    settings.setValue("baiduTranslate", g_settings->enableBaiduTranslate());
    settings.setValue("youdaoTranslate", g_settings->enableYoudaoTranslate());
    settings.setValue("sogouTranslate", g_settings->enableSogouTranslate());
    settings.setValue("deepLTranslate", g_settings->enableDeepLTranslate());
    settings.setValue("previewTheme", g_settings->previewTheme());
    settings.setValue("codeBlockStyle", g_settings->codeBlockStyle());
    settings.setValue("markdownEngine", g_settings->markdownEngine());
    settings.setValue("enableLineNumbers", g_settings->enableLineNumbers());
    settings.setValue("macTerminalStyleCodeBlock", g_settings->macTerminalStyleCodeBlock());
    settings.setValue("previewMode", g_settings->previewMode());
    settings.setValue("customPreviewThemeStyle", g_settings->customPreviewThemeStyle());
    settings.setValue("codeEditorFontFamily", g_settings->codeEditorFontFamily());
    settings.setValue("codeEditorFontPointSize", g_settings->codeEditorFontPointSize());

    settings.sync();
    onSetCurrentWorkspace(fileName);
}

void MainWindow::updateWindowTitle()
{
    Q_ASSERT(m_clientView);
    if (!m_currentOpenedFile.isEmpty())
    {
        qDebug() << __FUNCTION__ << __LINE__ << m_clientView->isModified();
        setWindowTitle(QString("%1%2 - KarenMeu").arg(QFileInfo(m_currentOpenedFile).fileName(), (m_clientView->isModified() ? "*" : "")));
    }
    else
    {
        setWindowTitle("KarenMeu");
    }
}

FindReplaceDialog *MainWindow::getFindReplaceDialog()
{
    static auto *dlg = new FindReplaceDialog(m_clientView->editor(), this);
    return dlg;
}

void MainWindow::on_actionExit_triggered()
{
    QCoreApplication::quit();
}

void MainWindow::on_actionContent_triggered()
{
    QDesktopServices::openUrl(QUrl("https://minidump.info/KarenMeu/"));
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, tr("KarenMeu"), tr("KarenMeu is a Markdown based Wechat public account article editor for programmers."));
}

void MainWindow::updateTranslationActions()
{
    ui->actionGoogle->setChecked(g_settings->enableGoogleTranslate());
    ui->actionBaidu->setChecked(g_settings->enableBaiduTranslate());
    ui->actionSogou->setChecked(g_settings->enableSogouTranslate());
    ui->actionYoudao->setChecked(g_settings->enableYoudaoTranslate());
    ui->actionDeepL->setChecked(g_settings->enableDeepLTranslate());
}

void MainWindow::translateText(const QString &text)
{
    if (text.isEmpty())
    {
        return;
    }
    if (g_settings->enableGoogleTranslate())
    {
        Q_ASSERT(m_googleTranslateEditor);
        m_googleTranslateEditor->translate(text);
    }
    if (g_settings->enableBaiduTranslate())
    {
        Q_ASSERT(m_baiduTranslateEditor);
        m_baiduTranslateEditor->translate(text);
    }
    if (g_settings->enableSogouTranslate())
    {
        Q_ASSERT(m_sogouTranslateEditor);
        m_sogouTranslateEditor->translate(text);
    }
    if (g_settings->enableYoudaoTranslate())
    {
        Q_ASSERT(m_youdaoTranslateEditor);
        m_youdaoTranslateEditor->translate(text);
    }
    if (g_settings->enableDeepLTranslate())
    {
        Q_ASSERT(m_deepLTranslateEditor);
        m_deepLTranslateEditor->translate(text);
    }
}

void MainWindow::updateNewFromTemplateMenus()
{
    // clear old actions
    for (auto *action : std::as_const(m_newFromTemplateActions))
    {
        ui->menuNewFromTemplate->removeAction(action);
    }
    m_newFromTemplateActions.clear();

    // add new actions
    m_templateManager->load();
    auto templates = m_templateManager->templates();
    for (const auto &t : templates)
    {
        auto *action = new QAction(this);
        m_newFromTemplateActions.push_back(action);
        action->setText(t->templateName());
        connect(action, &QAction::triggered, this, &MainWindow::onNewFromTemplateTriggered);
        ui->menuNewFromTemplate->addAction(action);
    }
}

void MainWindow::newDocumentWithContent(const QString &content)
{
    Q_ASSERT(m_clientView);
    m_clientView->newDocument();
    m_clientView->setInitialDocument(content);
}

void MainWindow::on_actionPreference_triggered()
{
    PreferenceDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted)
    {
        m_cbPreviewMode->setCurrentText(g_settings->previewMode());
        m_cbMarkdownEngine->setCurrentText(g_settings->markdownEngine());
        m_cbCodeBlockStyle->setCurrentText(g_settings->codeBlockStyle());
        m_cbPreviewTheme->setCurrentText(g_settings->previewTheme());

        updateTranslationActions();

        m_clientView->updatePreviewTheme();
        m_clientView->updatePreviewMode();
        m_clientView->updateMacStyleCodeBlock();
        m_clientView->forceConvert();
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_clientView->maybeSave())
    {
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void MainWindow::updateRecentFileActions(const QStringList &files)
{
    int numRecentFiles = qMin(files.size(), static_cast<int>(MaxRecentFiles));

    for (int i = 0; i < numRecentFiles; ++i)
    {
        if (!QFile::exists(files.at(i)))
        {
            continue;
        }
        QString text = tr("%1. %2").arg(i + 1).arg(strippedName(files.at(i)));
        recentFileActs.at(i)->setText(text);
        recentFileActs.at(i)->setData(files.at(i));
        recentFileActs.at(i)->setVisible(true);
    }
    for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
    {
        recentFileActs[j]->setVisible(false);
    }
}

void MainWindow::updateRecentWorkspaceActions(const QStringList &files)
{
    int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

    for (int i = 0; i < numRecentFiles; ++i)
    {
        QString text = tr("%1. %2").arg(i + 1).arg(strippedName(files.at(i)));
        recentWorkspaceActs.at(i)->setText(text);
        recentWorkspaceActs.at(i)->setData(files.at(i));
        recentWorkspaceActs.at(i)->setVisible(true);
    }
    for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
    {
        recentWorkspaceActs[j]->setVisible(false);
    }
}

void MainWindow::onSetCurrentDocument(const QString &fileName)
{
    qDebug() << __FUNCTION__ << __LINE__ << fileName;
    m_currentOpenedFile = fileName;
    g_settings->setLastOpenedFilePath(fileName);
    setWindowFilePath(m_currentOpenedFile);

    updateWindowTitle();
    if (!QFile::exists(fileName))
    {
        return;
    }
    auto &settings = g_settings->getSettings();
    settings.beginGroup("recentFile");
    QStringList files = settings.value("recentFileList").toStringList();
    files.removeAll(fileName);
    files.prepend(fileName);
    while (files.size() > MaxRecentFiles)
    {
        files.removeLast();
    }

    settings.setValue("recentFileList", files);
    settings.endGroup();
    settings.sync();

    updateRecentFileActions(files);
}

void MainWindow::onSetCurrentWorkspace(const QString &fileName)
{
    m_currentOpenedFile = fileName;
    g_settings->setLastOpenedFilePath(fileName);
    setWindowFilePath(m_currentOpenedFile);

    updateWindowTitle();
    if (!QFile::exists(fileName))
    {
        return;
    }
    auto &settings = g_settings->getSettings();
    settings.beginGroup("workspace");
    QStringList files = settings.value("recentWorkspaceList").toStringList();
    files.removeAll(fileName);
    files.prepend(fileName);
    while (files.size() > MaxRecentFiles)
    {
        files.removeLast();
    }

    settings.setValue("recentWorkspaceList", files);
    settings.endGroup();
    settings.sync();

    updateRecentWorkspaceActions(files);
}

void MainWindow::onFileSystemItemActivated(const QModelIndex &index)
{
    auto fi = m_fsModel->fileInfo(index);
    if (fi.isFile())
    {
        openFile(fi.absoluteFilePath());
    }
}

void MainWindow::onCurrentPreviewModeChanged(const QString &text)
{
    if (g_settings->previewMode() != text)
    {
        g_settings->setPreviewMode(text);
        m_clientView->updatePreviewMode();
        m_clientView->forceConvert();
    }
}

void MainWindow::onCurrentMarkdownEngineChanged(const QString &text)
{
    if (g_settings->markdownEngine() != text)
    {
        g_settings->setMarkdownEngine(text);
        m_clientView->forceConvert();
    }
}

void MainWindow::onCurrentCodeBlockStyleChanged(const QString &text)
{
    if (g_settings->codeBlockStyle() != text)
    {
        g_settings->setCodeBlockStyle(text);
        m_clientView->forceConvert();
    }
}

void MainWindow::onCurrentPreviewThemeChanged(const QString &text)
{
    if (g_settings->previewTheme() != text)
    {
        g_settings->setPreviewTheme(text);
        m_clientView->updatePreviewTheme();
        m_clientView->forceConvert();
    }
}

void MainWindow::onCustomPreviewThemeChanged()
{
    if (g_settings->previewTheme() == tr("Custom"))
    {
        m_clientView->updatePreviewTheme();
        m_clientView->forceConvert();
    }
}

void MainWindow::onYoudaoDictResult(const QString &res)
{
    Q_ASSERT(m_youdaoDictionaryEditor);
    ClientUtils::setHtmlContent(m_youdaoDictionaryEditor, res);
}

void MainWindow::onNewFromTemplateTriggered()
{
    auto *act = qobject_cast<QAction *>(sender());
    Q_UNUSED(act);
    QString templateName = act->text();
    auto    tmpl         = m_templateManager->find(templateName);
    if (!tmpl)
    {
        return;
    }

    QString title;
    if (tmpl->needTitle())
    {
        title = QInputDialog::getText(this, tr("Input Title"), tr("Title for document"));
        if (title.isEmpty())
        {
            return;
        }
    }
    auto filePath = QFileDialog::getSaveFileName(this, tr("Save file to"), QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    if (filePath.isEmpty())
    {
        return;
    }
    QFileInfo finfo(filePath);
    auto      baseName    = finfo.baseName();
    auto      fileName    = tmpl->templateExecutedName(title, baseName);
    auto      fileContent = tmpl->templateExecutedContent(title, baseName);
    if (!fileName.isEmpty())
    {
        QDir dir(finfo.absoluteDir());
        if (!dir.exists())
        {
            if (!dir.mkpath(dir.absolutePath()))
            {
                QMessageBox::warning(
                    this, tr("Error"), tr("Making directory %1 failed.").arg(QDir::toNativeSeparators(dir.absolutePath())), QMessageBox::Ok);
                return;
            }
        }

        filePath = dir.absolutePath() + "/" + fileName;
        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        {
            QMessageBox::warning(this, tr("Error"), tr("Creating file at %1 failed.").arg(QDir::toNativeSeparators(filePath)), QMessageBox::Ok);
            return;
        }

        file.write(fileContent.toUtf8());
        file.close();
        // open file
        openDocument(filePath);
    }
    else
    {
        Q_ASSERT(m_clientView);
        m_clientView->newDocument();
        m_clientView->setInitialDocument(fileContent);
    }
}

void MainWindow::onDocumentModified()
{
    qDebug() << __FUNCTION__ << __LINE__;
    updateWindowTitle();
}

void MainWindow::onWebBrowserSelectionChangedTimeout()
{
    auto text = m_webBrowser->page()->selectedText();
    if (!text.isEmpty())
    {
        translateText(text);
    }
}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void MainWindow::openRecentFile()
{
    auto *action = qobject_cast<QAction *>(sender());
    Q_ASSERT(action);
    openDocument(action->data().toString());
}

void MainWindow::openRecentWorkspace()
{
    auto *action = qobject_cast<QAction *>(sender());
    Q_ASSERT(action);
    openWorkspace(action->data().toString());
}

void MainWindow::on_actionClearRecentFilesList_triggered()
{
    auto &      settings = g_settings->getSettings();
    QStringList files;
    settings.beginGroup("recentFile");
    settings.setValue("recentFileList", files);
    settings.endGroup();
    settings.sync();

    updateRecentFileActions(files);
}

void MainWindow::applyMarkdownEditorTheme()
{
    Q_ASSERT(g_settings);
}

void MainWindow::setupWebBrowserPane()
{
    auto *defaultProfile = QWebEngineProfile::defaultProfile();

    defaultProfile->setPersistentCookiesPolicy(QWebEngineProfile::AllowPersistentCookies);

    auto *defaultSettings = defaultProfile->settings();

    defaultSettings->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    defaultSettings->setAttribute(QWebEngineSettings::ScrollAnimatorEnabled, true);
    defaultSettings->setAttribute(QWebEngineSettings::PluginsEnabled, true);

    auto *browserDock = new QDockWidget(tr("WebBrowser"), this);
    browserDock->setObjectName("webBrowser");
    auto *toggleViewAction = browserDock->toggleViewAction();
    toggleViewAction->setShortcut(QKeySequence("Shift+Alt+W"));
    ui->menuView->addAction(toggleViewAction);

    auto *browserContainer = new QWidget(browserDock);
    m_webBrowser           = new WebBrowser(browserContainer);
    auto *browserLayout    = new QVBoxLayout();
    auto *browserToolBar   = new QToolBar(browserContainer);
    browserToolBar->addAction(m_webBrowser->pageAction(QWebEnginePage::Back));
    browserToolBar->addAction(m_webBrowser->pageAction(QWebEnginePage::Forward));
    browserToolBar->addAction(m_webBrowser->pageAction(QWebEnginePage::Reload));
    browserToolBar->addAction(m_webBrowser->pageAction(QWebEnginePage::Stop));
    auto *browserAddressBar = new WebBrowserAddressBar(browserContainer);
    m_urlCompleter = new QCompleter(m_urlCompleterModel, browserAddressBar);
    m_urlCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    m_urlCompleter->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
    browserAddressBar->setCompleter(m_urlCompleter);
    browserToolBar->addWidget(browserAddressBar);
    browserLayout->addWidget(browserToolBar);
    browserLayout->setContentsMargins(0, 0, 0, 0);
    browserLayout->addWidget(m_webBrowser);
    browserContainer->setLayout(browserLayout);
    browserDock->setWidget(browserContainer);
    addDockWidget(Qt::LeftDockWidgetArea, browserDock);
    connect(browserAddressBar, &QLineEdit::returnPressed, [this, browserAddressBar]() {
        m_urlCompleterModel.append(browserAddressBar->text());
        m_webBrowser->load(QUrl::fromUserInput(browserAddressBar->text()));
    });
    if (!m_webPageSelectionChangedTimer)
    {
        m_webPageSelectionChangedTimer = new QTimer();
        m_webPageSelectionChangedTimer->setSingleShot(true);
        connect(m_webPageSelectionChangedTimer, &QTimer::timeout, this, &MainWindow::onWebBrowserSelectionChangedTimeout);
    }
    connect(m_webBrowser->page(), &QWebEnginePage::selectionChanged, [this]() { m_webPageSelectionChangedTimer->start(1500); });
    connect(m_webBrowser, &QWebEngineView::urlChanged, [browserAddressBar](const QUrl &url) {
        Q_ASSERT(browserAddressBar);
        browserAddressBar->setText(url.toString());
    });
    connect(m_webBrowser, &QWebEngineView::titleChanged, [browserDock](const QString &title) {
        Q_ASSERT(browserDock);
        browserDock->setWindowTitle(title);
    });
}

void MainWindow::setupDockPanels()
{
    ui->menuView->addSeparator();

    setDockOptions(QMainWindow::AnimatedDocks | QMainWindow::AllowTabbedDocks | QMainWindow::GroupedDragging);

    QVector<QAction *> toggleViewActions;

    auto  getSelectionCallback = [this] { return m_clientView->selectedText(); };
    auto *googleTranslateDock  = new QDockWidget(tr("Google Translate"), this);
    googleTranslateDock->setObjectName("googleTranslate");
    m_googleTranslateEditor = new TranslateOutputWidget(new GoogleTranslator, googleTranslateDock);
    m_googleTranslateEditor->setGetSelectionCallback(getSelectionCallback);
    ClientUtils::InitializeWidgetFont(m_googleTranslateEditor->editor());
    googleTranslateDock->setWidget(m_googleTranslateEditor);
    addDockWidget(Qt::BottomDockWidgetArea, googleTranslateDock);
    auto *toggleViewAction = googleTranslateDock->toggleViewAction();
    toggleViewAction->setShortcut(QKeySequence("Shift+Alt+G"));
    ui->menuView->addAction(toggleViewAction);
    toggleViewActions.append(toggleViewAction);

    auto *baiduTranslateDock = new QDockWidget(tr("Baidu Translate"), this);
    baiduTranslateDock->setObjectName("baiduTranslateDock");
    m_baiduTranslateEditor = new TranslateOutputWidget(new BaiduTranslator, baiduTranslateDock);
    m_baiduTranslateEditor->setGetSelectionCallback(getSelectionCallback);
    ClientUtils::InitializeWidgetFont(m_baiduTranslateEditor->editor());
    baiduTranslateDock->setWidget(m_baiduTranslateEditor);
    addDockWidget(Qt::BottomDockWidgetArea, baiduTranslateDock);
    toggleViewAction = baiduTranslateDock->toggleViewAction();
    toggleViewAction->setShortcut(QKeySequence("Shift+Alt+B"));
    ui->menuView->addAction(toggleViewAction);
    toggleViewActions.append(toggleViewAction);

    auto *youdaoTranslateDock = new QDockWidget(tr("Youdao Translate"), this);
    youdaoTranslateDock->setObjectName("youdaoTranslateDock");
    m_youdaoTranslateEditor = new TranslateOutputWidget(new YoudaoTranslator, youdaoTranslateDock);
    m_youdaoTranslateEditor->setGetSelectionCallback(getSelectionCallback);
    ClientUtils::InitializeWidgetFont(m_youdaoTranslateEditor->editor());
    youdaoTranslateDock->setWidget(m_youdaoTranslateEditor);
    addDockWidget(Qt::BottomDockWidgetArea, youdaoTranslateDock);
    toggleViewAction = youdaoTranslateDock->toggleViewAction();
    toggleViewAction->setShortcut(QKeySequence("Shift+Alt+Y"));
    ui->menuView->addAction(toggleViewAction);
    toggleViewActions.append(toggleViewAction);

    auto *sogouTranslateDock = new QDockWidget(tr("Sogou Translate"), this);
    sogouTranslateDock->setObjectName("sogouTranslateDock");
    m_sogouTranslateEditor = new TranslateOutputWidget(new SogouTranslator, sogouTranslateDock);
    m_sogouTranslateEditor->setGetSelectionCallback(getSelectionCallback);
    ClientUtils::InitializeWidgetFont(m_sogouTranslateEditor->editor());
    sogouTranslateDock->setWidget(m_sogouTranslateEditor);
    addDockWidget(Qt::BottomDockWidgetArea, sogouTranslateDock);
    toggleViewAction = sogouTranslateDock->toggleViewAction();
    toggleViewAction->setShortcut(QKeySequence("Shift+Alt+S"));
    ui->menuView->addAction(toggleViewAction);
    toggleViewActions.append(toggleViewAction);

    auto *deepLTranslateDock = new QDockWidget(tr("DeepL Translate"), this);
    deepLTranslateDock->setObjectName("deepLTranslateDock");
    m_deepLTranslateEditor = new TranslateOutputWidget(new DeepLTranslator, deepLTranslateDock);
    m_deepLTranslateEditor->setGetSelectionCallback(getSelectionCallback);
    ClientUtils::InitializeWidgetFont(m_deepLTranslateEditor->editor());
    deepLTranslateDock->setWidget(m_deepLTranslateEditor);
    addDockWidget(Qt::BottomDockWidgetArea, deepLTranslateDock);
    toggleViewAction = deepLTranslateDock->toggleViewAction();
    toggleViewAction->setShortcut(QKeySequence("Shift+Alt+L"));
    ui->menuView->addAction(toggleViewAction);
    toggleViewActions.append(toggleViewAction);

    auto *youdaoDictionaryDock = new QDockWidget(tr("Youdao Dictionary"), this);
    youdaoDictionaryDock->setObjectName("youdaoDictionaryDock");
    m_youdaoDictionaryEditor = new QPlainTextEdit(youdaoDictionaryDock);
    ClientUtils::InitializeWidgetFont(m_youdaoDictionaryEditor);
    youdaoDictionaryDock->setWidget(m_youdaoDictionaryEditor);
    addDockWidget(Qt::BottomDockWidgetArea, youdaoDictionaryDock);
    toggleViewAction = youdaoDictionaryDock->toggleViewAction();
    toggleViewAction->setShortcut(QKeySequence("Shift+Alt+D"));
    ui->menuView->addAction(toggleViewAction);
    toggleViewActions.append(toggleViewAction);

    tabifyDockWidget(googleTranslateDock, baiduTranslateDock);
    tabifyDockWidget(baiduTranslateDock, youdaoTranslateDock);
    tabifyDockWidget(youdaoTranslateDock, sogouTranslateDock);
    tabifyDockWidget(sogouTranslateDock, deepLTranslateDock);
    tabifyDockWidget(deepLTranslateDock, youdaoDictionaryDock);

    auto *showAllLanguageDocks = new QAction(tr("Show All Language Docks"), this);
    ui->menuView->addAction(showAllLanguageDocks);
    connect(showAllLanguageDocks, &QAction::triggered, [toggleViewActions] {
        for (auto *toggleViewAction : toggleViewActions)
        {
            emit toggleViewAction->triggered(true);
        }
    });
    auto *hideAllLanguageDocks = new QAction(tr("Hide All Language Docks"), this);
    ui->menuView->addAction(hideAllLanguageDocks);
    connect(hideAllLanguageDocks, &QAction::triggered, [toggleViewActions] {
        for (auto *toggleViewAction : toggleViewActions)
        {
            emit toggleViewAction->triggered(false);
        }
    });

    ui->menuView->addSeparator();

    auto *fsDock = new QDockWidget(tr("File System"), this);
    fsDock->setObjectName("fsDock");
    m_fsView     = new QTreeView(fsDock);
    m_fsModel = new QFileSystemModel(m_fsView);
    m_fsModel->setRootPath(QDir::currentPath());
    m_fsModel->setNameFilters(QStringList() << "*.md"
                                            << "*.markdown"
                                            << "*.mdown"
                                            << "*.htm"
                                            << "*.html");
    m_fsModel->sort(0);
    m_fsView->setModel(m_fsModel);
    for (int i = 1, total = m_fsModel->columnCount(); i < total; ++i)
    {
        m_fsView->hideColumn(i);
    }
    m_fsView->header()->hide();
    connect(m_fsView, &QTreeView::activated, this, &MainWindow::onFileSystemItemActivated);
    fsDock->setWidget(m_fsView);
    addDockWidget(Qt::LeftDockWidgetArea, fsDock);
    toggleViewAction = fsDock->toggleViewAction();
    toggleViewAction->setShortcut(QKeySequence("Shift+Alt+F"));
    ui->menuView->addAction(toggleViewAction);

    auto *webDAVDock = new QDockWidget(tr("WebDAV"), this);
    webDAVDock->setObjectName("webDAVDock");
    m_webDAVView = new QTreeView(webDAVDock);
    webDAVDock->setWidget(m_webDAVView);
    addDockWidget(Qt::LeftDockWidgetArea, webDAVDock);
    toggleViewAction = webDAVDock->toggleViewAction();
    toggleViewAction->setShortcut(QKeySequence("Shift+Alt+C"));
    ui->menuView->addAction(toggleViewAction);

    tabifyDockWidget(fsDock, webDAVDock);

    auto *previewHTMLDock = new QDockWidget(tr("Preview HTML"), this);
    previewHTMLDock->setObjectName("previewHTMLDock");
    m_previewHTMLEditor   = new PreviewThemeEditor(previewHTMLDock);
    m_previewHTMLEditor->initialize("hypertext");
    previewHTMLDock->setWidget(m_previewHTMLEditor);
    addDockWidget(Qt::RightDockWidgetArea, previewHTMLDock);
    toggleViewAction = previewHTMLDock->toggleViewAction();
    toggleViewAction->setShortcut(QKeySequence("Shift+Alt+H"));
    ui->menuView->addAction(toggleViewAction);
    m_clientView->setPreviewHTMLEditor(m_previewHTMLEditor);

    auto *customThemeEditorDock = new QDockWidget(tr("Custom Theme Editor"), this);
    customThemeEditorDock->setObjectName("customThemeEditorDock");
    m_customPreivewThemeEditor = new CustomPreviewThemeEditWidget(customThemeEditorDock);
    customThemeEditorDock->setWidget(m_customPreivewThemeEditor);
    addDockWidget(Qt::RightDockWidgetArea, customThemeEditorDock);
    toggleViewAction = customThemeEditorDock->toggleViewAction();
    toggleViewAction->setShortcut(QKeySequence("Shift+Alt+T"));
    ui->menuView->addAction(toggleViewAction);
    m_clientView->setCustomPreivewThemeEditor(m_customPreivewThemeEditor->editor());
    connect(m_customPreivewThemeEditor, &CustomPreviewThemeEditWidget::contentModified, this, &MainWindow::onCustomPreviewThemeChanged);

    auto *devToolDock = new QDockWidget(tr("DevTool"), this);
    devToolDock->setObjectName("devToolDock");
    auto *devToolView = new QWebEngineView(devToolDock);
    devToolView->setPage(m_clientView->devToolPage());
    devToolDock->setWidget(devToolView);
    addDockWidget(Qt::RightDockWidgetArea, devToolDock);
    toggleViewAction = devToolDock->toggleViewAction();
    toggleViewAction->setShortcut(QKeySequence("F12"));
    ui->menuView->addAction(toggleViewAction);

    tabifyDockWidget(previewHTMLDock, devToolDock);
    tabifyDockWidget(previewHTMLDock, customThemeEditorDock);

    setupWebBrowserPane();

    // setup samples panels

    auto *dockSampleResult =
        new QDockWidget(tr("PlantUML Sample Result"), this);
    connect(dockSampleResult, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)), this, SLOT(onSampleResultDockLocationChanged(Qt::DockWidgetArea)));

    m_tabWidgetSampleResult = new QTabWidget(dockSampleResult);
    connect(m_tabWidgetSampleResult, &QTabWidget::currentChanged, [this]() {
        focusSampleResult();
        onSampleItemSelectionChanged();
    });

    dockSampleResult->setWidget(m_tabWidgetSampleResult);
    dockSampleResult->setObjectName("sampleResult");
    addDockWidget(Qt::LeftDockWidgetArea, dockSampleResult);
    onSampleResultDockLocationChanged(Qt::LeftDockWidgetArea);

    auto *sampleResult          = new SampleResult;
    sampleResult->samplePath    = ":/samples/UML";
    sampleResult->sampleToolBox = new QToolBox(m_tabWidgetSampleResult);
    m_tabWidgetSampleResult->addTab(sampleResult->sampleToolBox, tr("UML"));
    connect(sampleResult->sampleToolBox, SIGNAL(currentChanged(int)), this, SLOT(onCurrentSampleResultChanged(int)));
    m_sampleResults << sampleResult;

    sampleResult                = new SampleResult;
    sampleResult->samplePath    = ":/samples/non-UML";
    sampleResult->sampleToolBox = new QToolBox(m_tabWidgetSampleResult);
    m_tabWidgetSampleResult->addTab(sampleResult->sampleToolBox, tr("non-UML"));
    connect(sampleResult->sampleToolBox, SIGNAL(currentChanged(int)), this, SLOT(onCurrentSampleResultChanged(int)));
    m_sampleResults << sampleResult;

    sampleResult                = new SampleResult;
    sampleResult->samplePath    = ":/samples/Advanced";
    sampleResult->sampleToolBox = new QToolBox(m_tabWidgetSampleResult);
    m_tabWidgetSampleResult->addTab(sampleResult->sampleToolBox, tr("Advanced"));
    connect(sampleResult->sampleToolBox, SIGNAL(currentChanged(int)), this, SLOT(onCurrentSampleResultChanged(int)));
    m_sampleResults << sampleResult;

    auto *m_showSampleResultDockAction = dockSampleResult->toggleViewAction();
    m_showSampleResultDockAction->setIconVisibleInMenu(false);
    m_showSampleResultDockAction->setStatusTip(tr("Show or hide the Sample Result dock"));
#if !defined(Q_WS_WIN) // BUG: icons are not displayed when cross-linking
    m_showSampleResultDockAction->setIcon(QIcon(":/rc/icons/sample-result.png"));
#endif
    m_showSampleResultDockAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_1));
    ui->menuView->addAction(m_showSampleResultDockAction);

    auto *dockSampleSource =
        new QDockWidget(tr("PlantUML Sample Source Code"), this);
    m_sampleSourcePreview  = new PlantUMLSourceEditor(dockSampleSource);
    m_sampleSourcePreview->initialize();
    m_sampleSourcePreview->setReadOnly(true);
    dockSampleSource->setWidget(m_sampleSourcePreview);
    dockSampleSource->setObjectName("sampleSource");
    addDockWidget(Qt::RightDockWidgetArea, dockSampleSource);

    auto *m_showSampleSourceDockAction = dockSampleSource->toggleViewAction();
    m_showSampleSourceDockAction->setIconVisibleInMenu(false);
    m_showSampleSourceDockAction->setStatusTip(tr("Show or hide the Sample Source dock"));
#if !defined(Q_WS_WIN) // BUG: icons are not displayed when cross-linking
    m_showSampleSourceDockAction->setIcon(QIcon(":/rc/icons/sample-source.png"));
#endif
    m_showSampleSourceDockAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_2));
    ui->menuView->addAction(m_showSampleSourceDockAction);
}

void MainWindow::setupShortcutToolbar()
{
    ui->shortcutToolbar->addWidget(new QLabel(tr("Mode:")));
    m_cbPreviewMode = new QComboBox(ui->shortcutToolbar);
    ui->shortcutToolbar->addWidget(m_cbPreviewMode);
    ui->shortcutToolbar->addWidget(new QLabel(tr("Engine:")));
    m_cbMarkdownEngine = new QComboBox(ui->shortcutToolbar);
    ui->shortcutToolbar->addWidget(m_cbMarkdownEngine);
    ui->shortcutToolbar->addWidget(new QLabel(tr("Code Block Style:")));
    m_cbCodeBlockStyle = new QComboBox(ui->shortcutToolbar);
    ui->shortcutToolbar->addWidget(m_cbCodeBlockStyle);
    ui->shortcutToolbar->addWidget(new QLabel(tr("Preview Theme:")));
    m_cbPreviewTheme = new QComboBox(ui->shortcutToolbar);
    ui->shortcutToolbar->addWidget(m_cbPreviewTheme);

    m_cbPreviewMode->addItems(QStringList({tr("Wechat Public Account Article"), tr("Blog Post")}));
    m_cbPreviewMode->setCurrentText(g_settings->previewMode());
    connect(m_cbPreviewMode, &QComboBox::currentTextChanged, this, &MainWindow::onCurrentPreviewModeChanged);

    m_cbMarkdownEngine->addItems(QStringList({tr("Goldmark"), tr("Lute")}));
    m_cbMarkdownEngine->setCurrentText(g_settings->markdownEngine());
    connect(m_cbMarkdownEngine, &QComboBox::currentTextChanged, this, &MainWindow::onCurrentMarkdownEngineChanged);

    m_cbCodeBlockStyle->addItems(QStringList({
        "abap",
        "algol",
        "algol_nu",
        "api",
        "arduino",
        "autumn",
        "borland",
        "bw",
        "colorful",
        "dracula",
        "emacs",
        "friendly",
        "fruity",
        "github",
        "igor",
        "lovelace",
        "manni",
        "monokai",
        "monokailight",
        "murphy",
        "native",
        "paraiso-dark",
        "paraiso-light",
        "pastie",
        "perldoc",
        "pygments",
        "rainbow_dash",
        "rrt",
        "solarized-dark",
        "solarized-dark256",
        "solarized-light",
        "swapoff",
        "tango",
        "trac",
        "vim",
        "vs",
        "xcode",
    }));
    m_cbCodeBlockStyle->setCurrentText(g_settings->codeBlockStyle());
    connect(m_cbCodeBlockStyle, &QComboBox::currentTextChanged, this, &MainWindow::onCurrentCodeBlockStyleChanged);

    QDir dir(":/rc/theme");
    auto entries = dir.entryInfoList();
    for (const auto &entry : entries)
    {
        m_cbPreviewTheme->addItem(entry.baseName());
    }
    m_cbPreviewTheme->addItem(tr("Custom"));
    m_cbPreviewTheme->setCurrentText(g_settings->previewTheme());
    connect(m_cbPreviewTheme, &QComboBox::currentTextChanged, this, &MainWindow::onCurrentPreviewThemeChanged);
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::WindowStateChange)
    {
        if (windowState() & Qt::WindowFullScreen)
        {
            QVector<QToolBar *> toolbars = {ui->fileToolbar, ui->editToolbar, ui->formatToolbar, ui->shortcutToolbar, ui->translateToolBar};
            for (auto *toolbar : toolbars)
            {
                if (toolbar->isVisible())
                {
                    m_visibleToolbars.append(toolbar);
                    toolbar->setVisible(false);
                }
            }
            ui->actionFullScreen->setText(tr("Exit Full Screen"));
            menuBar()->hide();
            auto *shortcut = new QShortcut(QKeySequence("F11"), this);
            connect(shortcut, &QShortcut::activated, [this, shortcut] {
                on_actionFullScreen_triggered();
                shortcut->deleteLater();
            });
            event->accept();
            return;
        }

        auto *stateChangeEvent = dynamic_cast<QWindowStateChangeEvent *>(event);
        if (stateChangeEvent->oldState() & Qt::WindowFullScreen)
        {
            for (auto *toolbar : std::as_const(m_visibleToolbars))
            {
                toolbar->setVisible(true);
            }
            m_visibleToolbars.clear();
            menuBar()->show();
            ui->actionFullScreen->setText(tr("Full Screen"));
            event->accept();
            return;
        }
    }
    event->ignore();
}

void MainWindow::on_actionDictionary_triggered()
{
    Q_ASSERT(m_youdaoDictionaryEditor);
    m_youdaoDictionaryEditor->clear();
    Q_ASSERT(m_clientView);
    QString text = m_clientView->selectedText();
    if (text.isEmpty())
    {
        return;
    }
    Q_ASSERT(m_youdaoDict);
    m_youdaoDict->query(text);
}

void MainWindow::on_actionTranslateSelected_triggered()
{
    Q_ASSERT(m_clientView);
    Q_ASSERT(m_webBrowser);
    QString text;
    if (m_webBrowser->hasFocus())
    {
        if (!m_webBrowser->selectedText().isEmpty())
        {
            text = m_webBrowser->selectedText();
        }
        else
        {
            text = m_clientView->selectedText();
        }
    }
    else
    {
        if (!m_clientView->selectedText().isEmpty())
        {
            text = m_clientView->selectedText();
        }
        else
        {
            text = m_webBrowser->selectedText();
        }
    }
    translateText(text);
}

void MainWindow::on_actionGoogle_triggered()
{
    g_settings->setEnableGoogleTranslate(ui->actionGoogle->isChecked());
}

void MainWindow::on_actionBaidu_triggered()
{
    g_settings->setEnableBaiduTranslate(ui->actionBaidu->isChecked());
}

void MainWindow::on_actionYoudao_triggered()
{
    g_settings->setEnableYoudaoTranslate(ui->actionYoudao->isChecked());
}

void MainWindow::on_actionSogou_triggered()
{
    g_settings->setEnableSogouTranslate(ui->actionSogou->isChecked());
}

void MainWindow::on_actionDeepL_triggered()
{
    g_settings->setEnableDeepLTranslate(ui->actionDeepL->isChecked());
}

void MainWindow::on_actionTranslateTextInClipboard_triggered()
{
    auto *clipboard = QGuiApplication::clipboard();
    Q_ASSERT(clipboard);
    auto text = clipboard->text();
    translateText(text);
}

void MainWindow::on_actionTemplateManager_triggered()
{
    TemplateManagerDialog dlg(*m_templateManager, this);
    dlg.exec();
    if (dlg.isModified())
    {
        updateNewFromTemplateMenus();
    }
}

void MainWindow::on_actionOpenWorkspace_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(
        this, tr("Open Workspace"), ClientUtils::getDefaultFileSaveOpenDirectory(), tr("KarenMeu Workspace (*.krm);;All files (*.*)"));
    if (!QFile::exists(fileName))
    {
        return;
    }
    openWorkspace(fileName);
}

void MainWindow::on_actionSaveWorkspace_triggered()
{
    if (!QFile::exists(m_currentOpenedFile) || !m_currentOpenedFile.endsWith(".krm", Qt::CaseInsensitive))
    {
        on_actionSaveWorkspaceAs_triggered();
    }
    else
    {
        saveWorkspace(m_currentOpenedFile);
    }
}

void MainWindow::on_actionSaveWorkspaceAs_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(
        this, tr("Save Workspace"), ClientUtils::getDefaultFileSaveOpenDirectory(), tr("KarenMeu Workspace (*.krm);;All files (*.*)"));

    if (fileName.isEmpty())
    {
        return;
    }

    saveWorkspace(fileName);
}

void MainWindow::on_actionClearRecentWorkspaceList_triggered()
{
    auto &      settings = g_settings->getSettings();
    QStringList files;
    settings.beginGroup("workspace");
    settings.setValue("recentWorkspaceList", files);
    settings.endGroup();
    updateRecentWorkspaceActions(files);
}

void MainWindow::on_actionTranslateFullText_triggered()
{
    Q_ASSERT(m_clientView);
    QString text = m_clientView->fullText();
    translateText(text);
}

void MainWindow::on_actionFullScreen_triggered()
{
    if (isFullScreen())
    {
        switch (m_lastWindowState)
        {
        case Minimized:
            showMinimized();
            break;
        case Maximized:
            showMaximized();
            break;
        case Normal:
            showNormal();
            break;
        default:
            showNormal();
            break;
        }
    }
    else
    {
        if (isMinimized())
        {
            m_lastWindowState = Minimized;
        }
        else if (isMaximized())
        {
            m_lastWindowState = Maximized;
        }
        else
        {
            m_lastWindowState = Normal;
        }
        showFullScreen();
    }
}

void MainWindow::on_actionFindReplace_triggered()
{
    auto *dlg = getFindReplaceDialog();
    dlg->show();
}

void MainWindow::on_actionLeftRightOrTopBottomViews_triggered()
{
    auto mode = g_settings->markdownViewArrange();
    g_settings->setMarkdownViewArrange(!mode);
    QMessageBox::information(this, tr("Notice"), tr("It is required to restart KarenMeu application to take effect."), QMessageBox::Ok);
}

void MainWindow::onSampleResultFocus()
{
    focusSampleResult();
}

void MainWindow::onSampleItemInsert(QObject *widget)
{
    auto *listWidget = qobject_cast<QListWidget *>(widget);
    if (listWidget)
    {
        onSampleItemDoubleClicked(listWidget->currentItem());
    }
}

void MainWindow::onSampleItemSelectionChanged()
{
    auto *sampleResultToolBox = qobject_cast<QToolBox *>(m_tabWidgetSampleResult->currentWidget());
    Q_ASSERT(sampleResultToolBox);
    auto *widget = qobject_cast<QListWidget *>(sampleResultToolBox->currentWidget());
    if (widget)
    {
        auto *item = widget->currentItem();
        if (item)
        {
            auto notes = item->data(SAMPLE_ITEM_NOTES_ROLE).toString();
            m_sampleSourcePreview->setToolTip(notes);
            m_sampleSourcePreview->setReadOnly(false);
            m_sampleSourcePreview->setContent(item->data(SAMPLE_ITEM_DATA_ROLE).toByteArray());
            m_sampleSourcePreview->setReadOnly(true);
        }
    }
}

void MainWindow::onCurrentSampleResultChanged(int /*index*/)
{
    focusSampleResult();
    onSampleItemSelectionChanged(); // make sure we don't show stale info
}

void MainWindow::onSampleItemDoubleClicked(QListWidgetItem *item)
{
    insertSampleSource(item->data(SAMPLE_ITEM_DATA_ROLE).toString());
}

void MainWindow::reloadSamples(SampleResult *sampleResult)
{
    for (auto *widget : std::as_const(sampleResult->sampleWidgets))
    {
        widget->deleteLater();
    }
    sampleResult->sampleWidgets.clear();

    if (sampleResult->samplePath.isEmpty())
    {
        qDebug() << "No sample defined.";
        return;
    }
    SampleReader reader;
    reader.scan(sampleResult->samplePath);

    for (auto *sample : reader)
    {
        auto *view = newSampleListWidget(SAMPLE_ICON_SIZE, this);
        for (const auto *sampleItem : *sample)
        {
            auto *listWidgetItem = new QListWidgetItem(ClientUtils::iconFromSvg(SAMPLE_ICON_SIZE, sampleItem->icon()), sampleItem->name(), view);
            listWidgetItem->setData(SAMPLE_ITEM_DATA_ROLE, sampleItem->data());
            listWidgetItem->setData(SAMPLE_ITEM_NOTES_ROLE, sampleItem->notes());
            listWidgetItem->setData(SAMPLE_ITEM_PATH_ROLE, sampleItem->icon());
            listWidgetItem->setToolTip(sampleItem->notes());
        }
        sampleResult->sampleToolBox->addItem(view, sample->name());
        connect(view, &QListWidget::itemDoubleClicked, this, &MainWindow::onSampleItemDoubleClicked);
        connect(view, &QListWidget::itemSelectionChanged, this, &MainWindow::onSampleItemSelectionChanged);

        auto *action = new QAction(this);
        action->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Return));
        m_sampleInsertSignalMapper->setMapping(action, view);
        connect(action, &QAction::triggered, m_sampleInsertSignalMapper, QOverload<>::of(&QSignalMapper::map));
        view->addAction(action);

        sampleResult->sampleWidgets << view;
    }
}

void MainWindow::insertSampleSource(const QString &code)
{
    emit m_clientView->insertText(code);
}

QListWidget *MainWindow::newSampleListWidget(const QSize &iconSize, QWidget *parent)
{
    auto *view = new QListWidget(parent);
    view->setUniformItemSizes(true);
    view->setMovement(QListView::Static);
    view->setResizeMode(QListView::Adjust);
    view->setIconSize(iconSize);
    view->setViewMode(QListView::IconMode);
    return view;
}

void MainWindow::focusSampleResult()
{
    auto *currentToolBox = qobject_cast<QToolBox *>(m_tabWidgetSampleResult->currentWidget());
    auto *widget         = qobject_cast<QListWidget *>(currentToolBox->currentWidget());
    if (widget)
    {
        widget->setFocus();
        if (widget->selectedItems().count() == 0)
        {
            widget->setCurrentItem(widget->itemAt(0, 0));
        }
    }
}

void MainWindow::onSampleResultDockLocationChanged(Qt::DockWidgetArea area)
{
    QMap<Qt::DockWidgetArea, QTabWidget::TabPosition> dockPositionMap = {
        {Qt::LeftDockWidgetArea, QTabWidget::West},
        {Qt::RightDockWidgetArea, QTabWidget::East},
        {Qt::TopDockWidgetArea, QTabWidget::North},
        {Qt::BottomDockWidgetArea, QTabWidget::South},
    };
    m_tabWidgetSampleResult->setTabPosition(dockPositionMap[area]);
}
