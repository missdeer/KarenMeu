#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSharedPointer>
#include <QString>

#include "Theme.h"

class Settings
{
public:
    void save();
    void load();
    void initialize();

    const QString &codeEditorFontFamily() const;
    void           setCodeEditorFontFamily(const QString &codeEditorFontFamily);

    const QString &codeEditorTheme() const;
    void           setCodeEditorTheme(const QString &codeEditorTheme);

    int  autoRefreshInterval() const;
    void setAutoRefreshInterval(int autoRefreshInterval);

    int  editorZoomFactor() const;
    void setEditorZoomFactor(int editorZoomFactor);

    const QString &previewTheme() const;
    void           setPreviewTheme(const QString &previewTheme);

    const QString &codeBlockStyle() const;
    void           setCodeBlockStyle(const QString &codeBlockStyle);

    const QString &markdownEngine() const;
    void           setMarkdownEngine(const QString &markdownEngine);

    bool enableLineNumbers() const;
    void setEnableLineNumbers(bool enableLineNumbers);

    int  codeEditorFontPointSize() const;
    void setCodeEditorFontPointSize(int codeEditorFontPointSize);

    const QString &previewMode() const;
    void           setPreviewMode(const QString &previewMode);

    const QByteArray &customPreviewThemeStyle() const;
    void              setCustomPreviewThemeStyle(const QByteArray &customPreviewThemeStyle);

    Theme &theme();

    bool macTerminalStyleCodeBlock() const;
    void setMacTerminalStyleCodeBlock(bool macTerminalStyleCodeBlock);

    QByteArray mainWindowState();
    void       setMainWindowState(const QByteArray &state);

    QByteArray mainWindowGeometry();
    void       setMainWindowGeometry(const QByteArray &geometry);

private:
    bool m_enableLineNumbers {true};
    bool m_macTerminalStyleCodeBlock {true};
    int  m_autoRefreshInterval {1000}; // millisecond
    int  m_editorZoomFactor {100};     // percent
    int        m_codeEditorFontPointSize {14};
    QString    m_codeEditorFontFamily;
    QString    m_codeEditorTheme;
    QString    m_previewTheme;
    QString    m_codeBlockStyle;
    QString    m_markdownEngine;
    QString    m_previewMode;
    QByteArray m_customPreviewThemeStyle;
    Theme      m_theme;
};

inline QSharedPointer<Settings> g_settings;

#endif // SETTINGS_H
