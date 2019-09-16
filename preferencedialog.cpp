#include "settings.h"
#include "preferencedialog.h"
#include "ui_preferencedialog.h"

PreferenceDialog::PreferenceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferenceDialog)
{
    ui->setupUi(this);
    ui->cbPreviewTheme->setCurrentText(g_settings->previewTheme());
    ui->cbCodeBlockStyle->setCurrentText(g_settings->codeBlockStyle());
    ui->cbMarkdownEngine->setCurrentText(g_settings->markdownEngine());
    ui->cbEnableLineNumbers->setChecked(g_settings->enableLineNumbers());
    QFont f;
    f.setFamily(g_settings->codeEditorFontFamily());
    ui->cbCodeEditorFont->setCurrentFont(f);
    ui->sbFontPointSize->setValue(g_settings->codeEditorFontPointSize());
    ui->cbPreviewMode->setCurrentText(g_settings->previewMode());
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
    g_settings->setCodeEditorFontFamily(ui->cbCodeEditorFont->currentFont().family());
    g_settings->setCodeEditorFontPointSize(ui->sbFontPointSize->value());
    QDialog::accept();
}
