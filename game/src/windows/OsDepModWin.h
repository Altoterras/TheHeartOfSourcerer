/***********************************************************************//**
 *	OsDepModWin.h
 *	Enlight Windows Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/05.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _WINDOWS_OS_DEP_MOD_WIN_H_
#define _WINDOWS_OS_DEP_MOD_WIN_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../etk/body/OsDepMod.h"

////////////////////////////////////////////////////////////////////////////
// マクロ

#if defined(UNICODE)
	#define STRING_CLASS	tfw::VwString
	#define STRARRAY_CLASS	tfw::VwStringArray
#else
	#define STRING_CLASS	tfw::VcString
	#define STRARRAY_CLASS	tfw::VcStringArray
#endif

////////////////////////////////////////////////////////////////////////////
// クラス

namespace app
{

/*---------------------------------------------------------------------*//**
 *	Windows OS 依存部
 *
**//*---------------------------------------------------------------------*/
class OsDepModWin : public etk::OsDepMod
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	// アプリケーションパラメータの取得
	void getAppVersionString(VcString* ver);

	// リソースディレクトリの取得
	bool getResourceDirPath(tfw::VcString* path, const CcString prod);
	bool getDocumentDirPath(tfw::VcString* path, bool isMakeDirIfNoExist);
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

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	OsDepModWin(const char* marker, const char* appnam);

	//======================================================================
	// 変数
private:
	const char* _marker;
	const char* _appnam;
};

}	// namespace app

////////////////////////////////////////////////////////////////////////////

#endif	// _WINDOWS_OS_DEP_MOD_WIN_H_
