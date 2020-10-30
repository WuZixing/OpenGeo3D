#include "DlgOpenSimpleDrillLog.h"
#include <g3dvtk/ObjectFactory.h>
#include "DrillLogFile.h"
#include "Strings.h"

DlgOpenSimpleDrillLog::DlgOpenSimpleDrillLog(wxWindow* parent) : wxDialog(parent, wxID_ANY, Strings::TitleOfMenuItemOpenSimpleDrillLog()) {
	sizerOfDrillFiles_ = new SizerOfSimpleDrillLogFiles(this, Strings::TitleOfSizerOfSimpleDrillLog());

	wxSizer* btnSizer = wxDialog::CreateSeparatedButtonSizer(wxOK | wxCANCEL);
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &DlgOpenSimpleDrillLog::OnButtonOK, this, wxID_OK);

	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
	mainSizer->Add(sizerOfDrillFiles_, wxSizerFlags(1).Expand().Border(wxALL, 4));
	mainSizer->Add(btnSizer, wxSizerFlags(0).Expand().Border(wxALL, 4));

	SetSizerAndFit(mainSizer);
}

DlgOpenSimpleDrillLog::~DlgOpenSimpleDrillLog() {

}

void DlgOpenSimpleDrillLog::OnButtonOK(wxCommandEvent& event) {
	// check drill position
	wxString pathOfDrillPosFile = sizerOfDrillFiles_->GetDrillPositionFilePath();
	if (pathOfDrillPosFile.IsEmpty() || sizerOfDrillFiles_->CountDrillPositins() < 1) {
		wxMessageBox(Strings::TipOfInvalidDrillPositionData(), Strings::AppName(), wxICON_ERROR);
		sizerOfDrillFiles_->GetDrillPositionFilePathCtrl()->SetFocus();
		return;
	}
	// check drill log data
	if (sizerOfDrillFiles_->CountDrillLogFiles() < 1 || sizerOfDrillFiles_->CountDrillLogFields() < 1) {
		wxMessageBox(Strings::TipOfInvalidDrillLogData(), Strings::AppName(), wxICON_ERROR);
		sizerOfDrillFiles_->GetDrillLogListCtrl()->SetFocus();
		return;
	}
	EndModal(wxID_OK);
}

geo3dml::Model* DlgOpenSimpleDrillLog::LoadAsG3DModel() const {
	DrillPositionMap&& drillPositions = sizerOfDrillFiles_->GetDrillPositions();
	if (drillPositions.empty()) {
		return nullptr;
	}
	DrillLogFileMap&& drillFiles = sizerOfDrillFiles_->GetDrillLogFiles();
	if (drillFiles.empty()) {
		return nullptr;
	}
	DrillLogFieldMap&& targetFields = sizerOfDrillFiles_->GetDrillLogFields();
	// model
	g3dvtk::ObjectFactory g3dFactory;
	geo3dml::Model* model = g3dFactory.NewModel();
	model->SetID("Drills");
	model->SetName(Strings::NameOfDrillModel().ToUTF8().data());
	model->SetType(geo3dml::Model::ModelType::Model3D);
	// feature class of drill
	geo3dml::Field fieldPath;
	fieldPath.Name("file").Label("File Path").DataType(geo3dml::Field::Text);
	geo3dml::FeatureClass* fcDrills = g3dFactory.NewFeatureClass();
	fcDrills->SetID("Drills");
	fcDrills->SetName(Strings::NameOfDrillFeatureClass().ToUTF8().data());
	fcDrills->AddField(fieldPath);
	model->AddFeatureClass(fcDrills);
	// features of drill
	geo3dml::Feature* featurePos = g3dFactory.NewFeature();
	featurePos->SetID("DrillPosition");
	featurePos->SetName(Strings::NameOfDrillPositionFeature().ToUTF8().data());
	geo3dml::TextFieldValue* textValue = new geo3dml::TextFieldValue(fieldPath.Name());
	textValue->Value("");
	featurePos->SetField(textValue);
	geo3dml::Annotation* annotation = g3dFactory.NewAnnotation();
	featurePos->AddGeometry(annotation);
	fcDrills->AddFeature(featurePos);
	for (auto drillFile = drillFiles.begin(); drillFile != drillFiles.end(); ++drillFile) {
		if (drillPositions.find(drillFile->first) == drillPositions.end()) {
			continue;
		}
		geo3dml::Point3D drillPos = drillPositions[drillFile->first];
		annotation->AddPoint(drillPos.x, drillPos.y, drillPos.z);
		annotation->SetLabelOfPointAt(annotation->GetPointCount() - 1, drillFile->first.ToUTF8().data());
		DrillLogFile drillLog(drillFile->first.ToUTF8().data(), drillPos, drillFile->second.ToUTF8().data());
		geo3dml::Feature* feature = g3dFactory.NewFeature();
		feature->SetID(drillLog.GetDrillNo());
		feature->SetName(drillLog.GetDrillNo());
		geo3dml::TextFieldValue* textValue = new geo3dml::TextFieldValue(fieldPath.Name());
		textValue->Value(drillLog.GetFilePath());
		feature->SetField(textValue);
		g3dvtk::MultiPoint* mPt = drillLog.ToMultiPoint(targetFields);
		feature->AddGeometry(mPt);
		fcDrills->AddFeature(feature);
	}
	return model;
}