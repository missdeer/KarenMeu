#include <QtCore>
#include <QSettings>
#include <QFile>
#include "settings.h"

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
    settings.setValue("autoRefreshInterval", m_autoRefreshInterval);
    settings.setValue("editorZoomFactor", m_editorZoomFactor);
    settings.setValue("codeEditorFontFamily", m_codeEditorFontFamily);
    settings.setValue("codeEditorTheme", m_codeEditorTheme);
    settings.setValue("previewTheme", m_previewTheme);
    settings.setValue("codeBlockStyle", m_codeBlockStyle);
    settings.setValue("markdownEngine", m_markdownEngine);
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
    m_editorZoomFactor = settings.value("editorZoomFactor", 100).toInt();
    m_codeEditorFontFamily = settings.value("codeEditorFontFamily", QString("Source Code Pro")).toString();
    m_codeEditorTheme = settings.value("codeEditorTheme", QString("Default")).toString();
    setPreviewTheme(settings.value("previewTheme", QString("默认")).toString());
    m_codeBlockStyle = settings.value("codeBlockStyle", QString("xcode")).toString();
    m_markdownEngine = settings.value("markdownEngine", QString("Goldmark")).toString();
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
    QMap<QString, QString> m = {
        { "墨黑", "black.css" },
        { "姹紫", "purple.css" },
        { "嫩青", "blue.css" },
        { "橙心", "orange.css" },
        { "红绯", "red.css" },
        { "绿意", "green.css" },
        { "默认", "default.css" },
        };
    QFile f(":/rc/theme/" + m[m_previewTheme]);
    if (f.open(QIODevice::ReadOnly))
    {
        m_previewThemeContent = f.readAll();
        f.close();
    }
}

const QString &Settings::codeBlockStyle() const
{
    return m_codeBlockStyle;
}

void Settings::setCodeBlockStyle(const QString &codeBlockStyle)
{
    m_codeBlockStyle = codeBlockStyle;
}

QByteArray Settings::previewThemeContent() const
{
    return m_previewThemeContent;
}

const QString &Settings::markdownEngine() const
{
    return m_markdownEngine;
}

void Settings::setMarkdownEngine(const QString &markdownEngine)
{
    m_markdownEngine = markdownEngine;
}



