/***********************************************************************//**
 *	CharStat.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/17.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_STATUS_CHAR_STAT_H_
#define _SRCR_STATUS_CHAR_STAT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "../../tfw/serialize/SerializeBase.h"

#include "EleneStat.h"
#include "CharParam.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Rect;
typedef Rect<f32> RectF32;
class ExecCtx;
class RenderCtx;
class Texture;

TFW_END_NS

SRCR_BEGIN_NS

class EleneStat;
class EquipStat;
class TransStat;
class Unit;

/*---------------------------------------------------------------------*//**
 *	キャラクター ステータス
 *
**//*---------------------------------------------------------------------*/
class CharStat
{
	//======================================================================
	// 定数
public:
	// 状態
	enum Stat
	{
		STAT_NULL			= 0x0000,
		STAT_ALIVE			= 0x0001,			// 活動中
		STAT_DOWN			= 0x0002,			// 活動停止中
		MASK_STAT			= 0x000f,
	};
	// 特殊状態
	enum SpStat
	{
		SPSTAT_HEAVY		= 0x0010,			// 倦怠
		SPSTAT_SLEEP		= 0x0020,			// 眠り
		SPSTAT_CONFU		= 0x0040,			// 錯乱
		SPSTAT_TRANS		= 0x0100,			// 半覚醒
		MASK_SPSTAT			= 0xfff0,
	};

	// 生命エネルギー
	static const s32 LFENE_VALID_FIGURE			= 1000;					// 表示上の有効桁

	// 意識深度
	static const s32 CONCD_VALID_FIGURE			= 1000;					// 表示上の有効桁
	static const s32 CONCD_EL_MAX				= 2000000;				// １元素分の意識深度
	static const s32 CONCD_THRSLD_TRANCE		= 10000000;				// トランス状態の意識深度しきい値
	static const s32 CONCD_THRSLD_ZERO_ERROR	= 40000000;				// 誤差ゼロの意識深度しきい値
	static const s32 CONCD_TOTAL_MAX			= 100000000;			// 意識深度の最大値
	static const s32 CONCD_LAYER_NUM			= 50;					// 意識深度の意識レイヤー数
	static const s32 CONCD_HENL_MIN				= CONCD_TOTAL_MAX / 2;	// 半覚醒時の最低意識深度

	// チャージ率
	static const u16 CHARGECNT_MAX				= 60000;

	// 作成フラグ
	static const u8 CF_ITEM						= 0x01;		// アイテムである
	static const u8 CF_IGNORE_ATTACK			= 0x02;		// 攻撃無効
	static const u8 CF_NEGATIVE_ATRB			= 0x04;		// 反属性（純性でダメージを受ける）

	// 生命エネルギーから意識深度への変換率
	static const s32 RATIO_LFENE_TO_CONCD_DEFAULT	= 1024;		//128～1024:3.0- ///65536:1.3- ///1024:-1.3
	static const s32 RATIO_LFENE_TO_CONCD_MIN		= 128;

private:
	// フレームフラグ
	static const u8 FF_DIANA					= 0x01;		// 瞑想中
	static const u8 FF_CALMING					= 0x02;		// 静止中
	static const u8 FF_RUNNING					= 0x04;		// 走行中
	static const u8 FF_GUARDING					= 0x08;		// 防御中
	static const u8 FF_PERIODIC_EXEC			= 0x10;		// 間引き処理フレーム

	// 作成フラグ（非公開）
	static const u8 CF_NEW_EESTAT				= 0x10;		// EleneStat を内部で生成し、保持する

