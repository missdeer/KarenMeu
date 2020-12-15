#ifndef TOKEN_H_
#define TOKEN_H_

#include <QString>

/**
 * A token for use with a parser of marked up text that will have syntax
 * highlighting applied to it in an editor.  This class has the conventional
 * token type, but unconventionally does not have a token value.  Instead
 * of a token string value, a starting position in the parsed string and the
 * length of the token text is provided.  This saves some space in the class,
 * since only the starting position and length of the token are needed to
 * style the text in the editor.
 *
 * Note that this class is especially suited to markup formats (i.e., Markdown,
 * ReStructured Text, etc.), but does not need to be restricted solely to that
 * format.  To avoid burdening the parser with having to have separate token
 * types for markup, it was decided to specify the markup (if any) within
 * main text tokens.  This also makes highlighting in an editor smoother,
 * since the markup tends to be visually formatted the same way as the text it
 * marks up.  For example, to render `**bold**` text, we wouldn't want the
 * `**` having a separate bold formatting applied from `bold`.  We would want
 * them to be formatted together in one chunk, as that is more efficient for
 * the editor.  Of course, this could be solved by traversing an AST, but
 * it was instead decided to use a flattened AST in the form of a list of tokens
 * per line in the document.  See MarkdownTokenizer.h for rationale.
 */
class Token
{
public:
    /**
     * Returns the token type.
     */
    int getType() const;

    /**
     * Sets the token type.
     */
    void setType(int type);

    /**
     * Gets the starting position of the token in the text (i.e., the
     * column number in a line).
     */
    int getPosition() const;

    /**
     * Sets the starting position of the token in the text (i.e., the
     * column number in a line).
     */
    void setPosition(int position);

    /**
     * Gets the length (in characters) of the token.
     */
    int getLength() const;

    /**
     * Sets the length (in characters) of the token.
     */
    void setLength(int length);

    /**
     * If the token begins with markup characters, gets the length of
     * the markup.  Otherwise, returns 0.
     */
    int getOpeningMarkupLength() const;

    /**
     * If the token begins with markup characters, gets the length of
     * the markup.  Defaults to 0 if not set.
     */
    void setOpeningMarkupLength(int length);

    /**
     * If the token ends with markup characters, gets the length of
     * the markup.  Otherwise, returns 0.
     */
    int getClosingMarkupLength() const;

    /**
     * If the token ends with markup characters, gets the length of
     * the markup.  Defaults to 0 if not set.
     */
    void setClosingMarkupLength(int length);

private:
    int type {-1};
    int position {0};
    int length {0};
    int openingMarkupLength {0};
    int closingMarkupLength {0};
};

#endif
