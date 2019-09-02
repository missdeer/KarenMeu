#ifndef SCINTILLACONFIG_H
#define SCINTILLACONFIG_H

#include <QString>

QT_FORWARD_DECLARE_CLASS(QDomElement);
class ScintillaEdit;

class ScintillaConfig
{
public:
    explicit ScintillaConfig(ScintillaEdit* sci) : m_sci(sci) {}
    void initScintilla();
    void initEditorFolderStyle();
    void initLexerStyle(const QString &lang);
    void initEditorMargins();
private:
    ScintillaEdit* m_sci;
    void applyStyle(const QDomElement& styleElem);

    void applyLanguageStyle(const QString& configPath, const QString& lang);
    void applyThemeStyle(const QString& themePath, const QString &lang);
};

#endif // SCINTILLACONFIG_H
