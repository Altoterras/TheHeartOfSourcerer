/***********************************************************************//**
 *	QuestListWindow.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2010/11/15.
 *	Copyright 2010 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

#ifndef _SRCR_GUI_WINDOW_IMPLEMENT_QUEST_LIST_WINDOW_H_
#define _SRCR_GUI_WINDOW_IMPLEMENT_QUEST_LIST_WINDOW_H_

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

#include "../ListWindowBase.h"

////////////////////////////////////////////////////////////////////////////
// クラス

TFW_BEGIN_NS

class Panel;
class StringDrawer;
class TiledRectDraw;

TFW_END_NS

SRCR_BEGIN_NS

class QuestArticle;
class QuestManager;

/*---------------------------------------------------------------------*//**
 *	クエスト リスト ウインドウ
 *
**//*---------------------------------------------------------------------*/
class QuestListWindow : public ListWindowBase
{
	//======================================================================
	// 定数
public:
	// 使用場面
	enum QlwCase
	{
		QLWCASE_NULL,
		QLWCASE_MENU,	// メニューモード
	};

private:
	// テキスト描画パーツ
	enum TdParts
	{
		TDP_NAME,
		TDP_REQS,
		TDP_RWRD,
		TDP_CLNT,
		NUM_TDP
	};

	// タイル描画パーツ
	enum TrdParts
	{
		TRDP_PROP,
		NUM_TRDP
	};

	//======================================================================
	// メソッド

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// アクセサ
public:
	// 選択されたクエストを得る
	QuestArticle* getSelectedQuest() const;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 外部サービス
public:
	// 使用場面設定
	void setCase(QlwCase qlwcase) {	_qlwcase = qlwcase;	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// 内部制御
public:
	QuestListWindow();
	~QuestListWindow();
	bool create(Font* fontRef, Texture* texRef);
	void destroy();
	void execContents(ExecRes* res, const ExecCtx* ec);
	void draw(const RenderCtx* rc);
	void notifyChangeLanguage();

	bool onClickMenuNode(const ExecCtx* ec, MenuTreeNode* mtnodeCall, MenuTreeNode* mtnodeParent, void* objCreateParam, void* objShowParam);
	bool onClickSubMenuNode(const ExecCtx* ec, MenuTreeNode* mtnodeCall, MenuTreeNode* mtnodeParent, void* objCreateParam, void* objShowParam);

protected:
	void onShowWindow(bool isShow);

private:
	s32 makeList();

	//======================================================================
	// メンバ変数
private:
	QuestManager* _qstmngRef;
	QlwCase _qlwcase; 
	StringDrawer* _strdraw[NUM_TDP];
	TiledRectDraw* _trdraw[NUM_TRDP];
	Panel* _pnlStat;
	s32 _idxFocusedRem;
};

SRCR_END_NS

////////////////////////////////////////////////////////////////////////////

#endif	// _SRCR_GUI_WINDOW_IMPLEMENT_QUEST_LIST_WINDOW_H_
