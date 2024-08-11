#include "DrillLogFile.h"
#include <codecvt>
#include <fstream>
#include <regex>
#include <g3dvtk/ObjectFactory.h>

DrillLogFile::DrillLogFile(const QString& no, const geo3dml::Point3D& headPos, const QString& filePath) {
	drillNo_ = no;
	headPos_ = headPos;
	filePath_ = filePath;
}

DrillLogFile::~DrillLogFile() {

}

const QString& DrillLogFile::GetDrillNo() const {
	return drillNo_;
}

const QString& DrillLogFile::GetFilePath() const {
	return filePath_;
}

geo3dml::Point3D DrillLogFile::GetHeadPosition() const {
	return headPos_;
}

g3dvtk::MultiPoint* DrillLogFile::ToMultiPoint(const std::map<std::string, geo3dml::Field>& shapeProperties) const {
	g3dvtk::ObjectFactory g3dFactory;
	g3dvtk::MultiPoint* pts = static_cast<g3dvtk::MultiPoint*>(g3dFactory.NewMultiPoint());
	pts->SetID(geo3dml::Object::NewID());
	if (ToMultiPoinit(shapeProperties, pts)) {
		return pts;
	} else {
		delete pts;
		return nullptr;
	}
}

bool DrillLogFile::ToMultiPoinit(const std::map<std::string, geo3dml::Field>& shapeProperties, g3dvtk::MultiPoint* pts) const {
	std::ifstream logFile(filePath_.toStdString());
	geo3dml::ShapeProperty* shapeProp = pts->GetProperty(geo3dml::ShapeProperty::Vertex);
	if (shapeProp == NULL) {
		g3dvtk::ObjectFactory g3dFactory;
		shapeProp = g3dFactory.NewShapeProperty();
		shapeProp->SetID(geo3dml::Object::NewID());
		shapeProp->Name("Drill Sample");
		pts->SetProperty(shapeProp, geo3dml::ShapeProperty::Vertex);
	}
	// parse field names from the first line
	std::string strLine;
	std::vector<std::string> fieldNames;
	std::getline(logFile, strLine);
	std::regex pattern(R"([\t,]{1,1})");
	std::regex_token_iterator<std::string::iterator> rend;
	std::regex_token_iterator<std::string::iterator> token(strLine.begin(), strLine.end(), pattern, -1);
	if (token != rend) {
		++token;	// first column is depth.
	}
	while (token != rend) {
		fieldNames.push_back(*token);
		std::map<std::string, geo3dml::Field>::const_iterator fieldItor = shapeProperties.find(*token);
		if (fieldItor != shapeProperties.cend()) {
			shapeProp->AddField(fieldItor->second);
		}
		++token;
	}
	// read properties of points
	int ptIndex = pts->GetPointCount();
	while (!logFile.eof()) {
		std::getline(logFile, strLine);
		if (strLine.empty()) {
			continue;
		}
		std::regex_token_iterator<std::string::iterator> token(strLine.begin(), strLine.end(), pattern, -1);
		if (token == rend) {
			continue;
		}
		double depth = std::stod(*token++);
		pts->AddPoint(headPos_.x, headPos_.y, headPos_.z - depth);
		for (int f = 0; f < (int)fieldNames.size() && token != rend; ++f) {
			auto fieldItor = shapeProperties.find(fieldNames[f]);
			if (fieldItor == shapeProperties.cend()) {
				++token;
				continue;
			}
			switch (fieldItor->second.DataType()) {
			case geo3dml::Field::ValueType::Integer: {
				int v = std::stoi(*token);
				shapeProp->IntValue(f, ptIndex, v);
				break;
			}
			case geo3dml::Field::ValueType::Double: {
				double v = std::stod(*token);
				shapeProp->DoubleValue(f, ptIndex, v);
				break;
			}
			case geo3dml::Field::ValueType::Text: {
				shapeProp->TextValue(f, ptIndex, *token);
				break;
			}
			case geo3dml::Field::ValueType::Boolean: {
				shapeProp->BooleanValue(f, ptIndex, geo3dml::FieldValue::IsTrue(*token));
				break;
			}
			default:
				break;
			}
			++token;
		}
		++ptIndex;
	}
	return true;
}
