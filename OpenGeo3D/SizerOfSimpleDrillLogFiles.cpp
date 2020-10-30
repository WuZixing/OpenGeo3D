#include "SizerOfSimpleDrillLogFiles.h"
#include <wx/filename.h>
#include <wx/textfile.h>
#include <wx/tokenzr.h>
#include <geo3dml/Field.h>
#include "Strings.h"

SizerOfSimpleDrillLogFiles::SizerOfSimpleDrillLogFiles(wxWindow* parent, const wxString& label) : wxStaticBoxSizer(wxVERTICAL, parent, label) {
	// drill position
	wxBoxSizer* sizerOfPositionFilePath = new wxBoxSizer(wxHORIZONTAL);
	sizerOfPositionFilePath->Add(new wxStaticText(GetStaticBox(), wxID_ANY, Strings::LabelOfDrillPositionFilePath()), wxSizerFlags().CenterVertical());
	ctrlDrillPositionFilePath_ = new wxTextCtrl(GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, GetStaticBox()->FromDIP(wxSize(200, -1)), wxTE_READONLY);
	sizerOfPositionFilePath->Add(ctrlDrillPositionFilePath_, wxSizerFlags(1).Border(wxLEFT, 4).CenterVertical());
	wxButton* btnOpenFile = new wxButton(GetStaticBox(), wxID_ANY, Strings::LabelOfOpen());
	btnOpenFile->Bind(wxEVT_BUTTON, &SizerOfSimpleDrillLogFiles::OnOpenDrillPositionFile, this);
	sizerOfPositionFilePath->Add(btnOpenFile, wxSizerFlags().Border(wxLEFT, 4).CenterVertical());
	listOfDrills_ = new wxDataViewListCtrl(GetStaticBox(), wxID_ANY, wxDefaultPosition, GetStaticBox()->FromDIP(wxSize(-1, 190)));
	listOfDrills_->AppendTextColumn(Strings::LabelOfNo(), wxDATAVIEW_CELL_INERT, 30);
	listOfDrills_->AppendTextColumn(Strings::LabelOfDrillNo(), wxDATAVIEW_CELL_INERT, 40);
	listOfDrills_->AppendTextColumn(wxS("X"), wxDATAVIEW_CELL_INERT, 80);
	listOfDrills_->AppendTextColumn(wxS("Y"), wxDATAVIEW_CELL_INERT, 80);
	listOfDrills_->AppendTextColumn(wxS("Z"), wxDATAVIEW_CELL_INERT, 80);
	listOfDrills_->AppendTextColumn(Strings::LabelOfDrillDepth(), wxDATAVIEW_CELL_INERT, 40);

	// drill log
	wxBoxSizer* sizerDirllLog = new wxBoxSizer(wxHORIZONTAL);
	sizerDirllLog->Add(new wxStaticText(GetStaticBox(), wxID_ANY, Strings::LabelOfDrillLog()), wxSizerFlags().CenterVertical());
	wxButton* btnAppendFile = new wxButton(GetStaticBox(), wxID_ANY, Strings::LabelOfAppend());
	btnAppendFile->Bind(wxEVT_BUTTON, &SizerOfSimpleDrillLogFiles::OnAppendDrillLogFiles, this);
	wxButton* btnClearFiles = new wxButton(GetStaticBox(), wxID_ANY, Strings::LabelOfClear());
	btnClearFiles->Bind(wxEVT_BUTTON, &SizerOfSimpleDrillLogFiles::OnClearDrillLogFiles, this);
	sizerDirllLog->Add(btnAppendFile, wxSizerFlags().Border(wxLEFT, 4).CenterVertical());
	sizerDirllLog->Add(btnClearFiles, wxSizerFlags().Border(wxLEFT, 4).CenterVertical());
	wxBoxSizer* sizerOfDrillLogData = new wxBoxSizer(wxHORIZONTAL);
	listOfLogFiles_ = new wxDataViewListCtrl(GetStaticBox(), wxID_ANY, wxDefaultPosition, GetStaticBox()->FromDIP(wxSize(260, 200)));
	listOfLogFields_ = new wxDataViewListCtrl(GetStaticBox(), wxID_ANY, wxDefaultPosition, GetStaticBox()->FromDIP(wxSize(170, 200)));
	listOfLogFiles_->AppendTextColumn(Strings::LabelOfNo(), wxDATAVIEW_CELL_INERT, 30);
	listOfLogFiles_->AppendTextColumn(Strings::LabelOfDrillNo(), wxDATAVIEW_CELL_INERT, 50);
	listOfLogFiles_->AppendTextColumn(Strings::LabelOfFilePath(), wxDATAVIEW_CELL_INERT, 180);
	listOfLogFields_->AppendTextColumn(Strings::LabelOfNo(), wxDATAVIEW_CELL_INERT, 30);
	listOfLogFields_->AppendTextColumn(Strings::LabelOfField(), wxDATAVIEW_CELL_INERT, 70);
	wxArrayString choices;
	choices.Add(wxString::FromUTF8(geo3dml::Field::ValueTypeToName(geo3dml::Field::ValueType::Integer)));
	choices.Add(wxString::FromUTF8(geo3dml::Field::ValueTypeToName(geo3dml::Field::ValueType::Double)));
	choices.Add(wxString::FromUTF8(geo3dml::Field::ValueTypeToName(geo3dml::Field::ValueType::Text)));
	choices.Add(wxString::FromUTF8(geo3dml::Field::ValueTypeToName(geo3dml::Field::ValueType::Boolean)));
	wxDataViewChoiceRenderer* choiceRenderer = new wxDataViewChoiceRenderer(choices);
	wxDataViewColumn* typeColumn = new wxDataViewColumn(Strings::LabelOfFieldValueType(), choiceRenderer, 2, 70);
	listOfLogFields_->AppendColumn(typeColumn);
	sizerOfDrillLogData->Add(listOfLogFiles_, wxSizerFlags(1).Expand());
	sizerOfDrillLogData->Add(listOfLogFields_, wxSizerFlags().Expand().Border(wxLEFT, 4));

	Add(sizerOfPositionFilePath, wxSizerFlags().Expand().Border(wxALL, 4));
	Add(listOfDrills_, wxSizerFlags().Expand().Border(wxLEFT | wxRIGHT | wxBOTTOM, 4));
	Add(sizerDirllLog, wxSizerFlags().Border(wxLEFT | wxRIGHT | wxBOTTOM, 4));
	Add(sizerOfDrillLogData, wxSizerFlags(1).Expand().Border(wxLEFT | wxRIGHT | wxBOTTOM, 4));
}

