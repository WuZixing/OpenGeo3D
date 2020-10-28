#include "RenderOptionPage.h"
#include <vtkProperty.h>
#include <wx/propgrid/advprops.h>
#include <geo3dml/Annotation.h>
#include <geo3dml/CornerPointGrid.h>
#include <geo3dml/LineString.h>
#include <geo3dml/MultiPoint.h>
#include <geo3dml/Point.h>
#include <geo3dml/TIN.h>
#include <geo3dml/UniformGrid.h>
#include "Events.h"
#include "Strings.h"

class wxTransparenceProperty : public wxFloatProperty {
public:
	wxTransparenceProperty(const wxString& label = wxPG_LABEL, const wxString& name = wxPG_LABEL, double value = 0.0) : wxFloatProperty(label, name, value) {
	}

	virtual bool ValidateValue(wxVariant& value, wxPGValidationInfo& validationInfo) const override {
		if (!value.IsType(wxS("double"))) {
			validationInfo.SetFailureMessage(wxS("Transparency should be a number in interval [0, 1]."));
			return false;
		}
		double v = value.GetDouble();
		if (v < 0 || v > 1) {
			validationInfo.SetFailureMessage(wxS("Transparency should be a number in interval [0, 1]."));
			return false;
		}
		return true;
	}
};

RenderOptionPage::RenderOptionPage(wxWindow* parent) : wxPropertyGrid(parent) {
	g3dActor_ = nullptr;
	Bind(wxEVT_PG_CHANGED, &RenderOptionPage::OnRenderOptionChanged, this);
}

RenderOptionPage::~RenderOptionPage() {

}

void RenderOptionPage::BindToItem(geo3dml::Object* g3dObject, G3DTreeItemData::ItemType itemType) {
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
	SetSplitterLeft();
}

void RenderOptionPage::BindToStructureModels(geo3dml::Project* g3dProject) {

}

void RenderOptionPage::BindToG3DMap(geo3dml::Map* g3dMap) {

}

void RenderOptionPage::BindToG3DLayer(geo3dml::Layer* g3dLayer) {

}

void RenderOptionPage::BindToG3DActor(geo3dml::Actor* g3dActor) {
	geo3dml::Geometry* g3dGeometry = g3dActor->GetBindingGeometry();
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
	// basic info
	wxPGProperty* category = Append(new wxPropertyCategory(Strings::MetadataCategoryBasicInfo()));
	AppendIn(category, new wxStringProperty(Strings::MetadataEntryClassName(), wxS("geometry_class_name"), geoClassName))->Enable(false);
	AppendIn(category, new wxStringProperty(Strings::MetadataEntryId(), wxS("geometry_id"), wxString::FromUTF8(g3dGeometry->GetID())))->ChangeFlag(wxPG_PROP_READONLY, true);
	// render options
	g3dActor_ = dynamic_cast<g3dvtk::Actor*>(g3dActor);
	if (g3dActor_ == nullptr || !g3dActor_->GetVTKProp()->IsA("vtkActor")) {
		return;
	}
	vtkProperty* renderProp = ((vtkActor*)g3dActor_->GetVTKProp())->GetProperty();
	wxPGProperty* categoryObjectOption = Append(new wxPropertyCategory(Strings::RenderCategoryObjectOption()));
	double r = 0, g = 0, b = 0;
	renderProp->GetColor(r, g, b);
	wxColor clr((unsigned char)(r * 255), (unsigned char)(g * 255), (unsigned char)(b * 255));
	AppendIn(categoryObjectOption, new wxColourProperty(Strings::RenderEntryColor(), EntryName_ObjectColor_, clr));
	AppendIn(categoryObjectOption, new wxTransparenceProperty(Strings::RenderEntryTransparency(), EntryName_ObjectTransparency_, 1 - renderProp->GetOpacity()));
	wxArrayString representations;
	representations.push_back(Strings::NameOfRenderRepresentationModePoint());
	representations.push_back(Strings::NameOfRenderRepresentationModeWireframe());
	representations.push_back(Strings::NameOfRenderRepresentationModeSurface());
	wxArrayInt representationIndices;
	representationIndices.push_back(VTK_POINTS);
	representationIndices.push_back(VTK_WIREFRAME);
	representationIndices.push_back(VTK_SURFACE);
	wxEnumProperty* repModeProp = new wxEnumProperty(Strings::RenderEntryRepresentationMode(), EntryName_ObjectRepresentation_, representations, representationIndices, renderProp->GetRepresentation());
	AppendIn(categoryObjectOption, repModeProp);
	wxBoolProperty* boolProp = new wxBoolProperty(Strings::RenderEntryDrawVertex(), EntryName_ObjectDrawVertex_, renderProp->GetVertexVisibility());
	boolProp->SetAttribute(wxPG_BOOL_USE_CHECKBOX, 1);
	AppendIn(categoryObjectOption, boolProp);
	boolProp = new wxBoolProperty(Strings::RenderEntryDrawWireframe(), EntryName_ObjectDrawEdge_, renderProp->GetEdgeVisibility());
	boolProp->SetAttribute(wxPG_BOOL_USE_CHECKBOX, 1);
	AppendIn(categoryObjectOption, boolProp);
}

void RenderOptionPage::OnRenderOptionChanged(wxPropertyGridEvent& event) {
	vtkProperty* renderProp = nullptr;
	if (g3dActor_->GetVTKProp()->IsA("vtkActor")) {
		renderProp = ((vtkActor*)g3dActor_->GetVTKProp())->GetProperty();
	}

	wxPGProperty* property = event.GetProperty();
	wxString propName = property->GetName();
	if (propName == EntryName_ObjectColor_ && renderProp != nullptr) {
		wxColour clr;
		clr << property->GetValue();
		renderProp->SetColor(clr.Red() / 255.0, clr.Green() / 255.0, clr.Blue() / 255.0);
		Events::Notify(Events::ID::Notify_RefreshRenderWindow);
	} else if (propName == EntryName_ObjectTransparency_ && renderProp != nullptr) {
		double t = property->GetValue().GetDouble();
		renderProp->SetOpacity(1 - t);
		Events::Notify(Events::ID::Notify_RefreshRenderWindow);
	} else if (propName == EntryName_ObjectRepresentation_ && renderProp != nullptr) {
		renderProp->SetRepresentation(property->GetValue().GetInteger());
		Events::Notify(Events::ID::Notify_RefreshRenderWindow);
	} else if (propName == EntryName_ObjectDrawVertex_ && renderProp != nullptr) {
		long v = property->GetValue().GetInteger();
		renderProp->SetVertexVisibility(v);
		Events::Notify(Events::ID::Notify_RefreshRenderWindow);
	} else if (propName == EntryName_ObjectDrawEdge_) {
		long v = property->GetValue().GetInteger();
		renderProp->SetEdgeVisibility(v);
		Events::Notify(Events::ID::Notify_RefreshRenderWindow);
	}
}