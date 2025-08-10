#!/bin/bash
cp stb/ /usr/local/include/ -r


# glfw依赖libxinerama-dev	libxcursor-dev libxi-dev
apt install libxinerama-dev	
apt install libxcursor-dev
apt install libxi-dev
apt install libxrandr-dev

# curl依赖libssl-dev libpsl-dev
apt install libssl-dev
apt install libpsl-dev


# glfw
rm glfw glfw_build -rf
unzip glfw.zip
cmake -S glfw -B glfw_build
cd glfw_build
make -j32
make install

#cd .. 
#cd boost-1.85.0
#sudo sh ./bootstrap.sh
#sudo ./b2 install

# curl
cd .. 
rm curl-8_14_1 curl-8_14_1_build -rf
unzip curl-8_14_1.zip
cmake -S curl-8_14_1 -B curl-8_14_1_build
cd curl-8_14_1_build
make -j32
make install

# glm
cd .. 
rm glm-1.0.1 glm-1.0.1_build -rf
unzip glm-1.0.1.zip
cmake -S glm-1.0.1 -B glm-1.0.1_build
cd glm-1.0.1_build
make -j32
make install

# zlib
cd .. 
rm zlib-1.3.1 zlib-1.3.1_build -rf
unzip zlib-1.3.1.zip
cmake -S zlib-1.3.1 -B zlib-1.3.1_build
cd zlib-1.3.1_build
make -j32
make install

# libzip
cd .. 
rm libzip-1.11.4 libzip-1.11.4_build -rf
unzip libzip-1.11.4.zip
cmake -S libzip-1.11.4 -B libzip-1.11.4_build
cd libzip-1.11.4_build
make -j32
make install

# assimp-5.4.3 依赖zlib
cd .. 
cmake -S assimp-5.4.3 -B assimp-5.4.3_build
cd assimp-5.4.3_build
make -j32
make install

# double-conversion
cd .. 
rm double-conversion-3.3.1 double-conversion-3.3.1_build -rf
unzip double-conversion-3.3.1.zip
cmake -S double-conversion-3.3.1 -B double-conversion-3.3.1_build
cd double-conversion-3.3.1_build
make -j32
make install

# utf8proc
cd .. 
rm utf8proc-2.10.0 utf8proc-2.10.0_build -rf
unzip utf8proc-2.10.0.zip
cmake -S utf8proc-2.10.0 -B utf8proc-2.10.0_build
cd utf8proc-2.10.0_build
make -j32
make install

# libdeflate
cd .. 
rm libdeflate-1.18 libdeflate-1.18_build -rf
unzip libdeflate-1.18.zip
cmake -S libdeflate-1.18 -B libdeflate-1.18_build
cd libdeflate-1.18_build
make -j32
make install

# Imath
cd .. 
rm Imath-3.1.9 Imath-3.1.9_build -rf
unzip Imath-3.1.9.zip
cmake -S Imath-3.1.9 -B Imath-3.1.9_build
cd Imath-3.1.9_build
make -j32
make install

# openexr-3.2.1依赖libdeflate-1.18和Imath-3.1.9
cd .. 
rm openexr-3.2.1 openexr-3.2.1_build -rf
unzip openexr-3.2.1.zip
cmake -S openexr-3.2.1 -B openexr-3.2.1_build -D BUILD_TESTING=false
cd openexr-3.2.1_build
make -j32
make install

#cd .. 
#rm openexr openexr -rf
#unzip openexr.zip
#cmake -S openexr openexr_build -D BUILD_TESTING=false
#cd openexr_build
#make -j32
#make install
#cp OpenEXR/IlmImf/*so* /usr/local/lib/


# Sqlite3
cd .. 
rm sqlite-version-3.50.1 I -rf
unzip sqlite-version-3.50.1.zip
cd sqlite-version-3.50.1
./configure
make -j32
make install

# boost-1.85
cd .. 
#wget https://github.com/boostorg/boost/releases/download/boost-1.85.0/boost-1.85.0-cmake.zip
rm boost-1.85.0 boost-1.85.0_build -rf
unzip boost-1.85.0-cmake.zip
cd boost-1.85.0
chmod 777 ./bootstrap.sh ./tools/build/src/engine/build.sh
sh ./bootstrap.sh
./b2 install
make -j32
make install


wget https://imagemagick.org/download/releases/ImageMagick-6.9.12-0.tar.xz
tar -xf ImageMagick-6.9.12-0.tar.xz
cd ImageMagick-6.9.12-0

./configure --prefix=/usr/local --with-webp=yes --with-jpeg=yes --with-png=yes --with-tiff=yes --with-gif=yes --with-freetype=yes --with-fontconfig=yes
make -j32
sudo make install
sudo ldconfig /usr/local/lib