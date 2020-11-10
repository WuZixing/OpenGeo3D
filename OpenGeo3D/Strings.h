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
	static wxString TitleOfMenuItemOpenSimpleDrillLog();
	static wxString TitleOfMenuItemOpenSGeMSGrid();
	static wxString TitleOfMenuItemQuit();
	static wxString TitleOfMenuItemAbout();
	static wxString TitleOfMenuItemFullView();
	static wxString TitleOfMenuItemBackgroundColor();
	static wxString TitleOfMenuItemScaleUpZ();
	static wxString TitleOfMenuItemScaleDownZ();
	static wxString TitleOfMenuItemCustomizedZScale();
	static wxString TitleOfMenuItemResetZScale();
	static wxString TitleOfMenuItemProjectPanel();
	static wxString TitleOfMenuItemEditVoxelGrid();
	static wxString TitleOfMenuItemDeleteGridLOD();
	static wxString TitleOfMenuItemAppendGridLOD();
	static wxString TitleOfMenuItemCloseAllModels();
	static wxString TitleOfMenuItemCloseStructureModel();
	static wxString TitleOfMenuItemCloseVoxelGridModel();
	static wxString TitleOfMenuItemSaveToGeo3DML();
	static wxString TitleOfMenuItemSaveToVoxelGrid();
	///@}

	/// @name title of windows/dialogs
	///@{
	static wxString TitleOfProjectPanel();
	static wxString TitleOfMetadata();
	static wxString TitleOfRenderOption();
	static wxString TitleOfSizerOfSimpleDrillLog();
	///@}

	static wxString NameOfDefaultG3DMap();
	static wxString NameOfStructureModel();
	static wxString NameOfGridModel();
	static wxString NameOfClassG3DMap();
	static wxString NameOfClassG3DLayer();
	static wxString NameOfClassG3DActor();
	static wxString NameOfClassG3DPoint();
	static wxString NameOfClassG3DMPoint();
	static wxString NameOfClassG3DAnnotation();
	static wxString NameOfClassG3DLineString();
	static wxString NameOfClassG3DTIN();
	static wxString NameOfClassG3DUniformGrid();
	static wxString NameOfClassG3DCornerGrid();
	static wxString NameOfClassUnknown();
	static wxString NameOfRenderRepresentationModePoint();
	static wxString NameOfRenderRepresentationModeWireframe();
	static wxString NameOfRenderRepresentationModeSurface();
	static wxString NameOfVoxelGrid();

	static wxString NameOfDrillModel();
	static wxString NameOfDrillFeatureClass();
	static wxString NameOfDrillPositionFeature();
	
	static wxString ConfirmToQuit();
	static wxString TipOfOpenGeo3DML();
	static wxString WildcardOfGeo3DMLFile();
	static wxString WildcardOfGeo3DMLFileWithVersion();
	static wxString TipOfInputCusotimizedZScale();
	static wxString TipOfInvalidZScale();
	static wxString TipOfOpenDrillPositionFile();
	static wxString WildcardOfDrillPositionFile();
	static wxString TipOfOpenDrillLogFile();
	static wxString WildcardOfDrillLogFile();
	static wxString ConfirmToEmptyFileList();
	static wxString TipOfOpenFileFailed(const wxString& filePath);
	static wxString TipOfInvalidDrillPositionData();
	static wxString TipOfInvalidDrillLogData();
	static wxString TipOfInvalidName();
	static wxString TipOfInvalidSRS();
	static wxString TipOfInvalidGridOrigin();
	static wxString ConfirmToDeleteGridLOD(int level);
	static wxString TipOfGridLODAndCellScale();
	static wxString TipOfInvalidGridCellSize();
	static wxString ConfirmToCloseAllModels();
	static wxString ConfirmToCloseStructureModel();
	static wxString ConfirmToCloseVoxelGridModel();
	static wxString TipOfSucceedInSavingToGeo3DMLFile(const wxString& modelName);
	static wxString TipOfErrorInSavingToGeo3DMLFile(const wxString& modelName, const wxString& error);
	static wxString TipOfSucceedInSavingToVoxelGridFile(const wxString& modelName);
	static wxString TipOfErrorInSavingToVoxelGridFile(const wxString& modelName, const wxString& error);

	static wxString LabelOfOpen();
	static wxString LabelOfAppend();
	static wxString LabelOfClear();
	static wxString LabelOfID();
	static wxString LabelOfName();
	static wxString LabelOfDescription();
	static wxString LabelOfSRS();
	static wxString LabelOfX();
	static wxString LabelOfY();
	static wxString LabelOfZ();
	static wxString LabelOfNo();
	static wxString LabelOfFilePath();
	static wxString LabelOfField();
	static wxString LabelOfFieldValueType();
	static wxString LabelOfDrillPositionFilePath();
	static wxString LabelOfDrillNo();
	static wxString LabelOfDrillDepth();
	static wxString LabelOfDrillLog();
	static wxString LabelOfGridOrigin();
	static wxString LabelOfGridCellSize();
	static wxString LabelOfGridCellScale();
	static wxString LabelOfGridLOD();
	static wxString LabelOfCellSizeX();
	static wxString LabelOfCellSizeY();
	static wxString LabelOfCellSizeZ();
	static wxString LabelOfCellScaleX();
	static wxString LabelOfCellScaleY();
	static wxString LabelOfCellScaleZ();
	static wxString LabelOfCellCount();
	static wxString LabelOfSchema();

	/// @name metadata
	///@{
	static wxString MetadataCategoryBasicInfo();
	static wxString MetadataCategoryMBR();
	static wxString MetadataCategoryFeatureClass();
	static wxString MetadataCategorySchema();
	static wxString MetadataCategoryFeature();
	static wxString MetadataCategoryGeometry();
	static wxString MetadataCategoryShapeProperty();
	static wxString MetadataCategoryVoxelGrid();
	static wxString MetadataEntryId();
	static wxString MetadataEntryName();
	static wxString MetadataEntryDescription();
	static wxString MetadataEntrySRS();
	static wxString MetadataEntryDatasetClassName();
	static wxString MetadataEntryChildrenNumber();
	static wxString MetadataEntryMBRRangeX();
	static wxString MetadataEntryMBRRangeY();
	static wxString MetadataEntryMBRRangeZ();
	static wxString MetadataEntryClassName();
	static wxString MetadataEntryGeometryLODLevel();
	static wxString MetadataEntryNumberOfVertices();
	static wxString MetadataEntryNumberOfTriangles();
	static wxString MetadataEntryGridOrigin();
	static wxString MetadataEntryGridCellSize();
	static wxString MetadataEntryGridCellDimension();
	///@}

	/// @name render options
	///@{
	static wxString RenderCategoryObjectOption();
	static wxString RenderCategoryShapePropertyOption();
	static wxString RenderEntryColor();
	static wxString RenderEntryTransparency();
	static wxString RenderEntryRepresentationMode();
	static wxString RenderEntryDrawWireframe();
	///@}
};
