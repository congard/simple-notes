#ifndef SIMPLE_NOTES_ERRORBOX_H
#define SIMPLE_NOTES_ERRORBOX_H

#include <QMessageBox>

#include "backend/Error.h"

class ErrorBox : public QMessageBox {
public:
    explicit ErrorBox(const Error &error, QWidget *parent = nullptr);

    static void handler(const Error &error);
};

#endif //SIMPLE_NOTES_ERRORBOX_H
