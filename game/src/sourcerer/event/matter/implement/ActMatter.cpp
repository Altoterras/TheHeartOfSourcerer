/***********************************************************************//**
 *	ActMatter.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/12/07.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "ActMatter.h"

// Friends
#include "act/Act.h"
#include "act/ActFactory.h"
#include "RealtimeMovieMatter.h"
#include "TalkMatter.h"
#include "../EventMatterType.h"
#include "../EventMatterCreateParam.h"
#include "../../EventData.h"
#include "../../EventSys.h"
#include "../../EventCause.h"
#include "../../../debug/GameTest.h"
#include "../../../body/Game.h"
#include "../../../body/GameNotifier.h"
#include "../../../gui/GameFontSet.h"
#include "../../../episode/Situation.h"
#include "../../../episode/StoryManager.h"

// External
#include "../../../../etk/gui/window/MessageWindow.h"
#include "../../../../etk/sg/SorSgm.h"
#include "../../../../etk/sound/SoundMngr.h"
#include "../../../../tfw/collection/List.h"
#include "../../../../tfw/g2d/font/Font.h"
#include "../../../../tfw/g3d/sg/node/implement/AnimCamera.h"
#include "../../../../tfw/g3d/sg/node/implement/ShapeModel.h"
#include "../../../../tfw/g3d/sg/node/implement/LocAngCamera.h"
#include "../../../../tfw/lib/XmlParser.h"
#include "../../../../tfw/lib/XmlParserUtils.h"
#include "../../../../tfw/lib/Vector.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// ActMatter メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	実行中の Act を全てスキップする
**//*---------------------------------------------------------------------*/
void ActMatter::skip()
{
	// 一時停止 Act 実行中はスキップできない
	ListIterator<Act*> itPlaying = _listActPlaying->iterator();
	while(itPlaying.has())
	{
		Act* act = itPlaying.object();
		if(TFW_IS_FLAG(act->_flags, Act::F_PAUSE))
		{
			return;
		}

		itPlaying.next();
	}

	// 実行中の Act を強制終了
	itPlaying = _listActPlaying->iterator();
	while(itPlaying.has())
	{
		ListIterator<Act*> itNextPlaying = itPlaying;
		itNextPlaying.next();

		Act* act = itPlaying.object();
		stopAct(act);	// 停止する

		itPlaying = itNextPlaying;
	}

	// 台詞待ちを強制解除
	if(_frameWaitMovieNext > 0.0f)
	{
		_frameWaitMovieNext = 0.0f;
		_frameWaitMovieMax = 0.0f;
		_frameWaitMovieCur = 0.0f;
		getOwner()->setWaitMovieMessageNow(false);
	}

	// スキップ中でも実行すべき Act を強制実行
	//	※ ただし、単発実行型の Act のみが有効であることに注意
	ListIterator<Act*> itWk = *_itNext;
	while(itWk.has())
	{
		Act* act = itWk.object();
		if(TFW_IS_FLAG(act->_flags, Act::F_SKIPSTOP))
		{
			// フレームを停止地点まで進めてスキップ終了
			_frameCur = (f32)act->_frame;
			return;
		}
		if(TFW_IS_FLAG(act->_flags, Act::F_NOSKIP) || TFW_IS_FLAG(act->_flags, Act::F_SKIPONLY))
		{
			playAct(act);	// 実行する

			*_itNext = itWk;
			_itNext->next();

			// 中断確認
			if(TFW_IS_FLAG(act->_flags, Act::F_TERMINATE))
			{
				end();
				return;
			}
		}

		itWk.next();
	}

	// 再度、実行中の Act を強制終了
	itPlaying = _listActPlaying->iterator();
	while(itPlaying.has())
	{
		ListIterator<Act*> itNextPlaying = itPlaying;
		itNextPlaying.next();

		Act* act = itPlaying.object();
		stopAct(act);	// 停止する

		itPlaying = itNextPlaying;
	}

	// 強制中断
	end();
}

