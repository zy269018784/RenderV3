glslangValidator.exe --target-env vulkan1.2 rtscene.rgen  -o ..\..\rtscene\rtscene_rgs.spv
glslangValidator.exe --target-env vulkan1.2 rtscene.rmiss -o ..\..\rtscene\rtscene_rms.spv
glslangValidator.exe --target-env vulkan1.2 shadow.rmiss  -o ..\..\rtscene\shadow_rms.spv 
glslangValidator.exe --target-env vulkan1.2 rtscene.rchit -o ..\..\rtscene\rtscene_rhs.spv
glslangValidator.exe --target-env vulkan1.2 rtscene.rahit -o ..\..\rtscene\rtscene_ras.spv
echo finish, press any key to exit...
pause >nul