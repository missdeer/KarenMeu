#include <QBrush>
#include <QFileInfo>
#include <QIcon>
#include <QImage>
#include <QPainter>
#include <QPixmap>
#include <QPlainTextEdit>
#include <QStandardPaths>
#include <QSvgRenderer>

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
        QFileInfo fileInfo(lastOpenedFilePath);
        auto      dir = fileInfo.absolutePath();
        if (dir.isEmpty())
        {
            dir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
        }
        return dir;
    }

    void setHtmlContent(QPlainTextEdit *editor, const QString &html)
    {
        Q_ASSERT(editor);
        editor->clear();
        editor->appendHtml(html);
        editor->moveCursor(QTextCursor::Start);
    }

    QIcon iconFromSvg(const QSize &size, const QString &path)
    {
        QPixmap     pixmap(size);
        QPainter    painter(&pixmap);
        const QRect bounding_rect(QPoint(0, 0), size);

        if (!path.isEmpty())
        {
            painter.setRenderHint(QPainter::Antialiasing, true);

            painter.setBrush(QBrush(Qt::white, Qt::SolidPattern));
            painter.setPen(Qt::NoPen);
            painter.drawRect(bounding_rect);

            if (path.endsWith(".svg", Qt::CaseInsensitive))
            {
                QSvgRenderer svg(path);
                QSize        target_size = svg.defaultSize();
                target_size.scale(size, Qt::KeepAspectRatio);
                QRect target_rect = QRect(QPoint(0, 0), target_size);
                target_rect.translate(bounding_rect.center() - target_rect.center());
                svg.render(&painter, target_rect);
            }
            else
            {
                QImage image(path);

                if (image.width() > image.height())
                {
                    image = image.scaledToWidth(size.width());
                }
                else
                {
                    image = image.scaledToHeight(size.height());
                }
                painter.drawImage(0, 0, image);
            }
        }
        else
        {
            painter.setBrush(QBrush(Qt::white, Qt::SolidPattern));
            painter.setPen(Qt::NoPen);
            painter.drawRect(bounding_rect);

            const int margin      = 5;
            QRect     target_rect = bounding_rect.adjusted(margin, margin, -margin, -margin);
            painter.setPen(Qt::SolidLine);
            painter.drawRect(target_rect);
            painter.drawLine(target_rect.topLeft(), target_rect.bottomRight() + QPoint(1, 1));
            painter.drawLine(target_rect.bottomLeft() + QPoint(0, 1), target_rect.topRight() + QPoint(1, 0));
        }

        QIcon icon;
        icon.addPixmap(pixmap);
        return icon;
    }
} // namespace ClientUtils
