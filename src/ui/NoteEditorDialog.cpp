#include "NoteEditorDialog.h"

#include <QVBoxLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QDialogButtonBox>

NoteEditorDialog::NoteEditorDialog(Note &note, QWidget *parent)
    : QDialog(parent), m_note(note)
{
    setWindowTitle(tr("Edit note"));

    auto edTitle = new QLineEdit();
    edTitle->setPlaceholderText(tr("Title"));
    edTitle->setText(note.getTitle());

    auto edBody = new QTextEdit();
    edBody->setPlaceholderText(tr("Note body"));
    edBody->setText(note.getBody());

    auto ctrlBtn = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    auto layout = new QVBoxLayout();
    layout->addWidget(edTitle);
    layout->addWidget(edBody);
    layout->addWidget(ctrlBtn);

    setLayout(layout);

    connect(ctrlBtn, &QDialogButtonBox::accepted, [=]() {
        m_note.setTitle(edTitle->text());
        m_note.setBody(edBody->toPlainText());
        accept();
    });

    connect(ctrlBtn, &QDialogButtonBox::rejected, this, &QDialog::reject);
}
