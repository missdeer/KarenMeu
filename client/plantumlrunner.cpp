#include <QCoreApplication>
#include <QFile>
#include <QProcess>
#include <QtCore>

#include "plantumlrunner.h"

#include "utils.h"

static QString gSettingsCustomJavaDefaultPath;
static QString gSettingsCustomPlantUMLDefaultPath;
static QString gSettingsCustomGraphvizDefaultPath;

bool QuickGetFilesByFileName(const QString& fileName, QStringList& results);

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
        gSettingsCustomGraphvizDefaultPath = dotPath;

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

    for (const auto& path : paths)
    {
        qDebug() << path;
        if (gSettingsCustomJavaDefaultPath.isEmpty() && QFile::exists(path + "/" + javaExecutable))
            gSettingsCustomJavaDefaultPath = path + "/" + javaExecutable;
        if (gSettingsCustomGraphvizDefaultPath.isEmpty() && QFile::exists(path + "/" + dotExecutable))
            gSettingsCustomGraphvizDefaultPath = path + "/" + dotExecutable;
        if (gSettingsCustomPlantUMLDefaultPath.isEmpty() && QFile::exists(path + "/plantuml.jar"))
            gSettingsCustomPlantUMLDefaultPath = path + "/plantuml.jar";
    }
}

void PlantUMLRunner::updateJavaPath()
{
    m_javaPath = (m_useCustomJava && QFile::exists(m_customJavaPath)) ? m_customJavaPath : gSettingsCustomJavaDefaultPath;
}

void PlantUMLRunner::updateJavaVersion()
{
    m_javaVersion = tr("Unknown");
    if (QFile::exists(m_javaPath))
    {
        // Determine java version
        m_process = new QProcess(this);

#if defined(Q_OS_MAC)
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        env.insert("JAVA_TOOL_OPTIONS", "-Dapple.awt.UIElement=true");
        m_process->setProcessEnvironment(env);
#endif
        m_process->setWorkingDirectory(QFileInfo(m_javaPath).absolutePath());
        m_process->setProcessChannelMode(QProcess::MergedChannels);
        QStringList arguments;
        arguments << "-version";
        m_process->start(m_javaPath, arguments);

        if (!m_process->waitForStarted()) {
            qDebug() << "refresh subprocess failed to start";
        } else {
            QByteArray data;

            while (m_process->waitForReadyRead()) {
                data.append(m_process->readAll());
            }

            QRegExp regex("version (\\S+)");
            int pos = 0;
            pos = regex.indexIn(data.data(), pos);

            if (pos > -1) {
                m_javaVersion = regex.cap(1);
                m_javaVersion.replace("\"", "");
            }
        }

        //delete m_process;
        m_process = nullptr;
    }
}

void PlantUMLRunner::updatePlantUMLPath()
{
    m_plantUmlPath = (m_useCustomPlantUml && QFile::exists(m_customPlantUmlPath)) ? m_customPlantUmlPath : gSettingsCustomPlantUMLDefaultPath;
}

void PlantUMLRunner::updatePlantUMLVersion()
{
    m_plantUmlVersion = tr("Unknown");
    if (QFile::exists(m_plantUmlPath))
    {
        // Determine plantuml version
        m_process = new QProcess(this);

#if defined(Q_OS_MAC)
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        env.insert("JAVA_TOOL_OPTIONS", "-Dapple.awt.UIElement=true");
        m_process->setProcessEnvironment(env);
#endif
        m_process->setWorkingDirectory(QFileInfo(m_plantUmlPath).absolutePath());
        m_process->setProcessChannelMode(QProcess::MergedChannels);
        QStringList arguments;
        arguments << "-splash:no" << "-jar" << m_plantUmlPath << "-version";
        m_process->start(m_javaPath, arguments);

        if (!m_process->waitForStarted()) {
            qDebug() << "refresh subprocess failed to start";
        } else {
            QByteArray data;

            while (m_process->waitForReadyRead()) {
                data.append(m_process->readAll());
            }

            QRegExp regex("version ([\\d\\.]+)");
            int pos = 0;
            pos = regex.indexIn(data.data(), pos);

            if (pos > -1) {
                m_plantUmlVersion = regex.cap(1);
            }
        }

        //delete m_process;
        m_process = nullptr;
    }
}

void PlantUMLRunner::updateGraphvizPath()
{
    m_graphvizPath = (m_useCustomGraphiz && QFile::exists(m_customGraphizPath)) ? m_customGraphizPath : gSettingsCustomGraphvizDefaultPath;
}

