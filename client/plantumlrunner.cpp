#include <QCoreApplication>
#include <QFile>
#include <QProcess>
#include <QtCore>

#include "plantumlrunner.h"

#include "utils.h"

PlantUMLRunner::PlantUMLRunner(QObject *parent) : QObject(parent) {}

void PlantUMLRunner::searchDefaultExecutablePaths()
{
#if defined(Q_OS_WIN) && QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    auto dotPath = qEnvironmentVariable("GRAPHVIZ_DOT");
    auto envPath = qEnvironmentVariable("PATH");
#else
    auto dotPath = QString(qgetenv("GRAPHVIZ_DOT"));
    auto envPath = QString(qgetenv("PATH"));
#endif

    if (QFile::exists(dotPath))
        m_graphvizPath = dotPath;

#if defined(Q_OS_WIN)
    const QString javaExecutable = "java.exe";
    const QString dotExecutable  = "dot.exe";
    auto          paths          = envPath.split(";");
    paths << QCoreApplication::applicationDirPath() + "/graphviz/bin/" << QCoreApplication::applicationDirPath();
#else
    const QString javaExecutable = "java";
    const QString dotExecutable  = "dot";
    auto          paths          = envPath.split(":");
    paths << "/usr/local/bin";
#endif

#if defined(Q_OS_MAC)
    paths << QCoreApplication::applicationDirPath() + "/../Resources/";
#endif

    for (const auto &path : qAsConst(paths))
    {
        qDebug() << path;
        if (m_javaPath.isEmpty() && QFile::exists(path + "/" + javaExecutable))
            m_javaPath = path + "/" + javaExecutable;
        if (m_graphvizPath.isEmpty() && QFile::exists(path + "/" + dotExecutable))
            m_graphvizPath = path + "/" + dotExecutable;
        if (m_plantUmlPath.isEmpty() && QFile::exists(path + "/plantuml.jar"))
            m_plantUmlPath = path + "/plantuml.jar";
    }
}

bool PlantUMLRunner::isRunning()
{
    return m_process != nullptr;
}

void PlantUMLRunner::checkPaths()
{
    m_hasValidPaths = QFile::exists(m_javaPath) && QFile::exists(m_plantUmlPath);
}

void PlantUMLRunner::exportByPlantUML(const QString &saveAs, const QByteArray &doc)
{
    if (m_process)
    {
        qDebug() << "still processing previous refresh. skipping...";
        return;
    }
    m_input = doc;

    m_saveAs = saveAs;
    m_process = new QProcess(this);
    QFileInfo fi(saveAs);
    m_process->setWorkingDirectory(fi.absolutePath());

    QStringList arguments;

    arguments << "-jar" << m_plantUmlPath << "-t"+fi.suffix();
    arguments << "-charset" << "UTF-8" << "-pipe";

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
#if defined(Q_OS_MAC)
    env.insert("JAVA_TOOL_OPTIONS", "-Dapple.awt.UIElement=true");
#endif

    arguments << "-graphvizdot" << QDir::toNativeSeparators(m_graphvizPath);
    env.insert("GRAPHVIZ_DOT", QDir::toNativeSeparators(m_graphvizPath));

    m_process->setProcessEnvironment(env);
    m_process->start(m_javaPath, arguments);

    if (!m_process->waitForStarted()) {
        qDebug() << "refresh subprocess failed to start";
        return;
    }

    connect(m_process, SIGNAL(finished(int)), this, SLOT(exportFinished()));

    m_process->write(doc);
    m_process->closeWriteChannel();
}

void PlantUMLRunner::exportByGraphviz(const QString &saveAs, const QByteArray &doc, const QString &engine)
{
    if (m_process)
    {
        qDebug() << "still processing previous refresh. skipping...";
        return;
    }
    m_input = doc;

    m_saveAs = saveAs;
    m_process = new QProcess(this);
    QFileInfo fi(saveAs);
    m_process->setWorkingDirectory(fi.absolutePath());

    QStringList arguments;
    arguments << "-T" + fi.suffix();
    QString path = QFileInfo(m_graphvizPath).absolutePath() + "/" + engine;
    m_process->start(path, arguments);

    if (!m_process->waitForStarted())
    {
        qDebug() << "refresh subprocess failed to start";
        return;
    }

    connect(m_process, SIGNAL(finished(int)), this, SLOT(exportFinished()));

    m_process->write(doc);
    m_process->closeWriteChannel();
}

