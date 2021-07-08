#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <QStringList>
QT_FORWARD_DECLARE_CLASS(QTimer);
QT_FORWARD_DECLARE_CLASS(QWebEnginePage);

class ITranslator
{
public:
    virtual ~ITranslator()                                                                                      = default;
    [[nodiscard]] virtual QStringList fromLanguages()                                                           = 0;
    [[nodiscard]] virtual QStringList toLanguages()                                                             = 0;
    [[nodiscard]] virtual QString     defaultFrom() const                                                       = 0;
    [[nodiscard]] virtual QString     defaultTo() const                                                         = 0;
    virtual void                      from(const QString &from)                                                 = 0;
    virtual void                      to(const QString &to)                                                     = 0;
    [[nodiscard]] virtual QString     from() const                                                              = 0;
    [[nodiscard]] virtual QString     to() const                                                                = 0;
    virtual void                      request(QWebEnginePage *page, QTimer *timer, const QString &originalText) = 0;
    [[nodiscard]] virtual QString     landingPageUrl() const                                                    = 0;
    [[nodiscard]] virtual QString     resultJavaScript() const                                                  = 0;
    [[nodiscard]] virtual bool        mustChinese() const
    {
        return false;
    }
};

inline int intervalStartupStep = 1000;

namespace TranslatorUtil
{
    [[nodiscard]] QStringList StringList(const QString &path, const QString &key);
    [[nodiscard]] QString     String(const QString &path, const QString &key);
} // namespace TranslatorUtil

#endif
