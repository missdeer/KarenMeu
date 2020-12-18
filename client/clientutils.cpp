#include <QPlainTextEdit>

#include "clientutils.h"

#include "settings.h"

namespace ClientUtils
{
    void InitializePlainTextEditFont(QPlainTextEdit *editor)
    {
        QFont font(editor->font());
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
        fonts << editor->font().families();
        QFont::insertSubstitutions(editor->font().family(), fonts);
        font.setFamilies(fonts);
        editor->setFont(font);
    }
} // namespace ClientUtils
