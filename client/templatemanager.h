#ifndef TEMPLATEMANAGER_H
#define TEMPLATEMANAGER_H

#include <template.h>

using Templates = QList<TemplatePtr>;

class TemplateManager
{
public:
    bool load();
    bool save();

    void        add(const QString &name, const QString &fileNameRule, const QString &content);
    void        remove(const QString &name);
    TemplatePtr find(const QString &name);

    Templates &templates();

private:
    Templates m_templates;
};

#endif // TEMPLATEMANAGER_H
