#include <QAction>
#include <QCloseEvent>
#include <QComboBox>
#include <QCoreApplication>
#include <QDesktopServices>
#include <QDockWidget>
#include <QFileInfo>
#include <QFileSystemModel>
#include <QGraphicsColorizeEffect>
#include <QHash>
#include <QHeaderView>
#include <QLabel>
#include <QMap>
#include <QMessageBox>
#include <QPainter>
#include <QPlainTextEdit>
#include <QSplitter>
#include <QTreeView>
#include <QUrl>
#include <QWebEngineView>
#include <QWindowStateChangeEvent>
#include <QtCore>

#include "mainwindow.h"

#include "ColorHelper.h"
#include "custompreviewthemeeditwidget.h"
#include "markdowneditor2.h"
#include "markdownview.h"
#include "preferencedialog.h"
#include "previewthemeeditor.h"
#include "settings.h"
#include "translatehelperpage.h"
#include "translateoutputwidget.h"
#include "ui_mainwindow.h"
#include "utils.h"
#include "youdao.h"

using LabelActionMap = QMap<QString, QAction *>;
using ActionLabelMap = QHash<QAction *, QString>;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), m_view(new MarkdownView(this)), m_youdaoDict(new Youdao(m_nam))
{
    ui->setupUi(this);
    setCentralWidget(m_view);
    connect(ui->actionNew, &QAction::triggered, m_view, &MarkdownView::newDocument);
    connect(ui->actionOpen, &QAction::triggered, m_view, &MarkdownView::openDocument);
    connect(ui->actionSave, &QAction::triggered, m_view, &MarkdownView::saveDocument);
    connect(ui->actionSaveAs, &QAction::triggered, m_view, &MarkdownView::saveAsDocument);
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
    connect(m_view, &MarkdownView::setCurrentFile, this, &MainWindow::onSetCurrentFile);
    connect(m_youdaoDict, &Youdao::result, this, &MainWindow::onYoudaoDictResult);

    ui->menuView->addAction(ui->fileToolbar->toggleViewAction());
    ui->menuView->addAction(ui->editToolbar->toggleViewAction());
    ui->menuView->addAction(ui->formatToolbar->toggleViewAction());
    ui->menuView->addAction(ui->optionToolbar->toggleViewAction());

    for (int i = 0; i < MaxRecentFiles; ++i)
    {
        recentFileActs[i] = new QAction(this);
        recentFileActs[i]->setVisible(false);
        connect(recentFileActs[i], &QAction::triggered, this, &MainWindow::openRecentFile);
        ui->menuRecentFiles->addAction(recentFileActs[i]);
    }

    updateTranslationActions();

    setupOptionToolbar();

    setupDockPanels();

    applyMarkdownEditorTheme();

    auto mainWindowState = g_settings->mainWindowState();
    restoreState(mainWindowState);

    auto mainWindowGeometry = g_settings->mainWindowGeometry();
    restoreGeometry(mainWindowGeometry);
}

MainWindow::~MainWindow()
{
    auto mainWindowState = saveState();
    g_settings->setMainWindowState(mainWindowState);
    auto mainWindowGeometry = saveGeometry();
    g_settings->setMainWindowGeometry(mainWindowGeometry);

    g_settings->save();
    delete ui;
}

void MainWindow::openFile(const QString &fileName)
{
    Q_ASSERT(m_view);

    m_view->newDocument();
    m_view->openFromFile(fileName);
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
}

