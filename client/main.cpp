#include <QApplication>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDir>
#include <QLibraryInfo>
#include <QMessageBox>
#include <QSplashScreen>
#include <QSslSocket>
#include <QTranslator>
#include <QWebEngineProfile>
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
    qputenv("QT_ENABLE_GLYPH_CACHE_WORKAROUND", "1");
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QCoreApplication::setOrganizationName("MiniDump.Info");
    QCoreApplication::setApplicationName("KarenMeu");
    QCoreApplication::setApplicationVersion("1.0");

    // main application and dynamic linked library locale
#if defined(Q_OS_MAC)
    MacApplication app(argc, argv);
    QDir           dir(QCoreApplication::applicationDirPath());
    qputenv("QTWEBENGINE_CHROMIUM_FLAGS", "--single-process");
    dir.cdUp();
    dir.cd("/Frameworks/QtWebEngineCore.framework/Helpers/QtWebEngineProcess.app/Contents/MacOS/");
    auto fn = dir.absoluteFilePath("QtWebEngineProcess");
    if (QFile::exists(fn))
    {
        qputenv("QTWEBENGINEPROCESS_PATH", fn.toUtf8());
    }
    dir.setPath(QApplication::applicationDirPath());
    dir.cdUp();
    dir.cd("Resources/translations");
    QString localeDirPath = dir.absolutePath();
#else
    QApplication app(argc, argv);
    QString      localeDirPath = QApplication::applicationDirPath() + "/translations";
    if (!QDir(localeDirPath).exists())
    {
        localeDirPath = QApplication::applicationDirPath() + "/../translations";
    }
#endif

    QPixmap       pixmap(":/KarenMeu.png");
    QSplashScreen splash(pixmap);
    splash.show();
    QApplication::processEvents();

    QString     localeName = QLocale::system().name().replace("-", "_");
    QTranslator translator;
    QTranslator qtTranslator;

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

    auto *defaultProfile = QWebEngineProfile::defaultProfile();
    defaultProfile->setHttpCacheType(QWebEngineProfile::MemoryHttpCache);
    defaultProfile->setPersistentCookiesPolicy(QWebEngineProfile::AllowPersistentCookies);
    defaultProfile->setHttpUserAgent("Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:98.0) Gecko/20100101 Firefox/98.0");

    // setup command line parser
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("file", QCoreApplication::translate("main", "The file to open."));
    parser.process(app);

    // get filename from command line arguments
    QString fileName(":/rc/sample.md");
    auto    lastOpenedFilePath = g_settings->getLastOpenedFilePath();
    if (QFile::exists(lastOpenedFilePath))
    {
        fileName = lastOpenedFilePath;
    }
    const QStringList cmdLineArgs = parser.positionalArguments();
    if (!cmdLineArgs.isEmpty() && QFile::exists(cmdLineArgs.at(0)))
    {
        fileName = cmdLineArgs.at(0);
    }

    MainWindow win;
    win.showMaximized();
    win.openFile(fileName);

#if defined(Q_OS_MAC)
    app.connect(&app, &MacApplication::openFile, &win, &MainWindow::openFile);
#endif

    splash.finish(&win);
    return QCoreApplication::exec();
}
