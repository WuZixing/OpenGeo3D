#pragma once

#include <QMainWindow>

class AppFrame : public QMainWindow
{
	Q_OBJECT

public:
	AppFrame(QWidget* parent = nullptr);
	virtual ~AppFrame();
};
