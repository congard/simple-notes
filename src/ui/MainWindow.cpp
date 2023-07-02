#include <QVBoxLayout>
#include <QLabel>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QPushButton>
#include <QScrollArea>
#include <QSettings>

#include "MainWindow.h"
#include "App.h"
#include "NoteWidget.h"
#include "ErrorBox.h"
#include "NoteEditorDialog.h"
#include "AuthDialog.h"
#include "AboutDialog.h"
#include "HelpDialog.h"
#include "Details.h"

#include "backend/provider/AuthProvider.h"
#include "backend/provider/NoteProvider.h"
#include "backend/StatusHandler.h"

namespace ActionName {
constexpr static auto Sign = "action_sign";
constexpr static auto Sync = "action_sync";
}

namespace WidgetName {
constexpr static auto BtnSync = "btn_sync";
constexpr static auto NoteLayout = "noteLayout";
}

namespace SettingsKey {
constexpr static auto Geometry = "MainWindow_geometry";
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(Details::AppName);
    setWindowIcon(QIcon(":/icon.png"));
    setMinimumWidth(200);
    setMinimumHeight(250);

    setMenuBar(mkMenuBar());
    setStatusBar(mkStatusBar());

    auto layout = new QVBoxLayout();
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(mkToolBar());
    layout->addWidget(mkContent());

    auto window = new QWidget();
    window->setLayout(layout);
    setCentralWidget(window);

    connect(&AuthProvider::getInstance(), &AuthProvider::statusChanged, this, &MainWindow::statusChanged);
    connect(&NoteProvider::getInstance(), &NoteProvider::notesUpdated, this, &MainWindow::notesUpdated);

    loadSettings();

    checkAuthStatus();
    notesUpdated();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    saveSettings();
}

void MainWindow::statusChanged(AuthProvider::Status status) {
    switch (status) {
        case AuthProvider::Status::Unknown:
        case AuthProvider::Status::SignedOut:
            signedOut();
            break;
        case AuthProvider::Status::SignedIn:
            signedIn();
            break;
    }
}

void MainWindow::notesUpdated() {
    auto noteLayout = findChild<QVBoxLayout*>(WidgetName::NoteLayout);
    auto widgets = findChildren<NoteWidget*>();

    for (auto note : NoteProvider::getInstance().getNotes()) {
        auto pred = [note](NoteWidget *widget) {
            return widget->getNoteId() == note->getId();
        };

        if (auto it = std::find_if(widgets.begin(), widgets.end(), pred); it != widgets.end()) {
            // update existing
            (*it)->display(*note);
            widgets.erase(it);
        } else {
            // create new
            auto widget = new NoteWidget(*note);
            noteLayout->addWidget(widget);
        }
    }

    // remove deleted
    for (auto widget : widgets) {
        widget->setParent(nullptr);
        delete widget;
    }
}

void MainWindow::signAction() {
    auto &auth = AuthProvider::getInstance();

    if (auth.status() == AuthProvider::Status::SignedOut) {
        showAuthWindow();
    } else {
        auth.signOut();
    }
}

void MainWindow::showAddNoteWindow() {
    Note note;
    NoteEditorDialog editor(note);

    if (editor.exec() == QDialog::Accepted) {
        NoteProvider::getInstance().addNote(note);
    }
}

void MainWindow::showAboutDialog() {
    AboutDialog(this).exec();
}

void MainWindow::showHelpDialog() {
    HelpDialog(this).exec();
}

template<typename R, typename F>
struct Slot {
    R receiver;
    F functionPtr;

    Slot(R receiver, F functionPtr)
        : receiver(receiver), functionPtr(functionPtr) {}
};

// Since Qt 6 'operator+' is deprecated: Use operator| instead
// Keep backward compatibility with Qt 5
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
constexpr int operator|(Qt::Modifier modifier, Qt::Key key) noexcept {
    return static_cast<int>(modifier + key);
}
#endif

QMenuBar* MainWindow::mkMenuBar() {
    auto mkAction = [](
            const QString &title, const QString &statusTip,
            auto slot, const QKeySequence &shortcut = 0)
    {
        auto action = new QAction(title);
        action->setStatusTip(statusTip);
        action->setShortcut(shortcut);

        using SlotType = decltype(slot);

        if constexpr (std::is_invocable_v<SlotType>) {
            connect(action, &QAction::triggered, slot);
        } else {
            connect(action, &QAction::triggered, slot.receiver, slot.functionPtr);
        }

        return action;
    };

    auto menuBar = new QMenuBar();

    {
        auto fileMenu = new QMenu(tr("&File"));

        fileMenu->addAction(mkAction(tr("New note"), tr("Create a new note"),
                                     &MainWindow::showAddNoteWindow,
                                     Qt::CTRL | Qt::Key_N));
        fileMenu->addSeparator();

        auto actionSync = mkAction(tr("Synchronize"), tr("Synchronize"), [this]() { sync(); });
        actionSync->setObjectName(ActionName::Sync);
        actionSync->setEnabled(false);
        fileMenu->addAction(actionSync);

        auto actionSign = mkAction(tr("Sign in / up"),
                                   tr("Sign in or sign up if you haven't been registered yet"),
                                   Slot(this, &MainWindow::signAction));
        actionSign->setObjectName(ActionName::Sign);
        fileMenu->addAction(actionSign);

        fileMenu->addSeparator();
        fileMenu->addAction(mkAction(tr("Exit"), tr("Exit"), Slot(this, &MainWindow::close), Qt::CTRL | Qt::Key_Q));

        menuBar->addMenu(fileMenu);
    }

    {
        auto helpMenu = new QMenu(tr("&Help"));
        helpMenu->addAction(mkAction(tr("About"), tr("About SimpleNotes"),
                                     Slot(this, &MainWindow::showAboutDialog),
                                     Qt::CTRL | Qt::Key_A));
        helpMenu->addAction(mkAction(tr("About Qt"), tr("About Qt"), []() { QApplication::aboutQt(); }));
        helpMenu->addAction(mkAction(tr("Help"), tr("Help"),
                                     Slot(this, &MainWindow::showHelpDialog),
                                     Qt::CTRL | Qt::Key_H));
        menuBar->addMenu(helpMenu);
    }

    return menuBar;
}

