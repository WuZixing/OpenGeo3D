#pragma once

#include <QtWidgets/QDialog>
#include <geo3dml/Model.h>
#include "GroupOfSimpleDrillLogFiles.h"

class DlgOpenSimpleDrillLog : public QDialog {
	Q_OBJECT

public:
	DlgOpenSimpleDrillLog(QWidget* parent = nullptr);
	virtual ~DlgOpenSimpleDrillLog();

	geo3dml::Model* loadAsG3DModel() const;

public slots:
	virtual void accept() override;

private:
	void initUI();


private:
	GroupOfSimpleDrillLogFiles* group_;
};