void MainWindow::showDictTranslateResult(QPlainTextEdit *editor, const QString &res)
{
    Q_ASSERT(editor);
    editor->clear();
    editor->appendHtml(res);
    editor->moveCursor(QTextCursor::Start);
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

void MainWindow::updateRecentFileActions()
{
    QSettings   settings;
    QStringList files = settings.value("recentFileList").toStringList();

    int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

    for (int i = 0; i < numRecentFiles; ++i)
    {
        QString text = tr("%1. %2").arg(i + 1).arg(strippedName(files[i]));
        recentFileActs[i]->setText(text);
        recentFileActs[i]->setData(files[i]);
        recentFileActs[i]->setVisible(true);
    }
    for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
        recentFileActs[j]->setVisible(false);

    // separatorAct->setVisible(numRecentFiles > 0);
}

void MainWindow::onSetCurrentFile(const QString &fileName)
{
    m_curFile = fileName;
    setWindowFilePath(m_curFile);

    setWindowTitle(QString("%1 - KarenMeu").arg(QFileInfo(m_curFile).fileName()));

    QSettings   settings;
    QStringList files = settings.value("recentFileList").toStringList();
    files.removeAll(fileName);
    files.prepend(fileName);
    while (files.size() > MaxRecentFiles)
        files.removeLast();

    settings.setValue("recentFileList", files);

    updateRecentFileActions();
}

void MainWindow::onFileSystemItemActivated(const QModelIndex &index)
{
    auto fi = m_fsModel->fileInfo(index);
    if (fi.isFile() && (fi.fileName().endsWith(".md", Qt::CaseInsensitive) || fi.fileName().endsWith(".markdown", Qt::CaseInsensitive) ||
                        fi.fileName().endsWith(".mdown", Qt::CaseInsensitive)))
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
    showDictTranslateResult(m_youdaoDictionaryEditor, res);
}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void MainWindow::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
        openFile(action->data().toString());
}

void MainWindow::on_actionClearRecentFilesList_triggered()
{
    QSettings   settings;
    QStringList files;
    settings.setValue("recentFileList", files);

    updateRecentFileActions();
}

