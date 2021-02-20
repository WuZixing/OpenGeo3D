#pragma once

#include <QtWidgets/QTabWidget>
#include "MetadataPage.h"

class ProjectMetaBook : public QTabWidget {
	Q_OBJECT

public:
	ProjectMetaBook(QWidget* parent);
	virtual ~ProjectMetaBook();

	void setCurrentItem(QTreeWidgetItem* item);

private:
	MetadataPage* metadataPage_;
};
