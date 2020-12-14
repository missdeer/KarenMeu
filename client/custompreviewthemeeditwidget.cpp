#include <QBoxLayout>
#include <QFileDialog>
#include <QToolBar>

#include "custompreviewthemeeditwidget.h"

#include "previewthemeeditor.h"

CustomPreviewThemeEditWidget::CustomPreviewThemeEditWidget(QWidget *parent)
    : QWidget(parent)
    , m_toolbar(new QToolBar(this))
    , m_editor(new PreviewThemeEditor(this))
{
    auto toolLayout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    // set margins to zero so the toolbar touches the widget's edges
    toolLayout->setContentsMargins(0, 0, 0, 0);

    toolLayout->addWidget(m_toolbar);
    toolLayout->addWidget(m_editor);

    setLayout(toolLayout);

    initializeEditor();
    initializeToolbar();
}

PreviewThemeEditor *CustomPreviewThemeEditWidget::editor() const
{
    return m_editor;
}

void CustomPreviewThemeEditWidget::onSave() {}

void CustomPreviewThemeEditWidget::onImportFromFile()
{
    QString fn = QFileDialog::getOpenFileName(this, tr("Import from css file"), QString(), "Custom theme file (*.css)");
}

void CustomPreviewThemeEditWidget::onExportToFile()
{
    QString fn = QFileDialog::getSaveFileName(this, tr("Export to css file"), QString(), "Custom theme file (*.css)");
}

void CustomPreviewThemeEditWidget::initializeEditor()
{
    m_editor->initialize("css");
}

void CustomPreviewThemeEditWidget::initializeToolbar()
{
    m_toolbar->setIconSize(QSize(16, 16));
    auto saveAction = m_toolbar->addAction(QIcon(":/rc/images/save.png"), tr("Save"));
    connect(saveAction, &QAction::triggered, this, &CustomPreviewThemeEditWidget::onSave);
    auto importAction = m_toolbar->addAction(QIcon(":/rc/images/import.png"), tr("Import From File"));
    connect(importAction, &QAction::triggered, this, &CustomPreviewThemeEditWidget::onImportFromFile);
    auto exportAction = m_toolbar->addAction(QIcon(":/rc/images/export.png"), tr("Export To File"));
    connect(exportAction, &QAction::triggered, this, &CustomPreviewThemeEditWidget::onExportToFile);
}
