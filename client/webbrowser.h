#ifndef WEBBROWSER_H
#define WEBBROWSER_H

#include <QWebEngineView>

class WebBrowser : public QWebEngineView
{
    Q_OBJECT
public:
    WebBrowser(QWidget *parent = nullptr);

protected:
    QWebEngineView *createWindow(QWebEnginePage::WebWindowType type) override;
};

#endif // WEBBROWSER_H
