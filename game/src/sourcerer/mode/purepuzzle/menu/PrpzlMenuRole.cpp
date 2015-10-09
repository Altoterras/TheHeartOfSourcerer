/***********************************************************************//**
 *	PrpzlMenuRole.cpp
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/11/10.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "PrpzlMenuRole.h"

// Friends
#include "PrpzlMenuMode.h"
#include "../../../file/GameFileIdDef.h"
#include "../../../body/Game.h"
#include "../../../body/GameExecCtx.h"
#include "../../../body/GameFixedStrTbl.h"
#include "../../../body/GameTaskModDef.h"
#include "../../../gui/GameGui.h"
#include "../../../gui/window/implement/GameSysMsgWindow.h"
#include "../../../gui/menu/GameMenuCreateParam.h"
#include "../../../gui/menu/GameMenuShowParam.h"
#include "../../../gui/menu/GameMenuPanelFactory.h"
#include "../../../mode/GameMode.h"
#include "../../../purepuzzle/PrpzlOperationData.h"
#include "../../../sound/GameSoundDef.h"

// External
#include "../../../../etk/effect/ScreenEffect.h"
#include "../../../../etk/sound/SoundMngr.h" 
#include "../../../../tfw/Tfw.h"
#include "../../../../tfw/framemod/FedTaskSys.h"
#include "../../../../tfw/gcmn/Renderer.h"
#include "../../../../tfw/gcmn/RendererUtils.h"
#include "../../../../tfw/gcmn/Texture.h"
#include "../../../../tfw/gui/Panel.h"
#include "../../../../tfw/lib/Vector.h"
#include "../../../../tfw/lib/Color.h"
#include "../../../../tfw/modemng/Role.h"
#include "../../../../tfw/smenu/Menu.h"
#include "../../../../tfw/smenu/MenuTreeNode.h"
#include "../../../../tfw/string/StringUtils.h"
#include "../../../../tfw/ui/TouchPanel.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

// GUI タスクシステムの処理順、兼 ID
#define GTSORDID_MENU		(GameTaskModDef::MOD__2D_ + 1)

// メニュー項目 ID
const s32 MTNODEID_VS_CPU			= 1001;
const s32 MTNODEID_VS_2P			= 2001;

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// PrpzlMenuRole メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
PrpzlMenuRole::PrpzlMenuRole(PrpzlOperationData* opdatRef)
	: _opdatRef(opdatRef)
	, _texBg(0L)
	, _menu(0L)
	, _msprm(0L)
	, _isRecreateMenuReserved(false)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
PrpzlMenuRole::~PrpzlMenuRole()
{
	ASSERT(_texBg == 0L);
	ASSERT(_menu == 0L);
	ASSERT(_msprm == 0L);
}

/*---------------------------------------------------------------------*//**
	開始通知
**//*---------------------------------------------------------------------*/
void PrpzlMenuRole::notifyBegin()
{
	// テクスチャの読み込み
	Texture::Format fmtTex = Texture::FMT_PNG;
	u32 fileidTex = 0;
	fileidTex = SYSTEM_TITLE_MAIN_V01_PNG;
	Texture* texBg = new Texture();
	if(texBg->create(fileidTex, fmtTex))
	{
		_texBg = texBg;
	}
	else
	{
		delete texBg;
		ASSERT(false);
	}

	// メニューの作成
	createMenu();

	// 値の初期化
	_isRecreateMenuReserved = false;
}

