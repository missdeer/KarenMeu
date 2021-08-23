#include <QFile>
#include <QSettings>
#include <QtCore>

#include "settings.h"

#include <vtextedit/vmarkdowneditor.h>
#include <vtextedit/vtexteditor.h>

void Settings::initialize()
{
    load();

    vte::VTextEditor::addSyntaxCustomSearchPaths(QStringList(QStringLiteral(":/rc/syntax-highlighting")));

    m_textEditorConfig                                            = QSharedPointer<vte::TextEditorConfig>::create();
    m_textEditorConfig->m_centerCursor                            = vte::CenterCursor::CenterOnBottom;
    m_markdownEditorConfig                                        = QSharedPointer<vte::MarkdownEditorConfig>::create(m_textEditorConfig);
    m_markdownEditorConfig->m_constrainInplacePreviewWidthEnabled = true;
    m_textEditorParameters                                        = QSharedPointer<vte::TextEditorParameters>::create();
}

Settings::~Settings()
{
    auto &settings = getSettings();
    delete &settings;
}

void Settings::save()
{
    auto &settings = getSettings();
    settings.setValue("autoRefreshInterval", m_autoRefreshInterval);
    settings.setValue("editorZoomFactor", m_editorZoomFactor);
    settings.setValue("codeEditorFontFamily", m_codeEditorFontFamily);
    settings.setValue("codeEditorFontPointSize", m_codeEditorFontPointSize);
    settings.setValue("previewTheme", m_previewTheme);
    settings.setValue("codeBlockStyle", m_codeBlockStyle);
    settings.setValue("markdownEngine", m_markdownEngine);
    settings.setValue("enableLineNumbers", m_enableLineNumbers);
    settings.setValue("macTerminalStyleCodeBlock", m_macTerminalStyleCodeBlock);
    settings.setValue("previewMode", m_previewMode);
    settings.setValue("customPreviewThemeStyle", m_customPreviewThemeStyle);
    settings.setValue("googleTranslate", m_enableGoogleTranslate);
    settings.setValue("baiduTranslate", m_enableBaiduTranslate);
    settings.setValue("youdaoTranslate", m_enableYoudaoTranslate);
    settings.setValue("sogouTranslate", m_enableSogouTranslate);
    settings.setValue("deepLTranslate", m_enableDeepLTranslate);
    settings.setValue("translateTimeout", m_translateTimeout);
    settings.setValue("lastOpenedFilePath", m_lastOpenedFilePath);
    settings.sync();
}

void Settings::load()
{
    auto &settings = getSettings();
#if defined(Q_OS_IOS) || defined(Q_OS_ANDROID)
    m_autoRefreshInterval = settings.value("autoRefreshInterval", 1500).toInt();
#else
    m_autoRefreshInterval = settings.value("autoRefreshInterval", 1000).toInt();
#endif
    m_editorZoomFactor     = settings.value("editorZoomFactor", 100).toInt();
    m_codeEditorFontFamily = settings
                                 .value("codeEditorFontFamily",
#if defined(Q_OS_MAC) || defined(Q_OS_IOS)
                                        QString("Menlo")
#elif defined(Q_OS_WIN)
                                        QString("Consolas")
#elif defined(Q_OS_ANDROID)
                                        QString("Droid Sans Mono")
#else
                                        QString("Monospace")
#endif
                                            )
                                 .toString();
    m_codeEditorFontPointSize = settings.value("codeEditorFontPointSize", 14).toInt();
    setPreviewTheme(settings.value("previewTheme", QSettings::tr("Default")).toString());
    m_codeBlockStyle          = settings.value("codeBlockStyle", QSettings::tr("xcode")).toString();
    m_markdownEngine          = settings.value("markdownEngine", QSettings::tr("Goldmark")).toString();
    m_enableLineNumbers         = settings.value("enableLineNumbers", true).toBool();
    m_macTerminalStyleCodeBlock = settings.value("macTerminalStyleCodeBlock", true).toBool();
    m_previewMode               = settings.value("previewMode", QSettings::tr("Wechat Public Account Article")).toString();
    m_customPreviewThemeStyle = settings.value("customPreviewThemeStyle").toByteArray();
    m_enableGoogleTranslate     = settings.value("googleTranslate", true).toBool();
    m_enableBaiduTranslate      = settings.value("baiduTranslate", false).toBool();
    m_enableYoudaoTranslate     = settings.value("youdaoTranslate", true).toBool();
    m_enableSogouTranslate      = settings.value("sogouTranslate", false).toBool();
    m_enableDeepLTranslate      = settings.value("deepLTranslate", true).toBool();
    m_translateTimeout          = settings.value("translateTimeout", 3000).toInt();
    m_lastOpenedFilePath        = settings.value("lastOpenedFilePath").toString();
}

