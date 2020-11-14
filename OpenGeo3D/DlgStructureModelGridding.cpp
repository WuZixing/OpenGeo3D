#include "DlgStructureModelGridding.h"
#include <wx/notebook.h>
#include <geo3dml/MultiPoint.h>
#include <geo3dml/Point.h>
#include <geo3dml/TIN.h>
#include "Strings.h"

DlgStructureModelGridding::DlgStructureModelGridding(wxWindow* parent) : wxDialog(parent, wxID_ANY, Strings::TitleOfStructureModelGridding()), voxelGrid_(nullptr) {
	// infomation of structure model
	wxStaticBoxSizer* structureModelSizer = new wxStaticBoxSizer(wxVERTICAL, this, Strings::NameOfStructureModel());
	structureModelSizer->Add(new wxStaticText(structureModelSizer->GetStaticBox(), wxID_ANY, Strings::TipOfStructureModelGridding()), wxSizerFlags().Expand().Border(wxALL, 6));
	featureClassViewList_ = new wxDataViewListCtrl(structureModelSizer->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxSize(-1, FromDIP(250)));
	featureClassViewList_->AppendTextColumn(Strings::LabelOfNo(), wxDATAVIEW_CELL_INERT, 30);
	featureClassViewList_->AppendTextColumn(Strings::LabelOfID(), wxDATAVIEW_CELL_INERT, 80);
	featureClassViewList_->AppendTextColumn(Strings::LabelOfName(), wxDATAVIEW_CELL_INERT, 80);
	wxArrayString choices;
	choices.Add(Strings::NameOfGeologicFeatureBody());
	choices.Add(Strings::NameOfGeologicFeatureSamplePoint());
	choices.Add(Strings::Strings::NameOfGeologicFeatureUnknown());
	wxDataViewChoiceRenderer* choiceRenderer = new wxDataViewChoiceRenderer(choices);
	wxDataViewColumn* typeColumn = new wxDataViewColumn(Strings::LabelOfGeologicFeatureType(), choiceRenderer, 3, 60);
	featureClassViewList_->AppendColumn(typeColumn);
	structureModelSizer->Add(featureClassViewList_, wxSizerFlags(1).Expand().Border(wxLEFT | wxRIGHT | wxBOTTOM, 6));

	wxStaticBoxSizer* mbrSizer = new wxStaticBoxSizer(wxVERTICAL, structureModelSizer->GetStaticBox(), Strings::LabelOfGriddingRange());
	wxFlexGridSizer* mbrCtrls = new wxFlexGridSizer(3, 4, wxSize(2, 4));
	mbrCtrls->Add(new wxStaticText(mbrSizer->GetStaticBox(), wxID_ANY, wxEmptyString));
	mbrCtrls->Add(new wxStaticText(mbrSizer->GetStaticBox(), wxID_ANY, Strings::LabelOfX()), wxSizerFlags().CenterHorizontal());
	mbrCtrls->Add(new wxStaticText(mbrSizer->GetStaticBox(), wxID_ANY, Strings::LabelOfY()), wxSizerFlags().CenterHorizontal());
	mbrCtrls->Add(new wxStaticText(mbrSizer->GetStaticBox(), wxID_ANY, Strings::LabelOfZ()), wxSizerFlags().CenterHorizontal());
	mbrCtrls->Add(new wxStaticText(mbrSizer->GetStaticBox(), wxID_ANY, Strings::LabelOfMBRMinimumCoordinates()));
	ctrlMinX_ = new wxTextCtrl(mbrSizer->GetStaticBox(), wxID_ANY);
	mbrCtrls->Add(ctrlMinX_);
	ctrlMinY_ = new wxTextCtrl(mbrSizer->GetStaticBox(), wxID_ANY);
	mbrCtrls->Add(ctrlMinY_);
	ctrlMinZ_ = new wxTextCtrl(mbrSizer->GetStaticBox(), wxID_ANY);
	mbrCtrls->Add(ctrlMinZ_);
	mbrCtrls->Add(new wxStaticText(mbrSizer->GetStaticBox(), wxID_ANY, Strings::LabelOfMBRMaximumCoordinates()));
	ctrlMaxX_ = new wxTextCtrl(mbrSizer->GetStaticBox(), wxID_ANY);
	mbrCtrls->Add(ctrlMaxX_);
	ctrlMaxY_ = new wxTextCtrl(mbrSizer->GetStaticBox(), wxID_ANY);
	mbrCtrls->Add(ctrlMaxY_);
	ctrlMaxZ_ = new wxTextCtrl(mbrSizer->GetStaticBox(), wxID_ANY);
	mbrCtrls->Add(ctrlMaxZ_);
	mbrSizer->Add(mbrCtrls, wxSizerFlags(1).Expand().Border(wxALL, 4));
	ctrlGriddingRangeBySources_ = new wxCheckBox(mbrSizer->GetStaticBox(), wxID_ANY, Strings::LabelOfSetGriddingRangeBySourceFeatureClasses());
	ctrlGriddingRangeBySources_->SetValue(true);
	ctrlGriddingRangeBySources_->Bind(wxEVT_CHECKBOX, &DlgStructureModelGridding::OnCheckGridingRange, this);
	ctrlMinX_->Enable(false);
	ctrlMinY_->Enable(false);
	ctrlMinZ_->Enable(false);
	ctrlMaxX_->Enable(false);
	ctrlMaxY_->Enable(false);
	ctrlMaxZ_->Enable(false);
	mbrSizer->Add(ctrlGriddingRangeBySources_, wxSizerFlags().Expand().Border(wxLEFT | wxRIGHT | wxBOTTOM, 4));
	structureModelSizer->Add(mbrSizer, wxSizerFlags().Expand().Border(wxLEFT | wxRIGHT | wxBOTTOM, 6));

	// information of target grid model
	wxStaticBoxSizer* gridModelSizer = new wxStaticBoxSizer(wxVERTICAL, this, Strings::NameOfGridModel());
	gridModelSizer->Add(new wxStaticText(gridModelSizer->GetStaticBox(), wxID_ANY, Strings::TipOfTargetGridOfGridding()), wxSizerFlags().Expand().Border(wxALL, 6));
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
	gridModelSizer->Add(gridSourceSizer, wxSizerFlags().Expand().Border(wxLEFT | wxRIGHT | wxBOTTOM, 6));
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
	// source structure model
	if (featureClassViewList_->GetItemCount() < 1) {
		wxMessageBox(Strings::TipOfEmptyListOfVisibleLayersForGridding(), Strings::AppName(), wxOK | wxICON_EXCLAMATION);
		featureClassViewList_->SetFocus();
		return;
	}
	// gridding range
	double minX = 0, minY = 0, minZ = 0, maxX = 0, maxY = 0, maxZ = 0;
	if (!ctrlMinX_->GetValue().ToDouble(&minX)) {
		wxMessageBox(Strings::TipOfInvalidGriddingRange(), Strings::AppName(), wxOK | wxICON_EXCLAMATION);
		ctrlMinX_->SetFocus();
		return;
	}
	if (!ctrlMinY_->GetValue().ToDouble(&minY)) {
		wxMessageBox(Strings::TipOfInvalidGriddingRange(), Strings::AppName(), wxOK | wxICON_EXCLAMATION);
		ctrlMinY_->SetFocus();
		return;
	}
	if (!ctrlMinZ_->GetValue().ToDouble(&minZ)) {
		wxMessageBox(Strings::TipOfInvalidGriddingRange(), Strings::AppName(), wxOK | wxICON_EXCLAMATION);
		ctrlMinZ_->SetFocus();
		return;
	}
	if (!ctrlMaxX_->GetValue().ToDouble(&maxX)) {
		wxMessageBox(Strings::TipOfInvalidGriddingRange(), Strings::AppName(), wxOK | wxICON_EXCLAMATION);
		ctrlMaxX_->SetFocus();
		return;
	}
	if (!ctrlMaxY_->GetValue().ToDouble(&maxY)) {
		wxMessageBox(Strings::TipOfInvalidGriddingRange(), Strings::AppName(), wxOK | wxICON_EXCLAMATION);
		ctrlMaxY_->SetFocus();
		return;
	}
	if (!ctrlMaxZ_->GetValue().ToDouble(&maxZ)) {
		wxMessageBox(Strings::TipOfInvalidGriddingRange(), Strings::AppName(), wxOK | wxICON_EXCLAMATION);
		ctrlMaxZ_->SetFocus();
		return;
	}
	if (minX > maxX || minY > maxY || minZ > maxZ) {
		wxMessageBox(Strings::TipOfInvalidGriddingRange(), Strings::AppName(), wxOK | wxICON_EXCLAMATION);
		ctrlMinX_->SetFocus();
		return;
	}
	// target grid model and its LOD
	wxDataViewItem lodItem = gridLODList_->GetSelection();
	if (!lodItem.IsOk()) {
		wxMessageBox(Strings::TipOfSelectLODFroGridding(), Strings::AppName(), wxOK | wxICON_EXCLAMATION);
		gridLODList_->SetFocus();
		return;
	}
	EndModal(wxID_OK);
}

