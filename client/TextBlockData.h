#ifndef TEXTBLOCKDATA_H
#define TEXTBLOCKDATA_H

#include <QObject>
#include <QTextBlock>
#include <QTextBlockUserData>

#include "TextDocument.h"

/**
 * User data for use with the MarkdownHighlighter and DocumentStatistics.
 */
class TextBlockData
    : public QObject
    , public QTextBlockUserData
{
    Q_OBJECT

public:
    /**
     * Constructor.
     */
    TextBlockData(TextDocument *document, const QTextBlock &block) : document(document), blockRef(block)
    {
        wordCount                  = 0;
        alphaNumericCharacterCount = 0;
        sentenceCount              = 0;
        lixLongWordCount           = 0;
        blankLine                  = true;
    }

    /**
     * Destructor.
     */
    virtual ~TextBlockData()
    {
        document->notifyTextBlockRemoved(blockRef);
    }

    TextDocument *document;

    int  wordCount;
    int  alphaNumericCharacterCount;
    int  sentenceCount;
    int  lixLongWordCount;
    bool blankLine;

    /**
     * Parent text block.  For use with fetching the block's document
     * position, which can shift as text is inserted and deleted.
     */
    QTextBlock blockRef;
};

#endif // TEXTBLOCKDATA_H
