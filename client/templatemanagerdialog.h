#ifndef TEMPLATEMANAGERDIALOG_H
#define TEMPLATEMANAGERDIALOG_H

#include <QDialog>

namespace Ui {
    class TemplateManagerDialog;
}

class TemplateManager;

class TemplateManagerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TemplateManagerDialog(TemplateManager &mgr, QWidget *parent = nullptr);
    ~TemplateManagerDialog();

private slots:
    void on_cbTemplates_currentTextChanged(const QString &arg1);

    void on_btnAddTemplate_clicked();

    void on_btnSaveTemplate_clicked();

    void on_btnRemoveTemplate_clicked();

private:
    Ui::TemplateManagerDialog *ui;
    TemplateManager &          m_mgr;
};

#endif // TEMPLATEMANAGERDIALOG_H
