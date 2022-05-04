#ifndef PREFERENCEDIALOG_H
#define PREFERENCEDIALOG_H

#include <QDialog>

namespace Ui
{
    class PreferenceDialog;
}

QT_FORWARD_DECLARE_CLASS(QListWidgetItem);

class PreviewThemeEditor;
class QWebdav;

class PreferenceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferenceDialog(QWidget *parent = nullptr);
    ~PreferenceDialog() override;

protected:
    void accept() override;
private slots:
    void on_cbPreviewTheme_currentTextChanged(const QString &text);

    void on_btnBrowseJavaPath_clicked();
    void on_btnBrowseGraphvizDotPath_clicked();
    void on_btnBrowsePlantUMLJarPath_clicked();

    void on_btnTestWebDAV_clicked();

    void on_btnAddWebDAV_clicked();

    void on_btnRemoveWebDAV_clicked();

    void on_listWebDAV_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

private:
    Ui::PreferenceDialog *ui;
    PreviewThemeEditor   *m_previewThemeEditor;
    QWebdav              *m_webDav;
    void                  setupPreviewThemeEditor();
};

#endif // PREFERENCEDIALOG_H
