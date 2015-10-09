/***********************************************************************//**
 *	Spirit.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/12/13.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "Spirit.h"

// Friends
#include "SpiritMode.h"
#include "../confront/Confront.h"
#include "../body/Conductor.h"
#include "../body/Game.h"
#include "../body/GameExecCtx.h"
#include "../body/GameFixedStrTbl.h"
#include "../souma/Souma.h"
#include "../status/ActStat.h"
#include "../status/CharStat.h"
#include "../status/StatusChangeResult.h"
#include "../unit/Unit.h"
#include "../unit/char/CharUnit.h"

// External

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// Spirit メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	行動を開始する
**//*---------------------------------------------------------------------*/
void Spirit::startBehavior(Behavior behv, const BehaviorParam* bparam)
{
	if(_behvCur == behv)	{	return;	}

	Behavior behvPrev = _behvCur;
	_behvCur = behv;

	// 行動フレーム数のリセット
	resetBefaviorFrame();

	// 子に通知
	onChangeBehavior(_behvCur, behvPrev, false, false);
}

/*---------------------------------------------------------------------*//**
	行動を中断する
**//*---------------------------------------------------------------------*/
void Spirit::breakBehavior(Behavior behv, bool isByGui, bool duetoConfu)
{
	Behavior behvPrev = _behvCur;
	if((_behvCur == behv) || (behv == BEHV_NULL))
	{
		// 行動をデフォルトに
		_behvCur = BEHV_DEFAULT;

		// 行動フレーム数のリセット
		resetBefaviorFrame();

		// 子に通知
		onChangeBehavior(_behvCur, behvPrev, true, duetoConfu);
	}
}

/*---------------------------------------------------------------------*//**
	行動中かどうかを得る
**//*---------------------------------------------------------------------*/
bool Spirit::isBehaviorActing() const
{
	if(_mode == 0L)	{	return false;	}
	return _mode->isActing();
}

