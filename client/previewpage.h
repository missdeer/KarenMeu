#ifndef PREVIEWPAGE_H
#define PREVIEWPAGE_H

#include <QWebEnginePage>

class PreviewPage : public QWebEnginePage
{
    Q_OBJECT
public:
    explicit PreviewPage(QObject *parent = nullptr) : QWebEnginePage(parent) {}

public slots:
    void onEditorScrollMoved(int pos, int max);

protected:
    bool acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame);
};

#endif // PREVIEWPAGE_H
