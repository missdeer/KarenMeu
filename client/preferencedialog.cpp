#include <QDir>
#include <QFile>
#include <QFileDialog>

#include "preferencedialog.h"
#include "previewthemeeditor.h"
#include "settings.h"
#include "ui_preferencedialog.h"

PreferenceDialog::PreferenceDialog(QWidget *parent) : QDialog(parent), ui(new Ui::PreferenceDialog)
{
    ui->setupUi(this);
    setupPreviewThemeEditor();

    ui->cbPreviewTheme->clear();

    QDir dir(":/rc/theme");
    auto entries = dir.entryInfoList();
    for (const auto &entry : entries)
    {
        ui->cbPreviewTheme->addItem(entry.baseName());
    }
    ui->cbPreviewTheme->addItem(tr("Custom"));

    ui->cbPreviewTheme->setCurrentText(g_settings->previewTheme());
    ui->cbCodeBlockStyle->setCurrentText(g_settings->codeBlockStyle());
    ui->cbMarkdownEngine->setCurrentText(g_settings->markdownEngine());
    ui->cbEnableLineNumbers->setChecked(g_settings->enableLineNumbers());
    ui->cbMacTerminalStyleCodeBlock->setChecked(g_settings->macTerminalStyleCodeBlock());
    QFont f;
    f.setFamily(g_settings->codeEditorFontFamily());
    ui->cbCodeEditorFont->setCurrentFont(f);
    ui->sbFontPointSize->setValue(g_settings->codeEditorFontPointSize());
    ui->cbPreviewMode->setCurrentText(g_settings->previewMode());

    ui->edtWebDAVServerAddress->setText(g_settings->webDAVServerAddress());
    ui->edtWebDAVUsername->setText(g_settings->webDAVUsername());
    ui->edtWebDAVPassword->setText(g_settings->webDAVPassword());

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
}

PreferenceDialog::~PreferenceDialog()
{
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
    g_settings->setWebDAVServerAddress(ui->edtWebDAVServerAddress->text());
    g_settings->setWebDAVUsername(ui->edtWebDAVUsername->text());
    g_settings->setWebDAVPassword(ui->edtWebDAVPassword->text());

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
    QDialog::accept();
}

void PreferenceDialog::on_cbPreviewTheme_currentTextChanged(const QString &text)
{
    if (text == tr("Custom"))
    {
        m_previewThemeEditor->setContent(g_settings->customPreviewThemeStyle());
        return;
    }

    QFile f(":/rc/theme/" + text + ".css");
    if (f.open(QIODevice::ReadOnly))
    {
        QByteArray ba = f.readAll();
        m_previewThemeEditor->setContent(ba);
        f.close();
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
}
