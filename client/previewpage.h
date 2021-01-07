#ifndef PREVIEWPAGE_H
#define PREVIEWPAGE_H

#include <QWebEnginePage>

class PreviewPage : public QWebEnginePage
{
    Q_OBJECT
public:
    explicit PreviewPage(QNetworkAccessManager *nam, QObject *parent = nullptr);
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
    int                    m_imageCount;
    int                    m_imageHandledCount;
    QNetworkAccessManager *m_nam;
    void                   embedImages(const QStringList &images);
    void                   embedImage(const QString &from, const QString &to);
    QByteArray             compressPNG(const QByteArray &ba);
};

#endif // PREVIEWPAGE_H
