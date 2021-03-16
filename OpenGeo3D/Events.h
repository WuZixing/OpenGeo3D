#pragma once

#include <QtCore/QEvent>

class Events {
public:
	enum Type {
		UpdateScene = QEvent::Type::User + 1,
		ResetAndUpdateScene,
		Menu_OpenGeo3DML,
		Menu_OpenDrillLog,
		Menu_SaveToGeo3DML,
		Menu_CloseStructureModel
	};

	static void PostEvent(Events::Type evtType, QObject* receiver = nullptr);
};
