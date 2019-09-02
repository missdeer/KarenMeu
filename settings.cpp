#include <QSettings>
#include "settings.h"

Settings* g_settings = nullptr;

Settings::~Settings()
{
    save();
}

void Settings::initialize()
{
    load();
}

void Settings::save()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "minidump.info", "KarenMeu");
    settings.setValue("rememberLastSession", m_rememberLastSession);
    settings.setValue("autoRefreshInterval", m_autoRefreshInterval);
    settings.setValue("editorZoomFactor", m_editorZoomFactor);
    settings.setValue("defaultLanguageIndex", m_defaultLanguageIndex);
    settings.setValue("codeEditorFontFamily", m_codeEditorFontFamily);
    settings.setValue("codeEditorTheme", m_codeEditorTheme);
    settings.setValue("codeInspectorFontFamily", m_codeInspectorFontFamily);
    settings.setValue("codeInspectorTheme", m_codeInspectorTheme);
    settings.setValue("binary", m_binary);
    settings.setValue("label", m_label);
    settings.setValue("functions", m_functions);
    settings.setValue("directives", m_directives);
    settings.setValue("comments", m_comments);
    settings.setValue("trim", m_trim);
    settings.setValue("intel", m_intel);
    settings.setValue("demangle", m_demangle);
    settings.sync();
}

void Settings::load()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "minidump.info", "KarenMeu");
    m_rememberLastSession = settings.value("rememberLastSession", true).toBool();
    m_binary = settings.value("binary", false).toBool();
    m_label = settings.value("label", true).toBool();
    m_functions = settings.value("functions", true).toBool();
    m_directives = settings.value("directives", true).toBool();
    m_comments = settings.value("comments", true).toBool();
    m_trim = settings.value("trim", true).toBool();
    m_intel = settings.value("intel", true).toBool();
    m_demangle = settings.value("demangle", true).toBool();
    m_defaultLanguageIndex = settings.value("defaultLanguageIndex", 0).toInt();
#if defined(Q_OS_IOS) || defined(Q_OS_ANDROID)
    m_autoRefreshInterval = settings.value("autoRefreshInterval", 1500).toInt();
#else
    m_autoRefreshInterval = settings.value("autoRefreshInterval", 750).toInt();
#endif
    m_editorZoomFactor = settings.value("editorZoomFactor", 100).toInt();
    m_codeEditorFontFamily = settings.value("codeEditorFontFamily", QVariant("Source Code Pro")).toString();
    m_codeEditorTheme = settings.value("codeEditorTheme", QVariant("Default")).toString();
    m_codeInspectorFontFamily = settings.value("codeInspectorFontFamily", QVariant("Source Code Pro")).toString();
    m_codeInspectorTheme = settings.value("codeInspectorTheme", QVariant("Default")).toString();
}

const QString & Settings::codeEditorFontFamily() const
{
    return m_codeEditorFontFamily;
}

void Settings::setCodeEditorFontFamily(const QString &codeEditorFontFamily)
{
    m_codeEditorFontFamily = codeEditorFontFamily;
}

const QString & Settings::codeEditorTheme() const
{
    return m_codeEditorTheme;
}

void Settings::setCodeEditorTheme(const QString &codeEditorTheme)
{
    m_codeEditorTheme = codeEditorTheme;
}

const QString & Settings::codeInspectorFontFamily() const
{
    return m_codeInspectorFontFamily;
}

void Settings::setCodeInspectorFontFamily(const QString &codeInspectorFontFamily)
{
    m_codeInspectorFontFamily = codeInspectorFontFamily;
}

const QString & Settings::codeInspectorTheme() const
{
    return m_codeInspectorTheme;
}

void Settings::setCodeInspectorTheme(const QString &codeInspectorTheme)
{
    m_codeInspectorTheme = codeInspectorTheme;
}

bool Settings::rememberLastSession() const
{
    return m_rememberLastSession;
}

void Settings::setRememberLastSession(bool rememberLastSession)
{
    m_rememberLastSession = rememberLastSession;
}

int Settings::defaultLanguageIndex() const
{
    return m_defaultLanguageIndex;
}

void Settings::setDefaultLanguageIndex(int defaultLanguageIndex)
{
    m_defaultLanguageIndex = defaultLanguageIndex;
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

bool Settings::binary() const
{
    return m_binary;
}

void Settings::setBinary(bool binary)
{
    m_binary = binary;
}

bool Settings::label() const
{
    return m_label;
}

void Settings::setLabel(bool label)
{
    m_label = label;
}

bool Settings::functions() const
{
    return m_functions;
}

void Settings::setFunctions(bool functions)
{
    m_functions = functions;
}

bool Settings::directives() const
{
    return m_directives;
}

void Settings::setDirectives(bool directives)
{
    m_directives = directives;
}

bool Settings::comments() const
{
    return m_comments;
}

void Settings::setComments(bool comments)
{
    m_comments = comments;
}

bool Settings::trim() const
{
    return m_trim;
}

void Settings::setTrim(bool trim)
{
    m_trim = trim;
}

bool Settings::intel() const
{
    return m_intel;
}

void Settings::setIntel(bool intel)
{
    m_intel = intel;
}

bool Settings::demangle() const
{
    return m_demangle;
}

void Settings::setDemangle(bool demangle)
{
    m_demangle = demangle;
}




