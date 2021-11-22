#include <QFocusEvent>
#include <QTimer>

#include "webbrowseraddressbar.h"

WebBrowserAddressBar::WebBrowserAddressBar(QWidget *parent) : QLineEdit(parent) {}

void WebBrowserAddressBar::focusInEvent(QFocusEvent *e)
{
    QLineEdit::focusInEvent(e);
    QTimer::singleShot(0, this, &QLineEdit::selectAll);
}
