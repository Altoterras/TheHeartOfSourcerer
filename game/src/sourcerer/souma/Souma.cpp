/***********************************************************************//**
 *	Souma.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/04/07.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "Souma.h"

// Friends
#include "script/SoumaScript.h"
#include "script/SoumaScriptContext.h"
#include "../common/CalcUtils.h"
#include "../effect/EleneEffect.h"
#include "../effect/StatusChangeEffect.h"
#include "../es/Es.h"
#include "../es/EsAtom.h"
#include "../es/EsContext.h"
#include "../es/EsError.h"
#include "../es/EsExternalExtension.h"
#include "../es/EsRunStat.h"
#include "../es/EsValue.h"
#include "../body/Conductor.h"
#include "../body/Game.h"
#include "../body/GameFixedStrTbl.h"
#include "../body/GameNotifier.h"
#include "../gui/FocusCursor.h"
#include "../gui/GameGui.h"
#include "../gui/window/implement/GameActMsgWindow.h"
#include "../item/ItemEquip.h"
#include "../magic/MagicCluster.h"
#include "../magic/MagicClusterCreateParam.h"
#include "../magic/MagicSys.h"
#include "../unit/Unit.h"
#include "../unit/UnitManager.h"
#include "../unit/char/CharUnit.h"
#include "../unit/char/pc/PcUnit.h"
#include "../sound/GameSoundDef.h"
#include "../source/Source.h"
#include "../source/SourceManager.h"
#include "../spirit/Spirit.h"
#include "../status/ActStat.h"
#include "../status/CharStat.h"
#include "../status/Elene.h"
#include "../status/EquipStat.h"
#include "../status/StatusIfEx.h"
#include "../status/TransStat.h"
#include "../episode/party/MyParty.h"

// External
#include "../../etk/particle/Particle.h"
#include "../../etk/particle/ParticleSys.h"
#include "../../etk/sound/SoundMngr.h"
#include "../../tfw/collection/List.h"
#include "../../tfw/lib/Vector.h"
#include "../../tfw/lib/Matrix.h"
#include "../../tfw/string/StringUtils.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// マクロ

#define ESMODE_SS_DEFAULT	(1)

////////////////////////////////////////////////////////////////////////////
// 定数

#if 0
// 意識集中度
static const s32	CONCD_COMM_ATMAN		= CharStat::CONCD_EL_MAX * Elene::NUM_EL;	// アートマンと通信
static const s32	CONCD_COMM_GODS			= CharStat::CONCD_EL_MAX * Elene::EL_AE;	// ほとんど全ての神々と通信
static const s32	CONCD_COMM_AE			= CharStat::CONCD_EL_MAX * Elene::EL_AE;	// 空元素への操作
static const s32	CONCD_COMM_WI			= CharStat::CONCD_EL_MAX * Elene::EL_WI;	// 風元素への操作
static const s32	CONCD_COMM_FI			= CharStat::CONCD_EL_MAX * Elene::EL_FI;	// 火元素への操作
static const s32	CONCD_COMM_WA			= CharStat::CONCD_EL_MAX * Elene::EL_WA;	// 水元素への操作
static const s32	CONCD_COMM_SO			= CharStat::CONCD_EL_MAX * Elene::EL_SO;	// 地元素への操作
#endif

// ソウマ詠唱速度（ディレイ）
///#define DELAY_CHANT		8.8f	// 【Ver1.3.1】
///#define DELAY_CHANT		6.4f	// 【2012/03/26】
///#define DELAY_CHANT		5.5f	// 【Ver2.0.1】
#define DELAY_CHANT			5.0f	// current version

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// Souma メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	実行中かどうか得る
**//*---------------------------------------------------------------------*/
bool Souma::isRunning() const
{
	if(_ssctx == 0L)	{	return false;	}
	return _ssctx->getEsRunStat()->isRunning();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	状態を得る
**//*---------------------------------------------------------------------*/
void Souma::getState(const VcString** code, bool* isRun, s32* idxBegin, s32* idxEnd) const
{
	*code = _ssctx->getEsContext()->getScriptCode();
	*isRun = _ssctx->getEsRunStat()->isRunning();
	*idxBegin = _ssctx->getEsRunStat()->getBeginCodeIndex();
	*idxEnd = _ssctx->getEsRunStat()->getEndCodeIndex();
}

/*---------------------------------------------------------------------*//**
	コード文字列を得る
**//*---------------------------------------------------------------------*/
const VcString* Souma::getCode() const
{
	if(_ssctx == 0L)					{	return 0L;	}
	if(_ssctx->getEsContext() == 0L)	{	return 0L;	}
	return _ssctx->getEsContext()->getScriptCode();
}

/*---------------------------------------------------------------------*//**
	ソース実行
**//*---------------------------------------------------------------------*/
bool Souma::start(const VcString* srccode, SourceDef::SourceKind srckind, u16 flags)
{
	if(_ssctx == 0L)	{	return false;	}
	ASSERT(!_ssctx->getEsRunStat()->isRunning());
	ASSERT(_sprtRef->getCharStat()->getStat() == CharStat::STAT_ALIVE);

	// スクリプトシステムリセット
	_ssctx->getEsContext()->reset();
	_ssctx->getEsRunStat()->clear();
	_frmcntStep = 0.0f;		// 開始
	_frmdcntWait = 0.0f;
	_isMistake = false;
	_flags = flags;

	// 詠唱遅延値を算出
	f32 delay = DELAY_CHANT;
	if(_sprtRef->getCharStat() != 0)
	{
		f32 r = _sprtRef->getCharStat()->getCharParam()->getUnity() * 0.001f;
		if(r > 1.0f)		{	r = 1.0f;	}
		r = ::sinf(r);
		delay -= 4.0f * r;
		if(delay < 1.0f)	{	delay = 1.0f;	}
	}
	// 半覚醒時は最速に
	if((_unitRef != 0L) && _unitRef->isCategory(Unit::UNITCAT_CHAR) && TFW_IS_FLAG(((CharUnit*)_unitRef)->getCharStat()->getSpStatFlags(), CharStat::SPSTAT_TRANS))
	{
		delay = 0.5f;
	}
	_frmDelay = delay;

	// 開始する
	ASSERT((_scriptRef != 0L) && (_ssctx->getEsContext() != 0L) && (_ssctx->getEsRunStat() != 0L));
	if(_scriptRef->parse(_ssctx, srccode, srckind))
	{
		_ssctx->getEsRunStat()->setRunning(true, false);

		// スピリットへ通知
		_sprtRef->notifySoumaAction(SNOTIFID_CHANTING_START, 0L, 0L);

		return true;
	}
	else
	{
		// スピリットへ通知
		_sprtRef->notifySoumaAction(SNOTIFID_ERROR_SOURCE, 0L, 0L);

		// エラーメッセージ表示
		Conductor* conductorRef = Game::getGame()->getConductor(); ASSERT(conductorRef != 0L);
		if(conductorRef != 0L)
		{
			ColorU8 col;
			Conductor::getUserSsDebugColor(&col);
			for(ListIterator<EsError::Report*> it = _ssctx->getEsContext()->getError()->getCompileErrorList()->iterator(); it.has(); it.next())
			{
				const EsError::Report* r = it.object();
				VcString msg(VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_SOUMA_SCRIPT_ERROR_MSG)->getRaw(), "Compile Error ", r->_pos.getBegin()->getLineNo() + 1, "\n"));
				conductorRef->on(&msg, &col, 0);
			}
		}

		return false;
	}
}

