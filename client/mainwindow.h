#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>

namespace Ui
{
    class MainWindow;
}
class MarkdownView;
class PreviewThemeEditor;
class CustomPreviewThemeEditWidget;
class Youdao;
class TranslateHelperPage;
class TranslateOutputWidget;
class TemplateManager;

QT_FORWARD_DECLARE_CLASS(QComboBox);
QT_FORWARD_DECLARE_CLASS(QTreeView);
QT_FORWARD_DECLARE_CLASS(QPlainTextEdit);
QT_FORWARD_DECLARE_CLASS(QFileSystemModel);

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

    void onFileSystemItemActivated(const QModelIndex &index);

    void onCurrentPreviewModeChanged(const QString &text);
    void onCurrentMarkdownEngineChanged(const QString &text);
    void onCurrentCodeBlockStyleChanged(const QString &text);
    void onCurrentPreviewThemeChanged(const QString &text);
    void onCustomPreviewThemeChanged();
    void onYoudaoDictResult(QString res);
    void onNewFromTemplateTriggered();
    void onDocumentModified();

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

protected:
    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void changeEvent(QEvent *event) override;

private:
    enum
    {
        MaxRecentFiles = 10
    };

    Ui::MainWindow *              ui;
    MarkdownView *                m_view;
    QFileSystemModel *            m_fsModel;
    QTreeView *                   m_fsView;
    QTreeView *                   m_cloudView;
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
    CustomPreviewThemeEditWidget *m_customPreivewThemeEditor;
    QList<QToolBar *>             m_visibleToolbars;
    Youdao *                      m_youdaoDict;
    TemplateManager *             m_templateManager {nullptr};
    QAction *                     recentFileActs[MaxRecentFiles];
    QAction *                     recentWorkspaceActs[MaxRecentFiles];
    QList<QAction *>              m_newFromTemplateActions;
    QString                       m_currentMarkdownDocument;
    QString                       m_currentWorkspace;
    QNetworkAccessManager         m_nam;

    void    updateRecentFileActions(const QStringList &files);
    void    updateRecentWorkspaceActions(const QStringList &files);
    QString strippedName(const QString &fullFileName);
    void    openRecentFile();
    void    openRecentWorkspace();
    void    adjustEditorWidth(int width);
    void    applyMarkdownEditorTheme();
    void    predrawBackgroundImage();
    void    setupDockPanels();
    void    setupShortcutToolbar();
    void    updateTranslationActions();
    void    translateText(const QString &text);
    void    updateNewFromTemplateMenus();
    void    newDocumentWithContent(const QString &content);
    void    openMarkdownDocument(const QString &fileName);
    void    openWorkspace(const QString &fileName);
    void    updateWindowTitle();
};

#endif // MAINWINDOW_H
