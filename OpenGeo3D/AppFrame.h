#pragma once

#include <QMainWindow>
#include "ProjectPanel.h"

class AppFrame : public QMainWindow {
	Q_OBJECT

public:
	AppFrame(QWidget* parent = nullptr);
	virtual ~AppFrame();

private slots:
	void openGeo3DML();
	void openDrillLog();
	void about();
	void quit();

protected:
    virtual void closeEvent(QCloseEvent *event) override;

private:
	void setupMenu();
	void setupWidgets();

private:
	ProjectPanel* projectPanel_;
};