void PlantUMLRunner::runPlantUML(const QByteArray &doc, const QString& outputFormat)
{
    if (m_process)
    {
        qDebug() << "still processing previous refresh. skipping...";
        return;
    }
    m_input = doc;

    m_process = new QProcess(this);

    QStringList arguments;

    arguments << "-jar" << m_plantUmlPath << "-t"+outputFormat;
    arguments << "-charset" << "UTF-8" << "-pipe";


    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
#if defined(Q_OS_MAC)
    env.insert("JAVA_TOOL_OPTIONS", "-Dapple.awt.UIElement=true");
#endif

    env.insert("GRAPHVIZ_DOT", QDir::toNativeSeparators(m_graphvizPath));
    arguments << "-graphvizdot" << QDir::toNativeSeparators(m_graphvizPath);

    m_process->setProcessEnvironment(env);
    m_process->start(m_javaPath, arguments);


    if (!m_process->waitForStarted())
    {
        qDebug() << "refresh subprocess failed to start";
        return;
    }

    connect(m_process, SIGNAL(finished(int)), this, SLOT(runFinished()));

    m_process->write(doc);
    m_process->closeWriteChannel();
}

void PlantUMLRunner::runGraphviz(const QByteArray &doc, const QString& outputFormat, const QString &engine)
{
    if (m_process)
    {
        qDebug() << "still processing previous refresh. skipping...";
        return;
    }
    m_input   = doc;
    m_process = new QProcess(this);

    QStringList arguments;
    arguments << "-T" + outputFormat;
    QFileInfo fi(m_graphvizPath);
    QString path = fi.absolutePath() + "/" + engine;
    m_process->start(path, arguments);

    if (!m_process->waitForStarted())
    {
        qDebug() << "refresh subprocess failed to start";
        return;
    }

    connect(m_process, SIGNAL(finished(int)), this, SLOT(runFinished()));

    m_process->write(doc);
    m_process->closeWriteChannel();
}

bool PlantUMLRunner::hasPlantUMLStartEndMark(const QByteArray &doc)
{
    return (doc.contains("@startuml") || doc.contains("@enduml") || doc.contains("@startdot") || doc.contains("@enddot") ||
            doc.contains("@startlatex") || doc.contains("@endlatex") || doc.contains("@startmath") || doc.contains("@endmath") ||
            doc.contains("@startsalt") || doc.contains("@endsalt") || doc.contains("@startditaa") || doc.contains("@endditaa") ||
            doc.contains("@startjcckit") || doc.contains("@endjcckit") || doc.contains("@startmindmap") || doc.contains("@endmindmap") ||
            doc.contains("@startjson") || doc.contains("@endjson") || doc.contains("@startyaml") || doc.contains("@endyaml"));
}

void PlantUMLRunner::exportFinished()
{
    m_output = m_process->readAll();
    m_process->deleteLater();
    m_process = nullptr;

    QFileInfo fi(m_saveAs);
    if (!fi.absoluteDir().exists())
        fi.absoluteDir().mkpath(fi.absolutePath());

    QFile f(m_saveAs);
    if (f.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        f.write(m_output);
        f.close();
        emit exported(m_saveAs);
    }
}

void PlantUMLRunner::runFinished()
{
    m_output = m_process->readAll();
    m_process->deleteLater();
    m_process = nullptr;
    emit result();
}

const QString &PlantUMLRunner::cacheKey() const
{
    return m_cacheKey;
}

void PlantUMLRunner::setCacheKey(const QString &cacheKey)
{
    m_cacheKey = cacheKey;
}

const QByteArray &PlantUMLRunner::output() const
{
    return m_output;
}

const QByteArray &PlantUMLRunner::input() const
{
    return m_input;
}

bool PlantUMLRunner::hasValidPaths() const
{
    return m_hasValidPaths;
}

void PlantUMLRunner::setHasValidPaths(bool hasValidPaths)
{
    m_hasValidPaths = hasValidPaths;
}

const QString &PlantUMLRunner::graphvizPath() const
{
    return m_graphvizPath;
}

void PlantUMLRunner::setGraphvizPath(const QString &graphvizPath)
{
    if (QFile::exists(graphvizPath))
        m_graphvizPath = graphvizPath;
}

const QString &PlantUMLRunner::plantUmlPath() const
{
    return m_plantUmlPath;
}

void PlantUMLRunner::setPlantUmlPath(const QString &plantUmlPath)
{
    if (QFile::exists(plantUmlPath))
        m_plantUmlPath = plantUmlPath;
}

const QString &PlantUMLRunner::javaPath() const
{
    return m_javaPath;
}

void PlantUMLRunner::setJavaPath(const QString &javaPath)
{
    if (QFile::exists(javaPath))
        m_javaPath = javaPath;
}
