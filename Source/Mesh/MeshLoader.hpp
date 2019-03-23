//  Copyright © 2019 Whelan Callahan. All rights reserved.
#pragma once
#include <memory>
#include "MeshData.hpp"

class MeshLoader
{
public:

    static std::shared_ptr<std::vector<MeshData>> LoadFromFile(const char *filepath);

};