void PlantUMLRunner::updateGraphvizVersion()
{
    m_graphvizVersion = tr("Unknown");
    if (QFile::exists(m_graphvizPath))
    {
        // Determine graphviz version
        m_process = new QProcess(this);
        m_process->setWorkingDirectory(QFileInfo(m_graphvizPath).absolutePath());
        m_process->setProcessChannelMode(QProcess::MergedChannels);
        QStringList arguments;
        arguments << "-V";
        m_process->start(m_graphvizPath, arguments);

        if (!m_process->waitForStarted()) {
            qDebug() << "refresh subprocess failed to start";
        } else {
            QByteArray data;

            while (m_process->waitForReadyRead()) {
                data.append(m_process->readAll());
            }

            QRegExp regex("version (\\S+)");
            int pos = 0;
            pos = regex.indexIn(data.data(), pos);

            if (pos > -1) {
                m_graphvizVersion = regex.cap(1);
            }
        }

        //delete m_process;
        m_process = nullptr;
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
    if (m_useCustomGraphiz)
    {
        arguments << "-graphvizdot" << QDir::toNativeSeparators(m_graphvizPath);
        env.insert("GRAPHVIZ_DOT", QDir::toNativeSeparators(m_graphvizPath));
    }
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

    m_process = new QProcess(this);

    QStringList arguments;

    arguments << "-jar" << m_plantUmlPath << "-t"+outputFormat;
    arguments << "-charset" << "UTF-8" << "-pipe";


    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
#if defined(Q_OS_MAC)
    env.insert("JAVA_TOOL_OPTIONS", "-Dapple.awt.UIElement=true");
#endif
    if (m_useCustomGraphiz)
    {
        env.insert("GRAPHVIZ_DOT", QDir::toNativeSeparators(m_graphvizPath));
        arguments << "-graphvizdot" << QDir::toNativeSeparators(m_graphvizPath);
    }
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
    return (doc.contains("@startuml") || doc.contains("@enduml") ||
            doc.contains("@startdot") || doc.contains("@enddot") ||
            doc.contains("@startlatex") || doc.contains("@endlatex") ||
            doc.contains("@startmath") || doc.contains("@endmath") ||
            doc.contains("@startsalt") || doc.contains("@endsalt") ||
            doc.contains("@startditaa") || doc.contains("@endditaa") ||
            doc.contains("@startjcckit") || doc.contains("@endjcckit")
            );
}

void PlantUMLRunner::exportFinished()
{
    QByteArray c = m_process->readAll();
    m_process->deleteLater();
    m_process = nullptr;

    QFileInfo fi(m_saveAs);
    if (!fi.absoluteDir().exists())
        fi.absoluteDir().mkpath(fi.absolutePath());

    QFile f(m_saveAs);
    if (f.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        f.write(c);
        f.close();
        emit exported(m_saveAs);
    }
}

void PlantUMLRunner::runFinished()
{
    QByteArray c = m_process->readAll();
    m_process->deleteLater();
    m_process = nullptr;
    emit result(c);
}

bool PlantUMLRunner::useRemoteServerFirst() const
{
    return m_useRemoteServerFirst;
}

void PlantUMLRunner::setUseRemoteServerFirst(bool useRemoteServerFirst)
{
    m_useRemoteServerFirst = useRemoteServerFirst;
}

bool PlantUMLRunner::hasValidPaths() const
{
    return m_hasValidPaths;
}

void PlantUMLRunner::setHasValidPaths(bool hasValidPaths)
{
    m_hasValidPaths = hasValidPaths;
}

QString PlantUMLRunner::remoteServerAddress() const
{
    return m_remoteServerAddress;
}

void PlantUMLRunner::setRemoteServerAddress(const QString &remoteServerAddress)
{
    m_remoteServerAddress = remoteServerAddress;
}

QString PlantUMLRunner::graphvizVersion() const
{
    return m_graphvizVersion;
}

void PlantUMLRunner::setGraphvizVersion(const QString &graphvizVersion)
{
    m_graphvizVersion = graphvizVersion;
}

QString PlantUMLRunner::javaVersion() const
{
    return m_javaVersion;
}

void PlantUMLRunner::setJavaVersion(const QString &javaVersion)
{
    m_javaVersion = javaVersion;
}

QString PlantUMLRunner::plantUmlVersion() const
{
    return m_plantUmlVersion;
}

void PlantUMLRunner::setPlantUmlVersion(const QString &plantUmlVersion)
{
    m_plantUmlVersion = plantUmlVersion;
}

QString PlantUMLRunner::customGraphizPath() const
{
    return m_customGraphizPath;
}

void PlantUMLRunner::setCustomGraphizPath(const QString &customGraphizPath)
{
    m_customGraphizPath = customGraphizPath;
}

QString PlantUMLRunner::customPlantUmlPath() const
{
    return m_customPlantUmlPath;
}

void PlantUMLRunner::setCustomPlantUmlPath(const QString &customPlantUmlPath)
{
    m_customPlantUmlPath = customPlantUmlPath;
}

QString PlantUMLRunner::customJavaPath() const
{
    return m_customJavaPath;
}

void PlantUMLRunner::setCustomJavaPath(const QString &customJavaPath)
{
    m_customJavaPath = customJavaPath;
}

const QString &PlantUMLRunner::graphvizPath() const
{
    return m_graphvizPath;
}

void PlantUMLRunner::setGraphvizPath(const QString &graphvizPath)
{
    m_graphvizPath = graphvizPath;
}

const QString &PlantUMLRunner::plantUmlPath() const
{
    return m_plantUmlPath;
}

void PlantUMLRunner::setPlantUmlPath(const QString &plantUmlPath)
{
    m_plantUmlPath = plantUmlPath;
}

const QString &PlantUMLRunner::javaPath() const
{
    return m_javaPath;
}

void PlantUMLRunner::setJavaPath(const QString &javaPath)
{
    m_javaPath = javaPath;
}

bool PlantUMLRunner::useCustomGraphiz() const
{
    return m_useCustomGraphiz;
}

void PlantUMLRunner::setUseCustomGraphiz(bool useCustomGraphiz)
{
    m_useCustomGraphiz = useCustomGraphiz;
}

bool PlantUMLRunner::useCustomPlantUml() const
{
    return m_useCustomPlantUml;
}

void PlantUMLRunner::setUseCustomPlantUml(bool useCustomPlantUml)
{
    m_useCustomPlantUml = useCustomPlantUml;
}

bool PlantUMLRunner::useCustomJava() const
{
    return m_useCustomJava;
}

void PlantUMLRunner::setUseCustomJava(bool useCustomJava)
{
    m_useCustomJava = useCustomJava;
}
