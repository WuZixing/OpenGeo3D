#pragma once

#include <QtCore/QEvent>

class Events {
public:
	enum Type {
		UpdateScene = QEvent::Type::User + 1,
		ResetAndUpdateScene
	};

	static void PostEvent(Events::Type evtType, QObject* receiver = nullptr);
};