/*---------------------------------------------------------------------*//**
	終了通知
**//*---------------------------------------------------------------------*/
void PrpzlMenuRole::notifyEnd()
{
	// メニューの削除
	destroyMenu();

	// テクスチャの削除
	if(_texBg != 0L)
	{
		_texBg->destroy();
		delete _texBg;
		_texBg = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	メニュー作成
**//*---------------------------------------------------------------------*/
bool PrpzlMenuRole::createMenu()
{
	s32 fileidMenuXml = 0;
	Menu* menu = 0L;
	MenuFuncTable* ftmv = 0L;
	MenuPanelFactory* pf = 0L;
	GameMenuCreateParam* mcprm = 0L;
	GameMenuShowParam* msprm = 0L;

	// 機能テーブル
	ftmv = new MenuFuncTable_PrpzlMainMenu();
	if(ftmv == 0L)									{	goto FAILED;	}

	// メニューパネルファクトリ
	pf = new GameMenuPanelFactory();
	if(pf == 0L)									{	goto FAILED;	}

	// 作成パラメータ
	mcprm = new GameMenuCreateParam();
	if(mcprm == 0L)									{	goto FAILED;	}
	mcprm->_case = GameMenuCreateParam::CASE_TITLE;
	mcprm->_owner = this;

	// 表示パラメータ
	msprm = new GameMenuShowParam();
	if(msprm == 0L)									{	goto FAILED;	}
	msprm->_xBase = 100;
	msprm->_yBase = 140;
	msprm->_wMax = Game::getGame()->getLogicalWidth();
	msprm->_hMax = Game::getGame()->getLogicalHeight() - msprm->_yBase;
	msprm->_ownerRef = this;

	// メニュー作成
	menu = new Menu();
	fileidMenuXml = FILEID_CONV_LANG(MENU_MENU_PRPZLMAIN_EN_XML);
	if(menu == 0L)									{	goto FAILED;	}
	if(!menu->create(fileidMenuXml, ftmv, pf, mcprm))
	{
		goto FAILED;
	}
	_menu = menu;
	_msprm = msprm;

	// GUI タスクにメニューとヘッダーパネルを登録
	{
		FedTaskSys* tasksys = Game::getGame()->getTaskSys();	ASSERT(tasksys != 0L);
		if(!tasksys->registerExecModule(_menu, GameTaskModDef::calcExecOrder(GTSORDID_MENU), GTSORDID_MENU, 0))			{	ASSERT(false);	}
		if(!tasksys->registerDrawModule(_menu, GameTaskModDef::calcDrawOrder(GTSORDID_MENU), false, GTSORDID_MENU, 0))	{	ASSERT(false);	}
	}

	// メニュー項目の変更
	{
		#if defined(_ENLIGHT_PRPZL01_LITE)
		{
			// 2P 対戦無効化
			MenuTreeNode* mtnode = _menu->findMenuTreeNode(MTNODEID_VS_2P); ASSERT(mtnode != 0L);
			mtnode->setDisabled(true);
		}
		#endif
	}

	return true;

FAILED:

	ASSERT(false);
	if(menu != 0L)			{	delete menu;			}
	if(msprm != 0L)			{	delete msprm;			}
	if(mcprm != 0L)			{	delete mcprm;			}
	if(pf != 0L)			{	delete pf;				}
	if(ftmv != 0L)			{	delete ftmv;			}

	return false;
}

/*---------------------------------------------------------------------*//**
	メニュー破棄
**//*---------------------------------------------------------------------*/
void PrpzlMenuRole::destroyMenu()
{
	if(_menu == 0L)	{	return;	}

	// タスクからメニューとヘッダーパネルを削除
	FedTaskSys* tasksys = Game::getGame()->getTaskSys();
	if(tasksys != 0L)	// Game::cleanup 時などは既にタスクシステムが破棄されていることがある
	{
		if(!tasksys->removeModule(GTSORDID_MENU))	{	ASSERT(false);	}
	}

	// メニュー削除
	MenuFuncTable* ftmv = _menu->getFuncTable();
	MenuPanelFactory* pf = _menu->getPanelFactory();
	GameMenuCreateParam* mcprm = (GameMenuCreateParam*)_menu->getCreateParam();
	_menu->destroy();
	delete _menu;
	delete mcprm;
	delete pf;
	delete ftmv;
	_menu = 0L;
	delete _msprm;
	_msprm = 0L;
}

/*---------------------------------------------------------------------*//**
	コマンドフレーム処理
**//*---------------------------------------------------------------------*/
int PrpzlMenuRole::execCommand(ExecRes* res, const ExecCtx* ec, const CmdParam* param)
{
	///STRACE("paste board valid: %d, cmd: %d", isTransferSavesPasterboardValid(), param->_cid);
	
	// メニュー再作成要求
	if(_isRecreateMenuReserved)
	{
		destroyMenu();
		createMenu();
		_menu->showMenu(_msprm);	// メニューを開く
		_isRecreateMenuReserved = false;
	}

	// コマンド処理
	switch(param->_cid)
	{
	case STABLE:		return execCommand_stable(res, ec, param);
	}
	return -1;
}

/*---------------------------------------------------------------------*//**
	タイトル画面演出
**//*---------------------------------------------------------------------*/
int PrpzlMenuRole::execCommand_stable(ExecRes* res, const ExecCtx* ec, const CmdParam* param)
{
	const int	STEP_IN				= 1;
	const int	STEP_RUN			= 2;
	const int	STEP_OUT			= 3;
	const int	STEP_END			= 999;
	
	int step = param->_step;

	GameExecCtx* gec = (GameExecCtx*)ec;
	ScreenEffect* screff = Game::getGame()->getSceneScreenEffect();
	//TouchPanel* ui = gec->getTouchPanel();
	//GameSysMsgWindow* wndSys = (GameSysMsgWindow*)Game::getGame()->getGui()->getSysMsgWindow();
	
	switch(step)
	{
	case STEP_IN:
		if(param->_ecntStep == 1)
		{
			// フェードイン
			ColorU8 colBlack(0, 0, 0, 255);
			screff->setColor1(&colBlack);
			screff->setMaxFrame(60);
			screff->start(ScreenEffect::MODE_FADE_IN);

			// 曲の再生
			Game::getGame()->getSoundManager()->playBgm(GameSoundDef::BGM_103_VILLAGE, true);

			// メニューを開く
			_menu->showMenu(_msprm);
		}
		if(!screff->isWorking())
		{
			step = STEP_RUN;
		}
		break;

	case STEP_RUN:
		if(_menu->isDone() && !_isRecreateMenuReserved)
		{
			step = STEP_OUT;
		}
		break;
		
	case STEP_OUT:
		if(param->_ecntStep == 1)
		{
			// 曲の停止
			Game::getGame()->getSoundManager()->stopBgm();

			// 決定音
			Game::getGame()->getSoundManager()->playUiSe(GameSoundDef::SE_OK, false);

			// フェードアウト
			ColorU8 colBlack(0, 0, 0, 255);
			screff->setColor1(&colBlack);
			screff->setMaxFrame(60);
			screff->start(ScreenEffect::MODE_FADE_OUT);
		}
		if(!screff->isWorking())
		{
			screff->end(0);

			// 次のモードへ
			Game::getGame()->getGameMode()->changeEndMode(GameMode::MODE_GAME_PRPZL_PLAY);

			// 終了
			res->setDone();
		}
		break;

			
	case STEP_END:
		break;
	}

	return step;
}

/*---------------------------------------------------------------------*//**
	コマンド描画
**//*---------------------------------------------------------------------*/
void PrpzlMenuRole::drawCommand(const RenderCtx* rc, const CmdParam* param)
{
	if(param->_ecntBegun == 0)	{	return;	}	// 0 フレーム目は描画しない

	Renderer* rdr = rc->getRenderer();

	// 背景を描画する
	switch(param->_cid)
	{
	case STABLE:
		if(_texBg != 0L)		{	rdr->bindTexture(_texBg);	}	// テクスチャをバインド
		RendererUtils::draw2dTextureRect(rdr, 0, 0, 480, 320, 0.0f, 0.0f, 960.0f / 1024.0f, 640.0f / 1024.0f);
		break;
	}
}

/*---------------------------------------------------------------------*//**
	メニューノードクリック通知
**//*---------------------------------------------------------------------*/
bool PrpzlMenuRole::onClickNode(const ExecCtx* ec, MenuTreeNode* mtnodeCall, MenuTreeNode* mtnodeParent, void* objCreateParam, void* objShowParam)
{
	const VcString* cmd = mtnodeCall->getName();
	if( cmd->equals("VsCpu") )
	{
		_opdatRef->_conf._pzlmode = PrpzlOperationData::PZLMODE_VS_CPU;
		_opdatRef->_conf._vsface = PrpzlOperationData::VSFACE_SIDE;
	}
	else if( cmd->equals("Vs2p_SideBySide") )
	{
		_opdatRef->_conf._pzlmode = PrpzlOperationData::PZLMODE_VS_2P;
		_opdatRef->_conf._vsface = PrpzlOperationData::VSFACE_SIDE;
	}
	else if( cmd->equals("Vs2p_FaceToFace") )
	{
		_opdatRef->_conf._pzlmode = PrpzlOperationData::PZLMODE_VS_2P;
		_opdatRef->_conf._vsface = PrpzlOperationData::VSFACE_FACE;
	}
	else if( cmd->equals("Inf1p") )
	{
		_opdatRef->_conf._pzlmode = PrpzlOperationData::PZLMODE_INF_1P;
	}
	else
	{
		VcStringArray saCmd = StringUtils::split(cmd, "_");
		if((saCmd.getCount() == 2) && saCmd.get(0)->equals("Quest"))
		{
			s32 qno = StringUtils::toInteger(saCmd.get(1));
			_opdatRef->_conf._pzlmode = PrpzlOperationData::PZLMODE_QUEST;
			_opdatRef->_conf._questNo = qno;
		}
	}

	return true;
}

//==========================================================================
// SourceListWindow::MenuFuncTable_PrpzlMainMenu メソッド

/*---------------------------------------------------------------------*//**
	表示通知
**//*---------------------------------------------------------------------*/
void PrpzlMenuRole::MenuFuncTable_PrpzlMainMenu::onShow(Menu* menu, MenuTreeNode* mtnodeShow, void* objCreateParam, void* objShowParam)
{
}

/*---------------------------------------------------------------------*//**
	非表示通知
**//*---------------------------------------------------------------------*/
void PrpzlMenuRole::MenuFuncTable_PrpzlMainMenu::onHide(Menu* menu, void* objCreateParam, void* objShowParam)
{
}

/*---------------------------------------------------------------------*//**
	呼び出し通知
**//*---------------------------------------------------------------------*/
bool PrpzlMenuRole::MenuFuncTable_PrpzlMainMenu::onCall(Menu* menu, const ExecCtx* ec, MenuTreeNode* mtnodeCall, MenuTreeNode* mtnodeParent, void* objCreateParam, void* objShowParam)
{
	GameMenuShowParam* sprm = (GameMenuShowParam*)objShowParam;	ASSERT(sprm != 0L);
	PrpzlMenuRole* own = (PrpzlMenuRole*)sprm->_ownerRef;	ASSERT(own != 0L);
	return own->onClickNode(ec, mtnodeCall, mtnodeParent, objCreateParam, objShowParam);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
