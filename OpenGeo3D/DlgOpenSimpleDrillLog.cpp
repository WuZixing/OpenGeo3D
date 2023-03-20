#include "DlgOpenSimpleDrillLog.h"
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QVBoxLayout>
#include <g3dvtk/ObjectFactory.h>
#include "DrillLogFile.h"
#include "Text.h"

DlgOpenSimpleDrillLog::DlgOpenSimpleDrillLog(QWidget* parent) : QDialog(parent, Qt::WindowType::Dialog | Qt::WindowType::WindowCloseButtonHint) {
	setWindowTitle(Text::menuOpenDrillLog());
	initUI();
}

DlgOpenSimpleDrillLog::~DlgOpenSimpleDrillLog() {

}

void DlgOpenSimpleDrillLog::initUI() {
	group_ = new GroupOfSimpleDrillLogFiles(this);
	QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::StandardButton::Ok | QDialogButtonBox::StandardButton::Cancel, Qt::Orientation::Horizontal);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(group_, 1, Qt::AlignmentFlag::AlignLeft);
	layout->addWidget(buttonBox, 0, Qt::AlignmentFlag::AlignHCenter);
	setLayout(layout);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &DlgOpenSimpleDrillLog::accept);
	connect(buttonBox, &QDialogButtonBox::rejected, this, &DlgOpenSimpleDrillLog::reject);
}

void DlgOpenSimpleDrillLog::accept() {
	if (!group_->validate()) {
		return;
	}
	QDialog::accept();
}

geo3dml::Model* DlgOpenSimpleDrillLog::loadAsG3DModel() const {
	DrillPositionMap&& drillPositions = group_->getDrillPositions();
	if (drillPositions.empty()) {
		return nullptr;
	}
	DrillLogFileMap&& drillFiles = group_->getDrillLogFiles();
	if (drillFiles.empty()) {
		return nullptr;
	}
	if (group_->isSavingPositionToSHPEnabled()) {
		group_->savePositionToSHP();
	}
	DrillLogFieldMap&& targetFields = group_->getDrillLogFields();
	// model
	g3dvtk::ObjectFactory g3dFactory;
	geo3dml::Model* model = g3dFactory.NewModel();
	model->SetID("Drills");
	model->SetName(Text::nameOfDrillModel().toUtf8().constData());
	model->SetType(geo3dml::Model::ModelType::Model3D);
	// feature class of drill
	geo3dml::Field fieldPath;
	fieldPath.Name("file").Label("File Path").DataType(geo3dml::Field::Text);
	geo3dml::FeatureClass* fcDrills = g3dFactory.NewFeatureClass();
	fcDrills->SetID("Drills");
	fcDrills->SetName(Text::nameOfDrillFeatureClass().toUtf8().constData());
	fcDrills->AddField(fieldPath);
	model->AddFeatureClass(fcDrills);
	// features of drill
	geo3dml::Feature* featurePos = g3dFactory.NewFeature();
	featurePos->SetID("DrillPosition");
	featurePos->SetName(Text::nameOfDrillPositionFeature().toUtf8().constData());
	geo3dml::FieldValue textValue(fieldPath.Name(), std::string());
	featurePos->SetField(textValue);
	geo3dml::Annotation* annotation = g3dFactory.NewAnnotation();
	featurePos->AddGeometry(annotation);
	fcDrills->AddFeature(featurePos);
	for (auto drillFile = drillFiles.cbegin(); drillFile != drillFiles.cend(); ++drillFile) {
		if (drillPositions.find(drillFile->first) == drillPositions.cend()) {
			continue;
		}
		geo3dml::Point3D drillPos = drillPositions[drillFile->first];
		annotation->AddPoint(drillPos.x, drillPos.y, drillPos.z);
		annotation->SetLabelOfPointAt(annotation->GetPointCount() - 1, drillFile->first.toUtf8().constData());
		DrillLogFile drillLog(drillFile->first, drillPos, drillFile->second);
		geo3dml::Feature* feature = g3dFactory.NewFeature();
		feature->SetID(drillLog.GetDrillNo().toUtf8().constData());
		feature->SetName(drillLog.GetDrillNo().toUtf8().constData());
		textValue.SetString(drillLog.GetFilePath().toUtf8().constData());
		feature->SetField(textValue);
		g3dvtk::MultiPoint* mPt = drillLog.ToMultiPoint(targetFields);
		feature->AddGeometry(mPt);
		fcDrills->AddFeature(feature);
	}
	return model;
}
