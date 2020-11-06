#include "DlgEditVoxelGrid.h"
#include <wx/notebook.h>
#include "GridLODViewListCtrl.h"
#include "GridSchemaViewListCtrl.h"
#include "Strings.h"

DlgEditVoxelGrid::DlgEditVoxelGrid(wxWindow* parent, g3dgrid::VoxelGrid* voxelGrid) : wxDialog(parent, wxID_ANY, Strings::TitleOfMenuItemEditVoxelGrid()) {
	voxelGrid_ = voxelGrid;

	wxFlexGridSizer* sizer = new wxFlexGridSizer(4, 4, wxSize(2, 4));
	sizer->Add(new wxStaticText(this, wxID_ANY, Strings::LabelOfID()), wxSizerFlags().Right());
	ctrlID_ = new wxTextCtrl(this, wxID_ANY, wxString::FromUTF8(voxelGrid_->GetID()), wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
	sizer->Add(ctrlID_);
	sizer->Add(new wxStaticText(this, wxID_ANY, Strings::LabelOfName()), wxSizerFlags().Right());
	ctrlName_ = new wxTextCtrl(this, wxID_ANY, wxString::FromUTF8(voxelGrid_->GetName()));
	sizer->Add(ctrlName_);
	sizer->Add(new wxStaticText(this, wxID_ANY, Strings::LabelOfDescription()), wxSizerFlags().Right());
	ctrlDescription_ = new wxTextCtrl(this, wxID_ANY, wxString::FromUTF8(voxelGrid_->GetDescription()));
	sizer->Add(ctrlDescription_);
	sizer->Add(new wxStaticText(this, wxID_ANY, Strings::LabelOfSRS()), wxSizerFlags().Right());
	ctrlSRS_ = new wxTextCtrl(this, wxID_ANY, wxString::FromUTF8(voxelGrid_->GetSRS()));
	sizer->Add(ctrlSRS_);

	sizer->Add(new wxStaticText(this, wxID_ANY, wxEmptyString));
	sizer->Add(new wxStaticText(this, wxID_ANY, Strings::LabelOfX()), wxSizerFlags().CenterHorizontal());
	sizer->Add(new wxStaticText(this, wxID_ANY, Strings::LabelOfY()), wxSizerFlags().CenterHorizontal());
	sizer->Add(new wxStaticText(this, wxID_ANY, Strings::LabelOfZ()), wxSizerFlags().CenterHorizontal());
	sizer->Add(new wxStaticText(this, wxID_ANY, Strings::LabelOfGridOrigin()), wxSizerFlags().Right());
	geo3dml::Point3D origin = voxelGrid_->GetOrigin();
	ctrlX_ = new wxTextCtrl(this, wxID_ANY, wxString::FromDouble(origin.x, 6));
	ctrlY_ = new wxTextCtrl(this, wxID_ANY, wxString::FromDouble(origin.y, 6));
	ctrlZ_ = new wxTextCtrl(this, wxID_ANY, wxString::FromDouble(origin.z, 6));
	sizer->Add(ctrlX_);
	sizer->Add(ctrlY_);
	sizer->Add(ctrlZ_);

	wxNotebook* notebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, FromDIP(wxSize(500, 250)), wxNB_DEFAULT | wxNB_NOPAGETHEME);
	notebook->AddPage(new GridLODViewListCtrl(notebook, voxelGrid_), Strings::LabelOfGridLOD());
	notebook->AddPage(new GridSchemaViewListCtrl(notebook, voxelGrid_), Strings::LabelOfSchema());

	wxSizer* btnSizer = wxDialog::CreateSeparatedButtonSizer(wxOK | wxCANCEL);
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &DlgEditVoxelGrid::OnButtonOK, this, wxID_OK);

	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
	mainSizer->Add(sizer, wxSizerFlags().Expand().Border(wxALL, 10));
	mainSizer->Add(notebook, wxSizerFlags(1).Expand().Border(wxLEFT | wxRIGHT | wxBOTTOM, 10));
	mainSizer->Add(new wxStaticText(this, wxID_ANY, Strings::TipOfGridLODAndCellScale()), wxSizerFlags().Expand().Border(wxLEFT | wxRIGHT | wxBOTTOM, 10));
	mainSizer->Add(btnSizer, wxSizerFlags().Expand().Border(wxLEFT | wxRIGHT | wxBOTTOM, 10));
	SetSizerAndFit(mainSizer);
}

DlgEditVoxelGrid::~DlgEditVoxelGrid() {

}

void DlgEditVoxelGrid::OnButtonOK(wxCommandEvent& event) {

}