/*---------------------------------------------------------------------*//**
	終了
**//*---------------------------------------------------------------------*/
bool Souma::end(bool isNoEffect)
{
	// 強制終了
	_ssctx->getEsRunStat()->setRunning(false, false);

	// SE 停止
	if(_seidMc != -1)
	{
		Game::getGame()->getSoundManager()->stopSe(_seidMc);
		_seidMc = -1;
	}

	// まだクラスタを保持しているようならば消滅させる
	MagicSys* mgcsys = Game::getGame()->getMagicSys();	ASSERT(mgcsys != 0L);
	for(int i = 0; i < NUM_CLUSTER_MAX; i++)
	{
		if(_mcidtbl[i] != 0)
		{
			MagicCluster* mc = mgcsys->getCluster(_mcidtbl[i]);
			if(mc != 0L)
			{
				// 消滅エフェクトを出す
				if(!isNoEffect)
				{
					appearParticle(GameParticleDef::PTCLK_SHOCK_WAVE, mc->getPosition(), 0);
				}
				// クラスタを消滅
				mgcsys->disappearCluster(_mcidtbl[i], MagicCluster::DCF_IMMATURE_SOUMA);
			}
			// クラスタの保持を手放す
			releaseCluster(_mcidtbl[i]);
		}
	}

	// その他値の初期化
	_frmcntStep = -1.0f;	// 無効化
#if 0
	_handleBase = 0;
#endif

	return true;
}

