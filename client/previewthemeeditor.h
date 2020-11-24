#ifndef PREVIEWTHEMEEDITOR_H
#define PREVIEWTHEMEEDITOR_H

#include "ScintillaEdit.h"

class ScintillaConfig;

class PreviewThemeEditor : public ScintillaEdit
{
    Q_OBJECT
public:
    explicit PreviewThemeEditor(QWidget* parent);
    void initialize();
    void setContent(const QByteArray &content);
    QByteArray content();
    
signals:
    void contentModified();
private slots:
    
    void modified(int type, int position, int length, int linesAdded,
                  const QByteArray &text, int line, int foldNow, int foldPrev);
private:
    ScintillaConfig *m_sc;
};

#endif // PREVIEWTHEMEEDITOR_H
