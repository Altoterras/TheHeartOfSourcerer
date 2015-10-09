/***********************************************************************//**
 *	Act.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/05/13.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_MATTER_IMPLEMENT_ACT_ACT_H_
#define _SRCR_EVENT_MATTER_IMPLEMENT_ACT_ACT_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../../../Base.h"

#include "../../EventMatterCreateParam.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class XmlNode;

TFW_END_NS

SRCR_BEGIN_NS

class ActMatter;

/*---------------------------------------------------------------------*//**
 *	動作基本クラス
 *		※	Act の基本
 *			それぞれの Act はスクリプトの一行に相当する．
 *			Act には単発実行型と継続実行型の２種類がある．
 *			単発実行型は EventMatter を必要としないが、
 *			継続実行型は対応する EventMatter を必要とする．
 *	
**//*---------------------------------------------------------------------*/
class Act
{
public:
	static const u8	F_NULL		= 0x00;
	static const u8	F_PAUSE		= 0x01;		// この Act が終わるまでフレームを進めない
	static const u8	F_TERMINATE	= 0x02;		// 全ての Act の実行を中断する
	static const u8	F_NOSKIP	= 0x04;		// スキップ中でも実行する
	static const u8	F_SKIPONLY	= 0x10;		// スキップ中のみ実行する
	static const u8	F_SKIPSTOP	= 0x20;		// スキップを強制停止する

public:
	u8				_type;			// 種別（ActType）
	u8				_flags;			// フラグ（F_*）
	u16				_evid;			// イベント ID
	s32				_frame;			// ［必須］開始フレーム
	s32				_duration;		// ［オプション（デフォルト: 0）］継続フレーム数
	VcString*		_name;			// ［オプション（デフォルト: NULL）］名前

public:
	Act(u8 type) : _type(type), _flags(F_NULL), _evid(0), _frame(0), _duration(0), _name(0L) {}
	virtual ~Act();
	virtual bool analyze(ActMatter* owner, const XmlNode* node, const EventMatterCreateParam* emcp);
	virtual void play(ActMatter* owner) {}
	virtual void stop(ActMatter* owner) {}
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_MATTER_IMPLEMENT_ACT_ACT_H_
