#include <QAction>
#include <QClipboard>
#include <QCloseEvent>
#include <QComboBox>
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
#include <QMap>
#include <QMessageBox>
#include <QPainter>
#include <QPlainTextEdit>
#include <QShortcut>
#include <QSplitter>
#include <QTreeView>
#include <QUrl>
#include <QWebEngineView>
#include <QWindowStateChangeEvent>
#include <QtCore>

#include "mainwindow.h"

#include "clientutils.h"
#include "custompreviewthemeeditwidget.h"
#include "filecache.h"
#include "findreplacedialog.h"
#include "markdowneditor4.h"
#include "markdownview.h"
#include "preferencedialog.h"
#include "previewthemeeditor.h"
#include "settings.h"
#include "templatemanager.h"
#include "templatemanagerdialog.h"
#include "translatehelperpage.h"
#include "translateoutputwidget.h"
#include "ui_mainwindow.h"
#include "utils.h"
#include "xmlSettings.h"
#include "youdao.h"

using LabelActionMap = QMap<QString, QAction *>;
using ActionLabelMap = QHash<QAction *, QString>;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_fileCache(new FileCache(50 * 1024 * 1024, this))
    , m_view(new MarkdownView(&m_nam, m_fileCache, this))
    , m_youdaoDict(new Youdao(m_nam))
    , m_templateManager(new TemplateManager)
{
    ui->setupUi(this);
    statusBar()->hide();
    setCentralWidget(m_view);
    connect(ui->actionNewMarkdown, &QAction::triggered, m_view, &MarkdownView::newDocument);
    connect(ui->actionOpenMarkdown, &QAction::triggered, m_view, &MarkdownView::openDocument);
    connect(ui->actionSaveMarkdown, &QAction::triggered, m_view, &MarkdownView::saveDocument);
    connect(ui->actionSaveMarkdownAs, &QAction::triggered, m_view, &MarkdownView::saveAsDocument);
    connect(ui->actionCut, &QAction::triggered, m_view, &MarkdownView::cut);
    connect(ui->actionCopy, &QAction::triggered, m_view, &MarkdownView::copy);
    connect(ui->actionPaste, &QAction::triggered, m_view, &MarkdownView::paste);
    connect(ui->actionUndo, &QAction::triggered, m_view, &MarkdownView::undo);
    connect(ui->actionRedo, &QAction::triggered, m_view, &MarkdownView::redo);
    connect(ui->actionSelectAll, &QAction::triggered, m_view, &MarkdownView::selectAll);
    connect(ui->actionCopyAsHTML, &QAction::triggered, m_view, &MarkdownView::copyAsHTML);
    connect(ui->actionExportAsPDF, &QAction::triggered, m_view, &MarkdownView::exportAsPDF);
    connect(ui->actionExportAsHTML, &QAction::triggered, m_view, &MarkdownView::exportAsHTML);
    connect(ui->actionStrong, &QAction::triggered, m_view, &MarkdownView::formatStrong);
    connect(ui->actionEmphasize, &QAction::triggered, m_view, &MarkdownView::formatEmphasize);
    connect(ui->actionStrikethrough, &QAction::triggered, m_view, &MarkdownView::formatStrikethrough);
    connect(ui->actionInlineCode, &QAction::triggered, m_view, &MarkdownView::formatInlineCode);
    connect(ui->actionCodeBlock, &QAction::triggered, m_view, &MarkdownView::formatCodeBlock);
    connect(ui->actionComment, &QAction::triggered, m_view, &MarkdownView::formatComment);
    connect(ui->actionOrderedList, &QAction::triggered, m_view, &MarkdownView::formatOrderedList);
    connect(ui->actionUnorderedList, &QAction::triggered, m_view, &MarkdownView::formatUnorderedList);
    connect(ui->actionBlockquote, &QAction::triggered, m_view, &MarkdownView::formatBlockquote);
    connect(ui->actionHyperlink, &QAction::triggered, m_view, &MarkdownView::formatHyperlink);
    connect(ui->actionImage, &QAction::triggered, m_view, &MarkdownView::formatImage);
    connect(ui->actionNewParagraph, &QAction::triggered, m_view, &MarkdownView::formatNewParagraph);
    connect(ui->actionHorizontalRule, &QAction::triggered, m_view, &MarkdownView::formatHorizontalRule);
    connect(ui->actionHeader1, &QAction::triggered, m_view, &MarkdownView::formatHeader1);
    connect(ui->actionHeader2, &QAction::triggered, m_view, &MarkdownView::formatHeader2);
    connect(ui->actionHeader3, &QAction::triggered, m_view, &MarkdownView::formatHeader3);
    connect(ui->actionHeader4, &QAction::triggered, m_view, &MarkdownView::formatHeader4);
    connect(ui->actionHeader5, &QAction::triggered, m_view, &MarkdownView::formatHeader5);
    connect(ui->actionHeader6, &QAction::triggered, m_view, &MarkdownView::formatHeader6);
    connect(ui->actionShiftRight, &QAction::triggered, m_view, &MarkdownView::formatShiftRight);
    connect(ui->actionShiftLeft, &QAction::triggered, m_view, &MarkdownView::formatShiftLeft);
    connect(m_view, &MarkdownView::setCurrentFile, this, &MainWindow::onSetCurrentMarkdownDocument);
    connect(m_view, &MarkdownView::contentModified, this, &MainWindow::onDocumentModified);
    connect(m_youdaoDict, &Youdao::result, this, &MainWindow::onYoudaoDictResult);

    ui->menuView->addAction(ui->fileToolbar->toggleViewAction());
    ui->menuView->addAction(ui->editToolbar->toggleViewAction());
    ui->menuView->addAction(ui->formatToolbar->toggleViewAction());
    ui->menuView->addAction(ui->shortcutToolbar->toggleViewAction());

    for (int i = 0; i < MaxRecentFiles; ++i)
    {
        recentFileActs[i] = new QAction(this);
        recentFileActs[i]->setVisible(false);
        connect(recentFileActs[i], &QAction::triggered, this, &MainWindow::openRecentFile);
        ui->menuRecentFiles->addAction(recentFileActs[i]);

        recentWorkspaceActs[i] = new QAction(this);
        recentWorkspaceActs[i]->setVisible(false);
        connect(recentWorkspaceActs[i], &QAction::triggered, this, &MainWindow::openRecentWorkspace);
        ui->menuRecentWorkspaces->addAction(recentWorkspaceActs[i]);
    }

    updateNewFromTemplateMenus();

    updateTranslationActions();

    setupShortcutToolbar();

    setupDockPanels();

    applyMarkdownEditorTheme();

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
        fileName.endsWith(".mdown", Qt::CaseInsensitive))
    {
        openMarkdownDocument(fileName);
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

void MainWindow::openMarkdownDocument(const QString &fileName)
{
    Q_ASSERT(m_view);

    m_view->newDocument();
    m_view->openFromFile(fileName);
}

void MainWindow::openWorkspace(const QString &fileName)
{
    Q_ASSERT(m_view);
    auto editor = m_view->editor();
    Q_ASSERT(editor);
    QSettings settings(fileName, xmlFormat);
    auto      content = settings.value("content").toString();
    newDocumentWithContent(content);
    auto mainWindowState = settings.value("windowState").toByteArray();
    if (!mainWindowState.isEmpty())
        restoreState(mainWindowState);
    auto mainWindowGeometry = settings.value("windowGeometry").toByteArray();
    if (!mainWindowGeometry.isEmpty())
        restoreGeometry(mainWindowGeometry);

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
    Q_ASSERT(m_view);
    auto editor = m_view->editor();
    Q_ASSERT(editor);
    QSettings settings(fileName, xmlFormat);
    settings.setValue("content", m_view->fullText());
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
        settings.setValue("googleTranslateResult", html);

    Q_ASSERT(m_baiduTranslateEditor);
    html = m_baiduTranslateEditor->content();
    if (!html.isEmpty())
        settings.setValue("baiduTranslateResult", html);

    Q_ASSERT(m_youdaoTranslateEditor);
    html = m_youdaoTranslateEditor->content();
    if (!html.isEmpty())
        settings.setValue("youdaoTranslateResult", html);

    Q_ASSERT(m_sogouTranslateEditor);
    html = m_sogouTranslateEditor->content();
    if (!html.isEmpty())
        settings.setValue("sogouTranslateResult", html);

    Q_ASSERT(m_deepLTranslateEditor);
    html = m_deepLTranslateEditor->content();
    if (!html.isEmpty())
        settings.setValue("deepLTranslateResult", html);

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
    Q_ASSERT(m_view);
    auto editor = m_view->editor();
    Q_ASSERT(editor);
    if (!m_currentOpenedFile.isEmpty())
        setWindowTitle(QString("%1%2 - KarenMeu").arg(QFileInfo(m_currentOpenedFile).fileName(), (editor->isModified() ? "*" : "")));
    else
        setWindowTitle("KarenMeu");
}

FindReplaceDialog *MainWindow::getFindReplaceDialog()
{
    static FindReplaceDialog *dlg = new FindReplaceDialog(m_view->editor(), this);
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
        return;
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
    for (auto action : qAsConst(m_newFromTemplateActions))
    {
        ui->menuNewFromTemplate->removeAction(action);
    }
    m_newFromTemplateActions.clear();

    // add new actions
    m_templateManager->load();
    auto templates = m_templateManager->templates();
    for (auto t : templates)
    {
        auto action = new QAction(this);
        m_newFromTemplateActions.push_back(action);
        action->setText(t->templateName());
        connect(action, &QAction::triggered, this, &MainWindow::onNewFromTemplateTriggered);
        ui->menuNewFromTemplate->addAction(action);
    }
}

void MainWindow::newDocumentWithContent(const QString &content)
{
    Q_ASSERT(m_view);
    m_view->newDocument();
    m_view->setInitialDocument(content);
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

        m_view->updatePreviewTheme();
        m_view->updateMarkdownEngine();
        m_view->updatePreviewMode();
        m_view->updateMacStyleCodeBlock();
        m_view->forceConvert();
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_view->maybeSave())
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
    int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

    for (int i = 0; i < numRecentFiles; ++i)
    {
        if (!QFile::exists(files[i]))
        {
            continue;
        }
        QString text = tr("%1. %2").arg(i + 1).arg(strippedName(files[i]));
        recentFileActs[i]->setText(text);
        recentFileActs[i]->setData(files[i]);
        recentFileActs[i]->setVisible(true);
    }
    for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
        recentFileActs[j]->setVisible(false);
}

