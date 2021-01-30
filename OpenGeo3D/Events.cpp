#include "Events.h"

void Events::Notify(ID id) {
	wxNotifyEvent evt(wxEVT_NULL, id);
	wxPostEvent(wxWindow::FindWindowById(ID::Window_Frame), evt);
}

void Events::QueueThreadEvent(wxEvent* evt) {
	wxQueueEvent(wxWindow::FindWindowById(ID::Window_Frame), evt);
}
