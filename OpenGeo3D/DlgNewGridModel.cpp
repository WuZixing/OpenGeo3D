#include "DlgNewGridModel.h"
#include "Strings.h"

DlgNewGridModel::DlgNewGridModel(wxWindow* parent) : wxDialog(parent, wxID_ANY, Strings::TitleOfMenuItemNewGridModel()) {
	wxFlexGridSizer* sizer = new wxFlexGridSizer(4, 4, wxSize(2, 4));
	sizer->Add(new wxStaticText(this, wxID_ANY, Strings::LabelOfName()), wxSizerFlags().Right());
	ctrlName_ = new wxTextCtrl(this, wxID_ANY);
	sizer->Add(ctrlName_);
	sizer->Add(new wxStaticText(this, wxID_ANY, Strings::LabelOfDescription()), wxSizerFlags().Right());
	ctrlDescription_ = new wxTextCtrl(this, wxID_ANY);
	sizer->Add(ctrlDescription_);
	sizer->Add(new wxStaticText(this, wxID_ANY, Strings::LabelOfSRS()), wxSizerFlags().Right());
	ctrlSRS_ = new wxTextCtrl(this, wxID_ANY);
	sizer->Add(ctrlSRS_);

	sizer->Add(new wxStaticText(this, wxID_ANY, wxEmptyString));
	sizer->Add(new wxStaticText(this, wxID_ANY, wxEmptyString));
	sizer->Add(new wxStaticText(this, wxID_ANY, wxEmptyString));
	sizer->Add(new wxStaticText(this, wxID_ANY, Strings::LabelOfX()), wxSizerFlags().CenterHorizontal());
	sizer->Add(new wxStaticText(this, wxID_ANY, Strings::LabelOfY()), wxSizerFlags().CenterHorizontal());
	sizer->Add(new wxStaticText(this, wxID_ANY, Strings::LabelOfZ()), wxSizerFlags().CenterHorizontal());
	sizer->Add(new wxStaticText(this, wxID_ANY, Strings::LabelOfGridOrigin()), wxSizerFlags().Right());
	ctrlX_ = new wxTextCtrl(this, wxID_ANY);
	ctrlY_ = new wxTextCtrl(this, wxID_ANY);
	ctrlZ_ = new wxTextCtrl(this, wxID_ANY);
	sizer->Add(ctrlX_);
	sizer->Add(ctrlY_);
	sizer->Add(ctrlZ_);

	wxSizer* btnSizer = wxDialog::CreateSeparatedButtonSizer(wxOK | wxCANCEL);
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &DlgNewGridModel::OnButtonOK, this, wxID_OK);

	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
	mainSizer->Add(sizer, wxSizerFlags(1).Expand().Border(wxALL, 10));
	mainSizer->Add(btnSizer, wxSizerFlags(0).Expand().Border(wxLEFT | wxRIGHT | wxBOTTOM, 10));
	SetSizerAndFit(mainSizer);
}

DlgNewGridModel::~DlgNewGridModel() {

}

void DlgNewGridModel::OnButtonOK(wxCommandEvent& event) {
	if (ctrlName_->GetValue().IsEmpty()) {
		wxMessageBox(Strings::TipOfInvalidName(), Strings::AppName(), wxICON_ERROR);
		ctrlName_->SetFocus();
		return;
	}
	if (ctrlSRS_->GetValue().IsEmpty()) {
		wxMessageBox(Strings::TipOfInvalidSRS(), Strings::AppName(), wxICON_ERROR);
		ctrlSRS_->SetFocus();
		return;
	}
	double v = 0;
	wxString str = ctrlX_->GetValue();
	if (str.IsEmpty() || !str.ToCDouble(&v)) {
		wxMessageBox(Strings::TipOfInvalidGridOrigin(), Strings::AppName(), wxICON_ERROR);
		ctrlX_->SetFocus();
		return;
	}
	str = ctrlY_->GetValue();
	if (str.IsEmpty() || !str.ToCDouble(&v)) {
		wxMessageBox(Strings::TipOfInvalidGridOrigin(), Strings::AppName(), wxICON_ERROR);
		ctrlY_->SetFocus();
		return;
	}
	str = ctrlZ_->GetValue();
	if (str.IsEmpty() || !str.ToCDouble(&v)) {
		wxMessageBox(Strings::TipOfInvalidGridOrigin(), Strings::AppName(), wxICON_ERROR);
		ctrlZ_->SetFocus();
		return;
	}
	EndModal(wxID_OK);
}

g3dgrid::VoxelGrid* DlgNewGridModel::MakeGrid() const {
	wxString name = ctrlName_->GetValue();
	wxString descriptoin = ctrlDescription_->GetValue();
	wxString srs = ctrlSRS_->GetValue();
	double x = 0, y = 0, z = 0;
	ctrlX_->GetValue().ToCDouble(&x);
	ctrlY_->GetValue().ToCDouble(&y);
	ctrlZ_->GetValue().ToCDouble(&z);
	g3dgrid::VoxelGrid* grid = new g3dgrid::VoxelGrid();
	grid->SetID(geo3dml::Object::NewID());
	grid->SetName(name.ToUTF8().data())
		.SetDescription(descriptoin.ToUTF8().data())
		.SetSRS(srs.ToUTF8().data())
		.SetOrigin(geo3dml::Point3D(x, y, z));
	return grid;
}