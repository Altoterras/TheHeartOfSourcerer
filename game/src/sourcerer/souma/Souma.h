/***********************************************************************//**
 *	Souma.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/04/07.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_SOUMA_SOUMA_H_
#define _SRCR_SOUMA_SOUMA_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "../source/SourceDef.h"

#include "../../es/EsExtParam.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Vector3;
typedef Vector3<f32> Vector3F;
class ExecCtx;

TFW_END_NS

ETK_BEGIN_NS

class Particle;

ETK_END_NS

namespace es
{
	class EsObject;
	class EsValue;
}
using namespace es;

SRCR_BEGIN_NS

class Conductor;
class Elene;
class Game;
class Unit;
class Spirit;
class SoumaScript;
class SoumaScriptContext;

/*---------------------------------------------------------------------*//**
 *	ソウマ
 *		※術者はこのインスタンスを持つことによってソウマを扱えるようになる
 *		　術者は Unit と Sprit（と CharStat） を持っている必要がある
 *
**//*---------------------------------------------------------------------*/
class Souma
{
	//======================================================================
	// 定数
public:
	// 最大クラスター数
	static const s32 NUM_CLUSTER_MAX = 2;

	// ソウマ通知 ID
	enum SoumaNotifyId
	{
		SNOTIFID_CHANTING_START,	// 詠唱開始
		SNOTIFID_CHANTING_BREAK,	// 詠唱中断
		SNOTIFID_SERVE,				// ソウマ発動
		SNOTIFID_WASTE_ENE,			// ソウマ発動時のエネルギー消費
		SNOTIFID_NOT_HAPPENED,		// 何も起きなかった
		SNOTIFID_ERROR_SOURCE,		// ソースエラー
		SNOTIFID_FORCE_WASTE_ENE,	// 中断時の強制エネルギー消費
		SNOTIFID_FORCE_FEEDBACK,	// 中断時の強制フィードバック
		SNOTIFID_ERROR_MSG,			// エラーメッセージ
	};

	// フラグ
	static const u16 F_USER_CUSTOMIZED_SOURCE	= 0x0001;	// ユーザー独自のソースである

	//======================================================================
	// クラス
private:

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 現在作成中のマジッククラスタ ID を得る
	inline u16 getCurClusterId() const { return _mcidtbl[_indexCurMc]; }
	// 作成中のマジッククラスタ ID を得る
	inline u16 getClusterId(int index) const { ASSERT((0 <= index) && (index < NUM_CLUSTER_MAX)); return _mcidtbl[index]; }
	// 実行中かどうか得る
	bool isRunning() const;
	// オーナーのユニットを得る
	inline const Unit* getOwnerUnit() const { return _unitRef; }
	inline Unit* ownerUnit() const { return _unitRef; }
	// オーナーのスピリットを得る
	inline const Spirit* getOwnerSpirit() const { return _sprtRef; }
	inline Spirit* ownerSpirit() const { return _sprtRef; }
	// 呼び出し可能元素量を得る
	inline const Elene* getEvokableElene() const { return _eleneEvokable; }
	// フラグを得る
	inline u32 getFlags() const { return _flags; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void getState(const VcString** code, bool* isRun, s32* idxBegin, s32* idxEnd) const;
	const VcString* getCode() const;

	bool start(const VcString* srccode, SourceDef::SourceKind srckind, u16 flags);
	bool end(bool isNoEffect);
	bool terminate();
	void wait(f32 frame);
	void slowChant();

#if 0
	// 有効なハンドルか判定する
	inline bool isValidHandle(u32 handle) { return (HANDLE_MIN <= handle) && (handle <= HANDLE_MAX); }
#endif

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	Souma();
	virtual ~Souma();
	virtual bool create(Spirit* sprtRef, Unit* unitRef);
	virtual void destroy();
	void exec(bool* isStep, const ExecCtx* ec);

public:
	s32 doError(s32 retcode);
	s32 doFatalError(s32 retcode);
	s32 doMistake(s32 retcode);

	bool prepare();
	bool isPrepared();
	bool meditate();
	s32 makeCluster(u16* mcid, const Elene* eleneMakeOql, const Elene* elenePreset, u32 funcflagsPreset);
	bool checkCluster(u16 mcid, bool setCurrent);
	void releaseCluster(u16 mcid);

#if 0
	s32 descendElement(s32 elTo, s32 eeTo);
#endif

#if 0
	s32 convHandleStrToUnitId(u16* unitid, Unit** unit, const VcString* str);
	s32 convHandlePropStrToUnitId(u16* unitid, Unit** unit, const VcString* strProp, Unit* unitGlobal, s32 ieqp);
	bool convHandleToUnit(Unit** unit, u32 handle);
#endif

private:
	Particle* appearParticle(u16 ptcldid, const Vector3F* pos, u32 flags);
	void calcMagicClusterOffset(Vector3F* offset, f32 xofs, f32 radius) const;

	//======================================================================
	// 変数
protected:
	// 基本設定変数
	SoumaScriptContext* _ssctx;
	SoumaScript* _scriptRef;
	Spirit* _sprtRef;
	Unit* _unitRef;
	u16 _flags;

	// 作業変数
	f32 _frmcntStep;				// フレームカウンタ
	f32 _frmcntChargePtcl;			// チャージパーティクルのフレームカウンタ
	f32 _frmDelay;					// 詠唱遅延値
	f32 _frmdcntWait;				// 待機フレーム減算カウンタ
	u16 _mcidtbl[NUM_CLUSTER_MAX];	// クラスタ ID テーブル
	s32 _indexCurMc;				// 現在のクラスターインデックス
	s32 _seidMc;					// クラスター作成の SE ID
	Elene* _eleneEvokable;			// 呼び出し可能元素量
	bool _isMistake;				// 実行ミスが起きた
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_SOUMA_SOUMA_H_
