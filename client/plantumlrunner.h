#ifndef PLANTUMLRUNNER_H
#define PLANTUMLRUNNER_H

#include <QObject>

class QProcess;

class PlantUMLRunner : public QObject
{
    Q_OBJECT
public:
    explicit PlantUMLRunner(QObject *parent = nullptr);

    void searchDefaultExecutablePaths();
    void updateJavaPath();
    void updateJavaVersion();
    void updatePlantUMLPath();
    void updatePlantUMLVersion();
    void updateGraphvizPath();
    void updateGraphvizVersion();
    bool isRunning();
    void checkPaths();
    void exportByPlantUML(const QString& saveAs, const QByteArray& doc);
    void exportByGraphviz(const QString& saveAs, const QByteArray& doc, const QString& engine);
    void runPlantUML(const QByteArray& doc, const QString &outputFormat);
    void runGraphviz(const QByteArray& doc, const QString& outputFormat, const QString& engine);
    bool hasPlantUMLStartEndMark(const QByteArray& doc);

    const QString& javaPath() const;
    void setJavaPath(const QString &javaPath);

    const QString& plantUmlPath() const;
    void setPlantUmlPath(const QString &plantUmlPath);

    const QString& graphvizPath() const;
    void setGraphvizPath(const QString &graphvizPath);

    bool hasValidPaths() const;
    void setHasValidPaths(bool hasValidPaths);

    const QByteArray &input() const;

    const QByteArray &output() const;

    const QString &cacheKey() const;
    void           setCacheKey(const QString &cacheKey);

signals:
    void exported(const QString& fileName);
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
