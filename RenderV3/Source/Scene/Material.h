#pragma once
#include <glm/glm.hpp>
namespace Rt 
{
    /*
        p2.x: texture id
        p2.y: normal map id
        p2.z: material type
        p3.xyz = RGB
        p4.x: old texture id
        p4.y: old normal map id
    */

    /*
        玻璃材质
        p1.x: reflectivity
        p1.y: transmissivity;
        p2.x: texture id
        p2.y: normal map id
        p2.z: material type
        p3.xyz = RGB
    */
    /*
        导体材质
        p1.x: uRoughness
        p1.y: vRoughness;
        p1.z: remaproughness;
        p2.x: texture id
        p2.y: normal map id
        p2.z: material type
        p3.xyz = RGB
    */
   /*
        非金属材质
        p1.x: uRoughness
        p1.y: vRoughness;
        p1.z: remaproughness;
        p1.w: reflectivity;
        p2.x: texture id
        p2.y: normal map id
        p2.z: material type
        p3.xyz = RGB
    */
    /*
        disney材质
        p1.x : roughness;	p1.y : metallic;	p1.z : specular;	p1.w : specularTint;
        p2.x : texture id;	p2.y : normal map id;	p2.z : material type;	p2.w : bsdf_props;
        p3.xyz : albedo; 	p3.w : transmission;
        p4.xyz : k;		p4.w : ior;
        p5.x : sheen;	p5.y : sheenTint;	p5.z : clearcoat;	p5.w : clearcoatGloss;
        p6.x : subsurface;		p6.y : flatness;		p6.z : anisotropy; 		p6.w : null;
        p7.xyz : emission;		p7.w : emissionTint;
        p8.x : roughness map id;	p8.y : metallic map id；	p8.z : height map id;	p8.w : null;
    */
    /*
        幻彩铝板1材质:
            p1.x: uRoughness
            p1.y: vRoughness;
            p1.z: remaproughness;
            p2.x: texture id
            p2.y: normal map id
            p2.z: material type
            p3.xyz = RGB
    */

    enum class MaterialType : int
    {
        Diffuse = 1,
        Glass = 2,
        Conductor = 3,
        Dielectric = 4,
        Disney = 5,
        /*
            幻彩铝板
        */
        ColoredAluminumPlate1,
        ColoredAluminumPlate2,
        ColoredAluminumPlate3,
        ColoredAluminumPlate4,
        ColoredAluminumPlate5,
        ColoredAluminumPlate6,
        ColoredAluminumPlate7,
        ColoredAluminumPlate8,
        ColoredAluminumPlate9,
        ColoredAluminumPlate10,
    };

    // BSDF Props
    #define BSDF_FLAG_DIFFUSE 		1 << 0
    #define BSDF_FLAG_SPECULAR 		1 << 1
    #define BSDF_FLAG_GLOSSY 		1 << 2
    #define BSDF_FLAG_REFLECTION 	1 << 3
    #define BSDF_FLAG_TRANSMISSION 	1 << 4

    #define BSDF_FLAG_DIFFUSE_REFLECTION 	BSDF_FLAG_DIFFUSE | BSDF_FLAG_REFLECTION
    #define BSDF_FLAG_DIFFUSE_TRANSMISSION 	BSDF_FLAG_DIFFUSE | BSDF_FLAG_TRANSMISSION
    #define BSDF_FLAG_SPECULAR_REFLECTION 	BSDF_FLAG_SPECULAR | BSDF_FLAG_REFLECTION
    #define BSDF_FLAG_SPECULAR_TRANSMISSION BSDF_FLAG_SPECULAR | BSDF_FLAG_TRANSMISSION

    #define BSDF_FLAG_ALL BSDF_FLAG_DIFFUSE | BSDF_FLAG_SPECULAR | BSDF_FLAG_REFLECTION | BSDF_FLAG_TRANSMISSION

