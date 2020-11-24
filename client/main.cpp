#include <QtCore>
#include <QApplication>
#include <QWebEngineSettings>
#include <QTranslator>
#include <QDir>
#include <QCommandLineParser>
#include <QLibraryInfo>
#include "mainwindow.h"
#include "settings.h"

#ifdef Q_OS_WIN
#include <QFileInfo>
#include <QSettings>

// Helper function to register supported file types
// This is needed to enable the application jump list to show the desired recent files
static void associateFileTypes(const QStringList &fileTypes)
{
    QString displayName = QGuiApplication::applicationDisplayName();
    QString filePath = QCoreApplication::applicationFilePath();
    QString fileName = QFileInfo(filePath).fileName();

    QSettings settings("HKEY_CURRENT_USER\\Software\\Classes\\Applications\\" + fileName, QSettings::NativeFormat);
    settings.setValue("FriendlyAppName", displayName);

    settings.beginGroup("SupportedTypes");
    foreach (const QString& fileType, fileTypes)
        settings.setValue(fileType, QString());
    settings.endGroup();

    settings.beginGroup("shell");
    settings.beginGroup("open");
    settings.setValue("FriendlyAppName", displayName);
    settings.beginGroup("Command");
    settings.setValue(".", QChar('"') + QDir::toNativeSeparators(filePath) + QString("\" \"%1\""));
}
#endif


int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);
    
    QApplication::setOrganizationName("MiniDump.Info");
    QApplication::setApplicationName("KarenMeu");
    QApplication::setApplicationDisplayName("KarenMeu");
    QApplication::setApplicationVersion("1.0");
    
#ifdef Q_OS_WIN
    associateFileTypes(QStringList()<< ".markdown" << ".md" << ".mdown");
#endif
    
    QString locale = "zh_CN";
    QTranslator translator;
    QTranslator qtTranslator;

    // main application and dynamic linked library locale
#if defined(Q_OS_MAC)
    QString localeDirPath = QApplication::applicationDirPath() + "/../Resources/translations";
#else
    QString localeDirPath = QApplication::applicationDirPath() + "/translations";
    if (!QDir(localeDirPath).exists())
    {
        localeDirPath = QApplication::applicationDirPath() + "/../translations";
    }
#endif

    if (!translator.load("KarenMeu_" + locale, localeDirPath))
    {
        qDebug() << "loading " << locale << " from " << localeDirPath << " failed";
    }
    else
    {
        qDebug() << "loading " << locale << " from " << localeDirPath << " success";
        if (!QApplication::installTranslator(&translator))
        {
            qDebug() << "installing translator failed ";
        }
    }

    // qt locale
    if (!qtTranslator.load("qt_" + locale, localeDirPath))
    {
        qDebug() << "loading " << locale << " from " << localeDirPath << " failed";
    }
    else
    {
        qDebug() << "loading " << locale << " from " << localeDirPath << " success";
        if (!QApplication::installTranslator(&qtTranslator))
        {
            qDebug() << "installing qt translator failed ";
        }
    }
    
    Settings settings;
    settings.initialize();
    g_settings = &settings;

    QWebEngineSettings::globalSettings()->setAttribute(QWebEngineSettings::JavascriptCanAccessClipboard, true);
    
    // setup command line parser
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("file", QApplication::translate("main", "The file to open."));
    parser.process(a);

    // get filename from command line arguments
    QString fileName = ":/rc/sample.md";
    const QStringList cmdLineArgs = parser.positionalArguments();
    if (!cmdLineArgs.isEmpty()) {
        fileName = cmdLineArgs.at(0);
    }
    
    MainWindow w;
    w.showMaximized();
    w.openFile(fileName);
    return QApplication::exec();
}
