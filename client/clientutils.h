#ifndef UTILS_H
#define UTILS_H

#include <QtGlobal>

QT_FORWARD_DECLARE_CLASS(QWidget);

namespace ClientUtils
{
    void InitializeWidgetFont(QWidget *widget);
    QString getDefaultFileSaveOpenDirectory();
};

#endif // UTILS_H
