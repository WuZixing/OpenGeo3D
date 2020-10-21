#pragma once

#include <QMainWindow>
#include <QDockWidget>


class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent* event) override;

private Q_SLOTS:
    void quit();
    void about();
    void toggleWndScene(bool toggle);
    void toggleWndData(bool toggle);
    void toggleWndVis(bool toggle);

private:
    void createActions();
    void createDockWindows();

private:
    QDockWidget* dockSceneTree_;
    QDockWidget* dockDataPropBrowser_;
    QDockWidget* dockVisPropBrowser_;
};