void MainWindow::updateRecentWorkspaceActions(const QStringList &files)
{
    int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

    for (int i = 0; i < numRecentFiles; ++i)
    {
        QString text = tr("%1. %2").arg(i + 1).arg(strippedName(files[i]));
        recentWorkspaceActs[i]->setText(text);
        recentWorkspaceActs[i]->setData(files[i]);
        recentWorkspaceActs[i]->setVisible(true);
    }
    for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
        recentWorkspaceActs[j]->setVisible(false);
}

void MainWindow::onSetCurrentMarkdownDocument(const QString &fileName)
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
    settings.beginGroup("recentFile");
    QStringList files = settings.value("recentFileList").toStringList();
    files.removeAll(fileName);
    files.prepend(fileName);
    while (files.size() > MaxRecentFiles)
        files.removeLast();

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
        files.removeLast();

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
        m_view->updatePreviewMode();
        m_view->forceConvert();
    }
}

void MainWindow::onCurrentMarkdownEngineChanged(const QString &text)
{
    if (g_settings->markdownEngine() != text)
    {
        g_settings->setMarkdownEngine(text);
        m_view->updateMarkdownEngine();
        m_view->forceConvert();
    }
}

void MainWindow::onCurrentCodeBlockStyleChanged(const QString &text)
{
    if (g_settings->codeBlockStyle() != text)
    {
        g_settings->setCodeBlockStyle(text);
        m_view->forceConvert();
    }
}

