//  Copyright © 2019 Whelan Callahan. All rights reserved.
#include "RendererResourceDelegate.hpp"
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#include <iostream>


RendererResourceDelegate::RendererResourceDelegate(id Device,
                                                   id cmdQ,
                                                   std::vector<id> &vertexIndexBuffers,
                                                   std::vector<id> &uniformBuffers,
                                                   std::vector<id> &boneBuffers,
                                                   std::vector<id> &textures) :
    mDevice(Device),
    mCmdQ(cmdQ),
    mVertexIndexBuffers(vertexIndexBuffers),
    mUniformBuffers(uniformBuffers),
    mNextFreeUniform(0),
    mBoneBuffers(boneBuffers),
    mNextFreeBoneOffset(0),
    mTextures(textures)
{
    id<MTLDevice> device = mDevice;
    id<MTLBuffer> uniformBuff = [device newBufferWithLength:UniformBufferSize options:MTLResourceStorageModeShared];
    id<MTLBuffer> boneBuff = [device newBufferWithLength:BoneBufferSize options:MTLResourceStorageModeShared];
    mUniformBuffers.push_back(uniformBuff);
    mBoneBuffers.push_back(boneBuff);
}


RendererResourceDelegate::~RendererResourceDelegate()
{}


std::vector<Mesh> RendererResourceDelegate::createMeshes(std::shared_ptr<std::vector<MeshData>> datas)
{
    id<MTLDevice> device = mDevice;
    id<MTLCommandQueue> cmdQ = mCmdQ;
    id<MTLCommandBuffer> cmdBuff = [cmdQ commandBuffer];
    id<MTLBlitCommandEncoder> encoder = [cmdBuff blitCommandEncoder];
    std::vector<id<MTLBuffer>> uploads;
    std::vector<Mesh> meshes;
    
    for (MeshData &data : *datas) {
        Mesh mesh;
        unsigned int vertsSize = sizeof(Vertex) * data.vertices.size();
        unsigned int indicesSize = sizeof(Index) * data.indices.size();
        unsigned int totalSize = vertsSize + indicesSize;
        
        id<MTLBuffer> upload = [device newBufferWithLength:totalSize
                                                   options:MTLResourceStorageModeShared];
        void *indexContentsStart = (char*)upload.contents + vertsSize;
        memcpy(upload.contents, &data.vertices[0], vertsSize);
        memcpy(indexContentsStart, &data.indices[0], indicesSize);
        
        id<MTLBuffer> viBuffer = [device newBufferWithLength:totalSize
                                                     options:MTLResourceStorageModePrivate];
        
        [encoder copyFromBuffer:upload
                   sourceOffset:0
                       toBuffer:viBuffer
              destinationOffset:0
                           size:totalSize];
        
        mesh.vertexIndexBuffer = mVertexIndexBuffers.size();
        mesh.indexCount = data.indices.size();
        mesh.indexOffset = vertsSize;
        assignUniform(mesh.uniformBuffer, mesh.uniformBufferOffset);
        if (data.skeleton.boneCount)
            assignBoneMatrices(data.skeleton.boneCount, mesh.boneBuffer, mesh.boneBufferOffset);
        uploads.push_back(upload);
        meshes.push_back(mesh);
        mVertexIndexBuffers.push_back(viBuffer);
    }
    
    [encoder endEncoding];
    [cmdBuff commit];
    return meshes;
}


uint64_t RendererResourceDelegate::createTexture(const char *fp)
{
    id<MTLDevice> device = mDevice;
    id<MTLCommandQueue> cmdQ = mCmdQ;
    id<MTLCommandBuffer> commandBuffer = [cmdQ commandBuffer];
    id<MTLBlitCommandEncoder> encoder = [commandBuffer blitCommandEncoder];

    NSString *NSfp = [[NSString alloc] initWithUTF8String:fp];
    NSURL *textureURL = [[NSURL alloc] initFileURLWithPath:NSfp];
    NSError *error = NULL;
    MTKTextureLoader *textureLoader = [[MTKTextureLoader alloc] initWithDevice:device];
    id<MTLTexture> mtlTexture = [textureLoader newTextureWithContentsOfURL:textureURL options:nil error:&error];

    [encoder endEncoding];
    [commandBuffer commit];

    mTextures.push_back(mtlTexture);
    return mTextures.size()-1;
}


void RendererResourceDelegate::assignUniform(uint32_t &buffer, uint32_t &offset)
{
    if (mNextFreeUniform + sizeof(Uniform) > UniformBufferSize) {
        id<MTLDevice> device = mDevice;
        id<MTLBuffer> newBuff = [device newBufferWithLength:UniformBufferSize
                                                    options:MTLResourceStorageModeShared];
        mUniformBuffers.push_back(newBuff);
        mNextFreeUniform = 0;
    }
    
    buffer = mUniformBuffers.size()-1;
    offset = mNextFreeUniform;
    mNextFreeUniform += sizeof(Uniform);
}


void RendererResourceDelegate::assignBoneMatrices(uint32_t boneCount, uint32_t &buffer, uint32_t &offset)
{
    if (boneCount * sizeof(BoneData) > BoneBufferSize)
    {
        std::cout << "Too many bones" << std::endl;
        return;
    }
    
    if (mNextFreeBoneOffset + (sizeof(BoneData) * boneCount) > BoneBufferSize) {
        id<MTLDevice> device = mDevice;
        id<MTLBuffer> newBuff = [device newBufferWithLength:BoneBufferSize
                                                    options:MTLResourceStorageModeShared];
        mBoneBuffers.push_back(newBuff);
        mNextFreeBoneOffset = 0;
    }
    
    buffer = mBoneBuffers.size()-1;
    offset = mNextFreeBoneOffset;
    mNextFreeBoneOffset += sizeof(BoneData) * boneCount;
}
