#ifndef MARKDOWNEDITOR_H
#define MARKDOWNEDITOR_H

#include "ScintillaEdit.h"
#include "scintillaconfig.h"

class MarkdownEditor : public ScintillaEdit
{
    Q_OBJECT
public:
    explicit MarkdownEditor(QWidget *parent = nullptr);
    void initialize();
    void setContent(const QString& content);
    void setContent(const QByteArray& content);
    
private:
    ScintillaConfig m_sc;
};

#endif // MARKDOWNEDITOR_H
