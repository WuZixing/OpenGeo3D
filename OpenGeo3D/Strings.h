#pragma once

#include "wxWidgets.h"

class Strings {
public:
	static wxString AppName();
	static wxString AppVersion();
	static wxString AppDescription();

	/// @name menu
	///@{
	static wxString TitleOfMenuFile();
	static wxString TitleOfMenuWindow();
	static wxString TitleOfMenuHelp();
	static wxString TitleOfMenuItemOpenFile();
	static wxString TitleOfMenuItemQuit();
	static wxString TitleOfMenuItemAbout();
	///@}

	static wxString ConfirmToQuit();

	/// @name title of windows
	///@{
	static wxString TitleOfSceneTree();
	static wxString TitleOfDataInfo();
	static wxString TitleOfVisInfo();
	///@}

	static wxString NameOfStructureModel();
	static wxString NameOfGridModel();
};
