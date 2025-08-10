#include <Primitive/BVHAggregate.h>
#include <Primitive/Primitive.h>
#include <Intersection/Intersection.h>
#include <Scene/Scene.h>
#include <Math/Transform.h>
#include <Util/Parallel.h>
#include <Init.h>
#include <iostream>
#include <algorithm>
using namespace std;
namespace Render {

    struct BVHPrimitive {
        BVHPrimitive() {}
        BVHPrimitive(size_t primitiveIndex, const Bounds3f& bounds)
            : primitiveIndex(primitiveIndex), bounds(bounds) {}
        size_t primitiveIndex;  // 图元索引
        Bounds3f bounds;        // 图元包围盒
        // BVHPrimitive Public Methods
        Point3f Centroid() const { return .5f * bounds.pMin + .5f * bounds.pMax; }
    };

    BVHAggregate::BVHAggregate(Primitive* primitives, int n, 
        SplitMethod splitMethod, Allocator* alloc) :
 
        GpuPrimitives(nullptr), gpuroot(nullptr), orderedPrimitiveIndices(nullptr), primitiveCount(n) {

       if (alloc == nullptr)
           return;

        if (n <= 0)
            return;

        // device内存
        GpuPrimitives = primitives;
        orderedPrimitiveIndices = alloc->allocate_object<unsigned int>(n);

        // host 内存
        std::vector<BVHPrimitive> bvhPrimitives(n);
         // Primitive -> BVHPrimitive
        for (size_t i = 0; i < n; ++i) {
           bvhPrimitives[i] = BVHPrimitive(i, primitives[i].Bounds());
        }
        // BVHBuildNode* root;
        totalNodes = 0;
        std::atomic<int> orderedPrimsOffset{0};

        gpuroot = buildRecursive(bvhPrimitives, 0, bvhPrimitives.size(), orderedPrimitiveIndices, &orderedPrimsOffset,  &totalNodes);

       //gpuroot = alloc->allocate_object<BVHBuildNode>(totalNodes);
       //if (gpuroot == nullptr) {
       //    cout << "gpuroot error\n";
       //}
       //int offset = 0;
       //flattenBVH(gpuroot, cpuroot, &offset);

        //FreeNode(root);
        //cout << "BVH nodes " << totalNodes << endl;
    }


    BVHAggregate::BVHAggregate(Primitive* p, int n, struct BVHBuildNode* root, unsigned int* orderedPrimitiveIndices) :
        GpuPrimitives(p), gpuroot(root), orderedPrimitiveIndices(orderedPrimitiveIndices) {
    
    }

    BVHAggregate* BVHAggregate::Create(std::vector<Primitive> prims) {

        int maxPrimsInNode = 4;
        return nullptr;
        //return new BVHAggregate(std::move(prims), maxPrimsInNode);
    }

    Bounds3f BVHAggregate::Bounds() const {
        if (gpuroot)
            return gpuroot->bounds;
        return {};
    }


    Optional<ShapeIntersection> BVHAggregate::Intersect(const Ray& ray, Float tMax) const {
        Vector3f invDir(1 / ray.d.x, 1 / ray.d.y, 1 / ray.d.z);
        int dirIsNeg[3] = { int(invDir.x < 0), int(invDir.y < 0), int(invDir.z < 0) };

        struct BVHBuildNode* nodeToVist[64];
        int top = -1;
        nodeToVist[++top] = gpuroot;
        Optional<ShapeIntersection> si = {};
        while (top >= 0) {
            struct BVHBuildNode* node = nodeToVist[top--];
            // tMax保存当前primitive的距离,  当前node的距离小于tMax才算相交.
            if (node->bounds.IntersectP(ray.o, ray.d, tMax, invDir, dirIsNeg)) {
                if (node->type == NodeType::Leaf) {
                    for (int i = node->firstPrimOffset; i < node->firstPrimOffset + node->nPrimitives; i++) {
                       // printf("BVHAggregate 3\n");
                        // 排序后的primitive索引
                        Optional<ShapeIntersection> ss = GpuPrimitives[orderedPrimitiveIndices[i]].Intersect(ray, tMax);
                        // 与primitive相相交,不退出循环, tMax实际上保存的是最小距离.
                        if (ss)
                        {
                            //printf("BVHAggregate 2\n");
                            si = ss;
                            tMax = ss->tHit;
                        }
                    }
                }
                else if (node->type == NodeType::Interior) {
                    if (dirIsNeg[node->splitAxis]) {
                        // 朝轴负方向看, 先遍历轴最大的
                        nodeToVist[++top] = node->children[0];
                        nodeToVist[++top] = node->children[1];
                    }
                    else {
                        // 朝轴正方向看,先遍历轴最小的
                        nodeToVist[++top] = node->children[1];
                        nodeToVist[++top] = node->children[0];
                    }
                }
            }
            else {
                //break;
            }
        }
        return si;
    }

    std::string BVHAggregate::ToString() const {
        return "";
    }

    Bool BVHAggregate::SplitMiddle(std::vector<BVHPrimitive>& bvhPrimitives, int first, int n, int &mid, int dim,  Float pmid)
    {
        auto midIter = std::partition(bvhPrimitives.begin() + first, bvhPrimitives.begin() + first + n,
            [dim, pmid](const BVHPrimitive& pi) {
                return pi.Centroid()[dim] < pmid;
            });
        mid = midIter - bvhPrimitives.begin();
        if ((midIter != bvhPrimitives.begin() + first) && (midIter != bvhPrimitives.begin() + (n - 1)))
            return true;
        return false;
    }


