#pragma once

#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include "GroupOfSimpleDrillLogFiles.h"

class DlgOpenSimpleDrillLog : public QDialog {
	Q_OBJECT

public:
	DlgOpenSimpleDrillLog(QWidget* parent = nullptr);
	virtual ~DlgOpenSimpleDrillLog();

	QString getDataFilePath() const;
	bool getGridDim(int& dimX, int& dimY, int& dimZ) const;
	bool getGridStep(double& stepX, double& stepY, double& stepZ) const;
	bool getNoDataValue(int& value) const;

public slots:
	virtual void accept() override;
	void openDataFile();

private:
	void initUI();


private:
	GroupOfSimpleDrillLogFiles* group_;
	QLineEdit* dataFilePath_, * editNoData_;
	QLineEdit* editDimX_, * editDimY_, * editDimZ_;
	QLineEdit* editStepX_, * editStepY_, * editStepZ_;
};
