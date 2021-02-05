#pragma once

#include <QtCore/QString>

class Text {
public:
	static QString appName();
	static QString appVersion();
	static QString appDescription();
	static QString appWebSite();

	static QString menuHelp();
	static QString menuAbout();

	static QString titleOfDlgAbout();

};
