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
        
    UpdateMarkdownEngineActions(true);
    UpdatePreviewThemeActions(true);
    UpdateCodeBlockStyleActions(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onMarkdownEngineChanged()
{
    ActionLabelMap markdownEngineMap = {
        { ui->actionGoldmark, "Goldmark"},
        { ui->actionLute,     "Lute"    },
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
    ActionLabelMap codeBlockStyleMap = {
        {ui->actionAbap            ,"abap"              },
        {ui->actionAlgol           ,"algol"             },
        {ui->actionAlgolNu         ,"algol_nu"          },
        {ui->actionArduino         ,"arduino"           },
        {ui->actionAutumn          ,"autumn"            },
        {ui->actionBorland         ,"borland"           },
        {ui->actionBW              ,"bw"                },
        {ui->actionColorful        ,"colorful"          },
        {ui->actionDracula         ,"dracula"           },
        {ui->actionEmacs           ,"emacs"             },
        {ui->actionFriendly        ,"friendly"          },
        {ui->actionFruity          ,"fruity"            },
        {ui->actionGithub          ,"github"            },
        {ui->actionIgor            ,"igor"              },
        {ui->actionLovelace        ,"lovelace"          },
        {ui->actionManni           ,"manni"             },
        {ui->actionMonokai         ,"monokai"           },
        {ui->actionMonokaiLight    ,"monokailight"      },
        {ui->actionMurphy          ,"murphy"            },
        {ui->actionNative          ,"native"            },
        {ui->actionParaisoDark     ,"paraiso-dark"      },
        {ui->actionParaisoLight    ,"paraiso-light"     },
        {ui->actionPastie          ,"pastie"            },
        {ui->actionPerldoc         ,"perldoc"           },
        {ui->actionPygments        ,"pygments"          },
        {ui->actionRainbowDash     ,"rainbow_dash"      },
        {ui->actionRRT             ,"rrt"               },
        {ui->actionSolarizedDark   ,"solarized-dark"    },
        {ui->actionSolarizedDark256,"solarized-dark256" },
        {ui->actionSolarizedLight  ,"solarized-light"   },
        {ui->actionSwapOff         ,"swapoff"           },
        {ui->actionTango           ,"tango"             },
        {ui->actionTrac            ,"trac"              },
        {ui->actionVIM             ,"vim"               },
        {ui->actionVS              ,"vs"                },
        {ui->actionXcode           ,"xcode"             },
        };
    auto action = qobject_cast<QAction*>(sender());
    auto name = codeBlockStyleMap[action];
    Q_ASSERT(g_settings);
    g_settings->setCodeBlockStyle(name);
    Q_ASSERT(m_view);
    m_view->forceConvert();
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
        UpdateMarkdownEngineActions(false);
        UpdatePreviewThemeActions(false);
        UpdateCodeBlockStyleActions(false);
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

void MainWindow::UpdateMarkdownEngineActions(bool first)
{
    LabelActionMap markdownEngineMap = {
        { "Goldmark", ui->actionGoldmark},
        { "Lute", ui->actionLute},
    };
    if (first)
    {
        auto *actionGroup = new QActionGroup(this);
        actionGroup->setExclusive(true);
        for (const auto action : markdownEngineMap)
        {
            actionGroup->addAction(action);
            connect(action, &QAction::triggered, this, &MainWindow::onMarkdownEngineChanged);
        }
    }
    auto action = markdownEngineMap[g_settings->markdownEngine()];
    action->setChecked(true);
}

void MainWindow::UpdatePreviewThemeActions(bool first)
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
    if (first)
    {
        auto *actionGroup = new QActionGroup(this);
        actionGroup->setEnabled(true);
        for (const auto action : previewThemeMap)
        {
            actionGroup->addAction(action);
            connect(action, &QAction::triggered, this, &MainWindow::onPreviewThemeChanged);
        }
    }
    auto action = previewThemeMap[g_settings->previewTheme()];
    action->setChecked(true);
}

void MainWindow::UpdateCodeBlockStyleActions(bool first)
{
    LabelActionMap codeBlockStyleMap = {
        {"abap", ui->actionAbap},
        {"algol", ui->actionAlgol},
        {"algol_nu", ui->actionAlgolNu},
        {"arduino", ui->actionArduino},
        {"autumn", ui->actionAutumn},
        {"borland", ui->actionBorland},
        {"bw", ui->actionBW},
        {"colorful", ui->actionColorful},
        {"dracula", ui->actionDracula},
        {"emacs", ui->actionEmacs},
        {"friendly", ui->actionFriendly},
        {"fruity", ui->actionFruity},
        {"github", ui->actionGithub},
        {"igor", ui->actionIgor},
        {"lovelace", ui->actionLovelace},
        {"manni", ui->actionManni},
        {"monokai", ui->actionMonokai},
        {"monokailight", ui->actionMonokaiLight},
        {"murphy", ui->actionMurphy},
        {"native", ui->actionNative},
        {"paraiso-dark", ui->actionParaisoDark},
        {"paraiso-light", ui->actionParaisoLight},
        {"pastie", ui->actionPastie},
        {"perldoc", ui->actionPerldoc},
        {"pygments", ui->actionPygments},
        {"rainbow_dash", ui->actionRainbowDash},
        {"rrt", ui->actionRRT},
        {"solarized-dark", ui->actionSolarizedDark},
        {"solarized-dark256", ui->actionSolarizedDark256},
        {"solarized-light", ui->actionSolarizedLight},
        {"swapoff", ui->actionSwapOff},
        {"tango", ui->actionTango},
        {"trac", ui->actionTrac},
        {"vim", ui->actionVIM},
        {"vs", ui->actionVS},
        {"xcode", ui->actionXcode},
    };
    if (first)
    {
        auto *actionGroup = new QActionGroup(this);
        actionGroup->setEnabled(true);
        for (const auto action : codeBlockStyleMap)
        {
            actionGroup->addAction(action);
            connect(action, &QAction::triggered, this, &MainWindow::onCodeBlockStyleChanged);
        }
    }
    auto action = codeBlockStyleMap[g_settings->codeBlockStyle()];
    action->setChecked(true);
}
