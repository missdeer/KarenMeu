#ifndef WEBBROWSERADDRESSBAR_H
#define WEBBROWSERADDRESSBAR_H

#include <QLineEdit>

class WebBrowserAddressBar : public QLineEdit
{
    Q_OBJECT
public:
    explicit WebBrowserAddressBar(QWidget *parent = nullptr);

protected:
    virtual void focusInEvent(QFocusEvent *e) override;
};

#endif // WEBBROWSERADDRESSBAR_H
