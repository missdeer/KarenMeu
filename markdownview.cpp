#include <QVBoxLayout>
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
}

void MarkdownView::openDocument()
{
    
}

void MarkdownView::saveDocument()
{
    
}

void MarkdownView::saveAsDocument()
{
    
}

void MarkdownView::newDocument()
{
    
}

void MarkdownView::copy()
{
    if (m_editor->hasFocus())
        m_editor->copy();
}

void MarkdownView::cut()
{
    if (m_editor->hasFocus())
        m_editor->cut();
}

void MarkdownView::paste()
{
    if (m_editor->hasFocus())
        m_editor->paste();
}
