#include <QApplication>
#include "mainwindow.h"
#include "settings.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    Settings settings;
    settings.initialize();
    g_settings = &settings;
    
    MainWindow w;
    w.showMaximized();
    
    return a.exec();
}
