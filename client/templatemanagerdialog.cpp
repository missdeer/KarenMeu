#include "templatemanagerdialog.h"
#include "ui_templatemanagerdialog.h"

TemplateManagerDialog::TemplateManagerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TemplateManagerDialog)
{
    ui->setupUi(this);
}

TemplateManagerDialog::~TemplateManagerDialog()
{
    delete ui;
}

void TemplateManagerDialog::on_cbTemplates_currentTextChanged(const QString &arg1) {}

void TemplateManagerDialog::on_btnAddTemplate_clicked() {}

void TemplateManagerDialog::on_btnSaveTemplate_clicked() {}

void TemplateManagerDialog::on_btnRemoveTemplate_clicked() {}