SizerOfSimpleDrillLogFiles::~SizerOfSimpleDrillLogFiles() {

}

void SizerOfSimpleDrillLogFiles::OnOpenDrillPositionFile(wxCommandEvent& event) {
	wxString filePath = wxFileSelector(Strings::TipOfOpenDrillPositionFile(), wxEmptyString, wxEmptyString, wxEmptyString, Strings::WildcardOfDrillPositionFile());
	if (filePath.IsEmpty())
		return;
	wxTextFile drillPosFile(filePath);
	if (!drillPosFile.Open()) {
		wxMessageBox(Strings::TipOfOpenFileFailed(filePath), Strings::AppName(), wxICON_ERROR);
		return;
	}
	wxBusyCursor waiting;
	ctrlDrillPositionFilePath_->SetValue(filePath);
	listOfDrills_->DeleteAllItems();
	wxVector<wxVariant> itemValue;
	wxStringTokenizer tokenizer;
	wxString line = drillPosFile.GetFirstLine();	// drop the first line
	while (!drillPosFile.Eof()) {
		line = drillPosFile.GetNextLine();
		if (line.IsEmpty()) {
			continue;
		}
		tokenizer.SetString(line);
		if (tokenizer.CountTokens() < 5) {
			continue;
		}
		itemValue.push_back(wxString::Format(wxS("%d"), listOfDrills_->GetItemCount() + 1));
		for (int i = 0; i < 5; ++i) {
			itemValue.push_back(tokenizer.GetNextToken());
		}
		listOfDrills_->AppendItem(itemValue);
		itemValue.clear();
	}
}

