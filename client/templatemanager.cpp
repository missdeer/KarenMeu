#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>

#include "templatemanager.h"

bool TemplateManager::load()
{
    bool res         = true;
    auto templateDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/templates";
    QDir dir(templateDir);
    auto entries = dir.entryInfoList(QStringList() << "*.xml", QDir::Files);
    for (const auto &entry : entries)
    {
        TemplatePtr t(new Template);
        t->setPath(entry.absoluteFilePath());
        res &= t->save();
    }
    return res;
}

bool TemplateManager::save()
{
    bool res = true;
    for (auto t : m_templates)
        res &= t->save();
    return res;
}

void TemplateManager::add(const QString &name, const QString &fileNameRule, const QString &content)
{
    auto        templateDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/templates";
    TemplatePtr t(new Template);
    t->setPath(QString("%1/templates/%2.xml").arg(templateDir, name));
    t->setNameRule(fileNameRule);
    t->setContentTemplate(content);
    m_templates.append(t);
}

void TemplateManager::remove(const QString &name)
{
    auto it = std::find_if(m_templates.begin(), m_templates.end(), [&name](auto t) { return QFileInfo(t->path()).baseName() == name; });
    if (m_templates.end() != it)
        m_templates.erase(it);
}

TemplatePtr TemplateManager::find(const QString &name)
{
    auto it = std::find_if(m_templates.begin(), m_templates.end(), [&name](auto t) { return QFileInfo(t->path()).baseName() == name; });
    if (m_templates.end() != it)
        return *it;

    return nullptr;
}

Templates &TemplateManager::templates()
{
    return m_templates;
}
