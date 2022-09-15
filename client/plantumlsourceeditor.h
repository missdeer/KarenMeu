#ifndef PLANTUMLSOURCEEDITOR_H
#define PLANTUMLSOURCEEDITOR_H

#include <QWidget>

#include "ScintillaEdit.h"

class ScintillaConfig;

class PlantUMLSourceEditor : public ScintillaEdit
{
    Q_OBJECT
public:
    explicit PlantUMLSourceEditor(QWidget *parent = nullptr);

    void       initialize();
    void       setContent(const QByteArray &content);
    QByteArray content();

signals:
    void contentModified();
private slots:

    void modified(Scintilla::ModificationFlags type,
                  Scintilla::Position          position,
                  Scintilla::Position          length,
                  Scintilla::Position          linesAdded,
                  const QByteArray            &text,
                  Scintilla::Position          line,
                  Scintilla::FoldLevel         foldNow,
                  Scintilla::FoldLevel         foldPrev);

private:
    ScintillaConfig *m_sc;
};

#endif // PLANTUMLSOURCEEDITOR_H
