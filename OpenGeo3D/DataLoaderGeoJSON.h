// UTF-8编码
#pragma once

#include <geo3dml/Model.h>

namespace opengeo3d {
    class DataLoaderGeoJSON {
    public:
        DataLoaderGeoJSON();
        virtual ~DataLoaderGeoJSON();

        geo3dml::Model* loadAsG3DModel() const;
    };
}
