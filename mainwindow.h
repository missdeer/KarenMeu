#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}
class MarkdownView;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
    
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
    void ApplySettingsToRenderer();
};

#endif // MAINWINDOW_H
