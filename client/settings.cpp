#include <QFile>
#include <QSettings>
#include <QtCore>

#include "settings.h"
#include "ThemeFactory.h"

void Settings::initialize()
{
    load();
}

void Settings::save()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "minidump.info", "KarenMeu");
    settings.setValue("autoRefreshInterval", m_autoRefreshInterval);
    settings.setValue("editorZoomFactor", m_editorZoomFactor);
    settings.setValue("codeEditorFontFamily", m_codeEditorFontFamily);
    settings.setValue("codeEditorFontPointSize", m_codeEditorFontPointSize);
    settings.setValue("codeEditorTheme", m_codeEditorTheme);
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
    settings.sync();
}

void Settings::load()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "minidump.info", "KarenMeu");
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
    m_codeEditorTheme         = settings.value("codeEditorTheme", ThemeFactory::PLAINSTRACTION_LIGHT_THEME_NAME).toString();
    QString err;
    m_theme = ThemeFactory::getInstance()->loadTheme(m_codeEditorTheme, err);
    setPreviewTheme(settings.value("previewTheme", QSettings::tr("默认")).toString());
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

Theme &Settings::theme()
{
    return m_theme;
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
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "minidump.info", "KarenMeu");
    return settings.value("mainWindowState").toByteArray();
}

void Settings::setMainWindowState(const QByteArray &state)
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "minidump.info", "KarenMeu");
    settings.setValue("mainWindowState", state);
    settings.sync();
}

QByteArray Settings::mainWindowGeometry()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "minidump.info", "KarenMeu");
    return settings.value("mainWindowGeometry").toByteArray();
}

void Settings::setMainWindowGeometry(const QByteArray &geometry)
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "minidump.info", "KarenMeu");
    settings.setValue("mainWindowGeometry", geometry);
    settings.sync();
}

QString Settings::cloudServerAddress()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "minidump.info", "KarenMeu");
    return settings.value("cloudServerAddress").toString();
}

void Settings::setCloudServerAddress(const QString &server)
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "minidump.info", "KarenMeu");
    settings.setValue("cloudServerAddress", server);
    settings.sync();
}

QString Settings::cloudUsername()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "minidump.info", "KarenMeu");
    return settings.value("cloudUsername").toString();
}

void Settings::setCloudUsername(const QString &username)
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "minidump.info", "KarenMeu");
    settings.setValue("cloudUsername", username);
    settings.sync();
}

QString Settings::cloudPassword()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "minidump.info", "KarenMeu");
    return settings.value("cloudPassword").toString();
}

void Settings::setCloudPassword(const QString &password)
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "minidump.info", "KarenMeu");
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
