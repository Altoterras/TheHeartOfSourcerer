/***********************************************************************//**
 *	MagicCluster.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/17.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_MAGIC_MAGIC_CLUSTER_H_
#define _SRCR_MAGIC_MAGIC_CLUSTER_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "MagicClusterCreateParam.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Color;
typedef Color<u8> ColorU8;
template<class TYPE> class Vector3;
typedef Vector3<f32> Vector3F;
class ExecCtx;
class RenderCtx;

TFW_END_NS

SRCR_BEGIN_NS

class FocusArray;
class MagicSys;
class TransStat;
class Elene;

/*---------------------------------------------------------------------*//**
 *	マ法クラスター
 *
**//*---------------------------------------------------------------------*/
class MagicCluster
{
	//======================================================================
	// 定数
public:
	// 消滅フラグ
	static const u16		DCF_COLL_UNIT		= 0x0001;	// ユニットと衝突した
	static const u16		DCF_COLL_GCOL		= 0x0002;	// 地形と衝突した
	static const u16		DCF_OUT_MAP			= 0x0004;	// マップから出た
	static const u16		DCF_IMMATURE_SOUMA	= 0x0008;	// 未熟なソウマ
	static const u16		DCF_SELF_VANISH		= 0x0010;	// 自己消滅
	static const u16		DCF_ZERO_LIFE		= 0x0020;	// 生存期間による
	static const u16		DCF_END_SYS			= 0x0100;	// システム終了
	static const u16		DCF_IS_CREATION		= 0x1000;	// 創作中クラスターである

	// 設定フラグ（作成時から消滅時まで有効な設定）
	static const u16		CONFF_HAS_POS_VEL	= 0x0001;	// 位置と速度ベクトルを自らが持つ
	static const u16		CONFF_SHOW_FC		= 0x0002;	// フォーカスカーソルを表示する

	// 動作フラグ（途中で切り替わる可能性のある動作設定）
	static const u16		CTRLF_VALID			= 0x0001;	// 有効である
	static const u16		CTRLF_NO_COLL		= 0x0002;	// 当たりをとらない
	static const u16		CTRLF_ONLY_TRG_COLL	= 0x0004;	// 対象のみ当たりをとる
	static const u16		CTRLF_DIR_TO_ROTZ	= 0x0008;	// 進行方向を Z 軸回転に反映させる
	static const u16		CTRLF_EXTINCTION	= 0x0010;	// 消滅中

	// 生存フレーム
	static const s32		LIFEFRAME_INFINITY	= -1;		// 無限

	// クラスターの限界値
	static const f32		SPEED_MC_MIN;
	static const f32		SPEED_MC_MAX;

protected:
	static const u32		NUM_PTCL_MAX		= 8;		// 最大保持パーティクル数
	static const u16		NUM_FOCUS_MAX		= 16;		// 最大保持フォーカス数

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// クラスター ID を得る
	inline u16 getId() const { return _mcid; }
	// 設定フラグを得る
	inline u16 getConfFlags() const { return _confflags; }
	inline u16* confFlags() { return &_confflags; }
	// 動作フラグを得る
	inline u16 getCtrlFlags() const { return _ctrlflags; }
	inline u16* ctrlFlags() { return &_ctrlflags; }
	// 消滅フラグを得る
	inline u16 getSelfDisappearFlags() const { return _dcflags; }
	// 有効かどうかを得る
	inline bool isValid() { return TFW_IS_FLAG(_ctrlflags, CTRLF_VALID); }
	// 消化済みユニット数を得る
	inline s32 getDissolvedUnitCount() const { return _cntDissolvedUnit; }
	// 位置を得る
	inline const Vector3F* getPosition() const { return _pos; }
	// 伝達ステータスを得る
	inline const TransStat* getTransStat() const { return _tstat; }
	inline TransStat* transStat() { return _tstat; }
	// 既定の目標元素量を得る（無い場合は NULL が返る）
	inline const Elene* getPresetElene() const { return _elenePreset; }
	// 既定の機能フラグを得る
	inline u32 getPresetFuncFlags() const { return _funcflagsPreset; }
	// オーナーユニットを得る（無い場合は NULL が返る）
	inline Unit* getOwnerUnit() const { return _unitOwnrRef; }
	// パーティクル ID を得る
	inline s16 getParticleId(s32 idx) const { return _arrptclid[idx]; }
	// フォーカス配列を得る
	inline const FocusArray* getFocusArray() const { return _focusarr; }
	inline FocusArray* focusArray() const { return _focusarr; }
	// 半径を得る
	virtual f32 getRadius() const = 0;
	// 見た目上の半径を得る
	virtual f32 getLookRadius() const { return getRadius(); }

	// 位置を設定する
	void setPosition(const Vector3F* pos);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	virtual void setLifeFrame(f32 frame);
	virtual void setInitialVelocity(const Vector3F* vel);
	virtual void setMaxSpeed(f32 speed);
	virtual void accelerate(const Vector3F* acc);
	virtual void addFocusUnit(Unit* unit, bool isReset);
	virtual void setColor(const ColorU8* color);
	virtual void reflectPosture();
	void setPresetElene(const Elene* elenePreset);
	void addDissolvedUnitCount() { _cntDissolvedUnit++; }
	bool checkHealth();
	bool materialize(u32 form);
	bool synthesize(MagicCluster* mcMul);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	MagicCluster();
	virtual ~MagicCluster();
	virtual bool create(MagicSys* mgcsysOwnRef, u16 mcid, const MagicClusterCreateParam* mccprm);
	virtual void destroy();
	virtual void exec(const ExecCtx* ec);		// フレーム制御

protected:
	virtual void execExtinction(const ExecCtx* ec);
	virtual bool appearParticles(const MagicClusterCreateParam* mccprm) = 0;
	virtual bool hitUnit(const ExecCtx* ec, Unit* unit, const TransStat* tstat);
	inline bool hitUnit(const ExecCtx* ec, Unit* unit) { return hitUnit(ec, unit, _tstat); }
	virtual void disappearSelf(u16 dcflags, f32 frameExtinction);

	//======================================================================
	// 変数
protected:
	u16 _mcid;						// クラスター ID
	u16 _confflags;					// 設定フラグ
	u16 _ctrlflags;					// 動作フラグ
	u16 _dcflags;					// 消滅フラグ
	u16 _arrptclid[NUM_PTCL_MAX];	// パーティクル ID
	s32 _cntDissolvedUnit;			// 消化で吸収したユニット数
	Vector3F* _pos;					// 位置
	Vector3F* _vel;					// 速度
	f32 _speedMax;					// 最大速さ
	f32 _fcntElapsed;				// 経過フレームカウンタ
	f32 _frameLife;					// 生存フレーム数
	f32 _fcntExtinction;			// 消滅演出フレームカウンタ
	f32 _frameExtinctionMax;		// 消滅演出フレームカウンタ数
	TransStat* _tstat;				// 伝達ステータス
	Elene* _elenePreset;			// 既定の元素量
	u32 _funcflagsPreset;			// 既定の機能フラグ
	FocusArray* _focusarr;			// フォーカス配列
	Unit* _unitOwnrRef;				// オーナー ユニット
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_MAGIC_MAGIC_CLUSTER_H_
