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
	
	/// @name utilities
	static wxString ConfirmToQuit();
	static wxString TipOfOpenGeo3DML();
	static wxString WildcardOfOpenGeo3DML();

	static wxString MetadataCategoryBasicInfo();
	static wxString MetadataCategoryMBR();
	static wxString MetadataCategoryFeatureClass();
	static wxString MetadataCategorySchema();
	static wxString MetadataCategoryFeature();
	static wxString MetadataCategoryGeometry();
	static wxString MetadataCategoryShapeProperty();
	static wxString MetadataEntryId();
	static wxString MetadataEntryName();
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
};
