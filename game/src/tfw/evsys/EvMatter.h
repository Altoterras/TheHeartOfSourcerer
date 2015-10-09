/***********************************************************************//**
 *	EvMatter.cpp
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/09/29.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_EVSYS_EV_MATTER_H_
#define _TFW_EVSYS_EV_MATTER_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "../framemod/FrameExecDraw.h"

#include "EvCause.h"
#include "EvMatterCreateParam.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class Vector3;
typedef Vector3<f32> Vector3F;
class EvData;
class XmlNode;

/*---------------------------------------------------------------------*//**
 *	イベント マター
 *
 *		※１：
 *			イベント ID は同時存在するマター間で重複してはならない
 *		※２：
 *			ケース ID はマター間で重複しても良い（重複すること前提）が、
 *			同時存在するマター間で別の意味合いのものがあってはならない
 *
**//*---------------------------------------------------------------------*/
class EvMatter : public FrameExecDraw
{
	//======================================================================
	// 定数
protected:
	// 制御フラグ
	static const u16	CTRLF_UNLOCK		= 0x0001;	// 操作ロックしないイベントである
	static const u16	CTRLF_OVERRIDED		= 0x0002;	// 上書きされて無効化されたイベントである

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// マタータイプを得る
	inline u16 getMatterType() const { return _emtype; }
	// イベント ID を得る
	inline u16 getEvId() const { return _evidThis; }
	// 次に実行されるイベントのイベント ID を得る
	inline u16 getNextEvId() const { return _evidNext; }
	// 併発するイベントのイベント ID を得る
	inline u16 getConcurrentEvId() const { return _evidConcurrent; }
	// 親イベントのイベント ID を得る
	inline u16 getParentEvId() const { return _evidParent; }
	// 実行原因を得る
	inline const EvCause* getCause() const { return &_cause; }
	inline EvCause* cause() { return &_cause; }
	// 汎用保持パラメータを得る
	inline addr_size getPreserveParam() const { return _paramPresv; }
	// 汎用 Begin パラメータを得る
	inline u32 getBeginParam() const { return _paramBegin; }
	// 現在の経過フレーム値を得る
	inline f32 getElapsedFrame() const { return _fcntMatter; }
	// ポーズ時の限界フレーム値を得る
	inline f32 getPauseLimitFrame() const { return _fcntPauseLimit; }
	
	// 開始されているかどうかを得る	
	inline bool isBegin() const { return _step > 0; }
	// 終了しているかどうかを得る
	inline bool isEnd() const { return _step <= 0; }
	// 終了直後のチェック
	inline bool isEndTrigger() { return _step == -1 ? (_step++ <= 0) : false; }
	// 操作ロックイベントかどうかを得る
	inline bool isLock() const { return !TFW_IS_FLAG(_ctrlflags, CTRLF_UNLOCK); }

	// 親イベントのイベント ID を設定する
	void setParentEvId(u16 evid) { _evidParent = evid; }
	// 汎用保持パラメータを設定する
	void setPreserveParam(addr_size param) { _paramPresv = param; }
	// 操作ロック制御
	void setLock(bool isLock) { TFW_SET_FLAG(_ctrlflags, CTRLF_UNLOCK, !isLock); }
	// ポーズ時の限界フレーム値を設定する
	void setPauseLimitFrame(f32 framePauseLimit) { _fcntPauseLimit = framePauseLimit; }

protected:
	// ステップを得る
	s16 getStep() const { return _step; }
	// ステップを進める
	void advanceStep() { _step++; }
	// ステップをジャンプする
	void jumpStep(s16 step) { _step = step; }

	// イベント ID を設定する
	inline void setEvId(u16 evid) { _evidThis = evid; }
	// 次に実行されるイベントのイベント ID を設定する
	inline void setNextEvId(u16 evid) { _evidNext = evid; }
	// 併発するイベントのイベント ID を設定する
	inline void setConcurrentEvId(u16 evid) { _evidConcurrent = evid; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	virtual bool		judgeBeginFlags(u16 evid, u32 flagsCond) const;
	virtual bool		judgeBeginCustom() const;
	virtual void		markBegin();
	bool				overrideMatter(u16 evidOvrd);
	bool				unoverrideMatter();
	bool				isOverrideMatter() const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EvMatter(u16 emtype);
	virtual ~EvMatter();

	virtual bool		create(EvData* evdat, u16 evid);
	virtual void		destroy();
	virtual bool		setupFromXmlNode(const XmlNode* node, const EvMatterCreateParam* cparam);
	void				setCause(const EvCause* cause);
	void				setOption(u16 evidNext, u16 evidConcurrent);
	virtual void		begin(u32 paramBegin);
	virtual void		end();
	void				exec(ExecRes* res, const ExecCtx* ec);
	virtual void		execImplement(ExecRes* res, const ExecCtx* ec, f32 frameBegun);
	virtual void		render(const RenderCtx* rc) {}
	virtual void		draw(const RenderCtx* rc) {}

	//======================================================================
	// 変数
private:
	u16					_emtype;			// マタータイプ
	u16					_evidThis;			// イベント ID（※１）
	u16					_evidNext;
	u16					_evidConcurrent;
	u16					_evidParent;
	EvCause				_cause;
	addr_size			_paramPresv;
	u32					_paramBegin;

	f32					_fcntMatter;
	f32					_fcntPauseLimit;
	s16					_step;
	u16					_ctrlflags;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_EVSYS_EV_MATTER_H_
