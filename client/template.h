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

    const QString &nameRule() const;
    void           setNameRule(const QString &nameRule);
    QString        ruleAppliedName();

    const QString &contentTemplate() const;
    void           setContentTemplate(const QString &content);
    QString        content();

private:
    QString m_path;
    QString m_nameRule;
    QString m_content;
};

using TemplatePtr = QSharedPointer<Template>;

#endif // TEMPLATE_H
