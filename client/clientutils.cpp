#include <QPlainTextEdit>

#include "clientutils.h"

#include "settings.h"

namespace ClientUtils
{
    void InitializeWidgetFont(QWidget *widget)
    {
        QFont font(widget->font());
        font.setWeight(QFont::Normal);
        font.setItalic(false);
        font.setPointSizeF(g_settings->codeEditorFontPointSize());

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
} // namespace ClientUtils
