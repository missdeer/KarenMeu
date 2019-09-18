#include <QFile>
#include "settings.h"
#include "previewthemeeditor.h"
#include "preferencedialog.h"
#include "ui_preferencedialog.h"


PreferenceDialog::PreferenceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferenceDialog)
{
    ui->setupUi(this);
    setupPreviewThemeEditor();
    
    ui->cbPreviewTheme->setCurrentText(g_settings->previewTheme());
    ui->cbCodeBlockStyle->setCurrentText(g_settings->codeBlockStyle());
    ui->cbMarkdownEngine->setCurrentText(g_settings->markdownEngine());
    ui->cbEnableLineNumbers->setChecked(g_settings->enableLineNumbers());
    QFont f;
    f.setFamily(g_settings->codeEditorFontFamily());
    ui->cbCodeEditorFont->setCurrentFont(f);
    ui->sbFontPointSize->setValue(g_settings->codeEditorFontPointSize());
    ui->cbPreviewMode->setCurrentText(g_settings->previewMode());
}

PreferenceDialog::~PreferenceDialog()
{
    delete ui;
}

void PreferenceDialog::accept()
{
    g_settings->setPreviewMode(ui->cbPreviewMode->currentText());
    g_settings->setPreviewTheme(ui->cbPreviewTheme->currentText());
    g_settings->setCodeBlockStyle(ui->cbCodeBlockStyle->currentText());
    g_settings->setMarkdownEngine(ui->cbMarkdownEngine->currentText());
    g_settings->setEnableLineNumbers(ui->cbEnableLineNumbers->isChecked());
    g_settings->setCodeEditorFontFamily(ui->cbCodeEditorFont->currentFont().family());
    g_settings->setCodeEditorFontPointSize(ui->sbFontPointSize->value());
    QDialog::accept();
}

void PreferenceDialog::on_cbPreviewTheme_currentTextChanged(const QString &text)
{
    QMap<QString, QString> m = {
        { "墨黑",    "ink.css" },
        { "姹紫",    "purple.css" },
        { "嫩青",    "cyan.css" },
        { "橙心",    "orangeHeart.css" },
        { "红绯",    "red.css" },
        { "绿意",    "green.css" },
        { "默认",    "default.css" },
        { "Gopher", "gopher.css"},
        };
    QFile f(":/rc/theme/" + m[text]);
    if (f.open(QIODevice::ReadOnly))
    {
        QByteArray ba = f.readAll();
        m_previewThemeEditor->setContent(ba);
        f.close();
    }
}

void PreferenceDialog::setupPreviewThemeEditor()
{
    auto *layout = new QVBoxLayout;
    m_previewThemeEditor = new PreviewThemeEditor(ui->previewThemeEditorContainer);
    layout->addWidget(m_previewThemeEditor);
    layout->setMargin(0);
    ui->previewThemeEditorContainer->setLayout(layout);    
    
    m_previewThemeEditor->initialize();
}
