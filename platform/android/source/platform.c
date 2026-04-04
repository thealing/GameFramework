#include "platform.h"

jobject activity_instance;

AAssetManager* asset_manager;

JNIEnv* env;

jclass ByteArrayInputStream;

jclass Bitmap;

jclass BitmapFactory;

jclass Activity;

jclass File;

jmethodID ByteArrayInputStream_init;

jmethodID Bitmap_getWidth;

jmethodID Bitmap_getHeight;

jmethodID Bitmap_getPixels;

jmethodID BitmapFactory_decodeByteArray;

jmethodID Activity_getFilesDir;

jmethodID File_getPath;

void platform_init(ANativeActivity* activity)
{
	activity_instance = activity->clazz;

	asset_manager = activity->assetManager;

	(*activity->vm)->AttachCurrentThread(activity->vm, &env, NULL);

	ByteArrayInputStream = (*env)->FindClass(env, "java/io/ByteArrayInputStream");

	Bitmap = (*env)->FindClass(env, "android/graphics/Bitmap");

	BitmapFactory = (*env)->FindClass(env, "android/graphics/BitmapFactory");

	Activity = (*env)->FindClass(env, "android/app/Activity");

	File = (*env)->FindClass(env, "java/io/File");

	ByteArrayInputStream_init = (*env)->GetMethodID(env, ByteArrayInputStream, "<init>", "([B)V");

	Bitmap_getWidth = (*env)->GetMethodID(env, Bitmap, "getWidth", "()I");

	Bitmap_getHeight = (*env)->GetMethodID(env, Bitmap, "getHeight", "()I");

	Bitmap_getPixels = (*env)->GetMethodID(env, Bitmap, "getPixels", "([IIIIIII)V");

	BitmapFactory_decodeByteArray = (*env)->GetStaticMethodID(env, BitmapFactory, "decodeByteArray", "([BII)Landroid/graphics/Bitmap;");

	Activity_getFilesDir = (*env)->GetMethodID(env, Activity, "getFilesDir", "()Ljava/io/File;");

	File_getPath = (*env)->GetMethodID(env, File, "getPath", "()Ljava/lang/String;");
}
