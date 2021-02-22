#include "MetadataPage.h"
#include "Strings.h"
#include <geo3dml/Annotation.h>
#include <geo3dml/CornerPointGrid.h>
#include <geo3dml/LineString.h>
#include <geo3dml/MultiPoint.h>
#include <geo3dml/Point.h>
#include <geo3dml/TIN.h>
#include <geo3dml/UniformGrid.h>


MetadataPage::MetadataPage(wxWindow* parent) : wxPropertyGrid(parent) {

}

MetadataPage::~MetadataPage() {

}

void MetadataPage::BindToItem(geo3dml::Object* g3dObject, G3DTreeItemData::ItemType itemType) {
	Clear();
	switch (itemType) {
	case G3DTreeItemData::ItemType::G3D_StructureModel: {
		geo3dml::Project* g3dProject = static_cast<geo3dml::Project*>(g3dObject);
		BindToStructureModels(g3dProject);
		break;
	}
	case G3DTreeItemData::ItemType::G3D_Map: {
		geo3dml::Map* g3dMap = static_cast<geo3dml::Map*>(g3dObject);
		BindToG3DMap(g3dMap);
		break;
	}
	case G3DTreeItemData::ItemType::G3D_Layer: {
		geo3dml::Layer* g3dLayer = static_cast<geo3dml::Layer*>(g3dObject);
		BindToG3DLayer(g3dLayer);
		break;
	}
	case G3DTreeItemData::ItemType::G3D_Actor: {
		geo3dml::Actor* g3dActor = static_cast<geo3dml::Actor*>(g3dObject);
		BindToG3DActor(g3dActor);
		break;
	}
	case G3DTreeItemData::ItemType::G3D_VoxelGrid: {
		g3dgrid::VoxelGrid* g3dGrid = static_cast<g3dgrid::VoxelGrid*>(g3dObject);
		BindToG3DVoxelGrid(g3dGrid);
		break;
	}
	default:
		break;
	}
	SetSplitterLeft();
}

void MetadataPage::BindToStructureModels(geo3dml::Project* g3dProject) {
	SetBasicInfo(Strings::NameOfStructureModel(), wxString::FromUTF8(g3dProject->GetID()), g3dProject->GetMapCount());
	double minX = 0, maxX = -1, minY = 0, maxY = -1, minZ = 0, maxZ = -1;
	g3dProject->GetMinimumBoundingRectangle(minX, minY, minZ, maxX, maxY, maxZ);
	SetMBRProperty(minX, minY, minZ, maxX, maxY, maxZ);
}

void MetadataPage::BindToG3DMap(geo3dml::Map* g3dMap) {
	SetBasicInfo(Strings::NameOfClassG3DMap(), wxString::FromUTF8(g3dMap->GetID()), g3dMap->GetLayerCount());
	double minX = 0, maxX = -1, minY = 0, maxY = -1, minZ = 0, maxZ = -1;
	g3dMap->GetMinimumBoundingRectangle(minX, minY, minZ, maxX, maxY, maxZ);
	SetMBRProperty(minX, minY, minZ, maxX, maxY, maxZ);
}

void MetadataPage::BindToG3DLayer(geo3dml::Layer* g3dLayer) {
	SetBasicInfo(Strings::NameOfClassG3DLayer(), wxString::FromUTF8(g3dLayer->GetID()), g3dLayer->GetActorCount());
	double minX = 0, maxX = -1, minY = 0, maxY = -1, minZ = 0, maxZ = -1;
	g3dLayer->GetMinimumBoundingRectangle(minX, minY, minZ, maxX, maxY, maxZ);
	SetMBRProperty(minX, minY, minZ, maxX, maxY, maxZ);
	// feature class
	geo3dml::FeatureClass* g3dFeatureClass = g3dLayer->GetBindingFeatureClass();
	wxPGProperty* categoryFC = Append(new wxPropertyCategory(Strings::MetadataCategoryFeatureClass()));
	wxString strId, strName;
	if (g3dFeatureClass != nullptr) {
		strId = wxString::FromUTF8(g3dFeatureClass->GetID());
		strName = wxString::FromUTF8(g3dFeatureClass->GetName());
	} else {
		strId = wxString::FromUTF8(g3dLayer->GetBindingFeatureClassID());
	}
	AppendIn(categoryFC, new wxStringProperty(Strings::MetadataEntryId(), wxS("feature_class_id"), strId))->ChangeFlag(wxPG_PROP_READONLY, true);
	AppendIn(categoryFC, new wxStringProperty(Strings::MetadataEntryName(), wxS("feature_class_name"), strName))->Enable(false);
	wxPGProperty* categorySchema = AppendIn(categoryFC, new wxPropertyCategory(Strings::MetadataCategorySchema()));
	for (int i = 0; i < g3dFeatureClass->GetFieldCount(); ++i) {
		const geo3dml::Field& field = g3dFeatureClass->GetFieldAt(i);
		SetFieldProperty(categorySchema, field, i);
	}
}

