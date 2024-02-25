#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QUrl>
#include <QUrlQuery>

#include "youdaodict.h"

struct YoudaoDictKey
{
    QString keyFrom;
    QString key;
};

YoudaoDict::YoudaoDict(QNetworkAccessManager &nam, QObject *parent) : QObject(parent), m_nam(nam) {}

void YoudaoDict::query(const QString &keyword)
{
    static int                    keyIndex = 0;
    static QVector<YoudaoDictKey> keys     = {{"f2ec-org", "1787962561"}, {"go-ydict", "252639882"}, {"Youdao-dict-v21", "1945325576"}};

    m_content.clear();
    // http://fanyi.youdao.com/openapi.do?keyfrom=f2ec-org&key=1787962561&type=data&doctype=json&version=1.1&q=
    QUrl      url("http://fanyi.youdao.com/openapi.do");
    QUrlQuery query;

    query.addQueryItem("keyfrom", keys.at(keyIndex).keyFrom);
    query.addQueryItem("key", keys.at(keyIndex).key);
    query.addQueryItem("type", "data");
    query.addQueryItem("doctype", "json");
    query.addQueryItem("version", "1.1");
    query.addQueryItem("q", keyword.toUtf8());
    if (++keyIndex >= keys.size())
    {
        keyIndex = 0;
    }
    url.setQuery(query.query());
    QNetworkRequest req(url);
    req.setAttribute(QNetworkRequest::Http2AllowedAttribute, true);
    req.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);

    QNetworkReply *reply = m_nam.get(req);

    connect(reply, SIGNAL(finished()), this, SLOT(onFinished()));
    connect(reply, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
}

void YoudaoDict::onFinished()
{
    auto *reply = qobject_cast<QNetworkReply *>(sender());
    Q_ASSERT(reply);
    reply->deleteLater();
    m_content.append(reply->readAll());

    QJsonDocument doc = QJsonDocument::fromJson(m_content);
    if (!doc.isObject())
    {
        emit result("");
        return;
    }

    auto obj       = doc.object();
    auto errorCode = obj["errorCode"].toInt();
    if (errorCode != 0)
    {
        emit result("");
        return;
    }

    QString     output;
    QTextStream textStream(&output);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    textStream.setCodec("UTF-8");
#else
    textStream.setEncoding(QStringConverter::Utf8);
#endif

    auto        query        = obj["query"].toString();
    auto        translations = obj["translation"].toArray();
    QStringList trans;
    for (auto translation : translations)
    {
        trans.append(translation.toString());
    }
    auto basic = obj["basic"].toObject();
    if (basic["phonetic"].isString())
    {
        trans.append("[" + basic["phonetic"].toString() + "]");
    }
    if (basic["us-phonetic"].isString())
    {
        trans.append("US [" + basic["us-phonetic"].toString() + "]");
    }
    if (basic["uk-phonetic"].isString())
    {
        trans.append("UK [" + basic["uk-phonetic"].toString() + "]");
    }

    textStream << QString("<h4>%1 %2</h4>").arg(QObject::tr("[Translation]"), query) << QString("<p>%1</p><br>").arg(trans.join("; "));

    auto explains = basic["explains"].toArray();
    for (auto explain : explains)
    {
        textStream << QString("<h4>%1 %2</h4>").arg(QObject::tr("[Explain]"), query) << QString("<p>%1</p><br>").arg(explain.toString());
    }

    auto webs = obj["web"].toArray();
    for (auto web : webs)
    {
        auto        webObj = web.toObject();
        QString     key    = webObj["key"].toString();
        auto        values = webObj["value"].toArray();
        QStringList value;
        for (auto val : values)
        {
            value.append(val.toString());
        }
        textStream << QString("<h4>%1 %2</h4>").arg(QObject::tr("[Web]"), key) << QString("<p>%1</p><br>").arg(value.join("; "));
    }
    emit result(output);
}

void YoudaoDict::onError(QNetworkReply::NetworkError err)
{
    auto *reply = qobject_cast<QNetworkReply *>(sender());
    Q_ASSERT(reply);
    qDebug() << err << reply->errorString();
}

void YoudaoDict::onReadyRead()
{
    auto *reply = qobject_cast<QNetworkReply *>(sender());
    Q_ASSERT(reply);
    m_content.append(reply->readAll());
}
