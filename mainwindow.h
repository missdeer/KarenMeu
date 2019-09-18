#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
    
protected:
    void closeEvent(QCloseEvent *event) override;
private:
    Ui::MainWindow *ui;
    MarkdownView *m_view;
};

#endif // MAINWINDOW_H
