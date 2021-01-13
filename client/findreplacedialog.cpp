#include <QTextCursor>
#include <QTextDocument>
#include <QtCore>

#include "findreplacedialog.h"

#include <vtextedit/vtextedit.h>

#include "ui_findreplacedialog.h"

FindReplaceDialog::FindReplaceDialog(vte::VTextEditor *editor, QWidget *parent) : QDialog(parent), ui(new Ui::FindReplaceDialog), m_editor(editor)
{
    ui->setupUi(this);
}

FindReplaceDialog::~FindReplaceDialog()
{
    delete ui;
}

vte::FindFlags FindReplaceDialog::getFindFlags()
{
    vte::FindFlags flags = vte::None;
    if (ui->cbBackward->isChecked())
        flags |= vte::FindBackward;
    if (ui->cbCaseSensitive->isChecked())
        flags |= vte::CaseSensitive;
    if (ui->cbWholeWordMatches->isChecked())
        flags |= vte::WholeWordOnly;
    if (ui->cbRegularExpression->isChecked())
        flags |= vte::RegularExpression;

    return flags;
}

void FindReplaceDialog::on_btnFind_clicked()
{
    if (ui->edtFindKeyword->text().isEmpty())
        return;

    vte::FindFlags flags = getFindFlags();

    auto textedit = m_editor->getTextEdit();
    Q_ASSERT(textedit);
    auto textCursor = textedit->textCursor();
    auto fr         = m_editor->findText(ui->edtFindKeyword->text(), flags, textCursor.position());
    qDebug() << flags << textCursor.position() << fr.m_currentMatchIndex << fr.m_totalMatches << fr.m_wrapped;
}

void FindReplaceDialog::on_btnReplace_clicked()
{
    if (ui->edtFindKeyword->text().isEmpty())
        return;
    vte::FindFlags flags = getFindFlags();

    auto textedit = m_editor->getTextEdit();
    Q_ASSERT(textedit);
    auto textCursor = textedit->textCursor();
    auto fr         = m_editor->replaceText(ui->edtFindKeyword->text(), flags, ui->edtReplaceText->text(), textCursor.position());
    qDebug() << flags << textCursor.position() << fr.m_currentMatchIndex << fr.m_totalMatches << fr.m_wrapped;
}

void FindReplaceDialog::on_btnReplaceAll_clicked()
{
    if (ui->edtFindKeyword->text().isEmpty())
        return;
    vte::FindFlags flags = getFindFlags();

    auto textedit = m_editor->getTextEdit();
    Q_ASSERT(textedit);
    auto textCursor = textedit->textCursor();
    auto fr         = m_editor->replaceAll(ui->edtFindKeyword->text(), flags, ui->edtReplaceText->text(), textCursor.position());
    qDebug() << flags << textCursor.position() << fr.m_currentMatchIndex << fr.m_totalMatches << fr.m_wrapped;
}