	//======================================================================
	// クラス
public:
	/*-----------------------------------------------------------------*//**
	 *	キャラクターステータスのセーブストラクチャ
	 *		※現在、_unitEfldMaster は保存しない仕様である
	 *
	**//*-----------------------------------------------------------------*/
	class SaveStructure
	{
	public:
		// [  0]
		u16							_flagsStat;			// [  2] 状態フラグ
		u8							_flagsFrame;		// [  1] フレームフラグ
		u8							_flagsCreate;		// [  1] 作成フラグ
		s32							_lfeneMax;			// [  4] 最大生命エネルギー
		s32							_damageEne;			// [  4] エネルギーダメージ値
		s32							_damagePhys;		// [  4] 物理ダメージ
		// [ 16]
		s32							_depthConc;			// [  4] 意識深度
		f32							_fcntPeriodic;		// [  4] 自然回復などの間引きフレームカウンタ
		u16							_cntCharge;			// [  2] チャージ率
		u8							__pad_0010[2];		// [  4]
		u16							_lv;				// [  2] レベル
		u16							_experience;		// [  2] 経験値
		// [ 32]
		CharParam					_chrprm;			// [ 16] 成長要素
		// [ 48]
		EleneStat::SaveStructure	_eessvst;			// [ 96] 元素エネルギー状態への参照
		// [144]
		static const s32 SIZE_THIS = 144;
		SaveStructure() { TFW_ZERO_MEMORY(this, sizeof(SaveStructure)); }
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 状態を得る
	inline Stat getStat() const { return (Stat)(_flagsStat & MASK_STAT); }
	// 特殊状態フラグを得る
	inline u16 getSpStatFlags() const { return _flagsStat & MASK_SPSTAT; }
	// 作成フラグを得る
	inline u8 getCreateFlags() const { return _flagsCreate; }
	// 現在の質フラグを得る
	inline u32 getQualityFlags() const { return _eeqf; }
	// 現在の興奮率を得る
	inline u16 getExcitednessRate() const { return _rateExct; }
	// 現在の倦怠率を得る
	inline u16 getDullnessRate() const { return _rateDull; }

	// 現在のエネルギーを得る
	inline s32 getEnergy() const { return _lfeneMax - _damagePhys - _damageEne; }
	// 最大エネルギー値を得る
	inline s32 getMaxEnergy() const {	return _lfeneMax; }
	// 現在のエネルギーダメージ値を得る
	inline s32 getEnergyDamage() const { return _damageEne; }
	// 現在の物理ダメージ値を得る
	inline s32 getPhysicsDamage() const { return _damagePhys; }

	// 現在のエネルギーを得る（表示用の短桁数値）
	inline s32 getEnergyShort() const { return (getEnergy() + LFENE_VALID_FIGURE - 1) / LFENE_VALID_FIGURE; }
	// 最大エネルギー値を得る（表示用の短桁数値）
	inline s32 getMaxEnergyShort() const {	return (getMaxEnergy() + LFENE_VALID_FIGURE - 1) / LFENE_VALID_FIGURE; }
	// 現在のエネルギーダメージ値を得る（表示用の短桁数値）
	inline s32 getEnergyDamageShort() const { return (getEnergyDamage() + LFENE_VALID_FIGURE - 1) / LFENE_VALID_FIGURE; }
	// 現在の物理ダメージ値を得る（表示用の短桁数値）
	inline s32 getPhysicsDamageShort() const { return (getPhysicsDamage() + LFENE_VALID_FIGURE - 1) / LFENE_VALID_FIGURE; }

	// 意識深度を得る
	inline s32 getConcentrateDepth() const { return _depthConc; }
	// 表示用意識深度を得る
//	inline s32 getConcentrateDepthForDisplay() const { return _depthConcDisp; }
	// チャージカウンタ値を得る（10000 で 100%）
	inline u16 getChargeCounter() const { return _cntCharge; }
	// 表示用チャージカウンタ値を得る（10000 で 100%）
//	inline u16 getChargeCounterForDisplay() const { return _cntChargeDisp; }
	// レベルを得る
	inline u16 getLevel() const { return _lv; }
	// 経験値を得る
	inline u16 getExperience() const { return _experience; }
	// 成長要素を得る
	inline const CharParam* getCharParam() const { return &_chrprm; }
	inline CharParam* charParam() {	return &_chrprm; }
	// 五元素を得る
	inline const EleneStat*	getEleneStat() const { return _eestat; }
	inline EleneStat* eleneStat() { return _eestat; }
	// 装備状態を得る
	inline const EquipStat* getEquipStat() const { return _eqstatRef; }
	// このステータスを持つユニットを得る
	inline Unit* getOwnerUnit() const { return _unitSelfRef; }

