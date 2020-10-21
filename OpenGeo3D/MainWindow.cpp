#include "MainWindow.h"
#include <QPointer>
#include <QTreeView>
#include <QtTreePropertyBrowser>
#include <QMessageBox>
#include <QCloseEvent>
#include <QMenuBar>
#include <QApplication>
#include <QDesktopWidget>
#include <vtkGenericOpenGLRenderWindow.h>
#include <QVTKOpenGLNativeWidget.h>
#include <Strings.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {

    vtkNew<vtkGenericOpenGLRenderWindow> window;
    QPointer<QVTKOpenGLNativeWidget> widget = new QVTKOpenGLNativeWidget(window.GetPointer(), this);
    setCentralWidget(widget);

    createDockWindows();
    createActions();
}

MainWindow::~MainWindow() {

}

void MainWindow::createActions() {
    QMenu* menuFile = menuBar()->addMenu(Strings::menuTitleOfFile());
    QAction* actQuit = menuFile->addAction(Strings::menuTitleOfQuit());
    connect(actQuit, &QAction::triggered, this, &MainWindow::quit);

    QMenu* menuWnd = menuBar()->addMenu(Strings::menuTitleOfWindows());
    QAction* actWndScene = dockSceneTree_->toggleViewAction();
    menuWnd->addAction(actWndScene);
    connect(actWndScene, &QAction::toggled, this, &MainWindow::toggleWndScene);
    QAction* actWndData = dockDataPropBrowser_->toggleViewAction();
    menuWnd->addAction(actWndData);
    connect(actWndData, &QAction::toggled, this, &MainWindow::toggleWndData);
    QAction* actWndVis = dockVisPropBrowser_->toggleViewAction();
    menuWnd->addAction(actWndVis);
    connect(actWndVis, &QAction::toggled, this, &MainWindow::toggleWndVis);
    /*
    QAction* actWndScene = menuWnd->addAction(Strings::titleOfSceneTree());
    actWndScene->setCheckable(true);
    actWndScene->setChecked(true);
    connect(actWndScene, &QAction::toggled, this, &MainWindow::toggleWndScene);
    QAction* actWndData = menuWnd->addAction(Strings::titleOfSceneDataTab());
    actWndData->setCheckable(true);
    actWndData->setChecked(true);
    QAction* actWndVis = menuWnd->addAction(Strings::titleOfSceneVisualizationTab());
    actWndVis->setCheckable(true);
    actWndVis->setChecked(true);
    */

    QMenu* menuHelp = menuBar()->addMenu(Strings::menuTitleOfHelp());
    QAction* actAbout = menuHelp->addAction(Strings::menuTitleOfAbout());
    connect(actAbout, &QAction::triggered, this, &MainWindow::about);
}

void MainWindow::createDockWindows() {
    dockSceneTree_ = new QDockWidget(Strings::titleOfSceneTree(), this);
    dockSceneTree_->setAllowedAreas(Qt::LeftDockWidgetArea);
    QTreeView* sceneTree = new QTreeView(dockSceneTree_);
    int wndWidth = QApplication::desktop()->width();
    sceneTree->setMinimumWidth(wndWidth / 8);
    dockSceneTree_->setWidget(sceneTree);
    addDockWidget(Qt::LeftDockWidgetArea, dockSceneTree_);

    dockDataPropBrowser_ = new QDockWidget(Strings::titleOfSceneDataTab(), this);
    dockDataPropBrowser_->setAllowedAreas(Qt::LeftDockWidgetArea);
    QtTreePropertyBrowser* dataPropBrowser = new QtTreePropertyBrowser(dockDataPropBrowser_);
    dockDataPropBrowser_->setWidget(dataPropBrowser);
    addDockWidget(Qt::LeftDockWidgetArea, dockDataPropBrowser_);

    dockVisPropBrowser_ = new QDockWidget(Strings::titleOfSceneVisualizationTab(), this);
    dockVisPropBrowser_->setAllowedAreas(Qt::LeftDockWidgetArea);
    QtTreePropertyBrowser* visPropBrowser = new QtTreePropertyBrowser(dockVisPropBrowser_);
    dockVisPropBrowser_->setWidget(visPropBrowser);
    addDockWidget(Qt::LeftDockWidgetArea, dockVisPropBrowser_);

    tabifyDockWidget(dockDataPropBrowser_, dockVisPropBrowser_);
    dockDataPropBrowser_->raise();
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

void MainWindow::toggleWndScene(bool toggle) {
    dockSceneTree_->setVisible(toggle);
}

void MainWindow::toggleWndData(bool toggle) {
    dockDataPropBrowser_->setVisible(toggle);
}

void MainWindow::toggleWndVis(bool toggle) {
    dockVisPropBrowser_->setVisible(toggle);
}