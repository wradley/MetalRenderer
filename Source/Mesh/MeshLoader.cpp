//  Copyright © 2019 Whelan Callahan. All rights reserved.

#include "MeshLoader.hpp"
//#include <fbxsdk.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <iostream>
#include <map>
#include <string>


// weights, boneIDs, and weightsFound must be resized to bone count before calling function
static void FindBone(const std::map<std::string, aiBone*> &bones,
                     const aiNode *node,
                     uint32_t parent,
                     std::vector<uint32_t> &parents,
                     std::vector<AnimationTransform> &transforms,
                     std::vector<Vertex> &verts,
                     std::vector<simd_int4> &weightsFound)
{
    if (bones.find(node->mName.C_Str()) != bones.end())
    {
        const aiBone *bone = bones.at(node->mName.C_Str());
        uint32_t self = parents.size();
        
        // get transform from bone
        AnimationTransform transform;
        aiQuaterniont<float> rotation;
        aiVector3t<float> position;
        bone->mOffsetMatrix.DecomposeNoScaling(rotation, position);
        transform.rotation = simd_quaternion(rotation.x, rotation.y, rotation.z, rotation.w);
        transform.position = simd_make_float3(position.x, position.y, position.z);
        
        // get weights
        for (int i = 0; i < bone->mNumWeights; ++i)
        {
            aiVertexWeight weight = bone->mWeights[i];
            
            for (int j = 0; j < 4; ++j)
            {
                // add to next free spot in bone weights
                if (!weightsFound[weight.mVertexId][j])
                {
                    weightsFound[weight.mVertexId][j] = 1;
                    verts[weight.mVertexId].boneWeights[j] = weight.mWeight;
                    verts[weight.mVertexId].boneIDs[j] = self;
                    break;
                }
            }
        }
        
        // set parent and add to list
        parents.push_back(parent);
        transforms.push_back(transform);
        parent = self;
    }
    
    // add children
    for (unsigned int i = 0; i < node->mNumChildren; ++i)
        FindBone(bones, node->mChildren[i], parent, parents, transforms, verts, weightsFound);
}


// verts should be filled out before calling
static void LoadSkeletonAndWeights(const aiScene *scene,
                                   const aiMesh *mesh,
                                   Skeleton &skeleton,
                                   std::vector<Vertex> &verts)
{
    std::map<std::string, aiBone*> bones;
    for (unsigned int i = 0; i < mesh->mNumBones; ++i)
        bones[mesh->mBones[i]->mName.C_Str()] = mesh->mBones[i];
    
    std::vector<uint32_t> parents;
    std::vector<AnimationTransform> transforms;
    parents.reserve(bones.size());
    transforms.reserve(bones.size());

    std::vector<simd_int4> weightsFound;
    weightsFound.resize(verts.size(), simd_make_int4(0, 0, 0, 0));
    
    FindBone(bones, scene->mRootNode, 0, parents, transforms, verts, weightsFound);
    skeleton = Skeleton(&parents[0], &transforms[0], bones.size());
}


static void LoadMesh(const aiScene *scene, const aiMesh *mesh, MeshData &meshData)
{
    meshData.vertices.resize(mesh->mNumVertices);
    meshData.indices.reserve(mesh->mNumFaces * 3);
    
    for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
    {
        Vertex &vertex = meshData.vertices[i];
        vertex.position = simd_make_float3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        vertex.normal = simd_make_float3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        if (mesh->mTextureCoords[0])
            vertex.uv = simd_make_float2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        vertex.boneWeights = simd_make_float4(0, 0, 0, 0);
    }
    
    for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
    {
        meshData.indices.push_back(mesh->mFaces[i].mIndices[0]);
        meshData.indices.push_back(mesh->mFaces[i].mIndices[1]);
        meshData.indices.push_back(mesh->mFaces[i].mIndices[2]);
    }
    
    if (mesh->HasBones())
        LoadSkeletonAndWeights(scene, mesh, meshData.skeleton, meshData.vertices);
}


