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
    static int           keyIndex  = 0;
    static YoudaoDictKey keys[]    = {{"f2ec-org", "1787962561"}, {"go-ydict", "252639882"}, {"Youdao-dict-v21", "1945325576"}};
    static int           keysCount = sizeof(keys) / sizeof(keys[0]);
    m_content.clear();
    // http://fanyi.youdao.com/openapi.do?keyfrom=f2ec-org&key=1787962561&type=data&doctype=json&version=1.1&q=
    QUrl url("http://fanyi.youdao.com/openapi.do");
    QUrlQuery query;

    query.addQueryItem("keyfrom", keys[keyIndex].keyFrom);
    query.addQueryItem("key", keys[keyIndex].key);
    query.addQueryItem("type", "data");
    query.addQueryItem("doctype", "json");
    query.addQueryItem("version", "1.1");
    query.addQueryItem("q", keyword.toUtf8());
    if (++keyIndex >= keysCount)
        keyIndex = 0;
    url.setQuery(query.query());
    QNetworkRequest req(url);
    req.setAttribute(QNetworkRequest::Http2AllowedAttribute, true);

    QNetworkReply* reply = m_nam.get(req);

    connect(reply, SIGNAL(finished()), this, SLOT(onFinished()));
    connect(reply, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(onError(QNetworkReply::NetworkError)));
}

void YoudaoDict::onFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT(reply);
    reply->deleteLater();
    m_content.append( reply->readAll());

    QJsonDocument doc = QJsonDocument::fromJson(m_content);
    if (!doc.isObject())
    {
        emit result("");
        return;
    }

    auto o = doc.object();
    auto errorCode = o["errorCode"].toInt();
    if (errorCode != 0)
    {
        emit result("");
        return;
    }

    QString     output;
    QTextStream ts(&output);
    ts.setCodec("UTF-8");

    auto query = o["query"].toString();
    auto translation = o["translation"].toArray();
    QStringList trans;
    for (auto t : translation)
    {
        trans.append(t.toString());
    }
    auto basic = o["basic"].toObject();
    if (basic["phonetic"].isString())
        trans.append("[" + basic["phonetic"].toString() +"]");
    if (basic["us-phonetic"].isString())
        trans.append("US [" + basic["us-phonetic"].toString() +"]");
    if (basic["uk-phonetic"].isString())
        trans.append("UK [" + basic["uk-phonetic"].toString() +"]");

    ts << QString("<h4>%1 %2</h4>").arg(QObject::tr("[Translation]"), query) << QString("<p>%1</p><br>").arg(trans.join("; "));

    auto explains = basic["explains"].toArray();
    for (auto e : explains)
    {
        ts << QString("<h4>%1 %2</h4>").arg(QObject::tr("[Explain]"), query) << QString("<p>%1</p><br>").arg(e.toString());
    }

    auto web = o["web"].toArray();
    for (auto w : web)
    {
        auto i = w.toObject();
        QString key = i["key"].toString();
        auto values = i["value"].toArray();
        QStringList value;
        for (auto v : values)
        {
            value.append(v.toString());
        }
        ts << QString("<h4>%1 %2</h4>").arg(QObject::tr("[Web]"), key) << QString("<p>%1</p><br>").arg(value.join("; "));
    }
    emit result(output);
}

void YoudaoDict::onError(QNetworkReply::NetworkError e)
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT(reply);
    qDebug() << e << reply->errorString();
}

void YoudaoDict::onReadyRead()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT(reply);
    m_content.append( reply->readAll());
}
