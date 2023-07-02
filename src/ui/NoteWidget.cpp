#include "NoteWidget.h"
#include "App.h"

#include "backend/provider/NoteProvider.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFile>

constexpr static auto TitleObjName = "title";
constexpr static auto ContentObjName = "content";
constexpr static auto DateObjName = "date";

inline static auto hSeparator() {
    auto line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    return line;
}

NoteWidget::NoteWidget(const Note &note, QWidget *parent)
    : QWidget(parent), m_noteId(note.getId())
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    auto layout = new QVBoxLayout();
    layout->setSpacing(6);
    layout->setContentsMargins(12, 12, 12, 12);

    layout->addLayout(mkHeader());
    layout->addWidget(hSeparator());
    layout->addLayout(mkContent());
    layout->addWidget(hSeparator());
    layout->addLayout(mkFooter());

    setLayout(layout);
    setObjectName("NoteWidget");

    display(note);

    QFile styleFile(":/styles/NoteWidget.qss");
    styleFile.open(QFile::ReadOnly);
    setStyleSheet(styleFile.readAll());
}

void NoteWidget::display(const Note &note) {
    findChild<QLabel*>(TitleObjName)->setText(note.getTitle());
    findChild<QLabel*>(ContentObjName)->setText(note.getBody());
    findChild<QLabel*>(DateObjName)->setText(note.getDate().toString());
}

const QString& NoteWidget::getNoteId() const {
    return m_noteId;
}

QLayout* NoteWidget::mkHeader() {
    auto title = new QLabel("Title");
    title->setObjectName(TitleObjName);
    title->setWordWrap(true);

    auto btnRemove = new QPushButton();
    btnRemove->setIcon(App::getInstance()->fa().icon(fa::fa_solid, fa::fa_minus));
    btnRemove->setFlat(true);
    btnRemove->setStatusTip(tr("Remove note"));

    auto layout = new QHBoxLayout();
    layout->addWidget(title);
    layout->addWidget(btnRemove, 0, Qt::AlignRight);

    connect(btnRemove, &QPushButton::clicked, [this]() {
        NoteProvider::getInstance().removeNote(getNoteId());
    });

    return layout;
}

QLayout* NoteWidget::mkContent() {
    auto content = new QLabel("Content");
    content->setObjectName(ContentObjName);
    content->setWordWrap(true);

    auto layout = new QVBoxLayout();
    layout->addWidget(content);

    return layout;
}

QLayout* NoteWidget::mkFooter() {
    auto date = new QLabel("Footer");
    date->setObjectName(DateObjName);

    auto layout = new QHBoxLayout();
    layout->addWidget(date);

    return layout;
}
