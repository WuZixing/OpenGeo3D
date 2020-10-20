#pragma once

#include <QMainWindow>
#include <QTreeView>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    void createActions();
    void createDockWindows();

private:
    QTreeView* sceneTree_;
    QTabWidget* propertyTab_;
};