void MetadataPage::BindToG3DActor(geo3dml::Actor* g3dActor) {
	SetBasicInfo(Strings::NameOfClassG3DActor(), wxString::FromUTF8(g3dActor->GetID()), 0);
	double minX = 0, maxX = -1, minY = 0, maxY = -1, minZ = 0, maxZ = -1;
	g3dActor->GetMinimumBoundingRectangle(minX, minY, minZ, maxX, maxY, maxZ);
	SetMBRProperty(minX, minY, minZ, maxX, maxY, maxZ);
	// feature
	SetFeatureProperty(g3dActor->GetBindingFeature());
	// geometry
	SetGeometryProperty(g3dActor->GetBindingGeometry());
}

void MetadataPage::BindToG3DVoxelGrid(g3dgrid::VoxelGrid* g3dVoxelGrid) {
	SetBasicInfo(Strings::NameOfVoxelGrid(), wxString::FromUTF8(g3dVoxelGrid->GetID()), g3dVoxelGrid->GetLODCount());
	double minX = 0, maxX = -1, minY = 0, maxY = -1, minZ = 0, maxZ = -1;
	g3dVoxelGrid->GetMinimumBoundingRectangle(minX, minY, minZ, maxX, maxY, maxZ);
	SetMBRProperty(minX, minY, minZ, maxX, maxY, maxZ);
	// voxel grid
	wxPGProperty* categoryVG = Append(new wxPropertyCategory(Strings::MetadataCategoryVoxelGrid()));
	AppendIn(categoryVG, new wxStringProperty(Strings::MetadataEntryDescription(), wxS("voxel_grid_description"), wxString::FromUTF8(g3dVoxelGrid->GetDescription())))->Enable(false);
	AppendIn(categoryVG, new wxStringProperty(Strings::MetadataEntrySRS(), wxS("voxel_grid_srs"), wxString::FromUTF8(g3dVoxelGrid->GetSRS())))->Enable(false);
	geo3dml::Point3D pt = g3dVoxelGrid->GetOrigin();
	wxString str = wxString::Format("(%.6f, %.6f, %.6f)", pt.x, pt.y, pt.z);
	AppendIn(categoryVG, new wxStringProperty(Strings::MetadataEntryGridOrigin(), wxS("voxel_grid_origin"), str))->ChangeFlag(wxPG_PROP_READONLY, true);
	// LOD
}

void MetadataPage::SetBasicInfo(const wxString& datasetCategory, const wxString& datasetId, unsigned int numberOfChildren) {
	wxPGProperty* category = Append(new wxPropertyCategory(Strings::MetadataCategoryBasicInfo()));
	AppendIn(category, new wxStringProperty(Strings::MetadataEntryDatasetClassName(), wxS("dataset_class"), datasetCategory))->Enable(false);
	AppendIn(category, new wxUIntProperty(Strings::MetadataEntryChildrenNumber(), wxS("children_number"), numberOfChildren))->Enable(false);
	AppendIn(category, new wxStringProperty(Strings::MetadataEntryId(), wxS("dataset_id"), datasetId))->ChangeFlag(wxPG_PROP_READONLY, true);
}

void MetadataPage::SetMBRProperty(double minX, double minY, double minZ, double maxX, double maxY, double maxZ) {
	wxPGProperty* category = Append(new wxPropertyCategory(Strings::MetadataCategoryMBR()));
	wxString xRange, yRange, zRange;
	if (minX <= maxX) {
		xRange = wxString::Format("[%.6f, %.6f]", minX, maxX);
	}
	if (minY <= maxY) {
		yRange = wxString::Format("[%.6f, %.6f]", minY, maxY);
	}
	if (minZ <= maxZ) {
		zRange = wxString::Format("[%.6f, %.6f]", minZ, maxZ);
	}
	AppendIn(category, new wxStringProperty(Strings::MetadataEntryMBRRangeX(), wxS("mbr_x_range"), xRange))->ChangeFlag(wxPG_PROP_READONLY, true);
	AppendIn(category, new wxStringProperty(Strings::MetadataEntryMBRRangeY(), wxS("mbr_y_range"), yRange))->ChangeFlag(wxPG_PROP_READONLY, true);
	AppendIn(category, new wxStringProperty(Strings::MetadataEntryMBRRangeZ(), wxS("mbr_z_range"), zRange))->ChangeFlag(wxPG_PROP_READONLY, true);
}

