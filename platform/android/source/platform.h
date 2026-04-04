#pragma once

#include "engine/macros.h"

#include <android/log.h>

#include <android/native_activity.h>

#include <android/native_window.h>

#include <android/asset_manager.h>

#include <GLES/egl.h>

#include <SLES/OpenSLES.h>

#include <SLES/OpenSLES_Android.h>

#include <pthread.h>

extern jobject activity_instance;

extern AAssetManager* asset_manager;

extern JNIEnv* env;

extern jclass ByteArrayInputStream;

extern jclass Bitmap;

extern jclass BitmapFactory;

extern jclass Activity;

extern jclass File;

extern jmethodID ByteArrayInputStream_init;

extern jmethodID Bitmap_getWidth;

extern jmethodID Bitmap_getHeight;

extern jmethodID Bitmap_getPixels;

extern jmethodID BitmapFactory_decodeByteArray;

extern jmethodID Activity_getFilesDir;

extern jmethodID File_getPath;

void platform_init(ANativeActivity* activity);
