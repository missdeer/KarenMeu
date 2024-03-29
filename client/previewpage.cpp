#include <QBuffer>
#include <QClipboard>
#include <QDesktopServices>
#include <QGuiApplication>
#include <QImageReader>
#include <QtCore>

#include "previewpage.h"
#include "networkreplyhelper.h"
#include "utils.h"

PreviewPage::PreviewPage(QNetworkAccessManager *nam, QObject *parent) : QWebEnginePage(parent), m_nam(nam)
{
    connect(this, &PreviewPage::embeded, this, &PreviewPage::onEmbeded);
}

void PreviewPage::refreshImage(const QString &imgAlt, const QString &imgSrc)
{
    const QString refreshImageJavascript("var images = document.getElementsByTagName('img');"
                                         "for (i=0;i < images.length;i++) {                 "
                                         "  var alt = images[i].alt;                        "
                                         "  var src = images[i].src;                        "
                                         "  queryPos = src.indexOf('?');                    "
                                         "  if(queryPos != -1) {                            "
                                         "    src = src.substring(0, queryPos);             "
                                         "  }                                               "
                                         "  if(alt==\"%1\"){                                "
                                         "    images[i].src = '%2?t=' + Math.random();      "
                                         "  }                                               "
                                         "}");
    runJavaScript(refreshImageJavascript.arg(imgAlt, imgSrc));
}

void PreviewPage::inlineImages()
{
    runJavaScript("var images = document.getElementsByTagName('img');"
                  "var sources = [];                                 "
                  "for (i=0;i < images.length;i++) {                 "
                  "  sources.push(images[i].src);                    "
                  "}                                                 "
                  "sources;                                          ",
                  [this](const QVariant &var) {
                      auto images         = var.toStringList();
                      m_imageCount        = images.length();
                      m_imageHandledCount = 0;
                      embedImages(images);
                  });
}

void PreviewPage::copy1stImage()
{
    runJavaScript("var images = document.getElementsByTagName('img');"
                  "images[0].src;                                    ",
                  [this](const QVariant &var) {
                      auto image = var.toString();
                      copyImage(image);
                  });
}

void PreviewPage::getImages()
{
    runJavaScript("var images = document.getElementsByTagName('img');"
                  "var sources = [];                                 "
                  "for (i=0;i < images.length;i++) {                 "
                  "  sources.push(images[i].src);                    "
                  "}                                                 "
                  "sources;                                          ",
                  [this](const QVariant &var) {
                      auto res = var.toStringList();
                      emit gotAllImages(res);
                  });
}

void PreviewPage::onEditorScrollMoved(int pos, int max)
{
    const QString scrollJavascript("window.scrollTo(0, document.body.scrollHeight * %1 / %2);");
    runJavaScript(scrollJavascript.arg(pos).arg(max));
}

void PreviewPage::onEmbeded()
{
    m_imageHandledCount++;
    if (m_imageCount == m_imageHandledCount)
    {
        emit allImagesEmbeded();
    }
}

void PreviewPage::onRequestRemoteImageDoneThenCopy()
{
    auto *helper = qobject_cast<NetworkReplyHelper *>(sender());
    Q_ASSERT(helper);
    helper->deleteLater();
    auto   imgData = helper->content();
    QImage img;
    if (!img.loadFromData(imgData))
    {
        qDebug() << "loading from data failed:" << helper->reply()->request().url();
        return;
    }
    auto *clipboard = QGuiApplication::clipboard();
    if (clipboard)
    {
        clipboard->setImage(img);
    }
}

bool PreviewPage::acceptNavigationRequest(const QUrl &url, QWebEnginePage::NavigationType /*type*/, bool /*isMainFrame*/)
{
    // Only allow qrc:/index.html.
    if (url.scheme() == QString("qrc"))
    {
        return true;
    }
    QDesktopServices::openUrl(url);
    return false;
}

