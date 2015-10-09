/***********************************************************************//**
 *	Serializer.h
 *	Terras Framework
 *	
 *	Created by Ryoutarou Kishi on 2010/03/23.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_SERIALIZE_SERIALIZER_H_
#define _TFW_SERIALIZE_SERIALIZER_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Vector3;
typedef Vector3<f32> Vector3F;
class VcString;

/*---------------------------------------------------------------------*//**
 *	直列化モジュール
 *	
**//*---------------------------------------------------------------------*/
class Serializer
{
	//======================================================================
	// 定数
private:
	// 設定フラグ
	static const u8	CF_STRUCTURE_MODE	= 0x01;	// ストラクチャモード

	//======================================================================
	// クラス
public:
	/*-----------------------------------------------------------------*//**
	 *	ヘッダ
	 *	※ 使うかどうかは任意
	 *	
	**//*-----------------------------------------------------------------*/
	class SeriHeader
	{
	public:
		u16 _verMajor;		// メジャーバージョン番号
		u8 _verMinor;		// マイナーバージョン番号
		u8 _flags;			// フラグ

	public:
		SeriHeader() : _verMajor(0), _verMinor(0), _flags(0) {}
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// メジャーバージョン番号を得る
	inline u16 getMajorVersionNumber() const { return _header._verMajor; }
	// マイナーバージョン番号を得る
	inline u8 getMinorVersionNumber() const { return _header._verMinor; }
	// バージョン番号を得る
	inline u32 getVersionNumber() const { return (_header._verMajor * 1000) + _header._verMinor; }
	// ストラクトモードかどうかを得る
	inline bool isStructureMode() const { return TFW_IS_FLAG(_header._flags, CF_STRUCTURE_MODE); }
	// ストラクトポインタを得る
	inline void* getStructure() { return _struct; }
	// 文字列ポインタを得る
	inline VcString* getString() { return _string; }
	// 汎用フラグを得る
	inline u32 getGeneralFlags() const { return _gflags; }
	// 汎用パラメータを得る
	inline void* getGeneralParameter() const { return _gparam; }

	// ストラクトポインタを設定する
	inline void setStructure(void* p) { _struct = p; }
	// 文字列ポインタを設定する
	inline void setString(VcString* p) { _string = p; }
	// 汎用フラグを設定する
	inline void setGeneralFlags(u32 gflags, bool on) { TFW_SET_FLAG(_gflags, gflags, on); }
	// 汎用パラメータを設定する
	inline void setGeneralParameter(void* p) { _gparam = p; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	virtual bool store(const void* val, s32 size) = 0;
	virtual bool restore(void* val, s32 size) = 0;

	inline bool store(const s8* val) { return store(val, sizeof(s8)); }
	inline bool store(const u8* val) { return store(val, sizeof(u8)); }
	inline bool store(const s16* val) { return store(val, sizeof(s16)); }
	inline bool store(const u16* val) { return store(val, sizeof(u16)); }
	inline bool store(const s32* val) { return store(val, sizeof(s32)); }
	inline bool store(const u32* val) { return store(val, sizeof(u32)); }
	inline bool store(const f32* val) { return store(val, sizeof(f32)); }
	inline bool store(const f64* val) { return store(val, sizeof(f64)); }
	inline bool store(const bool* val) { return store(val, sizeof(bool)); }
	inline bool storeHeader() { return store(&_header, sizeof(SeriHeader)); }
	bool store(const VcString* val);
	bool store(const Vector3F* val);
	inline bool restore(s8* val) { return restore(val, sizeof(s8)); }
	inline bool restore(u8* val) { return restore(val, sizeof(u8)); }
	inline bool restore(s16* val) { return restore(val, sizeof(s16)); }
	inline bool restore(u16* val) { return restore(val, sizeof(u16)); }
	inline bool restore(s32* val) { return restore(val, sizeof(s32)); }
	inline bool restore(u32* val) { return restore(val, sizeof(u32)); }
	inline bool restore(f32* val) { return restore(val, sizeof(f32)); }
	inline bool restore(f64* val) { return restore(val, sizeof(f64)); }
	inline bool restore(bool* val) { return restore(val, sizeof(bool)); }
	inline bool restoreHeader() { return restore(&_header, sizeof(SeriHeader)); }
	bool restore(VcString* val);
	bool restore(Vector3F* val);

	void setupHeader(u16 verMajor, u8 verMinor, bool isStructureMode)
	{
		_header._verMajor = verMajor;
		_header._verMinor = verMinor;
		TFW_SET_FLAG(_header._flags, CF_STRUCTURE_MODE, isStructureMode);
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	Serializer();
	virtual ~Serializer();

private:

	//======================================================================
	// 変数
private:
	SeriHeader _header;		// シリアライズヘッダ
	void* _struct;			// 構造体へのポインタ
	VcString* _string;		// 保存文字列へのポインタ
	u32 _gflags;			// 汎用フラグ
	void* _gparam;			// 汎用パラメータ
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_SERIALIZE_SERIALIZER_H_