#include <QtCore>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QCloseEvent>
#include "markdownview.h"
#include "preferencedialog.h"
#include "renderer.h"
#include "settings.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

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
    ApplySettingsToRenderer();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionExit_triggered()
{
    qApp->quit();
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

void MainWindow::ApplySettingsToRenderer()
{
    QByteArray theme;
    const QString& themeFile = g_settings->previewTheme();
    QMap<QString, QString> m = {
        { "墨黑", "black.css" },
        { "姹紫", "purple.css" },
        { "嫩青", "blue.css" },
        { "橙心", "orange.css" },
        { "红绯", "red.css" },
        { "绿意", "green.css" },
        { "默认", "default.css" },
        };
    QFile f(":/rc/theme/" + m[themeFile]);
    if (f.open(QIODevice::ReadOnly))
    {
        theme = f.readAll();
        f.close();
    }
    qDebug() << "theme:" << themeFile << QString(theme);
    GoString rawThemeContent { (const char *)theme.data(), theme.length()};
    SetPreviewTheme(rawThemeContent);
    
    const QString& style = g_settings->codeBlockStyle();
    qDebug() << "code block style:" << style;
    GoString codeblockStyle { (const char *)style.toStdString().c_str(), style.length()};
    SetCodeBlockStyle(codeblockStyle);
    
    //SetHTMLTempalte();
    
    EnableLineNumbers(true);    
}

void MainWindow::on_actionPreference_triggered()
{
    PreferenceDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted)
    {
        ApplySettingsToRenderer();
        
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
