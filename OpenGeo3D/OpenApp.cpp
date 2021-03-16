#include "OpenApp.h"
#include "Text.h"

OpenApp::OpenApp(int& argc, char** argv) : QApplication(argc, argv) {
	QApplication::setApplicationName(Text::appName());
	QApplication::setApplicationVersion(Text::appVersion());
	QApplication::setApplicationDisplayName(Text::appName());
}

OpenApp::~OpenApp() {

}

void OpenApp::showWindow() {
	appFrame_.showMaximized();
}

bool OpenApp::event(QEvent* event) {
	if (QCoreApplication::sendEvent(&appFrame_, event)) {
		return true;
	}

	return QApplication::event(event);
}
