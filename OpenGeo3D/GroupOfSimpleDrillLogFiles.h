#pragma once

#include <map>
#include <QtCore/QSet>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableWidget>
#include <geo3dml/Field.h>
#include <geo3dml/Point3D.h>

using DrillPositionMap = std::map<QString, geo3dml::Point3D>;	// drill No => point
using DrillLogFileMap = std::map<QString, QString>;				// drill No => file path
using DrillLogFieldMap = std::map<std::string, geo3dml::Field>;		// field name => field

class GroupOfSimpleDrillLogFiles : public QGroupBox {
	Q_OBJECT

public:
	GroupOfSimpleDrillLogFiles(QWidget* parent = nullptr);
	virtual ~GroupOfSimpleDrillLogFiles();

	bool validate();

	DrillPositionMap getDrillPositions() const;
	DrillLogFileMap getDrillLogFiles() const;
	DrillLogFieldMap getDrillLogFields() const;

	bool isSavingPositionToSHPEnabled() const;
	bool savePositionToSHP() const;

private slots:
	void openDrillPosition();
	void appendLogs();
	void clearLogs();
	void shpFileChecked(int state);
	void saveDrillPositionToFile();

private:
	QLineEdit* positionFilePath_, *shpFilePath_;
	QTableWidget *drillList_, *logFileList_, *logFieldList_;
	QSet<QString> fieldSet_;
	QPushButton* btnSavePositionToShp_;
};
