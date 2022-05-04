#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QList>
#include <QMainWindow>
#include <QNetworkAccessManager>

namespace Ui
{
    class MainWindow;
}
class MarkdownView;
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
    QList<QListWidget *> sampleWidgets;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
    MainWindow(const MainWindow &) = delete;
    MainWindow(MainWindow &&)      = delete;
    void operator=(const MainWindow &) = delete;
    void operator=(MainWindow &&) = delete;

    void openFile(const QString &fileName);
private slots:
    void on_actionExit_triggered();

    void on_actionContent_triggered();

    void on_actionAbout_triggered();

    void on_actionPreference_triggered();

    void on_actionClearRecentFilesList_triggered();
    void on_actionDictionary_triggered();

    void on_actionTranslateSelected_triggered();

    void onSetCurrentMarkdownDocument(const QString &fileName);
    void onSetCurrentWorkspace(const QString &fileName);
    void onFileSystemItemActivated(const QModelIndex &index);

    void onCurrentPreviewModeChanged(const QString &text);
    void onCurrentMarkdownEngineChanged(const QString &text);
    void onCurrentCodeBlockStyleChanged(const QString &text);
    void onCurrentPreviewThemeChanged(const QString &text);
    void onCustomPreviewThemeChanged();
    void onYoudaoDictResult(QString res);
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

    Ui::MainWindow *              ui;
    FileCache *                   m_fileCache;
    MarkdownView *                m_view;
    QFileSystemModel *            m_fsModel;
    QTreeView *                   m_fsView;
    QTreeView *                   m_webDAVView;
    QComboBox *                   m_cbPreviewMode;
    QComboBox *                   m_cbMarkdownEngine;
    QComboBox *                   m_cbCodeBlockStyle;
    QComboBox *                   m_cbPreviewTheme;
    TranslateOutputWidget *       m_googleTranslateEditor;
    TranslateOutputWidget *       m_baiduTranslateEditor;
    TranslateOutputWidget *       m_youdaoTranslateEditor;
    TranslateOutputWidget *       m_sogouTranslateEditor;
    TranslateOutputWidget *       m_deepLTranslateEditor;
    QPlainTextEdit *              m_youdaoDictionaryEditor;
    PreviewThemeEditor *          m_previewHTMLEditor;
    PlantUMLSourceEditor         *m_sampleSourcePreview;
    QTabWidget                   *m_tabWidgetSampleResult;
    QList<SampleResult *>         m_sampleResults;
    CustomPreviewThemeEditWidget *m_customPreivewThemeEditor;
    QList<QToolBar *>             m_visibleToolbars;
    YoudaoDict *                  m_youdaoDict;
    WebBrowser *                  m_webBrowser {nullptr};
    QCompleter *                  m_urlCompleter {nullptr};
    QTimer *                      m_webPageSelectionChangedTimer {nullptr};
    TemplateManager *             m_templateManager;
    QSignalMapper                *m_sampleInsertSignalMapper;
    QAction *                     recentFileActs[MaxRecentFiles];
    QAction *                     recentWorkspaceActs[MaxRecentFiles];
    QList<QAction *>              m_newFromTemplateActions;
    QString                       m_currentOpenedFile;
    QStringList                   m_urlCompleterModel;
    QNetworkAccessManager         m_nam;
    int                           m_lastWindowState;

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
