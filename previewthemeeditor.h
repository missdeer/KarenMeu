#ifndef PREVIEWTHEMEEDITOR_H
#define PREVIEWTHEMEEDITOR_H

#include "ScintillaEdit.h"

class ScintillaConfig;

class PreviewThemeEditor : public ScintillaEdit
{
public:
    explicit PreviewThemeEditor(QWidget* parent);
    void initialize();
    void setContent(const QByteArray &content);
private:
    ScintillaConfig *m_sc;
};

#endif // PREVIEWTHEMEEDITOR_H