void MetadataPage::SetFeatureProperty(geo3dml::Feature* g3dFeature) {
	wxPGProperty* categoryFeature = Append(new wxPropertyCategory(Strings::MetadataCategoryFeature()));
	if (g3dFeature == nullptr) {
		return;
	}
	AppendIn(categoryFeature, new wxStringProperty(Strings::MetadataEntryId(), wxS("feature_id"), wxString::FromUTF8(g3dFeature->GetID())))->ChangeFlag(wxPG_PROP_READONLY, true);
	AppendIn(categoryFeature, new wxStringProperty(Strings::MetadataEntryName(), wxS("feature_name"), wxString::FromUTF8(g3dFeature->GetName())))->Enable(false);
	// fields
	std::vector<std::string>&& fields = g3dFeature->GetFieldNames();
	for (std::string fieldName : fields) {
		geo3dml::FieldValue* fieldValue = g3dFeature->GetField(fieldName);
		switch (fieldValue->ValueType()) {
		case geo3dml::Field::ValueType::Text: {
			geo3dml::TextFieldValue* textValue = static_cast<geo3dml::TextFieldValue*>(fieldValue);
			AppendIn(categoryFeature, new wxStringProperty(wxString::FromUTF8(fieldName), wxPG_LABEL, wxString::FromUTF8(textValue->Value())))->ChangeFlag(wxPG_PROP_READONLY, true);
			break;
		}
		case geo3dml::Field::ValueType::Integer: {
			geo3dml::IntegerFieldValue* intValue = static_cast<geo3dml::IntegerFieldValue*>(fieldValue);
			AppendIn(categoryFeature, new wxIntProperty(wxString::FromUTF8(fieldName), wxPG_LABEL, intValue->Value()))->ChangeFlag(wxPG_PROP_READONLY, true);
			break;
		}
		case geo3dml::Field::ValueType::Double: {
			geo3dml::DoubleFieldValue* doubleValue = static_cast<geo3dml::DoubleFieldValue*>(fieldValue);
			AppendIn(categoryFeature, new wxFloatProperty(wxString::FromUTF8(fieldName), wxPG_LABEL, doubleValue->Value()))->ChangeFlag(wxPG_PROP_READONLY, true);
			break;
		}
		case geo3dml::Field::ValueType::Boolean: {
			geo3dml::BooleanFieldValue* boolValue = static_cast<geo3dml::BooleanFieldValue*>(fieldValue);
			AppendIn(categoryFeature, new wxBoolProperty(wxString::FromUTF8(fieldName), wxPG_LABEL, boolValue->Value()))->ChangeFlag(wxPG_PROP_READONLY, true);
			break;
		}
		default: {
			AppendIn(categoryFeature, new wxStringProperty(wxString::FromUTF8(fieldName), wxPG_LABEL, wxString::FromUTF8(geo3dml::Field::ValueTypeToName(fieldValue->ValueType()))));
			break;
		}
		}
	}
}