void PreviewPage::embedImages(const QStringList &images)
{
    std::map<QString, QString> formatMap = {
        {"svg", "data:image/svg+xml;base64,"},
        {"png", "data:image/png;base64,"},
        {"gif", "data:image/gif;base64,"},
        {"jpeg", "data:image/jpeg;base64,"},
        {"jpg", "data:image/jpg;base64,"},
        {"bmp", "data:image/bmp;base64,"},
        {"ico", "data:image/x-icon;base64,"},
        {"webp", "data:image/webp;base64,"},
    };
    for (const auto &src : images)
    {
        QUrl url(src);
        if (url.isLocalFile())
        {
            QString origSrc = url.toLocalFile();
            auto format = QImageReader::imageFormat(origSrc);
            auto it     = formatMap.find(format.toLower());
            if (formatMap.end() == it)
            {
                emit embeded();
                continue;
            }
            QFile f(origSrc);
            if (!f.open(QIODevice::ReadOnly))
            {
                emit embeded();
                continue;
            }
            auto ba = f.readAll();
            f.close();
            QString header = it->second;
            if (format.compare("png", Qt::CaseInsensitive) != 0 && format.compare("jpg", Qt::CaseInsensitive) != 0)
            {
                // convert to png
                auto       image = QImage::fromData(ba);
                QByteArray baJpeg;
                QBuffer    buffer(&baJpeg);
                if (buffer.open(QIODevice::WriteOnly))
                {
                    if (image.save(&buffer, "PNG"))
                    {
                        ba     = baJpeg;
                        header = formatMap["png"];
                        format = "png";
                    }
                }
            }
            if (format.compare("png", Qt::CaseInsensitive) == 0)
            {
                ba = compressPNG(ba);
            }
            QString newSrc = header + QString(ba.toBase64());
            embedImage(src, newSrc);
            continue;
        }
        if (url.scheme() == "http" || url.scheme() == "https")
        {
            QNetworkRequest req(url);
            req.setAttribute(QNetworkRequest::Http2AllowedAttribute, true);
            req.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
            req.setRawHeader("Accept-Encoding", "gzip, deflate");
            req.setRawHeader("Referer", QString("%1://%2").arg(url.scheme(), url.host()).toUtf8());
            Q_ASSERT(m_nam);
            auto *reply  = m_nam->get(req);
            auto *helper = new NetworkReplyHelper(reply);
            helper->waitForFinished();
            auto ba = helper->content();
            helper->deleteLater();

            QBuffer buf(&ba);
            if (!buf.open(QIODevice::ReadOnly))
            {
                emit embeded();
                continue;
            }
            auto format = QImageReader::imageFormat(&buf);
            auto it     = formatMap.find(format.toLower());
            if (formatMap.end() == it)
            {
                emit embeded();
                continue;
            }

            QString header = it->second;
            if (format.compare("png", Qt::CaseInsensitive) != 0 && format.compare("jpg", Qt::CaseInsensitive) != 0)
            {
                // convert to png
                auto       image = QImage::fromData(ba);
                QByteArray baJpeg;
                QBuffer    buffer(&baJpeg);
                if (buffer.open(QIODevice::WriteOnly))
                {
                    if (image.save(&buffer, "PNG"))
                    {
                        ba     = baJpeg;
                        header = formatMap["png"];
                        format = "png";
                    }
                }
            }
            if (format.compare("png", Qt::CaseInsensitive) == 0)
            {
                ba = compressPNG(ba);
            }
            QString newSrc = header + QString(ba.toBase64());
            embedImage(src, newSrc);
            continue;
        }
        emit embeded();
    }
}

void PreviewPage::embedImage(const QString &from, const QString &to)
{
    runJavaScript(QString("var images = document.getElementsByTagName('img');"
                          "for (i=0;i < images.length;i++) {                 "
                          "  var src = images[i].src;                        "
                          "  if(src==\"%1\"){                                "
                          "    images[i].src = '%2';                         "
                          "    break;                                        "
                          "  }                                               "
                          "}")
                      .arg(from, to),
                  [this](const QVariant &) { emit embeded(); });
}

QByteArray PreviewPage::compressPNG(const QByteArray &ba)
{
    GoSlice input;
    input.data = (void *)ba.data();
    input.len  = ba.length();
    input.cap  = ba.length();
    GoSlice output;
    auto    res = Crush(input, &output);
    if (!res)
    {
        return ba;
    }
    return {static_cast<char *>(output.data), static_cast<int>(output.len)};
}

void PreviewPage::copyImage(const QString &image)
{
    if (image.startsWith("file://"))
    {
        copyLocalImage(image);
    }
    else if (image.startsWith("http://") || image.startsWith("https://"))
    {
        copyRemoteImage(QUrl(image));
    }
    else
    {
        copyFromImageData(image);
    }
}

void PreviewPage::copyRemoteImage(const QUrl &url)
{
    QNetworkRequest req(url);
    req.setAttribute(QNetworkRequest::Http2AllowedAttribute, true);
    req.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
    req.setRawHeader("Accept-Encoding", "gzip, deflate");
    req.setRawHeader("Referer", QString("%1://%2").arg(url.scheme(), url.host()).toUtf8());
    Q_ASSERT(m_nam);
    auto *reply  = m_nam->get(req);
    auto *helper = new NetworkReplyHelper(reply);
    helper->setTimeout(10000);
    connect(helper, &NetworkReplyHelper::done, this, &PreviewPage::onRequestRemoteImageDoneThenCopy);
}

void PreviewPage::copyLocalImage(const QString &filePath)
{
    auto *clipboard = QGuiApplication::clipboard();
    if (clipboard)
    {
        QImage img(filePath.startsWith("file:///") ? filePath.mid(8) : filePath);
        clipboard->setImage(img);
    }
}

void PreviewPage::copyFromImageData(const QString &data)
{
    auto *clipboard = QGuiApplication::clipboard();
    if (clipboard)
    {
        static auto pattern = QRegularExpression("^data:image/([a-zA-Z0-9]+);base64,");
        auto        match   = pattern.match(data);
        if (match.hasMatch())
        {
            auto   matched   = match.captured(0);
            auto   imageData = data.mid(matched.length());
            auto   imgData   = QByteArray::fromBase64(imageData.toUtf8());
            QImage img;
            if (!img.loadFromData(imgData))
            {
                qDebug() << "loading from data failed:" << data;
            }
            clipboard->setImage(img);
        }
    }
}
