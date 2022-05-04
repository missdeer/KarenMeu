#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSharedPointer>
#include <QString>

#include <vtextedit/markdowneditorconfig.h>

QT_FORWARD_DECLARE_CLASS(QSettings);

struct WebDAVInfo
{
    QString server;
    QString user;
    QString password;
};

class Settings
{
public:
    Settings()                 = default;
    Settings(const Settings &) = delete;
    Settings(Settings &&)      = delete;
    void operator=(const Settings &) = delete;
    void operator=(Settings &&) = delete;
    ~Settings();
    void save();
    void load();
    void                     initialize();
    [[nodiscard]] QSettings &getSettings();

    [[nodiscard]] int markdownViewArrange();
    void              setMarkdownViewArrange(int mode);

    [[nodiscard]] const QString &codeEditorFontFamily() const;
    void                         setCodeEditorFontFamily(const QString &codeEditorFontFamily);

    [[nodiscard]] const QString &codeEditorTheme() const;
    void                         setCodeEditorTheme(const QString &codeEditorTheme);

    [[nodiscard]] int autoRefreshInterval() const;
    void              setAutoRefreshInterval(int autoRefreshInterval);

    [[nodiscard]] int editorZoomFactor() const;
    void              setEditorZoomFactor(int editorZoomFactor);

    [[nodiscard]] const QString &previewTheme() const;
    void                         setPreviewTheme(const QString &previewTheme);

    [[nodiscard]] const QString &codeBlockStyle() const;
    void                         setCodeBlockStyle(const QString &codeBlockStyle);

    [[nodiscard]] const QString &markdownEngine() const;
    void                         setMarkdownEngine(const QString &markdownEngine);

    [[nodiscard]] bool enableLineNumbers() const;
    void               setEnableLineNumbers(bool enableLineNumbers);

    [[nodiscard]] int codeEditorFontPointSize() const;
    void              setCodeEditorFontPointSize(int codeEditorFontPointSize);

    [[nodiscard]] const QString &previewMode() const;
    void                         setPreviewMode(const QString &previewMode);

    [[nodiscard]] const QByteArray &customPreviewThemeStyle() const;
    void                            setCustomPreviewThemeStyle(const QByteArray &customPreviewThemeStyle);

    [[nodiscard]] bool macTerminalStyleCodeBlock() const;
    void               setMacTerminalStyleCodeBlock(bool macTerminalStyleCodeBlock);

    [[nodiscard]] QByteArray mainWindowState();
    void                     setMainWindowState(const QByteArray &state);

    [[nodiscard]] QByteArray mainWindowGeometry();
    void                     setMainWindowGeometry(const QByteArray &geometry);

    [[nodiscard]] bool enableGoogleTranslate() const;
    void               setEnableGoogleTranslate(bool enableGoogleTranslate);

    [[nodiscard]] bool enableBaiduTranslate() const;
    void setEnableBaiduTranslate(bool enableBaiduTranslate);

    [[nodiscard]] bool enableYoudaoTranslate() const;
    void               setEnableYoudaoTranslate(bool enableYoudaoTranslate);

    [[nodiscard]] bool enableSogouTranslate() const;
    void               setEnableSogouTranslate(bool enableSogouTranslate);

    [[nodiscard]] QSharedPointer<vte::TextEditorConfig> textEditorConfig() const;

    [[nodiscard]] QSharedPointer<vte::MarkdownEditorConfig> markdownEditorConfig() const;

    [[nodiscard]] QSharedPointer<vte::TextEditorParameters> textEditorParameters() const;

    [[nodiscard]] int translateTimeout() const;
    void              setTranslateTimeout(int translateTimeout);

    [[nodiscard]] bool enableDeepLTranslate() const;
    void               setEnableDeepLTranslate(bool enableDeepLTranslate);

    [[nodiscard]] const QString &getLastOpenedFilePath() const;
    void                         setLastOpenedFilePath(const QString &lastOpenedFilePath);

    [[nodiscard]] QString plantUMLRemoteServiceAddress();
    void                  setPlantUMLRemoteService(const QString &path);
    [[nodiscard]] QString javaPath();
    void                  setJavaPath(const QString &path);
    [[nodiscard]] QString dotPath();
    void                  setDotPath(const QString &path);
    [[nodiscard]] QString plantUMLJarPath();
    void                  setPlantUMLJarPath(const QString &path);

    [[nodiscard]] bool plantUMLRemoteServiceEnabled();
    void               setPlantUMLRemoteServiceEnabled(bool enabled);
    [[nodiscard]] bool plantUMLLocalJarEnabled();
    void               setPlantUMLLocalJarEnabled(bool enabled);

    [[nodiscard]] const QVector<WebDAVInfo> &webDAVInfos() const;
    void                                     setWebDAVInfos(const QVector<WebDAVInfo> &newWebDAVInfos);

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

    QVector<WebDAVInfo>                       m_webDAVInfos;
    QSharedPointer<vte::TextEditorConfig>     m_textEditorConfig;
    QSharedPointer<vte::MarkdownEditorConfig> m_markdownEditorConfig;
    QSharedPointer<vte::TextEditorParameters> m_textEditorParameters;
};

inline QSharedPointer<Settings> g_settings;

#endif // SETTINGS_H
