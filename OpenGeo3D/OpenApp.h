#pragma once

#include <QtWidgets/QApplication>
#include "AppFrame.h"

class OpenApp : public QApplication {
	Q_OBJECT

public:
    OpenApp(int &argc, char **argv);
	virtual ~OpenApp();

	void showWindow();

protected:
    virtual bool event(QEvent *event) override;

private:
	AppFrame appFrame_;
};