void SizerOfSimpleDrillLogFiles::OnAppendDrillLogFiles(wxCommandEvent& event) {
	wxFileDialog dlg(nullptr, Strings::TipOfOpenDrillLogFile(), wxEmptyString, wxEmptyString, Strings::WildcardOfDrillLogFile(), wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE);
	if (dlg.ShowModal() != wxID_OK) {
		return;
	}
	wxBusyCursor waiting;
	wxVector<wxVariant> itemValue;
	wxArrayString filePaths;
	dlg.GetPaths(filePaths);
	for (int i = 0; i < filePaths.size(); ++i) {
		wxTextFile drillFile(filePaths[i]);
		if (!drillFile.Open()) {
			wxLogError(Strings::TipOfOpenFileFailed(filePaths[i]));
			continue;
		}
		wxString strPath, strName, strExt;
		wxFileName::SplitPath(filePaths[i], &strPath, &strName, &strExt);
		itemValue.push_back(wxString::Format(wxS("%d"), listOfLogFiles_->GetItemCount() + 1));
		itemValue.push_back(strName);
		itemValue.push_back(filePaths[i]);
		listOfLogFiles_->AppendItem(itemValue);
		itemValue.clear();
		wxString line = drillFile.GetFirstLine();
		wxStringTokenizer tokenizer(line);
		wxString fieldName = tokenizer.GetNextToken();
		while (tokenizer.HasMoreTokens()) {
			fieldName = tokenizer.GetNextToken();
			if (fieldSet_.find(fieldName) == fieldSet_.end()) {
				fieldSet_.insert(fieldName);
				itemValue.push_back(wxString::Format(wxS("%d"), listOfLogFields_->GetItemCount() + 1));
				itemValue.push_back(fieldName);
				itemValue.push_back(wxString::FromUTF8(geo3dml::Field::ValueTypeToName(geo3dml::Field::ValueType::Integer)));
				listOfLogFields_->AppendItem(itemValue);
				itemValue.clear();
			}
		}
	}
}

void SizerOfSimpleDrillLogFiles::OnClearDrillLogFiles(wxCommandEvent& event) {
	int confirm = wxMessageBox(Strings::ConfirmToEmptyFileList(), Strings::AppName(), wxYES_NO | wxICON_EXCLAMATION);
	if (confirm != wxYES) {
		return;
	}
	wxBusyCursor waiting;
	listOfLogFields_->DeleteAllItems();
	listOfLogFiles_->DeleteAllItems();
	fieldSet_.clear();
}

wxTextCtrl* SizerOfSimpleDrillLogFiles::GetDrillPositionFilePathCtrl() const {
	return ctrlDrillPositionFilePath_;
}

wxString SizerOfSimpleDrillLogFiles::GetDrillPositionFilePath() const {
	return ctrlDrillPositionFilePath_->GetValue();
}

int SizerOfSimpleDrillLogFiles::CountDrillPositins() const {
	return listOfDrills_->GetItemCount();
}

DrillPositionMap SizerOfSimpleDrillLogFiles::GetDrillPositions() const {
	DrillPositionMap drillPositions;
	for (int i = 0; i < listOfDrills_->GetItemCount(); ++i) {
		wxString drillNo = listOfDrills_->GetTextValue(i, 1);
		double x = 0, y = 0, z = 0;
		listOfDrills_->GetTextValue(i, 2).ToDouble(&x);
		listOfDrills_->GetTextValue(i, 3).ToDouble(&y);
		listOfDrills_->GetTextValue(i, 4).ToDouble(&z);
		drillPositions[drillNo] = geo3dml::Point3D(x, y, z);
	}
	return drillPositions;
}

wxDataViewListCtrl* SizerOfSimpleDrillLogFiles::GetDrillLogListCtrl() const {
	return listOfLogFiles_;
}

int SizerOfSimpleDrillLogFiles::CountDrillLogFiles() const {
	return listOfLogFiles_->GetItemCount();
}

DrillLogFileMap SizerOfSimpleDrillLogFiles::GetDrillLogFiles() const {
	DrillLogFileMap drillFiles;
	for (int i = 0; i < listOfLogFiles_->GetItemCount(); ++i) {
		wxString drillNo = listOfLogFiles_->GetTextValue(i, 1);
		wxString filePath = listOfLogFiles_->GetTextValue(i, 2);
		drillFiles[drillNo] = filePath;
	}
	return drillFiles;
}

int SizerOfSimpleDrillLogFiles::CountDrillLogFields() const {
	return listOfLogFields_->GetItemCount();
}

DrillLogFieldMap SizerOfSimpleDrillLogFiles::GetDrillLogFields() const {
	DrillLogFieldMap drillFields;
	for (int i = 0; i < listOfLogFields_->GetItemCount(); ++i) {
		std::string name = listOfLogFields_->GetTextValue(i, 1).ToUTF8().data();
		std::string typeName = listOfLogFields_->GetTextValue(i, 2).ToUTF8().data();
		geo3dml::Field field;
		field.Name(name).Label(name).DataType(geo3dml::Field::NameToValueType(typeName));
		drillFields[name] = field;
	}
	return drillFields;
}
