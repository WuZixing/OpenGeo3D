#include "MetadataPage.h"
#include "Strings.h"

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
	case G3DTreeItemData::ItemType::G3D_GridModel:
		break;
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
	default:
		break;
	}
}

void MetadataPage::BindToStructureModels(geo3dml::Project* g3dProject) {
	SetBasicInfo(Strings::NameOfStructureModel(), wxString::FromUTF8(g3dProject->GetID()), g3dProject->GetMapCount());
	double minX = 0, maxX = -1, minY = 0, maxY = -1, minZ = 0, maxZ = -1;
	g3dProject->GetMinimumBoundingRectangle(minX, minY, minZ, maxX, maxY, maxZ);
	SetMBR(minX, minY, minZ, maxX, maxY, maxZ);
}

void MetadataPage::BindToG3DMap(geo3dml::Map* g3dMap) {
	SetBasicInfo(Strings::NameOfClassG3DMap(), wxString::FromUTF8(g3dMap->GetID()), g3dMap->GetLayerCount());
	double minX = 0, maxX = -1, minY = 0, maxY = -1, minZ = 0, maxZ = -1;
	g3dMap->GetMinimumBoundingRectangle(minX, minY, minZ, maxX, maxY, maxZ);
	SetMBR(minX, minY, minZ, maxX, maxY, maxZ);
}

void MetadataPage::BindToG3DLayer(geo3dml::Layer* g3dLayer) {
	SetBasicInfo(Strings::NameOfClassG3DLayer(), wxString::FromUTF8(g3dLayer->GetID()), g3dLayer->GetActorCount());
	double minX = 0, maxX = -1, minY = 0, maxY = -1, minZ = 0, maxZ = -1;
	g3dLayer->GetMinimumBoundingRectangle(minX, minY, minZ, maxX, maxY, maxZ);
	SetMBR(minX, minY, minZ, maxX, maxY, maxZ);
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
	AppendIn(categoryFC, new wxStringProperty(Strings::MetadataEntryId(), wxT("feature_class_id"), strId))->ChangeFlag(wxPG_PROP_READONLY, true);
	AppendIn(categoryFC, new wxStringProperty(Strings::MetadataEntryName(), wxT("feature_class_name"), strName))->Enable(false);
	wxPGProperty* categorySchema = AppendIn(categoryFC, new wxPropertyCategory(Strings::MetadataCategorySchema()));
	for (int i = 0; i < g3dFeatureClass->GetFieldCount(); ++i) {
		const geo3dml::Field& field = g3dFeatureClass->GetFieldAt(i);
		wxPGProperty* propField = AppendIn(categorySchema, new wxStringProperty(wxString::Format("%d", i + 1), wxString::Format("field_%d", i), wxString::FromUTF8(field.Name())));
		propField->Enable(false);
		AppendIn(propField, new wxStringProperty(wxT("Label"), wxPG_LABEL, wxString::FromUTF8(field.Label())))->Enable(false);
		AppendIn(propField, new wxStringProperty(wxT("DataType"), wxPG_LABEL, wxString::FromUTF8(geo3dml::Field::ValueTypeToName(field.DataType()))))->Enable(false);
		AppendIn(propField, new wxStringProperty(wxT("UOM"), wxPG_LABEL, wxString::FromUTF8(field.Uom())))->Enable(false);
		AppendIn(propField, new wxStringProperty(wxT("Definition"), wxPG_LABEL, wxString::FromUTF8(field.Definition())))->Enable(false);
		AppendIn(propField, new wxStringProperty(wxT("Description"), wxPG_LABEL, wxString::FromUTF8(field.Description())))->Enable(false);
		propField->SetExpanded(false);
	}
}

void MetadataPage::BindToG3DActor(geo3dml::Actor* g3dActor) {
	SetBasicInfo(Strings::NameOfClassG3DActor(), wxString::FromUTF8(g3dActor->GetID()), 0);
	double minX = 0, maxX = -1, minY = 0, maxY = -1, minZ = 0, maxZ = -1;
	g3dActor->GetMinimumBoundingRectangle(minX, minY, minZ, maxX, maxY, maxZ);
	SetMBR(minX, minY, minZ, maxX, maxY, maxZ);
	// feature
	SetFeature(g3dActor->GetBindingFeature());
	// geometry
	SetGeometry(g3dActor->GetBindingGeometry());
}

void MetadataPage::SetBasicInfo(const wxString& datasetCategory, const wxString& datasetId, unsigned int numberOfChildren) {
	wxPGProperty* category = Append(new wxPropertyCategory(Strings::MetadataCategoryBasicInfo()));
	AppendIn(category, new wxStringProperty(Strings::MetadataEntryDatasetClassName(), wxT("dataset_class"), datasetCategory))->Enable(false);
	AppendIn(category, new wxUIntProperty(Strings::MetadataEntryChildrenNumber(), wxT("children_number"), numberOfChildren))->Enable(false);
	AppendIn(category, new wxStringProperty(Strings::MetadataEntryId(), wxT("dataset_id"), datasetId))->ChangeFlag(wxPG_PROP_READONLY, true);
}

void MetadataPage::SetMBR(double minX, double minY, double minZ, double maxX, double maxY, double maxZ) {
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
	AppendIn(category, new wxStringProperty(Strings::MetadataEntryMBRRangeX(), wxT("mbr_x_range"), xRange))->ChangeFlag(wxPG_PROP_READONLY, true);
	AppendIn(category, new wxStringProperty(Strings::MetadataEntryMBRRangeY(), wxT("mbr_y_range"), yRange))->ChangeFlag(wxPG_PROP_READONLY, true);
	AppendIn(category, new wxStringProperty(Strings::MetadataEntryMBRRangeZ(), wxT("mbr_z_range"), zRange))->ChangeFlag(wxPG_PROP_READONLY, true);
}

void MetadataPage::SetFeature(geo3dml::Feature* g3dFeature) {
	wxPGProperty* categoryFeature = Append(new wxPropertyCategory(Strings::MetadataCategoryFeature()));
	if (g3dFeature == nullptr) {
		return;
	}
	AppendIn(categoryFeature, new wxStringProperty(Strings::MetadataEntryId(), wxT("feature_id"), wxString::FromUTF8(g3dFeature->GetID())))->ChangeFlag(wxPG_PROP_READONLY, true);
	AppendIn(categoryFeature, new wxStringProperty(Strings::MetadataEntryName(), wxT("feature_name"), wxString::FromUTF8(g3dFeature->GetName())))->Enable(false);
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

void MetadataPage::SetGeometry(geo3dml::Geometry* g3dGeometry) {
	wxPGProperty* categoryGeometry = Append(new wxPropertyCategory(Strings::MetadataCategoryGeometry()));
	if (g3dGeometry == nullptr) {
		return;
	}
	AppendIn(categoryGeometry, new wxStringProperty(Strings::MetadataEntryId(), wxT("geometry_id"), wxString::FromUTF8(g3dGeometry->GetID())))->ChangeFlag(wxPG_PROP_READONLY, true);
	AppendIn(categoryGeometry, new wxStringProperty(Strings::MetadataEntryName(), wxT("geometry_name"), wxString::FromUTF8(g3dGeometry->GetName())))->Enable(false);
}
