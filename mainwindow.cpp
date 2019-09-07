#include <QtCore>
#include <QCoreApplication>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QCloseEvent>
#include <QAction>
#include <QMap>
#include <QHash>
#include "markdownview.h"
#include "preferencedialog.h"
#include "renderer.h"
#include "settings.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

using LabelActionMap = QMap<QString, QAction *>;
using ActionLabelMap = QHash<QAction *, QString>;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_view(new MarkdownView(this))
{
    ui->setupUi(this);
    setCentralWidget(m_view);
    connect(ui->actionNew, &QAction::triggered, m_view, &MarkdownView::newDocument);
    connect(ui->actionOpen, &QAction::triggered, m_view, &MarkdownView::openDocument);
    connect(ui->actionSave, &QAction::triggered, m_view, &MarkdownView::saveDocument);
    connect(ui->actionSaveAs, &QAction::triggered, m_view, &MarkdownView::saveAsDocument);
    connect(ui->actionCut, &QAction::triggered, m_view, &MarkdownView::cut);
    connect(ui->actionCopy, &QAction::triggered, m_view, &MarkdownView::copy);
    connect(ui->actionPaste, &QAction::triggered, m_view, &MarkdownView::paste);
    connect(ui->actionUndo, &QAction::triggered, m_view, &MarkdownView::undo);
    connect(ui->actionRedo, &QAction::triggered, m_view, &MarkdownView::redo);
    connect(ui->actionSelectAll, &QAction::triggered, m_view, &MarkdownView::selectAll);
    connect(ui->actionCopyAsHTML, &QAction::triggered, m_view, &MarkdownView::copyAsHTML);
    connect(ui->actionExportAsPDF, &QAction::triggered, m_view, &MarkdownView::exportAsPDF);
    connect(ui->actionExportAsHTML, &QAction::triggered, m_view, &MarkdownView::exportAsHTML);
    connect(ui->actionStrong, &QAction::triggered, m_view, &MarkdownView::formatStrong);
    connect(ui->actionEmphasize, &QAction::triggered, m_view, &MarkdownView::formatEmphasize);
    connect(ui->actionStrikethrough, &QAction::triggered, m_view, &MarkdownView::formatStrikethrough);
    connect(ui->actionInlineCode, &QAction::triggered, m_view, &MarkdownView::formatInlineCode);
    connect(ui->actionCodeBlock, &QAction::triggered, m_view, &MarkdownView::formatCodeBlock);
    connect(ui->actionComment, &QAction::triggered, m_view, &MarkdownView::formatComment);
    connect(ui->actionOrderedList, &QAction::triggered, m_view, &MarkdownView::formatOrderedList);
    connect(ui->actionUnorderedList, &QAction::triggered, m_view, &MarkdownView::formatUnorderedList);
    connect(ui->actionBlockquote, &QAction::triggered, m_view, &MarkdownView::formatBlockquote);
    connect(ui->actionHyperlink, &QAction::triggered, m_view, &MarkdownView::formatHyperlink);
    connect(ui->actionImage, &QAction::triggered, m_view, &MarkdownView::formatImage);
    connect(ui->actionNewParagraph, &QAction::triggered, m_view, &MarkdownView::formatNewParagraph);
    connect(ui->actionHorizontalRule, &QAction::triggered, m_view, &MarkdownView::formatHorizontalRule);
    connect(ui->actionHeader1, &QAction::triggered, m_view, &MarkdownView::formatHeader1);
    connect(ui->actionHeader2, &QAction::triggered, m_view, &MarkdownView::formatHeader2);
    connect(ui->actionHeader3, &QAction::triggered, m_view, &MarkdownView::formatHeader3);
    connect(ui->actionHeader4, &QAction::triggered, m_view, &MarkdownView::formatHeader4);
    connect(ui->actionHeader5, &QAction::triggered, m_view, &MarkdownView::formatHeader5);
    connect(ui->actionHeader6, &QAction::triggered, m_view, &MarkdownView::formatHeader6);
    connect(ui->actionShiftRight, &QAction::triggered, m_view, &MarkdownView::formatShiftRight);
    connect(ui->actionShiftLeft, &QAction::triggered, m_view, &MarkdownView::formatShiftLeft);
        
    InitMarkdownEngineActions();
    InitPreviewThemeActions();
    InitCodeBlockStyleActions();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onMarkdownEngineChanged()
{
    ActionLabelMap markdownEngineMap = {
        { ui->actionGoldmark, "Goldmark"},
        { ui->actionLute, "Lute"},
        };
    auto action = qobject_cast<QAction*>(sender());
    auto name = markdownEngineMap[action];
    Q_ASSERT(g_settings);
    g_settings->setMarkdownEngine(name);
    Q_ASSERT(m_view);
    m_view->updateMarkdownEngine();
    m_view->forceConvert();
}

void MainWindow::onPreviewThemeChanged()
{
    ActionLabelMap previewThemeMap = {
        { ui->actionDefault,"默认" },
        { ui->actionOrange ,"橙心" },
        { ui->actionInk    ,"墨黑" },
        { ui->actionPurple ,"姹紫" },
        { ui->actionGreen  ,"绿意" },
        { ui->actionBlue   ,"嫩青" },
        { ui->actionRed    ,"红绯" },
        };
    auto action = qobject_cast<QAction*>(sender());
    auto name = previewThemeMap[action];
    Q_ASSERT(g_settings);
    g_settings->setPreviewTheme(name);
    Q_ASSERT(m_view);
    m_view->setThemeStyle();
    m_view->forceConvert();
}

void MainWindow::onCodeBlockStyleChanged()
{

}

void MainWindow::on_actionExit_triggered()
{
    QCoreApplication::quit();
}

void MainWindow::on_actionContent_triggered()
{
    QDesktopServices::openUrl(QUrl("https://minidump.info/KarenMeu/"));
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, tr("KarenMeu"),
                       tr("KarenMeu is a Markdown based Wechat public account article editor for programmers."));
}

