#pragma once

#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QTableWidget>

class GroupOfSimpleDrillLogFiles : public QGroupBox {
	Q_OBJECT

public:
	GroupOfSimpleDrillLogFiles(QWidget* parent = nullptr);
	virtual ~GroupOfSimpleDrillLogFiles();

private slots:
	void openDrillPosition();
	void appendLogs();
	void clearLogs();

private:
	QLineEdit* positionFilePath_;
	QTableWidget *drillList_, *logFileList_, *logFieldList_;
};
