#pragma once

#include <QtWidgets/QDialog>

namespace Ui {
	class DlgAbout;
}

class DlgAbout : public QDialog {
	Q_OBJECT

public:
	DlgAbout(QWidget* parent = nullptr);
	virtual ~DlgAbout();

private:
	Ui::DlgAbout* ui_;
};
