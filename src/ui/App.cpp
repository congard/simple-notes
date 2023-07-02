#include "App.h"
#include "backend/FirebaseApp.h"
#include "ErrorBox.h"

#include <QThread>

App* App::m_instance;

App::App(int &argc, char **argv)
    : m_app(argc, argv),
      m_fa(&m_app)
{
    assert(m_instance == nullptr);
    m_instance = this;
    m_fa.initFontAwesome();

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

    // It doesn't matter, where object lives.
    // It is safe to emit signals from another thread.
    // Warning: receiver must be specified explicitly
    // do not omit 3rd parameter!
    connect(this, &App::invokeLater, this, [](QRunnable *runnable) {
        runnable->run();

        if (runnable->autoDelete()) {
            delete runnable;
        }
    });
}

fa::QtAwesome& App::fa() {
    return m_fa;
}

int App::exec() {
    int retCode = QApplication::exec();
    FirebaseApp::destroy();
    return retCode;
}

App* App::getInstance() {
    return m_instance;
}
