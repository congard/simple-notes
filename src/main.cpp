#include "ui/App.h"
#include "ui/MainWindow.h"
#include "backend/FirebaseApp.h"

int main(int argc, char **argv) {
    App app(argc, argv);

    MainWindow loginWindow;
    loginWindow.show();

    return App::exec();
}
