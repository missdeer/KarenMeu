#ifndef PREFERENCEDIALOG_H
#define PREFERENCEDIALOG_H

#include <QDialog>

namespace Ui {
class PreferenceDialog;
}

class PreviewThemeEditor;

class PreferenceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferenceDialog(QWidget *parent = nullptr);
    ~PreferenceDialog() override;
protected:
     void accept() override;
private:
    Ui::PreferenceDialog *ui;
    PreviewThemeEditor* m_previewThemeEditor;
};

#endif // PREFERENCEDIALOG_H
