#include "AppFrame.h"
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMessageBox>
#include "DlgAbout.h"
#include "Text.h"

AppFrame::AppFrame(QWidget* parent) : QMainWindow(parent) {
	QMenu* menu = menuBar()->addMenu(Text::menuHelp());
	menu->addAction(Text::menuAbout(), this, &AppFrame::about);
}

AppFrame::~AppFrame() {
}

void AppFrame::about() {
	// QMessageBox::about(this, tr("OpenGeo3D"), tr("about-open-geo3d"));
	DlgAbout about(this);
	about.exec();
}