    void BVHAggregate::SplitEqualCounts(std::vector<BVHPrimitive>& bvhPrimitives, int first, int n, int& mid, int dim)
    {
        mid = first + n / 2;
        // 升序排列
        std::nth_element(bvhPrimitives.begin() + first, bvhPrimitives.begin() + mid, bvhPrimitives.begin() + first + n,
            [dim](const BVHPrimitive& a, const BVHPrimitive& b) {
                return a.Centroid()[dim] < b.Centroid()[dim];
            });
    }



    BVHBuildNode* BVHAggregate::buildRecursive(std::vector<BVHPrimitive>& bvhPrimitives, int first, int n,
        //Primitive* primitives, Primitive* orderedPrimitives,
        unsigned int* orderedPrimitiveIndices, std::atomic<int>* orderedPrimitivesOffset,
        int* totalNodes)
    {
        BVHBuildNode* node = new BVHBuildNode;
        if (node == nullptr) {
            cout << "out of memory\n";
            return nullptr;
        }
        totalNodes[0]++;
       //totalNodes->fetch_add(1);

        Bounds3f bounds;
        // 求first开始n个primitive的包围盒
        for (auto prim = (bvhPrimitives.begin() + first); prim < bvhPrimitives.begin() + first + n; prim++) {
            bounds = Union(bounds, prim->bounds);
        }

        if (bounds.SurfaceArea() == 0 || n == 1) {
            int firstPrimOffset = orderedPrimitivesOffset->fetch_add(n);
            for (size_t i = first; i < first + n; ++i) {
                orderedPrimitiveIndices[firstPrimOffset + i - first] = bvhPrimitives[i].primitiveIndex;
            }
            node->InitLeaf(firstPrimOffset, n, bounds);
            return node;
        }
        else {
            // 结点包围盒
            Bounds3f centroidBounds;
            for (auto prim = bvhPrimitives.begin() + first; prim < bvhPrimitives.begin() + first + n; prim++) {
                centroidBounds = Union(centroidBounds, prim->Centroid());
            }     
            // 最大轴
            int dim = centroidBounds.MaxDimension();
            // 如果所有primitive的包围盒的最大轴范围相同, 放在一个结点中.
            if (centroidBounds.pMax[dim] == centroidBounds.pMin[dim]) {
                int firstPrimOffset = orderedPrimitivesOffset->fetch_add(n);
                for (size_t i = first; i < first + n; ++i) {
                    orderedPrimitiveIndices[firstPrimOffset + i - first] = bvhPrimitives[i].primitiveIndex;
                }
                // n个结点
                node->InitLeaf(firstPrimOffset, n, bounds);
                return node;
            }
            else
            {
                Float pmid = (centroidBounds.pMin[dim] + centroidBounds.pMax[dim]) / 2;
                int mid = -1;
                if (!SplitMiddle(bvhPrimitives, first, n, mid, dim, pmid)) {
                    SplitEqualCounts(bvhPrimitives, first, n, mid, dim);
                }
#if 0
                BVHBuildNode* children[2];
                ParallelFor(0, 2, [&](int i) {
                    //cout << i << " ";
                    if (i == 0)
                        children[0] = buildRecursive(bvhPrimitives, first, mid - first, primitives, GpuPrimitives, orderedPrimitiveIndices, orderedPrimitivesOffset, totalNodes);
                    else
                        children[1] = buildRecursive(bvhPrimitives, mid, first + n - mid, primitives, GpuPrimitives, orderedPrimitiveIndices, orderedPrimitivesOffset, totalNodes);
                    });
#else
                // 子结点
                BVHBuildNode* children[2];
                //children[0] = buildRecursive(bvhPrimitives, first, mid - first, primitives, GpuPrimitives, orderedPrimitiveIndices, orderedPrimitivesOffset, totalNodes);
                //children[1] = buildRecursive(bvhPrimitives, mid, first + n - mid, primitives, GpuPrimitives, orderedPrimitiveIndices, orderedPrimitivesOffset, totalNodes);
                children[0] = buildRecursive(bvhPrimitives, first, mid - first,  orderedPrimitiveIndices, orderedPrimitivesOffset, totalNodes);
                children[1] = buildRecursive(bvhPrimitives, mid, first + n - mid, orderedPrimitiveIndices, orderedPrimitivesOffset, totalNodes);

#endif

                // 设置结点
                node->InitInterior(first, n, dim, children[0], children[1]);

            }
        }
        return node;
    }

    int BVHAggregate::flattenBVH(BVHBuildNode* linearNode1, BVHBuildNode* node, int* offset) {
        BVHBuildNode* linearNode = &linearNode1[*offset];
        int nodeOffset = (*offset)++;

        linearNode->bounds = node->bounds;
        linearNode->firstPrimOffset = node->firstPrimOffset;
        linearNode->nPrimitives = node->nPrimitives;
        linearNode->type = node->type;
        linearNode->splitAxis = node->splitAxis;
        if (node->type == NodeType::Leaf) {
            linearNode->children[0] = linearNode->children[1] = nullptr;
        }
        else {
            flattenBVH(linearNode1, node->children[0], offset);
            int secondChildOffset = flattenBVH(linearNode1, node->children[1], offset);
            linearNode->children[0] = &linearNode1[nodeOffset + 1];
            linearNode->children[1] = &linearNode1[secondChildOffset];
        }
        return nodeOffset;
    }
}