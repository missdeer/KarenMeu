#ifndef MARKDOWNVIEW_H
#define MARKDOWNVIEW_H

#include <QWidget>

QT_FORWARD_DECLARE_CLASS(QSplitter);
QT_FORWARD_DECLARE_CLASS(QWebEngineView);
class MarkdownEditor;

class MarkdownView : public QWidget
{
    Q_OBJECT
public:
    explicit MarkdownView(QWidget *parent = nullptr);
    
signals:
    
public slots:
    void openDocument();
    void saveDocument();
    void saveAsDocument();
    void newDocument();
    void copy();
    void cut();
    void paste();
    void selectAll();
    void undo();
    void redo();
private:
    QSplitter *m_splitter;
    MarkdownEditor *m_editor;
    QWebEngineView *m_preview;
    QString m_savePath;
    
    void saveToFile(const QString &savePath);
};

#endif // MARKDOWNVIEW_H
