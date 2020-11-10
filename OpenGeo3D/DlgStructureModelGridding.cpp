#include "DlgStructureModelGridding.h"
#include <wx/dataview.h>
#include <wx/notebook.h>
#include "Strings.h"

DlgStructureModelGridding::DlgStructureModelGridding(wxWindow* parent) : wxDialog(parent, wxID_ANY, Strings::TitleOfStructureModelGridding()), voxelGrid_(nullptr) {
	// infomation of structure model
	wxStaticBoxSizer* structureModelSizer = new wxStaticBoxSizer(wxVERTICAL, this, Strings::NameOfStructureModel());
	structureModelSizer->Add(new wxStaticText(structureModelSizer->GetStaticBox(), wxID_ANY, Strings::TipOfStructureModelGridding()), wxSizerFlags().Expand().Border(wxALL, 6));
	wxDataViewListCtrl* featureClassViewList = new wxDataViewListCtrl(structureModelSizer->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxSize(-1, FromDIP(250)));
	featureClassViewList->AppendTextColumn(Strings::LabelOfNo(), wxDATAVIEW_CELL_INERT, 40);
	featureClassViewList->AppendTextColumn(Strings::LabelOfID(), wxDATAVIEW_CELL_INERT, 80);
	featureClassViewList->AppendTextColumn(Strings::LabelOfName(), wxDATAVIEW_CELL_INERT, 80);
	wxArrayString choices;
	choices.Add(Strings::NameOfGeologicFeatureBody());
	choices.Add(Strings::NameOfGeologicFeatureSamplePoint());
	wxDataViewChoiceRenderer* choiceRenderer = new wxDataViewChoiceRenderer(choices);
	wxDataViewColumn* typeColumn = new wxDataViewColumn(Strings::LabelOfGeologicFeatureType(), choiceRenderer, 3, 60);
	featureClassViewList->AppendColumn(typeColumn);
	structureModelSizer->Add(featureClassViewList, wxSizerFlags(1).Expand().Border(wxLEFT | wxRIGHT | wxBOTTOM, 6));

	// information of target grid model
	wxStaticBoxSizer* gridModelSizer = new wxStaticBoxSizer(wxVERTICAL, this, Strings::NameOfGridModel());
	// grid model from local or remote data srouce
	wxFlexGridSizer* gridSourceSizer = new wxFlexGridSizer(2, FromDIP(wxSize(4, 2)));
	gridSourceSizer->Add(new wxStaticText(gridModelSizer->GetStaticBox(), wxID_ANY, Strings::LabelOfGridModelSource()), wxSizerFlags().CenterVertical());
	ctrlLocalGrid_ = new wxRadioButton(gridModelSizer->GetStaticBox(), wxID_ANY, Strings::LabelOfLocalDataSource(), wxDefaultPosition, wxDefaultSize, wxRB_SINGLE);
	ctrlLocalGrid_->SetValue(true);
	ctrlLocalGrid_->Bind(wxEVT_RADIOBUTTON, &DlgStructureModelGridding::OnLocalGrid, this);
	gridSourceSizer->Add(ctrlLocalGrid_, wxSizerFlags().CenterVertical());
	gridSourceSizer->Add(new wxStaticText(gridModelSizer->GetStaticBox(), wxID_ANY, wxEmptyString));
	wxBoxSizer* remoteGridSizer = new wxBoxSizer(wxHORIZONTAL);
	ctrlRemoteGrid_ = new wxRadioButton(gridModelSizer->GetStaticBox(), wxID_ANY, Strings::LabelOfRemoteDataSource(), wxDefaultPosition, wxDefaultSize, wxRB_SINGLE);
	ctrlRemoteGrid_->Bind(wxEVT_RADIOBUTTON, &DlgStructureModelGridding::OnRemoteGrid, this);
	ctrlRemoteGrid_->Enable(false);
	remoteGridSizer->Add(ctrlRemoteGrid_, wxSizerFlags().CenterVertical());
	wxComboBox* remoteGrids = new wxComboBox(gridModelSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, FromDIP(wxSize(150, -1)), wxArrayString());
	remoteGrids->Enable(false);
	remoteGridSizer->Add(remoteGrids, wxSizerFlags().CenterVertical().Border(wxLEFT, 2));
	gridSourceSizer->Add(remoteGridSizer);
	gridModelSizer->Add(gridSourceSizer, wxSizerFlags().Expand().Border(wxALL, 6));
	// metadata of grid model
	wxFlexGridSizer* gridInfoSizer = new wxFlexGridSizer(4, FromDIP(wxSize(4, 4)));
	gridInfoSizer->Add(new wxStaticText(gridModelSizer->GetStaticBox(), wxID_ANY, Strings::LabelOfID()), wxSizerFlags().CenterVertical());
	ctrlGridId_ = new wxTextCtrl(gridModelSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, FromDIP(wxSize(150, -1)), wxTE_READONLY);
	gridInfoSizer->Add(ctrlGridId_);
	gridInfoSizer->Add(new wxStaticText(gridModelSizer->GetStaticBox(), wxID_ANY, Strings::LabelOfName()), wxSizerFlags().CenterVertical().Border(wxLEFT, 6));
	ctrlGridName_ = new wxTextCtrl(gridModelSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, FromDIP(wxSize(150, -1)), wxTE_READONLY);
	gridInfoSizer->Add(ctrlGridName_);
	gridInfoSizer->Add(new wxStaticText(gridModelSizer->GetStaticBox(), wxID_ANY, Strings::LabelOfDescription()), wxSizerFlags().CenterVertical());
	ctrlGridDescription_ = new wxTextCtrl(gridModelSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, FromDIP(wxSize(150, -1)), wxTE_READONLY);
	gridInfoSizer->Add(ctrlGridDescription_);
	gridInfoSizer->Add(new wxStaticText(gridModelSizer->GetStaticBox(), wxID_ANY, Strings::LabelOfSRS()), wxSizerFlags().CenterVertical().Border(wxLEFT, 6));
	ctrlGridSRS_ = new wxTextCtrl(gridModelSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, FromDIP(wxSize(150, -1)), wxTE_READONLY);
	gridInfoSizer->Add(ctrlGridSRS_);
	gridInfoSizer->Add(new wxStaticText(gridModelSizer->GetStaticBox(), wxID_ANY, Strings::LabelOfGridOrigin()), wxSizerFlags().CenterVertical());
	ctrlGridOrigin_ = new wxTextCtrl(gridModelSizer->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, FromDIP(wxSize(150, -1)), wxTE_READONLY);
	gridInfoSizer->Add(ctrlGridOrigin_);
	gridModelSizer->Add(gridInfoSizer, wxSizerFlags().Expand().Border(wxALL, 6));
	// LODs of grid model
	gridLODList_ = new GridLODViewListCtrl(gridModelSizer->GetStaticBox(), FromDIP(wxSize(500, 250)), false);
	gridModelSizer->Add(gridLODList_, wxSizerFlags(1).Expand().Border(wxLEFT | wxRIGHT | wxBOTTOM, 6));

	wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);
	topSizer->Add(structureModelSizer, wxSizerFlags().Expand());
	topSizer->Add(gridModelSizer, wxSizerFlags().Expand().Border(wxLEFT, 6));

	wxSizer* btnSizer = wxDialog::CreateSeparatedButtonSizer(wxOK | wxCANCEL);
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &DlgStructureModelGridding::OnButtonOK, this, wxID_OK);

	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
	mainSizer->Add(topSizer, wxSizerFlags(1).Expand().Border(wxALL, 10));
	mainSizer->Add(btnSizer, wxSizerFlags().Expand().Border(wxLEFT | wxRIGHT | wxBOTTOM, 10));
	SetSizerAndFit(mainSizer);
}

