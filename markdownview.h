#ifndef MARKDOWNVIEW_H
#define MARKDOWNVIEW_H

#include <QWidget>

QT_FORWARD_DECLARE_CLASS(QSplitter);
QT_FORWARD_DECLARE_CLASS(QWebEngineView);
QT_FORWARD_DECLARE_CLASS(QTimer);
class MarkdownEditor;

class MarkdownView : public QWidget
{
    Q_OBJECT
public:
    explicit MarkdownView(QWidget *parent = nullptr);
    void forceConvert();
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

private slots:
    void documentModified();
    void convertTimeout();
private:
    bool m_modified{false};
    QSplitter *m_splitter;
    MarkdownEditor *m_editor;
    QWebEngineView *m_preview;
    QTimer *m_convertTimer;
    QString m_savePath;
    
    void saveToFile(const QString &savePath);
    void convert();
};

#endif // MARKDOWNVIEW_H
