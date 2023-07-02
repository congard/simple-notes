#ifndef SIMPLE_NOTES_APP_H
#define SIMPLE_NOTES_APP_H

#include <QApplication>
#include <QRunnable>
#include <QtAwesome.h>

#include <memory>

#include "backend/Error.h"

class App : public QObject {
    Q_OBJECT

public:
    App(int &argc, char **argv);

    fa::QtAwesome& fa();

    static int exec();
    static App* getInstance();

signals:
    void invokeLater(QRunnable *runnable);

private:
    static App *m_instance;

    QApplication m_app;
    fa::QtAwesome m_fa;
};

#endif //SIMPLE_NOTES_APP_H
