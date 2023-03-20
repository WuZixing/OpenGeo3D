#pragma once

#include <map>
#include <geo3dml/Field.h>
#include <geo3dml/Point3D.h>
#include <g3dvtk/MultiPoint.h>
#include <QString>

class DrillLogFile {
public:
	DrillLogFile(const QString& no, const geo3dml::Point3D& headPos, const QString& filePath);
	virtual ~DrillLogFile();

	const QString& GetDrillNo() const;
	const QString& GetFilePath() const;
	geo3dml::Point3D GetHeadPosition() const;

	g3dvtk::MultiPoint* ToMultiPoint(const std::map<std::string, geo3dml::Field>& shapeProperties) const;
	bool ToMultiPoinit(const std::map<std::string, geo3dml::Field>& shapeProperties, g3dvtk::MultiPoint* pts) const;

private:
	QString drillNo_, filePath_;
	geo3dml::Point3D headPos_;
};
