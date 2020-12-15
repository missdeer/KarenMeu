#ifndef THEME_FACTORY_H
#define THEME_FACTORY_H

#include <QDir>
#include <QList>
#include <QString>
#include <QStringList>

#include "MarkdownEditorTheme.h"

class QSettings;

/**
 * Singleton class to fetch themes, either built-in or from the hard disk.
 */
class MarkdownEditorThemeFactory
{
public:
    /**
     * Gets the single instance of this class.
     */
    static MarkdownEditorThemeFactory *getInstance();

    /**
     * Gets the list of available theme names, including built-in themes
     * (listed at the front of the list).
     */
    QStringList getAvailableThemes() const;

    /**
     * Gets the default theme.
     */
    MarkdownEditorTheme getDefaultTheme() const;

    /**
     * Returns the theme with the given name.  If an error occurs, the
     * err string will be populated with an error message.  In this event,
     * the theme return will be one of the built-in themes.  If no error
     * occurs while loading the desired theme, err will be set to a null
     * QString.
     */
    MarkdownEditorTheme loadTheme(const QString &name, QString &err) const;

    /**
     * Deletes the theme with the given name from the hard disk.  Note
     * that this operation results in an error for built-in themes.  If an
     * error occurs while attempting to delete the theme, the err string
     * will be populated with an error message string.  Otherwise, err will
     * be set to a null QString.
     */
    void deleteTheme(const QString &name, QString &err);

    /**
     * Saves the theme with the given name to the hard disk.  Note
     * that this operation results in an error for built-in themes.  If an
     * error occurs while attempting to save the theme, the err string
     * will be populated with an error message string.  Otherwise, err will
     * be set to a null QString.
     */
    void saveTheme(const QString &name, MarkdownEditorTheme &theme, QString &err);

    /**
     * Gets the theme storage location directory.
     */
    QDir getThemeDirectory() const;

    /**
     * Gets the directory path for storing the theme with the given name.
     */
    QDir getDirectoryForTheme(const QString &name) const;

    /**
     * Returns an untitled theme name that is unique.
     */
    QString generateUntitledThemeName() const;

    static const QString CLASSIC_LIGHT_THEME_NAME;
    static const QString CLASSIC_DARK_THEME_NAME;
    static const QString PLAINSTRACTION_LIGHT_THEME_NAME;
    static const QString PLAINSTRACTION_DARK_THEME_NAME;

private:
    static MarkdownEditorThemeFactory *instance;

    QList<MarkdownEditorTheme> builtInThemes;
    QStringList  customThemeNames;
    QString      themeDirectoryPath;
    QDir         themeDirectory;

    MarkdownEditorThemeFactory();

    /**
     * Destructor.
     */
    ~MarkdownEditorThemeFactory() = default;

    void loadClassicLightTheme();
    void loadClassicDarkTheme();
    void loadPlainstractionLightTheme();
    void loadPlainstractionDarkTheme();

    bool extractColorSetting(const QSettings &settings, const QString &key, QColor &value, QString &err) const;

    bool extractIntSetting(const QSettings &settings, const QString &key, int &value, int min, int max, QString &err) const;

    bool extractStringSetting(const QSettings &settings, const QString &key, QString &value, QString &err) const;
};

#endif
