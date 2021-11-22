#include "webbrowser.h"

WebBrowser::WebBrowser(QWidget *parent) : QWebEngineView(parent) {}

QWebEngineView *WebBrowser::createWindow(QWebEnginePage::WebWindowType type)
{
    Q_UNUSED(type);
    return this;
}