void MainWindow::on_actionPreference_triggered()
{
    PreferenceDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted)
    {
        m_view->setThemeStyle();
        m_view->updateMarkdownEngine();
        m_view->forceConvert();
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_view->maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::InitMarkdownEngineActions()
{
    LabelActionMap markdownEngineMap = {
        { "Goldmark", ui->actionGoldmark},
        { "Lute", ui->actionLute},
    };
    auto *actionGroup = new QActionGroup(this);
    actionGroup->setExclusive(true);
    for (const auto action : markdownEngineMap)
    {
        actionGroup->addAction(action);
        connect(action, &QAction::triggered, this, &MainWindow::onMarkdownEngineChanged);
    }
    auto action = markdownEngineMap[g_settings->markdownEngine()];
    action->setChecked(true);
}

void MainWindow::InitPreviewThemeActions()
{
    LabelActionMap previewThemeMap = {
        { "默认", ui->actionDefault },
        { "橙心", ui->actionOrange },
        { "墨黑", ui->actionInk },
        { "姹紫", ui->actionPurple },
        { "绿意", ui->actionGreen },
        { "嫩青", ui->actionBlue },
        { "红绯", ui->actionRed },
    };
    auto *actionGroup = new QActionGroup(this);
    actionGroup->setEnabled(true);
    for (const auto action : previewThemeMap)
    {
        actionGroup->addAction(action);
        connect(action, &QAction::triggered, this, &MainWindow::onPreviewThemeChanged);
    }
    auto action = previewThemeMap[g_settings->previewTheme()];
    action->setChecked(true);
}

void MainWindow::InitCodeBlockStyleActions()
{
    LabelActionMap codeBlockStyleMap = {
        
    };
    auto *actionGroup = new QActionGroup(this);
    actionGroup->setEnabled(true);
    for (const auto action : codeBlockStyleMap)
    {
        actionGroup->addAction(action);
        connect(action, &QAction::triggered, this, &MainWindow::onCodeBlockStyleChanged);
    }
    auto action = codeBlockStyleMap[g_settings->codeBlockStyle()];
    action->setChecked(true);
}