void MainWindow::applyMarkdownEditorTheme()
{
    Q_ASSERT(g_settings);
    auto &theme = g_settings->theme();

    if (EditorAspectStretch == theme.getEditorAspect())
    {
        theme.setBackgroundColor(theme.getEditorBackgroundColor());
    }

    QString     styleSheet;
    QTextStream stream(&styleSheet);

    double fgBrightness = ColorHelper::getLuminance(theme.getDefaultTextColor());
    double bgBrightness = ColorHelper::getLuminance(theme.getEditorBackgroundColor());

    QColor scrollBarColor;
    QColor chromeFgColor = theme.getDefaultTextColor();

    // If the background color is brighter than the foreground color...
    if (bgBrightness > fgBrightness)
    {
        // Create a UI chrome color based on a lightened editor text color,
        // such that the new color achieves a lower contrast ratio.
        //
        chromeFgColor = ColorHelper::lightenToMatchContrastRatio(theme.getDefaultTextColor(), theme.getEditorBackgroundColor(), 2.1);

        // Slightly blend the new UI chrome color with the editor background color
        // to help it match the theme better.
        //
        chromeFgColor.setAlpha(220);
        chromeFgColor = ColorHelper::applyAlpha(chromeFgColor, theme.getEditorBackgroundColor());

        // Blend the UI chrome color with the background color even further for
        // the scroll bar color, as the scroll bar will otherwise tend to
        // stand out.
        //
        scrollBarColor = chromeFgColor;
        scrollBarColor.setAlpha(200);
        scrollBarColor = ColorHelper::applyAlpha(scrollBarColor, theme.getEditorBackgroundColor());
    }
    // Else if the foreground color is brighter than the background color...
    else
    {
        chromeFgColor  = chromeFgColor.darker(120);
        scrollBarColor = chromeFgColor;
    }

    QString scrollbarColorRGB   = ColorHelper::toRgbString(scrollBarColor);
    QColor  scrollBarHoverColor = theme.getLinkColor();
    QString scrollBarHoverRGB   = ColorHelper::toRgbString(scrollBarHoverColor);

    QString backgroundColorRGBA;

    if (EditorAspectStretch == theme.getEditorAspect())
    {
        backgroundColorRGBA = "transparent";
    }
    else
    {
        backgroundColorRGBA = ColorHelper::toRgbaString(theme.getEditorBackgroundColor());
    }

    QString editorSelectionFgColorRGB = ColorHelper::toRgbString(theme.getEditorBackgroundColor());

    QString editorSelectionBgColorRGB = ColorHelper::toRgbString(theme.getDefaultTextColor());

    QString menuBarItemFgColorRGB;
    QString menuBarItemBgColorRGBA;
    QString menuBarItemFgPressColorRGB;
    QString menuBarItemBgPressColorRGBA;

    QString fullScreenIcon;
    QString focusIcon;
    QString hemingwayIcon;
    QString htmlPreviewIcon;
    QString hideOpenHudsIcon;
    QString copyHtmlIcon;
    QString exportIcon;
    QString markdownOptionsIcon;

    QString statusBarItemFgColorRGB;
    QString statusBarButtonFgPressHoverColorRGB;
    QString statusBarButtonBgPressHoverColorRGBA;

    if (EditorAspectStretch == theme.getEditorAspect())
    {
        fullScreenIcon      = ":/resources/images/fullscreen-dark.svg";
        focusIcon           = ":/resources/images/focus-dark.svg";
        hemingwayIcon       = ":/resources/images/hemingway-dark.svg";
        htmlPreviewIcon     = ":/resources/images/html-preview-dark.svg";
        hideOpenHudsIcon    = ":/resources/images/hide-huds-dark.svg";
        copyHtmlIcon        = ":/resources/images/copy-html-dark.svg";
        exportIcon          = ":/resources/images/export-dark.svg";
        markdownOptionsIcon = ":/resources/images/configure-dark.svg";

        QColor buttonPressColor(chromeFgColor);
        buttonPressColor.setAlpha(30);

        menuBarItemFgColorRGB       = ColorHelper::toRgbString(chromeFgColor);
        menuBarItemBgColorRGBA      = "transparent";
        menuBarItemFgPressColorRGB  = menuBarItemFgColorRGB;
        menuBarItemBgPressColorRGBA = ColorHelper::toRgbaString(buttonPressColor);

        statusBarItemFgColorRGB              = menuBarItemFgColorRGB;
        statusBarButtonFgPressHoverColorRGB  = menuBarItemFgPressColorRGB;
        statusBarButtonBgPressHoverColorRGBA = menuBarItemBgPressColorRGBA;

        // Remove menu bar text drop shadow effect.
        menuBar()->setGraphicsEffect(nullptr);
    }
    else
    {
        // Make the UI chrome color an off white.  A drop shadow will be
        // applied to supply contrast with the background image.
        //
        QColor chromeFgColor = QColor("#e5e8e8");

        menuBarItemFgColorRGB      = ColorHelper::toRgbString(chromeFgColor);
        menuBarItemBgColorRGBA     = "transparent";
        menuBarItemFgPressColorRGB = menuBarItemFgColorRGB;
        chromeFgColor.setAlpha(50);
        menuBarItemBgPressColorRGBA = ColorHelper::toRgbaString(chromeFgColor);

        fullScreenIcon      = ":/resources/images/fullscreen-light.svg";
        focusIcon           = ":/resources/images/focus-light.svg";
        hemingwayIcon       = ":/resources/images/hemingway-light.svg";
        htmlPreviewIcon     = ":/resources/images/html-preview-light.svg";
        hideOpenHudsIcon    = ":/resources/images/hide-huds-light.svg";
        copyHtmlIcon        = ":/resources/images/copy-html-light.svg";
        exportIcon          = ":/resources/images/export-light.svg";
        markdownOptionsIcon = ":/resources/images/configure-light.svg";

        statusBarItemFgColorRGB              = menuBarItemFgColorRGB;
        statusBarButtonFgPressHoverColorRGB  = menuBarItemFgPressColorRGB;
        statusBarButtonBgPressHoverColorRGBA = menuBarItemBgPressColorRGBA;

        QGraphicsDropShadowEffect *menuBarTextDropShadowEffect = new QGraphicsDropShadowEffect();
        menuBarTextDropShadowEffect->setColor(QColor(Qt::black));
        menuBarTextDropShadowEffect->setBlurRadius(3.5);
        menuBarTextDropShadowEffect->setXOffset(1.0);
        menuBarTextDropShadowEffect->setYOffset(1.0);

        // Set drop shadow effect for menu bar text.
        menuBar()->setGraphicsEffect(menuBarTextDropShadowEffect);
    }

    Q_ASSERT(m_view);
    m_view->editor()->setAspect(theme.getEditorAspect());
    styleSheet = "";

    QString corners           = "";
    QString scrollBarRadius   = "0px";
    QString scrollAreaPadding = "3px 3px 0px 3px";

    if (EditorAspectCenter == theme.getEditorAspect())
    {
        scrollAreaPadding = "3px";
        corners           = "border-radius: 8;";
    }

    scrollBarRadius = "4px";

    QString defaultTextColorRGB = ColorHelper::toRgbString(theme.getDefaultTextColor());

    stream << "QPlainTextEdit { border: 0; " << corners << "margin: 0; padding: 5px; background-color: " << backgroundColorRGBA
           << "; color: " << defaultTextColorRGB << "; selection-color: " << editorSelectionFgColorRGB
           << "; selection-background-color: " << editorSelectionBgColorRGB << " } "
           << "QAbstractScrollArea::corner { background: transparent } "
           << "QAbstractScrollArea { padding: " << scrollAreaPadding << "; margin: 0 } "
           << "QScrollBar::horizontal { border: 0; background: transparent; height: 8px; margin: 0 } "
           << "QScrollBar::handle:horizontal { border: 0; background: " << scrollbarColorRGB
           << "; min-width: 50px; border-radius: " << scrollBarRadius << "; } "
           << "QScrollBar::vertical { border: 0; background: transparent; width: 8px; margin: 0 } "
           << "QScrollBar::handle:vertical { border: 0; background: " << scrollbarColorRGB << "; min-height: 50px; border-radius: " << scrollBarRadius
           << "; } "
           << "QScrollBar::handle:vertical:hover { background: " << scrollBarHoverRGB << " } "
           << "QScrollBar::handle:horizontal:hover { background: " << scrollBarHoverRGB << " } "
           << "QScrollBar::add-line { background: transparent; border: 0 } "
           << "QScrollBar::sub-line { background: transparent; border: 0 } ";

    m_view->editor()->setColorScheme(theme.getDefaultTextColor(),
                                     theme.getEditorBackgroundColor(),
                                     theme.getMarkupColor(),
                                     theme.getLinkColor(),
                                     theme.getHeadingColor(),
                                     theme.getEmphasisColor(),
                                     theme.getBlockquoteColor(),
                                     theme.getCodeColor(),
                                     theme.getSpellingErrorColor());

    m_view->editor()->setStyleSheet(styleSheet);
    styleSheet = "";

    // Wipe out old background image drawing material.
    originalBackgroundImage = QPixmap();
    adjustedBackgroundImage = QPixmap();

    if (!theme.getBackgroundImageUrl().isNull() && !theme.getBackgroundImageUrl().isEmpty())
    {
        // Predraw background image for paintEvent()
        originalBackgroundImage.load(theme.getBackgroundImageUrl());
        predrawBackgroundImage();
    }

    stream << "#editorLayoutArea { background-color: transparent; border: 0; margin: 0 }"
           << "QMenuBar { background: transparent } "
           << "QMenuBar::item { background: " << menuBarItemBgColorRGBA << "; color: " << menuBarItemFgColorRGB << "; padding: 4px 6px 4px 6px } "
           << "QMenuBar::item:pressed { background-color: " << menuBarItemBgPressColorRGBA << "; color: " << menuBarItemFgPressColorRGB
           << "; border-radius: 3px"
           << " } ";

    // Do not call this->setStyleSheet().  Calling it more than once in a run
    // (i.e., when changing a theme) causes a crash in Qt 5.11.  Instead,
    // change the main window's style sheet via qApp.
    //
    qApp->setStyleSheet(styleSheet);
    styleSheet = "";

    stream << "QSplitter::handle:vertical { height: 0px; } "
           << "QSplitter::handle:horizontal { width: 0px; } ";

    m_view->splitter()->setStyleSheet(styleSheet);
}

