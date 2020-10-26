#include "Events.h"

void Events::Notify(ID id) {
	wxNotifyEvent evt(wxEVT_NULL, id);
	wxPostEvent(wxWindow::FindWindowById(ID::Window_Frame), evt);
}
