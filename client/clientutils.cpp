#include <QFileInfo>
#include <QPlainTextEdit>
#include <QStandardPaths>

#include "clientutils.h"

#include "settings.h"

namespace ClientUtils
{
    void InitializeWidgetFont(QWidget *widget)
    {
        QFont font(widget->font());
        font.setWeight(QFont::Normal);
        font.setItalic(false);
        font.setPointSize(g_settings->codeEditorFontPointSize());

        QStringList fonts;
#if defined(Q_OS_WIN)
        fonts << "Microsoft YaHei UI";
#elif defined(Q_OS_MAC)
        fonts << "PingFang SC"
              << "PingFang HK"
              << "PingFang TC";
#else
        fonts << "WenQuanYi Micro Hei";
#endif
        fonts << widget->font().families();
        QFont::insertSubstitutions(widget->font().family(), fonts);
        font.setFamilies(fonts);
        widget->setFont(font);
    }

    QString getDefaultFileSaveOpenDirectory()
    {
        auto      lastOpenedFilePath = g_settings->getLastOpenedFilePath();
        QFileInfo fi(lastOpenedFilePath);
        auto      dir = fi.absolutePath();
        if (dir.isEmpty())
        {
            dir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
        }
        return dir;
    }

} // namespace ClientUtils
