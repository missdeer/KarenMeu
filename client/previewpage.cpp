#include <QDesktopServices>
#include <QtCore>

#include "previewpage.h"

PreviewPage::PreviewPage(QObject *parent) : QWebEnginePage(parent)
{
    connect(this, &PreviewPage::embeded, this, &PreviewPage::onEmbeded);
}

void PreviewPage::refreshImage(const QString &imgAlt, const QString &imgSrc)
{
    const QString refreshImagesJavascript("var images = document.getElementsByTagName('img');"
                                          "for (i=0;i < images.length;i++) {                 "
                                          "	 var alt = images[i].alt;                        "
                                          "	 var src = images[i].src;                        "
                                          "	 queryPos = src.indexOf('?');                    "
                                          "	 if(queryPos != -1) {                            "
                                          "	   src = src.substring(0, queryPos);             "
                                          "	 }                                               "
                                          "  if(alt==\"%1\"){                                "
                                          "	   images[i].src = '%2?t=' + Math.random();      "
                                          "    break;                                        "
                                          "	 }                                               "
                                          "}");
    runJavaScript(refreshImagesJavascript.arg(imgAlt, imgSrc));
}

void PreviewPage::inlineImages()
{
    runJavaScript("var images = document.getElementsByTagName('img');"
                  "var sources = [];"
                  "for (i=0;i < images.length;i++) {                 "
                  "  sources.push(images[i].src);"
                  "}"
                  "sources;",
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
    };
    for (const auto &src : images)
    {
        if (!src.startsWith("file://"))
        {
            emit embeded();
            continue;
        }
        QString origSrc = QUrl(src).toLocalFile();
        if (int index = src.indexOf("?t="); index > 0)
        {
            origSrc = src.left(index);
        }
        QFile f(origSrc);
        if (!f.open(QIODevice::ReadOnly))
        {
            emit embeded();
            continue;
        }
        auto ba = f.readAll();
        f.close();
        QString newSrc;
        auto    it = std::find_if(formatMap.begin(), formatMap.end(), [&src](const auto &p) { return src.endsWith(p.first, Qt::CaseInsensitive); });
        if (formatMap.end() == it)
        {
            emit embeded();
            continue;
        }
        newSrc = it->second + QString(ba.toBase64());
        embedImage(src, newSrc);
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
