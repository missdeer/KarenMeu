#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSharedPointer>
#include <QString>

#include <vtextedit/markdowneditorconfig.h>
#include <vtextedit/vmarkdowneditor.h>
#include <vtextedit/vtexteditor.h>

#include "MarkdownEditorTheme.h"

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

    MarkdownEditorTheme &theme();

    bool macTerminalStyleCodeBlock() const;
    void setMacTerminalStyleCodeBlock(bool macTerminalStyleCodeBlock);

    QByteArray mainWindowState();
    void       setMainWindowState(const QByteArray &state);

    QByteArray mainWindowGeometry();
    void       setMainWindowGeometry(const QByteArray &geometry);

    QString cloudServerAddress();
    void    setCloudServerAddress(const QString &server);

    QString cloudUsername();
    void    setCloudUsername(const QString &username);

    QString cloudPassword();
    void    setCloudPassword(const QString &password);

    bool enableGoogleTranslate() const;
    void setEnableGoogleTranslate(bool enableGoogleTranslate);

    bool enableBaiduTranslate() const;
    void setEnableBaiduTranslate(bool enableBaiduTranslate);

    bool enableYoudaoTranslate() const;
    void setEnableYoudaoTranslate(bool enableYoudaoTranslate);

    bool enableSogouTranslate() const;
    void setEnableSogouTranslate(bool enableSogouTranslate);

    QSharedPointer<vte::TextEditorConfig> textEditorConfig() const;

    QSharedPointer<vte::MarkdownEditorConfig> markdownEditorConfig() const;

private:
    bool m_enableGoogleTranslate {true};
    bool m_enableBaiduTranslate {false};
    bool m_enableYoudaoTranslate {true};
    bool m_enableSogouTranslate {false};
    bool m_enableLineNumbers {true};
    bool m_macTerminalStyleCodeBlock {true};

    int m_autoRefreshInterval {1000}; // millisecond
    int m_editorZoomFactor {100};     // percent
    int m_codeEditorFontPointSize {14};

    QString m_codeEditorFontFamily;
    QString m_codeEditorTheme;
    QString m_previewTheme;
    QString m_codeBlockStyle;
    QString m_markdownEngine;
    QString m_previewMode;

    QByteArray m_customPreviewThemeStyle;

    MarkdownEditorTheme m_theme;

    QSharedPointer<vte::TextEditorConfig>     m_textEditorConfig;
    QSharedPointer<vte::MarkdownEditorConfig> m_markdownEditorConfig;
};

inline QSharedPointer<Settings> g_settings;

#endif // SETTINGS_H