void DlgStructureModelGridding::AppendFeatureClassFromStructureModel(geo3dml::FeatureClass* fc) {
	if (fc == nullptr || fc->GetFeatureCount() < 1) {
		return;
	}
	wxVector<wxVariant> itemValue;
	itemValue.push_back(wxString::Format("%d", featureClassViewList_->GetItemCount() + 1));
	itemValue.push_back(wxString::FromUTF8(fc->GetID()));
	itemValue.push_back(wxString::FromUTF8(fc->GetName()));
	for (int i = 0; i < fc->GetFeatureCount(); ++i) {
		geo3dml::Feature* g3dFeature = fc->GetFeatureAt(i);
		if (g3dFeature->GetGeometryCount() > 0) {
			geo3dml::Geometry* geo = g3dFeature->GetGeometryAt(0);
			geo3dml::TIN* tin = dynamic_cast<geo3dml::TIN*>(geo);
			if (tin != nullptr) {
				itemValue.push_back(Strings::NameOfGeologicFeatureBody());
			} else {
				geo3dml::Point* point = dynamic_cast<geo3dml::Point*>(geo);
				if (point != nullptr) {
					itemValue.push_back(Strings::NameOfGeologicFeatureSamplePoint());
				} else {
					geo3dml::MultiPoint* mPoint = dynamic_cast<geo3dml::MultiPoint*>(geo);
					if (mPoint != nullptr) {
						itemValue.push_back(Strings::NameOfGeologicFeatureSamplePoint());
					} else {
						itemValue.push_back(Strings::NameOfGeologicFeatureUnknown());
					}
				}
			}
			break;
		}
	}
	featureClassViewList_->AppendItem(itemValue, (wxUIntPtr)fc);
	if (ctrlGriddingRangeBySources_->GetValue()) {
		UpdateGriddingRangeByStructureModel();
	}
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

void DlgStructureModelGridding::UpdateGriddingRangeByStructureModel() {
	double minX = 0, minY = 0, minZ = 0, maxX = -1, maxY = -1, maxZ = -1;
	int r = 0;
	for (; r < featureClassViewList_->GetItemCount(); ++r) {
		wxUIntPtr itemData = featureClassViewList_->GetItemData(featureClassViewList_->RowToItem(r));
		geo3dml::FeatureClass* featureClass = (geo3dml::FeatureClass*)(itemData);
		if (featureClass->GetMinimumBoundingRectangle(minX, minY, minZ, maxX, maxY, maxZ)) {
			break;
		}
	}
	if (r >= featureClassViewList_->GetItemCount()) {
		return;
	}
	for (++r; r < featureClassViewList_->GetItemCount(); ++r) {
		double mbr[6] = { 0 };
		wxUIntPtr itemData = featureClassViewList_->GetItemData(featureClassViewList_->RowToItem(r));
		geo3dml::FeatureClass* featureClass = (geo3dml::FeatureClass*)(itemData);
		if (featureClass->GetMinimumBoundingRectangle(mbr[0], mbr[1], mbr[2], mbr[3], mbr[4], mbr[5])) {
			if (minX > mbr[0]) {
				minX = mbr[0];
			}
			if (minY > mbr[1]) {
				minY = mbr[1];
			}
			if (minZ > mbr[2]) {
				minZ = mbr[2];
			}
			if (maxX < mbr[3]) {
				maxX = mbr[3];
			}
			if (maxY < mbr[4]) {
				maxY = mbr[4];
			}
			if (maxZ < mbr[5]) {
				maxZ = mbr[5];
			}
		}
	}
	ctrlMinX_->SetValue(wxString::FromDouble(minX, 6));
	ctrlMinY_->SetValue(wxString::FromDouble(minY, 6));
	ctrlMinZ_->SetValue(wxString::FromDouble(minZ, 6));
	ctrlMaxX_->SetValue(wxString::FromDouble(maxX, 6));
	ctrlMaxY_->SetValue(wxString::FromDouble(maxY, 6));
	ctrlMaxZ_->SetValue(wxString::FromDouble(maxZ, 6));
}

void DlgStructureModelGridding::OnCheckGridingRange(wxCommandEvent& event) {
	wxBusyCursor waiting;
	if (ctrlGriddingRangeBySources_->GetValue()) {
		UpdateGriddingRangeByStructureModel();
		ctrlMinX_->Enable(false);
		ctrlMinY_->Enable(false);
		ctrlMinZ_->Enable(false);
		ctrlMaxX_->Enable(false);
		ctrlMaxY_->Enable(false);
		ctrlMaxZ_->Enable(false);
	} else {
		ctrlMinX_->Enable(true);
		ctrlMinY_->Enable(true);
		ctrlMinZ_->Enable(true);
		ctrlMaxX_->Enable(true);
		ctrlMaxY_->Enable(true);
		ctrlMaxZ_->Enable(true);
	}
}