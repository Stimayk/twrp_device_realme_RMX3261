/*
 * Copyright (C) 2022 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#ifdef ANDROID
#define LOG_TAG "tsd"
#include <log/log.h>
#endif

#define TOUCH_SUSPEND "/sys/touchscreen/ts_suspend"

static char* read_string(const char* filename, int length) {
    int fd;
    char* str = malloc(sizeof(char) * length + 1);

    fd = open(filename, O_RDONLY);
    if (fd < 0) {
#ifdef ANDROID
        ALOGE("open %s failed, errno=%d(%s)\n", filename,
              errno, strerror(errno));
#else
        printf("open %s failed, errno=%d(%s)\n", filename,
              errno, strerror(errno));
#endif
        return NULL;
    }

    if (read(fd, str, length) != length) {
#ifdef ANDROID
        ALOGE("read from %s failed, errno=%d(%s)\n", filename,
            errno, strerror(errno));
#else
        printf("read from %s failed, errno=%d(%s)\n", filename,
            errno, strerror(errno));
#endif
        close(fd);
        return NULL;
    }

    close(fd);

    str[length] = '\0';

    return str;
}

static int write_string(const char* filename, const char* value) {
    int fd;
    int ret;

    fd = open(filename, O_RDWR);
    if (fd < 0) {
#ifdef ANDROID
        ALOGE("open %s failed, errno=%d(%s)\n", filename,
              errno, strerror(errno));
#else
        printf("open %s failed, errno=%d(%s)\n", filename,
              errno, strerror(errno));
#endif
        return fd;
    }

    ret = write(fd, value, strlen(value));
    if (ret < 0) {
#ifdef ANDROID
        ALOGE("write %s to %s failed, errno=%d(%s)\n", value,
              filename, errno, strerror(errno));
#endif
        close(fd);
        return ret;
    }

    close(fd);

    return 0;
}

int main() {
#ifdef ANDROID
    ALOGD("Starting tsd\n");
#else
    printf("Starting tsd\n");
#endif

    write_string(TOUCH_SUSPEND, "1");
    sleep(1);
    write_string(TOUCH_SUSPEND, "0");
    sleep(1);

#ifdef ANDROID
    ALOGD("Monitoring suspend\n");
#else
    printf("Monitoring suspend\n");
#endif

    char* current;

    while (1) {
        current = read_string(TOUCH_SUSPEND, 1);
        if (strcmp(current, "1") == 0) {
#ifdef ANDROID
            ALOGD("Disabling suspend\n");
#else
            printf("Disabling suspend\n");
#endif
            write_string(TOUCH_SUSPEND, "0");
        }

        free(current);
        current = NULL;
        sleep(1);
    }
}