/*---------------------------------------------------------------------*//**
	行動フレーム数のリセット
**//*---------------------------------------------------------------------*/
void Spirit::resetBefaviorFrame()
{
	_frameBehvCur = 0.0f;
	_frameBehvMax = 0.0f;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
Spirit::Spirit(SpiritType sprttype)
	: _sprttype(sprttype)
	, _astat(0L)
	, _mode(0L)
	, _cstatRef(0L)
	, _behvCur(BEHV_NULL)
	, _frameBehvCur(0.0f)
	, _frameBehvMax(0.0f)
	, _unitCfrntRef(0L)
	, _unitFocusRef(0L)
	, _unitTalkRef(0L)
	, _chrdefRef(0L)
	, _chrlgdRef(0L)
	, _unitSelfRef(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
Spirit::~Spirit()
{
	ASSERT(_astat == 0L);
	ASSERT(_mode == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool Spirit::create(SpiritMode* modeEntr)
{
	// アクションステータスを作成
	_astat = new ActStat();

	// 行動をデフォルトに
	_behvCur = BEHV_DEFAULT;

	// モードを設定
	_mode = modeEntr;
	if(_mode != 0L)
	{
		_mode->begin(0L);
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void Spirit::destroy()
{
	// アクションステータスを削除
	if(_astat != 0)
	{
		delete _astat;
		_astat = 0L;
	}

	// モードの破棄
	if(_mode != 0L)
	{
		_mode->end(0L);
		_mode->destroy();
		delete _mode;
		_mode = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	作成された自らのキャラユニットをバインド
**//*---------------------------------------------------------------------*/
bool Spirit::bind(CharUnit* cunitSelfRef, const CharDef* chrdef, const CharLogicData* chrlgd)
{
	// 参照を保存する
	_unitSelfRef = cunitSelfRef;
	_chrdefRef = chrdef;
	_chrlgdRef = chrlgd;
	_cstatRef = _unitSelfRef->charStat();

	// アクションフレーム数を設定しておく
	f32 frameFirst = _cstatRef->getActFrame();	// すぐにアイテム使用などのアクションが実行出来るように、カウンタはゼロから開始する
	_astat->setup(frameFirst, 0.0f, frameFirst);

	return true;
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void Spirit::exec(ExecRes* res, const ExecCtx* ec)
{
	if(_unitSelfRef->isEnable())
	{
		if(!((GameExecCtx*)ec)->isLockOperating())
		{
			// 索敵
			if(isConfront())
			{
				execSearchConfrontTarget(ec);
			}

			// 現在参照している相手ユニットが無効の場合は、参照を外す
			if((_unitFocusRef != 0L) && !_unitFocusRef->isEnable())
			{
				setFocusUnit(0L, false);
			}
			if((_unitTalkRef != 0L) && !_unitTalkRef->isEnable())
			{
				setTalkUnit(0L);
			}

			// モードとアクション
			if(_mode != 0L)
			{
				// モード処理
				_mode->exec(res, ec);

				if(_astat->isWaitFrame())	// 次のアクションまでの待ち中
				{
					// 特殊状態がアクションに影響する
					if(_cstatRef->getStat() == CharStat::STAT_ALIVE)
					{
						u16 spstatFlags = _cstatRef->getSpStatFlags();
						if(TFW_IS_FLAG(spstatFlags, CharStat::SPSTAT_SLEEP))
						{
							// 眠り
						}
						else
						{
							f32 rateDeltaFrame = 1.0f;
					
							// 倦怠による悪化
							// ※『興奮と倦怠による影響.ods』参照
							if(_cstatRef->getDullnessRate() > 100)
							{
								rateDeltaFrame -= (_cstatRef->getDullnessRate() - 100) * 0.01f;
								if(rateDeltaFrame < 0.0f)	{	rateDeltaFrame = 0.0f;	}
							}

							// 興奮による加速
							// ※『興奮と倦怠による影響.ods』参照
							if(_cstatRef->getExcitednessRate() > 100)
							{
								rateDeltaFrame += (_cstatRef->getExcitednessRate() - 100) * 0.002f;
							}

							// フレームを進める
							///STRACE("[%d] RDF=%f\n", _sprttype, rateDeltaFrame);
							_astat->advanceFrame(ec->getDeltaFrame() * rateDeltaFrame);
						}
					}
				}

				if(_mode->isActing())		// アクション中
				{
					if(!_astat->isStopFrame())	// 停止中ではない場合、つまり ActStat によるものではない行動（ソウマなど）を実行の場合
					{
						_astat->stopFrame();
					}

					if(_cstatRef->getStat() == CharStat::STAT_DOWN)	// 行動不能中
					{
						// 行動不能による行動中断
						breakBehavior(Spirit::BEHV_NULL, false, false);	// 行動中断（BEHV_NULL は全てを対象とした汎用中断）
					}
					else if(_cstatRef->isPeriodicFrame())			// フレーム間引き処理実行タイミング
					{
						// 錯乱による行動中断
						if(TFW_IS_FLAG(_cstatRef->getSpStatFlags(), CharStat::SPSTAT_CONFU))			// 錯乱中
						{
							s32 exct = _cstatRef->getExcitednessRate() - (_cstatRef->getCharParam()->getUnity() / 128) - 100;	// 集中度が高いと抑えられる
							if((::rand() & 0x3FF) <= exct)	// 0 ～ 2047 の乱数が興奮度（1 ～）以下の場合
							{
								breakBehavior(Spirit::BEHV_NULL, false, true);	// 行動中断（BEHV_NULL は全てを対象とした汎用中断）
							}
						}
					}
				}
				else						// アクション中ではないとき
				{
					if(_astat->isActionFrame())		// アクションフレーム
					{
						if(_mode->action())
						{
							_astat->stopFrame();
						}
					}
					else if(_astat->isStopFrame())	// 停止中
					{
						_mode->resetAction();
						_astat->restartFrame();	// 次のアクション待ちへ
					}
				}
			}
		}
	}
}

/*---------------------------------------------------------------------*//**
	対決時の自動策敵フレーム処理
**//*---------------------------------------------------------------------*/
void Spirit::execSearchConfrontTarget(const ExecCtx* ec)
{
	if(!_unitSelfRef->isEnable())				{	return;	}
	GameExecCtx* gec = (GameExecCtx*)ec;
	if(gec->isLockOperating())					{	return;	}

	if(_unitCfrntRef != 0L)	// 対決中の場合
	{
		Confront* confront = Game::getGame()->getConfront();
		const UnitArray* uarrTrgs = confront->getTargets();

		// 現在フォーカスしている相手が対決相手の場合は、対決対象もそちらに変更する
		if((_unitFocusRef != 0L) && _unitFocusRef->isEnable() && (_unitFocusRef != _unitCfrntRef))
		{
			for(int i = 0; i < uarrTrgs->getCount(); i++)
			{
				Unit* unit = uarrTrgs->unit(i);
				if(_unitFocusRef == unit)
				{
					if(this->getType() == SPRTTYPE_ENEMY)
					{
						if(TFW_IS_FLAG(unit->getUnitFlags(), Unit::UNITFLAG_PC))
						{
							setConfrontUnit(unit);
							return;
						}
					}
					else
					{
						if(unit->getUnitType() == UNITTYPE_ENEMY_CHAR)
						{
							setConfrontUnit(unit);
							return;
						}
					}
				}
			}
		}

		// 対象が倒された場合は違う相手を探す
		if(!_unitCfrntRef->isEnable())
		{
			for(int i = 0; i < uarrTrgs->getCount(); i++)
			{
				Unit* unit = uarrTrgs->unit(i);
				if(unit->isEnable())
				{
					if(this->getType() == SPRTTYPE_ENEMY)
					{
						if(TFW_IS_FLAG(unit->getUnitFlags(), Unit::UNITFLAG_PC))
						{
							setConfrontUnit(unit);
							return;
						}
					}
					else
					{
						if(unit->getUnitType() == UNITTYPE_ENEMY_CHAR)
						{
							setConfrontUnit(unit);
							return;
						}
					}
				}
			}

			setConfrontUnit(0L);	// 対戦相手がいない
		}
	}
}

/*---------------------------------------------------------------------*//**
	マップ変更前通知
**//*---------------------------------------------------------------------*/
void Spirit::notifyChangeMapBefore()
{
	setConfrontUnit(0L);
	setFocusUnit(0L, false);
	setTalkUnit(0L);
}

/*---------------------------------------------------------------------*//**
	マップ変更通知
**//*---------------------------------------------------------------------*/
void Spirit::notifyChangedMap()
{
}

/*---------------------------------------------------------------------*//**
	対決ゲームモード変更通知
**//*---------------------------------------------------------------------*/
void Spirit::notifyChangedConfront(bool isConfront, Confront* confront, Unit* unitTrg, Unit* unitEntrant, bool isAddition)
{
	bool isCofrontUntil = this->isConfront();
	setConfrontUnit(isConfront ? unitTrg : 0L);

#if 1
	if((_unitFocusRef == 0L) || !TFW_IS_FLAG(_unitFocusRef->getUnitFlags(), Unit::UNITFLAG_PC))	// 既存のフォーカスが無いか味方以外の時のみ
	{
		setFocusUnit(unitTrg, false);
	}
#elif 0
	setFocusUnit(isConfront ? unitTrg : 0L, false);
#else
	// フォーカスの設定
	if(isConfront && (unitTrg != 0L))	// 新たに対決対象へのフォーカス設定
	{
		setFocusUnit(unitTrg, false);
	}
	else	// フォーカス外し
	{
		if((_unitFocusRef != 0L) && !TFW_IS_FLAG(_unitFocusRef->getUnitFlags(), Unit::UNITFLAG_PC))	// 既存のフォーカスが味方以外の時のみ
		{
			setFocusUnit(0L, false);
		}
	}
#endif

	// 対決開始時のアクションフレーム数を設定する
	if(isConfront && !isCofrontUntil && (_astat != 0L) && (_cstatRef != 0L))
	{
		f32 frameAct = _cstatRef->getActFrame();
		f32 frameFirstAdv = frameAct * 0.5f;
		f32 frameRandDifMax = (1.0f - (_cstatRef->getCharParam()->getStability() / (u16)CharParam::VALUE_MAX)) * frameFirstAdv;

		// 先制行動
		const Unit* unitProm = confront->getPromoterUnit();	// 主催者を得る
		if(unitProm != 0L)
		{
			switch(_sprttype)
			{
			case SPRTTYPE_MY:	case SPRTTYPE_PARTNER:	case SPRTTYPE_HELPER:	// 主催者が PC
				if(TFW_IS_FLAG(unitProm->getUnitFlags(), Unit::UNITFLAG_PC))
				{
					frameFirstAdv = frameAct;
				}
				break;
			case SPRTTYPE_ENEMY:	// 主催者が敵
				if(unitProm->getUnitType() == UNITTYPE_ENEMY_CHAR)
				{
					frameFirstAdv = frameAct;
				}
				break;
			}
		}

		_astat->setup(frameAct, frameRandDifMax, frameFirstAdv);
	}
}

/*---------------------------------------------------------------------*//**
	イベントゲームモード変更通知
**//*---------------------------------------------------------------------*/
void Spirit::notifyChangedEventMode(bool isEventMode)
{
}

/*---------------------------------------------------------------------*//**
	ステータス影響発生通知
**//*---------------------------------------------------------------------*/
void Spirit::notifyInfluenceStat(const StatusChangeResult* scres, const TransStat* tstat)
{
}

/*---------------------------------------------------------------------*//**
	ソウマ アクション通知
**//*---------------------------------------------------------------------*/
void Spirit::notifySoumaAction(s32 snotifid, const StatusChangeResult* scres, const VcString* msg)
{
	Conductor* conductor = Game::getGame()->getConductor();

	switch(snotifid)
	{
	case Souma::SNOTIFID_CHANTING_START:
		{
			#if !ENABLE_POPNUM
				// “詠唱を開始した”
				VcString msg(VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_CHANTING_START)->getRaw(),
						(_unitSelfRef->getMainName() != 0L) ? _unitSelfRef->getMainName()->getRaw() : "???", "\n"));
				conductor->on(&msg, _unitSelfRef, 0);
			#endif
		}
		break;
	case Souma::SNOTIFID_CHANTING_BREAK:
		{
			// “詠唱は中断された”
			VcString msg(VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_CHANTING_BREAK)->getRaw(),
					(_unitSelfRef->getMainName() != 0L) ? _unitSelfRef->getMainName()->getRaw() : "???", "\n"));
			conductor->on(&msg, _unitSelfRef, 0);
		}
		break;
	case Souma::SNOTIFID_SERVE:
		{
			#if !ENABLE_POPNUM
				// “ソウマを放った”
				VcString msg(VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_SERVE_SOUMA)->getRaw(),
					(_unitSelfRef->getMainName() != 0L) ? _unitSelfRef->getMainName()->getRaw() : "???", "\n"));
				conductor->on(&msg, _unitSelfRef, 0);
			#endif
		}
		break;
	case Souma::SNOTIFID_NOT_HAPPENED:
		{
			#if !ENABLE_POPNUM
				// “何も起きなかった”
				VcString msg(VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_NOT_HAPPENED)->getRaw(), "\n"));
				conductor->on(&msg, _unitSelfRef, 30);
			#endif
		}
		break;
	case Souma::SNOTIFID_ERROR_SOURCE:
		{
			// “ソースエラー”
			VcString msg(VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_SOUMA_ESCR_SOURCE_ERROR_MSG)->getRaw(),	"\n"));
			conductor->on(&msg, _unitSelfRef, 0);
		}
		break;
	case Souma::SNOTIFID_FORCE_WASTE_ENE:
		if(scres->getEneActualDamage() > 0)
		{
			conductor->on(scres, 0);
		}
		break;
	case Souma::SNOTIFID_ERROR_MSG:
		conductor->on(msg, _unitSelfRef, 0);
		break;
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
