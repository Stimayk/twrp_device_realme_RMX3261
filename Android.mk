ifneq ($(filter RMX3261,$(TARGET_DEVICE)),)

LOCAL_PATH := device/realme/RMX3261

include $(call all-makefiles-under,$(LOCAL_PATH))

endif
