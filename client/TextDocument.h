#ifndef MARKUPDOCUMENT_H
#define MARKUPDOCUMENT_H

#include <QDateTime>
#include <QString>
#include <QTextBlock>
#include <QTextDocument>

/**
 * Text document that maintains timestamp, read-only state, and new vs.
 * saved status.
 */
class TextDocument : public QTextDocument
{
    Q_OBJECT

public:
    /**
     * Constructor.
     */
    TextDocument(QObject *parent = 0);

    /**
     * Constructor.
     */
    TextDocument(const QString &text, QObject *parent = 0);

    /**
     * Destructor.
     */
    virtual ~TextDocument();

    /**
     * Gets display name for the document for displaying in a text
     * editor's containing window or tab.
     */
    QString getDisplayName() const;

    /**
     * Gets the document file path.
     */
    QString getFilePath() const;

    /**
     * Sets the document file path.
     */
    void setFilePath(const QString &path);

    /**
     * Returns true if the document is new with no file path.
     */
    bool isNew() const;

    /**
     * Returns true if the document has read only permissions.
     */
    bool isReadOnly() const;

    /**
     * Sets whether the document has read only permissions.
     */
    void setReadOnly(bool readOnly);

    /**
     * Gets the timestamp of the document, which is useful when comparing
     * the last modified time of the file represented on disk.
     */
    QDateTime getTimestamp() const;

    /**
     * Sets a timestamp to the document, which is useful when comparing
     * the last modified time of the file represented on disk.
     */
    void setTimestamp(const QDateTime &timestamp);

    /**
     * For internal use only with TextBlockData class.  Emits signals
     * to notify listeners that the given text block is about to be
     * removed from the document.
     */
    void notifyTextBlockRemoved(const QTextBlock &block);

signals:
    /**
     * Emitted when the file path changes.
     */
    void filePathChanged();

    /**
     * Emitted when the QTextBlock at the given position in the document
     * is removed.
     */
    void textBlockRemoved(int position);

    /**
     * Emitted when a QTextBlock is removed from the document.  Parameter
     * is the QTextBlock that is being removed.
     */
    void textBlockRemoved(const QTextBlock &block);

private:
    QString   displayName;
    QString   filePath;
    bool      readOnlyFlag {false};
    QDateTime timestamp;

    /*
     * Initializes the class for an untitled document.
     */
    void initializeUntitledDocument();
};

#endif // MARKUPDOCUMENT_H