void MetadataPage::SetGeometryProperty(geo3dml::Geometry* g3dGeometry) {
	wxPGProperty* categoryGeometry = Append(new wxPropertyCategory(Strings::MetadataCategoryGeometry()));
	if (g3dGeometry == nullptr) {
		return;
	}
	
	wxString geoClassName = Strings::NameOfClassUnknown();
	geo3dml::TIN* tin = nullptr;
	geo3dml::UniformGrid* uniformGrid = nullptr;
	geo3dml::CornerPointGrid* cornerGrid = nullptr;
	geo3dml::Point* point = nullptr;
	geo3dml::LineString* lineString = nullptr;
	geo3dml::Annotation* annotation = nullptr;
	geo3dml::MultiPoint* mPoint = nullptr;
	tin = dynamic_cast<geo3dml::TIN*>(g3dGeometry);
	if (tin != nullptr) {
		geoClassName = Strings::NameOfClassG3DTIN();
	} else {
		uniformGrid = dynamic_cast<geo3dml::UniformGrid*>(g3dGeometry);
		if (uniformGrid != nullptr) {
			geoClassName = Strings::NameOfClassG3DUniformGrid();
		} else {
			cornerGrid = dynamic_cast<geo3dml::CornerPointGrid*>(g3dGeometry);
			if (cornerGrid != nullptr) {
				geoClassName = Strings::NameOfClassG3DCornerGrid();
			} else {
				point = dynamic_cast<geo3dml::Point*>(g3dGeometry);
				if (point != nullptr) {
					geoClassName = Strings::NameOfClassG3DPoint();
				} else {
					lineString = dynamic_cast<geo3dml::LineString*>(g3dGeometry);
					if (lineString != nullptr) {
						geoClassName = Strings::NameOfClassG3DLineString();
					} else {
						annotation = dynamic_cast<geo3dml::Annotation*>(g3dGeometry);
						if (annotation != nullptr) {
							geoClassName = Strings::NameOfClassG3DAnnotation();
						} else {
							mPoint = dynamic_cast<geo3dml::MultiPoint*>(g3dGeometry);
							if (mPoint != nullptr) {
								geoClassName = Strings::NameOfClassG3DMPoint();
							}
						}
					}
				}
			}
		}
	}

	AppendIn(categoryGeometry, new wxStringProperty(Strings::MetadataEntryId(), wxS("geometry_id"), wxString::FromUTF8(g3dGeometry->GetID())))->ChangeFlag(wxPG_PROP_READONLY, true);
	AppendIn(categoryGeometry, new wxStringProperty(Strings::MetadataEntryName(), wxS("geometry_name"), wxString::FromUTF8(g3dGeometry->GetName())))->Enable(false);
	AppendIn(categoryGeometry, new wxIntProperty(Strings::MetadataEntryGeometryLODLevel(), wxS("geometry_lod"), g3dGeometry->GetLODLevel()))->Enable(false);
	AppendIn(categoryGeometry, new wxStringProperty(Strings::MetadataEntryClassName(), wxS("geometry_class_name"), geoClassName))->Enable(false);
	if (tin != nullptr) {
		AppendIn(categoryGeometry, new wxIntProperty(Strings::MetadataEntryNumberOfVertices(), wxS("geometry_tin_vertcies"), tin->GetVertexCount()))->ChangeFlag(wxPG_PROP_READONLY, true);
		AppendIn(categoryGeometry, new wxIntProperty(Strings::MetadataEntryNumberOfTriangles(), wxS("geometry_tin_triangles"), tin->GetTriangleCount()))->ChangeFlag(wxPG_PROP_READONLY, true);
	} else if (uniformGrid != nullptr) {
		double x = 0, y = 0, z = 0;
		uniformGrid->GetOrigin(x, y, z);
		wxString str = wxString::Format("(%.6f, %.6f, %.6f)", x, y, z);
		AppendIn(categoryGeometry, new wxStringProperty(Strings::MetadataEntryGridOrigin(), wxS("geometry_grid_origin"), str))->ChangeFlag(wxPG_PROP_READONLY, true);
		uniformGrid->GetSteps(x, y, z);
		str = wxString::Format("(%.6f, %.6f, %.6f)", x, y, z);
		AppendIn(categoryGeometry, new wxStringProperty(Strings::MetadataEntryGridCellSize(), wxS("geometry_grid_cell_size"), str))->ChangeFlag(wxPG_PROP_READONLY, true);
		int i = 0, j = 0, k = 0;
		uniformGrid->GetDimensions(i, j, k);
		str = wxString::Format("(%d, %d, %d)", i, j, k);
		AppendIn(categoryGeometry, new wxStringProperty(Strings::MetadataEntryGridCellDimension(), wxS("geometry_grid_cell_dimesion"), str))->ChangeFlag(wxPG_PROP_READONLY, true);
	} else if (cornerGrid != nullptr) {
		int i = 0, j = 0, k = 0;
		cornerGrid->GetDimensions(i, j, k);
		wxString str = wxString::Format("(%d, %d, %d)", i, j, k);
		AppendIn(categoryGeometry, new wxStringProperty(Strings::MetadataEntryGridCellDimension(), wxS("geometry_grid_cell_dimesion"), str))->ChangeFlag(wxPG_PROP_READONLY, true);
	} else if (lineString != nullptr) {
		AppendIn(categoryGeometry, new wxIntProperty(Strings::MetadataEntryNumberOfVertices(), wxS("geometry_line_vertcies"), lineString->GetVertexCount()))->ChangeFlag(wxPG_PROP_READONLY, true);
	} else if (annotation != nullptr) {
		AppendIn(categoryGeometry, new wxIntProperty(Strings::MetadataEntryNumberOfVertices(), wxS("geometry_annotation_vertcies"), annotation->GetPointCount()))->ChangeFlag(wxPG_PROP_READONLY, true);
	} else if (mPoint != nullptr) {
		AppendIn(categoryGeometry, new wxIntProperty(Strings::MetadataEntryNumberOfVertices(), wxS("geometry_mpoint_vertcies"), mPoint->GetPointCount()))->ChangeFlag(wxPG_PROP_READONLY, true);
	}
	SetShapePropertyOfGeometry(categoryGeometry, g3dGeometry->GetProperty(geo3dml::ShapeProperty::SamplingTarget::Vertex));
	SetShapePropertyOfGeometry(categoryGeometry, g3dGeometry->GetProperty(geo3dml::ShapeProperty::SamplingTarget::Edge));
	SetShapePropertyOfGeometry(categoryGeometry, g3dGeometry->GetProperty(geo3dml::ShapeProperty::SamplingTarget::Face));
	SetShapePropertyOfGeometry(categoryGeometry, g3dGeometry->GetProperty(geo3dml::ShapeProperty::SamplingTarget::Voxel));
}

