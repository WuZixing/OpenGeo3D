// UTF-8编码
#include "AppFrame.h"
#include <QtCore/QDir>
#include <QtGui/QCloseEvent>
#include <QtWidgets/QColorDialog>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QVBoxLayout>
#include <g3dvtk/ObjectFactory.h>
#include <g3dxml/XMLReader.h>
#include <g3dxml/XMLWriter.h>
#include "icon.xpm"
#include "BusyCursor.h"
#include "Events.h"
#include "DataLoaderGeoJSON.h"
#include "DlgAbout.h"
#include "DlgOpenSimpleDrillLog.h"
#include "ProjectPanel.h"
#include "RenderWidget.h"
#include "Text.h"

AppFrame::AppFrame(QWidget* parent) : QMainWindow(parent) {
	setWindowIcon(QIcon(QPixmap(xpm_icon)));
	setupWidgets();
	setupMenu();
}

AppFrame::~AppFrame() {

}

void AppFrame::about() {
	DlgAbout about(this);
	about.exec();
}

void AppFrame::setupMenu() {
	QMenu* menu = menuBar()->addMenu(Text::menuFile());
	QMenu* subMenu = menu->addMenu(Text::menuStructureModel());
	subMenu->addAction(Text::menuOpenGeo3DML(), this, &AppFrame::openGeo3DML);
	subMenu->addAction(Text::menuOpenDrillLog(), this, &AppFrame::openDrillLog);
	subMenu->addAction(Text::menuOpenGeoJSON(), this, &AppFrame::openGeoJSON);
	subMenu->addSeparator();
	subMenu->addAction(Text::menuSaveToGeo3DML(), this, &AppFrame::saveToGeo3DML);
	subMenu->addAction(Text::menuCloseStructureModel(), this, &AppFrame::closeStructureModel);
	menu->addSeparator();
	menu->addAction(Text::menuCloseAllModels(), this, &AppFrame::closeAllModels);
	menu->addAction(Text::menuQuit(), this, &AppFrame::quit);

	menu = menuBar()->addMenu(Text::menuWindow());
	menu->addAction(Text::menuFullView(), QKeySequence(Qt::Modifier::CTRL | Qt::Key::Key_R), this, &AppFrame::fullView);
	menu->addAction(Text::menuBackgroundColor(), QKeySequence(Qt::Modifier::CTRL | Qt::Key::Key_B), this, &AppFrame::changeBackgroundColor);
	menu->addSeparator();
	menu->addAction(Text::menuScaleZUp(), QKeySequence(Qt::Modifier::CTRL | Qt::Key::Key_Up), this, &AppFrame::scaleZUp);
	menu->addAction(Text::menuScaleZDown(), QKeySequence(Qt::Modifier::CTRL | Qt::Key::Key_Down), this, &AppFrame::scaleZDown);
	menu->addAction(Text::menuCustomizedZScale(), this, &AppFrame::customizedZScale);
	menu->addAction(Text::menuResetZScale(), this, &AppFrame::resetZScale);
	menu->addSeparator();
	menuProjectPanel_ = menu->addAction(Text::menuProjectPanel(), this, &AppFrame::toggleProjectPanel);
	menuProjectPanel_->setCheckable(true);
	connect(menu, &QMenu::aboutToShow, this, &AppFrame::windowMenuAboutToShow);

	menu = menuBar()->addMenu(Text::menuHelp());
	menu->addAction(Text::menuAbout(), this, &AppFrame::about);

}

void AppFrame::setupWidgets() {
	dockWidget_ = new QDockWidget(Text::titleOfProjectPanel(), this);
	projectPanel_ = new ProjectPanel(dockWidget_);
	dockWidget_->setWidget(projectPanel_);
	addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, dockWidget_);
	renderWidget_ = new RenderWidget(this, projectPanel_->getRenderer());
	setCentralWidget(renderWidget_);
}

void AppFrame::closeEvent(QCloseEvent* event) {
	QMessageBox::StandardButton btn = QMessageBox::warning(this, Text::titleOfConfirmingQuit(), Text::confirmToQuit(), 
		QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No, QMessageBox::StandardButton::No);
	if (btn != QMessageBox::StandardButton::Yes) {
		event->ignore();
	} else {
		QMainWindow::closeEvent(event);
	}
}

