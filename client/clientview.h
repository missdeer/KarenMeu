#ifndef MARKDOWNVIEW_H
#define MARKDOWNVIEW_H

#include <QWidget>

#include "rendereddocument.h"

QT_FORWARD_DECLARE_CLASS(QSplitter);
QT_FORWARD_DECLARE_CLASS(QWebEngineView);
QT_FORWARD_DECLARE_CLASS(QTimer);
QT_FORWARD_DECLARE_CLASS(QResizeEvent);
QT_FORWARD_DECLARE_CLASS(QWebEnginePage);
QT_FORWARD_DECLARE_CLASS(QNetworkAccessManager);
class MarkdownEditor;
class PreviewThemeEditor;
class PlantUMLUrlCodec;
class PlantUMLRunner;
class FileCache;

class ClientView : public QWidget
{
    Q_OBJECT
public:
    explicit ClientView(QNetworkAccessManager *nam, FileCache *fileCache, QWidget *parent = nullptr);
    ~ClientView() override;
    void                          forceConvert();
    [[nodiscard]] bool            maybeSave();
    void                          updatePreviewTheme();
    void                          updatePreviewMode();
    void                          updateMacStyleCodeBlock();
    void                          openFromFile(const QString &fileName);
    void                          setInitialDocument(const QString &content);
    [[nodiscard]] MarkdownEditor *editor();
    [[nodiscard]] QString         selectedText() const;
    [[nodiscard]] QString         fullText() const;
    [[nodiscard]] QSplitter      *splitter();
    void                          setPreviewHTMLEditor(PreviewThemeEditor *previewHTMLEditor);
    void                          setCustomPreivewThemeEditor(PreviewThemeEditor *customPreivewThemeEditor);
    [[nodiscard]] QWebEnginePage *devToolPage();

signals:
    void formatStrong();
    void formatEmphasize();
    void formatStrikethrough();
    void formatInlineCode();
    void formatCodeBlock();
    void formatComment();
    void formatOrderedList();
    void formatUnorderedList();
    void formatBlockquote();
    void formatHyperlink();
    void formatImage();
    void formatNewParagraph();
    void formatHorizontalRule();
    void formatHeader1();
    void formatHeader2();
    void formatHeader3();
    void formatHeader4();
    void formatHeader5();
    void formatHeader6();
    void formatShiftRight();
    void formatShiftLeft();
    void setCurrentFile(const QString &);
    void insertText(const QString &);
    void contentModified();

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
    void copyTheFirstImage();
    void listImages();
    void copyAsHTML();
    void exportAsHTML();
    void exportAsPDF();

private slots:
    void documentModified();
    void convertTimeout();
    void previewLoadFinished(bool);
    void pdfPrintingFinished(const QString &filePath, bool success);
    void onRequestRemoteImageDone();
    void onRequestLocalImageDone();
    void onAllImagesEmbeded();
    void onGotAllImages(QStringList images);
    void updatePreviewScrollBar();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    bool                   m_modified {false};
    QSplitter             *m_splitter;
    MarkdownEditor        *m_editor;
    QWebEngineView        *m_preview;
    QTimer                *m_convertTimer;
    QNetworkAccessManager *m_nam;
    PreviewThemeEditor    *m_previewHTMLEditor;
    PreviewThemeEditor    *m_customPreivewThemeEditor;
    PlantUMLUrlCodec      *m_plantUMLUrlCodec {nullptr};
    FileCache             *m_fileCache;
    QString                m_savePath;
    RenderedDocument       m_renderedContent;
    RenderedDocument       m_themeStyle;
    RenderedDocument       m_wxboxWidth;
    RenderedDocument       m_macStyleCodeBlock;
    void                   saveToFile(const QString &savePath);
    void                   renderMarkdownToHTML();
    void                   doRendering(const QByteArray &ba, QList<QByteArray> &metaDataLines, std::map<QString, QString> &images);
    void                   setRenderedHTML(const QString &html);
    void                   downloadImages(const std::map<QString, QString> &images);
    void                   preprocessMarkdown(QList<QByteArray>          &lines,
                                              QList<QByteArray>          &metaDataLines,
                                              std::map<QString, QString> &images,
                                              std::map<QString, QString> &imagesToDownload);
    void preprocessPlantUML(QList<QByteArray> &lines, std::map<QString, QString> &images, std::map<QString, QString> &imagesToDownload);
    void preprocessGraphviz(QList<QByteArray> &lines, std::map<QString, QString> &images, std::map<QString, QString> &imagesToDownload);

    enum DocumentType
    {
        MARKDOWN,
        PLANTUML,
        GRAPHVIZ,
    };

    [[nodiscard]] DocumentType guessDocumentType(QList<QByteArray> &lines);
};

#endif // MARKDOWNVIEW_H
