#include <QApplication>
#include <QCommandLineParser>
#include <QDir>
#include <QLibraryInfo>
#include <QMessageBox>
#include <QSslSocket>
#include <QTranslator>
#include <QWebEngineSettings>
#include <QtCore>

#include "mainwindow.h"
#include "settings.h"

#if defined(Q_OS_MAC)
#    include <QFileOpenEvent>

#    include "macapplication.h"
#endif

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setOrganizationName("MiniDump.Info");
    QCoreApplication::setApplicationName("KarenMeu");
    QCoreApplication::setApplicationVersion("1.0");

    QString     localeName = QLocale::system().name().replace("-", "_");
    QTranslator translator;
    QTranslator qtTranslator;

    // main application and dynamic linked library locale
#if defined(Q_OS_MAC)
    MacApplication a(argc, argv);
    QDir           dir(QApplication::applicationDirPath());
    dir.cdUp();
    dir.cd("Resources/translations");
    QString localeDirPath = dir.absolutePath();
#else
    QApplication a(argc, argv);
    QString      localeDirPath = QApplication::applicationDirPath() + "/translations";
    if (!QDir(localeDirPath).exists())
    {
        localeDirPath = QApplication::applicationDirPath() + "/../translations";
    }
#endif

    QString i18nFile = QString("KarenMeu_%1.qm").arg(localeName);
    if (!translator.load(i18nFile, localeDirPath))
    {
        qDebug() << "loading " << i18nFile << " from " << localeDirPath << " failed";
    }
    else
    {
        qDebug() << "loading " << i18nFile << " from " << localeDirPath << " success";
        if (!QApplication::installTranslator(&translator))
        {
            qDebug() << "installing translator failed ";
        }
    }

    i18nFile = QString("qt_%1.qm").arg(localeName);
    if (!qtTranslator.load(i18nFile, localeDirPath))
    {
        qDebug() << "loading " << i18nFile << " from " << localeDirPath << " failed";
    }
    else
    {
        qDebug() << "loading " << i18nFile << " from " << localeDirPath << " success";
        if (!QApplication::installTranslator(&qtTranslator))
        {
            qDebug() << "installing qt translator failed ";
        }
    }

    if (!QSslSocket::supportsSsl())
    {
        QMessageBox::critical(
            nullptr, QObject::tr("Critical error"), QObject::tr("SSL not supported, some features may not work as expected."), QMessageBox::Ok);
    }
    g_settings.reset(new Settings);
    g_settings->initialize();

    QWebEngineSettings::globalSettings()->setAttribute(QWebEngineSettings::JavascriptCanAccessClipboard, true);

    // setup command line parser
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("file", QApplication::translate("main", "The file to open."));
    parser.process(a);

    // get filename from command line arguments
    QString           fileName    = ":/rc/sample.md";
    auto              lastOpenedFilePath = g_settings->getLastOpenedFilePath();
    if (QFile::exists(lastOpenedFilePath))
    {
        fileName = lastOpenedFilePath;
    }
    const QStringList cmdLineArgs = parser.positionalArguments();
    if (!cmdLineArgs.isEmpty() && QFile::exists(cmdLineArgs.at(0)))
    {
        fileName = cmdLineArgs.at(0);
    }

    MainWindow w;
    w.showMaximized();
    w.openFile(fileName);

#if defined(Q_OS_MAC)
    a.connect(&a, &MacApplication::openFile, &w, &MainWindow::openFile);
#endif

    return QApplication::exec();
}
