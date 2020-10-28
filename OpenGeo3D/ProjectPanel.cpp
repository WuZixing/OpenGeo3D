#include "ProjectPanel.h"
#include "Events.h"
#include "Strings.h"

wxBEGIN_EVENT_TABLE(ProjectPanel, wxSplitterWindow)
	EVT_NOTIFY(wxEVT_NULL, Events::ID::Notify_ProjectTreeItemSelected, ProjectPanel::OnChildrenNotify)
wxEND_EVENT_TABLE()

ProjectPanel::ProjectPanel(wxWindow* parent, const wxSize& size) : wxSplitterWindow(parent, wxID_ANY, wxDefaultPosition, size) {
	projectTree_ = new ProjectTreeCtrl(this, wxSize(size.GetWidth(), -1));
	projectMetaBook_ = new ProjectItemMetaBook(this);
	
	this->SplitHorizontally(projectTree_, projectMetaBook_, size.GetHeight());
	this->SetMinimumPaneSize(20);
}

ProjectPanel::~ProjectPanel() {

}

vtkRenderer* ProjectPanel::GetRenderer() const {
	return projectTree_->GetRenderer();
}

vtkTransform* ProjectPanel::GetTransform() const {
	return projectTree_->GetTransform();
}

void ProjectPanel::AppendG3DModel(geo3dml::Model* model, bool appendToDefaultMap) {
	projectTree_->AppendG3DModel(model, appendToDefaultMap);
}

void ProjectPanel::AppendG3DMap(geo3dml::Map* map) {
	projectTree_->AppendG3DMap(map);
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
