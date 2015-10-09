/***********************************************************************//**
 *	FixedSourceDef.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/05/24.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_SOURCE_FIXED_SOURCE_DEF_H_
#define _SRCR_SOURCE_FIXED_SOURCE_DEF_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class FileBase;

TFW_END_NS

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	固定ソース定義
 *
**//*---------------------------------------------------------------------*/
class FixedSourceDef
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 固定ソース定義 ID を得る
	inline u16 getFixedSourceDefId() const { return _fsrcdid; }
	// 名前を得る
	inline const VcString* getName() const { return _name; }
	// JS ソースコードを得る
	inline const VcString* getJsSource() const { return _sourceJs; }
	// SS ソースコードを得る
	inline const VcString* getSsSource() const { return _sourceSs; }
	// 説明テキストを得る
	inline const VcString* getExplain() const { return _explain; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	FixedSourceDef();
	~FixedSourceDef();
	bool loadFromDataFile(FileBase* file);
	bool loadFromStringFile(FileBase* file);
	void cleanup();

private:
	bool loadString(VcString* str, FileBase* file);

	//======================================================================
	// 変数
private:
	u16 _fsrcdid;			// 定義 ID
	VcString* _name;		// 名前
	VcString* _sourceJs;	// JS ソース
	VcString* _sourceSs;	// SS ソース
	VcString* _explain;		// 説明
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_SOURCE_FIXED_SOURCE_DEF_H_
