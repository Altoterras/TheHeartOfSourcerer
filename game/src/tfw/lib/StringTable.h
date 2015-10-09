/***********************************************************************//**
 *	StringTable.h
 *	Terras Framework
 *	
 *	Created by Ryoutarou Kishi on 2010/03/17.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_LIB_STRING_TABLE_H_
#define _TFW_LIB_STRING_TABLE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class VcString;
class FileBase;

/*---------------------------------------------------------------------*//**
 *	文字列テーブル
 *
**//*---------------------------------------------------------------------*/
class StringTable
{
	//======================================================================
	// クラス
private:
	class StringDef
	{
	public:
		u16	_id;
		VcString* _strarr;

		StringDef();
		~StringDef();
		bool create(s32 numCol);
		void destroy();
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// ロードされたかどうか
	inline bool isValid() const { return _sdarr != 0L; }

	// データ数を得る
	inline s32 getRowNum() const { return _numArr; }
	// ID を得る
	inline const u16 getIdByIndex(s32 row) const { if((0 <= row) && (row <= _numArr)) { return _sdarr[row]._id; } return 0; }
	// 文字列を得る
	const VcString* getStringByIndex(s32 row, s32 col) const;
	// ID から文字列を得る
	inline const VcString* getString(u16 id) const { return getString(id, 0); }
	const VcString* getString(u16 id, s32 col) const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	StringTable();
	~StringTable();
	bool loadFromFile(FileBase* file, s32* isValidFileColArray, s32 numValidFileCol);
	void destroy();

	//======================================================================
	// 変数
private:
	StringDef* _sdarr;
	s32 _numCol;
	s32 _numArr;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////
	
#endif	// _TFW_LIB_STRING_TABLE_H_

