/***********************************************************************//**
 *	CharUnit.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/07/15.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_UNIT_CHAR_CHAR_UNIT_H_
#define _SRCR_UNIT_CHAR_CHAR_UNIT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Unit.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Vector2;
typedef Vector2<f32> Vector2F;
template<class TYPE> class Point;
typedef Point<f32> PointF32;
class ShapeModel;

TFW_END_NS

SRCR_BEGIN_NS

class CharDef;
class CharStat;
class CharUnitSaveStructure;
class CircleShadow;
class EquipStat;

/*---------------------------------------------------------------------*//**
 *	マップ クラス
 *
**//*---------------------------------------------------------------------*/
class CharUnit : public Unit
{
	//======================================================================
	// 定数
public:
	// 移動状態
	enum MoveState
	{
		MSTAT_NULL,
		MSTAT_STOP,
		MSTAT_WALK,
		MSTAT_RUN,
		MSTAT_DOWN,
	};

	// キャラクタ クラス（職業）
	enum CharClass
	{
		CHARCLS_NULL,
		CHARCLS_SOURCERER,		// ソウマ使い
		CHARCLS_FIGHTER,		// 戦士
		CHARCLS_ENHANCER,		// エンハンサー（強化モンスター）
	};

	// アニメーション ID
	static const s32	ANIMID_STOP				= 1;		// 停止
	static const s32	ANIMID_WALK				= 2;		// 歩行
	static const s32	ANIMID_RUN				= 3;		// 走り
	static const s32	ANIMID_CF_WALK			= 4;		// 対決時歩行
	static const s32	ANIMID_CF_RUN			= 5;		// 対決時走り
	static const s32	ANIMID_TO_DOWN			= 7;		// ダウンへ
	static const s32	ANIMID_CF_ENTER			= 12;		// 対決開始構え
	static const s32	ANIMID_CF_LEAVE			= 13;		// 対決解除
	static const s32	ANIMID_CF_STOP			= 14;		// 対決時停止
	static const s32	ANIMID_ATTACK_1			= 21;		// 攻撃１
	static const s32	ANIMID_ATTACK_2			= 22;		// 攻撃２
	static const s32	ANIMID_ATTACK_3			= 23;		// 攻撃３
	static const s32	ANIMID_GUARD_1			= 24;		// 防御１
	static const s32	ANIMID_GUARD_2			= 25;		// 防御２
	static const s32	ANIMID_GUARD_3			= 26;		// 防御３
	static const s32	ANIMID_MAGIC_CHANT_1	= 30;		// ソウマ詠唱（小） 30frame ループ
	static const s32	ANIMID_MAGIC_EMIT_1		= 33;		// ソウマ発動（小） 30frame
	static const s32	ANIMID_SKILL_1			= 42;		// 技１
	static const s32	ANIMID_SKILL_2			= 43;		// 技２
	static const s32	ANIMID_SKILL_3			= 44;		// 技３
	static const s32	ANIMID_SKILL_4			= 45;		// 技４
	static const s32	ANIMID_PICKUP_ITEM		= 48;		// アイテムを拾う
	static const s32	ANIMID_USE_ITEM			= 49;		// アイテムを使う
	static const s32	ANIMID_CHARGE_START_1	= 51;		// 溜め開始１
	static const s32	ANIMID_CHARGE_START_2	= 52;		// 溜め開始２
	static const s32	ANIMID_CHARGE_START_3	= 53;		// 溜め開始３
	static const s32	ANIMID_CHARGING_1		= 54;		// 溜め中１
	static const s32	ANIMID_CHARGING_2		= 55;		// 溜め中２
	static const s32	ANIMID_CHARGING_3		= 56;		// 溜め中３
	// アニメーション補間フレーム
	static const s32	ANIMFRAME_INTP			= 10;

	// 動作フラグ
	static const u32	F_IGNORE_KEEPOUT		= 0x00000001;	// 進入禁止当たりを無視する

	// クエストフラグ
	static const u16	QSF_ACCEPTABLE			= 0x0001;		// このユニットからクエストを受託できる
	static const u16	QSF_CANCELABLE			= 0x0002;		// このユニットからクエストをキャンセルできる
	static const u16	QSF_FINISHABLE			= 0x0004;		// このユニットからクエストをクリアできる

protected:
	// execChangeMoveAnimationAndState 用の移動状態フラグ
	static const u16	CMF_MOVABLE				= 0x0001;
	static const u16	CMF_CONFRONT			= 0x0002;
	static const u16	CMF_IMMEDIATELY			= 0x0100;		// 即座に

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 元素エネルギーを得る
	virtual const EleneStat* getEleneStat() const { return _eestat; }
	virtual EleneStat* eleneStat() { return _eestat; }
	// ステータスを得る
	inline const CharStat* getCharStat() const { return _cstat; }
	inline CharStat* charStat() { return _cstat; }
	// キャラクタ定義を得る（無い場合は NULL が返る）
	inline const CharDef* getCharDef() const { return _chrdefRef; }
	// 速度を得る
	inline const Vector3F* getVelocity() const { return _vel; }
	// 方向を得る
	inline const Vector3F* getDirection() const { return _dir; }
	// 移動目標位置を得る
	inline const Vector3F* getTargetPosition() const { return _posTarget; }
	// 中心位置を得る
	virtual const Vector3F* getCenterPos() const { return _posCenter; }
	// スピリットを得る
	inline const Spirit* getSpirit() const { return _sprtSelfRef; }
	inline Spirit* spirit() const { return _sprtSelfRef; }
	// キャラクタクラスを得る
	inline CharClass getCharClass() const { return _charcls; }
	// 代表名を得る（無い場合は NULL が返る）
	virtual const VcString* getMainName() const;
	// 現在のアニメーション ID を得る
	u16 getCurrentAnimId() const;
	// クエストフラグを得る
	inline u16 getQuestFlags() const { return _qsflags; }

