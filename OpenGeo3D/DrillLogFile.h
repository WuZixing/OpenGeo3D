#pragma once

#include <map>
#include <geo3dml/Field.h>
#include <geo3dml/Point.h>
#include <g3dvtk/MultiPoint.h>

class DrillLogFile {
public:
	DrillLogFile(const std::string& no, const geo3dml::Point3D& headPos, const std::string& filePath);
	virtual ~DrillLogFile();

	std::string GetDrillNo() const;
	std::string GetFilePath() const;
	geo3dml::Point3D GetHeadPosition() const;

	g3dvtk::MultiPoint* ToMultiPoint(const std::map<std::string, geo3dml::Field>& shapeProperties) const;
	bool ToMultiPoinit(const std::map<std::string, geo3dml::Field>& shapeProperties, g3dvtk::MultiPoint* pts) const;

private:
	std::string drillNo_, filePath_;
	geo3dml::Point3D headPos_;
};
