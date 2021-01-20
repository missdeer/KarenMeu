#ifndef UTILS_H
#define UTILS_H

#include <QtGlobal>

QT_FORWARD_DECLARE_CLASS(QWidget);
QT_FORWARD_DECLARE_CLASS(QPlainTextEdit);

namespace ClientUtils
{
    void    InitializeWidgetFont(QWidget *widget);
    void    setHtmlContent(QPlainTextEdit *editor, const QString &html);
    QString getDefaultFileSaveOpenDirectory();
}; // namespace ClientUtils

#endif // UTILS_H
