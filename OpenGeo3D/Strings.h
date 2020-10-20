#pragma once

#include <QString>

class Strings {
public:
	static QString appName();
	static QString confirmToQuitApp();

	static QString menuTitleOfFile();
	static QString menuTitleOfQuit();
	static QString menuTitleOfHelp();
	static QString menuTitleOfAbout();

	static QString aboutApp();

	static QString titleOfSceneTree();
	static QString titleOfSceneDataTab();
	static QString titleOfSceneVisualizationTab();
};
