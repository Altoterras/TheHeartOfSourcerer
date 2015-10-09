/***********************************************************************//**
 *	TalkMatter.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/09/30.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "TalkMatter.h"

// Friends
#include "../EventMatterType.h"
#include "../EventMatterCreateParam.h"
#include "../../EventSys.h"
#include "../../../body/Game.h"
#include "../../../body/GameFixedStrTbl.h"
#include "../../../gui/GameGui.h"
#include "../../../gui/window/implement/GameSysMsgWindow.h"
#include "../../../gui/window/implement/SubtitlesWindow.h"

// External
#include "../../../../tfw/collection/List.h"
#include "../../../../tfw/evsys/MsgDataSet.h"
#include "../../../../tfw/lib/XmlParser.h"
#include "../../../../tfw/lib/XmlParserUtils.h"
#include "../../../../tfw/string/VcStringArray.h"

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// TalkMatter::Data メソッド

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
TalkMatter::Data::Data(const VcString* msg, u16 charid)
	: _msgOrg(new VcString(msg))
	, _msgCur(new VcString(msg))
	, _charid(charid)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
TalkMatter::Data::~Data()
{
	delete _msgOrg;
	delete _msgCur;
}

//==========================================================================
// TalkMatter メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	データ数取得
**//*---------------------------------------------------------------------*/
int TalkMatter::getDataNum()
{
	return _listData->getCount();
}

