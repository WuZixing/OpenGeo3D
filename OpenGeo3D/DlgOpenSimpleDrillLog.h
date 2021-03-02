#pragma once

#include <QtWidgets/QDialog>

class DlgOpenSimpleDrillLog : public QDialog {
	Q_OBJECT

public:
	DlgOpenSimpleDrillLog(QWidget* parent = nullptr);
	virtual ~DlgOpenSimpleDrillLog();

private:
	void initUI();
};
