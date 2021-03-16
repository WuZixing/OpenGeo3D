#include "Events.h"
#include <QtCore/QCoreApplication>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

void Events::PostEvent(Events::Type evtType, QObject* receiver) {
	QEvent* evt = new QEvent((QEvent::Type)(int(evtType)));
	if (receiver == nullptr) {
		QCoreApplication::postEvent(QCoreApplication::instance(), evt);
	} else {
		QCoreApplication::postEvent(receiver, evt);
	}
}
