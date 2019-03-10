//  Copyright © 2019 Whelan Callahan. All rights reserved.
#include "Model.hpp"
#include "RenderGroup.hpp"
#include "../Mesh/GPUMesh.hpp"


Model::Model(RenderGroup *g) : RenderObject(g)
{}


Model::~Model()
{}


void Model::setMesh(std::shared_ptr<GPUMesh> mesh)
{
    mMesh = mesh;
}


void Model::setColor(std::shared_ptr<Texture> texture)
{
    mColor = texture;
}
