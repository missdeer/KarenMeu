#include <QtCore>
#include <QCoreApplication>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QCloseEvent>
#include <QAction>
#include <QMap>
#include <QHash>
#include <QLabel>
#include <QComboBox>
#include <QFileInfo>
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
    connect(m_view, &MarkdownView::setCurrentFile, this, &MainWindow::setCurrentFile);
            
    ui->menuView->addAction(ui->fileToolbar->toggleViewAction());
    ui->menuView->addAction(ui->editToolbar->toggleViewAction());
    ui->menuView->addAction(ui->formatToolbar->toggleViewAction());
    
    
    for (int i = 0; i < MaxRecentFiles; ++i) {
        recentFileActs[i] = new QAction(this);
        recentFileActs[i]->setVisible(false);
        connect(recentFileActs[i], &QAction::triggered, this, &MainWindow::openRecentFile);
        ui->menuRecentFiles->addAction(recentFileActs[i]);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openFile(const QString &fileName)
{
    Q_ASSERT(m_view);

    m_view->openFromFile(fileName);
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

void MainWindow::updateRecentFileActions()
{
    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();

    int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

    for (int i = 0; i < numRecentFiles; ++i) 
    {
        QString text = tr("%1. %2").arg(i + 1).arg(strippedName(files[i]));
        recentFileActs[i]->setText(text);
        recentFileActs[i]->setData(files[i]);
        recentFileActs[i]->setVisible(true);
    }
    for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
        recentFileActs[j]->setVisible(false);

    //separatorAct->setVisible(numRecentFiles > 0);
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    m_curFile = fileName;
    setWindowFilePath(m_curFile);
    
    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();
    files.removeAll(fileName);
    files.prepend(fileName);
    while (files.size() > MaxRecentFiles)
        files.removeLast();
    
    settings.setValue("recentFileList", files);
    
    updateRecentFileActions();
}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void MainWindow::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
        openFile(action->data().toString());
}

void MainWindow::on_actionClearRecentFilesList_triggered()
{
    QSettings settings;
    QStringList files;
    settings.setValue("recentFileList", files);
    
    updateRecentFileActions();
}
