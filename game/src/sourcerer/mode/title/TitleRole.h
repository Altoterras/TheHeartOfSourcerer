/***********************************************************************//**
 *	TitleRole.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/10.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_MODE_TITLE_TITLE_ROLE_H_
#define _SRCR_MODE_TITLE_TITLE_ROLE_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../../Base.h"

#include "../../../tfw/modemng/Role.h"
#include "../../../tfw/smenu/MenuFuncTable.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class Panel;
class Texture;
class VcString;
namespace smenu
{
	class Menu;
}

TFW_END_NS
using namespace smenu;

SRCR_BEGIN_NS

class Game;
class GameMenuShowParam;

/*---------------------------------------------------------------------*//**
 *	タイトル ロール
 *
**//*---------------------------------------------------------------------*/
class TitleRole : public Role
{
	//======================================================================
	// 定数
public:
	enum CommandType
	{
		LOGO = 1,
		PASTE_SAVES,
		STABLE,
	};

private:
	enum EnterGame
	{
		EG_NULL,
		EG_NEWGAME,
		EG_RESTARTGAME_1,
		EG_RESTARTGAME_2,
		EG_RESUME,
		EG_LOAD_1,
		EG_LOAD_2
	};

	enum Pnl
	{
		PNL_HP,
#if ENABLE_PSNS
		PNL_PSNS_SCORE,
		PNL_PSNS_TROPHY,
#endif
		NUM_PNL
	};

	//======================================================================
	// クラス
private:
	/*-----------------------------------------------------------------*//**
	 *	メニュー機能テーブル
	 *	
	**//*-----------------------------------------------------------------*/
	class MenuFuncTable_Title : public smenu::MenuFuncTable
	{
		//==================================================================
		// メソッド
	public:
		void onShow(Menu* menu, MenuTreeNode* mtnodeShow, void* objCreateParam, void* objShowParam);
		void onHide(Menu* menu, void* objCreateParam, void* objShowParam);
		bool onCall(Menu* menu, const ExecCtx* ec, MenuTreeNode* mtnodeCall, MenuTreeNode* mtnodeParent, void* objCreateParam, void* objShowParam);
	};

	//======================================================================
	// メソッド
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	// タイトルテクスチャを得る
	inline Texture* getTitleTexture() const { return _texTitle; }

	// ペーストボードが有効か調べる
	static bool isTransferSavesPasterboardValid();

	// 再開かどうかを設定する
	void setResumeStart() { _isResumeStart = true; }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	TitleRole();
	~TitleRole();
	bool onClickNode(const ExecCtx* ec, MenuTreeNode* mtnodeCall, MenuTreeNode* mtnodeParent, void* objCreateParam, void* objShowParam);

protected:
	void notifyBegin();
	void notifyEnd();
	int execCommand(ExecRes* res, const ExecCtx* ec, const CmdParam* param);
	void drawCommand(const RenderCtx* rc, const CmdParam* param);

private:
	bool createMenu();
	void destroyMenu();
	void updateMenu();
	int execCommand_logo(ExecRes* res, const ExecCtx* ec, const CmdParam* param);
	int execCommand_pasteSaves(ExecRes* res, const ExecCtx* ec, const CmdParam* param);
	int execCommand_stable(ExecRes* res, const ExecCtx* ec, const CmdParam* param);

	//======================================================================
	// 変数
private:
	Texture*				_texCorpLogo;
	Texture*				_texTitle;	
	Menu*					_menu;
	GameMenuShowParam*		_msprm;
	Panel*					_pnl[NUM_PNL];
	bool					_isSkipLogo;
	bool					_isRecreateMenuReserved;
	bool					_isResumeStart;
	EnterGame				_entergame;
	VcString*				_strVersion;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_MODE_TITLE_TITLE_ROLE_H_
