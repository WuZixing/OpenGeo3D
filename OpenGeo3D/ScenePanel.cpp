#include "ScenePanel.h"
#include "Strings.h"
#include <wx/artprov.h>
#include <wx/propgrid/propgrid.h>

ScenePanel::ScenePanel(wxWindow* parent, const wxSize& size) : wxSplitterWindow(parent, wxID_ANY, wxDefaultPosition, size) {
	sceneTree_ = new wxTreeCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(size.GetWidth(), -1), wxTR_DEFAULT_STYLE | wxNO_BORDER | wxTR_HIDE_ROOT);
	wxTreeItemId root = sceneTree_->AddRoot(wxEmptyString, 0);
	sceneTree_->AppendItem(root, Strings::NameOfStructureModel());
	sceneTree_->AppendItem(root, Strings::NameOfGridModel());

	dataPropNotebook_ = new wxAuiNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_BOTTOM | wxBORDER_NONE);
	//wxBitmap page_bmp = wxArtProvider::GetBitmap(wxART_INFORMATION, wxART_OTHER, FromDIP(wxSize(16, 16)));
	//dataPropNotebook_ = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_DEFAULT | wxNB_NOPAGETHEME);
	dataPropNotebook_->AddPage(new wxPropertyGrid(dataPropNotebook_), Strings::TitleOfDataInfo(), true);
	dataPropNotebook_->AddPage(new wxPropertyGrid(dataPropNotebook_), Strings::TitleOfVisInfo(), false);
	
	this->SplitHorizontally(sceneTree_, dataPropNotebook_, size.GetHeight());
	this->SetMinimumPaneSize(20);
}

ScenePanel::~ScenePanel() {
}
