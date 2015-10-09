/***********************************************************************//**
 *	OsDepModAndroid.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2012/12/10.
 *	Copyright 2012 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ANDROID_OS_DEP_MOD_ANDROID_H_
#define _ANDROID_OS_DEP_MOD_ANDROID_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base
#include "../etk/body/OsDepMod.h"

// External
#include "../tfw/common/AndroidMainModule.h"
#include "../tfw/string/CcString.h"
#include "../tfw/string/VcString.h"
#include "CompilerSwitch.h"

// Library

////////////////////////////////////////////////////////////////////////////
// マクロ

#if defined(UNICODE)
	#define STRING_CLASS	tfw::WcvString
	#define STRARRAY_CLASS	tfw::WcvStringArray
#else
	#define STRING_CLASS	tfw::CvString
	#define STRARRAY_CLASS	tfw::CvStringArray
#endif

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class CStringBase;

TFW_END_NS

namespace app
{

/*---------------------------------------------------------------------*//**
 *	Android OS 依存部
 *
**//*---------------------------------------------------------------------*/
class OsDepModAndroid : public etk::OsDepMod
{
	//======================================================================
	// 定数
public:
	enum MediaState
	{
		MSTAT_NULL,				// 使用不能
		MSTAT_READ_WRITE,		// 読み書き可能
		MSTAT_READ_ONLY,		// 読み込み専用
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	// リソースディレクトリの取得
	bool getResourceDirPath(tfw::VcString* path, const CcString prod);
	bool getDocumentDirPath(tfw::VcString* path, bool isMakeDirIfNoExist);
	bool getTemporaryDirPath(VcString* path);
	bool getExternalStorageDirPath(tfw::VcString* path, const tfw::CStringBase* subdir, const tfw::CStringBase* prod, bool isMakeDirIfNoExist);
	MediaState checkExternalMediaState() const; 

	// ホームページへのジャンプ動作
	void gotoHp(CStringBase* str) const;

	// テキストのコピー＆ペースト
	bool getPasteTextFromClipboard(Txt** txt);
	bool setCopyTextToClipboard(const Txt* txt);

	// 独自テキストフィールドの実装
	u32 createTextField(const Txt* txt, const RectS32* rect, const View* view, u32 flags);
	bool destroyTextField(u32 oscmpid);
	bool showTextField(u32 oscmpid, bool isShow);
	bool getTextFieldString(u32 oscmpid, Txt** txt);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	OsDepModAndroid(JNIEnv* jnienv, jobject clazzActivity);

	//======================================================================
	// 変数
private:
	AndroidMainModule _mainmod;
	VcString _prod;
};

}	// namespace app

////////////////////////////////////////////////////////////////////////////

#endif	// _ANDROID_OS_DEP_MOD_ANDROID_H_