    class RtMaterial
    {
    public:
        RtMaterial()
        {
           p1 = glm::vec4(0);
           p2 = glm::ivec4(0);
           p3 = glm::vec4(0);
           p4 = glm::vec4(0);
           p5 = glm::vec4(0);
           p6 = glm::vec4(0);
           p7 = glm::vec4(0);
           p8 = glm::ivec4(0);

           SetTextureID(-1);
        }


        void SetMaterialType(int Type)
        {
            p2.z = Type;
        }

        void SetAlbedo(glm::vec4 a)
        {
            p3 = a;
        }

        void SetAlbedo(glm::vec3 a)
        {
            p3.x = a.x;
            p3.y = a.y;
            p3.z = a.z;
        }

        glm::vec4 GetAlbedo()
        {
            return p3;
        }

        int GetTextureID()
        {
            return p2.x;
        }

        void SetTextureID(int ID)
        {
            p2.x = ID;
        }

        float GetGlassReflectivity(float r)
        {
           return  p1.x;
        }

        void SetGlassReflectivity(float r)
        {
            p1.x = r;
        }

        float GetGlassTransmissivity()
        {
            return p1.y;
        }

        void SetGlassTransmissivity(float r)
        {
            p1.y = r;
        }

        float GetConductorURoughness()
        {
            return p1.x;
        }

        void SetConductorURoughness(float r)
        {
            p1.x = r;
        }

        float GetConductorVRoughness()
        {
            return p1.y;
        }

        void SetConductorVRoughness(float r)
        {
            p1.y = r;
        }



        float GetDielectricReflectivity()
        {
            return p1.w;
        }

        void SetDielectricReflectivity(float r)
        {
            p1.w = r;
        }

        ////////////////////////////////////Disney
        void SetDisneyRoughness(float v) {
            p1.x = v;
        }

        void SetDisneyMetallic(float v) {
            p1.y = v;
        }

        void SetDisneySpecular(float v) {
            p1.z = v;
        }

        void SetDisneySpecularTint(float v) {
            p1.w = v;
        } 

        //p2
        void SetDisneyAlbedoMap(int ID) {
            p2.x = ID;
        }

        void SetDisneyNormalMap(int ID) {
            p2.y = ID;
        }

        void SetDisneyBSDFProperties(int bsdf_props) {
            p2.w = bsdf_props;
        }

        //p3
        void SetDisneyAlbedo(glm::vec3 v) {
            p3.x = v.x;
            p3.y = v.y;
            p3.z = v.z;
        }

        void SetDisneyTransmission(float v) {
            p3.w = v;
        }

        //p4
        void SetDisneyK(glm::vec3 v) {
            p4.x = v.x;
            p4.y = v.y;
            p4.z = v.z;
        }

        void SetDisneyIOR(float v) {
            p4.w = v;
        }

        //p5
        void SetDisneySheen(float v) {
            p5.x = v;
        } 

        void SetDisneySheenTint(float v) {
            p5.y = v;
        }

        void SetDisneyClearcoat(float v) {
            p5.z = v;
        }

        void SetDisneyClearcoatGloss(float v) {
            p5.w = v;
        }

        void SetDisneyClearcoatRoughness(float v) {
            p5.w = 1.0 - v;
        }

        //p6
        void SetDisneySubsurface(float v) {
            p6.x = v;
        }

        void SetDisneyFlatness(float v) {
            p6.y = v;
        }

        void SetDisneyAnisotropy(float v) {
            p6.z = v;
        }

        void SetDisneyAtDistance(float v) { //暂时不用
            p6.w = v;
        }

        //p7
        void SetDisneyEmission(glm::vec3 v) {
            p7.x = v.x;
            p7.y = v.y;
            p7.z = v.z;
        }

        void SetDisneyEmissionTint(float v) {
            p7.w = v;
        }

