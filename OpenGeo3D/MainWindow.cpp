#include "MainWindow.h"
#include <QPointer>
#include <QDockWidget>
#include <QMessageBox>
#include <QCloseEvent>
#include <QMenuBar>
#include <vtkGenericOpenGLRenderWindow.h>
#include <QVTKOpenGLNativeWidget.h>
#include <Strings.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {

    vtkNew<vtkGenericOpenGLRenderWindow> window;
    QPointer<QVTKOpenGLNativeWidget> widget = new QVTKOpenGLNativeWidget(window.GetPointer(), this);
    setCentralWidget(widget);

    createActions();
    createDockWindows();
}

MainWindow::~MainWindow() {

}

void MainWindow::createActions() {
    QMenu* menuFile = menuBar()->addMenu(Strings::menuTitleOfFile());
    QAction* actQuit = menuFile->addAction(Strings::menuTitleOfQuit());
    connect(actQuit, &QAction::triggered, this, &MainWindow::quit);

    QMenu* menuHelp = menuBar()->addMenu(Strings::menuTitleOfHelp());
    QAction* actAbout = menuHelp->addAction(Strings::menuTitleOfAbout());
    connect(actAbout, &QAction::triggered, this, &MainWindow::about);
}

void MainWindow::createDockWindows() {
    QDockWidget* dock = new QDockWidget(Strings::titleOfSceneTree(), this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea);
    sceneTree_ = new QTreeView(dock);
    dock->setWidget(sceneTree_);
    addDockWidget(Qt::LeftDockWidgetArea, dock);

    dock = new QDockWidget(Strings::titleOfSceneDataTab(), this);
    propertyTab_ = new QTabWidget(dock);
    dock->setWidget(propertyTab_);
    addDockWidget(Qt::LeftDockWidgetArea, dock);
}

void MainWindow::closeEvent(QCloseEvent* event) {
    auto result = QMessageBox::warning(nullptr, Strings::appName(), Strings::confirmToQuitApp(), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (result == QMessageBox::Yes) {
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::quit() {
    close();
}

void MainWindow::about() {
    QMessageBox::about(this, Strings::appName(), Strings::aboutApp());
}