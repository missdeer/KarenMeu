#ifndef MACAPPLICATION_H
#define MACAPPLICATION_H

#include <QApplication>

QT_FORWARD_DECLARE_CLASS(QEvent);

class MacApplication : public QApplication
{
    Q_OBJECT
public:
    MacApplication(int &argc, char **argv);
    ~MacApplication();

    bool event(QEvent *event) override;

signals:
    void openFile(QString);
};

#endif // MACAPPLICATION_H
