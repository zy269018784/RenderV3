del command.txt
set vulkan_path=D:\VulkanSDK\1.3.290.0\Bin
set glsl_path=E:\Work\project\global-illumination-dev\pbrt_optical_build_qt\asset\shaders\glsl\raytracing
echo start: 盘符d 路径p 文件名n 扩展名x>> command.txt

for /r %%i in (*.rgen) do (
%vulkan_path%\glslangValidator.exe --target-env vulkan1.2 %%i -o %%~ni_rgs.spv
echo %vulkan_path%\glslangValidator.exe --target-env vulkan1.2 %%i -o %%~ni_rgs.spv >> command.txt
)

for /r %%i in (*.rmiss) do (
%vulkan_path%\glslangValidator.exe --target-env vulkan1.2 %%i -o %%~ni_rms.spv
echo %vulkan_path%\glslangValidator.exe --target-env vulkan1.2 %%i -o %%~ni_rms.spv >> command.txt
)

for /r %%i in (*.rchit) do (
%vulkan_path%\glslangValidator.exe --target-env vulkan1.2 %%i -o %%~ni_rhs.spv
echo %vulkan_path%\glslangValidator.exe --target-env vulkan1.2 %%i -o %%~ni_rhs.spv >> command.txt
)

for /r %%i in (*.rahit) do (
%vulkan_path%\glslangValidator.exe --target-env vulkan1.2 %%i -o %%~ni_ras.spv
echo %vulkan_path%\glslangValidator.exe --target-env vulkan1.2 %%i -o %%~ni_ras.spv >> command.txt
)