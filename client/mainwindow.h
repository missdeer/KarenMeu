#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPixmap>

namespace Ui
{
    class MainWindow;
}
class MarkdownView;
class PreviewThemeEditor;
class CustomPreviewThemeEditWidget;

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

public slots:
    void openFile(const QString &fileName);

private slots:
    void on_actionExit_triggered();

    void on_actionContent_triggered();

    void on_actionAbout_triggered();

    void on_actionPreference_triggered();

    void on_actionClearRecentFilesList_triggered();

    void setCurrentFile(const QString &fileName);

    void onFileSystemItemActivated(const QModelIndex &index);

    void onCurrentPreviewModeChanged(const QString &text);
    void onCurrentMarkdownEngineChanged(const QString &text);
    void onCurrentCodeBlockStyleChanged(const QString &text);
    void onCurrentPreviewThemeChanged(const QString &text);
    void onCustomPreviewThemeChanged();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void moveEvent(QMoveEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void changeEvent(QEvent *event) override;

private:
    Ui::MainWindow *  ui;
    MarkdownView *    m_view;
    QFileSystemModel *m_fsModel;
    QTreeView *       m_fsView;
    QTreeView *       m_cloudView;
    QComboBox *       m_cbPreviewMode;
    QComboBox *       m_cbMarkdownEngine;
    QComboBox *       m_cbCodeBlockStyle;
    QComboBox *       m_cbPreviewTheme;
    QPlainTextEdit *    m_googleTranslateEditor;
    QPlainTextEdit *    m_baiduTranslateEditor;
    QPlainTextEdit *    m_youdaoTranslateEditor;
    QPlainTextEdit *    m_youdaoDictionaryEditor;
    PreviewThemeEditor *m_previewHTMLEditor;
    CustomPreviewThemeEditWidget *m_customPreivewThemeEditor;
    QList<QToolBar *> m_visibleToolbars;

    QString m_curFile;

    enum
    {
        MaxRecentFiles = 10
    };
    QAction *recentFileActs[MaxRecentFiles];

    QPixmap originalBackgroundImage;
    QPixmap adjustedBackgroundImage;

    void    updateRecentFileActions();
    QString strippedName(const QString &fullFileName);

    void openRecentFile();
    void adjustEditorWidth(int width);
    void applyMarkdownEditorTheme();
    void predrawBackgroundImage();
    void setupDockPanels();
    void setupOptionToolbar();
};

#endif // MAINWINDOW_H
