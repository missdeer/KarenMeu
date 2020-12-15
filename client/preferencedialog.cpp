#include <QDir>
#include <QFile>

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
    for (auto entry : entries)
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

    ui->edtCloudServerAddress->setText(g_settings->cloudServerAddress());
    ui->edtCloudUsername->setText(g_settings->cloudUsername());
    ui->edtCloudPassword->setText(g_settings->cloudPassword());

    ui->cbBaiduTranslate->setChecked(g_settings->enableBaiduTranslate());
    ui->cbGoogleTranslate->setChecked(g_settings->enableGoogleTranslate());
    ui->cbYoudaoTranslate->setChecked(g_settings->enableYoudaoTranslate());
    ui->cbSogouTranslate->setChecked(g_settings->enableSogouTranslate());
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
    g_settings->setCloudServerAddress(ui->edtCloudServerAddress->text());
    g_settings->setCloudUsername(ui->edtCloudUsername->text());
    g_settings->setCloudPassword(ui->edtCloudPassword->text());

    g_settings->setEnableGoogleTranslate(ui->cbGoogleTranslate->isChecked());
    g_settings->setEnableBaiduTranslate(ui->cbBaiduTranslate->isChecked());
    g_settings->setEnableSogouTranslate(ui->cbSogouTranslate->isChecked());
    g_settings->setEnableYoudaoTranslate(ui->cbYoudaoTranslate->isChecked());
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

void PreferenceDialog::setupPreviewThemeEditor()
{
    auto *layout         = new QVBoxLayout;
    m_previewThemeEditor = new PreviewThemeEditor(ui->previewThemeEditorContainer);
    layout->addWidget(m_previewThemeEditor);
    layout->setMargin(0);
    ui->previewThemeEditorContainer->setLayout(layout);

    m_previewThemeEditor->initialize("css");
}
