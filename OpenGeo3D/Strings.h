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
	static wxString TitleOfMenuItemOpenGeo3DML();
	static wxString TitleOfMenuItemOpenSGeMSGrid();
	static wxString TitleOfMenuItemQuit();
	static wxString TitleOfMenuItemAbout();
	///@}

	static wxString ConfirmToQuit();

	/// @name title of windows
	///@{
	static wxString TitleOfProjectPanel();
	static wxString TitleOfDataInfo();
	static wxString TitleOfVisInfo();
	///@}

	static wxString NameOfDefaultG3DProject();
	static wxString NameOfDefaultG3DMap();
	static wxString NameOfStructureModel();
	static wxString NameOfGridModel();

	static wxString TipOfOpenGeo3DML();
	static wxString WildcardOfOpenGeo3DML();
};
