#ifndef FILECACHE_H
#define FILECACHE_H

#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QMap>
#include <QObject>
#include <QSet>
#include <QString>
#include <functional>

inline QString cachePathFromPathAndKey(const QString &path, const QString &key)
{
    return QFileInfo(QDir(path), key).absoluteFilePath();
}

struct FileCacheError
{
};

class AbstractFileCacheItem : public QObject
{
    Q_OBJECT
public:
    AbstractFileCacheItem(const QString &path, const QString &key, int cost, const QDateTime &date_time, QObject *parent = 0);
    virtual ~AbstractFileCacheItem() {};

    const QString& path() const { return m_path; }
    const QString& key() const { return m_key; }
    int cost() const { return m_cost; }
    const QDateTime& dateTime() const { return m_dateTime; }

    void removeFileFromDisk() const { removeFileFromDisk(m_path); }

protected:
    virtual void removeFileFromDisk(const QString& path) const = 0;

private:
    QString m_path;
    QString m_key;
    int m_cost;
    QDateTime m_dateTime;
};

class FileCacheItem : public AbstractFileCacheItem
{
    Q_OBJECT
public:
    FileCacheItem(const QString &path, const QString &key, int cost, const QDateTime &date_time, QObject *parent = 0);
    virtual ~FileCacheItem();

private:
    virtual void removeFileFromDisk(const QString &some_path) const override;

    mutable bool m_removed;
};

inline AbstractFileCacheItem *defaultItemGenerator(const QString &path, const QString &key, int cost, const QDateTime &date_time, QObject *parent)
{
    return new FileCacheItem(path, key, cost, date_time, parent);
}

class FileCache : public QObject
{
    Q_OBJECT
public:
    using ItemGenerator = std::function<AbstractFileCacheItem *(const QString &,   // path
                                                                const QString &,   // key
                                                                int,               // cost
                                                                const QDateTime &, // access time
                                                                QObject *          // parent
                                                                )>;

    explicit FileCache(int maxCost = 0, QObject *parent = 0);
    virtual ~FileCache();

    int maxCost() const { return m_maxCost; }
    void setMaxCost(int max_cost);

    bool hasItem(const QString& key) const;
    void addItem(AbstractFileCacheItem* item);
    void addItem(const QByteArray &data, const QString &key, ItemGenerator item_generator = defaultItemGenerator);

    int totalCost() const { return m_totalCost; }

    int size() const { return m_items.size(); }
    QList<QString> keys() const { return m_items.keys(); }
    const AbstractFileCacheItem* item(const QString& key) const { return m_items.value(key); }

    void clear();
    void clearFromDisk();

    bool           setPath(const QString &path, ItemGenerator item_generator = defaultItemGenerator);
    const QString& path() const { return m_path; }

private:
    bool updateFromDisk(const QString &path, ItemGenerator item_generator = defaultItemGenerator);

    QString m_path;
    int m_maxCost;
    int m_totalCost;
    QMap<QString, AbstractFileCacheItem*> m_items;
    QList<QString> m_indexByDate;
};

#endif // FILECACHE_H
