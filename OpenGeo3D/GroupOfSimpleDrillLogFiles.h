#pragma once

#include <map>
#include <QtCore/QSet>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLineEdit>
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

private slots:
	void openDrillPosition();
	void appendLogs();
	void clearLogs();

private:
	QLineEdit* positionFilePath_;
	QTableWidget *drillList_, *logFileList_, *logFieldList_;
	QSet<QString> fieldSet_;
};
