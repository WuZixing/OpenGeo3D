#include "MainWindow.h"
#include <QPointer>
#include <QDockWidget>
#include <QMessageBox>
#include <QCloseEvent>
#include <vtkGenericOpenGLRenderWIndow.h>
#include <QVTKOpenGLNativeWidget.h>
#include <Strings.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {

    vtkNew<vtkGenericOpenGLRenderWindow> window;
    QPointer<QVTKOpenGLNativeWidget> widget = new QVTKOpenGLNativeWidget(window.GetPointer(), this);
    setCentralWidget(widget);

    createDockWindows();
}

MainWindow::~MainWindow() {

}

void MainWindow::closeEvent(QCloseEvent* event) {
    auto result = QMessageBox::warning(nullptr, Strings::appName(), Strings::confirmToQuitApp() , QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (result == QMessageBox::Yes) {
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::createActions() {

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