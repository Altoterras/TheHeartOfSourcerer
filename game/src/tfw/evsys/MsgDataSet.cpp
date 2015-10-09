/***********************************************************************//**
 *	MsgDataSet.cpp
 *	Terras Framework
 *
 *	Created by Ryoutarou Kishi on 2009/12/07.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "MsgDataSet.h"

// Friends

// External

TFW_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	メッセージから文字列を取得する
**//*---------------------------------------------------------------------*/
VcString MsgDataSet::getString(const MsgData::Msg* msg) const
{
	ASSERT((0 <= msg->_catid) && (msg->_catid < _numKind));
	u16 msgkind = msg->_catid;
	return _arrMsgData[msgkind]->getString(msg);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	特定種別のメッセージデータを検索する
**//*---------------------------------------------------------------------*/
const MsgData::Msg* MsgDataSet::findMessageFromMsgid(u16 kind, u16 msgid, MsgData::FindHint* hint) const
{
	if(kind >= _numKind)		{	return 0L;	}
	if(_arrMsgData[kind] == 0L)	{	return 0L;	}
	const MsgData::Msg* msg = _arrMsgData[kind]->findMessageFromMsgid(msgid, hint);
	if(msg == 0L)				{	return 0L;	}
	return msg;
}

/*---------------------------------------------------------------------*//**
	未特定種別のメッセージデータを検索する
**//*---------------------------------------------------------------------*/
const MsgData::Msg* MsgDataSet::findMessageFromMsgid(u16 msgid, MsgData::FindHint* hint) const
{
	for(int i = 0; i < _numKind; i++)
	{
		if(_arrMsgData[i] != 0L)
		{
			const MsgData::Msg* msg = _arrMsgData[i]->findMessageFromMsgid(msgid, hint);
			if(msg != 0L)
			{
				return msg;
			}
		}
	}
	return 0L;
}

/*---------------------------------------------------------------------*//**
	メッセージデータを追加する
**//*---------------------------------------------------------------------*/
bool MsgDataSet::addMsgData(u16 msgkind, FileBase* file, u16 ofsMsgid)
{
	MsgData* msgdat = new MsgData();
	if(!msgdat->create(msgkind, file, ofsMsgid))
	{
		delete msgdat;
		return false;
	}

	if(_arrMsgData[msgkind] != 0L)
	{
		removeMsgDataRange(msgkind);
	}

	ASSERT((0 <= msgkind) && (msgkind < _numKind));
	ASSERT(_arrMsgData[msgkind] == 0L);
	_arrMsgData[msgkind] = msgdat;
	return true;
}

/*---------------------------------------------------------------------*//**
	メッセージデータを削除する

	@param msgkind メッセージ種別
**//*---------------------------------------------------------------------*/
void MsgDataSet::removeMsgDataRange(u16 msgkind)
{
	ASSERT((0 <= msgkind) && (msgkind < _numKind));
	if(_arrMsgData[msgkind] != 0L)
	{
		_arrMsgData[msgkind]->destroy();
		delete _arrMsgData[msgkind];
		_arrMsgData[msgkind] = 0L;
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
MsgDataSet::MsgDataSet()
	: _arrMsgData(0L)
	, _numKind(0L)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
MsgDataSet::~MsgDataSet()
{
	ASSERT(_arrMsgData == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool MsgDataSet::create(u16 numKind)
{
	destroy();

	_numKind = numKind;
	_arrMsgData = new MsgData*[_numKind];
	if(_arrMsgData == 0L)
	{
		_numKind = 0;
		return false;
	}
	for(int i = 0; i < _numKind; i++)
	{
		_arrMsgData[i] = 0L;
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void MsgDataSet::destroy()
{
	for(int i = 0; i < _numKind; i++)
	{
		if(_arrMsgData[i] != 0L)
		{
			_arrMsgData[i]->destroy();
			delete _arrMsgData[i];
			_arrMsgData[i] = 0L;
		}
	}
	delete[] _arrMsgData;
	_arrMsgData = 0L;
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS
