#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QUrl>

#include "preferencedialog.h"
#include "previewthemeeditor.h"
#include "qwebdav.h"
#include "qwebdavdirparser.h"
#include "settings.h"
#include "ui_preferencedialog.h"

PreferenceDialog::PreferenceDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::PreferenceDialog), m_webDAV(new QWebdav), m_webDAVDirParser(new QWebdavDirParser)
{
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(0);

    setupPreviewThemeEditor();

    ui->cbCodeBlockStyle->setCurrentText(g_settings->codeBlockStyle());
    ui->cbMarkdownEngine->setCurrentText(g_settings->markdownEngine());
    ui->cbEnableLineNumbers->setChecked(g_settings->enableLineNumbers());
    ui->cbMacTerminalStyleCodeBlock->setChecked(g_settings->macTerminalStyleCodeBlock());
    QFont f;
    f.setFamily(g_settings->codeEditorFontFamily());
    ui->cbCodeEditorFont->setCurrentFont(f);
    ui->sbFontPointSize->setValue(g_settings->codeEditorFontPointSize());
    ui->cbPreviewMode->setCurrentText(g_settings->previewMode());

    ui->cbBaiduTranslate->setChecked(g_settings->enableBaiduTranslate());
    ui->cbGoogleTranslate->setChecked(g_settings->enableGoogleTranslate());
    ui->cbYoudaoTranslate->setChecked(g_settings->enableYoudaoTranslate());
    ui->cbSogouTranslate->setChecked(g_settings->enableSogouTranslate());
    ui->cbDeepLTranslate->setChecked(g_settings->enableDeepLTranslate());

    ui->sbTranslateTimeout->setValue(g_settings->translateTimeout() / 1000);

    ui->edtPlantUMLRemoteService->setText(g_settings->plantUMLRemoteServiceAddress());
    ui->edtJavaPath->setText(g_settings->javaPath());
    ui->edtGraphvizDotPath->setText(g_settings->dotPath());
    ui->edtPlantUMLJarPath->setText(g_settings->plantUMLJarPath());

    connect(ui->gbPlantUMLLocalJar, &QGroupBox::toggled, [this](bool on) { ui->gbPlantUMLRemoteService->setChecked(!on); });
    connect(ui->gbPlantUMLRemoteService, &QGroupBox::toggled, [this](bool on) { ui->gbPlantUMLLocalJar->setChecked(!on); });

    ui->gbPlantUMLRemoteService->setChecked(g_settings->plantUMLRemoteServiceEnabled());
    ui->gbPlantUMLLocalJar->setChecked(g_settings->plantUMLLocalJarEnabled());

    const auto &webDAVInfos = g_settings->webDAVInfos();
    for (const auto &info : webDAVInfos)
    {
        ui->listWebDAV->addItem(QString("%1 - %2").arg(info.server, info.user));
    }
    if (!webDAVInfos.empty())
    {
        ui->listWebDAV->setCurrentRow(0);
    }

    connect(m_webDAV, &QWebdav::errorChanged, this, &PreferenceDialog::onWebDAVError);
    connect(m_webDAVDirParser, &QWebdavDirParser::finished, this, &PreferenceDialog::onWebDAVFinished);
    connect(m_webDAVDirParser, &QWebdavDirParser::errorChanged, this, &PreferenceDialog::onWebDAVError);
}

PreferenceDialog::~PreferenceDialog()
{
    delete m_webDAVDirParser;
    delete m_webDAV;
    delete ui;
}

void PreferenceDialog::accept()
{
    g_settings->setPreviewMode(ui->cbPreviewMode->currentText());
    g_settings->setPreviewTheme(ui->cbPreviewTheme->currentText());
    g_settings->setCodeBlockStyle(ui->cbCodeBlockStyle->currentText());
    g_settings->setMarkdownEngine(ui->cbMarkdownEngine->currentText());
    g_settings->setEnableLineNumbers(ui->cbEnableLineNumbers->isChecked());
    g_settings->setMacTerminalStyleCodeBlock(ui->cbMacTerminalStyleCodeBlock->isChecked());
    g_settings->setCodeEditorFontFamily(ui->cbCodeEditorFont->currentFont().family());
    g_settings->setCodeEditorFontPointSize(ui->sbFontPointSize->value());
    if (ui->cbPreviewTheme->currentText() == tr("Custom"))
    {
        g_settings->setCustomPreviewThemeStyle(m_previewThemeEditor->content());
    }

    g_settings->setEnableGoogleTranslate(ui->cbGoogleTranslate->isChecked());
    g_settings->setEnableBaiduTranslate(ui->cbBaiduTranslate->isChecked());
    g_settings->setEnableSogouTranslate(ui->cbSogouTranslate->isChecked());
    g_settings->setEnableYoudaoTranslate(ui->cbYoudaoTranslate->isChecked());
    g_settings->setEnableDeepLTranslate(ui->cbDeepLTranslate->isChecked());

    g_settings->setTranslateTimeout(ui->sbTranslateTimeout->value() * 1000);

    g_settings->setPlantUMLJarPath(ui->edtPlantUMLJarPath->text());
    g_settings->setPlantUMLRemoteService(ui->edtPlantUMLRemoteService->text());
    g_settings->setJavaPath(ui->edtJavaPath->text());
    g_settings->setDotPath(ui->edtGraphvizDotPath->text());
    g_settings->setPlantUMLLocalJarEnabled(ui->gbPlantUMLLocalJar->isChecked());
    g_settings->setPlantUMLRemoteServiceEnabled(ui->gbPlantUMLRemoteService->isChecked());
    if (!m_webDAVInfos.empty())
    {
        g_settings->setWebDAVInfos(m_webDAVInfos);
    }
    QDialog::accept();
}

void PreferenceDialog::onWebDAVFinished()
{
    auto list = m_webDAVDirParser->getList();
    qDebug() << "returns" << list.size() << "items";
    for (auto &item : list)
    {
        qDebug() << item.name();
    }

    ui->btnAddWebDAV->setEnabled(true);
    QMessageBox::information(this, tr("Notice"), tr("This server seems ok."), QMessageBox::Ok);
}

void PreferenceDialog::onWebDAVError(QString errorMsg)
{
    ui->btnAddWebDAV->setEnabled(false);
    QMessageBox::warning(this, tr("Error"), errorMsg, QMessageBox::Ok);
}

void PreferenceDialog::on_cbPreviewTheme_currentTextChanged(const QString &text)
{
    if (text == tr("Custom"))
    {
        m_previewThemeEditor->setContent(g_settings->customPreviewThemeStyle());
        return;
    }

    QFile file(":/rc/theme/" + text + ".css");
    if (file.open(QIODevice::ReadOnly))
    {
        QByteArray ba = file.readAll();
        m_previewThemeEditor->setContent(ba);
        file.close();
    }
}

void PreferenceDialog::on_btnBrowseJavaPath_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select Java executable"), ui->edtJavaPath->text());
    if (!fileName.isEmpty())
    {
        ui->edtJavaPath->setText(QDir::toNativeSeparators(fileName));
    }
}

void PreferenceDialog::on_btnBrowseGraphvizDotPath_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select Dot (from Graphiz) executable"), ui->edtGraphvizDotPath->text());
    if (!fileName.isEmpty())
    {
        ui->edtGraphvizDotPath->setText(QDir::toNativeSeparators(fileName));
    }
}

void PreferenceDialog::on_btnBrowsePlantUMLJarPath_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select PlantUML jar"), ui->edtPlantUMLJarPath->text());
    if (!fileName.isEmpty())
    {
        ui->edtPlantUMLJarPath->setText(QDir::toNativeSeparators(fileName));
    }
}

void PreferenceDialog::setupPreviewThemeEditor()
{
    auto *layout         = new QVBoxLayout;
    m_previewThemeEditor = new PreviewThemeEditor(ui->previewThemeEditorContainer);
    layout->addWidget(m_previewThemeEditor);
    layout->setMargin(0);
    ui->previewThemeEditorContainer->setLayout(layout);

    m_previewThemeEditor->initialize("css");

    ui->cbPreviewTheme->clear();

    QDir dir(":/rc/theme");
    auto entries = dir.entryInfoList();
    for (const auto &entry : entries)
    {
        ui->cbPreviewTheme->addItem(entry.baseName());
    }
    ui->cbPreviewTheme->addItem(tr("Custom"));

    ui->cbPreviewTheme->setCurrentText(g_settings->previewTheme());
}

void PreferenceDialog::on_btnTestWebDAV_clicked()
{
    QUrl url(ui->edtWebDAVServerAddress->text());

    auto user   = ui->edtWebDAVUsername->text();
    auto passwd = ui->edtWebDAVPassword->text();

    m_webDAV->setConnectionSettings(url.scheme() == "https" ? QWebdav::HTTPS : QWebdav::HTTP, url.host(), url.path(), user, passwd, url.port());
    m_webDAVDirParser->listDirectory(m_webDAV, "/");

    qDebug() << url.host() << url.path() << url.port() << url.scheme() << user << passwd;
}

void PreferenceDialog::on_btnAddWebDAV_clicked()
{
    // check exists
    const auto &webDAVInfos = m_webDAVInfos.empty() ? g_settings->webDAVInfos() : m_webDAVInfos;
    auto        iter        = std::find_if(webDAVInfos.begin(), webDAVInfos.end(), [this](const WebDAVInfo &info) {
        return info.server == ui->edtWebDAVServerAddress->text() && info.user == ui->edtWebDAVUsername->text() &&
               info.password == ui->edtWebDAVPassword->text();
    });
    if (webDAVInfos.end() != iter)
    {
        QMessageBox::warning(this, tr("Warning"), tr("This WebDAV server exists."), QMessageBox::Ok);
        return;
    }
    // not exist
    if (!m_webDAVInfos.empty())
    {
        m_webDAVInfos = g_settings->webDAVInfos();
    }
    m_webDAVInfos.append({ui->edtWebDAVServerAddress->text(), ui->edtWebDAVUsername->text(), ui->edtWebDAVPassword->text()});
    ui->listWebDAV->addItem(QString("%1 - %2").arg(ui->edtWebDAVServerAddress->text(), ui->edtWebDAVUsername->text()));
    ui->listWebDAV->setCurrentRow(ui->listWebDAV->count() - 1);
}

void PreferenceDialog::on_btnRemoveWebDAV_clicked()
{
    if (!m_webDAVInfos.empty())
    {
        m_webDAVInfos = g_settings->webDAVInfos();
    }
    int row = ui->listWebDAV->currentIndex().row();
    if (row < 0 || row >= m_webDAVInfos.size())
    {
        return;
    }
    m_webDAVInfos.remove(row);
    ui->listWebDAV->removeItemWidget(ui->listWebDAV->item(row));
    ui->listWebDAV->setCurrentRow(row >= ui->listWebDAV->count() ? ui->listWebDAV->count() - 1 : row);
}

void PreferenceDialog::on_listWebDAV_currentRowChanged(int currentRow)
{
    const auto &webDAVInfos = m_webDAVInfos.empty() ? g_settings->webDAVInfos() : m_webDAVInfos;
    if (currentRow < 0 || currentRow >= webDAVInfos.size())
    {
        return;
    }
    const auto &info = webDAVInfos.at(currentRow);
    ui->edtWebDAVServerAddress->setText(info.server);
    ui->edtWebDAVUsername->setText(info.user);
    ui->edtWebDAVPassword->setText(info.password);
}
