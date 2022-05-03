#ifndef ASSISTANTXMLREADER_H
#define ASSISTANTXMLREADER_H

#include <QObject>

class Sample;
class SampleReader;

class SampleItem : public QObject
{
    Q_OBJECT
public:
    explicit SampleItem(const QString& name, const QString& data, const QString& notes, const QString &icon, QObject *parent = 0);

    const QString& name() const { return m_name; }
    const QString& data() const { return m_data; }
    const QString& icon() const { return m_icon; }
    const QString& notes() const { return m_notes; }

private:
    const QString m_name;
    const QString m_data;
    const QString m_notes;
    const QString m_icon;
};

class Sample : public QObject
{
    Q_OBJECT
public:
    explicit Sample(const QString& name, QObject *parent = 0);
    ~Sample();
    void scan(const QString& path);
    const QString& name() const { return m_name; }
    int size() const { return m_items.size(); }

    QList<SampleItem*>::iterator begin() { return m_items.begin(); }
    QList<SampleItem*>::iterator end() { return m_items.end(); }
    QList<SampleItem*>::const_iterator cbegin() { return m_items.cbegin(); }
    QList<SampleItem*>::const_iterator cend() { return m_items.cend(); }

    const SampleItem* item(int index) const { return m_items.at(index); }

private:
    QString m_name;
    QList<SampleItem*> m_items;
};

class SampleReader: public QObject
{
    Q_OBJECT
public:
    explicit SampleReader(QObject *parent = 0);

    void scan(const QString& path);

    int size() const { return m_items.size(); }
    const Sample* sample(int index) { return m_items.at(index); }

    QList<Sample*>::iterator begin() { return m_items.begin(); }
    QList<Sample*>::iterator end() { return m_items.end(); }
    QList<Sample*>::const_iterator cbegin() { return m_items.cbegin(); }
    QList<Sample*>::const_iterator cend() { return m_items.cend(); }

private:
    QList<Sample*> m_items;
};

#endif // ASSISTANTXMLREADER_H
