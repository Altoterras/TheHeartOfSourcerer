/***********************************************************************//**
 *	TalkMatter.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/09/30.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_EVENT_MATTER_IMPLEMENT_TALK_MATTER_H_
#define _SRCR_EVENT_MATTER_IMPLEMENT_TALK_MATTER_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../EventMatter.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

template<class TYPE> class List;
class MsgDataSet;
class VcString;
class VcStringArray;
class XmlNode;

TFW_END_NS

ETK_BEGIN_NS

class MessageWindow;

ETK_END_NS

SRCR_BEGIN_NS

/*---------------------------------------------------------------------*//**
 *	会話表示マター
 *	
**//*---------------------------------------------------------------------*/
class TalkMatter : public EventMatter
{
	//======================================================================
	// 定数
public:
	// 種別
	enum Kind
	{
		KIND_NULL,
		KIND_TALK,		// ユーザーの応答を受け付けるモード
		KIND_SAY,		// イベント制御などでユーザーの応答を受け付けないモード
		KIND_YESNO,		// YES or NO の応答を受け付けるモード
	};

	// 状態
	enum State
	{
		STATE_NULL,
		STATE_PREPARE,		// 準備中
		STATE_READING,		// 読み上げ中
		STATE_DONE_READING,	// 読み上げ終了
		STATE_LINGERING,	// 余韻
		STATE_DONE,			// 全体終了
	};

	//======================================================================
	// クラス
public:
	class Setting
	{
	public:
		Kind			_kind;			// ［必須］種類
		u16				_msgid;			// ［オプション（いずれか必須）］メッセージ ID （優先）
		s32				_fsstrid;		// ［オプション（いずれか必須）］固定システム文字列 ID
		
		Setting() : _kind(KIND_NULL), _msgid(0), _fsstrid(0) {}
		void copy(const Setting* s)
		{
			_kind = s->_kind;
			_msgid = s->_msgid;
			_fsstrid = s->_fsstrid;
		}
	};

	class Data
	{
	public:
		VcString* _msgOrg;
		VcString* _msgCur;
		u16 _charid;

		Data(const VcString* msg, u16 charid);

		~Data();
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// データ数取得
	int getDataNum();
	// データ取得
	Data* getData(int index);
	// 種別取得
	inline Kind getKind() const { return _setting._kind; }
	// 状態取得
	inline State getState() const { return _state; }
	// メッセージウインドウ取得
	inline MessageWindow* messageWindow() const { return _wndMsgRef; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	void setMessage(const VcString* msg, u16 charid, bool isAdd);
	void setMessage(const MsgDataSet* msgdatset, u16 msgid, bool isAdd);
	void setFixedStrMessage(s32 fsstrid, bool isAdd);
	void setParameter(VcStringArray* strarrParam);
	void setKind(Kind kind) { _setting._kind = kind; }
	void setWindowType(bool isSystem);
	void nextPage();
	void terminate();
	void lingering();
	void setting(const Setting* setting, const MsgDataSet* msgdatset);
	static bool	analyzeSettingFromXml(Setting* setting, const XmlNode* node, const EvMatterCreateParam* cparam);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	TalkMatter();
	~TalkMatter();
	bool create(EvData* evdat, u16 evid);
	bool setupFromXmlNode(const XmlNode* node, const EvMatterCreateParam* cparam);
	void destroy();
	void begin(u32 param);
	void end();
	void execImplement(ExecRes* res, const ExecCtx* ec, f32 frameBegun);
	void replaceTagString(VcString* dst, const VcString* src);
	VcString makeMessageString();
	void setupWindow();

	//======================================================================
	// 変数
private:
	List<Data*>* _listData;
	MessageWindow* _wndMsgRef;
	bool _isSystem;
	State _state;
	Setting	_setting;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_EVENT_MATTER_IMPLEMENT_TALK_MATTER_H_
