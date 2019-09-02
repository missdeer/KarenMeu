#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QSplitter>
#include <QWebEngineView>
#include "markdowneditor.h"
#include "markdownview.h"

MarkdownView::MarkdownView(QWidget *parent) 
    : QWidget(parent)
    , m_splitter(new QSplitter(this))
    , m_editor(new MarkdownEditor(this))
    , m_preview(new QWebEngineView(this))
{
    m_splitter->addWidget(m_editor);
    m_splitter->addWidget(m_preview);
    auto *layout = new QVBoxLayout;
    layout->addWidget(m_splitter);
    
    setLayout(layout);
    m_splitter->setSizes(QList<int>() << width()/2 << width() /2);
    m_editor->initialize();
}

void MarkdownView::openDocument()
{
    newDocument();
    
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Markdown file"),
                                                    "",
                                                    tr("Markdown files (*.md *.markdown);;All files (*.*)"));
    if (QFile::exists(fileName))
    {
        QFile f(fileName);
        if (f.open(QIODevice::ReadOnly))
        {
            QByteArray ba = f.readAll();
            m_editor->setContent(ba);
            m_editor->setSavePoint();
            f.close();
        }
    }
}

void MarkdownView::saveDocument()
{
    if (m_savePath.isEmpty())
        saveAsDocument();
    else
        saveToFile(m_savePath);
}

void MarkdownView::saveAsDocument()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Markdown file"),
                                                    "",
                                                    tr("Markdown file (*.md);;All files (*.*)"));
    
    if (fileName.isEmpty())
        return;
    
    m_savePath = fileName;    
    saveToFile(m_savePath);
}

void MarkdownView::newDocument()
{
    if (m_editor->modify())
    {
        // prompt user to save document first
        int res = QMessageBox::question(this, tr("Modified document has not been saved"), 
                                        tr("Do you want to save it?"),
                                        QMessageBox::Yes | QMessageBox::No);
        if (res == QMessageBox::Yes)
            saveDocument();
    }
    m_editor->clear();
    m_editor->setSavePoint();
}

void MarkdownView::copy()
{
    if (m_editor->hasFocus())
        m_editor->copy();
    else {
        m_preview->triggerPageAction(QWebEnginePage::Copy);
    }
}

void MarkdownView::cut()
{
    if (m_editor->hasFocus())
        m_editor->cut();
    else {
        m_preview->triggerPageAction(QWebEnginePage::Cut);
    }
}

void MarkdownView::paste()
{
    if (m_editor->hasFocus())
        m_editor->paste();
    else {
        m_preview->triggerPageAction(QWebEnginePage::Paste);
    }
}

void MarkdownView::selectAll()
{
    if (m_editor->hasFocus())
        m_editor->selectAll();
    else {
        m_preview->triggerPageAction(QWebEnginePage::SelectAll);
    }
}

void MarkdownView::undo()
{
    if (m_editor->hasFocus())
        m_editor->undo();
    else {
        m_preview->triggerPageAction(QWebEnginePage::Undo);
    }
}

void MarkdownView::redo()
{
    if (m_editor->hasFocus())
        m_editor->redo();
    else {
        m_preview->triggerPageAction(QWebEnginePage::Redo);
    }
}

void MarkdownView::saveToFile(const QString &savePath)
{
    QFile f(savePath);
    if (f.open(QIODevice::Truncate | QIODevice::WriteOnly))
    {
        f.write(m_editor->content());
        f.close();
    }
}
