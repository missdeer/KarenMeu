#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStringList>

#include "samplexmlreader.h"

namespace
{
    static const QString ROOT_TAG   = "samples";
    static const QString SAMPLE_TAG = "sample";
} // namespace

SampleReader::SampleReader(QObject *parent)
    : QObject(parent)
{
}

void SampleReader::scan(const QString &path)
{
    foreach(Sample* sample, m_items) {
        delete sample;
    }
    m_items.clear();

    QDir basicDir(path);
    auto samples = basicDir.entryList();
    for (const auto & sampleName : samples)
    {
        Sample* s = new Sample(sampleName);
        s->scan(path);
        m_items.append(s);
    }
}


SampleItem::SampleItem(const QString &name, const QString &data, const QString& notes, const QString& icon, QObject *parent)
    : QObject(parent)
    , m_name(name)
    , m_data(data)
    , m_notes(notes)
    , m_icon(icon)
{
}


Sample::Sample(const QString &name, QObject *parent)
    : QObject(parent)
    , m_name(name)
{
}

Sample::~Sample()
{
    foreach(SampleItem* item, m_items) {
        delete item;
    }
    m_items.clear();
}

void Sample::scan(const QString& path)
{
    QDir d(path + "/" + m_name);
    QFileInfoList sources = d.entryInfoList(QStringList() << "*.txt", QDir::Files, QDir::Name);
    for (QFileInfo& si : sources)
    {
        QString name = si.baseName();
        QString data;
        QFile fs(si.absoluteFilePath());
        if (fs.open(QIODevice::ReadOnly))
        {
             data = QString(fs.readAll());
             fs.close();
        }
        QString notes;
        QFile fn(si.absolutePath() + "/" + si.baseName() + ".notes");
        if (fn.open(QIODevice::ReadOnly))
        {
             notes = QString(fn.readAll());
             fn.close();
        }
        QString icon = si.absolutePath() + "/" + si.baseName() + ".svg";
        if (!QFile::exists(icon))
            icon = si.absolutePath() + "/" + si.baseName() + ".png";
        SampleItem* item = new SampleItem(name, data, notes, icon);

        m_items.append(item);
    }
}
