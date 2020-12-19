#include <QBoxLayout>
#include <QFile>
#include <QFileDialog>
#include <QToolBar>

#include "custompreviewthemeeditwidget.h"

#include "previewthemeeditor.h"
#include "settings.h"

CustomPreviewThemeEditWidget::CustomPreviewThemeEditWidget(QWidget *parent)
    : QWidget(parent)
    , m_toolbar(new QToolBar(this))
    , m_editor(new PreviewThemeEditor(this))
{
    auto toolLayout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    toolLayout->setContentsMargins(0, 0, 0, 0);
    toolLayout->setSpacing(0);

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

QByteArray CustomPreviewThemeEditWidget::content()
{
    Q_ASSERT(m_editor);
    return m_editor->content();
}

void CustomPreviewThemeEditWidget::onSave()
{
    Q_ASSERT(m_editor);
    if (g_settings->previewTheme() == tr("Custom"))
    {
        g_settings->setCustomPreviewThemeStyle(m_editor->content());
        emit contentModified();
    }
}

void CustomPreviewThemeEditWidget::onImportFromFile()
{
    QString fn = QFileDialog::getOpenFileName(this, tr("Import from css file"), QString(), tr("Custom theme file (*.css)"));
    QFile   f(fn);
    if (f.open(QIODevice::ReadOnly))
    {
        auto ba = f.readAll();
        f.close();
        Q_ASSERT(m_editor);
        m_editor->setContent(ba);
    }
}

void CustomPreviewThemeEditWidget::onExportToFile()
{
    QString fn = QFileDialog::getSaveFileName(this, tr("Export to css file"), QString(), tr("Custom theme file (*.css)"));
    QFile   f(fn);
    if (f.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        Q_ASSERT(m_editor);
        f.write(m_editor->content());
        f.close();
    }
}

void CustomPreviewThemeEditWidget::initializeEditor()
{
    Q_ASSERT(m_editor);
    m_editor->initialize("css");
}

void CustomPreviewThemeEditWidget::initializeToolbar()
{
    m_toolbar->setIconSize(QSize(16, 16));
    auto saveAction = m_toolbar->addAction(QIcon(":/rc/images/save.png"), tr("Save to configuration"));
    connect(saveAction, &QAction::triggered, this, &CustomPreviewThemeEditWidget::onSave);
    auto importAction = m_toolbar->addAction(QIcon(":/rc/images/import.png"), tr("Import From File"));
    connect(importAction, &QAction::triggered, this, &CustomPreviewThemeEditWidget::onImportFromFile);
    auto exportAction = m_toolbar->addAction(QIcon(":/rc/images/export.png"), tr("Export To File"));
    connect(exportAction, &QAction::triggered, this, &CustomPreviewThemeEditWidget::onExportToFile);
}
