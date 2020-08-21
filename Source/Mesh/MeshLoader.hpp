//  Copyright © 2019 Whelan Callahan. All rights reserved.
#pragma once
#include <memory>
#include "MeshData.hpp"
#include "../Renderer/Skeleton.hpp"
#include "../Renderer/Animation.hpp"

class SceneLoader
{
public:

    struct Data
    {
        std::vector<MeshData> meshDatas;
        Skeleton skeleton;
        std::vector<Animation> animations;
    };
    
    static std::shared_ptr<Data> LoadFromFile(const char *filepath);

};
