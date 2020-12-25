#include <QFileInfo>

#include "templatemanagerdialog.h"

#include "markdowneditor4.h"
#include "templatemanager.h"
#include "ui_templatemanagerdialog.h"

TemplateManagerDialog::TemplateManagerDialog(TemplateManager &mgr, QWidget *parent) : QDialog(parent), ui(new Ui::TemplateManagerDialog), m_mgr(mgr)
{
    ui->setupUi(this);
    auto templates = m_mgr.templates();
    for (auto t : templates)
    {
        ui->cbTemplates->addItem(t->templateName());
    }
    if (!templates.isEmpty())
    {
        ui->cbTemplates->addItem(tr("<New Template>"));
        ui->cbTemplates->setCurrentIndex(0);
    }
}

TemplateManagerDialog::~TemplateManagerDialog()
{
    delete ui;
}

bool TemplateManagerDialog::isModified() const
{
    return m_isModified;
}

void TemplateManagerDialog::on_cbTemplates_currentTextChanged(const QString &arg1)
{
    auto t = m_mgr.find(arg1);
    if (t)
    {
        ui->edtFileNameRule->setText(t->nameTemplate());
        ui->edtTemplateName->setText(QFileInfo(t->path()).baseName());
        ui->edtContent->setText(t->contentTemplate());
        ui->edtTemplateName->setReadOnly(true);
    }
    else
    {
        ui->edtTemplateName->setText("");
        ui->edtFileNameRule->setText("");
        ui->edtContent->setText("");
        ui->edtTemplateName->setReadOnly(false);
    }
}

void TemplateManagerDialog::on_btnAddTemplate_clicked()
{
    ui->edtTemplateName->setReadOnly(false);
    ui->cbTemplates->setCurrentText(tr("<New Template>"));
    ui->edtTemplateName->setText("");
    ui->edtFileNameRule->setText("");
    ui->edtContent->setText("");
}

void TemplateManagerDialog::on_btnSaveTemplate_clicked()
{
    if (ui->cbTemplates->currentText().isEmpty())
    {
        auto t = m_mgr.add(ui->edtTemplateName->text(), ui->edtFileNameRule->text(), ui->edtContent->getText());
        t->save();
        m_isModified = true;
        ui->cbTemplates->addItem(ui->edtTemplateName->text());
        ui->cbTemplates->setCurrentText(ui->edtTemplateName->text());
        ui->edtTemplateName->setReadOnly(true);
    }
    else
    {
        auto t = m_mgr.find(ui->cbTemplates->currentText());
        if (t)
        {
            if (t->nameTemplate() != ui->edtFileNameRule->text())
            {
                m_isModified = true;
                t->setNameTemplate(ui->edtFileNameRule->text());
            }
            t->setContentTemplate(ui->edtContent->getText());
            t->save();
        }
    }
}

void TemplateManagerDialog::on_btnRemoveTemplate_clicked()
{
    m_mgr.remove(ui->edtTemplateName->text());
    ui->cbTemplates->removeItem(ui->cbTemplates->currentIndex());
    m_isModified = true;
}
