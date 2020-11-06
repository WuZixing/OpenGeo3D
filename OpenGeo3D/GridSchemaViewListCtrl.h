#pragma once

#include "wxWidgets.h"
#include <wx/dataview.h>

class GridSchemaViewListCtrl : public wxDataViewListCtrl {
public:
	GridSchemaViewListCtrl(wxWindow* parent);
	virtual ~GridSchemaViewListCtrl();
	
};