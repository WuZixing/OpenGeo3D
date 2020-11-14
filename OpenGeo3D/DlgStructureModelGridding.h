#pragma once

#include "wxWidgets.h"
#include <geo3dml/FeatureClass.h>
#include <g3dgrid/VoxelGrid.h>
#include "GridLODViewListCtrl.h"

class DlgStructureModelGridding : public wxDialog {
public:
	DlgStructureModelGridding(wxWindow* parent);
	virtual ~DlgStructureModelGridding();

	void AppendFeatureClassFromStructureModel(geo3dml::FeatureClass* fc);
	void SetLocalGridModel(g3dgrid::VoxelGrid* voxelGrid);

private:
	void OnLocalGrid(wxCommandEvent& event);
	void OnRemoteGrid(wxCommandEvent& event);
	void OnButtonOK(wxCommandEvent& event);
	void OnCheckGridingRange(wxCommandEvent& event);

private:
	void UpdateGriddingRangeByStructureModel();

private:
	wxDataViewListCtrl* featureClassViewList_;
	wxCheckBox* ctrlGriddingRangeBySources_;
	wxTextCtrl *ctrlMinX_, *ctrlMinY_, *ctrlMinZ_, *ctrlMaxX_, *ctrlMaxY_, *ctrlMaxZ_;
	wxTextCtrl *ctrlGridId_, *ctrlGridName_, *ctrlGridSRS_, *ctrlGridDescription_, *ctrlGridOrigin_;
	wxRadioButton *ctrlLocalGrid_, *ctrlRemoteGrid_;
	GridLODViewListCtrl* gridLODList_;
	g3dgrid::VoxelGrid* voxelGrid_;
};