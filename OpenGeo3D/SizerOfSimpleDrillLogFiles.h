#pragma once

#include "wxWidgets.h"
#include <map>
#include <set>
#include <wx/dataview.h>
#include <geo3dml/Field.h>
#include <geo3dml/Point3D.h>

using DrillPositionMap = std::map<wxString, geo3dml::Point3D>;	///< drill No => point
using DrillLogFileMap = std::map<wxString, wxString>;			///< drill No => file path
using DrillLogFieldMap = std::map<std::string, geo3dml::Field>;	///< drill No => field

class SizerOfSimpleDrillLogFiles : public wxStaticBoxSizer {
public:
	SizerOfSimpleDrillLogFiles(wxWindow* parent, const wxString& label = wxEmptyString);
	virtual ~SizerOfSimpleDrillLogFiles();

	wxTextCtrl* GetDrillPositionFilePathCtrl() const;
	wxString GetDrillPositionFilePath() const;
	int CountDrillPositins() const;
	DrillPositionMap GetDrillPositions() const;

	wxDataViewListCtrl* GetDrillLogListCtrl() const;
	int CountDrillLogFiles() const;
	DrillLogFileMap GetDrillLogFiles() const;

	int CountDrillLogFields() const;
	DrillLogFieldMap GetDrillLogFields() const;

private:
	void OnOpenDrillPositionFile(wxCommandEvent& event);
	void OnAppendDrillLogFiles(wxCommandEvent& event);
	void OnClearDrillLogFiles(wxCommandEvent& event);

private:
	wxTextCtrl *ctrlDrillPositionFilePath_;
	wxDataViewListCtrl *listOfDrills_, *listOfLogFiles_, *listOfLogFields_;

	using FieldSet = std::set<wxString>;
	FieldSet fieldSet_;
};
