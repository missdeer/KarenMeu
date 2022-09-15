#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <array>

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QVector>

namespace Ui
{
    class MainWindow;
}
class ClientView;
class PreviewThemeEditor;
class PlantUMLSourceEditor;
class CustomPreviewThemeEditWidget;
class YoudaoDict;
class TranslateHelperPage;
class TranslateOutputWidget;
class TemplateManager;
class FileCache;
class FindReplaceDialog;
class WebBrowser;

QT_FORWARD_DECLARE_CLASS(QComboBox);
QT_FORWARD_DECLARE_CLASS(QTreeView);
QT_FORWARD_DECLARE_CLASS(QPlainTextEdit);
QT_FORWARD_DECLARE_CLASS(QFileSystemModel);
QT_FORWARD_DECLARE_CLASS(QCompleter);
QT_FORWARD_DECLARE_CLASS(QTimer);
QT_FORWARD_DECLARE_CLASS(QSignalMapper);
QT_FORWARD_DECLARE_CLASS(QListWidget);
QT_FORWARD_DECLARE_CLASS(QToolBox);
QT_FORWARD_DECLARE_CLASS(QListWidgetItem);
QT_FORWARD_DECLARE_CLASS(QTabWidget);

struct SampleResult
{
    QString              samplePath;
    QToolBox            *sampleToolBox;
    QVector<QListWidget *> sampleWidgets;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
    MainWindow(const MainWindow &)     = delete;
    MainWindow(MainWindow &&)          = delete;
    void operator=(const MainWindow &) = delete;
    void operator=(MainWindow &&)      = delete;

    void openFile(const QString &fileName);
private slots:
    void on_actionExit_triggered();

    void on_actionContent_triggered();

    void on_actionAbout_triggered();

    void on_actionPreference_triggered();

    void on_actionClearRecentFilesList_triggered();
    void on_actionDictionary_triggered();

    void on_actionTranslateSelected_triggered();

    void onSetCurrentDocument(const QString &fileName);
    void onSetCurrentWorkspace(const QString &fileName);
    void onFileSystemItemActivated(const QModelIndex &index);

    void onCurrentPreviewModeChanged(const QString &text);
    void onCurrentMarkdownEngineChanged(const QString &text);
    void onCurrentCodeBlockStyleChanged(const QString &text);
    void onCurrentPreviewThemeChanged(const QString &text);
    void onCustomPreviewThemeChanged();
    void onYoudaoDictResult(const QString &res);
    void onNewFromTemplateTriggered();
    void onDocumentModified();
    void onWebBrowserSelectionChangedTimeout();
    void onSampleItemDoubleClicked(QListWidgetItem *item);
    void onSampleResultFocus();
    void onSampleItemInsert(QWidget *widget);
    void onSampleItemSelectionChanged();
    void onCurrentSampleResultChanged(int index);
    void onSampleResultDockLocationChanged(Qt::DockWidgetArea area);

    void on_actionGoogle_triggered();

    void on_actionBaidu_triggered();

    void on_actionYoudao_triggered();

    void on_actionSogou_triggered();

    void on_actionDeepL_triggered();

    void on_actionTranslateTextInClipboard_triggered();

    void on_actionTemplateManager_triggered();

    void on_actionOpenWorkspace_triggered();

    void on_actionSaveWorkspace_triggered();

    void on_actionSaveWorkspaceAs_triggered();

    void on_actionClearRecentWorkspaceList_triggered();

    void on_actionTranslateFullText_triggered();

    void on_actionFullScreen_triggered();

    void on_actionFindReplace_triggered();

    void on_actionLeftRightOrTopBottomViews_triggered();

protected:
    void closeEvent(QCloseEvent *event) override;
    void changeEvent(QEvent *event) override;

private:
    enum
    {
        MaxRecentFiles = 10
    };

    enum
    {
        Minimized,
        Normal,
        Maximized,
        FullScreen,
    };

