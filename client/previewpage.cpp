#include <QDesktopServices>
#include <QtCore>

#include "previewpage.h"

void PreviewPage::onEditorScrollMoved(int pos, int max)
{
    const QString ScrollJavaScript("window.scrollTo(0, document.body.scrollHeight * %1 / %2);");
    runJavaScript(ScrollJavaScript.arg(pos).arg(max));
}

bool PreviewPage::acceptNavigationRequest(const QUrl &url,
                                          QWebEnginePage::NavigationType /*type*/,
                                          bool /*isMainFrame*/)
{
    // Only allow qrc:/index.html.
    if (url.scheme() == QString("qrc"))
        return true;
    QDesktopServices::openUrl(url);
    return false;
}
