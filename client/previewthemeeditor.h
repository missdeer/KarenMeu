#ifndef PREVIEWTHEMEEDITOR_H
#define PREVIEWTHEMEEDITOR_H

#include <QWidget>

#include "ScintillaEdit.h"

class ScintillaConfig;

class PreviewThemeEditor : public QWidget
{
    Q_OBJECT
public:
    explicit PreviewThemeEditor(QWidget *parent = nullptr);
    void       initialize(const QString &lexer);
    void       setContent(const QByteArray &content);
    QByteArray content();
    void       clearAll();
signals:
    void contentModified();
private slots:

    void modified(Scintilla::ModificationFlags type,
                  Scintilla::Position          position,
                  Scintilla::Position          length,
                  Scintilla::Position          linesAdded,
                  const QByteArray &           text,
                  Scintilla::Position          line,
                  Scintilla::FoldLevel         foldNow,
                  Scintilla::FoldLevel         foldPrev);

private:
    ScintillaEdit *  m_editor;
    ScintillaConfig *m_sc;
    QString          m_lexer;
};

#endif // PREVIEWTHEMEEDITOR_H