void AppFrame::openGeo3DML() {
	QString filePath = selectAFile(Text::menuOpenGeo3DML(), Text::filterOfGeo3DMLFile());
	if (filePath.isEmpty()) {
		return;
	}

	geo3dml::Object* g3dObject = nullptr;
	g3dvtk::ObjectFactory g3dVtkFactory;
	g3dxml::XMLReader xmlReader(&g3dVtkFactory);
	BusyCursor::beginWaiting();
	g3dObject = xmlReader.LoadXMLFile(filePath.toUtf8().constData());
	BusyCursor::endWaiting();
	if (g3dObject == nullptr) {
		QMessageBox::critical(this, this->windowTitle(), QString::fromUtf8(xmlReader.Error().c_str()));
	} else {
		BusyCursor waiting;
		geo3dml::Model* model = dynamic_cast<geo3dml::Model*>(g3dObject);
		if (model != nullptr) {
			projectPanel_->appendG3DModel(model, true);
		} else {
			geo3dml::Project* project = dynamic_cast<geo3dml::Project*>(g3dObject);
			if (project != nullptr) {
				project->BindFeatureClassesToLayers(&g3dVtkFactory);
				int numOfMaps = project->GetMapCount();
				while (project->GetModelCount() > 0) {
					geo3dml::Model* model = project->RemoveModelAt(0);
					projectPanel_->appendG3DModel(model, numOfMaps == 0);
				}
				while (project->GetMapCount() > 0) {
					geo3dml::Map* map = project->RemoveMapAt(0);
					projectPanel_->appendG3DMap(map);
				}
				// 工程的基本信息
				geo3dml::Project* innerProject = projectPanel_->getG3DProject();
				innerProject->SetName(project->GetName());
				innerProject->SetMetadata(project->GetMetadata());
			}
			delete g3dObject;
		}
		projectPanel_->expandStructureModelTree();
		Events::PostEvent(Events::Type::ResetAndUpdateScene, this);
	}
}

void AppFrame::openDrillLog() {
	DlgOpenSimpleDrillLog dlg(this);
	if (dlg.exec() != DlgOpenSimpleDrillLog::DialogCode::Accepted) {
		return;
	}
	BusyCursor waiting;
	geo3dml::Model* g3dModel = dlg.loadAsG3DModel();
	if (g3dModel != nullptr) {
		projectPanel_->appendG3DModel(g3dModel, true);
		projectPanel_->expandStructureModelTree();
		Events::PostEvent(Events::Type::ResetAndUpdateScene, this);
	}
}

void AppFrame::openGeoJSON() {
	opengeo3d::DataLoaderGeoJSON loader;
	BusyCursor waiting;
	geo3dml::Model* g3dModel = loader.loadAsG3DModel();
	if (g3dModel != nullptr) {
		projectPanel_->appendG3DModel(g3dModel, true);
		projectPanel_->expandStructureModelTree();
		Events::PostEvent(Events::Type::ResetAndUpdateScene, this);
	}
}

void AppFrame::closeStructureModel() {
	QMessageBox::StandardButton btn = QMessageBox::warning(this, Text::menuCloseStructureModel(), Text::confirmToCloseStructureModel(), 
		QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No, QMessageBox::StandardButton::No);
	if (btn != QMessageBox::StandardButton::Yes) {
		return;
	}
	BusyCursor waiting;
	projectPanel_->closeStructureModel();
	Events::PostEvent(Events::Type::ResetAndUpdateScene, this);
}

void AppFrame::closeAllModels() {
	QMessageBox::StandardButton btn = QMessageBox::warning(this, QString(), Text::confirmToCloseAllModels(),
		QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No, QMessageBox::StandardButton::No);
	if (btn != QMessageBox::StandardButton::Yes) {
		return;
	}
	BusyCursor waiting;
	projectPanel_->closeAllModels();
	Events::PostEvent(Events::Type::ResetAndUpdateScene, this);
}

void AppFrame::quit() {
	close();
}

bool AppFrame::event(QEvent* event) {
	switch ((int)(event->type())) {
	case Events::Type::UpdateScene: {
		BusyCursor waiting;
		renderWidget_->render();
		return true;
	}
	case Events::Type::ResetAndUpdateScene: {
		BusyCursor waiting;
		renderWidget_->resetAndRender();
		return true;
	}
	case Events::Type::Menu_OpenGeo3DML: {
		openGeo3DML();
		return true;
	}
	case Events::Type::Menu_OpenDrillLog: {
		openDrillLog();
		return true;
	}
	case Events::Type::Menu_SaveToGeo3DML: {
		saveToGeo3DML();
		return true;
	}
	case Events::Type::Menu_CloseStructureModel: {
		closeStructureModel();
		return true;
	}
	default:
		break;
	}
	return QMainWindow::event(event);
}

