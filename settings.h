#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>

class Settings
{
public:
    Settings() = default;
    Settings(const Settings&) = delete;
    void operator=(const Settings&) = delete;
    Settings(Settings&&) = delete;
    void operator=(Settings&&) = delete;
    ~Settings();

    void save();
    void load();
    void initialize();

    const QString & codeEditorFontFamily() const;
    void setCodeEditorFontFamily(const QString &codeEditorFontFamily);

    const QString & codeEditorTheme() const;
    void setCodeEditorTheme(const QString &codeEditorTheme);

    const QString & codeInspectorFontFamily() const;
    void setCodeInspectorFontFamily(const QString &codeInspectorFontFamily);

    const QString & codeInspectorTheme() const;
    void setCodeInspectorTheme(const QString &codeInspectorTheme);

    bool rememberLastSession() const;
    void setRememberLastSession(bool rememberLastSession);

    int defaultLanguageIndex() const;
    void setDefaultLanguageIndex(int defaultLanguageIndex);

    int autoRefreshInterval() const;
    void setAutoRefreshInterval(int autoRefreshInterval);

    int editorZoomFactor() const;
    void setEditorZoomFactor(int editorZoomFactor);

    bool binary() const;
    void setBinary(bool binary);
    
    bool label() const;
    void setLabel(bool label);
    
    bool functions() const;
    void setFunctions(bool functions);
    
    bool directives() const;
    void setDirectives(bool directives);
    
    bool comments() const;
    void setComments(bool comments);
    
    bool trim() const;
    void setTrim(bool trim);
    
    bool intel() const;
    void setIntel(bool intel);
    
    bool demangle() const;
    void setDemangle(bool demangle);
    
private:
    bool m_rememberLastSession;
    bool m_binary;
    bool m_label;
    bool m_functions;
    bool m_directives;
    bool m_comments;
    bool m_trim;
    bool m_intel;
    bool m_demangle;
    int m_defaultLanguageIndex;
    int m_autoRefreshInterval; // millisecond
    int m_editorZoomFactor; // percent

    QString m_codeEditorFontFamily;
    QString m_codeEditorTheme;
    QString m_codeInspectorFontFamily;
    QString m_codeInspectorTheme;
};

extern Settings *g_settings;

#endif // SETTINGS_H