void MainWindow::predrawBackgroundImage()
{
    qreal dpr = 1.0;

#if QT_VERSION >= 0x050600
    dpr = this->devicePixelRatioF();
#endif

    QPixmap image(originalBackgroundImage);

#if QT_VERSION >= 0x050600
    image.setDevicePixelRatio(dpr);
#endif

    adjustedBackgroundImage = QPixmap(this->size() * dpr);

#if QT_VERSION >= 0x050600
    adjustedBackgroundImage.setDevicePixelRatio(dpr);
#endif

    Q_ASSERT(g_settings);
    auto &theme = g_settings->theme();
    adjustedBackgroundImage.fill(theme.getBackgroundColor().rgb());

    QPainter painter(&adjustedBackgroundImage);
    painter.setPen(Qt::NoPen);

    if (PictureAspectTile == theme.getBackgroundImageAspect())
    {
        qreal inverseRatio = 1.0 / dpr;

        painter.scale(inverseRatio, inverseRatio);
        painter.fillRect(adjustedBackgroundImage.rect(), image);
    }
    else
    {
        Qt::AspectRatioMode aspectRatioMode = Qt::IgnoreAspectRatio;
        bool                scaleImage      = true;

        switch (theme.getBackgroundImageAspect())
        {
        case PictureAspectZoom:
            aspectRatioMode = Qt::KeepAspectRatioByExpanding;
            break;
        case PictureAspectScale:
            aspectRatioMode = Qt::KeepAspectRatio;
            break;
        case PictureAspectStretch:
            aspectRatioMode = Qt::IgnoreAspectRatio;
            break;
        default:
            // Centered
            scaleImage = false;
            break;
        }

        if (scaleImage)
        {
            image = image.scaled(adjustedBackgroundImage.size(), aspectRatioMode, Qt::SmoothTransformation);

#if QT_VERSION >= 0x050600
            image.setDevicePixelRatio(dpr);
#endif
        }

        int xpos = (adjustedBackgroundImage.width() - image.width()) / (2.0 * dpr);
        int ypos = (adjustedBackgroundImage.height() - image.height()) / (2.0 * dpr);

        painter.drawPixmap(xpos, ypos, image);
    }

    painter.end();
}

