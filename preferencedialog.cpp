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
}

PreferenceDialog::~PreferenceDialog()
{
    delete ui;
}

void PreferenceDialog::accept()
{
    g_settings->setPreviewTheme(ui->cbPreviewTheme->currentText());
    g_settings->setCodeBlockStyle(ui->cbCodeBlockStyle->currentText());
    g_settings->setMarkdownEngine(ui->cbMarkdownEngine->currentText());
    QDialog::accept();
}