        //p8
        //int GetDisneyRoughnessMap(Material m) {
        //    return m.p8.x;
        //} 
        //int GetDisneyMetallicMap(Material m) {
        //    return m.p8.y;
        //} 
        //int GetDisneyHeightMap(Material m) {
        //    return m.p8.z;
        //} 
        void SetDisneyThin(int v) {
            p8.w = v;
        }
        glm::vec4 p1;
        glm::ivec4 p2;
        glm::vec4 p3;
        glm::vec4 p4;
        glm::vec4 p5;
        glm::vec4 p6;
        glm::vec4 p7;
        glm::ivec4 p8;
        /*
            材质名
        */
        std::string name_;
        /*
            材质类型
        */
        std::string type_;
        /*
            纹理文件名
        */
        std::string textureName_;
        /*
           法线贴图文件名
        */
        std::string normalMap_;
        /*
           粗糙度贴图文件名
        */
        std::string roughnessMap_;
        /*
           金属度贴图文件名
        */
        std::string metallicMap_;
        /*
           高度（置换）贴图文件名
        */
        std::string heightMap_;

    };

    struct Material {
    public:
        Material() {

        }

        Material(const Rt::RtMaterial* rtMaterial) {
            this->p1 = rtMaterial->p1;
            this->p2 = rtMaterial->p2;
            this->p3 = rtMaterial->p3;
            this->p4 = rtMaterial->p4;
            this->p5 = rtMaterial->p5;
            this->p6 = rtMaterial->p6;
            this->p7 = rtMaterial->p7;
            this->p8 = rtMaterial->p8;
            //std::cout << "p1 " << p1.x << " " << p1.y << " " << p1.z << " " << p1.w << std::endl;
            //std::cout << "p2 " << p2.x << " " << p2.y << " " << p2.z << " " << p2.w << std::endl;
            //std::cout << "p3 " << p3.x << " " << p3.y << " " << p3.z << " " << p3.w << std::endl;
            //std::cout << "p4 " << p4.x << " " << p4.y << " " << p4.z << " " << p4.w << std::endl;
            //std::cout << "p5 " << p5.x << " " << p5.y << " " << p5.z << " " << p5.w << std::endl;
            //std::cout << "p6 " << p6.x << " " << p6.y << " " << p6.z << " " << p6.w << std::endl;
            //std::cout << "p7 " << p7.x << " " << p7.y << " " << p7.z << " " << p7.w << std::endl;
            //std::cout << "p8 " << p8.x << " " << p8.y << " " << p8.z << " " << p8.w << std::endl;
        }

        Material(const Rt::RtMaterial& rtMaterial) {
            this->p1 = rtMaterial.p1;
            this->p2 = rtMaterial.p2;
            this->p3 = rtMaterial.p3;
            this->p4 = rtMaterial.p4;
            this->p5 = rtMaterial.p5;
            this->p6 = rtMaterial.p6;
            this->p7 = rtMaterial.p7;
            this->p8 = rtMaterial.p8;
        }

        Material& operator=(const Rt::RtMaterial& rtMaterial)//赋值运算符
        {
            this->p1 = rtMaterial.p1;
            this->p2 = rtMaterial.p2;
            this->p3 = rtMaterial.p3;
            this->p4 = rtMaterial.p4;
            this->p5 = rtMaterial.p5;
            this->p6 = rtMaterial.p6;
            this->p7 = rtMaterial.p7;
            this->p8 = rtMaterial.p8;
            return *this;
        }

        int GetMaterialType() const
        {
            return p2.z;
        }

        void SetMaterialType(int Type)
        {
            p2.z = Type;
        }

        glm::vec3 GetAlbedo() const
        {
            return p3;
        }

        void SetAlbedo(glm::vec3 a)
        {
            p3.x = a.x;
            p3.y = a.y;
            p3.z = a.z;
        }

        int GetTextureID()
        {
            return p2.x;
        }

        void SetTextureID(int ID)
        {
            p2.x = ID;
        }

        int GetOldTextureID()
        {
            return p4.x;
        }