	// ガード中かを得る
	inline bool isGuarding() const { return TFW_IS_FLAG(_flagsFrame, FF_GUARDING); }
	// アクションフレーム数を得る
	f32 getActFrame() const;
	// 定期間引き処理フレームかどうかを得る
	inline bool isPeriodicFrame() const { return TFW_IS_FLAG(_flagsFrame, FF_PERIODIC_EXEC); }

	// 重さを得る
	s32 getWeight() const;

#if defined(_DEBUG)
	inline u16* debug_statFlags() { return &_flagsStat; }
#endif

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void dianaFrame();
	void calmingFrame();
	void runningFrame();
	void guardingFrame();

	s32 damageEnergy(s32 damageAdd, bool autoConcDepthDamage);
	s32 damageEnergy(s32 damageAdd) { return damageEnergy(damageAdd, true); }
	s32 damagePhysics(s32 damageAdd);
	void damageConcentrateDepth(s32 depthDamageConc);
	f32 calcLfDamageToConcdDamageRatio() const;
	s32 recoverEnergy(s32 recvAdd);
	s32 recoverPhysics(s32 recvAdd);
	#if defined(_DEBUG)
		inline void debug_damageEnergyDirect(s32 damageAdd) { damageEnergy(damageAdd); }
	#endif
	void revive(bool isBarely);
	void recoverFull();
	void naturation(s32 eerecv, bool isNoDamane);
	void calcVeilLimitElene(Elene* ee, s32 iveil);
	void clipVeilLimitElene(s32 iveil);
	bool strengthenVeil(const Elene* input, s32 iveil, Unit* unitMaster);
	bool weakenVeil(const Elene* input, s32 iveil);
	void resetVeil();
	void charge(u16 add);
	void resetCharge();
	void resetConcentrate();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	CharStat() : _flagsCreate(0)								{	clear();					}
	CharStat(bool isNewEleneStat);
	CharStat(const CharStat& src)								{	copy(&src);					}
	~CharStat();
	void copy(const CharStat* src);
	CharStat& operator =(const CharStat& src)					{	copy(&src);	return *this;	}
	void clear();
	void setup(const CharParam* chrprm, u8 flagsCreate, bool isPreserveBinding);
	void setup(u8 flagsCreate, bool isPreserveBinding);
	void levelup(const CharParam* chrprmNext);
	void bind(Unit* unitSelfRef, EleneStat* eestatRef, EquipStat* eqstatRef);
	void exec(const ExecCtx* ec);
	bool serialize(Serializer* ser) const;
	bool deserialize(Serializer* ser);
	bool storeToStructure(SaveStructure* svst) const;
	bool restoreFromStructure(const SaveStructure* svst);

private:
	void updateAutoInfluence(f32 frate);
	void updateConcentrateDepth(f32 frate);
	void setStat(Stat stat);
public:
	void checkStat();

#if defined(_DEBUG)
public:
	void _debug_set(s32 damageEne, s32 damagePhys, s32 lfeneMax);
#endif

	//======================================================================
	// 変数
private:
	u16				_flagsStat;			// 状態フラグ
	u8				_flagsFrame;		// フレームフラグ
	u8				_flagsFrameNext;	// 次のフレームフラグ
	u8				_flagsCreate;		// 作成フラグ
	s32				_lfeneMax;			// 最大生命エネルギー
	s32				_damageEne;			// エネルギーダメージ値
	s32				_damagePhys;		// 物理ダメージ
	s32				_depthConc;			// 意識深度
	s32				_depthConcDisp;		// 表示用意識深度
	u16				_lv;				// レベル
	u16				_experience;		// 経験値
	f32				_fcntPeriodic;		// 自然回復などの間引きフレームカウンタ
	u32				_eeqf;				// 現在の質
	u16				_rateExct;			// 興奮率
	u16				_rateDull;			// 倦怠率
	u16				_cntCharge;			// チャージ率（1000 で 100%）
	u16				_cntChargeDisp;		// 表示用チャージ率

	CharParam		_chrprm;			// 成長要素
	EquipStat*		_eqstatRef;			// 装備状態への参照
	Unit*			_unitSelfRef;		// 自らを保持するユニットへの参照
	Unit*			_unitEfldMaster;	// 瞬間結界マスターへの参照
	EleneStat*		_eestat;			// 元素エネルギー状態への参照
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_STATUS_CHAR_STAT_H_
