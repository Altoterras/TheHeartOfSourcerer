/***********************************************************************//**
 *	MySpirit.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/02/05.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EPISODE_CHAR_PC_MY_SPIRIT_H_
#define _SRCR_EPISODE_CHAR_PC_MY_SPIRIT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../../spirit/char/pc/PcMode.h"
#include "../../../spirit/char/pc/PcRole.h"
#include "../../../spirit/char/pc/PcSpirit.h"
#include "../../../es/EsExtParam.h"

////////////////////////////////////////////////////////////////////////////
// クラス

namespace es
{
	class Es;
}
using namespace es;

SRCR_BEGIN_NS

class AriaWindow;
class CharStat;
class GazeCam;
class MyUnit;
class MyMode;
class Souma;
class SourceManager;

/*---------------------------------------------------------------------*//**
 *	主人公（プレイヤーキャラクタ１）ロジック
 *
**//*---------------------------------------------------------------------*/
class MySpirit : public PcSpirit
{
	//======================================================================
	// クラス
public:
	/*-----------------------------------------------------------------*//**
	 *	主人公（プレイヤーキャラクタ１）ロール
	 *
	**//*-----------------------------------------------------------------*/
	class MyRole : public PcRole
	{
		//==================================================================
		// 定数
	public:
		enum CommandType
		{
			NOTHING = 1,		// 何もしない
			WALK_NORMAL,		// 通常歩行
			CHANTING,			// ソウマ詠唱
			USE_ITEM,			// アイテム使用
			PICKUP_ITEM,		// アイテム拾い
			ENTER_CF,			// 対決開始
			LEAVE_CF,			// 対決終了
		};
	
		//==================================================================
		// メソッド

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// アクセサ
	public:
		// 自らの型のスピリットを得る
		inline MySpirit* getThisSpirit() { return (MySpirit*)_sprtRef; }

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// 内部制御
	public:
		MyRole(MySpirit* sprtRef);
		~MyRole();

	protected:
		bool create(Mode* modeOwnerRef);
		void destroy();
		int execCommand(ExecRes* res, const ExecCtx* ec, const CmdParam* param);
		int execCommand_walkNormal(ExecRes* res, const ExecCtx* ec, const CmdParam* param);
		int execCommand_chanting(ExecRes* res, const ExecCtx* ec, const CmdParam* param);
		void notifyEnd();

	public:
		void notifySoumaAction(s32 snotifid, const StatusChangeResult* scres, const VcString* msg);

	private:
		void endChanting();
		void restoreGazeCamera();
		void updateChantingPerf(bool isChantingPerf);

		//==================================================================
		// 変数
	private:
		GazeCam* _camGaze;			// 詠唱中の専用カメラ
		u16 _camidBk;				// 専用カメラ変更前のカメラ ID
		bool _isChantPerf;			// 詠唱演出中
		AriaWindow* _wndAriaRef;	// 詠唱ウインドウへの参照
	};

	/*-----------------------------------------------------------------*//**
	 *	主人公（プレイヤーキャラクタ１）ロール管理
	 *
	**//*-----------------------------------------------------------------*/
	class MyMode : public PcMode
	{
		//==================================================================
		// メソッド

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// アクセサ
	public:
		// 自らの型のロールを得る
		inline MyRole* getThisRole() { return (MyRole*)_role; }

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// 外部サービス
	public:
		void toAria();
	
		bool action();

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// 内部制御
	public:
		bool create(MySpirit* sprtRef);

	protected:
		void notifyChildModeBegin(short modeid, void* objParamRef);

	private:
		bool judgeConfrontAutoBehavior();
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// ユニットを得る
	inline MyUnit* getThisUnit() const { return (MyUnit*)getUnit(); }
	// ソウマを得る
	virtual Souma* getSouma() const { return _souma; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
#if 0
	void setFocusUnit(Unit* unit, bool isByGui);
#endif

	void startBehavior(Behavior behv, const BehaviorParam* bparam);
	void breakBehavior(Behavior behv, bool isByGui, bool duetoConfu);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	MySpirit();
	bool create();
	void destroy();
	bool bind(CharUnit* cunitSelfRef, const CharDef* chrdef, const CharLogicData* chrlgd);
	void notifyChangeMapBefore();
	void notifyInfluenceStat(const StatusChangeResult* scres, const TransStat* tstat);
	void notifySoumaAction(s32 snotifid, const StatusChangeResult* scres, const VcString* msg);
	void notifyChangedConfront(bool isConfront, Confront* confront, Unit* unitTrg, Unit* unitEntrant, bool isAddition);

private:
	bool startSouma(const VcString* srccode, u8 srckind, bool isUserCustomizedSource);
public:
	bool endSouma();
	bool execSouma(const ExecCtx* ec, const VcString** code, bool* isStep, bool* isRun, s32* idxBegin, s32* idxEnd);

	//======================================================================
	// 変数
protected:
	Souma* _souma;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EPISODE_CHAR_PC_MY_SPIRIT_H_
