#pragma once

#include <QtCore/QString>

class Text {
public:
	static QString appName();
	static QString appVersion();
	static QString appDescription();
	static QString appWebSite();

	/// @name strings for menu.
	///@{
	static QString menuHelp();
	static QString menuAbout();
	static QString menuQuit();
	///@}

	static QString titleOfDlgAbout();
	static QString titleOfConfirmingQuit();
	static QString confirmToQuit();

};
