/***********************************************************************//**
 *	OsDepModIos.h
 *	Enlight iOS Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/05.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _IOS_OS_DEP_MOD_IOS_H_
#define _IOS_OS_DEP_MOD_IOS_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../etk/body/OsDepMod.h"

////////////////////////////////////////////////////////////////////////////
// クラス

#ifdef __OBJC__
@class EnlightAppDelegate;
#else
class EnlightAppDelegate;
#endif

/*---------------------------------------------------------------------*//**
 *	iOS OS 依存部
 *
**//*---------------------------------------------------------------------*/
class OsDepModIos : public etk::OsDepMod
{
	//======================================================================
	// 定数
public:
	// デバイスタイプ
	enum DeviceType
	{
		DeviceType_Unknown,
		
		DeviceType_iPhoneSimulator,
		DeviceType_iPhoneSimulator_iPhone,
		DeviceType_iPhoneSimulator_iPad,
		
		DeviceType_iPhone1G,
		DeviceType_iPhone3G,
		DeviceType_iPhone3GS,
		DeviceType_iPhone4,
		DeviceType_iPhone5,
		
		DeviceType_iPod1G,
		DeviceType_iPod2G,
		DeviceType_iPod3G,
		DeviceType_iPod4G,
		DeviceType_iPod5G,
		
		DeviceType_iPad1G,
		DeviceType_iPad2G,
		DeviceType_iPad3G,
		DeviceType_iPad4G,
		
		DeviceType_iPhone_Unknown,
		DeviceType_iPod_Unknown,
		DeviceType_iPad_Unknown,
		DeviceType_IFPGA,
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	// アプリケーションパラメータの取得
	void getAppVersionString(VcString* ver);

	// リソースディレクトリの取得
	bool getResourceDirPath(VcString* path, const CcString prod);
	bool getDocumentDirPath(VcString* path, bool isMakeDirIfNoExist);
	bool getTemporaryDirPath(VcString* path);

	// ホームページへのジャンプ動作
	void gotoHp(CStringBase* str) const;
	
	// 広告バナー表示制御
	void showAdBanner(bool show) const;
	
	// テキストのコピー＆ペースト
	bool getPasteTextFromClipboard(Txt** txt);
	bool setCopyTextToClipboard(const Txt* txt);
	
	// 独自テキストフィールドの実装
	u32 createTextField(const Txt* txt, const RectF32* rect, const View* view, u32 flags);
	bool destroyTextField(u32 oscmpid);
	bool showTextField(u32 oscmpid, bool isShow);
	bool getTextFieldString(u32 oscmpid, Txt** txt);
	
	// iOS の操作
	static bool getAppSchema(VcString* path, s32 proddefid);
	static bool isInstalledApp(s32 proddefid);
	static bool runApp(s32 proddefid, const CStringBase* param);
	static bool transferSavesTo(const VcString* namePasteBoard, const VcString* nameFiles, const VcString* keyFiles, s32 numFiles);
	static bool transferSavesFrom(const VcString* namePasteBoard, const VcString* nameFiles, const VcString* keyFiles, s32 numFiles);
	static bool isTransferSavesPasterboardValid(const VcString* namePasteBoard, const VcString* keyFiles, s32 numFiles);
	static void removeTransferSavesPasterboard(const VcString* namePasteBoard);
	
	// デバイスの判定
	static u32 getDeviceType();
	static bool isAvailableMotionDevice();
	static bool isAvailableAccelerometerDevice();
	static bool isAvailableMagnetometerDevice();
	static bool isAvailableGyroDevice();
	
	// メモリ容量情報を得る
	static bool getMemoryInfo(u32* sizeFree, u32* sizeTotal, u32* sizeResident);
	
	// 特殊ポインタの取得・設定
	EnlightAppDelegate* getEnlightAppDelegate();
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	OsDepModIos();
};

////////////////////////////////////////////////////////////////////////////

#endif	// _IOS_OS_DEP_MOD_IOS_H_