    Ui::MainWindow                       *ui;
    FileCache                            *m_fileCache;
    ClientView                           *m_clientView;
    QFileSystemModel                     *m_fsModel {nullptr};
    QTreeView                            *m_fsView {nullptr};
    QTreeView                            *m_webDAVView {nullptr};
    QComboBox                            *m_cbPreviewMode {nullptr};
    QComboBox                            *m_cbMarkdownEngine {nullptr};
    QComboBox                            *m_cbCodeBlockStyle {nullptr};
    QComboBox                            *m_cbPreviewTheme {nullptr};
    TranslateOutputWidget                *m_googleTranslateEditor {nullptr};
    TranslateOutputWidget                *m_baiduTranslateEditor {nullptr};
    TranslateOutputWidget                *m_youdaoTranslateEditor {nullptr};
    TranslateOutputWidget                *m_sogouTranslateEditor {nullptr};
    TranslateOutputWidget                *m_deepLTranslateEditor {nullptr};
    QPlainTextEdit                       *m_youdaoDictionaryEditor {nullptr};
    PreviewThemeEditor                   *m_previewHTMLEditor {nullptr};
    PlantUMLSourceEditor                 *m_sampleSourcePreview {nullptr};
    QTabWidget                           *m_tabWidgetSampleResult {nullptr};
    QVector<SampleResult *>               m_sampleResults;
    CustomPreviewThemeEditWidget         *m_customPreivewThemeEditor {nullptr};
    QVector<QToolBar *>                   m_visibleToolbars;
    YoudaoDict                           *m_youdaoDict;
    WebBrowser                           *m_webBrowser {nullptr};
    QCompleter                           *m_urlCompleter {nullptr};
    QTimer                               *m_webPageSelectionChangedTimer {nullptr};
    TemplateManager                      *m_templateManager;
    QSignalMapper                        *m_sampleInsertSignalMapper;
    std::array<QAction *, MaxRecentFiles> recentFileActs;
    std::array<QAction *, MaxRecentFiles> recentWorkspaceActs;
    QVector<QAction *>                    m_newFromTemplateActions;
    QString                               m_currentOpenedFile;
    QStringList                           m_urlCompleterModel {"https://medium.com/@ralph.kootker",
                                     "https://medium.com/@happy.cerberus",
                                     "https://ggulgulia.medium.com/",
                                     "https://brevzin.github.io/posts/",
                                     "https://shafik.github.io/",
                                     "https://www.foonathan.net/",
                                     "https://www.cppstories.com/p/archive/",
                                     "http://www.modernescpp.com/index.php",
                                     "https://www.fluentcpp.com/",
                                     "https://isocpp.org"};
    QNetworkAccessManager                 m_nam;
    int                                   m_lastWindowState;

    void               updateRecentFileActions(const QStringList &files);
    void               updateRecentWorkspaceActions(const QStringList &files);
    QString            strippedName(const QString &fullFileName);
    void               openRecentFile();
    void               openRecentWorkspace();
    void               applyMarkdownEditorTheme();
    void               predrawBackgroundImage();
    void               setupDockPanels();
    void               setupShortcutToolbar();
    void               updateTranslationActions();
    void               translateText(const QString &text);
    void               updateNewFromTemplateMenus();
    void               newDocumentWithContent(const QString &content);
    void               openMarkdownDocument(const QString &fileName);
    void               openWorkspace(const QString &fileName);
    void               saveWorkspace(const QString &fileName);
    void               updateWindowTitle();
    FindReplaceDialog *getFindReplaceDialog();
    void               setupWebBrowserPane();
    void               reloadSamples(SampleResult *sr);
    void               insertSampleSource(const QString &code);
    void               focusSampleResult();
    QListWidget       *newSampleListWidget(const QSize &icon_size, QWidget *parent);
};

#endif // MAINWINDOW_H
