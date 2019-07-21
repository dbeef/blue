#!/usr/bin/env bash
rm -r build-android
mkdir build-android
cd build-android

cmake .. \
-DCMAKE_PREFIX_PATH="/home/dbeef/Android/CrosscompiledLibs/bundle" \
-DCMAKE_TOOLCHAIN_FILE=/home/dbeef/Android/android-ndk-r15c/build/cmake/android.toolchain.cmake \
-DCMAKE_SYSTEM_NAME=Android \
-DCMAKE_SYSTEM_VERSION=15 \
-DANDROID_PLATFORM=android-15 \
-DCMAKE_ANDROID_ARCH_ABI=armv7-a \
-DCMAKE_ANDROID_NDK=/home/dbeef/Android/android-ndk-r15c \
-DCMAKE_ANDROID_STL_TYPE=c++_static \
-DANDROID_STL=c++_static \
-DCMAKE_MAKE_PROGRAM=make \
-DCMAKE_INSTALL_PREFIX=/home/dbeef/Android/android-ndk-r15c/sysroot/usr/ \

cmake --build . --config Release --target install -- -j12
