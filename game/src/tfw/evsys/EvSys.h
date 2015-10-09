/***********************************************************************//**
 *	EvSys.h
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/09/30.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_EVSYS_EV_SYS_H_
#define _TFW_EVSYS_EV_SYS_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"
#include "../framemod/FrameExecDraw.h"

#include "EvCause.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class EvData;
class EvMatter;
class VcString;

/*---------------------------------------------------------------------*//**
 *	イベント システム
 *		※　汎用システム化検討中．ゲーム固有ソースを極力減らすこと．
 *
**//*---------------------------------------------------------------------*/
class EvSys : public FrameExecDraw
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// イベントデータを得る
	EvData* getEvData() { return _evdataRef; }
	// 何らかのイベント実行中かを取得する
	inline bool isBegunEvent() const { return _isRunMatter; }
	// 操作ロック中かどうかを取得する
	inline bool isOperationLock() const { return _isOpLock; }
	// 開始中のイベント数を得る
	int getBegunEventCount() const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	EvMatter* begin(u16 evid, u32 paramBegin, const EvCause* cause);
	void terminateEventRange(u16 evidRangeStart, u16 evidRangeEnd);
	void terminateAllEvent(u16 evidExclusive);

	bool checkBegunOtherMatter(EvMatter* matterExclusion, bool isParentExclusion) const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	EvSys();
	~EvSys();
	bool create(EvData* evdatRef);
	void destroy();
	virtual void exec(ExecRes* res, const ExecCtx* ec);	// フレーム制御
	virtual void render(const RenderCtx* rc);			// 3D 描画
	virtual void draw(const RenderCtx* rc);				// 2D 描画

protected:
	EvMatter* begin(u16 evid, u32 paramBegin, u32 flagsCond, const EvCause* cause);

	//======================================================================
	// 変数
private:
	EvData*		_evdataRef;
	bool		_isRunMatter;
	bool		_isOpLock;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_EVSYS_EV_SYS_H_
