/***********************************************************************//**
 *	OsDepMod.h
 *	Envelopment Terras Kit
 *
 *	Created by Ryoutarou Kishi on 2010/03/05.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ETK_BODY_OS_DEP_MOD_H_
#define _ETK_BODY_OS_DEP_MOD_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "../../tfw/common/OsModule.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Rect;
typedef Rect<f32> RectF32;
class CcString;
class CStringBase;
class VcString;
class Txt;
class View;

TFW_END_NS

ETK_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	OS 依存部
 *
**//*---------------------------------------------------------------------*/
class OsDepMod : public tfw::OsModule
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	// アプリケーションパラメータの取得
	virtual const CStringBase* getAppParamStr() { return (CStringBase*)_strAppParam; }
	virtual void getAppVersionString(VcString* ver) = 0;

	// リソースディレクトリの取得
	virtual bool getResourceDirPath(VcString* path, const CcString prod) = 0;
	virtual bool getDocumentDirPath(VcString* path, bool isMakeDirIfNoExist) = 0;
	virtual bool getTemporaryDirPath(VcString* path) = 0;

	// ホームページへのジャンプ動作
	virtual void gotoHp(CStringBase* str) const = 0;
	
	// 広告バナー表示制御
	virtual void showAdBanner(bool show) const = 0;

	// テキストのコピー＆ペースト
	virtual bool getPasteTextFromClipboard(Txt** txt) = 0;
	virtual bool setCopyTextToClipboard(const Txt* txt) = 0;

	// 独自テキストフィールドの実装
	virtual u32 createTextField(const Txt* txt, const RectF32* rect, const View* view, u32 flags) = 0;
	virtual bool destroyTextField(u32 oscmpid) = 0;
	virtual bool showTextField(u32 oscmpid, bool isShow) = 0;
	virtual bool getTextFieldString(u32 oscmpid, Txt** txt) = 0;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	OsDepMod(OsMainModule* modStaticRef);
	virtual ~OsDepMod();

	//======================================================================
	// 変数
protected:
	VcString* _strAppParam;
};

ETK_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ETK_BODY_OS_DEP_MOD_H_