/*---------------------------------------------------------------------*//**
	中断
**//*---------------------------------------------------------------------*/
bool Souma::terminate()
{
	// 強制終了
	_ssctx->getEsRunStat()->setRunning(false, false);

	// スピリットへ通知
	_sprtRef->notifySoumaAction(SNOTIFID_CHANTING_BREAK, 0L, 0L);

	// 現時点のものが、強制的に自分自身にステータス影響
	if(_unitRef->isCategory(Unit::UNITCAT_CHAR))
	{
		MagicSys* mgcsys = Game::getGame()->getMagicSys();	ASSERT(mgcsys != 0L);
		for(int i = 0; i < NUM_CLUSTER_MAX; i++)
		{
			MagicCluster* mc = (_mcidtbl[i] != 0) ? mgcsys->getCluster(_mcidtbl[i]) : 0L;
			if(mc != 0L)
			{
				TransStat* tstatMc = mc->transStat();
				EleneStat* eestatMc = tstatMc->eleneStat();

				// 強制エネルギー消費
				{
					CharStat* cstatSelf = _sprtRef->charStat(); ASSERT(cstatSelf != 0L);
#if 1
					StatusIfEx::wasteSoumaMaterializeEnergy(cstatSelf, eestatMc->getCql(), true);
#else
					StatusChangeResult scres(_unitRef, mc);
					StatusInfluencer::wasteSoumaMaterializeEnergy(&scres, cstatSelf, eestatMc->getCql(), true);
					_sprtRef->notifySoumaAction(SNOTIFID_FORCE_WASTE_ENE, &scres, 0L);	// スピリットへ通知
#endif
				}

				// ステータス影響 ※現時点では必ず悪影響とする．従って、純性のものは影響を受けないこととする（激性化するなども候補だが辛すぎる気もする）
				if(!TFW_IS_FLAG(tstatMc->getQualityFlags(), EleneStat::QF_PURE_1))
				{
#if 1
					StatusIfEx::influence(((CharUnit*)_unitRef)->charStat(), tstatMc, true, false);
#else
					StatusChangeResult scres(_unitRef, mc);
					StatusInfluencer::influence(&scres, ((CharUnit*)_unitRef)->charStat(), tstatMc);
					StatusChangeEffect::on(&scres);									// エフェクト表示
#endif
					_sprtRef->notifySoumaAction(SNOTIFID_FORCE_FEEDBACK, 0L, 0L);	// スピリットへ通知
				}
			}
		}
	}

	return end(false);
}

/*---------------------------------------------------------------------*//**
	待機
**//*---------------------------------------------------------------------*/
void Souma::wait(f32 frame)
{
	_frmdcntWait = frame;
}

