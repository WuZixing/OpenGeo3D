#pragma once

#include "wxWidgets.h"
#include <wx/propgrid/propgrid.h>
#include <geo3dml/Project.h>
#include "G3DTreeItemData.h"

class MetadataPage : public wxPropertyGrid {
public:
	MetadataPage(wxWindow* parent);
	virtual ~MetadataPage();

	void BindToItem(geo3dml::Object* g3dObject, G3DTreeItemData::ItemType itemType);

private:
	void BindToStructureModels(geo3dml::Project* g3dProject);
	void BindToG3DMap(geo3dml::Map* g3dMap);
	void BindToG3DLayer(geo3dml::Layer* g3dLayer);
	void BindToG3DActor(geo3dml::Actor* g3dActor);

	void SetBasicInfo(const wxString& datasetCategory, const wxString& datasetId, unsigned int numberOfChildren);
	void SetMBRProperty(double minX, double minY, double minZ, double maxX, double maxY, double maxZ);
	void SetFeatureProperty(geo3dml::Feature* g3dFeature);
	void SetGeometryProperty(geo3dml::Geometry* g3dGeometry);
	void SetShapePropertyOfGeometry(wxPGProperty* parentCategory, geo3dml::ShapeProperty* shapeProperty);
	void SetFieldProperty(wxPGProperty* parentCategory, const geo3dml::Field& field, int fieldIndex, const wxString& namePrefix = wxEmptyString);
};
