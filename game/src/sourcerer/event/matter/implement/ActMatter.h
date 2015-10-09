/***********************************************************************//**
 *	ActMatter.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/12/07.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_MATTER_IMPLEMENT_ACT_MATTER_H_
#define _SRCR_EVENT_MATTER_IMPLEMENT_ACT_MATTER_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../EventMatter.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class List;
template<class TYPE> class ListIterator;
class VcString;
class XmlNode;

TFW_END_NS

SRCR_BEGIN_NS

class Act;
class EventSys;

/*---------------------------------------------------------------------*//**
 *	一連動作マター
 *	
**//*---------------------------------------------------------------------*/
class ActMatter : public EventMatter
{
	//======================================================================
	// 定数
private:

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	inline EventSys*		getEventSystemRefer()	{	return getOwner();		}
	inline List<Act*>*		getPlayingActList()		{	return _listActPlaying;	}
	inline List<Act*>*		getAllActList()			{	return _listActSong;	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部制御
public:
	void	skip();
	bool	checkValidSkip() const;
	bool	isWaitMovieMsg()	{	return _frameWaitMovieNext > 0.0f;	}
	void	doneWaitMovieMsg();

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	ActMatter();
	~ActMatter();
	bool	create(EvData* evdat, u16 evid);
	bool	setupFromXmlNode(const XmlNode* node, const EvMatterCreateParam* cparam);
	void	destroy();
	void	begin(u32 param);
	void	end();
	void	execImplement(ExecRes* res, const ExecCtx* ec, f32 frameBegun);
	void	draw(const RenderCtx* rc);

private:
	void	advFrame(f32 frameDelta);
	void	playAct(Act* act);
	void	stopAct(Act* act);
	void	pause(Act* actPause, bool isPause);

	//======================================================================
	// 変数
private:
	List<Act*>*			_listActSong;			// 全 Act のリスト
	List<Act*>*			_listActPlaying;		// 実行中の Act のリスト
	ListIterator<Act*>*	_itNext;				// 次回実行開始位置を示すイテレータ
	f32					_frameCur;
	f32					_framePrev;
	bool				_isNoAnimIntp;			// アニメーション補間をしない
	f32					_frameWaitMovieNext;	// ポーズ解除後のフレーム
	f32					_frameWaitMovieCur;		// ポーズ時も進行しているムービーのフレーム数
	f32					_frameWaitMovieMax;		// ポーズ時も進行しているムービーフレームの限界値
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_MATTER_IMPLEMENT_ACT_MATTER_H_
