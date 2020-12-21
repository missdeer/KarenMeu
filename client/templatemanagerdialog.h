#ifndef TEMPLATEMANAGERDIALOG_H
#define TEMPLATEMANAGERDIALOG_H

#include <QDialog>

namespace Ui {
    class TemplateManagerDialog;
}

class TemplateManagerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TemplateManagerDialog(QWidget *parent = nullptr);
    ~TemplateManagerDialog();

private slots:
    void on_cbTemplates_currentTextChanged(const QString &arg1);

    void on_btnAddTemplate_clicked();

    void on_btnSaveTemplate_clicked();

    void on_btnRemoveTemplate_clicked();

private:
    Ui::TemplateManagerDialog *ui;
};

#endif // TEMPLATEMANAGERDIALOG_H
