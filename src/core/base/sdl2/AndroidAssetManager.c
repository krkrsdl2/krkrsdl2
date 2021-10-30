
#if defined(__ANDROID__)

#include "AndroidAssetManager.h"

static AAssetManager *asset_manager = NULL;
static jobject javaAssetManagerRef = 0;

bool AndroidAssetManager_Create_AssetManager(void)
{
	if (asset_manager != NULL)
	{
		return true;
	}

	JNIEnv *env = (JNIEnv *)SDL_AndroidGetJNIEnv();
	if (env == NULL)
	{
		return false;
	}
	jobject context = (jobject)SDL_AndroidGetActivity();

	/* javaAssetManager = context.getAssets(); */
	jmethodID mid = (*env)->GetMethodID(env, (*env)->GetObjectClass(env, context),
			"getAssets", "()Landroid/content/res/AssetManager;");
	jobject javaAssetManager = (*env)->CallObjectMethod(env, context, mid);

	/**
	 * Given a Dalvik AssetManager object, obtain the corresponding native AAssetManager
	 * object.  Note that the caller is responsible for obtaining and holding a VM reference
	 * to the jobject to prevent its being garbage collected while the native object is
	 * in use.
	 */
	javaAssetManagerRef = (*env)->NewGlobalRef(env, javaAssetManager);
	asset_manager = AAssetManager_fromJava(env, javaAssetManagerRef);

	if (asset_manager == NULL)
	{
		(*env)->DeleteGlobalRef(env, javaAssetManagerRef);
		return false;
	}
	return true;
}

void AndroidAssetManager_Destroy_AssetManager(void)
{
	JNIEnv *env = (JNIEnv *)SDL_AndroidGetJNIEnv();

	if (env != NULL && asset_manager != NULL)
	{
		(*env)->DeleteGlobalRef(env, javaAssetManagerRef);
		asset_manager = NULL;
	}
}

AAssetManager *AndroidAssetManager_Get_AssetManager(void)
{
	AndroidAssetManager_Create_AssetManager();
	return asset_manager;
}

#endif
