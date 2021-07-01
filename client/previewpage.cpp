#include <QBuffer>
#include <QDesktopServices>
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
                  [this](const QVariant &v) {
                      auto images         = v.toStringList();
                      m_imageCount        = images.length();
                      m_imageHandledCount = 0;
                      embedImages(images);
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
        emit allImagesEmbeded();
}

bool PreviewPage::acceptNavigationRequest(const QUrl &url, QWebEnginePage::NavigationType /*type*/, bool /*isMainFrame*/)
{
    // Only allow qrc:/index.html.
    if (url.scheme() == QString("qrc"))
        return true;
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
        QUrl u(src);
        if (u.isLocalFile())
        {
            QString origSrc = u.toLocalFile();
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
        if (u.scheme() == "http" || u.scheme() == "https")
        {
            QNetworkRequest req(u);
            req.setAttribute(QNetworkRequest::Http2AllowedAttribute, true);
            req.setRawHeader("Accept-Encoding", "gzip, deflate");
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
        return ba;
    return QByteArray((char *)output.data, output.len);
}
