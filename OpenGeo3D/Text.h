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
	static QString nameOfClassUnknown();
	static QString nameOfClassG3DPoint();
	static QString nameOfClassG3DMPoint();
	static QString nameOfClassG3DAnnotation();
	static QString nameOfClassG3DLineString();
	static QString nameOfClassG3DTIN();
	static QString nameOfClassG3DUniformGrid();
	static QString nameOfClassG3DCornerGrid();
	static QString nameOfRepresentationModePoints();
	static QString nameOfRepresentationModeWireframe();
	static QString nameOfRepresentationModeSurface();
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
	static QString menuWindow();
	static QString menuFullView();
	static QString menuBackgroundColor();
	static QString menuScaleZUp();
	static QString menuScaleZDown();
	static QString menuCustomizedZScale();
	static QString menuResetZScale();
	static QString menuProjectPanel();
	///@}

	/// @name titles of dialogs/widgets.
	///@{
	static QString titleOfDlgAbout();
	static QString titleOfConfirmingQuit();
	static QString titleOfProjectPanel();
	static QString titleOfMetadataPage();
	static QString titleOfRenderOptionPage();
	///@}

	/// @name file filters
	///@{
	static QString filterOfGeo3DMLFile();
	static QString filterOfDrillPositionFile();
	static QString filterOfDrillLogFile();
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
	static QString labelOfFeatureClass();
	static QString labelOfFeature();
	static QString labelOfName();
	static QString labelOfSchema();
	static QString labelOfGeometry();
	static QString labelOfLOD();
	static QString labelOfClassName();
	static QString labelOfNumberOfVertices();
	static QString labelOfNumberOfTriangles();
	static QString labelOfGridOrigin();
	static QString labelOfGridCellSize();
	static QString labelOfGridCellDimension();
	static QString labelOfRenderOption();
	static QString labelOfColor();
	static QString labelOfRepresentationMode();
	static QString labelOfDrawEdge();
	static QString labelOfSimpleDrillLogGroup();
	static QString labelOfDrillPositionFilePath();
	static QString labelOfOpen();
	static QString labelOfNo();
	static QString labelOfDrillNo();
	static QString labelOfDrillDepth();
	static QString labelOfX();
	static QString labelOfY();
	static QString labelOfZ();
	static QString labelOfDrillLog();
	static QString labelOfAppend();
	static QString labelOfClear();
	static QString labelOfFilePath();
	static QString labelOfField();
	static QString labelOfFieldValueType();
	///@}

	static QString tipOfCustomizedZScale();
	static QString tipOfSpecifyFieldValueType();
	static QString errorOfOpenFile(const QString& filePath, const QString& errorMessage);
	static QString tipOfEmptyDrillPosition();
	static QString tipOfEmptyDrillLogFiles();
	static QString tipOfUnknownFieldValueType();

	static QString confirmToCloseStructureModel();
	static QString confirmToQuit();
	static QString confirmToClearFileList();
};
