/***********************************************************************//**
 *	EsAtom.h
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2009/10/22.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ES_ES_ATOM_H_
#define _ES_ES_ATOM_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "EsBase.h"

#include "../tfw/hash/HashEntry.h"

////////////////////////////////////////////////////////////////////////////
// クラス

ES_BEGIN_NS

class EsValue;

/*---------------------------------------------------------------------*//**
 *	アトム
 *
**//*---------------------------------------------------------------------*/
class EsAtom
{
	//======================================================================
	// 定数
public:
	static const u8		ATYPE_NULL		= 0;
	static const u8		ATYPE_DOUBLE	= 1;
	static const u8		ATYPE_STRING	= 2;
	static const u8		ATYPE_VALUE		= 3;

	static const u16	F_DEL_DATA		= 0x0001;	// デストラクタでのデータ削除要求

	//======================================================================
	// クラス
public:
	/*-----------------------------------------------------------------*//**
	 *	アトム ハッシュ エントリ
	 *
	**//*-----------------------------------------------------------------*/
	class EsAtomHashEntry : public HashEntry
	{
		//==================================================================
		// メソッド

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// アクセサ
	public:
		const EsAtom*	getAtom() const				{	return _atom;	}
		void			setAtom(const EsAtom* atom)	{	_atom = atom;	}

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// 外部サービス
	public:
		EsAtomHashEntry() : _atom(0L) {}

		//==================================================================
		// 変数
	private:
		const EsAtom* _atom;
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 値タイプを得る
	inline u8 getType() const { return _atype; }
	// フラグを得る
	inline u32 getFlag() const { return _flags; }
	// 有効かどうかを得る
	inline bool isEnable() const { return _atype != ATYPE_NULL; }
	// 文字列値のポインタを得る ⇒ js_AtomToPrintableString
	inline const VcString* getString() const { return (_atype == ATYPE_STRING) ? _data._str : 0L; }
	// ダブル値のポインタを得る
	inline const F64* getDouble() const { return (_atype == ATYPE_DOUBLE) ? _data._dval : 0L; }
	// 値を得る
	inline const EsValue* getValue() const { return (_atype == ATYPE_VALUE) ? _data._val : 0; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	EsAtom(const VcString* strRef);
	EsAtom(const f64* dval);
	EsAtom(const EsValue* val);
	EsAtom(const VcString* strRef, u16 flags);
	~EsAtom();
	bool equals(const EsValue* val) const;
	bool isSameString(const VcString* str) const;

private:

	//======================================================================
	// 変数
private:
	union
	{
		const VcString* _str;
		const F64* _dval;
		const EsValue* _val;
	} _data;
	u16 _flags;
	u8 _atype;
	u8 __pad_0010[ 1 ];
};

ES_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ES_ES_ATOM_H_
