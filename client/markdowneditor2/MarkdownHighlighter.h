#ifndef MARKDOWN_HIGHLIGHTER_H
#define MARKDOWN_HIGHLIGHTER_H

#include <QSyntaxHighlighter>

#include "MarkdownStyles.h"
#include "MarkdownTokenizer.h"
#include "TextDocument.h"
#include "Token.h"
#include "spelling/dictionary_ref.h"

class QColor;
class QRegularExpression;
class QString;
class QTextCharFormat;
class MarkdownEditor;

/**
 * Highlighter for the Markdown text format.
 */
class MarkdownHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    /**
     * Constructor.  Takes as a parameter the Markdown editor whose
     * TextDocument is to be highlighted.
     */
    MarkdownHighlighter(MarkdownEditor *editor);

    /**
     * Destructor.
     */
    ~MarkdownHighlighter();

    /**
     * Overridden method to highlight the given text for the current
     * text block in the document.
     */
    void highlightBlock(const QString &text);

    /**
     * Sets the dictionary to use for live spell checking.
     */
    void setDictionary(const DictionaryRef &dictionary);

    /**
     * Increases the font size by one point.
     */
    void increaseFontSize();

    /**
     * Decreases the font size by one point.
     */
    void decreaseFontSize();

    /**
     * Sets the color scheme.
     */
    void setColorScheme(const QColor &defaultTextColor,
                        const QColor &backgroundColor,
                        const QColor &markupColor,
                        const QColor &linkColor,
                        const QColor &headingColor,
                        const QColor &emphasisColor,
                        const QColor &blockquoteColor,
                        const QColor &codeColor,
                        const QColor &spellingErrorColor);

    /**
     * Sets whether large heading sizes are enabled.
     */
    void setEnableLargeHeadingSizes(const bool enable);

    /**
     * Sets whether emphasized text is underlined instead of italicized.
     */
    void setUseUnderlineForEmphasis(const bool enable);

    /**
     * Sets the font family and point size.
     */
    void setFont(const QString &fontFamily, const double fontSize);

    /**
     * Sets whether live spell checking is enabled.
     */
    void setSpellCheckEnabled(const bool enabled);

    /**
     * Sets the blockquote style.
     */
    void setBlockquoteStyle(const BlockquoteStyle style);

    /**
     * Sets whether manual linebreaks (2 spaces at the end of a line) will be highlighted
     */
    void setHighlightLineBreaks(bool enable);

signals:
    /**
     * Notifies listeners that a heading was found in the document in the
     * given QTextBlock and with the given level and heading text.
     */
    void headingFound(int level, const QString &text, QTextBlock block);

    /**
     * Notifies listeners that a heading was discovered to have been removed
     * from the document at the given cursor position.
     */
    void headingRemoved(int position);

    /**
     * FOR INTERNAL USE ONLY
     *
     * This signal is used internally to restart highlighting on the
     * previous line, which is needed for setext headings.  Unfortunately,
     * QSyntaxHighlighter only goes forward in its highlighting, not
     * backwards.  Neither can rehighlightBlock() be called internally,
     * since recursive calls to the class will wipe its state data and will
     * cause the application to crash. This is a workaround to queue a
     * highlighting action for the prior text block in the event system,
     * so that recursion isn't used.
     */
    void highlightBlockAtPosition(int position);

public slots:
    /**
     * Signalled by a text editor when the user has resumed typing.
     * This signal is used to ensure spell checking is not performed
     * for the current word the cursor is on while the user is still
     * typing.
     */
    void onTypingResumed();

    /**
     * Signalled by a text editor when the user has ceased typing.
     * This signal is used to ensure spell checking is performed
     * for the current word the cursor is on after the user has
     * stopped typing.
     */
    void onTypingPaused();

    /**
     * Signalled by a text editor when the user changes the text
     * cursor position.  This signal is used to ensure spell
     * checking is performed for the previous line in which the
     * text cursor was positioned before the user changed the text
     * position, in case the last word the user was typing was not
     * yet spell checked.
     */
    void onCursorPositionChanged();

private slots:
    /*
     * Highlights the text block at the given cursor position of the
     * document.  See explanation for highlightBlockAtPosition().
     */
    void onHighlightBlockAtPosition(int position);

    /*
     * Determines if the text block being removed is a heading, and if so,
     * emits the headingRemoved() signal.
     */
    void onTextBlockRemoved(const QTextBlock &block);

private:
    MarkdownEditor *   editor;
    MarkdownTokenizer *tokenizer;
    DictionaryRef      dictionary;
    bool               spellCheckEnabled;
    bool               typingPaused;
    QTextBlock         currentLine;
    bool               useUndlerlineForEmphasis;
    bool               highlightLineBreaks;
    bool               inBlockquote;
    BlockquoteStyle    blockquoteStyle;
    QColor             defaultTextColor;
    QColor             backgroundColor;
    QColor             markupColor;
    QColor             linkColor;
    QColor             headingColor;
    QColor             emphasisColor;
    QColor             blockquoteColor;
    QColor             codeColor;
    QColor             spellingErrorColor;

    QTextCharFormat defaultFormat;
    bool            applyStyleToMarkup[TokenLast];
    QColor          colorForToken[TokenLast];
    bool            emphasizeToken[TokenLast];
    bool            strongToken[TokenLast];
    bool            strongMarkup[TokenLast];
    bool            strikethroughToken[TokenLast];
    int             fontSizeIncrease[TokenLast];

    QRegularExpression heading1SetextRegex;
    QRegularExpression heading2SetextRegex;

    void spellCheck(const QString &text);
    void setupTokenColors();
    void setupHeadingFontSize(bool useLargeHeadings);

    void applyFormattingForToken(const Token &token);
    void storeHeadingData(const Token &token, const QString &text);

    /*
     * Returns true if the given QTextBlock userState indicates that the
     * text block contains a heading.
     */
    bool isHeadingBlockState(int state) const;

    /**
     * Returns true if the given block of text matches the markup for
     * the second line of a setext heading for level 1.
     */
    bool matchesHeading1SetextMarkup(const QString &text) const;

    /**
     * Returns true if the given block of text matches the markup for
     * the second line of a setext heading for level 2.
     */
    bool matchesHeading2SetextMarkup(const QString &text) const;
};

#endif