/***********************************************************************//**
 *	EsValue.h
 *	Enlight Script
 *
 *	Created by Ryoutarou Kishi on 2009/11/06.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _ES_ES_VALUE_H_
#define _ES_ES_VALUE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "EsBase.h"

#include "../../tfw/hash/HashOp.h"

////////////////////////////////////////////////////////////////////////////
// クラス

ES_BEGIN_NS

class EsAtom;
class EsObject;

/*---------------------------------------------------------------------*//**
 *	値
 *
**//*---------------------------------------------------------------------*/
class EsValue
{
	//======================================================================
	// 定数
public:
	// 型
	enum EsValueType
	{
		VTYPE_NULL = 0,
		VTYPE_VOID,			// ボイド
		VTYPE_OBJ,			// オブジェクト ⇒ JSVAL_OBJECT
		VTYPE_STRING,		// 文字列 ⇒ JSVAL_STRING
		VTYPE_INT,			// 整数値 ⇒ JSVAL_INT
		VTYPE_INT64,		// 64bit 整数値
		VTYPE_DOUBLE,		// 浮動小数点値 ⇒ JSVAL_DOUBLE
		VTYPE_BOOLEAN,		// 真偽値 ⇒ JSVAL_BOOLEAN
		VTYPE_ATOM,			// アトム
		VTYPE_PRIVATE,		// プライベートデータ
		VTYPE_ERRORCODE,	// エラーコード
		NUM_VTYPE
	};

private:
	static const s32 HI_HOLE		= 0xffffffff;
	static const s32 HI_UNFINISHED	= 0xefffffff;

	//======================================================================
	// 構造体
private:
	union Value
	{
		struct
		{
			s32				_hi;
			s32				_lo;
		} _i;
		u64				_u;
		f64				_d;
		void*			_p;
		const EsAtom*	_a;
		const VcString*	_s;
		EsObject*		_o;
	};

	//======================================================================
	// クラス
public:
	/*-----------------------------------------------------------------*//**
	 *	値アトムハッシュ演算
	 *
	**//*-----------------------------------------------------------------*/
	class EsValueAtomHashOp : public HashOp
	{
		//==================================================================
		// メソッド

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// 外部サービス
	public:
		virtual u32		getEntrySize();
		virtual void*	newEntryBuffer(u32 numEntry);
		virtual void	deleteEntryBuffer(void* entrybuf);
		virtual u32		hashCode(const void* key);
		virtual bool	matchEntry(HashEntry* entry, const void* key);
		virtual bool	initEntry(HashEntry* entry, const void* key);
		virtual void	cleanupEntry(HashEntry* entry);
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// タイプを得る
	inline EsValueType getType() const { return (EsValueType)_type; }
	// 実質的なタイプを得る
	EsValueType getSubstantiveType() const;
	// オブジェクトを得る ⇒ JSVAL_TO_OBJECT
	inline EsObject* getObject() const { ASSERT(_type == VTYPE_OBJ); return _val._o; }
	// 整数下位値を得る
	inline s32 getInt() const { ASSERT((_type == VTYPE_INT) || (_type == VTYPE_NULL)); return _val._i._lo; }
	// 整数上位値を得る
	inline s32 getHiInt() const { ASSERT((_type == VTYPE_INT) || (_type == VTYPE_NULL)); return _val._i._hi; }
	// 64bit 整数を得る
	inline u64 getInt64() const { ASSERT(_type == VTYPE_INT64); return _val._u; }
	// 浮動小数点値を得る
	f64 getDouble() const;
	// 数値を得る
	f64 getNumber() const;
	// ブーリアン値を得る
	inline bool getBoolean() const { ASSERT(_type == VTYPE_BOOLEAN); return _val._i._lo != 0; }
	// 文字列値を得る
	const VcString* getString() const;
	// アトム値を得る
	inline const EsAtom* getAtom() const { ASSERT(_type == VTYPE_ATOM); return _val._a; }
	// プライベートデータを得る
	inline void* getPrivateData() const { ASSERT(_type == VTYPE_PRIVATE); return _val._p; }
	// エラーコードを得る
	inline s32 getErrorCode() const { ASSERT(_type == VTYPE_ERRORCODE); return _val._i._lo; }
	// 内部エラーコードを得る
	inline s32 getInternalErrorCode() const { ASSERT(_type == VTYPE_ERRORCODE); return _val._i._hi; }

