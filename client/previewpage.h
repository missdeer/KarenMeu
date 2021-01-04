#ifndef PREVIEWPAGE_H
#define PREVIEWPAGE_H

#include <QWebEnginePage>

class PreviewPage : public QWebEnginePage
{
    Q_OBJECT
public:
    explicit PreviewPage(QObject *parent = nullptr);
    void refreshImages();
    void refreshImage(const QString &imgAlt, const QString &imgSrc);
    void inlineImages();

signals:
    void embeded();
    void allImagesEmbeded();
public slots:
    void onEditorScrollMoved(int pos, int max);
    void onEmbeded();

protected:
    bool acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame);

private:
    int  m_imageCount;
    int  m_imageHandledCount;
    void embedImages(const QStringList &images);
    void embedImage(const QString &from, const QString &to);
};

#endif // PREVIEWPAGE_H
