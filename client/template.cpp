#include <QDomDocument>
#include <QDomElement>
#include <QDomProcessingInstruction>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QtCore>

#include "template.h"

bool Template::load()
{
    QFile f(m_path);
    if (!f.open(QIODevice::ReadOnly))
        return false;

    QDomDocument doc;
    if (!doc.setContent(&f))
    {
        f.close();
        return false;
    }
    f.close();

    auto docEle = doc.documentElement();
    if (docEle.isNull())
    {
        return false;
    }
    auto fnEle = docEle.firstChildElement("fileName");
    if (!fnEle.isNull())
    {
        m_nameRule = fnEle.text();
    }

    QDomElement contentElem = docEle.firstChildElement("content");
    if (contentElem.isNull())
    {
        return false;
    }
    m_content = contentElem.text();

    return true;
}

bool Template::save()
{
    QDomDocument doc;
    QFile        f(m_path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return false;

    auto intruction = doc.createProcessingInstruction("xml", "version='1.0' encoding='UTF-8'");
    doc.appendChild(intruction);

    auto docElem = doc.createElement("template");
    doc.appendChild(docElem);

    auto fnEle = doc.createElement("fileName");
    docElem.appendChild(fnEle);

    auto text = doc.createTextNode(m_nameRule);
    fnEle.appendChild(text);

    auto contentEle = doc.createElement("content");
    docElem.appendChild(contentEle);

    auto contentSection = doc.createCDATASection(m_content);
    contentEle.appendChild(contentSection);

    QTextStream out(&f);
    doc.save(out, 4);
    f.close();
    return true;
}

const QString &Template::path() const
{
    return m_path;
}

void Template::setPath(const QString &path)
{
    m_path = path;
}

QString Template::templateName()
{
    return QFileInfo(m_path).baseName();
}

const QString &Template::nameRule() const
{
    return m_nameRule;
}

void Template::setNameRule(const QString &nameRule)
{
    m_nameRule = nameRule;
}

QString Template::ruleAppliedName()
{
    return m_nameRule;
}

const QString &Template::contentTemplate() const
{
    return m_content;
}

void Template::setContentTemplate(const QString &content)
{
    m_content = content;
}

QString Template::content()
{
    return m_content;
}
