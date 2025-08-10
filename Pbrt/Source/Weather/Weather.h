#pragma once
#include <cpu_gpu.h>
#include <Type/TypeDeclaration.h>
#include <Texture/RGB.h>
namespace Render {
	class Weather {
	public:
		enum WeatherType {
			Sunny,				// 晴天
			Cloudy,				// 多云
			Overcast,			// 阴天
			Rainy,				// 雨天
			Windy,				// 起风
			Snowy,				// 雪
			RainStorm,			// 暴雨
			ThunderStorm,		// 雷雨
			SandStorm,			// 沙尘暴
			Foggy,				// 雾
			Frosty,				// 霜冻
			Sleet,				// 雨夹雪
			Typhoon				// 台风
		};
	public:
		CPU_GPU Weather(enum WeatherType Type = Sunny);
		CPU_GPU void SetSkyBaseColor(RGB c);
	public:
		CPU_GPU RGB SkyColor(Vector3f d);
	private:
		enum WeatherType WeatherType;
		RGB SkyBaseColor;
	};
}