/***********************************************************************//**
 *	Confront.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/03/26.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_CONFRONT_CONFRONT_H_
#define _SRCR_CONFRONT_CONFRONT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

////////////////////////////////////////////////////////////////////////////
// クラス

SRCR_BEGIN_NS

class GameActMsgWindow;
class StatusChangeResult;
class CharUnit;
class Game;
class PlacementObj;
class Unit;
class UnitArray;

/*---------------------------------------------------------------------*//**
 *	対決（バトル）
 *		※	実質、ConfrontMode の媒介モジュール
 *			ゲーム（モード）としての外郭は ConfrontMode にあるが、
 *			各モジュールとのコミュニケーションは ConfrontMode 向きではない
 *			従って、このモジュールが対決時の総合処理を行う．
 *			媒介とはいえ、ConfrontMode よりも主体的であり、
 *			力関係的にも強い、とする．
 *
**//*---------------------------------------------------------------------*/
class Confront
{
	//======================================================================
	// 定数
public:
	const static u16	BGMID_DEFAULT	= 0;
	const static u16	BGMID_NO_CHANGE	= 0xffff;	// BGM を変更しない

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 対決中かどうかを得る
	inline bool isConfrontNow() const { return _isConfront;	}
	// 対決対象を得る
	inline const UnitArray* getTargets() const { return _uarrCfTrg; }
	// 主催ユニットを得る
	inline const Unit* getPromoterUnit() const { return _unitPromoter; }
	// 対象が全てダウンしたかどうか
	inline bool isAllTargetDown() const { return _isAllTrgDown;	}
	// 自キャラがダウンしたかどうか
	inline bool isMyUnitDown() const { return _isMyUnitDown; }
	// 勝利後実行のイベント ID を得る
	inline u16 getAfterWinEvid() const { return _evidAfterWin; }
	// 敗北後実行のイベント ID を得る
	inline u16 getAfterLoseEvid() const { return _evidAfterLose; }
	// 制御イベント ID を得る
	inline u16 getCtrlEvid() const { return _evidCtrl; }
	// 対決のフレームカウンタを取得する
	inline f32 getFrameCount() const { return _frameCnt; }

	/*
	// 逃げる防止を設定する
	inline void setPreventEscape(bool isPreventEscape) { _isPreventEscape = isPreventEscape; }
	// 勝利後実行のイベント ID を設定する
	inline void setAfterWinEvid(u16 evid) { _evidAfterWin = evid; }
	// 敗北後実行のイベント ID を設定する
	inline void setAfterLoseEvid(u16 evid) { _evidAfterLose = evid; }
	*/
	// 対決後のゲームモードを設定
///	inline void setConfrontAfterGameMode(s32 gmk) { _gmkAfterConfront = gmk; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void start(Unit* unitConfront, u16 evidCtrl, const Unit* unitCaller, u16 bgmid, u16 evidAfterWin, u16 evidAfterLose, bool isPreventEscape);
	void start(Unit* unitConfront, u16 evidCtrl, const Unit* unitCaller, const PlacementObj* pobjSetting);
	void start(Unit* unitConfront, u16 evidCtrl, const Unit* unitCaller);
	void end(bool* isBegunEvent);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	Confront();
	~Confront();
	bool create();
	void destroy();
	void exec(ExecRes* res, const ExecCtx* ec);

private:
	void startConfront(Unit* unitTrgRef, u16 evidCtrl, const Unit* unitCaller, u16 bgmid);
	void addConfront(Unit* unitTrgRef, u16 evidCtrl, const Unit* unitCaller, u16 bgmid);
	bool endConfront();
	bool checkAllTargetDown() const;
	void dropItem(CharUnit* unit);

	//======================================================================
	// 変数
protected:
	UnitArray* _uarrCfTrg;				// 対決相手の配列
	const Unit* _unitPromoter;			// 主催者
	bool _isConfront;					// 対決中かどうか
	bool _isAllTrgDown;					// 対象が全てダウンしたかどうか
	bool _isMyUnitDown;					// 自キャラがダウンしたかどうか
	bool _isPreventEscape;				// 逃げる防止
	u16 _bgmidCur;						// 現在の BGM ID
	u16 _evidAfterWin;					// 勝利後に実行するイベント ID
	u16 _evidAfterLose;					// 敗北後に実行するイベント ID
	u16 _evidCtrl;						// 制御イベント ID
	f32 _frameCnt;						// フレームカウンタ
///	s32 _gmkAfterConfront;				// 対決モード後のゲームモード
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_CONFRONT_CONFRONT_H_
