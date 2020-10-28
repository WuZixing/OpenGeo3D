#pragma once

#include "wxWidgets.h"

class Events {
public:
	enum ID {
		Window_Frame = wxID_HIGHEST + 1,

		MENU_OpenGeo3DML,
		MENU_OpenSGeMSGrid,
		Menu_FullView,

		// general notifications
		Notify_ResetAndRefreshRenderWindow,
		Notify_RefreshRenderWindow,
		// project panel notifications
		Notify_ProjectTreeItemSelected
	};

public:
	static void Notify(ID id);
};
