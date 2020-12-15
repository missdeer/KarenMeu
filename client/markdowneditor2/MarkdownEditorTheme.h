#ifndef THEME_H
#define THEME_H

#include <QColor>
#include <QString>

enum EditorAspect
{
    EditorAspectFirst,
    EditorAspectStretch = EditorAspectFirst,
    EditorAspectCenter,
    EditorAspectLast = EditorAspectCenter
};

enum TextWidth
{
    TextWidthNarrow,
    TextWidthMedium,
    TextWidthWide
};

enum PictureAspect
{
    PictureAspectFirst,
    PictureAspectNone = PictureAspectFirst,
    PictureAspectTile,
    PictureAspectCenter,
    PictureAspectStretch,
    PictureAspectScale,
    PictureAspectZoom,
    PictureAspectLast = PictureAspectZoom,
};

/**
 * Encapsulates a theme for customizing the look and feel of the application.
 */
class MarkdownEditorTheme
{
public:
    MarkdownEditorTheme();
    MarkdownEditorTheme(const QString &name, bool builtIn = true);
    ~MarkdownEditorTheme();

    QString getName() const;
    void    setName(const QString &value);

    bool isBuiltIn() const;
    void setBuiltIn(const bool builtIn);

    QColor getDefaultTextColor() const;
    void   setDefaultTextColor(const QColor &value);

    QColor getMarkupColor() const;
    void   setMarkupColor(const QColor &value);

    QColor getLinkColor() const;
    void   setLinkColor(const QColor &value);

    QColor getHeadingColor() const;
    void   setHeadingColor(const QColor &value);

    QColor getEmphasisColor() const;
    void   setEmphasisColor(const QColor &value);

    QColor getBlockquoteColor() const;
    void   setBlockquoteColor(const QColor &value);

    QColor getCodeColor() const;
    void   setCodeColor(const QColor &value);

    QColor getSpellingErrorColor() const;
    void   setSpellingErrorColor(const QColor &value);

    EditorAspect getEditorAspect() const;
    void         setEditorAspect(const EditorAspect value);

    PictureAspect getBackgroundImageAspect() const;
    void          setBackgroundImageAspect(const PictureAspect value);

    QString getBackgroundImageUrl() const;
    void    setBackgroundImageUrl(const QString &value);

    QColor getBackgroundColor() const;
    void   setBackgroundColor(const QColor &value);

    QColor getEditorBackgroundColor() const;
    void   setEditorBackgroundColor(const QColor &value);

    QColor getHudForegroundColor() const;
    void   setHudForegroundColor(const QColor &value);

    QColor getHudBackgroundColor() const;
    void   setHudBackgroundColor(const QColor &value);

private:
    QString name;

    // true if theme is built-in, false if custom (i.e., user-created).
    bool builtInFlag;

    QColor defaultTextColor;
    QColor editorBackgroundColor;
    QColor markupColor;
    QColor linkColor;
    QColor headingColor;
    QColor emphasisColor;
    QColor blockquoteColor;
    QColor codeColor;
    QColor spellingErrorColor;

    EditorAspect  editorAspect;
    PictureAspect backgroundImageAspect;
    QString       backgroundImageUrl;
    QColor        backgroundColor;

    QColor hudForegroundColor;
    QColor hudBackgroundColor;
};

#endif
