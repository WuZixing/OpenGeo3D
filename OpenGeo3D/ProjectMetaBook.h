#pragma once

#include <QtWidgets/QTabWidget>
#include "MetadataPage.h"
#include "RenderOptionPage.h"

class ProjectMetaBook : public QTabWidget {
	Q_OBJECT

public:
	ProjectMetaBook(QWidget* parent);
	virtual ~ProjectMetaBook();

	void setCurrentItem(QTreeWidgetItem* item);

private:
	MetadataPage* metadataPage_;
	RenderOptionPage* renderOptionPage_;
};
