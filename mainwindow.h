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
    explicit MainWindow(const QString& fileName, QWidget *parent = nullptr);
    ~MainWindow() override;
    MainWindow(const MainWindow&) = delete;
    MainWindow(MainWindow&&) = delete;
    void operator=(const MainWindow&) = delete;
    void operator=(MainWindow&&) = delete;
private slots:
    void onMarkdownEngineChanged();
    void onMarkdownEngineCurrentTextChanged(const QString& text);
    void onPreviewThemeChanged();
    void onPreviewThemeCurrentTextChanged(const QString& text);
    void onCodeBlockStyleChanged();
    void onCodeBlockStyleCurrentTextChanged(const QString& text);

    void on_actionExit_triggered();
    
    void on_actionContent_triggered();
    
    void on_actionAbout_triggered();
    
    void on_actionPreference_triggered();
    
protected:
    void closeEvent(QCloseEvent *event) override;
private:
    Ui::MainWindow *ui;
    MarkdownView *m_view;
    QComboBox *m_cbMarkdownEngine;
    QComboBox *m_cbPreviewTheme;
    QComboBox *m_cbCodeBlockStyle;
    void UpdateMarkdownEngineActions(bool first);
    void UpdatePreviewThemeActions(bool first);
    void UpdateCodeBlockStyleActions(bool first);
    void InitEngineToolbar();
};

#endif // MAINWINDOW_H