void MetadataPage::SetShapePropertyOfGeometry(wxPGProperty* parentCategory, geo3dml::ShapeProperty* shapeProperty) {
	if (shapeProperty == nullptr) {
		return;
	}
	wxString targetComponent = wxString::FromUTF8(geo3dml::ShapeProperty::SamplingTargetToName(shapeProperty->TargetComponent()));
	wxPGProperty* category = AppendIn(parentCategory, new wxPropertyCategory(targetComponent + Strings::MetadataCategoryShapeProperty()));
	AppendIn(category, new wxStringProperty(Strings::MetadataEntryName(), targetComponent + wxS("_name"), wxString::FromUTF8(shapeProperty->Name())))->ChangeFlag(wxPG_PROP_READONLY, true);
	AppendIn(category, new wxStringProperty(Strings::MetadataEntryId(), targetComponent + wxS("_id"), wxString::FromUTF8(shapeProperty->GetID())))->ChangeFlag(wxPG_PROP_READONLY, true);
	wxPGProperty* subCategory = AppendIn(category, new wxPropertyCategory(targetComponent + wxS("_") + Strings::MetadataCategorySchema()));
	for (int i = 0; i < shapeProperty->GetFieldCount(); ++i) {
		const geo3dml::Field& field = shapeProperty->GetFieldAt(i);
		SetFieldProperty(subCategory, field, i, targetComponent);
	}
	category->SetExpanded(false);
}

void MetadataPage::SetFieldProperty(wxPGProperty* parentCategory, const geo3dml::Field& field, int fieldIndex, const wxString& namePrefix) {
	wxPGProperty* propField = AppendIn(parentCategory, new wxStringProperty(wxString::Format("%d", fieldIndex + 1), namePrefix + wxS("_") + wxString::Format("field_%d", fieldIndex), wxString::FromUTF8(field.Name())));
	propField->Enable(false);
	AppendIn(propField, new wxStringProperty(wxS("Label"), wxPG_LABEL, wxString::FromUTF8(field.Label())))->Enable(false);
	AppendIn(propField, new wxStringProperty(wxS("DataType"), wxPG_LABEL, wxString::FromUTF8(geo3dml::Field::ValueTypeToName(field.DataType()))))->Enable(false);
	AppendIn(propField, new wxStringProperty(wxS("UOM"), wxPG_LABEL, wxString::FromUTF8(field.Uom())))->Enable(false);
	AppendIn(propField, new wxStringProperty(wxS("Definition"), wxPG_LABEL, wxString::FromUTF8(field.Definition())))->Enable(false);
	AppendIn(propField, new wxStringProperty(wxS("Description"), wxPG_LABEL, wxString::FromUTF8(field.Description())))->Enable(false);
	propField->SetExpanded(false);
}