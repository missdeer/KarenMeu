#include <QApplication>
#include <QWebEngineSettings>
#include "mainwindow.h"
#include "settings.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);
    
    Settings settings;
    settings.initialize();
    g_settings = &settings;

    QWebEngineSettings::globalSettings()->setAttribute(QWebEngineSettings::JavascriptCanAccessClipboard, true);
    
    MainWindow w;
    w.showMaximized();
    
    return a.exec();
}
