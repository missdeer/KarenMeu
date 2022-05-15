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
    void copy1stImage();
    void getImages();
signals:
    void embeded();
    void allImagesEmbeded();
    void gotAllImages(QStringList);
public slots:
    void onEditorScrollMoved(int pos, int max);
    void onEmbeded();

private slots:
    void onRequestRemoteImageDoneThenCopy();

protected:
    bool acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame);

private:
    int                    m_imageCount {0};
    int                    m_imageHandledCount {0};
    QNetworkAccessManager *m_nam;
    void                   embedImages(const QStringList &images);
    void                   embedImage(const QString &from, const QString &to);
    QByteArray             compressPNG(const QByteArray &ba);
    void                   copyImage(const QString &image);
    void                   copyRemoteImage(const QUrl &url);
    void                   copyLocalImage(const QString &filePath);
    void                   copyFromImageData(const QString &data);
};

#endif // PREVIEWPAGE_H