/*---------------------------------------------------------------------*//**
	詠唱を遅くする
**//*---------------------------------------------------------------------*/
void Souma::slowChant()
{
	_frmDelay *= 2.0f;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
Souma::Souma()
	: _ssctx(0L)
	, _scriptRef(0L)
	, _sprtRef(0L)
	, _unitRef(0L)
	, _flags(0)
	, _frmcntStep(0.0f)
	, _frmcntChargePtcl(0.0f)
	, _frmdcntWait(0.0f)
	, _frmDelay(0.0f)
	, _indexCurMc(0)
	, _seidMc(-1)
	, _eleneEvokable(0L)
	, _isMistake(false)
{
	for(int i = 0; i < NUM_CLUSTER_MAX; i++)	{	_mcidtbl[i] = 0;	}
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
Souma::~Souma()
{
	ASSERT(_ssctx == 0L);
	#if defined(_DEBUG)
		for(int i = 0; i < NUM_CLUSTER_MAX; i++)	{	ASSERT(_mcidtbl[i] == 0);	}
	#endif
	ASSERT(_eleneEvokable == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool Souma::create(Spirit* sprtRef, Unit* unitRef)
{
	_sprtRef = sprtRef;
	_unitRef = unitRef;
	_frmcntStep = -1.0f;
	_frmcntChargePtcl = 0.0f;
	_scriptRef = Game::getGame()->getSoumaScript(); ASSERT(_scriptRef != 0L);

	// コンテキスト作成
	_ssctx = new SoumaScriptContext();
	if(!_ssctx->create(this, _scriptRef))
	{
		delete _ssctx;
		_ssctx = 0L;
		ASSERT(false);
		return false;
	}

	// 呼び出し可能元素値の作成
	_eleneEvokable = new Elene();

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void Souma::destroy()
{
	// 呼び出し可能元素値の削除
	delete _eleneEvokable;
	_eleneEvokable = 0L;

	// コンテキストの削除
	if(_ssctx != 0L)
	{
		_ssctx->destroy();
		delete _ssctx;
		_ssctx = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void Souma::exec(bool* isStep, const ExecCtx* ec)
{
	if(_frmcntStep >= 0.0f)
	{
		// 待機処理
		if(_frmdcntWait > 0.0f)
		{
			_frmdcntWait -= ec->getDeltaFrame();
			if(_frmdcntWait < 0.0f) { _frmdcntWait = 0.0f; }
		}
		else
		{
			// ステップを進める
			_frmcntStep += ec->getDeltaFrame();
			if(_frmcntStep >= _frmDelay)
			{
				if(_ssctx->getEsRunStat()->isRunning())
				{
					// スクリプト処理を進める
					_scriptRef->run(_ssctx);

					// エラーが発生した
					if(!_ssctx->getEsRunStat()->isRunning() && _ssctx->getEsRunStat()->isErrorOccur() && !_isMistake)
					{
						// スピリットに通知
						_sprtRef->notifySoumaAction(SNOTIFID_ERROR_SOURCE, 0L, 0L);

						// エラーメッセージ表示
						Conductor* conductorRef = Game::getGame()->getConductor(); ASSERT(conductorRef != 0L);
						if(conductorRef != 0L)
						{
							ColorU8 col;
							Conductor::getUserSsDebugColor(&col);
							for(ListIterator<EsError::Report*> it = _ssctx->getEsContext()->getError()->getRunErrorList()->iterator(); it.has(); it.next())
							{
								const EsError::Report* r = it.object();
								VcString msg(VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_SOUMA_SCRIPT_ERROR_MSG)->getRaw(), "Runtime Error ", r->_pos.getBegin()->getLineNo() + 1, "\n"));
								conductorRef->on(&msg, &col, 0);
							}
						}
					}
				}

				*isStep = true;
				_frmcntStep = 0.0f;
			}
			else
			{
				if(_ssctx->getEsRunStat()->isRunning())
				{
					// 詠唱中
					if(_sprtRef->charStat() != 0L)
					{
						_sprtRef->charStat()->calmingFrame();
					}
				}

				*isStep = false;
			}
		}

		// クラスタの位置調整
		MagicSys* mgcsys = Game::getGame()->getMagicSys();	ASSERT(mgcsys != 0L);
		s32 numActiveMc = 0;
		for(int i = 0; i < NUM_CLUSTER_MAX; i++)
		{
			if(_mcidtbl[i] != 0)
			{
				// クラスタ数カウント
				numActiveMc++;
			}
		}
		switch(numActiveMc)
		{
		case 1:
			{
				MagicCluster* mc = mgcsys->getCluster((_mcidtbl[0] != 0) ? _mcidtbl[0] : _mcidtbl[1]);
				if(mc != 0L)
				{
					Vector3F ccofsTrg;
					calcMagicClusterOffset(&ccofsTrg, 0.0f, mc->getLookRadius());
					Vector3F ccofsCur(*mc->getPosition() - *_unitRef->getCenterPos()); 
					if(ccofsCur != ccofsTrg)
					{
						ccofsCur.x() = (ccofsCur.x() * 0.8f) + (ccofsTrg.x() * 0.2f);
						ccofsCur.y() = (ccofsCur.y() * 0.8f) + (ccofsTrg.y() * 0.2f);
						ccofsCur.z() = (ccofsCur.z() * 0.8f) + (ccofsTrg.z() * 0.2f);
						ccofsCur += *_unitRef->getCenterPos();
						mc->setPosition(&ccofsCur);
					}
				}
			}
			break;
		case 2:
			{
				MagicCluster* mc[2] = { mgcsys->getCluster(_mcidtbl[0]), mgcsys->getCluster(_mcidtbl[1]) };
				if((mc[0] != 0L) && (mc[1] != 0L))
				{
					f32 r = (mc[0]->getLookRadius() + mc[1]->getLookRadius()) * 0.33f;
					Vector3F ccofsTrg[2];
					calcMagicClusterOffset(&ccofsTrg[0], - r, mc[0]->getLookRadius());
					calcMagicClusterOffset(&ccofsTrg[1], r, mc[1]->getLookRadius());
					for(int i = 0; i < 2; i++)
					{
						Vector3F ccofsCur(*mc[i]->getPosition() - *_unitRef->getCenterPos()); 
						if(ccofsCur != ccofsTrg[i])
						{
							ccofsCur.x() = (ccofsCur.x() * 0.8f) + (ccofsTrg[i].x() * 0.2f);
							ccofsCur.y() = (ccofsCur.y() * 0.8f) + (ccofsTrg[i].y() * 0.2f);
							ccofsCur.z() = (ccofsCur.z() * 0.8f) + (ccofsTrg[i].z() * 0.2f);
							ccofsCur += *_unitRef->getCenterPos();
							mc[i]->setPosition(&ccofsCur);
						}
					}
				}
			}
			break;
		}
	}

	// チャージパーティクルのフレームカウンタを減算
	if(_frmcntChargePtcl > 0.0f)
	{
		_frmcntChargePtcl -= ec->getDeltaFrame();
	}
}

/*---------------------------------------------------------------------*//**
	エラーメッセージを表示する
**//*---------------------------------------------------------------------*/
s32 Souma::doError(s32 retcode)
{
	u16 strid = 0;
	if((SoumaScript::RET_GG__END_ <= retcode) && (retcode <= SoumaScript::RET_GG__START_))
	{
		strid = GameFixedStrTbl::SID_SOUMA_ERROR_GAME + SoumaScript::RET_GG__START_ - retcode;
	}
	else if((SoumaScript::RET_GL__END_ <= retcode) && (retcode <= SoumaScript::RET_GL__START_))
	{
		strid = GameFixedStrTbl::SID_SOUMA_ERROR_LOCAL + SoumaScript::RET_GL__START_ - retcode;
	}
	else if((SoumaScript::RET_SC__END_ <= retcode) && (retcode <= SoumaScript::RET_SC__START_))
	{
		strid = GameFixedStrTbl::SID_SOUMA_ERROR_SCRIPT + SoumaScript::RET_SC__START_ - retcode;
	}

	if(strid == 0)				{	return retcode;	}
	const VcString* msg = GameFixedStrTbl::getString(strid);
	if(msg == 0L)				{	return retcode;	}
	if(msg->getLength() <= 0)	{	return retcode;	}

	// スピリットへ通知
	VcString str;
	_sprtRef->notifySoumaAction(SNOTIFID_ERROR_MSG, 0L, VcString::format(&str, GameFixedStrTbl::getString(strid)->getRaw(), "\n"));

	return retcode;
}

/*---------------------------------------------------------------------*//**
	致命的エラーによる強制終了
**//*---------------------------------------------------------------------*/
s32 Souma::doFatalError(s32 retcode)
{
	// エラーメッセージ表示
	doError(retcode);

	// 中断処理へ
	return terminate();
}

/*---------------------------------------------------------------------*//**
	実行ミスによる強制終了
**//*---------------------------------------------------------------------*/
s32 Souma::doMistake(s32 retcode)
{
	_isMistake = true;

	// エラーメッセージ表示
	doError(retcode);

	// 中断処理へ
	return terminate();
}

/*---------------------------------------------------------------------*//**
	ソウマ環境準備
**//*---------------------------------------------------------------------*/
bool Souma::prepare()
{
	if(_unitRef == 0L)		{	return false;	}
	if(_sprtRef == 0L)		{	return false;	}

#if 0
	// ハンドルベース値の初期化
	_handleBase = 101 + (::rand() & 0xfff);	// 101 ～ 4196
#endif

	return true;
}

/*---------------------------------------------------------------------*//**
	ソウマ環境準備確認
**//*---------------------------------------------------------------------*/
bool Souma::isPrepared()
{
	if(_unitRef == 0L)		{	return false;	}
	if(_sprtRef == 0L)		{	return false;	}
#if 0
	return _handleBase != 0;
#endif
	return true;
}

/*---------------------------------------------------------------------*//**
	瞑想する
**//*---------------------------------------------------------------------*/
bool Souma::meditate()
{
	CharStat* cstat = _sprtRef->charStat(); ASSERT(cstat != 0L);

	// 活動状態確認
	if(	(cstat->getStat() != CharStat::STAT_ALIVE)
		|| TFW_IS_FLAG(cstat->getSpStatFlags(), CharStat::SPSTAT_SLEEP)	)
	{
		return false;
	}

	// 瞑想
	cstat->dianaFrame();

	// エフェクト表示
	if((_unitRef != 0L) && (_frmcntChargePtcl <= 0.0f))
	{
		ParticleSys* ptclsys = Game::getGame()->getParticleSys();	ASSERT(ptclsys != 0L);
		ptclsys->appearParticle(GameParticleDef::PTCLK_CHARGE, _unitRef->getCenterPos(), 0);
		_frmcntChargePtcl = GameParticleDef::FRAME_PTCLK_CHARGE;	// チャージパーティクルのフレームカウンタの設定
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	クラスタ作成
**//*---------------------------------------------------------------------*/
s32 Souma::makeCluster(u16* mcid, const Elene* eleneMakeOql, const Elene* elenePreset, u32 funcflagsPreset)
{
	if(!isPrepared())		{	return SoumaScript::RET_NO_PREPARED;	}
	
	// 呼び出し可能元素量の更新
	if(!_unitRef->getEvokableElene(_eleneEvokable, Unit::EVECAT_TOTAL))
	{
		return SoumaScript::RET_OTHER_PROBLEM;
	}

	// 空きクラスタースロットを探す
	s32 indexMc = -1;
	for(int i = 0; i < NUM_CLUSTER_MAX; i++)
	{
		if(_mcidtbl[i] == 0)
		{
			indexMc = i;
			break;
		}
	}
	if(indexMc == -1)	// 空きがなかった
	{
		return SoumaScript::RET_NO_CLUSTER_SLOT;
	}

	// キャラクタに対してのクラスター出現位置
	Vector3F pos;
	calcMagicClusterOffset(&pos, 0.0f, 0.0f);
	pos += *_unitRef->getCenterPos();

	// クラスター作成
	MagicSys* mgcsys = Game::getGame()->getMagicSys();	ASSERT(mgcsys != 0L);
	MagicClusterCreateParam cmmprm(MagicClusterType::CREATION, GameParticleDef::PTCLK_MAGIC_MAKING_1);
	cmmprm._unitOwnrRef = _unitRef;
	cmmprm._unitMainTrgRef = _sprtRef->focusUnit();
	cmmprm._pos = &pos;
	cmmprm._tstat.setupForMagicClusterMaking(_unitRef, eleneMakeOql, TFW_IS_FLAG(_flags, Souma::F_USER_CUSTOMIZED_SOURCE) ? TransStat::SF_USER_CUSTOMIZED_SOUMA : 0);
	cmmprm._elenePreset = elenePreset;
	cmmprm._funcflagsPreset = funcflagsPreset;
	cmmprm._lifeframe = MagicCluster::LIFEFRAME_INFINITY;	// 最初は無限生存として作成（放射時に有限化）
	cmmprm._isShowFocusCursor = true;
	u16 mcidNew = mgcsys->appearCluster(&cmmprm);
	if(mcidNew == 0)		{	return SoumaScript::RET_OTHER_PROBLEM;	}
	MagicCluster* mcNew = mgcsys->getCluster(mcidNew);
	if(mcNew == 0L)			{	return SoumaScript::RET_OTHER_PROBLEM;	}
	_mcidtbl[indexMc] = mcidNew;
	_indexCurMc = indexMc;		// 現在のクラスターに設定する

	// SE を鳴らす
	if(_seidMc == -1)	// まだ鳴っていない場合
	{
		Game::getGame()->getSoundManager()->playSe(GameSoundDef::SE_MAKE_MCLUSTER, false, 1.0f, _unitRef->getCenterPos(), 0L);
		_seidMc = Game::getGame()->getSoundManager()->playSe(GameSoundDef::SE_STREAM_NOISE, true, 0.7f, _unitRef->getCenterPos(), 0L);
	}

	// クラスタを解放するまでは当たりをとらない
	TFW_SET_FLAG(*mcNew->ctrlFlags(), MagicCluster::CTRLF_NO_COLL, true);

	*mcid = mcidNew;
	return SoumaScript::RET_OK;
}

/*---------------------------------------------------------------------*//**
	クラスタ作成が行われているかどうか
**//*---------------------------------------------------------------------*/
bool Souma::checkCluster(u16 mcid, bool setCurrent)
{
	if(!isPrepared())	{	return false;	}

	// クラスタの存在チェック
	bool isExist = false;
	for(int i = 0; i < NUM_CLUSTER_MAX; i++)
	{
		if(_mcidtbl[i] == mcid)
		{
			isExist = true;
			// カレントのクラスタに設定する
			if(setCurrent)
			{
				_indexCurMc = i;
			}
			break;
		}
	}
	if(!isExist)		{	return false;	}

	// クラスタを取得しなおす（消滅していることがあるので）
	MagicSys* mgcsys = Game::getGame()->getMagicSys();	ASSERT(mgcsys != 0L);
	MagicCluster* mc = mgcsys->getCluster(mcid);
	if(mc == 0L)		{	return false;	}

	// 健全性チェック
	mc->checkHealth();

	return true;
}

/*---------------------------------------------------------------------*//**
	クラスタの保持を手放す
**//*---------------------------------------------------------------------*/
void Souma::releaseCluster(u16 mcid)
{
	// テーブルから取り除く
	bool isHasCluster = false;
	for(int i = 0; i < NUM_CLUSTER_MAX; i++)
	{
		if(_mcidtbl[i] == mcid)
		{
			_mcidtbl[i] = 0;
		}
		else if(_mcidtbl[i] != 0)
		{
			isHasCluster = true;
		}
	}

	// SE 停止
	if(!isHasCluster)
	{
		if(_seidMc != -1)
		{
			Game::getGame()->getSoundManager()->stopSe(_seidMc);
			_seidMc = -1;
		}
	}
}

#if 0
/*---------------------------------------------------------------------*//**
	ハンドル文字列をユニット ID に変換する
**//*---------------------------------------------------------------------*/
s32 Souma::convHandleStrToUnitId(u16* unitid, Unit** unit, const VcString* str)
{
	if(str->equals("consot ha foc"))	// 無意識フォーカス
	{
		GameNotifier* gnotif = Game::getGame()->getGameNotifier(); ASSERT(gnotif != 0L);
		Unit* unitWk = 0L;
		if(!gnotif->notifyFindUnconsciousFocus(&unitWk))
		{
			// 見つからなかった
			CharStat* cstatSelf = _sprtRef->charStat(); ASSERT(cstatSelf != 0L);
			unitWk = cstatSelf->getOwnerUnit();	// 自分自身
		}

		if(unitWk != 0L)
		{
			if(unitid != 0L)	{	*unitid = unitWk->getId();	}
			if(unit != 0L)		{	*unit = unitWk;				}
			return (s32)SoumaScript::RET_OK;
		}
		else
		{
			return SoumaScript::RET_NO_TARGET;	// <ERROR> 対象が見つからなかった
		}
	}
	
	// 指定名を得る
	VcStringArray saDot = StringUtils::split(str, " ha ");	// "AA の BB" における『の』（英語では of、JavaSciript では . 、ソウマ語では ha）で分割
	VcString nameGlob;
	VcString nameProp;
	if(saDot.getCount() == 1)
	{
		nameGlob.set(str);
	}
	else if(saDot.getCount() == 2)
	{
		nameGlob.set(saDot.getString(0));
		nameProp.set(saDot.getString(1));
	}
	else
	{
		return SoumaScript::RET_INVALID_HANDLE_STRING;	// <ERROR> ハンドル指定文字列のミス
	}
	StringUtils::trim(&nameGlob);
	StringUtils::trim(&nameProp);

	// グローバルのユニットを得る
	u16 unitidGlob = 0;
	Unit* unitGlob = 0L;
	// アサ
	if(nameGlob.equals("asa") || nameGlob.equals("Asa"))
	{
		unitGlob = Game::getGame()->getMyParty()->pcUnit(MyParty::PC_MY);
	}
	// コルポ
	else if(nameGlob.equals("corpo") || nameGlob.equals("Corpo"))
	{
		unitGlob = Game::getGame()->getMyParty()->pcUnit(MyParty::PC_PTN);
	}
	// コツメ
	else if(nameGlob.equals("kotsume") || nameGlob.equals("Kotsume"))
	{
		unitGlob = Game::getGame()->getMyParty()->pcUnit(MyParty::PC_HLP);
	}
	// 見つからなかった
	else
	{
		return SoumaScript::RET_NO_TARGET;	// <ERROR> 対象が見つからなかった
	}
	unitidGlob = unitGlob->getId();

	// ひとまずグローバルによる取得値を設定する
	if(unitid != 0L)	{	*unitid = unitidGlob;	}
	if(unit != 0L)		{	*unit = unitGlob;		}

	// プロパティのハンドルを得る
	if(nameProp.getLength() > 0)
	{
		u16 unitidProp = 0;
		Unit* unitProp = 0L;
		s32 res = (s32)SoumaScript::RET_OK;
		if(nameProp.equals("arma"))				// 武器
		{
			res = convHandlePropStrToUnitId(&unitidProp, &unitProp, &nameProp, unitGlob, EquipPlace::EQP_RHAND);
		}
		else if(nameProp.equals("fura arma"))	// 右手武器
		{
			res = convHandlePropStrToUnitId(&unitidProp, &unitProp, &nameProp, unitGlob, EquipPlace::EQP_RHAND);
		}
		else if(nameProp.equals("hara arma"))	// 左手武器
		{
			return SoumaScript::RET_NO_TARGET;	// <ERROR> 対象が見つからなかった
		}
		// 何らかのエラー
		if(res != SoumaScript::RET_OK)
		{
			return res;	// <ERROR> エラー
		}

		// プロパティによる取得値を設定する
		if(unitid != 0L)	{	*unitid = unitidProp;	}
		if(unit != 0L)		{	*unit = unitProp;		}
	}

	return (s32)SoumaScript::RET_OK;
}

/*---------------------------------------------------------------------*//**
	プロパティのハンドル文字列をハンドルに変換する
**//*---------------------------------------------------------------------*/
s32 Souma::convHandlePropStrToUnitId(u16* unitid, Unit** unit, const VcString* strProp, Unit* unitGlobal, s32 ieqp)
{
	if(TFW_IS_FLAG(unitGlobal->getUnitFlags(), Unit::UNITFLAG_PC))
	{
		PcUnit* unitPc = (PcUnit*)unitGlobal;
		const ItemEquip* itmeq = unitPc->getItemEquip();
		if(itmeq == 0L)
		{
			return SoumaScript::RET_NO_TARGET;	// <ERROR> 対象が見つからなかった
		}
		const Item* item = itmeq->getEquipStat()->getEquipItem(ieqp);
		if(item == 0L)
		{
			return SoumaScript::RET_NO_TARGET;	// <ERROR> 対象が見つからなかった
		}
		Unit* unitItm = itmeq->getEquipUnit(ieqp);
		if(unitItm == 0L)
		{
			return SoumaScript::RET_NO_TARGET;	// <ERROR> 対象が見つからなかった
		}

		// 装備品は存在する

		if(unit != 0L)
		{
			*unit = unitItm;
			ASSERT(*unit != 0L);
		}
		if(unitid != 0L)
		{
			*unitid = unitItm->getId();
		}

		return (s32)SoumaScript::RET_OK;
	}

	return SoumaScript::RET_NO_TARGET;	// <ERROR> 対象が見つからなかった
}

/*---------------------------------------------------------------------*//**
	ハンドルからユニットに変換する
**//*---------------------------------------------------------------------*/
bool Souma::convHandleToUnit(Unit** unit, u32 handle)
{
	UnitManager* unitmng = Game::getGame()->getUnitManager(); ASSERT(unitmng != 0L);
	u16 unitid = (u16)(handle - _handleBase);
	*unit = unitmng->getUnitFromId(unitid);
	return *unit != 0L;
}
#endif

/*---------------------------------------------------------------------*//**
	パーティクル出現
**//*---------------------------------------------------------------------*/
Particle* Souma::appearParticle(u16 ptcldid, const Vector3F* pos, u32 flags)
{
	ParticleSys* ptclsys = Game::getGame()->getParticleSys();	ASSERT(ptclsys != 0L);
	s16 ptclid = ptclsys->appearParticle(ptcldid, pos, flags);
	if(ptclid <= 0)
	{
		return 0L;
	}
	Particle* ptcl = ptclsys->getParticle(ptclid);
	if(ptcl == 0L)
	{
		return 0L;
	}
	return ptcl;
}

/*---------------------------------------------------------------------*//**
	クラスターのキャラクターとのオフセット位置を算出する
**//*---------------------------------------------------------------------*/
void Souma::calcMagicClusterOffset(Vector3F* offset, f32 xofs, f32 radius) const
{
	const f32 X_BASIC_OFFSET = 0.0f;
	const f32 Y_BASIC_OFFSET = 0.0f;
	const f32 Z_BASIC_OFFSET = 21.0f;
	const Vector3F* dirChar = _unitRef->getDirection();
	Matrix4F mtxRot;
	mtxRot.rotateY(dirChar->y());
	offset->set(X_BASIC_OFFSET + xofs, Y_BASIC_OFFSET + (radius * 0.5f), Z_BASIC_OFFSET + radius);
	mtxRot.mult(offset, offset);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