/*---------------------------------------------------------------------*//**
	データ得る
**//*---------------------------------------------------------------------*/
TalkMatter::Data* TalkMatter::getData(int index)
{
	return _listData->getObject(index);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	会話を文章毎に設定する
**//*---------------------------------------------------------------------*/
void TalkMatter::setMessage(const VcString* msg, u16 charid, bool isAdd)
{
	ASSERT(msg != 0L);

	// 文章クリア
	if(!isAdd)
	{
		_listData->removeAll();
	}

	// タグの展開
	VcString msgt;
	replaceTagString(&msgt, msg);
	
	// 文章追加
	Data* data = new Data(&msgt, charid);
	_listData->addTail(data);

	// メッセージウインドウが既に作成済みならばメッセージウインドウにテキストを再設定する
	if(_wndMsgRef != 0L)
	{
		msgt = makeMessageString();
		_wndMsgRef->setReadingAnimation(true);
		_wndMsgRef->showMessage(&msgt, (s32)this->getEvId());
		_wndMsgRef->setGeneralParameter((u32)this->getEvId());
	}

	// 読み上げ開始
	_state = STATE_READING;
}

/*---------------------------------------------------------------------*//**
	会話をメッセージデータから設定する
**//*---------------------------------------------------------------------*/
void TalkMatter::setMessage(const MsgDataSet* msgdatset, u16 msgid, bool isAdd)
{
	ASSERT(msgdatset != 0L);

	// メッセージ ID 設定
	if(msgid != 0L)
	{
		_setting._msgid = msgid;
	}
	ASSERT(_setting._msgid != 0);	// ここで設定されない場合は既に setupFromXmlNode 等で設定されているはず
	if(_setting._msgid == 0)
	{
		end();
		return;
	}

	// メッセージ取得
	const MsgData::Msg* msg = msgdatset->findMessageFromMsgid(_setting._msgid);
	if(msg == 0L)
	{
		ASSERT(false);
		end();
		return;
	}

	// 文字列取得
	VcString str = msgdatset->getString(msg);

	// ウインドウタイプの設定
	setWindowType(msg->_param1 == 1);	// キャラクタ ID が１はシステムである

	// メッセージ設定
	setMessage(&str, msg->_param1, isAdd);
}

/*---------------------------------------------------------------------*//**
	会話を固定システム文字列から設定する
**//*---------------------------------------------------------------------*/
void TalkMatter::setFixedStrMessage(s32 fsstrid, bool isAdd)
{
	VcString msg = VcString::format(GameFixedStrTbl::getString(fsstrid)->getRaw(), "");

	// ウインドウタイプの設定
	setWindowType(true);	// システムである

	// メッセージ設定
	setMessage(&msg, 1, isAdd);
}

/*---------------------------------------------------------------------*//**
	パラメータセット
**//*---------------------------------------------------------------------*/
void TalkMatter::setParameter(VcStringArray* strarrParam)
{
	for(int istr = 0; istr < strarrParam->getCount(); istr++)
	{
		for(u32 idata = 0; idata < _listData->getCount(); idata++)
		{
			Data* data = _listData->getObject(idata);
			if(data == 0L)	{	continue;	}

			data->_msgCur = data->_msgOrg;
			VcString strWk(VcString::format("\\{\\$%d\\$\\}", istr + 1));
			data->_msgCur->replaceAll(&strWk, strarrParam->get(istr));
		}
	}

	// メッセージウインドウの文字列を更新する
	if(_wndMsgRef != 0L)
	{
		VcString msg(makeMessageString());
		_wndMsgRef->setWindowText(&msg);
	}
}

/*---------------------------------------------------------------------*//**
	ウインドウタイプを設定する
**//*---------------------------------------------------------------------*/
void TalkMatter::setWindowType(bool isSystem)
{
	if(_isSystem != isSystem)
	{
		_isSystem = isSystem;

		setupWindow();	// ウインドウを設定する
	}
}

/*---------------------------------------------------------------------*//**
	次のページへ送る
**//*---------------------------------------------------------------------*/
void TalkMatter::nextPage()
{
	if(_wndMsgRef != 0L)	{	_wndMsgRef->nextPage((s32)this->getEvId());	}
}

/*---------------------------------------------------------------------*//**
	終了させる
**//*---------------------------------------------------------------------*/
void TalkMatter::terminate()
{
	if(_wndMsgRef != 0L)	{	_wndMsgRef->setDone((s32)this->getEvId());	}
}

/*---------------------------------------------------------------------*//**
	状態を余韻に
**//*---------------------------------------------------------------------*/
void TalkMatter::lingering()
{
	_state = STATE_LINGERING;
}

/*---------------------------------------------------------------------*//**
	動作設定
**//*---------------------------------------------------------------------*/
void TalkMatter::setting(const Setting* setting, const MsgDataSet* msgdatset)
{
	_setting.copy(setting);

	if(_setting._msgid != 0)
	{
		setMessage(msgdatset, _setting._msgid, false);
	}
	else if(_setting._fsstrid != 0)
	{
		setFixedStrMessage(_setting._fsstrid, false);
	}
	else
	{
		ASSERTM(false, "No Message ID");
	}
}

/*---------------------------------------------------------------------*//**
	設定を XML から解析する
**//*---------------------------------------------------------------------*/
bool TalkMatter::analyzeSettingFromXml(Setting* setting, const XmlNode* node, const EvMatterCreateParam* cparam)
{
	s32 iwk = 0;

	// ［オプション（いずれか必須）］メッセージ ID
	if(XmlParserUtils::analyzeAtrb(&iwk, node, "msg_id"))
	{
		setting->_msgid = (u16)iwk;

		// メッセージ ID オフセットを加算
		if(cparam != 0L)
		{
			setting->_msgid += ((EventMatterCreateParam*)cparam)->_ofsMsgid;
		}
	}

	// ［オプション（いずれか必須）］［オプション（いずれか必須）］固定システム文字列 ID
	if(XmlParserUtils::analyzeAtrb(&iwk, node, "fixed_str_id"))
	{
		setting->_fsstrid = iwk;
	}

	// 必須判定
	if((setting->_msgid == 0) && (setting->_fsstrid == 0))
	{
		return false;
	}

	// ［オプション］種別
	const VcString* atrbv = node->findAttribute("mode");
	if(atrbv != 0L)	// オプション
	{
		if(atrbv->equalsIgnoreCase("TALK"))			{	setting->_kind = KIND_TALK;		}
		else if(atrbv->equalsIgnoreCase("SAY"))		{	setting->_kind = KIND_SAY;		}
		else if(atrbv->equalsIgnoreCase("YESNO"))	{	setting->_kind = KIND_YESNO;	}
	}

	return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
TalkMatter::TalkMatter()
	: EventMatter(EMTYPE_TALK)
	, _listData(0L)
	, _wndMsgRef(0L)
	, _isSystem(false)
	, _state(STATE_NULL)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
TalkMatter::~TalkMatter()
{
	ASSERT(_listData == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool TalkMatter::create(EvData* evdat, u16 evid)
{
	// データリストの作成
	_listData = new List<Data*>(true);

	// デフォルトのモード設定
	if(_setting._kind == KIND_NULL)	{	_setting._kind = KIND_TALK;	}

	// 状態を準備中に
	_state = STATE_PREPARE;

	return EventMatter::create(evdat, evid);
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void TalkMatter::destroy()
{
	// データリストの削除
	delete _listData;
	_listData = 0L;
}

/*---------------------------------------------------------------------*//**
	開始
**//*---------------------------------------------------------------------*/
void TalkMatter::begin(u32 param)
{
	EventMatter::begin(param);

	// ウインドウの設定
	setupWindow();
}

/*---------------------------------------------------------------------*//**
	終了
**//*---------------------------------------------------------------------*/
void TalkMatter::end()
{
	// ウインドウを消す
	if((_wndMsgRef != 0L) && _wndMsgRef->isDone())
	{
		_wndMsgRef->showWindow(false);
	}

	// 終了状態に
	_state = STATE_DONE;

	EventMatter::end();
}

/*---------------------------------------------------------------------*//**
	XML 要素から読み込む
**//*---------------------------------------------------------------------*/
bool TalkMatter::setupFromXmlNode(const XmlNode* node, const EvMatterCreateParam* cparam)
{
	if(!EventMatter::setupFromXmlNode(node, cparam))	{	return false;	}

	// - - - - - - - - - - - - - - - - - - - - - - - - -
	// オプション解析

	analyzeSettingFromXml(&_setting, node, cparam);

	// - - - - - - - - - - - - - - - - - - - - - - - - -
	// Sentence - 文定義解析
	{
		VcString data;
		VcString wk;
		u16 charid = 0;
		while(node != 0L)
		{
			node = XmlParserUtils::analyzeChildData(&wk, node, "Sentence");
			if(node != 0L)
			{
				data += wk + "\n";
				node = node->getSiblingNode();

				s32 iwk;
				if(XmlParserUtils::analyzeAtrb(&iwk, node, "char_id"))
				{
					charid = (u16)iwk;
				}
			}
		}
		if(data.getLength() > 0)
		{
			setMessage(&data, charid, true);
		}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	タグ表現された文字列を実表現に置換する
**//*---------------------------------------------------------------------*/
void TalkMatter::replaceTagString(VcString* dst, const VcString* src)
{
	VcString res = *src;

	/*
	Game game = Global.GetGame();
	MainCharEsp mainchar = (MainCharEsp)game.GetEspirito().GetEsp(EspType.MAINCHAR);
	Status stat = mainchar.GetStatus();
	
	res.replaceAll("\\{PlayerName\\}", stat.GetName());
	*/

	*dst = res;
}

/*---------------------------------------------------------------------*//**
	メッセージ文字列を得る
**//*---------------------------------------------------------------------*/
VcString TalkMatter::makeMessageString()
{
	VcString msg = "";
	for(u32 i = 0; i < _listData->getCount(); i++)
	{
		Data* data = _listData->getObject(i);
		if(data == 0L)	{	continue;	}
		if(i > 0)	{	msg += "\n";	}
		msg += *data->_msgCur;
	}
	return msg;
}

/*---------------------------------------------------------------------*//**
	毎フレーム処理実装
**//*---------------------------------------------------------------------*/
void TalkMatter::execImplement(ExecRes* res, const ExecCtx* ec, f32 frameBegun)
{
	if(_wndMsgRef->isDone())
	{
		end();	// 終了
	}

	// 読み上げ終了検出
	if((_state == STATE_READING) && !_wndMsgRef->isReadingAnimation())	// 読み上げアニメーションが終了している
	{
		_state = STATE_DONE_READING;
	}
}

/*---------------------------------------------------------------------*//**
	ウインドウのセットアップ
**//*---------------------------------------------------------------------*/
void TalkMatter::setupWindow()
{
	_wndMsgRef = 0L;

	if(_isSystem)
	{
		// メッセージウインドウの設定
		_wndMsgRef = Game::getGame()->getGui()->getSysMsgWindow();
		ASSERT(_wndMsgRef != 0L);
	}
	else
	{
		// 台詞ウインドウの設定
		_wndMsgRef = Game::getGame()->getGui()->getSubtitlesWindow();
		ASSERT(_wndMsgRef != 0L);
	}

	// モード設定
	switch(_setting._kind)
	{
	case KIND_TALK:
		_wndMsgRef->setKind(MessageWindow::KIND_TALK);
		_wndMsgRef->setEnableNextCursor(true);
		break;
	case KIND_SAY:
		_wndMsgRef->setKind(MessageWindow::KIND_SAY);
		_wndMsgRef->setEnableNextCursor(false);
		break;
	case KIND_YESNO:
		_wndMsgRef->setKind(MessageWindow::KIND_YESNO);	
		break;
	}

	// メッセージ設定
	VcString msg(makeMessageString());
	_wndMsgRef->setReadingAnimation(true);
	_wndMsgRef->showMessage(&msg, (s32)this->getEvId());
	_wndMsgRef->setGeneralParameter((u32)this->getEvId());
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
