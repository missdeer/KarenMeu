#include "markdowneditor4.h"

MarkdownEditor4::MarkdownEditor4(const QSharedPointer<vte::MarkdownEditorConfig> &p_config, QWidget *parent) : vte::VMarkdownEditor(p_config, parent)
{
}
