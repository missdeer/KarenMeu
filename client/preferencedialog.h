#ifndef PREFERENCEDIALOG_H
#define PREFERENCEDIALOG_H

#include <QDialog>
#include <QVector>

namespace Ui
{
    class PreferenceDialog;
}

QT_FORWARD_DECLARE_CLASS(QListWidgetItem);

class PreviewThemeEditor;
class QWebdav;
class QWebdavDirParser;
struct WebDAVInfo;

class PreferenceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferenceDialog(QWidget *parent = nullptr);
    ~PreferenceDialog() override;

protected:
    void accept() override;
private slots:
    void onWebDAVFinished();
    void onWebDAVError(QString errorMsg);

    void on_cbPreviewTheme_currentTextChanged(const QString &text);

    void on_btnBrowseJavaPath_clicked();
    void on_btnBrowseGraphvizDotPath_clicked();
    void on_btnBrowsePlantUMLJarPath_clicked();

    void on_btnTestWebDAV_clicked();

    void on_btnAddWebDAV_clicked();

    void on_btnRemoveWebDAV_clicked();

    void on_listWebDAV_currentRowChanged(int currentRow);

private:
    Ui::PreferenceDialog *ui;
    PreviewThemeEditor   *m_previewThemeEditor;
    QWebdav              *m_webDAV;
    QWebdavDirParser     *m_webDAVDirParser;
    QVector<WebDAVInfo>   m_webDAVInfos;
    void                  setupPreviewThemeEditor();
};

#endif // PREFERENCEDIALOG_H
