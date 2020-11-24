#include <QFileOpenEvent>

#include "macapplication.h"

MacApplication::MacApplication(int &argc, char **argv) : QApplication(argc, argv) {}

MacApplication::~MacApplication() {}

bool MacApplication::event(QEvent *event)
{
    if (event->type() == QEvent::FileOpen)
    {
        QFileOpenEvent *openEvent = static_cast<QFileOpenEvent *>(event);

        emit openFile(openEvent->file());
    }

    return QApplication::event(event);
}
