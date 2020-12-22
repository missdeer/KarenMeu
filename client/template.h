#ifndef TEMPLATE_H
#define TEMPLATE_H

#include <QSharedPointer>

class Template
{
public:
    bool load();
    bool save();

    const QString &path() const;
    void           setPath(const QString &path);
    QString        templateName();

    const QString &nameTemplate() const;
    void           setNameTemplate(const QString &nameRule);
    QString        templateExecutedName(const QString &title);

    const QString &contentTemplate() const;
    void           setContentTemplate(const QString &content);
    QString        templateExecutedContent(const QString &title);

    bool needTitle() const;

private:
    QString m_path;
    QString m_nameTemplate;
    QString m_contentTemplate;
    QString executeTemplate(const QString &t, const QString &title);
};

using TemplatePtr = QSharedPointer<Template>;

#endif // TEMPLATE_H
