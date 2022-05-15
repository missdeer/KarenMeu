#include <QFocusEvent>
#include <QTimer>

#include "webbrowseraddressbar.h"

WebBrowserAddressBar::WebBrowserAddressBar(QWidget *parent) : QLineEdit(parent) {}

void WebBrowserAddressBar::focusInEvent(QFocusEvent *event)
{
    QLineEdit::focusInEvent(event);
    QTimer::singleShot(0, this, &QLineEdit::selectAll);
}
