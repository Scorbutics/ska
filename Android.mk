TOP_LOCAL_PATH:=$(call my-dir)
LOCAL_PATH := $(TOP_LOCAL_PATH)
include $(CLEAR_VARS)

LOCAL_MODULE := ska

#traverse all the directory and subdirectory
define walk
  $(wildcard $(1)) $(foreach e, $(wildcard $(1)/*), $(call walk, $(e)))
endef

ALLFILES := $(call walk, $(LOCAL_PATH)/ska_audio)
ALLFILES += $(call walk, $(LOCAL_PATH)/ska_ai)
ALLFILES += $(call walk, $(LOCAL_PATH)/ska_audio)
ALLFILES += $(call walk, $(LOCAL_PATH)/ska_core)
ALLFILES += $(call walk, $(LOCAL_PATH)/ska_draw)
ALLFILES += $(call walk, $(LOCAL_PATH)/ska_gui)
ALLFILES += $(call walk, $(LOCAL_PATH)/ska_particle)
ALLFILES += $(call walk, $(LOCAL_PATH)/ska_physic)
ALLFILES += $(call walk, $(LOCAL_PATH)/ska_script)
ALLFILES += $(call walk, $(LOCAL_PATH)/ska_world)
FILE_LIST := $(filter %.cpp, $(ALLFILES))

LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)

LOCAL_CFLAGS += -std=c++14 -fexceptions -fpermissive

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../SDL2/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../SDL2_ttf/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../SDL2_mixer
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../SDL2_image/src
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../SDL2_giflib

LOCAL_C_INCLUDES += $(LOCAL_PATH)/ska_audio
LOCAL_C_INCLUDES += $(LOCAL_PATH)/ska_ai
LOCAL_C_INCLUDES += $(LOCAL_PATH)/ska_audio
LOCAL_C_INCLUDES += $(LOCAL_PATH)/ska_core
LOCAL_C_INCLUDES += $(LOCAL_PATH)/ska_draw
LOCAL_C_INCLUDES += $(LOCAL_PATH)/ska_gui
LOCAL_C_INCLUDES += $(LOCAL_PATH)/ska_particle
LOCAL_C_INCLUDES += $(LOCAL_PATH)/ska_physic
LOCAL_C_INCLUDES += $(LOCAL_PATH)/ska_script
LOCAL_C_INCLUDES += $(LOCAL_PATH)/ska_world

include $(BUILD_STATIC_LIBRARY)