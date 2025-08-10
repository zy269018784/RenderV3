linux安装glfw
apt-get install -y libx11-dev
apt-get install -y libxrandr-dev
apt-get install -y libxinerama-dev
apt-get install -y libxcursor-dev
apt-get install -y libxi-dev
cd glfw
cmake -S . -B build
cd build
make -j16
make install