	// NULL 値であるかどうかを得る ⇒ JSVAL_IS_NULL
	inline bool isNull() const { return _type == VTYPE_NULL; }
	// void 型であるかどうかを得る ⇒ JSVAL_IS_VOID
	inline bool isVoid() const { return _type == VTYPE_VOID; }
	// ホール値であるかどうかを得る
	inline bool isHole() const { return (_type == VTYPE_VOID) && (_val._i._hi == HI_HOLE); }
	// 実行未終了値であるかどうかを得る
	inline bool isUnfinished() const { return (_type == VTYPE_VOID) && (_val._i._hi == HI_UNFINISHED); }
	// プリミティブ（値）型であるかどうかを得る ⇒ JSVAL_IS_PRIMITIVE
	inline bool isPrimitive() const { return ((_type != VTYPE_OBJ) && (_type != VTYPE_PRIVATE) && (_type != VTYPE_ERRORCODE)) || isNull(); }
	// 整数型であるかどうかを得る ⇒ JSVAL_IS_INT
	inline bool isInt() const { return _type == VTYPE_INT; }
	inline bool isInt64() const { return _type == VTYPE_INT64; }
	// 浮動小数点型であるかどうかを得る ⇒ JSVAL_IS_DOUBLE
	bool isDouble() const;
	// オブジェクト型であるかどうかを得る ⇒ JSVAL_IS_OBJECT
	inline bool isObject() const { return _type == VTYPE_OBJ; }
	// ブーリアン型であるかどうかを得る ⇒ JSVAL_IS_BOOLEAN
	inline bool isBoolean() const { return _type == VTYPE_BOOLEAN; }
	// 文字列型であるかどうかを得る ⇒ JSVAL_IS_STRING
	bool isString() const;
	// アトム型であるかどうかを得る
	inline bool isAtom() const { return _type == VTYPE_ATOM; }
	// プライベートデータ型であるかどうかを得る
	inline bool isPrivateData() const { return _type == VTYPE_PRIVATE; }
	// エラーコード型であるかどうかを得る
	inline bool isErrorCode() const { return _type == VTYPE_ERRORCODE; }
	// 数値であるかどうかを得る ⇒ JSVAL_IS_NUMBER
	inline bool isNumber() const { return isInt() || isDouble(); }
	// 関数オブジェクトであるかどうかを得る
	bool isFunctionObject() const;

	// void 値を設定する
	void setVoid();
	// NULL を設定する
	void setNull();
	// ホール値を設定する
	void setHole();
	// 実行未終了値を設定する
	void setUnfinished();
	// オブジェクトを設定する
	void setValue(EsObject* obj);
	// 整数値を設定する
	void setValue(s32 val);
	void setValue(s32 ivalLo, s32 ivalHi);
	void setValue(u64 val);
	// ブーリアン値を設定する
	void setValue(bool b);
	// 浮動小数点値を設定する
	void setValue(f64 d);
	// 文字列を設定する
	void setValue(const VcString* str);
	// アトム値を設定する
	void setValue(const EsAtom* atom);
	// プライベートデータを設定する
	void setPrivateData(void* p);
	// エラーコードを設定する
	void setErrorCode(s32 codeError, s32 codeInternal);
	// 型を設定する
	void setType(EsValueType type);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	bool compareEq(EsContext* ctx, const EsValue* valR) const;
	bool compareNe(EsContext* ctx, const EsValue* valR) const;
	bool compareLt(EsContext* ctx, const EsValue* valR) const;
	bool compareLe(EsContext* ctx, const EsValue* valR) const;
	bool compareGt(EsContext* ctx, const EsValue* valR) const;
	bool compareGe(EsContext* ctx, const EsValue* valR) const;
	bool equals(const EsValue* valR) const;
	bool equalsStrict(const EsValue* valR) const;
	bool incdec(EsValue* valRes, EsContext* ctx, bool isDecrement, bool isPost);

	bool toNumber(f64* fval, EsContext* ctx) const { return toNumber(fval, 0L, ctx); }
	bool toNumber(f64* fval, EsValue* valConv, EsContext* ctx) const;
	bool toString(const VcString** cstr, EsContext* ctx) const;
	bool toString(VcString* str, EsContext* ctx) const;
	bool toBoolean(bool* bval, EsContext* ctx) const;
	bool toInt32(s32* ival, EsContext* ctx) const;
	bool toUint32(u32* uival, EsContext* ctx) const;

	static s32 convF64ToS32(f64 fval);
	static u32 convF64ToU32(f64 fval);

	void setNumber(f64 fval);

	EsLogicalType getLogicalType() const;
	u32 calcHashCode() const;

	// 高速なインクリメント／デクリメントが行えるか判定 ⇒ CAN_DO_FAST_INC_DEC
	inline bool isFastIncrement() const { return isInt() && ((getInt() & 0x40000000) == 0); }	// ± 0x3fffffff 以下である

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EsValue()								{	setVoid();					}
	explicit EsValue(EsObject* obj)			{	setValue(obj);				}
	explicit EsValue(s32 val)				{	setValue(val);				}
	explicit EsValue(u64 val)				{	setValue(val);				}
	explicit EsValue(bool b)				{	setValue(b);				}
	explicit EsValue(f64 d)					{	setValue(d);				}
	explicit EsValue(const VcString* str)	{	setValue(str);				}
	explicit EsValue(const EsAtom* atom)	{	setValue(atom);				}
	explicit EsValue(EsValueType type)		{	setType(type);				}
	inline static EsValue* setout(EsValue* v, EsObject* obj)		{	v->setValue(obj);	return v;	}
	inline static EsValue* setout(EsValue* v, s32 val)				{	v->setValue(val);	return v;	}
	inline static EsValue* setout(EsValue* v, bool b)				{	v->setValue(b);		return v;	}
	inline static EsValue* setout(EsValue* v, f64 d)				{	v->setValue(d);		return v;	}
	inline static EsValue* setout(EsValue* v, const VcString* str)	{	v->setValue(str);	return v;	}
	inline static EsValue* setout(EsValue* v, const EsAtom* atom)	{	v->setValue(atom);	return v;	}
	inline static EsValue* setout(EsValue* v, EsValueType type)		{	v->setType(type);	return v;	}

	EsValue(const EsValue& src)				{	copy(&src);					}
	EsValue& operator =(const EsValue& src)	{	copy(&src);	return *this;	}
	void copy(const EsValue* src);

	//======================================================================
	// 変数
private:
	EsValueType _type;
	Value _val;
};

ES_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _ES_ES_VALUE_H_
