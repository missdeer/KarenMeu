#ifndef CUSTOMPREVIEWTHEMEEDITWIDGET_H
#define CUSTOMPREVIEWTHEMEEDITWIDGET_H

#include <QWidget>

QT_FORWARD_DECLARE_CLASS(QToolBar);
class PreviewThemeEditor;

class CustomPreviewThemeEditWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CustomPreviewThemeEditWidget(QWidget *parent = nullptr);

    PreviewThemeEditor *editor() const;

private slots:
    void onSave();
    void onImportFromFile();
    void onExportToFile();

signals:

private:
    QToolBar *          m_toolbar;
    PreviewThemeEditor *m_editor;
    void                initializeEditor();
    void                initializeToolbar();
};

#endif // CUSTOMPREVIEWTHEMEEDITWIDGET_H
