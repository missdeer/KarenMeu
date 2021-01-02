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

    bool useCustomJava() const;
    void setUseCustomJava(bool useCustomJava);

    bool useCustomPlantUml() const;
    void setUseCustomPlantUml(bool useCustomPlantUml);

    bool useCustomGraphiz() const;
    void setUseCustomGraphiz(bool useCustomGraphiz);

    const QString& javaPath() const;
    void setJavaPath(const QString &javaPath);

    const QString& plantUmlPath() const;
    void setPlantUmlPath(const QString &plantUmlPath);

    const QString& graphvizPath() const;
    void setGraphvizPath(const QString &graphvizPath);

    QString customJavaPath() const;
    void setCustomJavaPath(const QString &customJavaPath);

    QString customPlantUmlPath() const;
    void setCustomPlantUmlPath(const QString &customPlantUmlPath);

    QString customGraphizPath() const;
    void setCustomGraphizPath(const QString &customGraphizPath);

    QString plantUmlVersion() const;
    void setPlantUmlVersion(const QString &plantUmlVersion);

    QString javaVersion() const;
    void setJavaVersion(const QString &javaVersion);

    QString graphvizVersion() const;
    void setGraphvizVersion(const QString &graphvizVersion);

    QString remoteServerAddress() const;
    void setRemoteServerAddress(const QString &remoteServerAddress);

    bool hasValidPaths() const;
    void setHasValidPaths(bool hasValidPaths);

    bool useRemoteServerFirst() const;
    void setUseRemoteServerFirst(bool useRemoteServerFirst);

signals:
    void exported(const QString& fileName);
    void result(const QByteArray& res);
private slots:
    void exportFinished();
    void runFinished();
private:
    QProcess *m_process {nullptr};
    bool      m_useCustomJava {false};
    bool      m_useCustomPlantUml {false};
    bool      m_useCustomGraphiz {false};

    QString m_javaPath;
    QString m_plantUmlPath;
    QString m_graphvizPath;

    QString m_customJavaPath;
    QString m_customPlantUmlPath;
    QString m_customGraphizPath;

    QString m_plantUmlVersion;
    QString m_javaVersion;
    QString m_graphvizVersion;

    bool    m_useRemoteServerFirst {false};
    QString m_remoteServerAddress;

    bool m_hasValidPaths {false};

    QString m_saveAs;
};

#endif // PLANTUMLRUNNER_H
