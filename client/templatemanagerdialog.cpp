#include <QFileInfo>

#include "templatemanagerdialog.h"

#include "markdowneditor4.h"
#include "templatemanager.h"
#include "ui_templatemanagerdialog.h"

TemplateManagerDialog::TemplateManagerDialog(TemplateManager &mgr, QWidget *parent) : QDialog(parent), ui(new Ui::TemplateManagerDialog), m_mgr(mgr)
{
    ui->setupUi(this);
    auto templates = m_mgr.templates();
    for (const auto &tmpl : templates)
    {
        ui->cbTemplates->addItem(tmpl->templateName());
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
    auto tmpl = m_mgr.find(arg1);
    if (tmpl != nullptr)
    {
        ui->edtFileNameRule->setText(tmpl->nameTemplate());
        ui->edtTemplateName->setText(QFileInfo(tmpl->path()).baseName());
        ui->edtContent->setText(tmpl->contentTemplate());
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
        auto tmpl = m_mgr.add(ui->edtTemplateName->text(), ui->edtFileNameRule->text(), ui->edtContent->getText());
        tmpl->save();
        m_isModified = true;
        ui->cbTemplates->addItem(ui->edtTemplateName->text());
        ui->cbTemplates->setCurrentText(ui->edtTemplateName->text());
        ui->edtTemplateName->setReadOnly(true);
    }
    else
    {
        auto tmpl = m_mgr.find(ui->cbTemplates->currentText());
        if (tmpl != nullptr)
        {
            if (tmpl->nameTemplate() != ui->edtFileNameRule->text())
            {
                m_isModified = true;
                tmpl->setNameTemplate(ui->edtFileNameRule->text());
            }
            tmpl->setContentTemplate(ui->edtContent->getText());
            tmpl->save();
        }
    }
}

void TemplateManagerDialog::on_btnRemoveTemplate_clicked()
{
    m_mgr.remove(ui->edtTemplateName->text());
    ui->cbTemplates->removeItem(ui->cbTemplates->currentIndex());
    m_isModified = true;
}