	// クエストフラグを設定する
	inline void setQuestFlags(u16 qsflags) { _qsflags = qsflags; }
	inline void setQuestFlags(u16 qsflags, bool isValid) { TFW_SET_FLAG(_qsflags, qsflags, isValid); }

protected:
	// シェイプモデルを得る
	inline ShapeModel* getShapeModel() const { return (ShapeModel*)getModel(0); }
	// 最大速さを得る
	inline f32 getMaxVelocity() const { return _velMax; }
	// 最大速さの２乗値を得る
	inline f32 getMaxVelocityPw() const { return _velMaxSq; }
	// 走り状態になる最大速さに対する割合
	inline f32 getRunVelocityRatio() const { return _ratioRun; }
	// 速度に対する摩擦率（非加速フレーム時）
	inline f32 getFrictNormalRatio() const { return _ratioFrictNrm; }
	// 速度に対する摩擦率（加速フレーム時）
	inline f32 getFrictAccelRatio() const { return _ratioFrictAcc; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void walk(const Vector2F* accXz, bool isAllowRun);
	virtual void changeAnim(u16 animid, bool isLoop, bool isChangeFirstFrame, bool isChangeEndFrame);
	inline void changeAnim(u16 animid, bool isLoop) { changeAnim(animid, isLoop, false, false); }
	virtual void setEnable(bool isEnable);
	virtual void setPosition(const Vector3F* pos);
	virtual void setDirection(const Vector3F* dir);
	void setTargetPosition(const Vector3F* pos);
	void setVelocityParam(f32 velMax, f32 ratioRun, f32 ratioFrictNrm, f32 ratioFrictAcc);

	virtual void execWalking(const ExecCtx* ec);
	virtual bool execChangeMoveAnimationAndState(const ExecCtx* ec);
	virtual void execChangeMoveAnimationToDown(const ExecCtx* ec, u16 cmflags);
	virtual void execChangeMoveAnimationToStop(const ExecCtx* ec, u16 cmflags);
	virtual void execChangeMoveAnimationToRun(const ExecCtx* ec, u16 cmflags);
	virtual void execChangeMoveAnimationToWalk(const ExecCtx* ec, u16 cmflags);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	CharUnit(UnitType unittype);
	virtual ~CharUnit();
	virtual bool create(Model* mdlRef, Spirit* sprtRef, u16 chrdid, u16 unitflags, CharClass charclass, bool isHasCircleShadow);
	virtual void destroy();
	virtual void exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御
	virtual void render(const RenderCtx* rc);			// 描画

	virtual void notifyChangeMapBefore();
	virtual void notifyChangedMap();
	virtual void notifyChangedConfront(bool isConfront, Confront* confront, Unit* unitTrg, Unit* unitEntrant, bool isAddition);
	virtual void notifyChangedEventMode(bool isEventMode);

	bool storeToStructure(CharUnitSaveStructure* svst) const;
	bool restoreFromStructure(const CharUnitSaveStructure* svst);
	void reflectParameterFromDefData(u16 unitflags);

protected:
	virtual bool checkBeforeMove(const Vector3F* pos, s32 idxPlane) { return true; }
	virtual void checkAfterMove(const Vector3F* pos, s32 idxPlaneCur, s32 idxPlanePrev)	{}
	virtual void setShadowPos(const Vector3F* pos);

private:
	void setPosition(const Vector3F* pos, s32 idxPlane);

	//======================================================================
	// 変数
protected:
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// モデル表示系変数
	CircleShadow*		_mdlCrcShdw;			// 丸影
	Vector3F*			_posCenter;				// 中心位置
	Vector3F*			_vel;					// 速度
	Vector3F*			_dir;					// 方向
	Vector3F*			_posTarget;				// 目標位置
	f32					_heightCenter;			// 中央位置高さ
	s32					_idxCurGcolPlane;		// 現在の地面コリジョン平面インデックス
	f32					_rateIsShadowCur;		// 現在の影中率
	f32					_rateIsShadowTrg;		// 影中率の目標値
	bool				_isAccFrame;			// 今フレームに加速したかどうか
	bool				_isHitKeepoutColl;		// 進入禁止コリジョンに触れた
	u32					_eflags;				// 制御フラグ
	MoveState			_mstat;					// 移動状態
	f32					_fdcntStopAnim;			// 停止アニメーション調整カウンタ
	u16					_qsflags;				// クエストフラグ

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 論理動作系変数
	EleneStat*			_eestat;				// エネルギー状態
	CharStat*			_cstat;					// キャラクタステータス
	Spirit*				_sprtSelfRef;			// スピリットへの参照
	const CharDef*		_chrdefRef;				// キャラクタ定義
	CharClass			_charcls;				// キャラクタクラス

private:
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// モデル表示系変数
	f32					_velMax;				// 最大速さ
	f32					_velMaxSq;				// 最大速さの二乗
	f32					_ratioRun;				// 走り状態になる最大速さに対する割合
	f32					_ratioFrictNrm;			// 速度に対する摩擦率（非加速フレーム時）
	f32					_ratioFrictAcc;			// 速度に対する摩擦率（加速フレーム時）
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_UNIT_CHAR_CHAR_UNIT_H_
