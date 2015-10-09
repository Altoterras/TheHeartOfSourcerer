/***********************************************************************//**
 *	Source.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/12.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_SOURCE_SOURCE_H_
#define _SRCR_SOURCE_SOURCE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "SourceDef.h"
#include "../status/Elene.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class Serializer;

TFW_END_NS

namespace es
{
	class EsAtom;
	class EsKeyword;
	class EsKeywordSet;
}

SRCR_BEGIN_NS

class FixedSourceDef;
class FixedSourceDefTbl;

/*---------------------------------------------------------------------*//**
 *	ソウマ ソース
 *
**//*---------------------------------------------------------------------*/
class Source
{
	//======================================================================
	// 定数
public:
	static const u32 MAX_SOURCE_SIZE_TRUE		= 0x00ffffff;	// （内部処理的に真の）最大ソースコード長
	static const u32 MAX_SOURCE_SIZE_REGULATION	= 2000;			// 仕様上の最大ソースコード長

private:
	// フラグ
	static const u32 F_KIND_MASK				= 0x00000007;	// ソース種別マスク
	static const u32 F_RESERVED_FIXED			= 0x00000100;	// 固定ソース用予約
	static const u32 F_RESERVED_USER			= 0x00000200;	// ユーザーソース用予約

	//======================================================================
	// クラス
public:
	/*-----------------------------------------------------------------*//**
	 *	ソースのセーブストラクチャ
	 *
	**//*-----------------------------------------------------------------*/
	class SaveStructure
	{
	public:
		// [  0]
		s32 _sidxName;							// [  4] 名前末端文字列インデックス
												//		 マイナス値の場合は空のソース
		s32 _sidxCode;							// [  4] コード末端文字列インデックス
												//		 マイナス値の場合は固定ソース定義 ID (の正負反転値)
		s32 _sidxExplain;						// [  4] 説明文末端文字列インデックス
												//		 0 の場合はなし
		u32	_flags;								// [  4] フラグ
		Elene _eeNqlNecs;						// [ 20] 必要原質量
		// [ 36]
		static const char CHAR_SPRIT = 0x1f;
		static const s32 SIZE_THIS = 36;
		SaveStructure() { TFW_ZERO_MEMORY(this, sizeof(SaveStructure)); }
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 名前を得る
	inline const VcString* getName() const { return _name; }
	// ソースコードを得る
	inline const VcString* getCode() const { return _code; }
	// 説明文を得る
	inline const VcString* getExplain() const { return _explain; }
	// 固定ソース定義を得る（NULL は大いにありうる）
	inline const FixedSourceDef* getFixedSourceDef() const { return _fsrcdf; }
	// ソース種別を得る
	inline SourceDef::SourceKind getSourceKind() const { return (SourceDef::SourceKind)(_flags & F_KIND_MASK); }
	// 必要原質量を得る
	inline const Elene* getRequirementOql() const { return &_eeNqlNecs; }
	// 未使用かどうかを調べる
	bool isEmpty() const;
	// 有効なソースかどうかどうかを調べる
	bool isValid() const;
	// 有効なユーザー作成ソースかを得る（バグがなく文法的に合っているかは別）
	bool isUserCustomized() const;
	// 固定予約ソースであるかを調べる
	inline bool isReservedForFixed() const { return TFW_IS_FLAG(_flags, F_RESERVED_FIXED); }
	// ユーザー予約ソースであるかを調べる
	inline bool isReservedForUser() const { return TFW_IS_FLAG(_flags, F_RESERVED_USER); }
	// 予約ソースであるかを調べる
	inline bool isReserved() const { return isReservedForFixed() || isReservedForUser(); }

	// 固定ソース用予約を設定する
	inline void setReservedForFixed(bool is) { TFW_SET_FLAG(_flags, F_RESERVED_FIXED, is); }
	// ユーザーソース用予約を設定する
	inline void setReservedForUser(bool is) { TFW_SET_FLAG(_flags, F_RESERVED_USER, is); }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	bool setCustomCode(const VcString* code);
	bool analyzeNameAndExplain(VcString* name, VcString* explain);
	bool rewriteName(const VcString* nameNew);

public:
	SourceDef::SourceKind detectSourceKind() const;
private:
	SourceDef::SourceKind detectSourceKindSub_checkPart(s32 score[SourceDef::NUM_SRCKIND], const VcString* codePart) const;

public:
	bool convToJs();
	bool convToSs();
	static bool convToJsCode(VcString* code);
	static bool convToSsCode(VcString* code);
private:
	static bool convLanguageByKeyword(VcString* code, const es::EsKeywordSet* kwsetOld, const es::EsKeywordSet* kwsetNew, bool isOldLangSs);
	static void convLanguageSub_writeKeyword(VcString* codeNew, const EsKeyword* kwd, bool isLangSs);
	static void convLanguageSub_writeStringKeyword(VcString* codeNew, const VcString* strKeyword, bool isOldLangSs);
#if 0
	void adjustExtraSpaceOfCode(VcString* code, const es::EsKeywordSet* kwset, void(* func)(VcString*));
	static void removeExtraSpaceOfJsCodePart(VcString* code);
	static void addExtraSpaceOfJsCodePart(VcString* code);
#endif

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	Source();
	Source(const Source& src);
	~Source();
	void clear();
	void setup(const FixedSourceDef* fsrcdf, SourceDef::SourceKind srckind);
	void setup(const CStringBase* name, const CStringBase* code, const CStringBase* explain, SourceDef::SourceKind srckind);
	void copy(const Source* src);
	void reproduce(const Source* src);
	bool serialize(Serializer* ser) const;
	bool deserialize(Serializer* ser, const FixedSourceDefTbl* fsrcdftbl);
	bool storeToStructure(SaveStructure* svst, VcString* strSave) const;
	bool restoreFromStructure(const SaveStructure* svst, const VcString* strSave, const FixedSourceDefTbl* fsrcdftbl);
	static void swap(Source* source1, Source* source2);

private:
	void analyzeOql();
	bool analyzeSub_skipSpace(s32* idx, const char** pc, s32 len);
	bool analyzeSub_skipSign(s32* idx, const char** pc, s32 len, char cSign);
	bool analyzeSub_getNumber(s32* number, s32* idx, const char** pc, s32 len, char splitter);

	//======================================================================
	// 変数
private:
	VcString* _name;
	VcString* _code;
	VcString* _explain;
	const FixedSourceDef* _fsrcdf;
	u32 _flags;						// フラグ
	Elene _eeNqlNecs;				// 必要原質量
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_SOURCE_SOURCE_H_