QToolBar* MainWindow::mkToolBar() {
    auto spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto btnSync = new QPushButton();
    btnSync->setIcon(App::getInstance()->fa().icon(fa::fa_solid, fa::fa_sync));
    btnSync->setEnabled(false);
    btnSync->setStatusTip(tr("Synchronize"));
    btnSync->setObjectName(WidgetName::BtnSync);
    connect(btnSync, &QPushButton::clicked, [this]() { sync(); });

    auto btnAdd = new QPushButton();
    btnAdd->setIcon(App::getInstance()->fa().icon(fa::fa_solid, fa::fa_add));
    btnAdd->setStatusTip(tr("Create a new note"));
    connect(btnAdd, &QPushButton::clicked, &MainWindow::showAddNoteWindow);

    auto toolBar = new QToolBar();
    toolBar->addWidget(spacer);
    toolBar->addWidget(btnSync);
    toolBar->addWidget(btnAdd);

    return toolBar;
}

QStatusBar* MainWindow::mkStatusBar() {
    auto statusBar = new QStatusBar();
    statusBar->showMessage(tr("Welcome to SimpleNotes!"));
    return statusBar;
}

QWidget* MainWindow::mkContent() {
    auto layout = new QVBoxLayout();
    layout->setSpacing(16);
    layout->setContentsMargins(12, 12, 12, 12);
    layout->setAlignment(Qt::AlignTop);
    layout->setObjectName(WidgetName::NoteLayout);

    auto scrollWidget = new QWidget();
    scrollWidget->setLayout(layout);

    auto scrollArea = new QScrollArea();
    scrollArea->setWidget(scrollWidget);
    scrollArea->setWidgetResizable(true);

    return scrollArea;
}

QAction* MainWindow::getMenuBarAction(const QString &name) {
    std::function<QAction*(QMenu*)> enumerateMenu = [&](QMenu *menu) -> QAction* {
        for (auto action : menu->actions()) {
            if (action->menu()) {
                if (auto result = enumerateMenu(action->menu()); result) {
                    return result;
                }
            } else {
                if (action->objectName() == name) {
                    return action;
                }
            }
        }

        return nullptr;
    };

    for (auto action : menuBar()->actions()) {
        if (action->menu()) {
            if (auto result = enumerateMenu(action->menu()); result) {
                return result;
            }
        }
    }

    return nullptr;
}

void MainWindow::signedIn() {
    auto &auth = AuthProvider::getInstance();
    statusBar()->showMessage(QString(tr("Authorized: %1")).arg(auth.email()));

    findChild<QPushButton*>(WidgetName::BtnSync)->setEnabled(true);
    getMenuBarAction(ActionName::Sync)->setEnabled(true);

    auto actionSign = getMenuBarAction(ActionName::Sign);
    actionSign->setText(tr("Sign out"));
    actionSign->setStatusTip(tr("Sign out"));
}

void MainWindow::signedOut() {
    findChild<QPushButton*>(WidgetName::BtnSync)->setEnabled(false);
    getMenuBarAction(ActionName::Sync)->setEnabled(false);

    auto actionSign = getMenuBarAction(ActionName::Sign);
    actionSign->setText(tr("Sign in / up"));
    actionSign->setStatusTip(tr("Sign in or sign up if you haven't been registered yet"));
}

void MainWindow::showAuthWindow() {
    AuthDialog authDialog;
    authDialog.exec();
}

void MainWindow::checkAuthStatus() {
    auto &auth = AuthProvider::getInstance();

    if (!auth.isValid())
        return;

    auth.refresh(StatusHandler<>::error(ErrorBox::handler));
}

void MainWindow::sync() {
    NoteProvider::getInstance().sync(StatusHandler<>::both([this]() {
        App::getInstance()->invokeLater(QRunnable::create([this]() {
            statusBar()->showMessage(tr("Synchronization completed"));
        }));
    }, ErrorBox::handler));
}

void MainWindow::saveSettings() {
    QSettings settings(Details::Organization, Details::AppName);
    settings.setValue(SettingsKey::Geometry, saveGeometry());
}

void MainWindow::loadSettings() {
    QSettings settings(Details::Organization, Details::AppName);

    auto geometryByteArray = settings.value(SettingsKey::Geometry).toByteArray();

    if (!geometryByteArray.isEmpty()) {
        restoreGeometry(geometryByteArray);
    } else {
        resize(400, 500);
    }
}
