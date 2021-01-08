#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <QtCore>

#include "templatemanager.h"

bool TemplateManager::load()
{
    bool res         = true;
    auto templateDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/templates";
    QDir dir(templateDir);
    if (!dir.exists())
        dir.mkpath(templateDir);
    auto entries = dir.entryInfoList(QStringList() << "*.xml", QDir::Files);
    for (const auto &entry : entries)
    {
        TemplatePtr t(new Template);
        t->setPath(entry.absoluteFilePath());
        auto loaded = t->load();
        if (loaded)
            m_templates.append(t);
        res &= loaded;
    }
    return res;
}

bool TemplateManager::save()
{
    bool res = true;
    for (auto t : qAsConst(m_templates))
        res &= t->save();
    return res;
}

TemplatePtr TemplateManager::add(const QString &name, const QString &fileNameRule, const QString &content)
{
    auto        templateDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/templates";
    TemplatePtr t(new Template);
    t->setPath(QString("%1/%2.xml").arg(templateDir, name));
    t->setNameTemplate(fileNameRule);
    t->setContentTemplate(content);
    m_templates.append(t);
    return t;
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
