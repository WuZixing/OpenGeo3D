#include "ProjectPanel.h"
#include "Strings.h"
#include <wx/artprov.h>
#include <wx/propgrid/propgrid.h>
#include <g3dvtk/ObjectFactory.h>
#include <g3dvtk/Actor.h>

ProjectPanel::ProjectPanel(wxWindow* parent, const wxSize& size) : wxSplitterWindow(parent, wxID_ANY, wxDefaultPosition, size) {
	projectTree_ = new ProjectTreeCtrl(this, wxSize(size.GetWidth(), -1));
	
	dataPropNotebook_ = new wxAuiNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_BOTTOM | wxBORDER_NONE);
	dataPropNotebook_->AddPage(new wxPropertyGrid(dataPropNotebook_), Strings::TitleOfDataInfo(), true);
	dataPropNotebook_->AddPage(new wxPropertyGrid(dataPropNotebook_), Strings::TitleOfVisInfo(), false);
	
	this->SplitHorizontally(projectTree_, dataPropNotebook_, size.GetHeight());
	this->SetMinimumPaneSize(20);

	g3dvtk::ObjectFactory g3dFactory;
	g3dProject_ = g3dFactory.NewProject();
	g3dProject_->SetID(geo3dml::Object::NewID());
	g3dProject_->SetName(Strings::NameOfDefaultG3DProject().ToUTF8().data());
}

ProjectPanel::~ProjectPanel() {
	if (g3dProject_ != nullptr) {
		delete g3dProject_;
	}
}

vtkRenderer* ProjectPanel::GetRenderer() const {
	return projectTree_->GetRenderer();
}

vtkTransform* ProjectPanel::GetTransform() const {
	return projectTree_->GetTransform();
}

void ProjectPanel::AppendG3DModel(geo3dml::Model* model, bool appendToDefaultMap) {
	g3dProject_->AddModel(model);
	if (!appendToDefaultMap) {
		return;
	}

	geo3dml::Map* g3dMap = GetDefaultMap();
	int numOfFeatureClasses = model->GetFeatureClassCount();
	for (int i = 0; i < numOfFeatureClasses; ++i) {
		AppendFeatureClassToMap(model->GetFeatureClassAt(i), g3dMap);
	}
	projectTree_->UpdateSubTreeOfMap(g3dMap);
}

void ProjectPanel::AppendG3DMap(geo3dml::Map* map) {
	g3dProject_->AddMap(map);
	projectTree_->UpdateSubTreeOfMap(map);
}

geo3dml::Map* ProjectPanel::GetDefaultMap() {
	if (g3dProject_->GetMapCount() > 0) {
		return g3dProject_->GetMapAt(0);
	} else {
		g3dvtk::ObjectFactory g3dFactory;
		geo3dml::Map* map = g3dFactory.NewMap();
		map->SetID(geo3dml::Object::NewID());
		map->SetName(Strings::NameOfDefaultG3DMap().ToUTF8().data());
		g3dProject_->AddMap(map);
		return map;
	}
}

void ProjectPanel::AppendFeatureClassToMap(geo3dml::FeatureClass* g3dFC, geo3dml::Map* g3dMp) {
	g3dvtk::ObjectFactory g3dFactory;
	geo3dml::Layer* layer = g3dFactory.NewLayer();
	layer->SetName(g3dFC->GetName());
	layer->BindFeatureClass(g3dFC);
	layer->BuildActorsFromFeatures(&g3dFactory);
	g3dMp->AddLayer(layer);
}


