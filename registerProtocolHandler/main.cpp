#include <QCoreApplication>
#include <QDir>
#include <QSettings>

#ifdef WIN32
#    include <shobjidl.h>

#    ifdef WIN64
QSettings::Format RegFormat = QSettings::Registry64Format;
#    else
QSettings::Format RegFormat = QSettings::Registry32Format;
#    endif
#endif

#define APPLICATION_ID "KarenMeu"
#define REGKEYFORMAT_ASSOCFILE APPLICATION_ID ".AssocFile.%1"

#ifdef EXECUTE_ON_UAC
#    define REGKEY_SOFTWARE "HKEY_LOCAL_MACHINE\\SOFTWARE"
#    define REGKEY_CLASSES REGKEY_SOFTWARE "\\Classes"
#    define REGKEYFORMAT_CLASSES REGKEY_SOFTWARE "\\Classes\\%1"
#    define REGKEYFORMAT_ASSOCPATH REGKEY_CLASSES "\\" APPLICATION_ID ".AssocFile.%1"
#    define REGKEY_REGISTEREDAPPLICATIONS REGKEY_SOFTWARE "\\RegisteredApplications"
#    define REGKEY_APPLICATION REGKEY_SOFTWARE "\\" APPLICATION_ID
#    define REGKEY_APPLICATION_INAPP REGKEY_CLASSES "\\Applications\\" APPLICATION_ID ".exe"
#else
#    define REGKEY_SOFTWARE "HKEY_CURRENT_USER\\Software"
#    define REGKEY_CLASSES REGKEY_SOFTWARE "\\Classes"
#    define REGKEYFORMAT_CLASSES REGKEY_SOFTWARE "\\Classes\\%1"
#    define REGKEYFORMAT_ASSOCPATH REGKEY_CLASSES "\\" APPLICATION_ID ".AssocFile.%1"
#    define REGKEY_REGISTEREDAPPLICATIONS REGKEY_SOFTWARE "\\RegisteredApplications"
#    define REGKEY_APPLICATION REGKEY_SOFTWARE "\\" APPLICATION_ID
#    define REGKEY_APPLICATION_INAPP REGKEY_CLASSES "\\Applications\\" APPLICATION_ID ".exe"
#endif

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

#if defined(Q_OS_WIN)
    QSettings mxKey("HKEY_CLASSES_ROOT\\karenmeu", RegFormat);
    mxKey.setValue(".", "URL:karenmeu Protocol");
    mxKey.setValue("URL Protocol", "");
    mxKey.sync();

    QSettings mxOpenKey("HKEY_CLASSES_ROOT\\karenmeu\\shell\\open\\command", RegFormat);
    mxOpenKey.setValue(".", QChar('"') + QDir::toNativeSeparators(QCoreApplication::applicationDirPath()) + QString("\\KarenMeu.exe\" \"%1\""));
    mxKey.sync();

    QStringList formats    = {"markdown"};
    QStringList extensions = {".markdown", ".md", ".mdown"};
    // assoiation for each extension
    foreach (const QString &fmt, formats)
    {
        QSettings settings(REGKEY_CLASSES, RegFormat);
        settings.beginGroup(QString(REGKEYFORMAT_ASSOCFILE).arg(fmt));
        settings.setValue(".", QObject::tr("Markdown Document"));
        settings.beginGroup("DefaultIcon");
        settings.setValue(".", QChar('"') + QDir::toNativeSeparators(QCoreApplication::applicationDirPath()) + QString("\\KarenMeu.exe\",1"));
        settings.endGroup();
        settings.beginGroup("shell");
        settings.beginGroup("open");
        settings.setValue(".", QObject::tr("&View with KarenMeu", "Menu displayed when right clicking on file in Explorer"));
        settings.beginGroup("command");
        settings.setValue(".", QDir::toNativeSeparators(QCoreApplication::applicationDirPath()));
        settings.endGroup();
        settings.endGroup();
        settings.endGroup();
        settings.endGroup();
        settings.sync();
    }

    // KarenMeu Capabilities
    QSettings settings(REGKEY_APPLICATION, RegFormat);
    settings.beginGroup("Capabilities");
    settings.setValue("ApplicationDescription", "KarenMeu for folders/archives");
    settings.setValue("ApplicationName", APPLICATION_ID);
    settings.beginGroup("FileAssociations");
    foreach (const QString &fmt, formats)
    {
        foreach (const QString &assoc, extensions)
            settings.setValue(assoc, QString(REGKEYFORMAT_ASSOCFILE).arg(fmt));
    }
    settings.endGroup();
    settings.endGroup();
    settings.sync();

    // RegisteredApplications
    QSettings settings2(REGKEY_REGISTEREDAPPLICATIONS, RegFormat);
    settings2.setValue(APPLICATION_ID, "Software\\" APPLICATION_ID "\\Capabilities");
    settings2.sync();

    // assoiation for application
    QSettings settings3(REGKEY_APPLICATION_INAPP, RegFormat);
    settings3.beginGroup("shell");
    settings3.beginGroup("open");
    settings3.setValue(".", QObject::tr("&View with KarenMeu", "Menu displayed when right clicking on file in Explorer"));
    settings3.beginGroup("command");
    settings3.setValue(".", QDir::toNativeSeparators(QCoreApplication::applicationDirPath()));
    settings3.endGroup();
    settings3.endGroup();
    settings3.endGroup();
    settings3.sync();

    IApplicationAssociationRegistrationUI *pAARUI = NULL;
    HRESULT                                hr     = ::CoCreateInstance(CLSID_ApplicationAssociationRegistrationUI,
                                    NULL,
                                    CLSCTX_INPROC,
                                    __uuidof(IApplicationAssociationRegistrationUI),
                                    reinterpret_cast<void **>(&pAARUI));

    if (SUCCEEDED(hr) && pAARUI != NULL)
    {
        hr = pAARUI->LaunchAdvancedAssociationUI(L"KarenMeu");
        pAARUI->Release();
    }
#endif

    return 0;
}
