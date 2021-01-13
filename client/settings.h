#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSharedPointer>
#include <QString>

#include <vtextedit/markdowneditorconfig.h>

QT_FORWARD_DECLARE_CLASS(QSettings);

class Settings
{
public:
    ~Settings();
    void save();
    void load();
    void initialize();
    QSettings &getSettings();

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

    int  translateTimeout() const;
    void setTranslateTimeout(int translateTimeout);

    bool enableDeepLTranslate() const;
    void setEnableDeepLTranslate(bool enableDeepLTranslate);

    const QString &getLastOpenedFilePath() const;
    void           setLastOpenedFilePath(const QString &lastOpenedFilePath);

private:
    bool m_enableGoogleTranslate {true};
    bool m_enableBaiduTranslate {false};
    bool m_enableYoudaoTranslate {true};
    bool m_enableSogouTranslate {false};
    bool m_enableDeepLTranslate {true};
    bool m_enableLineNumbers {true};
    bool m_macTerminalStyleCodeBlock {true};

    int m_autoRefreshInterval {1000}; // millisecond
    int m_editorZoomFactor {100};     // percent
    int m_codeEditorFontPointSize {14};
    int m_translateTimeout {3000}; // millisecond

    QString m_codeEditorFontFamily;
    QString m_codeEditorTheme;
    QString m_previewTheme;
    QString m_codeBlockStyle;
    QString m_markdownEngine;
    QString m_previewMode;
    QString m_lastOpenedFilePath;

    QByteArray m_customPreviewThemeStyle;

    QSharedPointer<vte::TextEditorConfig>     m_textEditorConfig;
    QSharedPointer<vte::MarkdownEditorConfig> m_markdownEditorConfig;
};

inline QSharedPointer<Settings> g_settings;

#endif // SETTINGS_H
