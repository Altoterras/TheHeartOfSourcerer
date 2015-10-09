/***********************************************************************//**
 *	EleneStat.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/21.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_STATUS_ELENE_STAT_H_
#define _SRCR_STATUS_ELENE_STAT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "Elene.h"
#include "EleneVeil.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class ExecCtx;
class Serializer;

TFW_END_NS

SRCR_BEGIN_NS

class StatusChangeResult;

/*---------------------------------------------------------------------*//**
 *	元素エネルギー状態
 *
**//*---------------------------------------------------------------------*/
class EleneStat
{
	//======================================================================
	// 定数
public:
	// 質フラグ
	//	※	質は、純性・激性・鈍性の３種類あるが、
	//		純正は他の激性・鈍性とは組み合わされない．
	//		激性は、鈍性と組み合わされることがる．
	//		従って、純正は本来フラグ化の必要が無いが、
	//		統一感のためにいちおうフラグの形をとっている
	static const u32 QF_MASK_PURE		= 0x0000000f;	// 純性マスク
	static const u32 QF_MASK_EXCT		= 0x00000f00;	// 激性マスク
	static const u32 QF_MASK_DULL		= 0x000f0000;	// 鈍性マスク
	static const u32 QF_PURE_1			= 0x00000001;	// 純性
	static const u32 QF_PURE_2			= 0x00000003;	// 純性+1
	static const u32 QF_PURE_3			= 0x00000007;	// 純性+2
	static const u32 QF_EXCT_1			= 0x00000100;	// 激性
	static const u32 QF_EXCT_2			= 0x00000300;	// 激性+1
	static const u32 QF_EXCT_3			= 0x00000700;	// 激性+2
	static const u32 QF_DULL_1			= 0x00010000;	// 鈍性
	static const u32 QF_DULL_2			= 0x00030000;	// 鈍性+1
	static const u32 QF_DULL_3			= 0x00070000;	// 鈍性+2
	// 陰陽フラグ
	static const u32 QF_MASK_PN			= 0xff000000;	// 陰陽マスク
	static const u32 QF_MOD_1			= 0x01000000;	// 中性
	static const u32 QF_MOD_2			= 0x02000000;	// 極中性
	static const u32 QF_POS_1			= 0x03000000;	// 陽性
	static const u32 QF_POS_2			= 0x04000000;	// 極陽性
	static const u32 QF_NEG_1			= 0x05000000;	// 陰性
	static const u32 QF_NEG_2			= 0x06000000;	// 極陰性

