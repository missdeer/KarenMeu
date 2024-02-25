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
    {
        dir.mkpath(templateDir);
    }
    auto entries = dir.entryInfoList(QStringList() << "*.xml", QDir::Files);
    for (const auto &entry : entries)
    {
        TemplatePtr tmpl(new Template);
        tmpl->setPath(entry.absoluteFilePath());
        auto loaded = tmpl->load();
        if (loaded)
        {
            m_templates.append(tmpl);
        }
        res &= loaded;
    }
    return res;
}

bool TemplateManager::save()
{
    bool res = true;
    for (const auto &tmpl : std::as_const(m_templates))
    {
        res &= tmpl->save();
    }
    return res;
}

TemplatePtr TemplateManager::add(const QString &name, const QString &fileNameRule, const QString &content)
{
    auto        templateDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/templates";
    TemplatePtr tmpl(new Template);
    tmpl->setPath(QString("%1/%2.xml").arg(templateDir, name));
    tmpl->setNameTemplate(fileNameRule);
    tmpl->setContentTemplate(content);
    m_templates.append(tmpl);
    return tmpl;
}

void TemplateManager::remove(const QString &name)
{
    auto iter = std::find_if(m_templates.begin(), m_templates.end(), [&name](auto tmpl) { return QFileInfo(tmpl->path()).baseName() == name; });
    if (m_templates.end() != iter)
    {
        m_templates.erase(iter);
    }
}

TemplatePtr TemplateManager::find(const QString &name)
{
    auto iter = std::find_if(m_templates.begin(), m_templates.end(), [&name](auto tmpl) { return QFileInfo(tmpl->path()).baseName() == name; });
    if (m_templates.end() != iter)
    {
        return *iter;
    }

    return nullptr;
}

Templates &TemplateManager::templates()
{
    return m_templates;
}