/*---------------------------------------------------------------------*//**
	スキップ可能か調べる
**//*---------------------------------------------------------------------*/
bool ActMatter::checkValidSkip() const
{
	// 一時停止 Act 実行中はスキップできない
	ListIterator<Act*> itPlaying = _listActPlaying->iterator();
	while(itPlaying.has())
	{
		Act* act = itPlaying.object();
		if(TFW_IS_FLAG(act->_flags, Act::F_PAUSE))
		{
			return false;
		}

		itPlaying.next();
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	ムービー台詞待ちを終える
**//*---------------------------------------------------------------------*/
void ActMatter::doneWaitMovieMsg()
{
	if(_frameWaitMovieNext > 0.0f)
	{
		EventData* evdat = getOwner()->getEventData(); ASSERT(evdat != 0L);

		// 台詞待ち解除
		f32 framePauseNextBk = _frameWaitMovieNext;
		_frameWaitMovieNext = 0.0f;
		getOwner()->setWaitMovieMessageNow(false);

		// アニメーションを残り分進める
		///bool isNoAnimIntpBk = Game::getGame()->isNoAnimInterpolation();
		///Game::getGame()->setNoAnimInterpolation(true);	// 補間を無効化
		{
			f32 frameSkipMovie = _frameWaitMovieMax - _frameWaitMovieCur;
			if(frameSkipMovie > 0.0f)
			{
				ExecCtx ecForSkipMovie(frameSkipMovie, 0);
				for(ListIterator<Act*> itPlaying = _listActPlaying->iterator(); itPlaying.has(); itPlaying.next())
				{
					EventMatter* matter = evdat->getEventMatterFromEvid(itPlaying.object()->_evid);
					if((matter != 0L) && matter->isBegin() && (matter->getMatterType() == EMTYPE_REALTIMEMOVIE))	// 実行中の REALTIMEMOVIE マター
					{
						RealtimeMovieMatter* rm = (RealtimeMovieMatter*)matter;

						// カメラ・モデルのフレームを残り分進める（スキップする）
						AnimCamera* cam = rm->animCamera();
						if(cam != 0L)
						{
							cam->exec(0L, &ecForSkipMovie);
						}
						ShapeModel** mdlarr = rm->shapeModelArray();
						if(mdlarr != 0L)
						{
							for(int i = 0; i < rm->getShapeModelNum(); i++)
							{
								if(mdlarr[i] != 0L)	{	mdlarr[i]->exec(0L, &ecForSkipMovie);	}
							}
						}

					}
				}
			}

			f32 frameSkipAct = framePauseNextBk - _frameCur;
			if(frameSkipAct > 0.0f)
			{
				ExecCtx ecForSkip(frameSkipAct, 0);
				execImplement(0L, &ecForSkip, 0.0f);
			}
		}
		///Game::getGame()->setNoAnimInterpolation(isNoAnimIntpBk);	// 補間を戻す

		// メッセージウインドウの次へカーソルを消す
		for(ListIterator<Act*> itPlaying = _listActPlaying->iterator(); itPlaying.has(); itPlaying.next())
		{
			EventMatter* matter = evdat->getEventMatterFromEvid(itPlaying.object()->_evid);
			if((matter != 0L) && matter->isBegin() && (matter->getMatterType() == EMTYPE_TALK))	// 実行中の TALK マター
			{
				TalkMatter* tm = (TalkMatter*)matter;
				MessageWindow* wndMsg = tm->messageWindow();
				if((tm->getKind() == TalkMatter::KIND_SAY) && (wndMsg != 0L))
				{
					wndMsg->setEnableNextCursor(false);
				}
			}
		}
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
ActMatter::ActMatter()
	: EventMatter(EMTYPE_ACT)
	, _listActSong(0L)
	, _listActPlaying(0L)
	, _itNext(0L)
	, _frameCur(0.0f)
	, _framePrev(0.0f)
	, _isNoAnimIntp(false)
	, _frameWaitMovieNext(0.0f)
	, _frameWaitMovieCur(0.0f)
	, _frameWaitMovieMax(0.0f)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
ActMatter::~ActMatter()
{
	ASSERT(_listActSong == 0L);
	ASSERT(_listActPlaying == 0L);
	ASSERT(_itNext == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool ActMatter::create(EvData* evdat, u16 evid)
{
	if(!EventMatter::create(evdat, evid))	{	return false;	}

	// Act リストを作成
	_listActSong = new List<Act*>(true);
	_listActPlaying = new List<Act*>(false);
	_itNext = new ListIterator<Act*>;

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void ActMatter::destroy()
{
	// Act リストを削除
	delete _itNext;
	_itNext = 0L;
	delete _listActPlaying;
	_listActPlaying = 0L;
	delete _listActSong;
	_listActSong = 0L;

	EventMatter::destroy();
}

/*---------------------------------------------------------------------*//**
	開始
**//*---------------------------------------------------------------------*/
void ActMatter::begin(u32 param)
{
	EventMatter::begin(param);
	
#if 0
	// 既存のパーティクルを全て閉じる
	ParticleSys* ptclsys = Game::getGame()->getParticleSys();	ASSERT(ptclsys != 0L);
	ptclsys->disappearAllParticle();
#endif

	// 最初のノードをイテレータでポイント
	ASSERT(_itNext != 0L);
	*_itNext = _listActSong->iterator();

	// 開始
	_framePrev = _frameCur = 0.0f;
	advFrame(1.0f);

	// 開始通知
	Game::getGame()->getGameNotifier()->notifyEventActMatterTrigger(getEvId(), true, (const Unit*)getCause()->getParameter());
}

/*---------------------------------------------------------------------*//**
	終了
**//*---------------------------------------------------------------------*/
void ActMatter::end()
{
	// 実行中の Act を強制終了後、クリア
	ListIterator<Act*> itPlaying = _listActPlaying->iterator();
	while(itPlaying.has())
	{
		ListIterator<Act*> itNextPlaying = itPlaying;
		itNextPlaying.next();

		Act* act = itPlaying.object();
		stopAct(act);	// 停止する

		itPlaying = itNextPlaying;
	}
	_listActPlaying->removeAll();

	// 子マターを全て削除する
	EventData* evdat = getOwner()->getEventData();	ASSERT(evdat != 0L);
	evdat->removeChildMatter(getEvId());

	// 自分以外の ActMatter があるかどうかチェック（Call などの作用で）
	bool isExistOtherActMatter = false;
	for(s32 i = 0; i < evdat->getEventMatterNum(); i++)
	{
		EventMatter* matter = evdat->getEventMatter(i);
		if(matter->getMatterType() == EMTYPE_ACT)	// ActMatter 判定
		{
			if(matter->isBegin())
			{
				if(matter->getEvId() != this->getEvId())
				{
					isExistOtherActMatter = true;	// 存在する
					break;
				}
			}
		}
	}

	// 他に ActMatter が無い場合は状態を戻す
	if(!isExistOtherActMatter)
	{
		// BGM 制御制限は強制解除
		Game::getGame()->getSoundManager()->restrictControlBgm(0);	// 解除設定

		// アニメーション補間禁を強制解除する
		Game::getGame()->endNoAnimInterpolation(0);
	}

	// 終了通知
	Game::getGame()->getGameNotifier()->notifyEventActMatterTrigger(getEvId(), false, (const Unit*)getCause()->getParameter());

	EventMatter::end();
}

/*---------------------------------------------------------------------*//**
	XML 要素から読み込む
**//*---------------------------------------------------------------------*/
bool ActMatter::setupFromXmlNode(const XmlNode* node, const EvMatterCreateParam* cparam)
{
	if(!EventMatter::setupFromXmlNode(node, cparam))	{	return false;	}

	ASSERT(_listActSong != 0L);

	// - - - - - - - - - - - - - - - - - - - - - - - - -
	// ノード解析
	{
		const XmlNode* nodeChild = node->getChildNode();
		while(nodeChild != 0L)
		{
			const VcString* nameNode = nodeChild->getName();
			Act* act = ActFactory::makeAct(nameNode);

			if(act != 0L)
			{
				if(act->analyze(this, nodeChild, (const EventMatterCreateParam*)cparam))
				{
					_listActSong->addTail(act);
				}
				else
				{
					delete act;
				}
			}

			nodeChild = nodeChild->getSiblingNode();
		}
	}
	
	return true;
}

/*---------------------------------------------------------------------*//**
	毎フレーム処理実装
**//*---------------------------------------------------------------------*/
void ActMatter::execImplement(ExecRes* res, const ExecCtx* ec, f32 frameBegun)
{
	EventData* evdat = getOwner()->getEventData(); ASSERT(evdat != 0L);

	// 文章読み上げ終了時のムービー台詞待ち処理
	for(ListIterator<Act*> itPlaying = _listActPlaying->iterator(); itPlaying.has(); itPlaying.next())
	{
		Act* act = itPlaying.object();
		EventMatter* matter = evdat->getEventMatterFromEvid(act->_evid);
		if((matter != 0L) && matter->isBegin() && (matter->getMatterType() == EMTYPE_TALK))	// 実行中の TALK マター
		{
			TalkMatter* tm = (TalkMatter*)matter;
			if(tm->getKind() == TalkMatter::KIND_SAY)
			{
				// ムービー台詞待ち設定変更の反映
				#if 0
				{
					MessageWindow* wndMsg = tm->messageWindow();
					if(wndMsg != 0L)
					{
						if(wndMsg->isWaitSayMode() != getOwner()->isEnableWaitMovieMessage())
						{
							getOwner()->setEnableWaitMovieMessage(wndMsg->isWaitSayMode());
						}
					}
				}
				#endif

				// 台詞待ちへ
				if(tm->getState() == TalkMatter::STATE_DONE_READING)
				{
					tm->lingering();	// 状態を余韻に
					if(getOwner()->isEnableWaitMovieMessage())	// ムービー台詞待ち有効時
					{
						_frameWaitMovieNext = (f32)((act->_duration != -1) ? act->_frame + act->_duration : act->_frame);
						_frameWaitMovieCur = _frameCur;
						#if 1	// ムービー行けるだけ行くバージョン
							_frameWaitMovieMax = _frameWaitMovieNext;
						#else	// ムービーも即座に止めるバージョン
							_frameWaitMovieMax = _frameCur;
						#endif

						MessageWindow* wndMsg = tm->messageWindow();
						if(wndMsg != 0L) { wndMsg->setEnableNextCursor(true); }	// 次へカーソルを有効化
					}
				}
			}
		}
	}
	// 待ち中は抜ける
	if(_frameWaitMovieNext > 0.0f)
	{
		if(_frameWaitMovieCur < _frameWaitMovieMax)
		{
			_frameWaitMovieCur += ec->getDeltaFrame();
			if(_frameWaitMovieCur >= _frameWaitMovieMax)
			{
				if(!getOwner()->isWaitMovieMessageNow())
				{
					getOwner()->setWaitMovieMessageNow(true);	// ムービー台詞待ちに
				}
			}
		}
		return;
	}

	// ポーズ処理と自己終了したマターの自動停止処理
	{
		bool isWaitMatterPlaying = false;
		ListIterator<Act*> itPlaying = _listActPlaying->iterator();
		while(itPlaying.has())
		{
			ListIterator<Act*> itNextPlaying = itPlaying;
			itNextPlaying.next();

			Act* act = itPlaying.object();
			EventMatter* matter = evdat->getEventMatterFromEvid(act->_evid);
			if((matter != 0L) && matter->isBegin())	// マター実行中
			{
				if(TFW_IS_FLAG(act->_flags, Act::F_PAUSE))	// 一時停止 Act を実行中
				{
					isWaitMatterPlaying = true;
				}
			}
			else // マターは自己終了している
			{
				TRACE("stopAct : curframe=%f, act{ frame=%d, durf=%d }\n", _frameCur, act->_frame, act->_duration);
				stopAct(act);			// 停止処理
			}

			itPlaying = itNextPlaying;
		}
		if(isWaitMatterPlaying)	{	return;	}	// 一時停止 Act 実行中の場合はここで抜ける
	}
	// ポーズから抜けた場合
	if(getPauseLimitFrame() >= 0.0f)	// ポーズ中であった
	{
		pause(0L, false);
	}

	// フレームを進める
	advFrame(ec->getDeltaFrame());

	// デュレイション（継続フレーム）による Act の停止処理
	ListIterator<Act*> itPlaying = _listActPlaying->iterator();
	while(itPlaying.has())
	{
		ListIterator<Act*> itNextPlaying = itPlaying;
		itNextPlaying.next();

		Act* act = itPlaying.object();
		if(	((act->_frame + act->_duration) <= _frameCur) && (act->_duration != -1) )
		{
			TRACE("stopAct(1) : curframe=%f, act{ frame=%d, durf=%d }\n", _frameCur, act->_frame, act->_duration);
			stopAct(act);	// 停止する
		}

		itPlaying = itNextPlaying;
	}

	// Act 開始処理
	ListIterator<Act*> itWk = *_itNext;
	while(itWk.has())
	{
		Act* act = itWk.object();
		if(act->_frame <= _frameCur)
		{
			if(!TFW_IS_FLAG(act->_flags, Act::F_SKIPONLY))	// スキップ中のみ実行の Act は除外
			{
				playAct(act);	// 実行する

				*_itNext = itWk;
				_itNext->next();

				// 中断確認
				if(TFW_IS_FLAG(act->_flags, Act::F_TERMINATE))
				{
					end();
					return;
				}
				// ポーズ確認
				if(TFW_IS_FLAG(act->_flags, Act::F_PAUSE))	// 一時停止 Act を実行中
				{
					pause(act, true);
					break;
				}
			}
		}
		else if(act->_frame > _frameCur)
		{
			break;
		}

		itWk.next();
	}

	// 0 デュレイション Act やスキップされた Act の停止処理
	itPlaying = _listActPlaying->iterator();
	while(itPlaying.has())
	{
		ListIterator<Act*> itNextPlaying = itPlaying;
		itNextPlaying.next();

		Act* act = itPlaying.object();
		if(	((act->_frame + act->_duration) <= _frameCur) && (act->_duration != -1) )
		{
			TRACE("stopAct(2) : curframe=%f, act{ frame=%d, durf=%d }\n", _frameCur, act->_frame, act->_duration);
			stopAct(act);	// 停止する
		}

		itPlaying = itNextPlaying;
	}

	// 再生中の Act もなく、次の Act もない場合、Matter 自体を終了する
	if((_listActPlaying->getCount() <= 0) && !_itNext->has())
	{
		end();
	}
}

/*---------------------------------------------------------------------*//**
	2D 描画
**//*---------------------------------------------------------------------*/
void ActMatter::draw(const RenderCtx* rc)
{
	#if defined(_DEBUG) && 0
	if(IS_TEST_FLAG(GameTest::F_SHOW_ACT_FRAME))	{	STRACE("[%d]ActFrame:%f %c\n", getEvId(), _frameCur, Game::getGame()->isNoAnimInterpolation() ? 'N' : ' ');	}
	#endif
}

/*---------------------------------------------------------------------*//**
	フレームを進める
**//*---------------------------------------------------------------------*/
void ActMatter::advFrame(f32 frameDelta)
{
	// フレームカウンタをインクリメント
	_framePrev = _frameCur;
	_frameCur += frameDelta;
	///TRACE("cur=%f, delta=%f\n", _frameCur, frameDelta);
}

/*---------------------------------------------------------------------*//**
	Act を実行する
**//*---------------------------------------------------------------------*/
void ActMatter::playAct(Act* act)
{
	// 動作開始
	act->play(this);

	// 実行中 Act リストに追加する
	_listActPlaying->addTail(act);
}

/*---------------------------------------------------------------------*//**
	Act を停止する
**//*---------------------------------------------------------------------*/
void ActMatter::stopAct(Act* act)
{
	// 動作停止
	act->stop(this);

	// 実行中 Act リストから削除する
	_listActPlaying->remove(act);
}

/*---------------------------------------------------------------------*//**
	一時停止処理（有効化／解除）
**//*---------------------------------------------------------------------*/
void ActMatter::pause(Act* actPause, bool isPause)
{
	EventData* evdat = getOwner()->getEventData(); ASSERT(evdat != 0L);

	// ポーズ時の限界フレーム値
	f32 frameLimit = isPause ? (f32)actPause->_frame : -1.0f;
	setPauseLimitFrame(frameLimit);

	// 実行中の子マターに限界フレーム値を設定する
	ListIterator<Act*> itPlaying = _listActPlaying->iterator();
	while(itPlaying.has())
	{
		ListIterator<Act*> itNextPlaying = itPlaying;
		itNextPlaying.next();

		Act* act = itPlaying.object();
		EventMatter* matter = evdat->getEventMatterFromEvid(act->_evid);
		if((matter != 0L) && matter->isBegin())	// マター実行中
		{
			// 子マターのポーズ時の限界フレーム値を設定する
			if(isPause)
			{
				if(act != actPause)	// ポーズの基点は除外
				{
					matter->setPauseLimitFrame(frameLimit - act->_frame);
				}
			}
			else
			{
				matter->setPauseLimitFrame(-1.0f);
			}
		}

		itPlaying = itNextPlaying;
	}
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