DlgStructureModelGridding::~DlgStructureModelGridding() {

}

void DlgStructureModelGridding::OnLocalGrid(wxCommandEvent& event) {
	bool value = ctrlLocalGrid_->GetValue();
	ctrlRemoteGrid_->SetValue(!value);
}

void DlgStructureModelGridding::OnRemoteGrid(wxCommandEvent& event) {
	bool value = ctrlRemoteGrid_->GetValue();
	ctrlLocalGrid_->SetValue(!value);
}

void DlgStructureModelGridding::OnButtonOK(wxCommandEvent& event) {

}

void DlgStructureModelGridding::AppendFeatureClassFromStructureModel(geo3dml::FeatureClass* fc) {

}

void DlgStructureModelGridding::SetLocalGridModel(g3dgrid::VoxelGrid* voxelGrid) {
	wxBusyCursor waiting;

	voxelGrid_ = voxelGrid;
	if (voxelGrid_ == nullptr || !ctrlLocalGrid_->GetValue()) {
		return;
	}
	ctrlGridId_->SetValue(wxString::FromUTF8(voxelGrid_->GetID()));
	ctrlGridName_->SetValue(wxString::FromUTF8(voxelGrid_->GetName()));
	ctrlGridSRS_->SetValue(wxString::FromUTF8(voxelGrid_->GetSRS()));
	ctrlGridDescription_->SetValue(wxString::FromUTF8(voxelGrid_->GetDescription()));
	geo3dml::Point3D origin = voxelGrid_->GetOrigin();
	ctrlGridOrigin_->SetValue(wxString::Format("(%.6f, %.6f, %.6f)", origin.x, origin.y, origin.z));
	gridLODList_->BindToLocalGrid(voxelGrid_);
}