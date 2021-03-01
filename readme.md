# HalideBlurEvaluate
Evaluate performance of Halide, and compare with OpenCV.

It's also an example about how to write a Halide generator, how to generate libs for multiple targets, and how to use the generated libs with CMake.



## Build the project

### Build BlurGenerator on PC

Run commands below (in the root dir of this repository) to build BlurGenerator:

```shell
cmake -DCMAKE_BUILD_TYPE=release -SBlurGenerator -Bbuild-Generator
cmake --build build-Generator
```

The CMake script for BlurGenerator not only builds the generator, but also calls the generator to generate libraries for different targets. You can find them in `build-Generator/libs`:  
- host
- x86-64-linux-opencl
- arm-64-android
- arm-64-android-opencl

### Build BlurEval for PC

Build BlurEval with the generated libs:

```shell
cmake -DCMAKE_BUILD_TYPE=release -SBlurEval -Bbuild-host-opencl -DBlur_DIR=build-Generator/libs/x86-64-linux-opencl
cmake --build build-host-opencl
```

and run it to see the result:

```shell
./build-host-opencl/blur_eval gray.png
```

### Build BlurEval for Android

Set `ANDROID_TOOLCHAIN` to the NDK toolchain file (absolute path of android.toolchain.cmake), and `ANDROID_OPENCV_DIR` to OpenCV config file (absolute directory where OpenCVConfig.cmake is in) in advance.

```shell
cmake -DCMAKE_BUILD_TYPE=release -SBlurEval -Bbuild-android -DBlur_DIR=build-Generator/libs/arm-64-android -DCMAKE_TOOLCHAIN_FILE=$ANDROID_TOOLCHAIN -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=16 -DOpenCV_DIR=$ANDROID_OPENCV_DIR
cmake --build build-android
```

Run on Android:

```shell
adb push build-android/blur_eval /data/local/tmp
adb push gray.png /data/local/tmp
adb shell chmod +x /data/local/tmp/blur_eval
adb shell /data/local/tmp/blur_eval /data/local/tmp/gray.png
adb pull /data/local/tmp/blur_halide.png
```

