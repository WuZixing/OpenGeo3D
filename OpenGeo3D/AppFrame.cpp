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
#include <QStandardPaths>
#include <vtkXMLImageDataWriter.h>
#include <g3dvtk/ObjectFactory.h>
#include <g3dvtk/TIN.h>
#include <g3dvtk/UniformGrid.h>
#include <g3dxml/XMLReader.h>
#include <g3dxml/XMLWriter.h>
#include "icon.xpm"
#include "BusyCursor.h"
#include "Events.h"
#include "DlgAbout.h"
#include "DlgOpenSimpleDrillLog.h"
#include "ProjectPanel.h"
#include "RenderWidget.h"
#include "Text.h"
#include <vtkDiscreteMarchingCubes.h>
#include <vtkCellData.h>
#include <vtkPointData.h>
#include <vtkWindowedSincPolyDataFilter.h>
#include <set>

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
    subMenu->addSeparator();
    subMenu->addAction(Text::menuSaveToGeo3DML(), this, &AppFrame::saveToGeo3DML);
    subMenu->addAction(Text::menuCloseStructureModel(), this, &AppFrame::closeStructureModel);
    menu->addSeparator();
    menu->addAction(Text::menuCloseAllModels(), this, &AppFrame::closeAllModels);
    menu->addAction(Text::menuQuit(), this, &AppFrame::quit);

    menu = menuBar()->addMenu(Text::menuWindow());
    menu->addAction(Text::menuFullView(), this, &AppFrame::fullView, QKeySequence(Qt::Modifier::CTRL | Qt::Key::Key_R));
    menu->addAction(Text::menuBackgroundColor(), this, &AppFrame::changeBackgroundColor, QKeySequence(Qt::Modifier::CTRL | Qt::Key::Key_B));
    menu->addSeparator();
    menu->addAction(Text::menuScaleZUp(), this, &AppFrame::scaleZUp, QKeySequence(Qt::Modifier::CTRL | Qt::Key::Key_Up));
    menu->addAction(Text::menuScaleZDown(), this, &AppFrame::scaleZDown, QKeySequence(Qt::Modifier::CTRL | Qt::Key::Key_Down));
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
    /*
    geo3dml::Model* g3dModel = dlg.loadAsG3DModel();
    if (g3dModel != nullptr) {
        projectPanel_->appendG3DModel(g3dModel, true);
        projectPanel_->expandStructureModelTree();
        Events::PostEvent(Events::Type::ResetAndUpdateScene, this);
    }
    */
    // QString filePath = QFileDialog::getOpenFileName(this, Text::appName(), QStandardPaths::standardLocations(QStandardPaths::HomeLocation).at(0), QStringLiteral("Data Files (*.dat)"));
    // if (filePath.isEmpty()) {
    //     return;
    // }
    // QFile dataFile(filePath);
    QFile dataFile(dlg.getDataFilePath());
    if (!dataFile.open(QIODeviceBase::ReadOnly)) {
        return;
    }
    QByteArray fileData = dataFile.readAll();
    dataFile.close();
    int dimI = 64, dimJ = 64, dimK = 160;
    dlg.getGridDim(dimI, dimJ, dimK);
    double stepI = 0, stepJ = 0, stepK = 0;
    dlg.getGridStep(stepI, stepJ, stepK);
    int noData = -99;
    dlg.getNoDataValue(noData);
    // 构造网格数据，并添加到项目中。
    g3dvtk::ObjectFactory g3dFactory;
    geo3dml::ShapeProperty* cellProp = g3dFactory.NewShapeProperty();
    if (cellProp == nullptr) {
        return;
    }
    geo3dml::Field field;
    field.Name("category").Label("Category").DataType(geo3dml::Field::Integer).NoDataValue(std::to_string(noData));
    cellProp->SetID(geo3dml::Object::NewID());
    cellProp->Name(field.Name());
    cellProp->AddField(field);
    geo3dml::UniformGrid* grid = g3dFactory.NewUniformGrid(0, 0, 0, stepI, stepJ, stepK, dimI, dimJ, dimK);
    g3dvtk::UniformGrid* uniGrid = dynamic_cast<g3dvtk::UniformGrid*>(grid);
    if (uniGrid == nullptr) {
        delete cellProp;
        return;
    }
    uniGrid->SetProperty(cellProp, geo3dml::ShapeProperty::Voxel);
    int valueIndex = 0;
    const int valueSize = sizeof(int);
    const char* data = fileData.constData();
    const qsizetype dataSize = fileData.length();
    for (int i = 0; i < dimI; ++i) {
        for (int j = 0; j < dimJ; ++j) {
            for (int k = dimK - 1; k >= 0; --k) {
                int cellValue = noData;
                const int cellIndex = k * dimJ * dimI + j * dimI + i;
               if (dataSize < valueSize * (valueIndex + 1)) {
                    break;
                }
                memcpy_s(&cellValue, valueSize, &data[valueIndex * valueSize], valueSize);
                ++valueIndex;
                cellProp->IntValue(0, cellIndex, cellValue);
                if (cellValue == noData) {
                    uniGrid->SetCellValidation(i, j, k, false);
                }
            }
        }
    }
    // 要素
    geo3dml::Feature* featureCellGrid = new geo3dml::Feature();
    if (featureCellGrid == nullptr) {
        delete uniGrid;
        return;
    }
    featureCellGrid->AddGeometry(uniGrid);
    featureCellGrid->SetName(QStringLiteral("网格整体").toUtf8().constData()).SetID("Grid-Whole");
    // 基于点集的“网格”：点位于网格的中间
    geo3dml::ShapeProperty* ptProp = g3dFactory.NewShapeProperty();
    if (ptProp == nullptr) {
        delete featureCellGrid;
        return;
    }
    ptProp->AddField(field);
    // 在有效数据点的外围增加一圈无效数据点。NewUniformGrid方法是在输入的dim参数上再加1构造点集。
    geo3dml::UniformGrid* ptGrid = g3dFactory.NewUniformGrid(-stepI / 2, -stepJ / 2, -stepK / 2, stepI, stepJ, stepK, dimI + 1, dimJ + 1, dimK + 1);
    g3dvtk::UniformGrid* uniPtGrid = dynamic_cast<g3dvtk::UniformGrid*>(ptGrid);
    if (uniPtGrid == nullptr) {
        delete ptProp;
        delete featureCellGrid;
        return;
    }
    uniPtGrid->SetProperty(ptProp, geo3dml::ShapeProperty::Vertex);
    std::set<int> attributes;
    vtkUniformGrid* vtkGrid = uniPtGrid->GetUniformGrid();
    valueIndex = 0;
    for (int i = -1; i <= dimI; ++i) {
        for (int j = -1; j <= dimJ; ++j) {
            for (int k = dimK; k >= -1; --k) {
                int ptValue = noData;
                const int ptIndex = (k + 1) * (dimJ + 2) * (dimI + 2) + (j + 1) * (dimI + 2) + (i + 1);
                if (i < 0 || i == dimI || j < 0 || j == dimJ || k < 0 || k == dimK) {
                    // 外增特意增加的无效点。
                    ptProp->IntValue(0, ptIndex, ptValue);
                    vtkGrid->BlankPoint(ptIndex);
                    continue;
                }
                if (dataSize < valueSize * (valueIndex + 1)) {
                    break;
                }
                memcpy_s(&ptValue, valueSize, &data[valueIndex * valueSize], valueSize);
                ++valueIndex;
                ptProp->IntValue(0, ptIndex, ptValue);
                if (ptValue == noData) {
                    vtkGrid->BlankPoint(ptIndex);
                } else {
                    attributes.insert(ptValue);
                }
            }
        }
    }
    geo3dml::Feature* featurePointGrid = new geo3dml::Feature();
    if (featurePointGrid == nullptr) {
        delete uniPtGrid;
        delete featureCellGrid;
        return;
    }
    featurePointGrid->AddGeometry(uniPtGrid);
    featurePointGrid->SetName(QStringLiteral("网格整体-点集").toUtf8().constData()).SetID("Grid-Whole-Point");
    // 要素类：网格
    geo3dml::FeatureClass* featureClass = new geo3dml::FeatureClass();
    if (featureClass == nullptr) {
        delete featureCellGrid;
        delete featurePointGrid;
        return;
    }
    featureClass->SetName(QStringLiteral("网格").toUtf8().constData()).SetID("Grid");
    featureClass->AddFeature(featureCellGrid);
    featureClass->AddFeature(featurePointGrid);
    // 模型
    geo3dml::Model* model = new geo3dml::Model();
    if (model == nullptr) {
        delete featureClass;
        return;
    }
    model->SetName(QStringLiteral("模型").toUtf8().constData());
    model->SetID("Model");
    model->AddFeatureClass(featureClass);
    // 等值面要素和要素类。基于点集网格构造等值面。
    {
        geo3dml::FeatureClass* featureClassISO = new geo3dml::FeatureClass();
        featureClassISO->AddField(field).SetName(QStringLiteral("原始等值面").toUtf8().constData()).SetID("ISO");
        geo3dml::FeatureClass* featureClassSmoothedISOOff = new geo3dml::FeatureClass();
        featureClassSmoothedISOOff->AddField(field).SetName(QStringLiteral("光滑的等值面-边界参数Off").toUtf8().constData()).SetID("Smoothed-ISO-Off");
        geo3dml::FeatureClass* featureClassSmoothedISOOn = new geo3dml::FeatureClass();
        featureClassSmoothedISOOn->AddField(field).SetName(QStringLiteral("光滑的等值面-边界参数On").toUtf8().constData()).SetID("Smoothed-ISO-On");
        vtkPointData* ptData = vtkGrid->GetPointData();
        vtkDataArray* da = ptData->GetArray(field.Name().c_str());
        ptData->SetScalars(da);
        // 追踪等值面
        vtkSmartPointer<vtkDiscreteMarchingCubes> marchingCube = vtkSmartPointer<vtkDiscreteMarchingCubes>::New();
        marchingCube->SetInputData(vtkGrid);
        vtkSmartPointer<vtkWindowedSincPolyDataFilter> smoother = vtkSmartPointer<vtkWindowedSincPolyDataFilter>::New();
        smoother->SetNumberOfIterations(8);
        smoother->BoundarySmoothingOff();
        smoother->FeatureEdgeSmoothingOff();
        smoother->SetFeatureAngle(120);
        smoother->SetEdgeAngle(120);
        smoother->SetPassBand(0.001);
        smoother->NonManifoldSmoothingOn();
        smoother->NormalizeCoordinatesOn();
        smoother->SetInputConnection(marchingCube->GetOutputPort());
        for (auto citor = attributes.cbegin(); citor != attributes.cend(); ++citor) {
            marchingCube->GenerateValues(1, *citor, *citor);
            marchingCube->Update();
            vtkPolyData* polyData = marchingCube->GetOutput();
            if (polyData != nullptr) {
                // 原始等值面
                geo3dml::TIN* tin = g3dFactory.NewTIN();
                if (tin != nullptr) {
                    g3dvtk::TIN* vtkTin = dynamic_cast<g3dvtk::TIN*>(tin);
                    if (vtkTin != nullptr) {
                        vtkTin->GetPolyData()->DeepCopy(polyData);
                    }
                    geo3dml::Feature* featureISO = new geo3dml::Feature();
                    featureISO->AddGeometry(tin).SetField(geo3dml::FieldValue(field.Name(), *citor));
                    featureISO->SetName(QStringLiteral("等值面-%1").arg(*citor).toUtf8().constData()).SetID(QStringLiteral("ISO-%1").arg(*citor).toUtf8().constData());
                    featureClassISO->AddFeature(featureISO);
                }
                // 光滑后的等值面
                smoother->BoundarySmoothingOff();
                smoother->FeatureEdgeSmoothingOff();
                smoother->Update();
                vtkPolyData* smoothedPolyData = smoother->GetOutput();
                if (smoothedPolyData != nullptr) {
                    geo3dml::TIN* tin = g3dFactory.NewTIN();
                    if (tin != nullptr) {
                        g3dvtk::TIN* vtkTin = dynamic_cast<g3dvtk::TIN*>(tin);
                        if (vtkTin != nullptr) {
                            vtkTin->GetPolyData()->DeepCopy(smoothedPolyData);
                        }
                        geo3dml::Feature* featureISO = new geo3dml::Feature();
                        featureISO->AddGeometry(tin).SetField(geo3dml::FieldValue(field.Name(), *citor));
                        featureISO->SetName(QStringLiteral("等值面-%1").arg(*citor).toUtf8().constData()).SetID(QStringLiteral("ISO-%1").arg(*citor).toUtf8().constData());
                        featureClassSmoothedISOOff->AddFeature(featureISO);
                    }
                }
                smoother->BoundarySmoothingOn();
                smoother->FeatureEdgeSmoothingOn();
                smoother->Update();
                smoothedPolyData = smoother->GetOutput();
                if (smoothedPolyData != nullptr) {
                    geo3dml::TIN* tin = g3dFactory.NewTIN();
                    if (tin != nullptr) {
                        g3dvtk::TIN* vtkTin = dynamic_cast<g3dvtk::TIN*>(tin);
                        if (vtkTin != nullptr) {
                            vtkTin->GetPolyData()->DeepCopy(smoothedPolyData);
                        }
                        geo3dml::Feature* featureISO = new geo3dml::Feature();
                        featureISO->AddGeometry(tin).SetField(geo3dml::FieldValue(field.Name(), *citor));
                        featureISO->SetName(QStringLiteral("等值面-%1").arg(*citor).toUtf8().constData()).SetID(QStringLiteral("ISO-%1").arg(*citor).toUtf8().constData());
                        featureClassSmoothedISOOn->AddFeature(featureISO);
                    }
                }
            }
        }
        model->AddFeatureClass(featureClassISO);
        model->AddFeatureClass(featureClassSmoothedISOOff);
        model->AddFeatureClass(featureClassSmoothedISOOn);
    }
    this->projectPanel_->appendG3DModel(model, true);
    this->projectPanel_->expandStructureModelTree();
    Events::PostEvent(Events::Type::ResetAndUpdateScene, this);
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
    case Events::Type::UpdateScene:
    {
        BusyCursor waiting;
        renderWidget_->render();
        return true;
    }
    case Events::Type::ResetAndUpdateScene:
    {
        BusyCursor waiting;
        renderWidget_->resetAndRender();
        return true;
    }
    case Events::Type::Menu_OpenGeo3DML:
    {
        openGeo3DML();
        return true;
    }
    case Events::Type::Menu_OpenDrillLog:
    {
        openDrillLog();
        return true;
    }
    case Events::Type::Menu_SaveToGeo3DML:
    {
        saveToGeo3DML();
        return true;
    }
    case Events::Type::Menu_CloseStructureModel:
    {
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
        selectedFilter.contains(QStringLiteral("v1.x")) ? g3dxml::SchemaVersion::Schema_1_x : g3dxml::SchemaVersion::Schema_1_0);
    BusyCursor::endWaiting();
    if (isOK) {
        QMessageBox::information(this, QString(), Text::tipOfSucceedInSavingToGeo3DMLFile(projName));
    } else {
        QMessageBox::critical(this, QString(), Text::tipOfErrorInSavingToGeo3DMLFile(projName, QString::fromUtf8(projectWriter.Error().c_str())));
    }
}
