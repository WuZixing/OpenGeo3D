#include "AppFrame.h"

#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    AppFrame frame;
    frame.showMaximized();
    return a.exec();
}