#pragma once

#include <QtCore/QString>

class Text {
public:
	static QString appName();
	static QString appVersion();
	static QString appDescription();
	static QString appWebSite();

	/// @name strings for data.
	///@{
	static QString nameOfStructureModel();
	static QString nameOfVoxelGridModel();
	///@}

	/// @name strings for menu.
	///@{
	static QString menuFile();
	static QString menuStructureModel();
	static QString menuOpenGeo3DML();
	static QString menuOpenDrillLog();
	static QString menuGridModel();
	static QString menuHelp();
	static QString menuAbout();
	static QString menuQuit();
	///@}

	/// @name titles of dialogs.
	///@{
	static QString titleOfDlgAbout();
	static QString titleOfConfirmingQuit();
	static QString confirmToQuit();
	///@}

	/// @name file filters
	///@{
	static QString filterOfGeo3DMLFiles();
	///@}

};
