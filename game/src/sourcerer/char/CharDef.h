/***********************************************************************//**
 *	CharDef.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/05/12.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_CHAR_CHAR_DEF_H_
#define _SRCR_CHAR_CHAR_DEF_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "../status/Elene.h"
#include "../status/CharParam.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class FileBase;
class VcString;

TFW_END_NS

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	アイテム定義
 *
**//*---------------------------------------------------------------------*/
class CharDef
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// キャラクタ定義 ID を得る
	inline u16 getCharDefId() const { return _chrdid; }
	// 名前を得る
	inline const VcString* getName(LangId lang) const { return _name[lang]; }
	// 苗字を得る
	inline const VcString* getFamilyName(LangId lang) const { return _fname[lang]; }
	// デフォルトのキャラクターパラメーターを得る
	inline const CharParam* getDefaultCharParam() const { return &_chrprm; }
	// 五元素原質値を得る
	inline const Elene* getOqlElene() const { return &_eleneOql; }
	// 五元素現在値を得る
	inline const Elene* getCurElene() const { return &_eleneCql; }
	// 影 X オフセットを得る
	inline s16 getShadowOffsetX() const { return _xShadow; }
	// 影 Z オフセットを得る
	inline s16 getShadowOffsetZ() const { return _zShadow; }
	// 影半径を得る
	inline s16 getShadowRadius() const { return _rShadow; }
	/*
	// 汎用パラメータ１を得る
	inline u16 getParam1() const { return _param1; }
	// 汎用パラメータ２を得る
	inline u16 getParam2() const { return _param2; }
	// 汎用パラメータ３を得る
	inline u16 getParam3() const { return _param3; }
	// 汎用パラメータ４を得る
	inline u16 getParam4() const { return _param4; }
	*/

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	CharDef();
	~CharDef();
	bool loadFromDataFile(FileBase* file);
	bool loadFromStringFile(FileBase* file);
	void cleanup();

private:
	bool loadString(VcString* str, FileBase* file);

	//======================================================================
	// 変数
private:
	u16 _chrdid;					// 定義 ID
	s16 _xShadow;					// 影オフセット X
	s16 _zShadow;					// 影オフセット Z
	s16 _rShadow;					// 影半径
	u16 _param1;					// 汎用パラメータ 1
	u16 _param2;					// 汎用パラメータ 2
	u16 _param3;					// 汎用パラメータ 3
	u16 _param4;					// 汎用パラメータ 4
	CharParam _chrprm;				// キャラクターパラメーター
	Elene _eleneOql;				// 五元素原質
	Elene _eleneCql;				// 五元素現在値
	VcString* _name[NUM_LANGID];	// 名前
	VcString* _fname[NUM_LANGID];	// 苗字
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_CHAR_CHAR_DEF_H_
