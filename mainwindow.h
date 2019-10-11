#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPixmap>

namespace Ui {
class MainWindow;
}
class MarkdownView;

QT_FORWARD_DECLARE_CLASS(QComboBox);

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
    MainWindow(const MainWindow&) = delete;
    MainWindow(MainWindow&&) = delete;
    void operator=(const MainWindow&) = delete;
    void operator=(MainWindow&&) = delete;
    void openFile(const QString& fileName);
private slots:
    void on_actionExit_triggered();
    
    void on_actionContent_triggered();
    
    void on_actionAbout_triggered();
    
    void on_actionPreference_triggered();
    
    void on_actionClearRecentFilesList_triggered();
    
    void setCurrentFile(const QString &fileName);
protected:
    void resizeEvent(QResizeEvent* event) override;
    void moveEvent(QMoveEvent* event) override;
    void closeEvent(QCloseEvent *event) override;
    void paintEvent(QPaintEvent* event) override;
private:
    Ui::MainWindow *ui;
    MarkdownView *m_view;
    
    QString m_curFile;
    
    enum { MaxRecentFiles = 10 };
    QAction *recentFileActs[MaxRecentFiles];
    
    QPixmap originalBackgroundImage;
    QPixmap adjustedBackgroundImage;
    
    void updateRecentFileActions();    
    QString strippedName(const QString &fullFileName);
    
    void openRecentFile();
    void adjustEditorWidth(int width);
    void applyTheme();
    void predrawBackgroundImage();
};

#endif // MAINWINDOW_H
