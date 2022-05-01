#ifndef LISTIMAGESDIALOG_H
#define LISTIMAGESDIALOG_H

#include <QDialog>

QT_FORWARD_DECLARE_CLASS(QListWidgetItem);
QT_FORWARD_DECLARE_CLASS(QNetworkAccessManager);

namespace Ui {
    class ListImagesDialog;
}

class ListImagesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ListImagesDialog(const QStringList &images, QNetworkAccessManager *nam, QWidget *parent = nullptr);
    ~ListImagesDialog();

private slots:
    void on_btnSaveAs_clicked();

    void on_btnCopyImage_clicked();

    void on_btnClose_clicked();

    void on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *);

protected:
    void resizeEvent(QResizeEvent *);

private:
    Ui::ListImagesDialog  *ui;
    QNetworkAccessManager *m_nam;
    QPixmap                m_currentPixmap;
    QPixmap                pixmapFromString(const QString &image);
    void                   previewCurrentImage();
};

#endif // LISTIMAGESDIALOG_H
