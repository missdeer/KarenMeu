#include <QDateTime>
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
        m_nameTemplate = fnEle.text();
    }

    QDomElement contentElem = docEle.firstChildElement("content");
    if (contentElem.isNull())
    {
        return false;
    }
    m_contentTemplate = contentElem.text();

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

    auto text = doc.createTextNode(m_nameTemplate);
    fnEle.appendChild(text);

    auto contentEle = doc.createElement("content");
    docElem.appendChild(contentEle);

    auto contentSection = doc.createCDATASection(m_contentTemplate);
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

const QString &Template::nameTemplate() const
{
    return m_nameTemplate;
}

void Template::setNameTemplate(const QString &nameRule)
{
    m_nameTemplate = nameRule;
}

QString Template::templateExecutedName(const QString &title)
{
    return executeTemplate(m_nameTemplate, title);
}

const QString &Template::contentTemplate() const
{
    return m_contentTemplate;
}

void Template::setContentTemplate(const QString &content)
{
    m_contentTemplate = content;
}

QString Template::templateExecutedContent(const QString &title)
{
    return executeTemplate(m_contentTemplate, title);
}

bool Template::needTitle() const
{
    return m_nameTemplate.contains("%title%") || m_contentTemplate.contains("%title%");
}

QString Template::executeTemplate(const QString &t, const QString &title)
{
    auto                       now       = QDateTime::currentDateTime();
    std::map<QString, QString> templates = {
        {"%title%", title},
        {"%h%", QString("%1").arg(now.time().hour())},
        {"%hh%", QString("%1").arg(now.time().hour())},
        {"%H%", QString("%1").arg(now.time().hour())},
        {"%HH%", QString("%1").arg(now.time().hour())},
        {"%m%", QString("%1").arg(now.time().minute())},
        {"%mm%", QString("%1").arg(now.time().minute())},
        {"%AP%", QString("%1").arg(now.time().hour() >= 12 ? "PM" : "AM")},
        {"%ap%", QString("%1").arg(now.time().hour() >= 12 ? "pm" : "am")},
        {"%d%", QString("%1").arg(now.date().day())},
        {"%dd%", QString("%1").arg(now.date().day())},
        {"%ddd%", QString("%1").arg(now.date().dayOfWeek())},
        {"%dddd%", QString("%1").arg(now.date().dayOfWeek())},
        {"%M%", QString("%1").arg(now.date().month())},
        {"%MM%", QString("%1").arg(now.date().month())},
        {"%MMM%", QString("%1").arg(now.date().month())},
        {"%MMMM%", QString("%1").arg(now.date().month())},
        {"%yy%", QString("%1").arg(now.date().year() % 100)},
        {"%yyyy%", QString("%1").arg(now.date().year())},
    };
    QString res = t;
    for (const auto &[from, to] : templates)
    {
        res = res.replace(from, to);
    }
    return res;
}
