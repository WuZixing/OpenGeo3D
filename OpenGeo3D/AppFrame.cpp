#include "AppFrame.h"
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMessageBox>
#include "icon.xpm"
#include "DlgAbout.h"
#include "Text.h"

AppFrame::AppFrame(QWidget* parent) : QMainWindow(parent) {
	QPixmap pixmap(xpm_icon);
	setWindowIcon(QIcon(pixmap));
	setupMenu();
}

AppFrame::~AppFrame() {
}

void AppFrame::about() {
	DlgAbout about(this);
	about.exec();
}

void AppFrame::setupMenu() {
	QMenu* menu = menuBar()->addMenu(Text::menuHelp());
	menu->addAction(Text::menuAbout(), this, &AppFrame::about);
}