void MainWindow::onCurrentPreviewThemeChanged(const QString &text)
{
    if (g_settings->previewTheme() != text)
    {
        g_settings->setPreviewTheme(text);
        m_view->updatePreviewTheme();
        m_view->forceConvert();
    }
}

void MainWindow::onCustomPreviewThemeChanged()
{
    if (g_settings->previewTheme() == tr("Custom"))
    {
        m_view->updatePreviewTheme();
        m_view->forceConvert();
    }
}

void MainWindow::onYoudaoDictResult(QString res)
{
    Q_ASSERT(m_youdaoDictionaryEditor);
    ClientUtils::setHtmlContent(m_youdaoDictionaryEditor, res);
}

void MainWindow::onNewFromTemplateTriggered()
{
    auto act = qobject_cast<QAction *>(sender());
    Q_UNUSED(act);
    QString templateName = act->text();
    auto    t            = m_templateManager->find(templateName);
    if (!t)
        return;

    QString title;
    if (t->needTitle())
    {
        title = QInputDialog::getText(this, tr("Input Title"), tr("Title for document"));
    }
    auto fileContent = t->templateExecutedContent(title);
    auto fileName    = t->templateExecutedName(title);
    if (!fileName.isEmpty())
    {
        // need a directory to save file
        // save to file
        auto path = QFileDialog::getExistingDirectory(
            this, tr("Select a directory to save document"), QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
        if (path.isEmpty())
        {
            return;
        }
        QDir dir(path);
        if (!dir.exists())
        {
            if (!dir.mkpath(path))
            {
                QMessageBox::warning(this, tr("Error"), tr("Making directory %1 failed.").arg(path), QMessageBox::Ok);
                return;
            }
        }
        auto  filePath = (QStringList() << path << fileName).join("/");
        QFile f(filePath);
        if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate))
        {
            QMessageBox::warning(this, tr("Error"), tr("Creating file at %1 failed.").arg(filePath), QMessageBox::Ok);
            return;
        }
        else
        {
            f.write(fileContent.toUtf8());
            f.close();
            // open file
            openMarkdownDocument(filePath);
        }
    }
    else
    {
        Q_ASSERT(m_view);
        m_view->newDocument();
        m_view->setInitialDocument(fileContent);
    }
}

