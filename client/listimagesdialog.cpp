#include <QClipboard>
#include <QFileDialog>
#include <QGuiApplication>
#include <QImageWriter>
#include <QMimeDatabase>
#include <QResizeEvent>

#include "listimagesdialog.h"
#include "networkreplyhelper.h"
#include "ui_listimagesdialog.h"

ListImagesDialog::ListImagesDialog(const QStringList &images, QNetworkAccessManager *nam, QWidget *parent)
    : QDialog(parent), ui(new Ui::ListImagesDialog), m_nam(nam)
{
    ui->setupUi(this);
    ui->listWidget->addItems(images);
    if (!images.isEmpty())
    {
        ui->listWidget->setCurrentRow(0);
    }
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
        m_currentPixmap.save(filePath);
    }
}

void ListImagesDialog::on_btnCopyImage_clicked()
{
    auto *clipboard = QGuiApplication::clipboard();
    if (clipboard)
    {
        clipboard->setPixmap(m_currentPixmap);
    }
}

void ListImagesDialog::on_btnClose_clicked()
{
    reject();
}

void ListImagesDialog::previewCurrentImage()
{
    auto previewSize = ui->imagePreview->size();
    if (m_currentPixmap.width() > previewSize.width() || m_currentPixmap.height() > previewSize.height())
    {
        auto pixmap = m_currentPixmap.scaled(previewSize, Qt::KeepAspectRatio);
        ui->imagePreview->setPixmap(pixmap);
    }
    else
    {
        ui->imagePreview->setPixmap(m_currentPixmap);
    }
}

void ListImagesDialog::on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem * /*previous*/)
{
    QString image   = current->text();
    m_currentPixmap = pixmapFromString(image);
    previewCurrentImage();
}

void ListImagesDialog::resizeEvent(QResizeEvent *)
{
    previewCurrentImage();
}

void ListImagesDialog::onRequestRemoteImageDone()
{
    auto *helper = qobject_cast<NetworkReplyHelper *>(sender());
    Q_ASSERT(helper);
    helper->deleteLater();
    auto *reply = helper->reply();
    Q_ASSERT(reply);
    auto imgData = helper->content();
#if !defined(QT_NO_DEBUG)
    qDebug() << "received" << content.length() << "bytes image data";
#endif
    if (imgData.isEmpty())
        return;

    m_currentPixmap.loadFromData(imgData);
    if (!m_currentPixmap.isNull())
    {
        previewCurrentImage();
    }
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
        Q_ASSERT(m_nam);
        auto *reply  = m_nam->get(req);
        auto *helper = new NetworkReplyHelper(reply);
        helper->setTimeout(10000);
        connect(helper, &NetworkReplyHelper::done, this, &ListImagesDialog::onRequestRemoteImageDone);
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
