#include <QDomDocument>
#include <QDomElement>
#include <QFile>

#include "template.h"

bool Template::load()
{
    QDomDocument doc;
    QFile        f(m_path);
    if (!f.open(QIODevice::ReadOnly))
        return false;

    auto c = f.readAll();
    f.close();

    if (!doc.setContent(c))
        return false;

    auto docEle = doc.documentElement();
    if (docEle.isNull())
        return false;
    auto fnEle = docEle.firstChildElement("fileName");
    if (!fnEle.isNull())
        m_nameRule = fnEle.text();
    auto contentEle = docEle.firstChildElement("content");
    if (contentEle.isNull())
        return false;
    m_content = contentEle.text();

    return true;
}

bool Template::save()
{
    QDomDocument doc("template");
    QFile        f(m_path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return false;

    auto docElem = doc.documentElement();

    auto fnEle = doc.createTextNode("fileName");
    fnEle.setNodeValue(m_nameRule);
    docElem.appendChild(fnEle);

    auto contentEle = doc.createCDATASection("content");
    contentEle.setData(m_content);
    docElem.appendChild(contentEle);

    f.write(doc.toByteArray(2));

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

const QString &Template::nameRule() const
{
    return m_nameRule;
}

void Template::setNameRule(const QString &nameRule)
{
    m_nameRule = nameRule;
}

QString Template::name()
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
