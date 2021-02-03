#include "AppFrame.h"
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMessageBox>

AppFrame::AppFrame(QWidget* parent) : QMainWindow(parent) {
	QMenu* menu = menuBar()->addMenu(tr("&Help"));
	menu->addAction(tr("About"), this, &AppFrame::about);
}

AppFrame::~AppFrame() {
}

void AppFrame::about() {
	QMessageBox::about(this, tr("OpenGeo3D"), tr("about-open-geo3d"));
}
