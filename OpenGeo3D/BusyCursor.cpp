#include "BusyCursor.h"

#include <QtWidgets/QApplication>

BusyCursor::BusyCursor() {
	BeginWaiting();
}

BusyCursor::~BusyCursor() {
	EndWaiting();
}

void BusyCursor::BeginWaiting() {
	QApplication::setOverrideCursor(Qt::CursorShape::WaitCursor);
}

void BusyCursor::EndWaiting() {
	QApplication::restoreOverrideCursor();
}