	// 五元素バランスによる機能フラグ
	static const u32 FF_NULL			= 0x00000000;	// 無効値（その他）
	// 作用値（この値内はフラグではない）
	static const u32 FF_MASK_OP			= 0x000000ff;	// 作用マスク
	static const u32 FF_OP_PURIFY		= 0x00000001;	// 純化
	static const u32 FF_OP_CURE			= 0x00000002;	// 回復
	static const u32 FF_OP_CATHARSIS	= 0x00000003;	// 浄化（純化 + 回復）
	static const u32 FF_OP_EXCEED		= 0x00000004;	// 増悪
	static const u32 FF_OP_WRAP			= 0x00000005;	// 持続結界強化
	static const u32 FF_OP_BARRIER		= 0x00000006;	// 瞬間結界強化
	static const u32 FF_OP_SUFFUSE		= 0x00000007;	// 両結界強化
	static const u32 FF_OP_UNWRAP		= 0x00000008;	// 持続結界弱体
	static const u32 FF_OP_UNBARRIER	= 0x00000009;	// 瞬間結界弱体
	static const u32 FF_OP_UNSUFFUSE	= 0x0000000a;	// 両結界弱体（持続結界弱体 + 瞬間結界弱体）
	static const u32 FF_OP_ASCEN_S		= 0x0000000b;	// 単元素上方シフト
	static const u32 FF_OP_ASCEN_A		= 0x0000000c;	// アセンション
	static const u32 FF_OP_DESCEN_S		= 0x0000000d;	// 単元素下方シフト
	static const u32 FF_OP_DESCEN_A		= 0x0000000e;	// ディセンション
	static const u32 FF_OP_ATTACK		= 0x00000010;	// 攻撃
	static const u32 FF_OP_EXC_ATTACK	= 0x00000014;	// 増悪攻撃（増悪 + 攻撃）
	static const u32 FF_OP_UNW_ATTACK	= 0x00000018;	// 持続結界弱体攻撃（持続結界弱体 + 攻撃）
	static const u32 FF_OP_UNB_ATTACK	= 0x00000019;	// 瞬間結界弱体攻撃（瞬間結界弱体 + 攻撃）
	static const u32 FF_OP_UNS_ATTACK	= 0x0000001a;	// 両結界弱体攻撃（両結界弱体 + 攻撃）
	static const u32 FF_OP_MISS_ATTACK	= 0x0000001f;	// 攻撃ミス
	static const u32 FF_OP_NOP			= 0x000000e1;	// エフェクト（対決時は特に作用なし）
	static const u32 FF_OP_IMMATURE		= 0x000000f1;	// 元素過小（未成熟）
	static const u32 FF_OP_OVERBREAK	= 0x000000f2;	// 元素過大（破裂）
	// 属性フラグ
	static const u32 FF_MASK_ATRB		= 0x0000ff00;	// 属性マスク
	static const u32 FF_ATRB_SOIL		= 0x00000100;	// 土
	static const u32 FF_ATRB_WATER		= 0x00000200;	// 水
	static const u32 FF_ATRB_FIRE		= 0x00000400;	// 火
	static const u32 FF_ATRB_WIND		= 0x00000800;	// 風
	static const u32 FF_ATRB_AETHEL		= 0x00001000;	// 空
	// 形態フラグ
	static const u32 FF_MASK_FORM		= 0x000f0000;	// 形態マスク
	static const u32 FF_FORM_SOIL		= 0x00010000;	// 土系
	static const u32 FF_FORM_WATER		= 0x00020000;	// 水系
	static const u32 FF_FORM_FIRE		= 0x00040000;	// 火系
	static const u32 FF_FORM_WIND		= 0x00080000;	// 風系
	// エフェクト種別値（この値内はフラグではない）
	static const u32 FF_MASK_EFF		= 0xff000000;	// エフェクトマスク
	static const u32 FF_EFF_SMOKE		= 0x01000000;	// 煙
	static const u32 FF_EFF_SMOKE_L		= 0x02000000;	// 光煙
	static const u32 FF_EFF_SMOKE_H		= 0x04000000;	// 温煙
	static const u32 FF_EFF_SAND		= 0x05000000;	// 砂
	static const u32 FF_EFF_SAND_L		= 0x06000000;	// 光砂
	static const u32 FF_EFF_SAND_H		= 0x07000000;	// 温砂
	static const u32 FF_EFF_MIST		= 0x08000000;	// 靄
	static const u32 FF_EFF_MIST_L		= 0x09000000;	// 光靄
	static const u32 FF_EFF_MIST_H		= 0x0a000000;	// 温靄
	static const u32 FF_EFF_FOG			= 0x0b000000;	// 霧
	static const u32 FF_EFF_FOG_L		= 0x0c000000;	// 光霧
	static const u32 FF_EFF_FOG_H		= 0x0d000000;	// 温霧
	static const u32 FF_EFF_WGRAIN		= 0x0e000000;	// 水粒
	static const u32 FF_EFF_WGRAIN_L	= 0x0f000000;	// 水粒（光）
	static const u32 FF_EFF_WGRAIN_H	= 0x10000000;	// 水粒（温）
	static const u32 FF_EFF_SPRAY		= 0x11000000;	// 飛沫
	static const u32 FF_EFF_SPRAY_L		= 0x12000000;	// 飛沫（光）
	static const u32 FF_EFF_SPRAY_H		= 0x13000000;	// 飛沫（温）
	static const u32 FF_EFF_SHINE		= 0x14000000;	// 光
	static const u32 FF_EFF_WIND		= 0x15000000;	// 風
	static const u32 FF_EFF_FIRE		= 0x16000000;	// 炎
	static const u32 FF_EFF_WATER		= 0x17000000;	// 水
	static const u32 FF_EFF_ICE			= 0x18000000;	// 氷
	static const u32 FF_EFF_SOIL		= 0x19000000;	// 土

	// 結界（ベール）の定義
	enum Veil
	{
		VEIL_WP,	// 持続結界．通常付帯用
		VEIL_BR,	// 瞬間結界．一時用（爆発ソウマキャンセルなど）
		NUM_VEIL
	};
	// 結界（ベール）の限界率
	static const s32 RATIO_WP_VEIL_MAX = 10;
	static const s32 RATIO_EF_VEIL_MAX = 10;

	// 質計算の閾値
	static const s32 PERCENT_UNPURE = 150;		// 非純性の閾％
	static const s32 PERCENT_OVERBREAK = 300;	// 元素過大の閾％