void MainWindow::onDocumentModified()
{
    updateWindowTitle();
}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void MainWindow::openRecentFile()
{
    auto *action = qobject_cast<QAction *>(sender());
    Q_ASSERT(action);
    openMarkdownDocument(action->data().toString());
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

void MainWindow::setupDockPanels()
{
    ui->menuView->addSeparator();

    setDockOptions(QMainWindow::AnimatedDocks | QMainWindow::AllowTabbedDocks | QMainWindow::GroupedDragging);

    QList<QAction *> toggleViewActions;

    auto  getSelectionCallback = std::bind(&MarkdownView::selectedText, m_view);
    auto *googleTranslateDock  = new QDockWidget(tr("Google Translate"), this);
    googleTranslateDock->setObjectName("googleTranslate");
    m_googleTranslateEditor = new TranslateOutputWidget(TST_GOOGLE, googleTranslateDock);
    m_googleTranslateEditor->setGetSelectionCallback(getSelectionCallback);
    ClientUtils::InitializeWidgetFont(m_googleTranslateEditor->editor());
    googleTranslateDock->setWidget(m_googleTranslateEditor);
    addDockWidget(Qt::BottomDockWidgetArea, googleTranslateDock);
    auto toggleViewAction = googleTranslateDock->toggleViewAction();
    toggleViewAction->setShortcut(QKeySequence("Shift+Alt+G"));
    ui->menuView->addAction(toggleViewAction);
    toggleViewActions.append(toggleViewAction);

    auto *baiduTranslateDock = new QDockWidget(tr("Baidu Translate"), this);
    baiduTranslateDock->setObjectName("baiduTranslateDock");
    m_baiduTranslateEditor = new TranslateOutputWidget(TST_BAIDU, baiduTranslateDock);
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
    m_youdaoTranslateEditor = new TranslateOutputWidget(TST_YOUDAO, youdaoTranslateDock);
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
    m_sogouTranslateEditor = new TranslateOutputWidget(TST_SOGOU, sogouTranslateDock);
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
    m_deepLTranslateEditor = new TranslateOutputWidget(TST_DEEPL, deepLTranslateDock);
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

    auto showAllLanguageDocks = new QAction(tr("Show All Language Docks"), this);
    ui->menuView->addAction(showAllLanguageDocks);
    connect(showAllLanguageDocks, &QAction::triggered, [toggleViewActions] {
        for (auto toggleViewAction : toggleViewActions)
        {
            emit toggleViewAction->triggered(true);
        }
    });
    auto hideAllLanguageDocks = new QAction(tr("Hide All Language Docks"), this);
    ui->menuView->addAction(hideAllLanguageDocks);
    connect(hideAllLanguageDocks, &QAction::triggered, [toggleViewActions] {
        for (auto toggleViewAction : toggleViewActions)
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
                                            << "*.mdown");
    m_fsModel->sort(0);
    m_fsView->setModel(m_fsModel);
    for (int i = 1; i < m_fsModel->columnCount(); ++i)
        m_fsView->hideColumn(i);
    m_fsView->header()->hide();
    connect(m_fsView, &QTreeView::activated, this, &MainWindow::onFileSystemItemActivated);
    fsDock->setWidget(m_fsView);
    addDockWidget(Qt::LeftDockWidgetArea, fsDock);
    toggleViewAction = fsDock->toggleViewAction();
    toggleViewAction->setShortcut(QKeySequence("Shift+Alt+F"));
    ui->menuView->addAction(toggleViewAction);

    auto *cloudDock = new QDockWidget(tr("Cloud"), this);
    cloudDock->setObjectName("cloudDock");
    m_cloudView     = new QTreeView(cloudDock);
    cloudDock->setWidget(m_cloudView);
    addDockWidget(Qt::LeftDockWidgetArea, cloudDock);
    toggleViewAction = cloudDock->toggleViewAction();
    toggleViewAction->setShortcut(QKeySequence("Shift+Alt+C"));
    ui->menuView->addAction(toggleViewAction);

    tabifyDockWidget(fsDock, cloudDock);

    auto *previewHTMLDock = new QDockWidget(tr("Preview HTML"), this);
    previewHTMLDock->setObjectName("previewHTMLDock");
    m_previewHTMLEditor   = new PreviewThemeEditor(previewHTMLDock);
    m_previewHTMLEditor->initialize("hypertext");
    previewHTMLDock->setWidget(m_previewHTMLEditor);
    addDockWidget(Qt::RightDockWidgetArea, previewHTMLDock);
    toggleViewAction = previewHTMLDock->toggleViewAction();
    toggleViewAction->setShortcut(QKeySequence("Shift+Alt+H"));
    ui->menuView->addAction(toggleViewAction);
    m_view->setPreviewHTMLEditor(m_previewHTMLEditor);

    auto *customThemeEditorDock = new QDockWidget(tr("Custom Theme Editor"), this);
    customThemeEditorDock->setObjectName("customThemeEditorDock");
    m_customPreivewThemeEditor = new CustomPreviewThemeEditWidget(customThemeEditorDock);
    customThemeEditorDock->setWidget(m_customPreivewThemeEditor);
    addDockWidget(Qt::RightDockWidgetArea, customThemeEditorDock);
    toggleViewAction = customThemeEditorDock->toggleViewAction();
    toggleViewAction->setShortcut(QKeySequence("Shift+Alt+T"));
    ui->menuView->addAction(toggleViewAction);
    m_view->setCustomPreivewThemeEditor(m_customPreivewThemeEditor->editor());
    connect(m_customPreivewThemeEditor, &CustomPreviewThemeEditWidget::contentModified, this, &MainWindow::onCustomPreviewThemeChanged);

    auto *devToolDock = new QDockWidget(tr("DevTool"), this);
    devToolDock->setObjectName("devToolDock");
    auto *devToolView = new QWebEngineView(devToolDock);
    devToolView->setPage(m_view->devToolPage());
    devToolDock->setWidget(devToolView);
    addDockWidget(Qt::RightDockWidgetArea, devToolDock);
    toggleViewAction = devToolDock->toggleViewAction();
    toggleViewAction->setShortcut(QKeySequence("F12"));
    ui->menuView->addAction(toggleViewAction);

    tabifyDockWidget(previewHTMLDock, devToolDock);
    tabifyDockWidget(previewHTMLDock, customThemeEditorDock);
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
            QList<QToolBar *> toolbars = {ui->fileToolbar, ui->editToolbar, ui->formatToolbar, ui->shortcutToolbar};
            for (auto tb : toolbars)
            {
                if (tb->isVisible())
                {
                    m_visibleToolbars.append(tb);
                    tb->setVisible(false);
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

        auto *e = static_cast<QWindowStateChangeEvent *>(event);
        if (e->oldState() & Qt::WindowFullScreen)
        {
            for (auto a : qAsConst(m_visibleToolbars))
            {
                a->setVisible(true);
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
    Q_ASSERT(m_view);
    QString text = m_view->selectedText();
    if (text.isEmpty())
        return;
    Q_ASSERT(m_youdaoDict);
    m_youdaoDict->query(text);
}

void MainWindow::on_actionTranslateSelected_triggered()
{
    Q_ASSERT(m_view);
    QString text = m_view->selectedText();
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
        return;
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
        return;

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
    Q_ASSERT(m_view);
    QString text = m_view->fullText();
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
            m_lastWindowState = Minimized;
        else if (isMaximized())
            m_lastWindowState = Maximized;
        else
            m_lastWindowState = Normal;
        showFullScreen();
    }
}

void MainWindow::on_actionFindReplace_triggered()
{
    auto *dlg = getFindReplaceDialog();
    dlg->show();
}
