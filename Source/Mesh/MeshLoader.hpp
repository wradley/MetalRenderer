//  Copyright © 2019 Whelan Callahan. All rights reserved.
#pragma once
#include <memory>
#include "CPUMesh.hpp"

class MeshLoader
{
public:

    static std::vector<std::shared_ptr<CPUMesh>> LoadFromFile(const char *filepath);

};
