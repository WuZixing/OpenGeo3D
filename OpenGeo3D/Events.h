#pragma once

#include "wxWidgets.h"

class Events {
public:
	enum ID {
		Window_Frame = wxID_HIGHEST + 1,

		Menu_OpenGeo3DML,
		Menu_OpenSimpleDrillLog,
		Menu_OpenSGeMSGrid,
		Menu_FullView,
		Menu_BackgroundColor,
		Menu_ScaleUpZ,
		Menu_ScaleDownZ,
		Menu_CustomizedZScale,
		Menu_ResetZScale,
		Menu_ProjectPanel,
		Menu_LogWindow,
		Menu_EditVoxelGrid,
		Menu_DeleteGridLOD,
		Menu_AppendGridLOD,
		Menu_CloseAllModels,
		Menu_CloseStructureModel,
		Menu_CloseVoxelGridModel,
		Menu_SaveToGeo3DML,
		Menu_SaveToVoxelGrid,
		Menu_StructureModelGridding,
		Menu_StopStructureModelGridding,

		// general notifications
		Notify_ResetAndRefreshRenderWindow,
		Notify_RefreshRenderWindow,
		// project panel notifications
		Notify_ProjectTreeItemSelected
	};

public:
	static void Notify(ID id);
};
