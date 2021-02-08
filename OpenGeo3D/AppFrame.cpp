#include "AppFrame.h"
#include <QtGui/QCloseEvent>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMessageBox>
#include "icon.xpm"
#include "DlgAbout.h"
#include "RenderWidget.h"
#include "Text.h"

AppFrame::AppFrame(QWidget* parent) : QMainWindow(parent) {
	QPixmap pixmap(xpm_icon);
	setWindowIcon(QIcon(pixmap));
	setupMenu();
	setupWidgets();
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

void AppFrame::setupWidgets() {
	RenderWidget* renderWidget = new RenderWidget(this);
	setCentralWidget(renderWidget);
}

void AppFrame::closeEvent(QCloseEvent* event) {
	QMessageBox::StandardButton btn = QMessageBox::warning(this, Text::titleOfConfirmingQuit(), Text::confirmToQuit(), QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No, QMessageBox::StandardButton::No);
	if (btn != QMessageBox::StandardButton::Yes) {
		event->ignore();
	} else {
		QMainWindow::closeEvent(event);
	}
}