void MainWindow::setupDockPanels()
{
    ui->menuView->addSeparator();

    setDockOptions(QMainWindow::AnimatedDocks | QMainWindow::AllowTabbedDocks | QMainWindow::GroupedDragging);

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
    auto toggleViewAction = fsDock->toggleViewAction();
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

    auto initializeEditor = [](QPlainTextEdit *editor) {
        QFont font(editor->font());
        font.setWeight(QFont::Normal);
        font.setItalic(false);
        font.setPointSizeF(g_settings->codeEditorFontPointSize());

        QStringList fonts;
#if defined(Q_OS_WIN)
        fonts << "Microsoft YaHei UI";
#elif defined(Q_OS_MAC)
        fonts << "PingFang SC"
              << "PingFang HK"
              << "PingFang TC";
#else
        fonts << "WenQuanYi Micro Hei";
#endif
        fonts << editor->font().families();
        QFont::insertSubstitutions(editor->font().family(), fonts);
        font.setFamilies(fonts);
        editor->setFont(font);
    };

    auto  getSelectionCallback = std::bind(&MarkdownView::selectedText, m_view);
    auto *googleTranslateDock = new QDockWidget(tr("Google Translate"), this);
    googleTranslateDock->setObjectName("googleTranslate");
    m_googleTranslateEditor = new TranslateOutputWidget(TST_GOOGLE, googleTranslateDock);
    m_googleTranslateEditor->setGetSelectionCallback(getSelectionCallback);
    initializeEditor(m_googleTranslateEditor->editor());
    googleTranslateDock->setWidget(m_googleTranslateEditor);
    addDockWidget(Qt::BottomDockWidgetArea, googleTranslateDock);
    toggleViewAction = googleTranslateDock->toggleViewAction();
    toggleViewAction->setShortcut(QKeySequence("Shift+Alt+G"));
    ui->menuView->addAction(toggleViewAction);

    auto *baiduTranslateDock = new QDockWidget(tr("Baidu Translate"), this);
    baiduTranslateDock->setObjectName("baiduTranslateDock");
    m_baiduTranslateEditor = new TranslateOutputWidget(TST_BAIDU, baiduTranslateDock);
    m_baiduTranslateEditor->setGetSelectionCallback(getSelectionCallback);
    initializeEditor(m_baiduTranslateEditor->editor());
    baiduTranslateDock->setWidget(m_baiduTranslateEditor);
    addDockWidget(Qt::BottomDockWidgetArea, baiduTranslateDock);
    toggleViewAction = baiduTranslateDock->toggleViewAction();
    toggleViewAction->setShortcut(QKeySequence("Shift+Alt+B"));
    ui->menuView->addAction(toggleViewAction);

    auto *youdaoTranslateDock = new QDockWidget(tr("Youdao Translate"), this);
    youdaoTranslateDock->setObjectName("youdaoTranslateDock");
    m_youdaoTranslateEditor = new TranslateOutputWidget(TST_YOUDAO, youdaoTranslateDock);
    m_youdaoTranslateEditor->setGetSelectionCallback(getSelectionCallback);
    initializeEditor(m_youdaoTranslateEditor->editor());
    youdaoTranslateDock->setWidget(m_youdaoTranslateEditor);
    addDockWidget(Qt::BottomDockWidgetArea, youdaoTranslateDock);
    toggleViewAction = youdaoTranslateDock->toggleViewAction();
    toggleViewAction->setShortcut(QKeySequence("Shift+Alt+Y"));
    ui->menuView->addAction(toggleViewAction);

    auto *sogouTranslateDock = new QDockWidget(tr("Sogou Translate"), this);
    sogouTranslateDock->setObjectName("sogouTranslateDock");
    m_sogouTranslateEditor = new TranslateOutputWidget(TST_SOGOU, sogouTranslateDock);
    m_sogouTranslateEditor->setGetSelectionCallback(getSelectionCallback);
    initializeEditor(m_sogouTranslateEditor->editor());
    sogouTranslateDock->setWidget(m_sogouTranslateEditor);
    addDockWidget(Qt::BottomDockWidgetArea, sogouTranslateDock);
    toggleViewAction = sogouTranslateDock->toggleViewAction();
    toggleViewAction->setShortcut(QKeySequence("Shift+Alt+S"));
    ui->menuView->addAction(toggleViewAction);

    auto *youdaoDictionaryDock = new QDockWidget(tr("Youdao Dictionary"), this);
    youdaoDictionaryDock->setObjectName("youdaoDictionaryDock");
    m_youdaoDictionaryEditor = new QPlainTextEdit(youdaoDictionaryDock);
    initializeEditor(m_youdaoDictionaryEditor);
    youdaoDictionaryDock->setWidget(m_youdaoDictionaryEditor);
    addDockWidget(Qt::BottomDockWidgetArea, youdaoDictionaryDock);
    toggleViewAction = youdaoDictionaryDock->toggleViewAction();
    toggleViewAction->setShortcut(QKeySequence("Shift+Alt+D"));
    ui->menuView->addAction(toggleViewAction);

    tabifyDockWidget(googleTranslateDock, baiduTranslateDock);
    tabifyDockWidget(baiduTranslateDock, youdaoTranslateDock);
    tabifyDockWidget(youdaoTranslateDock, sogouTranslateDock);
    tabifyDockWidget(sogouTranslateDock, youdaoDictionaryDock);

    auto *previewHTMLDock = new QDockWidget(tr("Preview HTML"), this);
    previewHTMLDock->setObjectName("previewHTMLDock");
    m_previewHTMLEditor   = new PreviewThemeEditor(previewHTMLDock);
    m_previewHTMLEditor->initialize("html");
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

void MainWindow::setupOptionToolbar()
{
    ui->optionToolbar->addWidget(new QLabel(tr("Mode:")));
    m_cbPreviewMode = new QComboBox(ui->optionToolbar);
    ui->optionToolbar->addWidget(m_cbPreviewMode);
    ui->optionToolbar->addWidget(new QLabel(tr("Engine:")));
    m_cbMarkdownEngine = new QComboBox(ui->optionToolbar);
    ui->optionToolbar->addWidget(m_cbMarkdownEngine);
    ui->optionToolbar->addWidget(new QLabel(tr("Code Block Style:")));
    m_cbCodeBlockStyle = new QComboBox(ui->optionToolbar);
    ui->optionToolbar->addWidget(m_cbCodeBlockStyle);
    ui->optionToolbar->addWidget(new QLabel(tr("Preview Theme:")));
    m_cbPreviewTheme = new QComboBox(ui->optionToolbar);
    ui->optionToolbar->addWidget(m_cbPreviewTheme);

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
    for (auto entry : entries)
    {
        m_cbPreviewTheme->addItem(entry.baseName());
    }
    m_cbPreviewTheme->addItem(tr("Custom"));
    m_cbPreviewTheme->setCurrentText(g_settings->previewTheme());
    connect(m_cbPreviewTheme, &QComboBox::currentTextChanged, this, &MainWindow::onCurrentPreviewThemeChanged);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    adjustEditorWidth(event->size().width());

    if (!originalBackgroundImage.isNull())
    {
        predrawBackgroundImage();
    }
}

void MainWindow::moveEvent(QMoveEvent *event)
{
    Q_UNUSED(event)

    // Need to redraw the background image in case the window has moved
    // onto a different screen where the device pixel ratio is different.
    //
    if (!originalBackgroundImage.isNull())
    {
        predrawBackgroundImage();
    }
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    Q_ASSERT(g_settings);
    auto &theme = g_settings->theme();

    QPainter painter(this);
    qreal    dpr = 1.0;

#if QT_VERSION >= 0x050600
    dpr = devicePixelRatioF();
#endif

    QRect rect(event->rect().topLeft() * dpr, event->rect().size() * dpr);

    painter.fillRect(rect, theme.getBackgroundColor().rgb());

    if (!adjustedBackgroundImage.isNull())
    {
        painter.drawPixmap(0, 0, adjustedBackgroundImage);
    }

    if (EditorAspectStretch == theme.getEditorAspect())
    {
        painter.fillRect(this->rect(), theme.getEditorBackgroundColor());
    }

    painter.end();

    QMainWindow::paintEvent(event);
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::WindowStateChange)
    {
        QList<QToolBar *> toolbars = {ui->fileToolbar, ui->editToolbar, ui->formatToolbar};
        if (windowState() & Qt::WindowFullScreen)
        {
            auto actions = ui->menuView->actions();
            for (auto tb : toolbars)
            {
                if (tb->isVisible())
                {
                    m_visibleToolbars.append(tb);
                    tb->setVisible(false);
                }
            }
            event->accept();
            return;
        }

        QWindowStateChangeEvent *e = static_cast<QWindowStateChangeEvent *>(event);
        if (e->oldState() & Qt::WindowFullScreen)
        {
            for (auto a : m_visibleToolbars)
            {
                a->setVisible(true);
            }
            m_visibleToolbars.clear();
            event->accept();
            return;
        }
    }
    event->ignore();
}

void MainWindow::adjustEditorWidth(int width)
{
    int editorWidth = width;

    editorWidth /= 2;

    QList<int> sizes;
    sizes.append(editorWidth);
    sizes.append(editorWidth);
    Q_ASSERT(m_view);
    m_view->splitter()->setSizes(sizes);

    // Resize the editor's margins based on the size of the window.
    m_view->editor()->setupPaperMargins(editorWidth);

    // Scroll to cursor position.
    m_view->editor()->centerCursor();
}

void MainWindow::on_actionDictionary_triggered()
{
    Q_ASSERT(m_youdaoDictionaryEditor);
    m_youdaoDictionaryEditor->clear();
    Q_ASSERT(m_view);
    QString text = m_view->selectedText();
    if (!text.isEmpty())
    {
        Q_ASSERT(m_youdaoDict);
        m_youdaoDict->query(text);
    }
}

void MainWindow::on_actionTranslate_triggered()
{
    Q_ASSERT(m_view);
    QString text = m_view->selectedText();
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
