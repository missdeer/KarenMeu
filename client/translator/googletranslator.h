#ifndef GOOGLETRANSLATOR_H
#define GOOGLETRANSLATOR_H

#include "translator.hpp"

class GoogleTranslator : public ITranslator
{
    QString m_from;
    QString m_to;

public:
    QStringList fromLanguages() override;
    QStringList toLanguages() override;
    QString     defaultFrom() override;
    QString     defaultTo() override;
    void        from(const QString &from) override;
    void        to(const QString &to) override;
    void        request(QWebEnginePage *page, QTimer *timer, const QString &originalText) override;
    QString     landingPageUrl() override;
    QString     resultJavaScript() override;
};
#endif