	//======================================================================
	// クラス
public:
	/*-----------------------------------------------------------------*//**
	 *	元素エネルギー状態のセーブストラクチャ
	 *
	**//*-----------------------------------------------------------------*/
	class SaveStructure
	{
	public:
		// [  0]
		Elene _eleneNql;			// [ 20] 原質（Native Quality）の元素エネルギー ※大元の五元素
		Elene _eleneCql;			// [ 20] 現在の元素エネルギー（Current Quality）
		s32 _shift;					// [  4] 全体シフト値
		EleneVeil _veils[NUM_VEIL];	// [ 48] 結界
		u8 __pad_0010[ 4];			// [  4]
		// [ 96]
		static const s32 SIZE_THIS = 96;
		SaveStructure() { TFW_ZERO_MEMORY(this, sizeof(SaveStructure)); }
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 現在の元素のエネルギーを得る
	inline const Elene* getCql() const { return &_eleneCql; }
	inline Elene* cql() { return &_eleneCql; }
	// 今世原質の元素エネルギーを得る
	inline const Elene* getNql() const { return &_eleneNql; }
	inline Elene* nql() { return &_eleneNql; }
	// 根本原質の元素エネルギーを得る
	inline const Elene* getOql() const { return &_eleneOql; }
	inline Elene* oql() { return &_eleneOql; }
	// 結界を得る
	inline const EleneVeil* getVeil(int idx) const { ASSERT((0 <= idx) && (idx < NUM_VEIL)); return &_veils[idx]; }
	inline EleneVeil* veil(int idx) { ASSERT((0 <= idx) && (idx < NUM_VEIL)); return &_veils[idx]; }
	// 表示用の（アニメーションされた）実効元素エネルギーを得る
	inline const Elene* getDfv() const { return &_eleneDfv; }
	// 初期値（全てゼロ）かどうかを得る
	bool isInitialValue() const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	inline u32 calcQualityFlags() const { return calcQualityFlags(0L, 0L, 0L, &_eleneCql, &_eleneNql); }
	inline u32 calcQualityFlags(u16* ratePure, u16* rateExct, u16* rateDull) const { return EleneStat::calcQualityFlags(ratePure, rateExct, rateDull, &_eleneCql, &_eleneNql); }
	static u32 calcQualityFlags(u16* ratePure, u16* rateExct, u16* rateDull, const Elene* eleneCql, const Elene* eleneNql);
	inline u32 calcFuncFlags(bool isSouma) const { return calcFuncFlags(calcQualityFlags(), &_eleneCql, &_eleneNql, isSouma); }
	inline u32 calcFuncFlags(u32 qulityfCalced, bool isSouma) const { return calcFuncFlags(qulityfCalced, &_eleneCql, &_eleneNql, isSouma); }
	static u32 calcFuncFlags(u32 qulityfCalced, const Elene* eleneCql, const Elene* eleneNql, bool isSouma);

	static u32 calcQualityFlagsToOperationFlag(u32 qulityfCalced);
	static u32 calcOperationFlagToQualityFlags(u32 flagFunc);
	static bool isNegativeOperation(u32 flagFunc);

	Elene calcCurrentAndWrapperElene() const;
	s32 calcVeilSubtotalElene(s32 el) const;
	bool isCleardVeil() const;

	void shift(s32 shiftval);
	void shiftSingle(s32 el, s32 shiftval);
	void purification(Elene* rest, const Elene* input);
	void impurification(const Elene* input);
	void clearVeil();

	void updateResultValue(bool isSyncAnimatedValue);
	void updateAnimatedValue(f32 framerate);

	void scale(s32 ratio);
	void scale(f32 ratio);

#if 0
private:
	u32 calcFuncFlags(u32 qf, s32 ratioSo, s32 ratioWa, s32 ratioFi, s32 ratioWi, s32 ratioAe) const;
#endif

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EleneStat() { clear(); }
	inline EleneStat(const EleneStat& src) { copy(&src); }
	inline explicit EleneStat(const Elene* eleneOql) { setup(eleneOql, eleneOql); }
	inline explicit EleneStat(const Elene* eleneOql, const Elene* eleneCql) { setup(eleneOql, eleneCql); }
	void copy(const EleneStat* src);
	inline EleneStat& operator =(const EleneStat& src) { copy(&src); return *this; }
	void clear();
	void reset();
	void setup(const Elene* eleneOql, const Elene* eleneCql, bool isNoClearVeil);
	void setup(const Elene* eleneOql, const Elene* eleneCql) { return setup(eleneOql, eleneCql, false); }
	void normalize();
	void exec(const ExecCtx* ec);

	bool serialize(Serializer* ser) const;
	bool deserialize(Serializer* ser);
	bool storeToStructure(SaveStructure* svst) const;
	bool restoreFromStructure(const SaveStructure* svst);

	//======================================================================
	// 変数
private:
	// [ 0]
	Elene _eleneOql;			// 根本原質（Original Quality）の元素エネルギー ※大元の五元素
	Elene _eleneNql;			// 今世原質（Native Quality）の元素エネルギー ※属性付加やシフト後の五元素
	Elene _eleneCql;			// 現在の元素エネルギー（Current Quality）
	Elene _eleneDfv;			// 表示用の結界後適用後元素エネルギー（外部に対して読み取り専用）
	// [100]
	s32 _shift;					// 全体シフト値
	EleneVeil _veils[NUM_VEIL];	// 結界
	// [152]

	static const u32 s_foptbl[2][3][4];	// 質フラグと機能フラグの変換テーブル
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_STATUS_ELENE_STAT_H_
