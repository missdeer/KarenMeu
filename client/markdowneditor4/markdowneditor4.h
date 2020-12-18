#ifndef MARKDOWNEDITOR4_H
#define MARKDOWNEDITOR4_H

#include <vtextedit/markdowneditorconfig.h>
#include <vtextedit/vmarkdowneditor.h>

class MarkdownEditor4 : public vte::VMarkdownEditor
{
    Q_OBJECT
public:
    explicit MarkdownEditor4(const QSharedPointer<vte::MarkdownEditorConfig> &p_config, QWidget *parent = nullptr);
};

#endif // MARKDOWNEDITOR4_H
