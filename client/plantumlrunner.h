#ifndef PLANTUMLRUNNER_H
#define PLANTUMLRUNNER_H

#include <QObject>

class QProcess;

class PlantUMLRunner : public QObject
{
    Q_OBJECT
public:
    explicit PlantUMLRunner(QObject *parent = nullptr);

    void                      searchDefaultExecutablePaths();
    void                      updateJavaPath();
    void                      updateJavaVersion();
    void                      updatePlantUMLPath();
    void                      updatePlantUMLVersion();
    void                      updateGraphvizPath();
    void                      updateGraphvizVersion();
    [[nodiscard]] bool        isRunning();
    void                      checkPaths();
    void                      exportByPlantUML(const QString &saveAs, const QByteArray &doc);
    void                      exportByGraphviz(const QString &saveAs, const QByteArray &doc, const QString &engine);
    void                      runPlantUML(const QByteArray &doc, const QString &outputFormat);
    void                      runGraphviz(const QByteArray &doc, const QString &outputFormat, const QString &engine);
    [[nodiscard]] static bool hasPlantUMLStartEndMark(const QByteArray &doc);

    [[nodiscard]] const QString &javaPath() const;
    void                         setJavaPath(const QString &javaPath);

    [[nodiscard]] const QString &plantUmlPath() const;
    void                         setPlantUmlPath(const QString &plantUmlPath);

    [[nodiscard]] const QString &graphvizPath() const;
    void                         setGraphvizPath(const QString &graphvizPath);

    [[nodiscard]] bool hasValidPaths() const;
    void               setHasValidPaths(bool hasValidPaths);

    [[nodiscard]] const QByteArray &input() const;

    [[nodiscard]] const QByteArray &output() const;

    [[nodiscard]] const QString &cacheKey() const;
    void                         setCacheKey(const QString &cacheKey);

signals:
    void exported(const QString &fileName);
    void result();
private slots:
    void exportFinished();
    void runFinished();

private:
    QProcess *m_process {nullptr};

    QString m_javaPath;
    QString m_plantUmlPath;
    QString m_graphvizPath;

    bool m_hasValidPaths {false};

    QString    m_saveAs;
    QByteArray m_input;
    QByteArray m_output;
    QString    m_cacheKey;
};

#endif // PLANTUMLRUNNER_H
