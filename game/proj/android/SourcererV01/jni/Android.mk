# Copyright (C) 2010 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH := $(call my-dir)

############################################################################
# OpenAL
############################################################################

#include $(CLEAR_VARS)
#LOCAL_MODULE    := openal
#LOCAL_SRC_FILES := libopenal.a
#include $(PREBUILT_STATIC_LIBRARY)

#--------------------------------------------------------------------------

include $(CLEAR_VARS)
LOCAL_MODULE := openal
#LOCAL_ARM_MODE := arm
LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../../../../src/android/library/include \
	$(LOCAL_PATH)/../../../../src/android/library/src/OpenAl/OpenAL32/Include \
	$(LOCAL_PATH)
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
LOCAL_SRC_FILES := $(wildcard ../../../../src/android/library/src/OpenAl/*.c)
##LOCAL_SRC_FILES := $(subst ../../../,../../../../,$(LOCAL_SRC_FILES))
#LOCAL_SRC_FILES := $(subst ../../../src,,$(LOCAL_SRC_FILES))
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
LOCAL_CFLAGS     := -ffast-math -DAL_BUILD_LIBRARY -DAL_ALEXT_PROTOTYPES -D_ANDROID
LOCAL_LDLIBS     := -llog -Wl,-s -lOpenSLES
include $(BUILD_STATIC_LIBRARY)
#include $(BUILD_SHARED_LIBRARY)

############################################################################
# External Library
############################################################################

#include $(CLEAR_VARS)
#LOCAL_MODULE    := external
#LOCAL_SRC_FILES := libexternal.a
#include $(PREBUILT_STATIC_LIBRARY)

#--------------------------------------------------------------------------

include $(CLEAR_VARS)
LOCAL_MODULE := external
LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../../../../src/common/library/include \
	$(LOCAL_PATH)
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
LOCAL_SRC_FILES := $(wildcard ../../../../src/common/library/src/ -name '*.c')
#LOCAL_SRC_FILES := $(subst ../../../,../../../../,$(LOCAL_SRC_FILES))
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
LOCAL_LDLIBS := -llog -landroid -lEGL -lGLESv1_CM
LOCAL_CFLAGS := -D_ANDROID
include $(BUILD_STATIC_LIBRARY)
#include $(BUILD_SHARED_LIBRARY)


############################################################################
# android_native_app_glue
############################################################################

include $(CLEAR_VARS)
LOCAL_MODULE    := android_native_app_glue
LOCAL_SRC_FILES := libandroid_native_app_glue.a
include $(PREBUILT_STATIC_LIBRARY)


############################################################################
# Sourcerer01
############################################################################

include $(CLEAR_VARS)
LOCAL_MODULE := sourcerer01
LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../../../../src/android/library/include \
	$(LOCAL_PATH)/../../../../src/common/library/include \
	$(LOCAL_PATH)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

LOCAL_SRC_FILES := $(wildcard ../../../../src/android/*.cpp)
LOCAL_SRC_FILES += $(wildcard ../../../../src/tfw/*.cpp)
#LOCAL_SRC_FILES += $(wildcard ../../../../src/basicmodel/*.cpp)
LOCAL_SRC_FILES += $(wildcard ../../../../src/etk/*.cpp)
LOCAL_SRC_FILES += $(wildcard ../../../../src/sourcerer/*.cpp)
#LOCAL_SRC_FILES := $(subst ../../../src,,$(LOCAL_SRC_FILES))
#LOCAL_SRC_FILES := $(subst ../../../src,../../../../src,$(LOCAL_SRC_FILES))
#LOCAL_SRC_FILES := ../../../../src/android/main.cpp
#LOCAL_SRC_FILES += $(wildcard ../../../src/common/library/src/*.c)

#### peculiar to android files
#LOCAL_SRC_FILES := $(wildcard ../../../src/tfw/txt/*.cpp)
##LOCAL_SRC_FILES := $(subst ../../../,../../../../,$(LOCAL_SRC_FILES))
#LOCAL_SRC_FILES += ../../../../src/android/DebugAndroid.cpp
#LOCAL_SRC_FILES += ../../../../src/android/OsDepModAndroid.cpp
#LOCAL_SRC_FILES += ../../../../src/android/StopWatchAndroid.cpp
#LOCAL_SRC_FILES += ../../../../src/etk/body/OsDepMod.cpp
#LOCAL_SRC_FILES += ../../../../src/tfw/common/OsModule.cpp
#LOCAL_SRC_FILES += ../../../../src/tfw/debug/TfwDebug.cpp
#LOCAL_SRC_FILES += ../../../../src/tfw/file/File.cpp
#LOCAL_SRC_FILES += ../../../../src/tfw/file/FileAndroidAsset.cpp
#LOCAL_SRC_FILES += ../../../../src/tfw/lib/TmpBuf.cpp
#LOCAL_SRC_FILES += ../../../../src/tfw/string/CcString.cpp
#LOCAL_SRC_FILES += ../../../../src/tfw/string/CvString.cpp
#LOCAL_SRC_FILES += ../../../../src/tfw/string/WcvString.cpp
#LOCAL_SRC_FILES += ../../../../src/android/main.cpp

#### min composition files
#LOCAL_SRC_FILES += ../../../../src/tfw/debug/TfwDebug.cpp
#LOCAL_SRC_FILES += ../../../../src/tfw/string/CvString.cpp
#LOCAL_SRC_FILES += ../../../../src/android/DebugAndroid.cpp
#LOCAL_SRC_FILES += ../../../../src/android/main.cpp

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

LOCAL_LDLIBS := -llog -landroid -lEGL -lGLESv1_CM -lOpenSLES

LOCAL_STATIC_LIBRARIES := android_native_app_glue libopenal libexternal
#LOCAL_STATIC_LIBRARIES := android_native_app_glue libopenal libexternal
#LOCAL_STATIC_LIBRARIES := android_native_app_glue
#LOCAL_SHARED_LIBRARIES := libopenal libexternal

##LOCAL_CFLAGS := -fsigned-char -D_ANDROID -D_DEBUG -D_BASICMODEL
LOCAL_CFLAGS := -fsigned-char -D_ANDROID -D_DEBUG -D_ENLIGHT_V01

include $(BUILD_SHARED_LIBRARY)

#!#$(call import-module,android/native_app_glue)
