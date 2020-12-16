#ifndef PREVIEWTHEMEEDITOR_H
#define PREVIEWTHEMEEDITOR_H

#include <QWidget>

class ScintillaConfig;
class ScintillaEdit;

class PreviewThemeEditor : public QWidget
{
    Q_OBJECT
public:
    explicit PreviewThemeEditor(QWidget *parent = nullptr);
    void       initialize(const QString &lexer);
    void       setContent(const QByteArray &content);
    QByteArray content();
    void       clearAll();
signals:
    void contentModified();
private slots:

    void modified(int type, int position, int length, int linesAdded, const QByteArray &text, int line, int foldNow, int foldPrev);

private:
    ScintillaEdit *  m_editor;
    ScintillaConfig *m_sc;
    QString          m_lexer;
};

#endif // PREVIEWTHEMEEDITOR_H