const QString &Settings::codeEditorFontFamily() const
{
    return m_codeEditorFontFamily;
}

void Settings::setCodeEditorFontFamily(const QString &codeEditorFontFamily)
{
    m_codeEditorFontFamily = codeEditorFontFamily;
}

const QString &Settings::codeEditorTheme() const
{
    return m_codeEditorTheme;
}

void Settings::setCodeEditorTheme(const QString &codeEditorTheme)
{
    m_codeEditorTheme = codeEditorTheme;
}

int Settings::autoRefreshInterval() const
{
    return m_autoRefreshInterval;
}

void Settings::setAutoRefreshInterval(int autoRefreshInterval)
{
    m_autoRefreshInterval = autoRefreshInterval;
}

int Settings::editorZoomFactor() const
{
    return m_editorZoomFactor;
}

void Settings::setEditorZoomFactor(int editorZoomFactor)
{
    m_editorZoomFactor = editorZoomFactor;
}

const QString &Settings::previewTheme() const
{
    return m_previewTheme;
}

void Settings::setPreviewTheme(const QString &previewTheme)
{
    m_previewTheme = previewTheme;
}

const QString &Settings::codeBlockStyle() const
{
    return m_codeBlockStyle;
}

void Settings::setCodeBlockStyle(const QString &codeBlockStyle)
{
    m_codeBlockStyle = codeBlockStyle;
}

const QString &Settings::markdownEngine() const
{
    return m_markdownEngine;
}

void Settings::setMarkdownEngine(const QString &markdownEngine)
{
    m_markdownEngine = markdownEngine;
}

bool Settings::enableLineNumbers() const
{
    return m_enableLineNumbers;
}

void Settings::setEnableLineNumbers(bool enableLineNumbers)
{
    m_enableLineNumbers = enableLineNumbers;
}

int Settings::codeEditorFontPointSize() const
{
    return m_codeEditorFontPointSize;
}

void Settings::setCodeEditorFontPointSize(int codeEditorFontPointSize)
{
    m_codeEditorFontPointSize = codeEditorFontPointSize;
}

const QString &Settings::previewMode() const
{
    return m_previewMode;
}

void Settings::setPreviewMode(const QString &previewMode)
{
    m_previewMode = previewMode;
}

const QByteArray &Settings::customPreviewThemeStyle() const
{
    return m_customPreviewThemeStyle;
}

void Settings::setCustomPreviewThemeStyle(const QByteArray &customPreviewThemeStyle)
{
    m_customPreviewThemeStyle = customPreviewThemeStyle;
}

bool Settings::macTerminalStyleCodeBlock() const
{
    return m_macTerminalStyleCodeBlock;
}

void Settings::setMacTerminalStyleCodeBlock(bool macTerminalStyleCodeBlock)
{
    m_macTerminalStyleCodeBlock = macTerminalStyleCodeBlock;
}

QByteArray Settings::mainWindowState()
{
    auto &settings = getSettings();
    return settings.value("mainWindowState").toByteArray();
}

void Settings::setMainWindowState(const QByteArray &state)
{
    auto &settings = getSettings();
    settings.setValue("mainWindowState", state);
    settings.sync();
}

QByteArray Settings::mainWindowGeometry()
{
    auto &settings = getSettings();
    return settings.value("mainWindowGeometry").toByteArray();
}

void Settings::setMainWindowGeometry(const QByteArray &geometry)
{
    auto &settings = getSettings();
    settings.setValue("mainWindowGeometry", geometry);
    settings.sync();
}

QString Settings::cloudServerAddress()
{
    auto &settings = getSettings();
    return settings.value("cloudServerAddress").toString();
}

void Settings::setCloudServerAddress(const QString &server)
{
    auto &settings = getSettings();
    settings.setValue("cloudServerAddress", server);
    settings.sync();
}

QString Settings::cloudUsername()
{
    auto &settings = getSettings();
    return settings.value("cloudUsername").toString();
}

void Settings::setCloudUsername(const QString &username)
{
    auto &settings = getSettings();
    settings.setValue("cloudUsername", username);
    settings.sync();
}

