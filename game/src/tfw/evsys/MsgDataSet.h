/***********************************************************************//**
 *	MsgDataSet.h
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/12/07.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _TFW_EVSYS_MSG_DATA_SET_H_
#define _TFW_EVSYS_MSG_DATA_SET_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../Base.h"

#include "MsgData.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class FileBase;

/*---------------------------------------------------------------------*//**
 *	メッセージ データ セット
 *	
**//*---------------------------------------------------------------------*/
class MsgDataSet
{
	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	VcString getString(const MsgData::Msg* msg) const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	const MsgData::Msg* findMessageFromMsgid(u16 kind, u16 msgid, MsgData::FindHint* hint) const;
	const MsgData::Msg* findMessageFromMsgid(u16 msgid, MsgData::FindHint* hint) const;
	inline const MsgData::Msg* findMessageFromMsgid(u16 kind, u16 msgid) const { return findMessageFromMsgid(kind, msgid, (MsgData::FindHint*)0L); }
	inline const MsgData::Msg* findMessageFromMsgid(u16 msgid) const { return findMessageFromMsgid(msgid, (MsgData::FindHint*)0L); }
	bool addMsgData(u16 msgkind, FileBase* file, u16 ofsMsgid);
	void removeMsgDataRange(u16 msgkind);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	MsgDataSet();
	~MsgDataSet();
	bool create(u16 numKind);
	void destroy();

	//======================================================================
	// 変数
private:
	MsgData** _arrMsgData;
	u16 _numKind;
};

TFW_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _TFW_EVSYS_MSG_DATA_SET_H_