// return false if no channels are found
//static bool LoadAnimation(const aiAnimation *animation, Animation &ret)
//{
//    // KeyFrame *keyFrames, uint32_t keyFrameCount, uint64_t fps, uint64_t totalFrames, const std::string &name
//    float fps = animation->mTicksPerSecond;
//    std::string name(animation->mName.C_Str());
//    uint64_t totalFrames = animation->mDuration;
//    unsigned int boneCount = animation->mNumChannels;
//    if (!boneCount) return false;
//
//    unsigned int keyFrameCount = animation->mChannels[0]->mNumPositionKeys;
//    KeyFrame *keyFrames = new KeyFrame[keyFrameCount];
//    for (unsigned int kf = 0; kf < keyFrameCount; ++kf)
//    {
//        keyFrames[kf].frameNumber = animation->mChannels[0]->mPositionKeys[0].mTime * fps;
//        keyFrames[kf].transformCount = boneCount;
//        keyFrames[kf].transforms = new AnimationTransform[boneCount];
//        for (unsigned int bn = 0; bn < boneCount; ++bn)
//        {
//            keyFrames[kf].transforms[bn].position = simd_make_float3(animation->mChannels[bn]->mPositionKeys[kf].mValue.x,
//                                                                     animation->mChannels[bn]->mPositionKeys[kf].mValue.y,
//                                                                     animation->mChannels[bn]->mPositionKeys[kf].mValue.z);
//            keyFrames[kf].transforms[bn].orientation = simd_quaternion(animation->mChannels[bn]->mRotationKeys[kf].mValue.x,
//                                                                       animation->mChannels[bn]->mRotationKeys[kf].mValue.y,
//                                                                       animation->mChannels[bn]->mRotationKeys[kf].mValue.z,
//                                                                       animation->mChannels[bn]->mRotationKeys[kf].mValue.w);
//        }
//    }
//
//    ret = Animation(keyFrames, keyFrameCount, fps, totalFrames, name);
//    return true;
//}


static void LoadScene(const std::string &filepath, std::vector<MeshData> &meshDatas, std::vector<Animation> &animations)
{
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(filepath,
                                             aiProcess_Triangulate |
                                             aiProcess_GenSmoothNormals |
                                             aiProcess_JoinIdenticalVertices |
                                             aiProcess_FlipWindingOrder);
    if (scene)
    {
        meshDatas.resize(scene->mNumMeshes);
        for (int i = 0; i < scene->mNumMeshes; ++i)
            LoadMesh(scene, scene->mMeshes[i], meshDatas[i]);
        
        animations.resize(scene->mNumAnimations);
//        for (int i = 0; i < scene->mNumAnimations; ++i)
//            LoadAnimation(scene->mAnimations[i], animations[i]);
    }
    
    else std::cout << "Could not load file from: " << filepath << std::endl;
    
}


std::shared_ptr<std::vector<MeshData>> MeshLoader::LoadFromFile(const char *filepath)
{
    auto meshDatas = new std::vector<MeshData>;
    auto animations = new std::vector<Animation>;
    LoadScene(filepath, *meshDatas, *animations);
    return std::shared_ptr<std::vector<MeshData>>(meshDatas);
}


