#include "ProjectPanel.h"
#include "Events.h"
#include "Strings.h"

wxBEGIN_EVENT_TABLE(ProjectPanel, wxSplitterWindow)
	EVT_NOTIFY(wxEVT_NULL, Events::ID::Notify_ProjectTreeItemSelected, ProjectPanel::OnChildrenNotify)
wxEND_EVENT_TABLE()

ProjectPanel::ProjectPanel(wxWindow* parent, const wxSize& size) : wxSplitterWindow(parent, wxID_ANY, wxDefaultPosition, size) {
	projectTree_ = new ProjectTreeCtrl(this, wxSize(size.GetWidth(), size.GetHeight() / 2));
	projectMetaBook_ = new ProjectItemMetaBook(this);
	
	SplitHorizontally(projectTree_, projectMetaBook_, size.GetHeight() / 2);
	SetMinimumPaneSize(20);
}

ProjectPanel::~ProjectPanel() {

}

vtkRenderer* ProjectPanel::GetRenderer() const {
	return projectTree_->GetRenderer();
}

vtkTransform* ProjectPanel::GetTransform() const {
	return projectTree_->GetTransform();
}

geo3dml::Project* ProjectPanel::GetG3DProject() const {
	return projectTree_->GetG3DProject();
}

g3dgrid::VoxelGrid* ProjectPanel::GetG3DVoxelGrid() const {
	return projectTree_->GetG3DVoxelGrid();
}

void ProjectPanel::AppendG3DModel(geo3dml::Model* model, bool appendToDefaultMap) {
	projectTree_->AppendG3DModel(model, appendToDefaultMap);
}

void ProjectPanel::AppendG3DMap(geo3dml::Map* map) {
	projectTree_->AppendG3DMap(map);
}

void ProjectPanel::UpdateVoxelGridModel() {
	projectTree_->UpdateVoxelGridModel();
}

void ProjectPanel::ExpandStructureModelTree() {
	projectTree_->ExpandStructureModelTree();
}

void ProjectPanel::ExpandGridModelNodeTree() {
	projectTree_->ExpandGridModelNodeTree();
}

void ProjectPanel::OnChildrenNotify(wxNotifyEvent& event) {
	wxBusyCursor waiting;
	switch (event.GetId()) {
	case Events::ID::Notify_ProjectTreeItemSelected: {
		projectMetaBook_->BindToItem(static_cast<geo3dml::Object*>(event.GetClientData()), G3DTreeItemData::ItemType(event.GetInt()));
		break;
	}
	default:
		break;
	}
}

void ProjectPanel::CloseAllModels() {
	projectTree_->CloseAllModels();
}

void ProjectPanel::CloseStructureModel() {
	projectTree_->CloseStructureModel();
}

void ProjectPanel::CloseVoxelGridModel() {
	projectTree_->CloseVoxelGridModel();
}
