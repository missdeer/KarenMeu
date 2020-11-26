#include <QFileInfo>
#include <QPlainTextDocumentLayout>
#include <QString>
#include <QTextDocument>

#include "TextDocument.h"

TextDocument::TextDocument(QObject* parent)
    : QTextDocument(parent)
{
    initializeUntitledDocument();
}

TextDocument::TextDocument(const QString& text, QObject* parent)
    : QTextDocument(text, parent)
{
    initializeUntitledDocument();
}

TextDocument::~TextDocument()
{
    QPlainTextDocumentLayout* documentLayout =
        new QPlainTextDocumentLayout(this);
    this->setDocumentLayout(documentLayout);

    filePath = QString();
    readOnlyFlag = false;
    displayName = tr("untitled");
    timestamp = QDateTime::currentDateTime();
}

QString TextDocument::getDisplayName() const
{
    return displayName;
}

QString TextDocument::getFilePath() const
{
    return filePath;
}

void TextDocument::setFilePath(const QString& path)
{
    if (!path.isNull() && !path.isEmpty())
    {
        QFileInfo fileInfo(path);
        filePath = fileInfo.absoluteFilePath();
        displayName = fileInfo.fileName();
    }
    else
    {
        filePath = QString();
        this->setReadOnly(false);
        this->setModified(false);
        displayName = tr("untitled");
    }

    emit filePathChanged();
}

bool TextDocument::isNew() const
{
    return filePath.isNull() || filePath.isEmpty();
}

bool TextDocument::isReadOnly() const
{
    return readOnlyFlag;
}

void TextDocument::setReadOnly(bool readOnly)
{
    readOnlyFlag = readOnly;
}

QDateTime TextDocument::getTimestamp() const
{
    return timestamp;
}

void TextDocument::setTimestamp(const QDateTime& timestamp)
{
    this->timestamp = timestamp;
}

void TextDocument::notifyTextBlockRemoved(const QTextBlock& block)
{
    emit textBlockRemoved(block.position());
    emit textBlockRemoved(block);
}

void TextDocument::initializeUntitledDocument()
{
    QPlainTextDocumentLayout* documentLayout =
        new QPlainTextDocumentLayout(this);
    this->setDocumentLayout(documentLayout);

    filePath = QString();
    readOnlyFlag = false;
    displayName = tr("untitled");
    timestamp = QDateTime::currentDateTime();
}
