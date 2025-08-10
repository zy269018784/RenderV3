#pragma once
#include <cpu_gpu.h>
#include <Math/Math.h>
#include <Type/Types.h>
namespace Render {
    class TriangleMesh {
    public:
        //unsigned int meshTriangleCount;			    // 网格的三角形数量 = meshIndexCount / 3
        //unsigned int meshFirstTriangleIndex;	    // 网格的第一个三角形索引 = meshFirstIndex / 3

        CPU_GPU unsigned int IndexOfFirstVertexIndex() {
            return indexOfFirstVertexIndex;
        }

        CPU_GPU void SetIndexOfFirstVertexIndex(unsigned int a) {
            indexOfFirstVertexIndex = a;
        }

        // 顶点数量
        CPU_GPU void SetVertexCount(unsigned int a) {
            vertexCount = a;
        }

        CPU_GPU unsigned int VertexCount() {
            return vertexCount;
        }


        // 顶点索引数量
        CPU_GPU void SetVertexIndexCount(unsigned int a) {
            vertexIndexCount = a;
        }

        CPU_GPU unsigned int VertexIndexCount() {
            return vertexIndexCount;
        }

        // 顶点
        CPU_GPU void SetPointerOfVertexs(Point3f* p) {
            pVertexs = p;
        }

        CPU_GPU Point3f* PointerOfVertexs() {
            return pVertexs;
        }

        // 索引
        CPU_GPU void SetPointerOfVertexIncices(unsigned int* p) {
            pVertexIndices = p;
        }

        CPU_GPU unsigned int* PointerOfVertexIncices() {
            return pVertexIndices;
        }

        // 法线
        CPU_GPU void SetPointerOfNormals(Normal3f* p) {
            pNormals = p;
        }

        CPU_GPU Normal3f* PointerOfNormals() {
            return pNormals;
        }

        // uv
        CPU_GPU void SetPointerOfUVs(Point2f* p) {
            pUVs = p;
        }

        CPU_GPU Point2f* PointerOfUVs() {
            return pUVs;
        }

        // 切线
        CPU_GPU void SetPointerOfTangents(Vector3f* p) {
            pTangents = p;
        }

        CPU_GPU Vector3f* PointerOfTangents() {
            return pTangents;
        }

        // 网格三角形的数量
        CPU_GPU unsigned int TriangleCount() {
            return vertexIndexCount / 3;
        }
    private:
        unsigned int vertexCount;                   // 顶点数量
        Point3f* pVertexs;                          // 顶点      
        Point2f* pUVs;                              // uv
        Normal3f* pNormals;	                        // 法线数组         
        Vector3f* pTangents;                        // 切线
        unsigned int* pVertexIndices;               // 顶点索引
        unsigned int vertexIndexCount;              // 顶点索引的数量
        unsigned int indexOfFirstVertexIndex;

    };
}