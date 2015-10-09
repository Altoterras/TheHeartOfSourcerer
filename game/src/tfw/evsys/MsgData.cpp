/***********************************************************************//**
 *	MsgData.cpp
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
#include "MsgData.h"

// Friends

// External
#include "../file/IdFile.h"
#include "../string/StringUtils.h"

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
VcString MsgData::getString(const Msg* msg) const
{
	ASSERT(msg->_indexStart <= _strAll->getLength());
	ASSERT(msg->_indexEnd <= _strAll->getLength());
	return _strAll->substring(msg->_indexStart, msg->_indexEnd);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	メッセージを取得する
**//*---------------------------------------------------------------------*/
const MsgData::Msg* MsgData::findMessageFromMsgid(u16 msgid) const
{
	for(int i = 0; i < _numMsg; i++)
	{
		if(_marrMsg[i]._msgid == msgid)
		{
			return &_marrMsg[i];
		}
	}
	return 0L;
}

/*---------------------------------------------------------------------*//**
	メッセージを取得する
		連続したメッセージを検索する場合に高速になるヒント対応版
**//*---------------------------------------------------------------------*/
const MsgData::Msg* MsgData::findMessageFromMsgid(u16 msgid, FindHint* hint) const
{
	if(hint == 0L)	{	return findMessageFromMsgid(msgid);	}

	s32 indexStart = hint->_index;
	while(hint->_index < _numMsg)
	{
		if(_marrMsg[hint->_index]._msgid == msgid)
		{
			return &_marrMsg[hint->_index++];
		}
		hint->_index++;
	}
	hint->_index = 0;
	while(hint->_index < indexStart)
	{
		if(_marrMsg[hint->_index]._msgid == msgid)
		{
			return &_marrMsg[hint->_index++];
		}
		hint->_index++;
	}
	return 0L;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
MsgData::MsgData()
	: _marrMsg(0L)
	, _numMsg(0)
	, _strAll(0L)
{
	ASSERT(sizeof(EnlightEventMsgDataHeader) == EnlightEventMsgDataHeader::SIZE_THIS);
	ASSERT(sizeof(EnlightEventMsgData) == EnlightEventMsgData::SIZE_THIS);
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
MsgData::~MsgData()
{
	ASSERT(_marrMsg == 0L);
	ASSERT(_strAll == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool MsgData::create(u16 catid, FileBase* file, u16 ofsMsgid)
{
	// メッセージデータ読み込み
	EnlightEventMsgDataHeader head;
	if(file->read(&head, sizeof(EnlightEventMsgDataHeader)) < sizeof(EnlightEventMsgDataHeader))
	{
		ASSERT(false);
		return false;
	}
	// データ読み込み
	Msg* marrMsg = new Msg[head._numMsg];
	for(int i = 0; i < (int)head._numMsg; i++)
	{
		EnlightEventMsgData data;
		if(file->read(&data, sizeof(EnlightEventMsgData)) < sizeof(EnlightEventMsgData))
		{
			delete marrMsg;
			ASSERT(false);
			return false;
		}

		marrMsg[i]._msgid = data._msgid + ofsMsgid;
		marrMsg[i]._param1 = data._param1;
		marrMsg[i]._indexStart = data._indexStart;
		marrMsg[i]._indexEnd = data._indexEnd;
		marrMsg[i]._catid = catid;
	}
	// 文字列読み込み
	char* cbuf = new char[head._lenString];
	if(file->read(cbuf, head._lenString) < head._lenString)
	{
		delete marrMsg;
		delete cbuf;
		ASSERT(false);
		return false;
	}
	_strAll = new VcString(cbuf);
	delete cbuf;

	_numMsg = head._numMsg;
	_marrMsg = marrMsg;
	
	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void MsgData::destroy()
{
	// 削除
	delete _strAll;
	_strAll = 0L;
	delete[] _marrMsg;
	_marrMsg = 0L;
}

////////////////////////////////////////////////////////////////////////////

TFW_END_NS