void AppFrame::fullView() {
	Events::PostEvent(Events::Type::ResetAndUpdateScene, this);
}

void AppFrame::changeBackgroundColor() {
	vtkRenderer* render = projectPanel_->getRenderer();
	if (render == nullptr) {
		return;
	}
	double r = 0, g = 0, b = 0;
	render->GetBackground(r, g, b);
	QColor color = QColorDialog::getColor(QColor::fromRgbF(r, g, b), this);
	if (color.isValid()) {
		render->SetBackground(color.redF(), color.greenF(), color.blueF());
		Events::PostEvent(Events::Type::UpdateScene, this);
	}
}

void AppFrame::scaleZUp() {
	vtkTransform* t = projectPanel_->getTransform();
	double scales[3] = { 1.0 };
	t->GetScale(scales);
	scales[2] = scales[2] * 1.1;
	t->Identity();
	t->Scale(scales);
	Events::PostEvent(Events::Type::UpdateScene, this);
}

void AppFrame::scaleZDown() {
	vtkTransform* t = projectPanel_->getTransform();
	double scales[3] = { 1.0 };
	t->GetScale(scales);
	scales[2] = scales[2] / 1.1;
	t->Identity();
	t->Scale(scales);
	Events::PostEvent(Events::Type::UpdateScene, this);
}

void AppFrame::customizedZScale() {
	vtkTransform* t = projectPanel_->getTransform();
	double scales[3] = { 1.0 };
	t->GetScale(scales);
	bool status = false;
	double zScale = QInputDialog::getDouble(this, Text::menuCustomizedZScale(), Text::tipOfCustomizedZScale(),
		scales[2], 0.01, 1000, 2, &status, Qt::WindowCloseButtonHint);
	if (status) {
		scales[2] = zScale;
		t->Identity();
		t->Scale(scales);
		Events::PostEvent(Events::Type::UpdateScene, this);
	}
}

void AppFrame::resetZScale() {
	vtkTransform* t = projectPanel_->getTransform();
	t->Identity();
	Events::PostEvent(Events::Type::UpdateScene, this);
}

void AppFrame::toggleProjectPanel() {
	dockWidget_->setVisible(!dockWidget_->isVisible());
}

void AppFrame::windowMenuAboutToShow() {
	menuProjectPanel_->setChecked(dockWidget_->isVisible());
}

QString AppFrame::selectAFile(const QString& dialogCaption, const QString& fileFilters) {
	QString filePath = QFileDialog::getOpenFileName(this, dialogCaption, QDir::currentPath(), fileFilters);
	if (!filePath.isEmpty()) {
		QDir dir(filePath);
		dir.cdUp();
		QDir::setCurrent(dir.path());
	}
	return filePath;
}

void AppFrame::saveToGeo3DML() {
	geo3dml::Project* g3dProject = projectPanel_->getG3DProject();
	QString projName = QString::fromUtf8(g3dProject->GetName().c_str());
	QString selectedFilter;
	QFileInfo fileInfo(QDir::currentPath(), projName);
	QString filePath = QFileDialog::getSaveFileName(this, Text::menuSaveToGeo3DML(), fileInfo.absoluteFilePath(), Text::filterOfGeo3DMLFileWithVersion(), &selectedFilter);
	if (filePath.isEmpty()) {
		return;
	}
	BusyCursor::beginWaiting();
	fileInfo.setFile(filePath);
	QDir::setCurrent(fileInfo.absolutePath());
	g3dProject->SetName(fileInfo.baseName().toUtf8().constData());
	g3dxml::XMLWriter projectWriter;
	bool isOK = projectWriter.Write(g3dProject, filePath.toUtf8().constData(), 
		selectedFilter.contains(QStringLiteral("v1.0")) ? g3dxml::SchemaVersion::Schema_1_0 : g3dxml::SchemaVersion::Schema_2_0);
	BusyCursor::endWaiting();
	if (isOK) {
		QMessageBox::information(this, QString(), Text::tipOfSucceedInSavingToGeo3DMLFile(projName));
	} else {
		QMessageBox::critical(this, QString(), Text::tipOfErrorInSavingToGeo3DMLFile(projName, QString::fromUtf8(projectWriter.Error().c_str())));
	}
}
