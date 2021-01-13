#ifndef FINDREPLACEDIALOG_H
#define FINDREPLACEDIALOG_H

#include <QDialog>

#include <vtextedit/vtexteditor.h>

namespace Ui {
    class FindReplaceDialog;
}

namespace vte
{
    class VTextEdit;
} // namespace vte

class FindReplaceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FindReplaceDialog(vte::VTextEditor *editor, QWidget *parent = nullptr);
    ~FindReplaceDialog();

private slots:
    void on_btnFind_clicked();

    void on_btnReplace_clicked();

    void on_btnReplaceAll_clicked();

private:
    Ui::FindReplaceDialog *ui;
    vte::VTextEditor *     m_editor;
    vte::FindFlags         getFindFlags();
    void                   promptMoveToBegin(vte::VTextEdit *textedit);
};

#endif // FINDREPLACEDIALOG_H
