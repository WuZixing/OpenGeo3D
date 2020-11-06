#include "DlgNewGridLOD.h"
#include "Strings.h"

DlgNewGridLOD::DlgNewGridLOD(wxWindow* parent, int level) : wxDialog(parent, wxID_ANY, wxEmptyString) {
	level_ = level;
	parentSizeX_ = parentSizeY_ = parentSizeZ_ = 1;
	wxString title = wxString::Format(wxS("%s - %d"), Strings::TitleOfMenuItemAppendGridLOD(), level_);
	SetTitle(title);

	wxFlexGridSizer* sizer = new wxFlexGridSizer(4, 4, wxSize(2, 4));
	sizer->Add(new wxStaticText(this, wxID_ANY, wxEmptyString));
	sizer->Add(new wxStaticText(this, wxID_ANY, Strings::LabelOfX()), wxSizerFlags().CenterHorizontal());
	sizer->Add(new wxStaticText(this, wxID_ANY, Strings::LabelOfY()), wxSizerFlags().CenterHorizontal());
	sizer->Add(new wxStaticText(this, wxID_ANY, Strings::LabelOfZ()), wxSizerFlags().CenterHorizontal());
	ctrlSizeX_ = new wxTextCtrl(this, wxID_ANY);
	ctrlSizeY_ = new wxTextCtrl(this, wxID_ANY);
	ctrlSizeZ_ = new wxTextCtrl(this, wxID_ANY);
	sizer->Add(new wxStaticText(this, wxID_ANY, Strings::LabelOfGridCellSize()));
	sizer->Add(ctrlSizeX_);
	sizer->Add(ctrlSizeY_);
	sizer->Add(ctrlSizeZ_);
	ctrlScaleX_ = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS | wxSP_WRAP | wxALIGN_RIGHT, 1, 4, 2);
	ctrlScaleY_ = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS | wxSP_WRAP | wxALIGN_RIGHT, 1, 4, 2);
	ctrlScaleZ_ = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS | wxSP_WRAP | wxALIGN_RIGHT, 1, 4, 2);
	sizer->Add(new wxStaticText(this, wxID_ANY, Strings::LabelOfGridCellScale()));
	sizer->Add(ctrlScaleX_);
	sizer->Add(ctrlScaleY_);
	sizer->Add(ctrlScaleZ_);
	if (level_ == 0) {
		ctrlScaleX_->SetValue(1);
		ctrlScaleY_->SetValue(1);
		ctrlScaleZ_->SetValue(1);
		ctrlScaleX_->Disable();
		ctrlScaleY_->Disable();
		ctrlScaleZ_->Disable();
	} else {
		ctrlSizeX_->SetEditable(false);
		ctrlSizeY_->SetEditable(false);
		ctrlSizeZ_->SetEditable(false);
	}
	ctrlScaleX_->Bind(wxEVT_SPINCTRL, &DlgNewGridLOD::OnSetScaleX, this);
	ctrlScaleY_->Bind(wxEVT_SPINCTRL, &DlgNewGridLOD::OnSetScaleY, this);
	ctrlScaleZ_->Bind(wxEVT_SPINCTRL, &DlgNewGridLOD::OnSetScaleZ, this);
	
	wxSizer* btnSizer = wxDialog::CreateSeparatedButtonSizer(wxOK | wxCANCEL);

	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
	mainSizer->Add(sizer, wxSizerFlags(1).Expand().Border(wxALL, 10));
	mainSizer->Add(new wxStaticText(this, wxID_ANY, Strings::TipOfGridLODAndCellScale()), wxSizerFlags().Expand().Border(wxLEFT | wxRIGHT | wxBOTTOM, 10));
	mainSizer->Add(btnSizer, wxSizerFlags(0).Expand().Border(wxLEFT | wxRIGHT | wxBOTTOM, 10));
	SetSizerAndFit(mainSizer);
}

DlgNewGridLOD::~DlgNewGridLOD() {

}

void DlgNewGridLOD::SetParentLevelCellSize(double x, double y, double z) {
	parentSizeX_ = x;
	parentSizeY_ = y;
	parentSizeZ_ = z;
	int scaleX, scaleY, scaleZ;
	GetCellScale(scaleX, scaleY, scaleZ);
	ctrlScaleX_->SetValue(wxString::FromDouble(x / scaleX, 6));
	ctrlScaleY_->SetValue(wxString::FromDouble(y / scaleY, 6));
	ctrlScaleX_->SetValue(wxString::FromDouble(z / scaleZ, 6));
}

void DlgNewGridLOD::OnSetScaleX(wxSpinEvent& evt) {
	int scale = ctrlScaleX_->GetValue();
	ctrlSizeX_->SetValue(wxString::FromDouble(parentSizeX_ / scale, 6));
}

void DlgNewGridLOD::OnSetScaleY(wxSpinEvent& evt) {
	int scale = ctrlScaleY_->GetValue();
	ctrlSizeY_->SetValue(wxString::FromDouble(parentSizeY_ / scale, 6));
}

void DlgNewGridLOD::OnSetScaleZ(wxSpinEvent& evt) {
	int scale = ctrlScaleZ_->GetValue();
	ctrlSizeZ_->SetValue(wxString::FromDouble(parentSizeZ_ / scale, 6));
}

bool DlgNewGridLOD::GetCellScale(int& x, int& y, int& z) const {
	x = ctrlScaleX_->GetValue();
	y = ctrlScaleY_->GetValue();
	z = ctrlScaleZ_->GetValue();
	return true;
}

bool DlgNewGridLOD::GetCellSize(double& x, double& y, double& z) const {
	if (!ctrlSizeX_->GetValue().ToCDouble(&x)) {
		return false;
	}
	if (!ctrlSizeX_->GetValue().ToCDouble(&y)) {
		return false;
	}
	if (!ctrlSizeX_->GetValue().ToCDouble(&z)) {
		return false;
	}
	return true;
}