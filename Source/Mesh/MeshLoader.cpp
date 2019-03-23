//  Copyright © 2019 Whelan Callahan. All rights reserved.

#include "MeshLoader.hpp"
//#include <fbxsdk.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <iostream>
#include <string>


static void LoadMesh(const aiMesh *mesh, MeshData &meshData)
{
    unsigned int vertexCount = mesh->mNumVertices;
    unsigned int faceCount = mesh->mNumFaces;
    meshData.vertices.resize(vertexCount);
    meshData.indices.reserve(faceCount * 3);
    
    for (unsigned int i = 0; i < vertexCount; ++i)
    {
        Vertex &vertex = meshData.vertices[i];
        vertex.position = simd_make_float3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        vertex.normal = simd_make_float3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        if (mesh->mTextureCoords[0])
            vertex.uv = simd_make_float2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
    }
    
    for (unsigned int i = 0; i < faceCount; ++i)
    {
        meshData.indices.push_back(mesh->mFaces[i].mIndices[0]);
        meshData.indices.push_back(mesh->mFaces[i].mIndices[1]);
        meshData.indices.push_back(mesh->mFaces[i].mIndices[2]);
    }
}


static void LoadScene(const std::string &filepath, std::vector<MeshData> &meshDatas)
{
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(filepath,
                                             aiProcess_Triangulate |
                                             aiProcess_GenSmoothNormals |
                                             aiProcess_JoinIdenticalVertices |
                                             aiProcess_FlipWindingOrder);
    if (scene) {
        unsigned int meshCount = scene->mNumMeshes;
        meshDatas.resize(meshCount);
        
        for (int i = 0; i < meshCount; ++i)
        {
            LoadMesh(scene->mMeshes[i], meshDatas[i]);
        }
    }
    
    else {
        std::cout << "Could not load file from: " << filepath << std::endl;
    }
}


std::shared_ptr<std::vector<MeshData>> MeshLoader::LoadFromFile(const char *filepath)
{
    auto meshDatas = new std::vector<MeshData>;
    LoadScene(filepath, *meshDatas);
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
