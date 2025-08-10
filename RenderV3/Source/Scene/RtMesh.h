#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <Type/Types.h>
#include <Scene/Vertex.h>
namespace Rt
{
    using IndexType = uint32_t;
    class Mesh {
    public:
        Mesh() = default;
        Mesh(std::string& InName, std::vector<Rt::Vertex> &InVertices, std::vector<IndexType>& InVi)
            : name_(InName), vertices(InVertices), indices(InVi)
        {
            matrix = glm::mat4(1.0);
        }
        Mesh(std::string& name, std::vector<Point3f>& verts, std::vector<Normal3f>& normals, std::vector<Point2f>& uvs, std::vector<int>& vi) {
            name_ = name;
            size_t vertexCount = verts.size();
            vertices.resize(vertexCount);
            for (int i = 0; i < vertexCount; i++) {
                Vertex& v = vertices[i];
                v.pos = glm::vec3(verts[i].x, verts[i].y, verts[i].z);
                v.normal = glm::vec3(normals[i].x, normals[i].y, normals[i].z);
                v.uv = glm::vec2(uvs[i].x, uvs[i].y);
            }
            size_t indexCount = vi.size();
            indices.resize(indexCount);
            for (int i = 0; i < indexCount; i++) {
                indices[i] = vi[i];
            }
            matrix = glm::mat4(1.0);
        }
        Mesh(std::vector<Vertex>& _vertices, std::vector<uint32_t>& _indices) {
            vertices.insert(vertices.end(), _vertices.begin(), _vertices.end());
            indices.insert(indices.end(), _indices.begin(), _indices.end());
            matrix = glm::mat4(1.0);
        }

        uint32_t GetVertexCount() {
            return vertices.size();
        }

        uint32_t GetVertexSize() {
            return vertices.size() * sizeof(Rt::Vertex);
        }

        uint32_t GetStride() {
            return sizeof(Rt::Vertex);
        }

        Vertex* GetVertexData() {
            return vertices.data();
        }

        uint32_t GetIndexSize() {
            return indices.size() * sizeof(IndexType);
        }

        uint32_t GetIndexCount() {
            return indices.size();
        }

        IndexType* GetIndexData() {
            return indices.data();
        }

        std::string name_;

        std::vector<Rt::Vertex> vertices;
        std::vector<IndexType> indices;

        //uint32_t indexOfFirstVertexIndex;

        uint32_t uMatIndex = 0; //默认材质
        /*
            材质名称
        */
        std::string MaterialName;
        glm::mat4 matrix;
    };

    class MeshInstance
    {
    public:
        MeshInstance(int meshId, glm::mat4 transformation, int matId) : modelTransform(transformation), materialId(matId), meshId(meshId) {}
        ~MeshInstance() = default;

        int meshId;
        glm::mat4 modelTransform = glm::mat4(1.f);
        int materialId;
    };
}