//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Android asset manager interface with SDL
//---------------------------------------------------------------------------
#ifndef AndroidAssetManagerH
#define AndroidAssetManagerH
//---------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__ANDROID__)
#include <SDL.h>
#include <android/asset_manager_jni.h>
#include <stdbool.h>

extern bool AndroidAssetManager_Create_AssetManager(void);
extern void AndroidAssetManager_Destroy_AssetManager(void);
extern AAssetManager *AndroidAssetManager_Get_AssetManager(void);
extern bool AndroidAssetManager_Check_Directory_Existent(AAssetManager *asset_manager, const char *path);
#endif

#ifdef __cplusplus
}
#endif

#endif
