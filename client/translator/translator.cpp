#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>

#include "translator.hpp"

namespace TranslatorUtil
{
    QStringList StringList(const QString &path, const QString &key)
    {
        QStringList res;
        QFile       f(path);
        if (f.open(QIODevice::ReadOnly))
        {
            auto ba    = f.readAll();
            auto doc   = QJsonDocument::fromJson(ba);
            auto value = doc[key];
            if (value.isArray())
            {
                auto arrvalue = value.toArray();
                for (const auto &ele : arrvalue)
                {
                    if (ele.isString())
                        res.append(ele.toString());
                }
            }
        }
        return res;
    }

    QString String(const QString &path, const QString &key)
    {
        QString res;
        QFile   f(path);
        if (f.open(QIODevice::ReadOnly))
        {
            auto ba    = f.readAll();
            auto doc   = QJsonDocument::fromJson(ba);
            auto value = doc[key];
            if (value.isString())
            {
                return value.toString();
            }
        }
        return res;
    }

} // namespace TranslatorUtil
