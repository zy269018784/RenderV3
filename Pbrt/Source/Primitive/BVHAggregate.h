#pragma once
#include <Math/Math.h>
#include <Primitive/Primitive.h>
#include <vector>
#include <atomic>
#include <Init.h>

namespace Render {
    struct BVHBuildNode;
    struct BVHPrimitive;
    struct ShapeIntersection;
    class Ray;

    enum class NodeType { Leaf, Interior };

    // BVHBuildNode Definition
    struct BVHBuildNode {

        
        // first: 第一个primitive的索引, n: primitive的数量 b: 包围盒
        void InitLeaf(int first, int n, const Bounds3f& b) {
            type = NodeType::Leaf;
            firstPrimOffset = first;
            nPrimitives = n;
            bounds = b;
            children[0] = children[1] = nullptr;

        }

        void InitInterior(int first, int n, int axis, BVHBuildNode* c0, BVHBuildNode* c1) {
            type = NodeType::Interior;
            firstPrimOffset = first;
            nPrimitives = n;
            bounds = Union(c0->bounds, c1->bounds);
            children[0] = c0;
            children[1] = c1;
            splitAxis = axis;
        }
        // 包围盒
        Bounds3f bounds;
        // 子结点
        BVHBuildNode* children[2];
        /*
            firstPrimOffset: 第一个primitive的索引.
            nPrimitives: 叶子结点时,为primitive数量; 内部结点时, 为0;
        */
        int splitAxis, firstPrimOffset, nPrimitives;
        NodeType type;
    };

    class BVHAggregate {
        public:
            enum class SplitMethod { SAH, HLBVH, Middle, EqualCounts };

   
            BVHAggregate(Primitive* p, int n, SplitMethod splitMethod = SplitMethod::SAH, Allocator *alloc = nullptr);
            BVHAggregate(Primitive* p, int n, struct BVHBuildNode* root, unsigned int* orderedPrimitiveIndices);

            void FreeNode(struct BVHBuildNode* node) {
                if (node != nullptr) {
                    FreeNode(node->children[0]);
                    FreeNode(node->children[1]);
                    delete node;
                }
            };

            CPU_GPU int NodeCount() { return totalNodes; };
            CPU_GPU Bounds3f Bounds() const;
            CPU_GPU Optional<ShapeIntersection> Intersect(const Ray& ray, Float tMax = Infinity) const;
            // 递归创建BVH
            BVHBuildNode* buildRecursive(std::vector<BVHPrimitive>& bvhPrimitives, int first, int n,
                                         unsigned int * orderedPrimitiveIndices, std::atomic<int>* orderedPrimitivesOffset,
                                          int* totalNodes);
            
            Bool SplitMiddle(std::vector<BVHPrimitive>& bvhPrimitives, int first, int n, int& mid, int dim,  Float pmid);
            void SplitEqualCounts(std::vector<BVHPrimitive>& bvhPrimitives, int first, int n, int& mid, int dim);
            

            std::string ToString() const;
            static BVHAggregate* Create(std::vector<Primitive> prims);
            int flattenBVH(BVHBuildNode* linearNode, BVHBuildNode* node, int* offset);
        public:
            unsigned int* orderedPrimitiveIndices;  // 排序后的图元索引
            Primitive* GpuPrimitives;               // 图元
            int primitiveCount;
            int totalNodes;   
            struct BVHBuildNode* gpuroot;
           // struct BVHBuildNode* cpuroot;
    };
}