//static FbxScene* LoadScene(FbxManager *fbxManager, const char *filepath)
//{
//    int sdkMajor,  sdkMinor,  sdkRevision;
//    int fileMajor, fileMinor, fileRevision;
//    FbxManager::GetFileFormatVersion(sdkMajor, sdkMinor, sdkRevision);
//
//    FbxIOSettings *ioSettings = FbxIOSettings::Create(fbxManager, IOSROOT);
//    if (!ioSettings) {
//        std::cout << "Could not create fbx io settings" << std::endl;
//        fbxManager->Destroy();
//        return nullptr;
//    }
//
//    fbxManager->SetIOSettings(ioSettings);
//
//    FbxScene *scene = FbxScene::Create(fbxManager, "Root Scene");
//    if (!scene) {
//        std::cout << "Could not create fbx scene" << std::endl;
//        fbxManager->Destroy();
//        return nullptr;
//    }
//
//    FbxImporter *importer = FbxImporter::Create(fbxManager, "Scene importer");
//    if (!importer) {
//        std::cout << "Could not create fbx importer" << std::endl;
//        fbxManager->Destroy();
//        return nullptr;
//    }
//
//    bool status = importer->Initialize(filepath, -1, fbxManager->GetIOSettings());
//    importer->GetFileVersion(fileMajor, fileMinor, fileRevision);
//    if (!status) {
//        FbxString error = importer->GetStatus().GetErrorString();
//        std::cout << "Could not import fbx file: [" << filepath << "]  FBX error: " << error.Buffer() << std::endl;
//        if (importer->GetStatus().GetCode() == FbxStatus::eInvalidFileVersion) {
//            printf("FBX file format version for this FBX SDK is %d.%d.%d\n", sdkMajor, sdkMinor, sdkRevision);
//            printf("FBX file format version for file '%s' is %d.%d.%d\n\n", filepath, fileMajor, fileMinor, fileRevision);
//        }
//        fbxManager->Destroy();
//        return nullptr;
//    }
//
//    status = importer->Import(scene);
//    if (!status) {
//        std::cout << "Could not load fbx scene from file: " << filepath << std::endl;
//        fbxManager->Destroy();
//        return nullptr;
//    }
//
//    importer->Destroy();
//
//    std::cout << "Loading fbx file: " << filepath << std::endl;
//    return scene;
//}
//
//
//static simd_float3 GetNormal(FbxMesh *mesh, int controlPoint, int vertex)
//{
//    simd_float3 ret = simd_make_float3(0.0f, 0.0f, 0.0f);
//    FbxGeometryElementNormal *normalElement = mesh->GetElementNormal(0);
//
//    if (normalElement) {
//        switch (normalElement->GetMappingMode()) {
//            case FbxGeometryElement::eByControlPoint:
//                switch (normalElement->GetReferenceMode()) {
//                    case FbxGeometryElement::eDirect:
//                        ret[0] = (float) normalElement->GetDirectArray().GetAt(controlPoint)[0];
//                        ret[1] = (float) normalElement->GetDirectArray().GetAt(controlPoint)[1];
//                        ret[2] = (float) normalElement->GetDirectArray().GetAt(controlPoint)[2];
//                        break;
//                    case FbxGeometryElement::eIndexToDirect: {
//                            int index = normalElement->GetIndexArray().GetAt(controlPoint);
//                            ret[0] = (float) normalElement->GetDirectArray().GetAt(index)[0];
//                            ret[1] = (float) normalElement->GetDirectArray().GetAt(index)[1];
//                            ret[2] = (float) normalElement->GetDirectArray().GetAt(index)[2];
//                        } break;
//                    default:
//                        break;
//                }
//                break;
//            case FbxGeometryElement::eByPolygonVertex:
//                switch (normalElement->GetReferenceMode()) {
//                    case FbxGeometryElement::eDirect:
//                        ret[0] = (float) normalElement->GetDirectArray().GetAt(vertex)[0];
//                        ret[1] = (float) normalElement->GetDirectArray().GetAt(vertex)[1];
//                        ret[2] = (float) normalElement->GetDirectArray().GetAt(vertex)[2];
//                        break;
//                    case FbxGeometryElement::eIndexToDirect: {
//                        int index = normalElement->GetIndexArray().GetAt(vertex);
//                        ret[0] = (float) normalElement->GetDirectArray().GetAt(index)[0];
//                        ret[1] = (float) normalElement->GetDirectArray().GetAt(index)[1];
//                        ret[2] = (float) normalElement->GetDirectArray().GetAt(index)[2];
//                    } break;
//                    default:
//                        break;
//                }
//                break;
//            default:
//                break;
//        }
//    }
//
//    return ret;
//}
//
//
//static simd_float2 GetUV(FbxMesh *mesh, int controlPoint, int vertex)
//{
//    simd_float2 uv = simd_make_float2(0.0f, 0.0f);
//    FbxGeometryElementUV *elementUV = mesh->GetElementUV(0);
//    if (elementUV) {
//        switch (elementUV->GetMappingMode()) {
//            case FbxGeometryElement::eByControlPoint:
//                switch (elementUV->GetReferenceMode()) {
//                    case FbxGeometryElement::eDirect:
//                        uv[0] = elementUV->GetDirectArray().GetAt(controlPoint)[0];
//                        uv[1] = elementUV->GetDirectArray().GetAt(controlPoint)[1];
//                    case FbxGeometryElement::eIndexToDirect: {
//                        int index = elementUV->GetIndexArray().GetAt(controlPoint);
//                        uv[0] = elementUV->GetDirectArray().GetAt(index)[0];
//                        uv[1] = elementUV->GetDirectArray().GetAt(index)[1];
//                    }   break;
//                    default:
//                        break;
//                }
//                break;
//            case FbxGeometryElement::eByPolygonVertex:
//                switch (elementUV->GetReferenceMode()) {
//                    case FbxGeometryElement::eDirect:
//                        uv[0] = elementUV->GetDirectArray().GetAt(vertex)[0];
//                        uv[1] = elementUV->GetDirectArray().GetAt(vertex)[1];
//                    case FbxGeometryElement::eIndexToDirect: {
//                        int index = elementUV->GetIndexArray().GetAt(vertex);
//                        uv[0] = elementUV->GetDirectArray().GetAt(index)[0];
//                        uv[1] = elementUV->GetDirectArray().GetAt(index)[1];
//                    }   break;
//                    default:
//                        break;
//                }
//                break;
//            default:
//                break;
//        }
//    }
//
//    return uv;
//}
//
//
//static std::shared_ptr<MeshData> ProccessMesh(FbxMesh *mesh)
//{
//    auto meshData = std::make_shared<MeshData>();
//
//    if (!mesh->IsTriangleMesh()) {
//        std::cout << "Mesh is not triangulated" << std::endl;
//        return meshData;
//    }
//
//    auto ctrlPts = mesh->GetControlPoints();
//    int triangleCount = mesh->GetPolygonCount();
//
//    int counter = 0;
//    for (int i = 0; i < triangleCount; ++i) {
//        for (int j = 0; j < 3; ++j) {
//            int index = mesh->GetPolygonVertex(i, j);
//            meshData->indices.push_back(counter);
//
//            Vertex v;
//            v.position = simd_make_float3(ctrlPts[index][0], ctrlPts[index][1], ctrlPts[index][2]);
//            v.normal = GetNormal(mesh, index, counter);
//            v.uv = GetUV(mesh, index, counter);
//            meshData->vertices.push_back(v);
//
//            ++counter;
//        }
//    }
//
//    return meshData;
//}
//
//
//static void ProccessNode(FbxNode *node, std::vector<std::shared_ptr<MeshData>> &meshDatas)
//{
//    FbxMesh *mesh = node->GetMesh();
//    if (mesh) {
//        std::shared_ptr<MeshData> meshData = ProccessMesh(mesh);
//        if (meshData) meshDatas.push_back(meshData);
//    }
//
//    int childCount = node->GetChildCount();
//    for (int i = 0; i < childCount; ++i) {
//        ProccessNode(node->GetChild(i), meshDatas);
//    }
//}
//
//
//static void LoadAnimationChannels(FbxAnimLayer *layer, FbxNode *node)
//{
//    FbxNodeAttribute *attrib = node->GetNodeAttribute();
//    if (attrib)
//    {
//        if (attrib->GetAttributeType() == FbxNodeAttribute::eMesh)
//        {
//            FbxGeometry *geom = (FbxGeometry*) attrib;
//            //int bendShapeDeformerCount = geom->GetDeformerCount(FbxDeformer::eBlendShape);
//            int skinDeformerCount = geom->GetDeformerCount(FbxDeformer::eSkin);
//
//            for (int i = 0; i < skinDeformerCount; ++i)
//            {
//                FbxSkin *skin = (FbxSkin*) geom->GetDeformer(i, FbxDeformer::eSkin);
//                int clusterCount = skin->GetClusterCount();
//                for (int j = 0; j < clusterCount; ++j)
//                {
//                    FbxCluster *cluster = skin->GetCluster(j);
//                    int count = cluster->GetControlPointIndicesCount();
//                    int test = count;
//                }
//            }
//        }
//    }
//}
//
//
//static void LoadAnimationLayer(FbxAnimLayer *layer, FbxNode *node)
//{
//    LoadAnimationChannels(layer, node);
//
//    int childCount = node->GetChildCount();
//    for (int i = 0; i < childCount; ++i)
//        LoadAnimationChannels(layer, node->GetChild(i));
//}
//
//
//static void LoadAnimationStack(FbxAnimStack *stack, FbxNode *node)
//{
//    int animLayerCount = stack->GetMemberCount<FbxAnimLayer>();
//    for (int i = 0; i < animLayerCount; ++i)
//    {
//        FbxAnimLayer *layer = stack->GetMember<FbxAnimLayer>(i);
//        LoadAnimationLayer(layer, node);
//    }
//}
//
//
//std::vector<std::shared_ptr<MeshData>> MeshLoader::LoadFromFile(const char *filepath)
//{
//    std::vector<std::shared_ptr<MeshData>> meshDatas;
//
//    FbxManager *manager = FbxManager::Create();
//    if (!manager) {
//        std::cout << "Could not create fbx manager" << std::endl;
//        return meshDatas;
//    }
//
//    FbxScene *scene = LoadScene(manager, filepath);
//    FbxGeometryConverter geomConverter(manager);
//    geomConverter.Triangulate(scene, true);
//    FbxNode *rootNode = scene->GetRootNode();
//    if (!rootNode) return meshDatas;
//
//    // temp animation -----------------------------------------------
//
//    for (int i = 0; i < scene->GetSrcObjectCount<FbxAnimStack>(); ++i)
//    {
//        FbxAnimStack *animStack = scene->GetSrcObject<FbxAnimStack>(i);
//        FbxString name = animStack->GetName();
//        std::cout << "Animation Stack (" << i << "): " << name << std::endl;
//        LoadAnimationStack(animStack, rootNode);
//    }
//
//    // --------------------------------------------------------------
//
//    ProccessNode(rootNode, meshDatas);
//
//    manager->Destroy();
//
//    return meshDatas;
//}