        void SetOldTextureID(int ID)
        {
            p4.x = ID;
        }

        int GetNormalMapID()
        {
            return p2.y;
        }

        void SetNormalMapID(int ID)
        {
            p2.y = ID;
        }

        void SetRoughnessMapID(int ID)
        {
            p8.x = ID;
        }

        void SetMetallicMapID(int ID)
        {
            p8.y = ID;
        }

        int GetOldNormalMapID()
        {
            return p4.y;
        }

        void SetOldNormalMapID(int ID)
        {
            p4.y = ID;
        }

        float GetGlassReflectivity() const
        {
            return p1.x;
        }

        void SetGlassReflectivity(float r)
        {
            p1.x = r;
        }

        float GetGlassTransmissivity() const
        {
            return p1.y;
        }

        void SetGlassTransmissivity(float r)
        {
            p1.y = r;
        }

        float GetConductorURoughness()
        {
            return p1.x;
        }

        void SetConductorURoughness(float r)
        {
            p1.x = r;
        }

        float GetConductorVRoughness()
        {
            return p1.y;
        }

        void SetConductorVRoughness(float r)
        {
            p1.y = r;
        }

        float GetDielectricReflectivity()
        {
            return p1.w;
        }

        void SetDielectricReflectivity(float r)
        {
            p1.w = r;
        }

        ////////////////////////////////////Disney
        void SetDisneyRoughness(float v) {
            p1.x = v;
        }

        void SetDisneyMetallic(float v) {
            p1.y = v;
        }

        void SetDisneySpecular(float v) {
            p1.z = v;
        }

        void SetDisneySpecularTint(float v) {
            p1.w = v;
        }

        //p2
        void SetDisneyAlbedoMap(int ID) {
            p2.x = ID;
        }

        void SetDisneyNormalMap(int ID) {
            p2.y = ID;
        }

        //p3
        void SetDisneyAlbedo(glm::vec3 v) {
            p3.x = v.x;
            p3.y = v.y;
            p3.z = v.z;
        }

        void SetDisneyTransmission(float v) {
            p3.w = v;
        }

        //p4
        void SetDisneyK(glm::vec3 v) {
            p4.x = v.x;
            p4.y = v.y;
            p4.z = v.z;
        }

        void SetDisneyIOR(float v) {
            p4.w = v;
        }

        //p5
        void SetDisneySheen(float v) {
            p5.x = v;
        }

        void SetDisneySheenTint(float v) {
            p5.y = v;
        }

        void SetDisneyClearcoat(float v) {
            p5.z = v;
        }

        void SetDisneyClearcoatGloss(float v) {
            p5.w = v;
        }

        void SetDisneyClearcoatRoughness(float v) {
            p5.w = 1.0 - v;
        }

        //p6
        void SetDisneySubsurface(float v) {
            p6.x = v;
        }

        void GetDisneyFlatness(float v) {
            p6.y = v;
        }

        void SetDisneyAnisotropy(float v) {
            p6.z = v;
        }

        void SetDisneyAtDistance(float v) { //暂时不用
            p6.w = v;
        }

        //p7
        void SetDisneyEmission(glm::vec3 v) {
            p7.x = v.x;
            p7.y = v.y;
            p7.z = v.z;
        }

        void SetDisneyEmissionTint(float v) {
            p7.w = v;
        }

        //p8
        //int GetDisneyRoughnessMap(Material m) {
        //    return m.p8.x;
        //} 
        //int GetDisneyMetallicMap(Material m) {
        //    return m.p8.y;
        //} 
        //int GetDisneyHeightMap(Material m) {
        //    return m.p8.z;
        //} 
        

        glm::vec4 p1;
        glm::ivec4 p2;
        glm::vec4 p3;
        glm::vec4 p4;
        glm::vec4 p5;
        glm::vec4 p6;
        glm::vec4 p7;
        glm::ivec4 p8;
    };
}
