#include <QDebug>

#include "MarkdownEditorTheme.h"

MarkdownEditorTheme::MarkdownEditorTheme() : builtInFlag(false) {}

MarkdownEditorTheme::MarkdownEditorTheme(const QString &name, bool builtIn) : name(name), builtInFlag(builtIn) {}

MarkdownEditorTheme::~MarkdownEditorTheme() {}

QString MarkdownEditorTheme::getName() const
{
    return name;
}

void MarkdownEditorTheme::setName(const QString &value)
{
    name = value;
}

bool MarkdownEditorTheme::isBuiltIn() const
{
    return builtInFlag;
}

void MarkdownEditorTheme::setBuiltIn(const bool builtIn)
{
    builtInFlag = builtIn;
}

QColor MarkdownEditorTheme::getDefaultTextColor() const
{
    return defaultTextColor;
}

void MarkdownEditorTheme::setDefaultTextColor(const QColor &value)
{
    defaultTextColor = value;
}

QColor MarkdownEditorTheme::getMarkupColor() const
{
    return markupColor;
}

void MarkdownEditorTheme::setMarkupColor(const QColor &value)
{
    markupColor = value;
}

QColor MarkdownEditorTheme::getLinkColor() const
{
    return linkColor;
}

void MarkdownEditorTheme::setLinkColor(const QColor &value)
{
    linkColor = value;
}

QColor MarkdownEditorTheme::getHeadingColor() const
{
    return headingColor;
}

void MarkdownEditorTheme::setHeadingColor(const QColor &value)
{
    headingColor = value;
}

QColor MarkdownEditorTheme::getEmphasisColor() const
{
    return emphasisColor;
}

void MarkdownEditorTheme::setEmphasisColor(const QColor &value)
{
    emphasisColor = value;
}

QColor MarkdownEditorTheme::getBlockquoteColor() const
{
    return blockquoteColor;
}

void MarkdownEditorTheme::setBlockquoteColor(const QColor &value)
{
    blockquoteColor = value;
}

QColor MarkdownEditorTheme::getCodeColor() const
{
    return codeColor;
}

void MarkdownEditorTheme::setCodeColor(const QColor &value)
{
    codeColor = value;
}

QColor MarkdownEditorTheme::getSpellingErrorColor() const
{
    return spellingErrorColor;
}

void MarkdownEditorTheme::setSpellingErrorColor(const QColor &value)
{
    spellingErrorColor = value;
}

EditorAspect MarkdownEditorTheme::getEditorAspect() const
{
    return editorAspect;
}

void MarkdownEditorTheme::setEditorAspect(const EditorAspect value)
{
    if ((value < EditorAspectFirst) || (value > EditorAspectLast))
    {
        editorAspect = EditorAspectFirst;
        qCritical() << "Theme editor aspect value of " << value << " is out of range. Defaulting to value of " << editorAspect;
    }
    else
    {
        editorAspect = value;
    }
}

PictureAspect MarkdownEditorTheme::getBackgroundImageAspect() const
{
    return backgroundImageAspect;
}

void MarkdownEditorTheme::setBackgroundImageAspect(const PictureAspect value)
{
    if ((value < PictureAspectFirst) || (value > PictureAspectLast))
    {
        backgroundImageAspect = PictureAspectFirst;
        qCritical() << "Theme background image aspect value of " << value << " is out of range. Defaulting to value of " << backgroundImageAspect;
    }
    else
    {
        backgroundImageAspect = value;
    }
}

QString MarkdownEditorTheme::getBackgroundImageUrl() const
{
    return backgroundImageUrl;
}

void MarkdownEditorTheme::setBackgroundImageUrl(const QString &value)
{
    backgroundImageUrl = value;
}

QColor MarkdownEditorTheme::getBackgroundColor() const
{
    return backgroundColor;
}

void MarkdownEditorTheme::setBackgroundColor(const QColor &value)
{
    backgroundColor = value;
}

QColor MarkdownEditorTheme::getEditorBackgroundColor() const
{
    return editorBackgroundColor;
}

void MarkdownEditorTheme::setEditorBackgroundColor(const QColor &value)
{
    editorBackgroundColor = value;
}

QColor MarkdownEditorTheme::getHudForegroundColor() const
{
    return hudForegroundColor;
}

void MarkdownEditorTheme::setHudForegroundColor(const QColor &value)
{
    hudForegroundColor = value;
}

QColor MarkdownEditorTheme::getHudBackgroundColor() const
{
    return hudBackgroundColor;
}

void MarkdownEditorTheme::setHudBackgroundColor(const QColor &value)
{
    hudBackgroundColor = value;
}
