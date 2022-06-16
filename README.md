# A Haptics Experiment using Cornell University Game Library (CUGL)

Currently only works on iPhone using Core Haptics.
Might break on Android/Windows

Important Files
- `Haptics.cpp`, shows usage. lines 176 and 184 are important.
- `cugl/include/cugl/haptics/CUHaptics.h`
- `cugl/lib/haptics/CUHaptics.cpp`
- `cugl/lib/haptics/platform/CUHaptics-impl.h`, Bridge for platform specific haptics functionality.
- `cugl/lib/haptics/platform/CUHaptics-iOS.mm`, Objective-c implementation on iOS using CoreHaptics.

TODO
- SDL Haptics