#ifndef SOGOUTRANSLATOR_H
#define SOGOUTRANSLATOR_H

#include "translator.hpp"

class SogouTranslator : public ITranslator
{
    QString m_from;
    QString m_to;

public:
    QStringList fromLanguages() override;
    QStringList toLanguages() override;
    QString     defaultFrom() const override;
    QString     defaultTo() const override;
    void        from(const QString &from) override;
    void        to(const QString &to) override;
    QString     from() const override;
    QString     to() const override;
    void        request(QWebEnginePage *page, QTimer *timer, const QString &originalText) override;
    QString     landingPageUrl() const override;
    QString     resultJavaScript() const override;
};
#endif
