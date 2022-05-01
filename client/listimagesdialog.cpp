#include <QClipboard>
#include <QFileDialog>
#include <QGuiApplication>
#include <QImageWriter>
#include <QMimeDatabase>

#include "listimagesdialog.h"
#include "networkreplyhelper.h"
#include "ui_listimagesdialog.h"

ListImagesDialog::ListImagesDialog(const QStringList &images, QNetworkAccessManager *nam, QWidget *parent)
    : QDialog(parent), ui(new Ui::ListImagesDialog), m_nam(nam)
{
    ui->setupUi(this);
    ui->listWidget->addItems(images);
}

ListImagesDialog::~ListImagesDialog()
{
    delete ui;
}

void ListImagesDialog::on_btnSaveAs_clicked()
{
    QStringList          mimeTypeFilters;
    const QByteArrayList supportedMimeTypes = QImageWriter::supportedMimeTypes();
    foreach (const QByteArray &mimeTypeName, supportedMimeTypes)
    {
        mimeTypeFilters.append(mimeTypeName);
    }
    mimeTypeFilters.sort(Qt::CaseInsensitive);

    // compose filter for all supported types
    QMimeDatabase mimeDB;
    QStringList   allSupportedFormats;
    for (const QString &mimeTypeFilter : mimeTypeFilters)
    {
        QMimeType mimeType = mimeDB.mimeTypeForName(mimeTypeFilter);
        if (mimeType.isValid())
        {
            allSupportedFormats.append(mimeType.globPatterns());
        }
    }
    QString allSupportedFormatsFilter = tr("All supported formats (%1)").arg(allSupportedFormats.join(' '));

    auto filePath = QFileDialog::getSaveFileName(this, tr("Save image to file"), QString(), allSupportedFormatsFilter);

    if (!filePath.isEmpty())
    {
        auto   *current = ui->listWidget->currentItem();
        QString image   = current->text();
        auto    pixmap  = pixmapFromString(image);
        pixmap.save(filePath);
    }
}

void ListImagesDialog::on_btnCopyImage_clicked()
{
    auto   *current   = ui->listWidget->currentItem();
    QString image     = current->text();
    auto    pixmap    = pixmapFromString(image);
    auto   *clipboard = QGuiApplication::clipboard();
    if (clipboard)
    {
        clipboard->setPixmap(pixmap);
    }
}

void ListImagesDialog::on_btnClose_clicked()
{
    reject();
}

void ListImagesDialog::on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem * /*previous*/)
{
    QString image       = current->text();
    auto    previewSize = ui->imagePreview->size();
    auto    pixmap      = pixmapFromString(image);
    if (pixmap.width() > previewSize.width() || pixmap.height() > previewSize.height())
    {
        pixmap = pixmap.scaled(previewSize, Qt::KeepAspectRatio);
    }
    ui->imagePreview->setPixmap(pixmap);
}

QPixmap ListImagesDialog::pixmapFromString(const QString &image)
{
    if (image.startsWith("file://"))
    {
        QPixmap pixmap(image.mid(8));
        return pixmap;
    }
    else if (image.startsWith("http://") || image.startsWith("https://"))
    {
        QNetworkRequest req(image);
        req.setAttribute(QNetworkRequest::Http2AllowedAttribute, true);
        req.setRawHeader("Accept-Encoding", "gzip, deflate");
        Q_ASSERT(m_nam);
        auto *reply  = m_nam->get(req);
        auto *helper = new NetworkReplyHelper(reply);
        helper->waitForFinished();
        auto imgData = helper->content();
        helper->deleteLater();
        QPixmap pixmap;
        pixmap.loadFromData(imgData);
        return pixmap;
    }
    else
    {
        static auto pattern = QRegularExpression("^data:image/([a-zA-Z0-9]+);base64,");
        auto        match   = pattern.match(image);
        if (match.hasMatch())
        {
            auto    matched   = match.captured(0);
            auto    imageData = image.mid(matched.length());
            auto    imgData   = QByteArray::fromBase64(imageData.toUtf8());
            QPixmap pixmap;
            pixmap.loadFromData(imgData);
            return pixmap;
        }
    }
    return {};
}
