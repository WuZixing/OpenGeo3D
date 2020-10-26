#pragma once

#include "wxWidgets.h"

class Events {
public:
	enum ID {
		Window_Frame = wxID_HIGHEST + 1,

		MENU_OpenGeo3DML,
		MENU_OpenSGeMSGrid,

		Notify_ResetAndRefreshRenderWindow,
		Notify_RefreshRenderWindow
	};

public:
	static void Notify(ID id);
};
