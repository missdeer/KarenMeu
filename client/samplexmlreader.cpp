#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStringList>

#include "samplexmlreader.h"

SampleReader::SampleReader(QObject *parent)
    : QObject(parent)
{
}

void SampleReader::scan(const QString &path)
{
    for (auto *sample : std::as_const(m_items))
    {
        delete sample;
    }
    m_items.clear();

    QDir basicDir(path);
    auto samples = basicDir.entryList();
    for (const auto & sampleName : samples)
    {
        auto *sample = new Sample(sampleName);
        sample->scan(path);
        m_items.append(sample);
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
    for (auto *item : std::as_const(m_items))
    {
        delete item;
    }
    m_items.clear();
}

void Sample::scan(const QString& path)
{
    QDir          dir(path + "/" + m_name);
    QFileInfoList sources = dir.entryInfoList(QStringList() << "*.txt", QDir::Files, QDir::Name);
    for (QFileInfo &sourceInfo : sources)
    {
        QString name = sourceInfo.baseName();
        QString data;
        QFile   file(sourceInfo.absoluteFilePath());
        if (file.open(QIODevice::ReadOnly))
        {
            data = QString(file.readAll());
            file.close();
        }
        QString notes;
        QFile   fileNotes(sourceInfo.absolutePath() + "/" + sourceInfo.baseName() + ".notes");
        if (fileNotes.open(QIODevice::ReadOnly))
        {
            notes = QString(fileNotes.readAll());
            fileNotes.close();
        }
        QString icon = sourceInfo.absolutePath() + "/" + sourceInfo.baseName() + ".svg";
        if (!QFile::exists(icon))
        {
            icon = sourceInfo.absolutePath() + "/" + sourceInfo.baseName() + ".png";
        }
        auto *item = new SampleItem(name, data, notes, icon);

        m_items.append(item);
    }
}
