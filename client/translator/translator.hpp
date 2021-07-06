#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <QStringList>
QT_FORWARD_DECLARE_CLASS(QTimer);
QT_FORWARD_DECLARE_CLASS(QWebEnginePage);

class ITranslator
{
public:
    virtual ~ITranslator()                                                                        = default;
    virtual QStringList fromLanguages()                                                           = 0;
    virtual QStringList toLanguages()                                                             = 0;
    virtual QString     defaultFrom()                                                             = 0;
    virtual QString     defaultTo()                                                               = 0;
    virtual void        from(const QString &from)                                                 = 0;
    virtual void        to(const QString &to)                                                     = 0;
    virtual void        request(QWebEnginePage *page, QTimer *timer, const QString &originalText) = 0;
    virtual QString     landingPageUrl()                                                          = 0;
    virtual QString     resultJavaScript()                                                        = 0;
};

inline int intervalStartupStep = 1000;

#endif
