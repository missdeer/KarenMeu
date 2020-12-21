#include <QFileInfo>

#include "templatemanagerdialog.h"

#include "markdowneditor4.h"
#include "templatemanager.h"
#include "ui_templatemanagerdialog.h"

TemplateManagerDialog::TemplateManagerDialog(TemplateManager &mgr, QWidget *parent) : QDialog(parent), ui(new Ui::TemplateManagerDialog), m_mgr(mgr)
{
    ui->setupUi(this);
}

TemplateManagerDialog::~TemplateManagerDialog()
{
    delete ui;
}

void TemplateManagerDialog::on_cbTemplates_currentTextChanged(const QString &arg1)
{
    auto t = m_mgr.find(arg1);
    ui->edtFileNameRule->setText(t->nameRule());
    ui->edtTemplateName->setText(QFileInfo(t->path()).baseName());
    ui->edtContent->setText(t->contentTemplate());
}

void TemplateManagerDialog::on_btnAddTemplate_clicked()
{
    m_mgr.add(ui->edtTemplateName->text(), ui->edtFileNameRule->text(), ui->edtContent->getText());
}

void TemplateManagerDialog::on_btnSaveTemplate_clicked()
{
    m_mgr.save();
}

void TemplateManagerDialog::on_btnRemoveTemplate_clicked()
{
    m_mgr.remove(ui->edtTemplateName->text());
    ui->cbTemplates->removeItem(ui->cbTemplates->currentIndex());
}
