#pragma once

#include <QtWidgets/QDialog>

class DlgAbout : public QDialog {
	Q_OBJECT

public:
	DlgAbout(QWidget* parent = nullptr);
	virtual ~DlgAbout();

private:
	void initUI();
};