QString Settings::cloudPassword()
{
    auto &settings = getSettings();
    return settings.value("cloudPassword").toString();
}

void Settings::setCloudPassword(const QString &password)
{
    auto &settings = getSettings();
    settings.setValue("cloudPassword", password);
    settings.sync();
}

bool Settings::enableGoogleTranslate() const
{
    return m_enableGoogleTranslate;
}

void Settings::setEnableGoogleTranslate(bool enableGoogleTranslate)
{
    m_enableGoogleTranslate = enableGoogleTranslate;
}

bool Settings::enableBaiduTranslate() const
{
    return m_enableBaiduTranslate;
}

void Settings::setEnableBaiduTranslate(bool enableBaiduTranslate)
{
    m_enableBaiduTranslate = enableBaiduTranslate;
}

bool Settings::enableYoudaoTranslate() const
{
    return m_enableYoudaoTranslate;
}

void Settings::setEnableYoudaoTranslate(bool enableYoudaoTranslate)
{
    m_enableYoudaoTranslate = enableYoudaoTranslate;
}

bool Settings::enableSogouTranslate() const
{
    return m_enableSogouTranslate;
}

void Settings::setEnableSogouTranslate(bool enableSogouTranslate)
{
    m_enableSogouTranslate = enableSogouTranslate;
}

QSharedPointer<vte::TextEditorConfig> Settings::textEditorConfig() const
{
    return m_textEditorConfig;
}

QSharedPointer<vte::MarkdownEditorConfig> Settings::markdownEditorConfig() const
{
    return m_markdownEditorConfig;
}

QSharedPointer<vte::TextEditorParameters> Settings::textEditorParameters() const
{
    return m_textEditorParameters;
}

int Settings::translateTimeout() const
{
    return m_translateTimeout;
}

void Settings::setTranslateTimeout(int translateTimeout)
{
    m_translateTimeout = translateTimeout;
}

bool Settings::enableDeepLTranslate() const
{
    return m_enableDeepLTranslate;
}

void Settings::setEnableDeepLTranslate(bool enableDeepLTranslate)
{
    m_enableDeepLTranslate = enableDeepLTranslate;
}

const QString &Settings::getLastOpenedFilePath() const
{
    return m_lastOpenedFilePath;
}

void Settings::setLastOpenedFilePath(const QString &lastOpenedFilePath)
{
    m_lastOpenedFilePath = lastOpenedFilePath;
}

QString Settings::plantUMLRemoteServiceAddress()
{
    auto &settings = getSettings();
    return settings.value("plantUMLRemoteService", "http://www.plantuml.com/").toString();
}

void Settings::setPlantUMLRemoteService(const QString &path)
{
    auto &settings = getSettings();
    settings.setValue("plantUMLRemoteService", path);
    settings.sync();
}

QString Settings::javaPath()
{
    auto &settings = getSettings();
    return settings.value("javaPath").toString();
}

void Settings::setJavaPath(const QString &path)
{
    auto &settings = getSettings();
    settings.setValue("javaPath", path);
    settings.sync();
}

QString Settings::dotPath()
{
    auto &settings = getSettings();
    return settings.value("dotPath").toString();
}

void Settings::setDotPath(const QString &path)
{
    auto &settings = getSettings();
    settings.setValue("dotPath", path);
    settings.sync();
}

QString Settings::plantUMLJarPath()
{
    auto &settings = getSettings();
    return settings.value("plantUMLJarPath").toString();
}

void Settings::setPlantUMLJarPath(const QString &path)
{
    auto &settings = getSettings();
    settings.setValue("plantUMLJarPath", path);
    settings.sync();
}

bool Settings::plantUMLRemoteServiceEnabled()
{
    auto &settings = getSettings();
    return settings.value("plantUMLRemoteServiceEnabled", true).toBool();
}

void Settings::setPlantUMLRemoteServiceEnabled(bool enabled)
{
    auto &settings = getSettings();
    settings.setValue("plantUMLRemoteServiceEnabled", enabled);
    settings.sync();
}

bool Settings::plantUMLLocalJarEnabled()
{
    auto &settings = getSettings();
    return settings.value("plantUMLLocalJarEnabled", false).toBool();
}

void Settings::setPlantUMLLocalJarEnabled(bool enabled)
{
    auto &settings = getSettings();
    settings.setValue("plantUMLLocalJarEnabled", enabled);
    settings.sync();
}

QSettings &Settings::getSettings()
{
    static QSettings *settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "minidump.info", "KarenMeu");
    return *settings;
}
