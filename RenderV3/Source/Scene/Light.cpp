#include <Scene/Light.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>
#include <iostream>
#include <Math/Transform.h>
#include <Util/Frame.h>
glm::vec3 MulPoint(glm::mat4 m, glm::vec4 p)
{
	float xp = m[0][0] * p.x + m[1][0] * p.y + m[2][0] * p.z + m[3][0];
	float yp = m[0][1] * p.x + m[1][1] * p.y + m[2][1] * p.z + m[3][1];
	float zp = m[0][2] * p.x + m[1][2] * p.y + m[2][2] * p.z + m[3][2];
	float wp = m[0][3] * p.x + m[1][3] * p.y + m[2][3] * p.z + m[3][3];
	if (wp == 1)
		return glm::vec3(xp, yp, zp);
	else
		return glm::vec3(xp, yp, zp) / wp;
}

void Rt::Light::CreatePointLight(glm::vec3 Color, float Power, float Range, glm::vec3 From)
{
	SetColor(Color);
	SetPower(Power);
	SetRange(Range);
	SetDirection(From, From);
	SetLightType(Light::Type::Point);
	glm::mat4 m = glm::translate(glm::mat4(1, 0, 0, 0,
		                         0, 1, 0, 0,
		                         0, 0, 1, 0,
		                         0, 0, 0, 1), From);
	m = glm::mat4(1, 0, 0, From.x,
		0, 1, 0, From.y,
		0, 0, 1, From.z,
		0, 0, 0, 1);
	m = glm::transpose(m);
	SetWorldFromLight(m);
}

void Rt::Light::CreateDistantLight(glm::vec3 Color, float Power, glm::vec3 From, glm::vec3 To)
{
	SetColor(Color);
	SetPower(Power);
	SetLightType(Light::Type::Distant);
	SetDirection(From, To);
	glm::mat4 m;

	Point3f to(To.x, To.y, To.z);
	Point3f from(From.x, From.y, From.z);
	Transform dirToZ = (Transform)Frame::FromZ(Normalize(to - from));
	Transform worldFromLight = Translate(Vector3f(from.x, from.y, from.z)) * Inverse(dirToZ);


	//std::cout << " worldFromLight " << std::endl;
	//showTransform(worldFromLight);
	//Vector3f wLight = worldFromLight(Vector3f(0, 0, 1));
	//std::cout << "===================== wLight ??????? "
	//	<< wLight.x << " "
	//	<< wLight.y << " "
	//	<< wLight.z << " "
	//	<< std::endl;

	for (int i = 0; i < 4; i++)
	{
		m[i][0] = worldFromLight.GetMatrix()[0][i];
		m[i][1] = worldFromLight.GetMatrix()[1][i];
		m[i][2] = worldFromLight.GetMatrix()[2][i];
		m[i][3] = worldFromLight.GetMatrix()[3][i];
	}

	//glm::vec3 wLight2 = m * glm::vec4(0, 0, 1, 0);
	//std::cout << "===================== wLight2 ??????? "
	//	<< wLight2.x << " "
	//	<< wLight2.y << " "
	//	<< wLight2.z << " "
	//	<< std::endl;
}

void Rt::Light::CreateSpotLight(glm::vec3 Color, float Power, float Range, glm::vec3 From, glm::vec3 To, float totalWidth, float falloffStart)
{
	SetColor(Color);
	SetPower(Power);
	SetLightType(Light::Type::Spot);
	SetRange(Range);
	SetDirection(From, To);
	float CosFalloffEnd = std::cos(glm::radians(totalWidth));
	float CosFalloffStart = std::cos(glm::radians(falloffStart));
	SetFalloff(CosFalloffStart, CosFalloffEnd);

	glm::mat4 m;

	Point3f to(To.x, To.y, To.z);
	Point3f from(From.x, From.y, From.z);
	Transform dirToZ = (Transform)Frame::FromZ(Normalize(to - from));
	Transform worldFromLight = Translate(Vector3f(from.x, from.y, from.z)) * Inverse(dirToZ);

	for (int i = 0; i < 4; i++)
	{
		m[i][0] = worldFromLight.GetMatrix()[0][i];
		m[i][1] = worldFromLight.GetMatrix()[1][i];
		m[i][2] = worldFromLight.GetMatrix()[2][i];
		m[i][3] = worldFromLight.GetMatrix()[3][i];
	}
	SetWorldFromLight(m);
}

void Rt::Light::CreateHDRLight(float Phi, float Theta, float Power)
{
	SetPower(Power);
}

void Rt::Light::CreateAreaLight(glm::vec3 Color, float Power, int MeshIndex)
{
	SetColor(Color);
	SetPower(Power);
	SetMeshIndex(MeshIndex);
}

void Rt::Light::SetColor(glm::vec3 Color)
{
	p1.r = Color.r;
	p1.g = Color.g;
	p1.b = Color.b;
}

void Rt::Light::SetPower(float Power)
{
	p1.a = Power;
}

void Rt::Light::SetRange(float Range)
{
	p3.a = Range;
}

void Rt::Light::SetMeshIndex(int Index)
{
	p2.g = Index;
}

void Rt::Light::SetFalloff(float CosFalloffStart, float CosFalloffEnd)
{
	p5.r = CosFalloffStart;
	p5.g = CosFalloffEnd;
}

void Rt::Light::SetLightType(Light::Type Type)
{
	p2.r = static_cast<int>(Type);
}

void Rt::Light::SetLightVisible(bool visible/* = true*/)
{
	p2.g = visible ? 1 : 0;
}

void Rt::Light::SetDirection(glm::vec3 From, glm::vec3 To)
{
	p3.r = From.r;
	p3.g = From.g;
	p3.b = From.b;

	p4.r = To.r;
	p4.g = To.g;
	p4.b = To.b;
}

void Rt::Light::SetWorldFromLight(glm::mat4 InWorldFromLight)
{
	this->WorldFromLight = InWorldFromLight;
}