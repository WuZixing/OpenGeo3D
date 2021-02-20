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
	static QString nameOfDefaultG3DMap();
	///@}

	/// @name strings for menu.
	///@{
	static QString menuFile();
	static QString menuStructureModel();
	static QString menuOpenGeo3DML();
	static QString menuOpenDrillLog();
	static QString menuGridModel();
	static QString menuCloseStructureModel();
	static QString menuCloseAllModels();
	static QString menuHelp();
	static QString menuAbout();
	static QString menuQuit();
	///@}

	/// @name titles of dialogs/widgets.
	///@{
	static QString titleOfDlgAbout();
	static QString titleOfConfirmingQuit();
	static QString titleOfMetadataPage();
	static QString titleOfRenderOptionPage();
	static QString confirmToCloseStructureModel();
	static QString confirmToQuit();
	///@}

	/// @name file filters
	///@{
	static QString filterOfGeo3DMLFiles();
	///@}

	/// @name Labels.
	///@{
	static QString labelOfBasicMetaInfo();
	static QString labelOfDataset();
	static QString labelOfSubNodeNumber();
	static QString labelOfId();
	static QString labelOfAABB();
	static QString labelOfAABBMinPoint();
	static QString labelOfAABBMaxPoint();
	static QString labelOfStructureModel();
	static QString labelOf3DMap();
	static QString labelOfLayer();
	static QString labelOfActor();
	///@}

};
