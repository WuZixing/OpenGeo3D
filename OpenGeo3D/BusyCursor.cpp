#include "BusyCursor.h"

#include <QtWidgets/QApplication>

BusyCursor::BusyCursor() {
	beginWaiting();
}

BusyCursor::~BusyCursor() {
	endWaiting();
}

void BusyCursor::beginWaiting() {
	QApplication::setOverrideCursor(Qt::CursorShape::WaitCursor);
}

void BusyCursor::endWaiting() {
	QApplication::restoreOverrideCursor();
}
