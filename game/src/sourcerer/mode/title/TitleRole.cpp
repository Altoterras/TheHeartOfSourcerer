/***********************************************************************//**
 *	TitleRole.cpp
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
#include "TitleRole.h"
// Friends

// External
#include "../../episode/save/SaveStructure.h"
#include "../../file/GameFileIdDef.h"
#include "../../body/Game.h"
#include "../../body/GameExecCtx.h"
#include "../../body/GameFixedStrTbl.h"
#include "../../body/GameTaskModDef.h"
#include "../../body/GameRenderCtx.h"
#include "../../gui/GameFontSet.h"
#include "../../gui/GameGui.h"
#include "../../gui/window/implement/GameActMsgWindow.h"
#include "../../gui/window/implement/GameSysMsgWindow.h"
#include "../../gui/menu/GameMenuCreateParam.h"
#include "../../gui/menu/GameMenuShowParam.h"
#include "../../gui/menu/GameMenuPanelFactory.h"
#include "../../mode/GameMode.h"
#include "../../save/SaveSys.h"
#include "../../save/SysConf.h"
#include "../../sound/GameSoundDef.h"
#if defined(_IOS)
#include "../../../ios/OsDepModIos.h"
#endif

// Library
#include "../../../etk/effect/ScreenEffect.h"
#include "../../../etk/body/OsDepMod.h"
#include "../../../etk/sound/SoundMngr.h" 
#include "../../../tfw/Tfw.h"
#include "../../../tfw/framemod/FedTaskSys.h"
#include "../../../tfw/gcmn/EasyStringDrawer.h"
#include "../../../tfw/gcmn/Gcalc.h"
#include "../../../tfw/gcmn/Renderer.h"
#include "../../../tfw/gcmn/RendererUtils.h"
#include "../../../tfw/gcmn/Texture.h"
#include "../../../tfw/gui/Panel.h"
#include "../../../tfw/lib/Vector.h"
#include "../../../tfw/lib/Color.h"
#include "../../../tfw/modemng/Role.h"
#include "../../../tfw/psns/PsnsBase.h"
#include "../../../tfw/smenu/Menu.h"
#include "../../../tfw/smenu/MenuTreeNode.h"
#include "../../../tfw/ui/TouchPanel.h"

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

// GUI タスクシステムの処理順、兼 ID
#define GTSORDID_MENU		(GameTaskModDef::MOD__2D_ + 1)

// メニュー項目 ID
const s32 MTNODEID_RESUME				= 101;
const s32 MTNODEID_LOAD					= 201;
const s32 MTNODEID_LOAD_F1				= 211;
const s32 MTNODEID_LOAD_F2				= 212;
const s32 MTNODEID_RESTART_GAME_1		= 312;
const s32 MTNODEID_RESTART_GAME_2		= 313;
const s32 MTNODEID_TRANSFER_TO			= 203;
const s32 MTNODEID_LAND_EN				= 421;
const s32 MTNODEID_LAND_JP				= 422;
const s32 MTNODEID_SCRROT_M90			= 431;
const s32 MTNODEID_SCRROT_P90			= 432;
const s32 MTNODEID_SAYMSG_PAUSE			= 441;
const s32 MTNODEID_SAYMSG_NOWAIT		= 442;
const s32 MTNODEID_MULTITASKING_FAST	= 451;
const s32 MTNODEID_MULTITASKING_ECO		= 452;
const s32 MTNODEID_PRPZL_START			= 501;

#define W_TEX				(1024.0f)
#define H_TEX				(1024.0f)
#define MARGIN_H_BTN		(8)
#define MARGIN_V_BTN		(8)
// HP ボタン
#define RECT_HPBTN			Game::getGame()->getLogicalWidth() - 56, Game::getGame()->getLogicalHeight() - 80, 32, 32
///#define RECT_HPBTN			Game::getGame()->getLogicalWidth() - 56, Game::getGame()->getLogicalHeight() - 40, 32, 32	// iAd 対応につき↑へ
#define UV_HPBTN_NRM		  0.0f, 640.0f, 64.0f, 64.0f, W_TEX, H_TEX
#define UV_HPBTN_HVR		 64.0f, 640.0f, 64.0f, 64.0f, W_TEX, H_TEX
// PSNS ボタン
#define RECT_PSNS_SCORE		Game::getGame()->getLogicalWidth() - 56, Game::getGame()->getLogicalHeight() - 160, 32, 32
///#define RECT_PSNS_SCORE		Game::getGame()->getLogicalWidth() - 56, Game::getGame()->getLogicalHeight() - 120, 32, 32	// iAd 対応につき↑へ
#define UV_PSNS_SCORE_NRM	128.0f, 640.0f, 64.0f, 64.0f, W_TEX, H_TEX
#define UV_PSNS_SCORE_HVR	192.0f, 640.0f, 64.0f, 64.0f, W_TEX, H_TEX
#define RECT_PSNS_TROPHY	Game::getGame()->getLogicalWidth() - 56, Game::getGame()->getLogicalHeight() - 120, 32, 32
///#define RECT_PSNS_TROPHY	Game::getGame()->getLogicalWidth() - 56, Game::getGame()->getLogicalHeight() - 80, 32, 32	// iAd 対応につき↑へ
#define UV_PSNS_TROPHY_NRM	256.0f, 640.0f, 64.0f, 64.0f, W_TEX, H_TEX
#define UV_PSNS_TROPHY_HVR	320.0f, 640.0f, 64.0f, 64.0f, W_TEX, H_TEX
#define UV_CORP_LOGO		0.0f, 0.0f, 960.0f, 640.0f, W_TEX, H_TEX
#define UV_TITLE_BG			0.0f, 0.0f, 960.0f, 640.0f, W_TEX, H_TEX
#define W_TITLE_LOGO		(920.0f * 0.5f)
#define H_TITLE_LOGO		(290.0f * 0.5f)
#define UV_TITLE_LOGO		0.0f, 706.0f, 920.0f, 290.0f, W_TEX, H_TEX
#define W_COPYRIGHT			(574.0f * 0.5f)
#define H_COPYRIGHT			(30.0f * 0.5f)
#define UV_COPYRIGHT		386.0f, 672.0f, 574.0f, 30.0f, W_TEX, H_TEX

// セーブデータ移行
#define NUM_TRANS_SAVES_FILES		4
#define NAME_TRANS_SAVES_PB			"jp.co.altoterras.sourcerer01lite.saves"
#define NAME_TRANS_SAVES_FILE_1		"sourcerer01_save_resume%s.dat"
#define NAME_TRANS_SAVES_FILE_2		"sourcerer01_save_f01%s.dat"
#define NAME_TRANS_SAVES_FILE_3		"sourcerer01_save_f02%s.dat"
#define NAME_TRANS_SAVES_FILE_4		"sysconf.dat"
#define KEY_TRANS_SAVES_FILE_1		"jp.co.altoterras.sourcerer01lite.saves.resume"
#define KEY_TRANS_SAVES_FILE_2		"jp.co.altoterras.sourcerer01lite.saves.f01"
#define KEY_TRANS_SAVES_FILE_3		"jp.co.altoterras.sourcerer01lite.saves.f02"
#define KEY_TRANS_SAVES_FILE_4		"jp.co.altoterras.sourcerer01lite.saves.sysconf"
#define URLQUERY_TRANS_SAVES_BOOT	"?PasteSaves"

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// TitleRole メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	ペーストボードが有効か調べる
**//*---------------------------------------------------------------------*/
bool TitleRole::isTransferSavesPasterboardValid()
{
	#if defined(_IOS)
		// セーブデータをインポート
		VcString keyFiles[] =
		{
			VcString(KEY_TRANS_SAVES_FILE_1),
			VcString(KEY_TRANS_SAVES_FILE_2),
			VcString(KEY_TRANS_SAVES_FILE_3),
			VcString(KEY_TRANS_SAVES_FILE_4)
		};
		VcString nameTransSavesPb(NAME_TRANS_SAVES_PB);
		return OsDepModIos::isTransferSavesPasterboardValid(&nameTransSavesPb, keyFiles, NUM_TRANS_SAVES_FILES);
	#else
		return false;
	#endif
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
TitleRole::TitleRole()
	: _texTitle(0L)
	, _texCorpLogo(0L)
	, _menu(0L)
	, _msprm(0L)
	, _isSkipLogo(false)
	, _isRecreateMenuReserved(false)
	, _isResumeStart(false)
	, _entergame(EG_NULL)
	, _strVersion(0L)
{
	for(int i = 0; i < NUM_PNL; i++)		{	_pnl[i] = 0;			}
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
TitleRole::~TitleRole()
{
	ASSERT(_texTitle == 0L);
	ASSERT(_texCorpLogo == 0L);
	ASSERT(_menu == 0L);
	ASSERT(_msprm == 0L);
	ASSERT(_strVersion == 0L);
	#if defined(_DEBUG)
		for(int i = 0; i < NUM_PNL; i++)	{	ASSERT(_pnl[i] == 0L);	}
	#endif
}

/*---------------------------------------------------------------------*//**
	開始通知
**//*---------------------------------------------------------------------*/
void TitleRole::notifyBegin()
{
	FTRACE("=== {TitleRole::notifyBegin} 000.\n");
	RectF32 rect;
	RectF32 uv;

	FTRACE("=== {TitleRole::notifyBegin} 011.\n");
	// タイトルテクスチャの読み込み
	{
		u32 fileidTex = 0;
		#if defined(_ENLIGHT_V01_LITE)
			fileidTex = Env_isRdResolution() ? SYSTEM_TITLE_MAIN_V01LITE_RD_PNG : SYSTEM_TITLE_MAIN_V01LITE_PNG;
		#else
			fileidTex = Env_isRdResolution() ? SYSTEM_TITLE_MAIN_V01_RD_PNG : SYSTEM_TITLE_MAIN_V01_PNG;
		#endif
		Texture* texTitle = new Texture();
		if(texTitle->create(fileidTex, Texture::FMT_PNG))
		{
			_texTitle = texTitle;
		}
		else
		{
			delete texTitle;
			ASSERT(false);
		}
	}
	// 社ロゴテクスチャの読み込み
	if(getCurCommandId() == TitleRole::LOGO)
	{
		u32 fileidTex = (Env_isHdResolution() || Env_isRdResolution()) ? SYSTEM_TITLE_CORPLOGO_RD_PNG : SYSTEM_TITLE_CORPLOGO_PNG;
		Texture* texCorpLogo = new Texture();
		if(texCorpLogo->create(fileidTex, Texture::FMT_PNG))
		{
			_texCorpLogo = texCorpLogo;
		}
		else
		{
			delete texCorpLogo;
			ASSERT(false);
		}
	}

	FTRACE("=== {TitleRole::notifyBegin} 021.\n");
	// メニューの作成
	createMenu();

	FTRACE("=== {TitleRole::notifyBegin} 031.\n");
	// 子パネル作成
	_pnl[PNL_HP] = new Panel();
	_pnl[PNL_HP]->create(RectF32::setout(&rect, RECT_HPBTN));
	if(_texTitle != 0L)
	{
		_pnl[PNL_HP]->setStateInfo(Panel::STATE_NORMAL, _texTitle, Gcalc::calcTexUv(&uv, UV_HPBTN_NRM, 1.0f));
		_pnl[PNL_HP]->setStateInfo(Panel::STATE_HOVER, _texTitle, Gcalc::calcTexUv(&uv, UV_HPBTN_HVR, 1.0f));
	}
	_pnl[PNL_HP]->setHorizontalHitMargin(MARGIN_H_BTN);
	_pnl[PNL_HP]->setVerticalHitMargin(MARGIN_V_BTN);
#if ENABLE_PSNS
	_pnl[PNL_PSNS_SCORE] = new Panel();
	_pnl[PNL_PSNS_SCORE]->create(RectF32::setout(&rect, RECT_PSNS_SCORE));
	if(_texTitle != 0L)
	{
		_pnl[PNL_PSNS_SCORE]->setStateInfo(Panel::STATE_NORMAL, _texTitle, Gcalc::calcTexUv(&uv, UV_PSNS_SCORE_NRM, 1.0f));
		_pnl[PNL_PSNS_SCORE]->setStateInfo(Panel::STATE_HOVER, _texTitle, Gcalc::calcTexUv(&uv, UV_PSNS_SCORE_HVR, 1.0f));
	}
	_pnl[PNL_PSNS_SCORE]->setHorizontalHitMargin(MARGIN_H_BTN);
	_pnl[PNL_PSNS_SCORE]->setVerticalHitMargin(MARGIN_V_BTN);
	_pnl[PNL_PSNS_SCORE]->setEnable(false);
	_pnl[PNL_PSNS_TROPHY] = new Panel();
	_pnl[PNL_PSNS_TROPHY]->create(RectF32::setout(&rect, RECT_PSNS_TROPHY));
	if(_texTitle != 0L)
	{
		_pnl[PNL_PSNS_TROPHY]->setStateInfo(Panel::STATE_NORMAL, _texTitle, Gcalc::calcTexUv(&uv, UV_PSNS_TROPHY_NRM, 1.0f));
		_pnl[PNL_PSNS_TROPHY]->setStateInfo(Panel::STATE_HOVER, _texTitle, Gcalc::calcTexUv(&uv, UV_PSNS_TROPHY_HVR, 1.0f));
	}
	_pnl[PNL_PSNS_TROPHY]->setHorizontalHitMargin(MARGIN_H_BTN);
	_pnl[PNL_PSNS_TROPHY]->setVerticalHitMargin(MARGIN_V_BTN);
	_pnl[PNL_PSNS_TROPHY]->setEnable(false);
#endif	// #if ENABLE_PSNS
	// GUI のクリア（クリア後のタイトル戻りのときのための処理）
	GameActMsgWindow* wndActMsg = (GameActMsgWindow*)Game::getGame()->getGui()->getActionMsgWindow(); ASSERT(wndActMsg != 0L);
	if(wndActMsg != 0L)
	{
		wndActMsg->clearMessage();
	}

	// バージョン文字列取得
	_strVersion = new VcString();
	Game::getGame()->getOsDepMod()->getAppVersionString(_strVersion);
	//*_strVersion = VcString("ver.") + *_strVersion;

	// 値の初期化
	_isSkipLogo = false;
	_isRecreateMenuReserved = false;

	FTRACE("=== {TitleRole::notifyBegin} 999.\n");
}

/*---------------------------------------------------------------------*//**
	終了通知
**//*---------------------------------------------------------------------*/
void TitleRole::notifyEnd()
{
	// 子パネル削除
	for(int i = 0; i < NUM_PNL; i++)
	{
		if(_pnl[i] != 0)
		{
			_pnl[i]->destroy();
			delete _pnl[i];
			_pnl[i] = 0L;
		}
	}

	// メニューの削除
	destroyMenu();

	// テクスチャの削除
	if(_texCorpLogo != 0L)
	{
		_texCorpLogo->destroy();
		delete _texCorpLogo;
		_texCorpLogo = 0L;
	}
	if(_texTitle != 0L)
	{
		_texTitle->destroy();
		delete _texTitle;
		_texTitle = 0L;
	}

	// バージョン文字列削除
	delete _strVersion;
	_strVersion = 0L;
}

/*---------------------------------------------------------------------*//**
	メニュー作成
**//*---------------------------------------------------------------------*/
bool TitleRole::createMenu()
{
	s32 fileidMenuXml = 0;
	Menu* menu = 0L;
	MenuFuncTable* ftmv = 0L;
	MenuPanelFactory* pf = 0L;
	GameMenuCreateParam* mcprm = 0L;
	GameMenuShowParam* msprm = 0L;

	// 機能テーブル
	ftmv = new MenuFuncTable_Title();
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
	#if defined(_ENLIGHT_PRPZL01)
		fileidMenuXml = FILEID_CONV_LANG(MENU_MENU_TITLEPRPZL_EN_XML);
	#elif defined(_ENLIGHT_V01_LITE)
		fileidMenuXml = FILEID_CONV_LANG(MENU_MENU_TITLELITE_EN_XML);
	#else
		fileidMenuXml = FILEID_CONV_LANG(MENU_MENU_TITLE_EN_XML);
	#endif
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
	updateMenu();

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
void TitleRole::destroyMenu()
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
	メニュー更新
**//*---------------------------------------------------------------------*/
void TitleRole::updateMenu()
{
	FTRACE("== {TitleRole::updateMenu} 000.\n");
	SaveSys* svsys = Game::getGame()->getSaveSys(); ASSERT(svsys != 0L);
	SysConf* sysconf = svsys->getSysConf(); ASSERT(sysconf != 0L);

	#if !defined(_ENLIGHT_PRPZL01)
	{
		FTRACE("== {TitleRole::updateMenu} 011.\n");
		// 再開
		{
			MenuTreeNode* mtnodeResume = _menu->findMenuTreeNode(MTNODEID_RESUME); ASSERT(mtnodeResume != 0L);
			if((mtnodeResume != 0L) && (!svsys->checkExistsResumeSaveFile() != mtnodeResume->isDisabled()))
			{
				mtnodeResume->setDisabled(!svsys->checkExistsResumeSaveFile());
			}
		}

		FTRACE("== {TitleRole::updateMenu} 012.\n");
		// ロードセーブデータ
		bool isExistsSaveFile1 = svsys->checkExistsSaveFile(1);
		bool isExistsSaveFile2 = svsys->checkExistsSaveFile(2);
		if(!isExistsSaveFile1 && !isExistsSaveFile2)
		{
			MenuTreeNode* mtnodeLoad = _menu->findMenuTreeNode(MTNODEID_LOAD);	ASSERT(mtnodeLoad != 0L);
			mtnodeLoad->setDisabled(true);
			MenuTreeNode* mtnodeRestartSave1 = _menu->findMenuTreeNode(MTNODEID_RESTART_GAME_1); ASSERT(mtnodeRestartSave1 != 0L);
			mtnodeRestartSave1->setDisabled(true);
			MenuTreeNode* mtnodeRestartSave2 = _menu->findMenuTreeNode(MTNODEID_RESTART_GAME_2); ASSERT(mtnodeRestartSave2 != 0L);
			mtnodeRestartSave2->setDisabled(true);
		}
		else
		{
			MenuTreeNode* mtnodeLoad = _menu->findMenuTreeNode(MTNODEID_LOAD);	ASSERT(mtnodeLoad != 0L);
			mtnodeLoad->setDisabled(false);

			MenuTreeNode* mtnodeFile1 = _menu->findMenuTreeNode(MTNODEID_LOAD_F1); ASSERT(mtnodeFile1 != 0L);
			MenuTreeNode* mtnodeFile2 = _menu->findMenuTreeNode(MTNODEID_LOAD_F2); ASSERT(mtnodeFile2 != 0L);
			SaveStructure::SummarySaveStructure sumsvst;
			bool isPsnsUserData = false;
			if(isExistsSaveFile1)
			{
				// セーブファイル１を有効化
				mtnodeFile1->setDisabled(false);
				MenuTreeNode* mtnodeRestartSave1 = _menu->findMenuTreeNode(MTNODEID_RESTART_GAME_1); ASSERT(mtnodeRestartSave1 != 0L);
				mtnodeRestartSave1->setDisabled(false);

				// セーブファイル１名に時間を付加
				if(svsys->readSaveData(1, &sumsvst, sizeof(SaveStructure::SummarySaveStructure), &isPsnsUserData))
				{
					VcString name(VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_SAVE_FILE_1_AND_TIME)->getRaw(),
							sumsvst._framePlay / 30 / 60,
							(sumsvst._framePlay / 30) % 60,
							TFW_IS_FLAG(sumsvst._flags, SaveStructure::SummarySaveStructure::F_CLEARED) ? "*" : ""));
					mtnodeFile1->setName(&name);
					mtnodeFile1->setGeneralParam((u32)isPsnsUserData);
				}
			}
			else
			{
				// セーブファイル１を無効化
				mtnodeFile1->setDisabled(true);
				MenuTreeNode* mtnode = _menu->findMenuTreeNode(MTNODEID_RESTART_GAME_1); ASSERT(mtnode != 0L);
				mtnode->setDisabled(true);
			}

			if(isExistsSaveFile2)
			{
				// セーブファイル２を有効化
				mtnodeFile2->setDisabled(false);
				MenuTreeNode* mtnodeRestartSave2 = _menu->findMenuTreeNode(MTNODEID_RESTART_GAME_2); ASSERT(mtnodeRestartSave2 != 0L);
				mtnodeRestartSave2->setDisabled(false);

				// セーブファイル２名に時間を付加
				if(svsys->readSaveData(2, &sumsvst, sizeof(SaveStructure::SummarySaveStructure), &isPsnsUserData))
				{
					VcString name(VcString::format(GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_SAVE_FILE_2_AND_TIME)->getRaw(),
							sumsvst._framePlay / 30 / 60,
							(sumsvst._framePlay / 30) % 60,
							TFW_IS_FLAG(sumsvst._flags, SaveStructure::SummarySaveStructure::F_CLEARED) ? "*" : ""));
					mtnodeFile2->setName(&name);
					mtnodeFile2->setGeneralParam((u32)isPsnsUserData);
				}
			}
			else
			{
				// セーブファイル２を無効化
				mtnodeFile2->setDisabled(true);
				MenuTreeNode* mtnode = _menu->findMenuTreeNode(MTNODEID_RESTART_GAME_2); ASSERT(mtnode != 0L);
				mtnode->setDisabled(true);
			}
		}
		
		FTRACE("== {TitleRole::updateMenu} 013.\n");
		// セーブデータ移行
		#if defined(_ENLIGHT_V01_LITE)
			#if defined(_WINDOWS)
			#elif defined(_IOS)
				if(!OsDepModIos::isInstalledApp(PRODDEFID_V01))
				{
					MenuTreeNode* mtnode = _menu->findMenuTreeNode(MTNODEID_TRANSFER_TO); ASSERT(mtnode != 0L);
					if(mtnode != 0L)	{	mtnode->setDisabled(true);	}
				}
			#endif
		#endif
	}
	#endif

	FTRACE("== {TitleRole::updateMenu} 021.\n");
	// 言語
	{
		LangId lang = sysconf->getLanguage();
		MenuTreeNode* mtnodeEn = _menu->findMenuTreeNode(MTNODEID_LAND_EN);	ASSERT(mtnodeEn != 0L);
		if(mtnodeEn != 0L)	{	mtnodeEn->setChecked(lang == LANGID_EN);	}
		MenuTreeNode* mtnodeJp = _menu->findMenuTreeNode(MTNODEID_LAND_JP);	ASSERT(mtnodeJp != 0L);
		if(mtnodeJp != 0L)	{	mtnodeJp->setChecked(lang == LANGID_JP);	}
	}

	// 画面の方向
	#if 0
		{
			MenuTreeNode* mtnodeSrP90 = _menu->findMenuTreeNode(MTNODEID_SCRROT_P90); ASSERT(mtnodeSrP90 != 0L);
			if(mtnodeSrP90 != 0L)	{	mtnodeSrP90->setChecked(sysconf->isScreenOrientationPlus90());	}
			MenuTreeNode* mtnodeSrM90 = _menu->findMenuTreeNode(MTNODEID_SCRROT_M90); ASSERT(mtnodeSrM90 != 0L);
			if(mtnodeSrM90 != 0L)	{	mtnodeSrM90->setChecked(!sysconf->isScreenOrientationPlus90());	}
		}
	#endif

	#if 0
		// イベントメッセージ送り
		{
			MenuTreeNode* mtnodeSayPause = _menu->findMenuTreeNode(MTNODEID_SAYMSG_PAUSE); ASSERT(mtnodeSayPause != 0L);
			if(mtnodeSayPause != 0L)	{	mtnodeSayPause->setChecked(sysconf->isEnableWaitMovieMessage());	}
			MenuTreeNode* mtnodeSayNowait = _menu->findMenuTreeNode(MTNODEID_SAYMSG_NOWAIT); ASSERT(mtnodeSayNowait != 0L);
			if(mtnodeSayNowait != 0L)	{	mtnodeSayNowait->setChecked(!sysconf->isEnableWaitMovieMessage());	}
		}
	#endif

	FTRACE("== {TitleRole::updateMenu} 051.\n");
	// マルチタスク切り替え
	{
		bool isEcoMultitasking = sysconf->isEcoMultiTasking();
		MenuTreeNode* mtnodeFast = _menu->findMenuTreeNode(MTNODEID_MULTITASKING_FAST);	ASSERT(mtnodeFast != 0L);
		if(mtnodeFast != 0L)    {   mtnodeFast->setChecked(!isEcoMultitasking); }
		MenuTreeNode* mtnodeEco = _menu->findMenuTreeNode(MTNODEID_MULTITASKING_ECO);	ASSERT(mtnodeEco != 0L);
		if(mtnodeEco != 0L)     {   mtnodeEco->setChecked(isEcoMultitasking);   }
	}
	FTRACE("== {TitleRole::updateMenu} 999.\n");
}

/*---------------------------------------------------------------------*//**
	コマンドフレーム処理
**//*---------------------------------------------------------------------*/
int TitleRole::execCommand(ExecRes* res, const ExecCtx* ec, const CmdParam* param)
{
	///STRACE("paste board valid: %d, cmd: %d", isTransferSavesPasterboardValid(), param->_cid);
	///FTRACE("== {TitleRole::execCommand} 000.\n");
	
	// メニュー再作成要求
	if(_isRecreateMenuReserved)
	{
		destroyMenu();
		createMenu();
		_menu->showMenu(_msprm);	// メニューを開く
		_isRecreateMenuReserved = false;
	}

	// コマンド処理
	///FTRACE("== {TitleRole::execCommand} 021. %d\n", param->_cid);
	switch(param->_cid)
	{
	case LOGO:			return execCommand_logo(res, ec, param);
	case PASTE_SAVES:	return execCommand_pasteSaves(res, ec, param);
	case STABLE:		return execCommand_stable(res, ec, param);
	}

	///FTRACE("== {TitleRole::execCommand} 999.\n");
	return -1;
}

/*---------------------------------------------------------------------*//**
	会社ロゴ演出
**//*---------------------------------------------------------------------*/
int TitleRole::execCommand_logo(ExecRes* res, const ExecCtx* ec, const CmdParam* param)
{
	const int	STEP_IN				= 1;
	const int	STEP_RUN			= 2;
	const int	STEP_OUT			= 3;
	const int	STEP_END			= 999;
	
	int step = param->_step;
	
	GameExecCtx* gec = (GameExecCtx*)ec;
	TouchPanel* touchp = gec->getTouchPanel();
	ScreenEffect* screff = Game::getGame()->getWholeScreenEffect();

	switch(step)
	{
	case STEP_IN:
		if(param->_ecntStep == 1)
		{
			// フェードイン開始
			ColorU8 colBlack(0, 0, 0, 255);
			screff->setColor1(&colBlack);
			screff->setMaxFrame(60);
			screff->start(ScreenEffect::MODE_FADE_IN);
		}
		if(!screff->isWorking())
		{
			// 次のステップへ
			step = STEP_RUN;
		}
		if(touchp->isTouching())
		{
			// スキップ
			_isSkipLogo = true;
			screff->setMaxFrame(15);	// 最大フレーム強制書き換え
		}
		break;

	case STEP_RUN:
		if(touchp->isTouching() || _isSkipLogo)
		{
			// 次のステップへ
			_isSkipLogo = true;
			step = STEP_OUT;
		}
		else if(param->_frmcntStep >= 120.0f)
		{	
			// 次のステップへ
			step = STEP_OUT;
		}
		break;
		
	case STEP_OUT:
		if(param->_ecntStep == 1)
		{
			// フェードアウト開始
			ColorU8 colBlack(0, 0, 0, 255);
			screff->setColor1(&colBlack);
			screff->setMaxFrame(_isSkipLogo ? 15.0f : 60.0f);
			screff->start(ScreenEffect::MODE_FADE_OUT);
		}
		else if(!screff->isWorking())
		{
			// 次のステップへ
			res->setDone();
		}
		break;

	case STEP_END:
		break;
	}
	
	return step;
}

/*---------------------------------------------------------------------*//**
	セーブデータ引き継ぎ処理
**//*---------------------------------------------------------------------*/
int TitleRole::execCommand_pasteSaves(ExecRes* res, const ExecCtx* ec, const CmdParam* param)
{
	const int	STEP_IN				= 1;
	const int	STEP_RUN			= 2;
	const int	STEP_OUT			= 3;
	const int	STEP_END			= 999;
	
	int step = param->_step;
	
	GameExecCtx* gec = (GameExecCtx*)ec;
	TouchPanel* touchp = gec->getTouchPanel();
	ScreenEffect* screff = Game::getGame()->getWholeScreenEffect();
	GameSysMsgWindow* wndSysMsg = (GameSysMsgWindow*)Game::getGame()->getGui()->getSysMsgWindow(); ASSERT(wndSysMsg != 0L);
	
	switch(step)
	{
	case STEP_IN:
		if(param->_ecntStep == 1)
		{
			// ペーストボードの有効性を確認
			#if defined(_WINDOWS)
			#elif defined(_IOS)
				if(!isTransferSavesPasterboardValid())
				{
					// 処理をスキップする
					res->setDone();
					return STEP_END;
				}
			#endif

			// フェードイン開始
			ColorU8 colBlack(0, 0, 0, 255);
			screff->setColor1(&colBlack);
			screff->setMaxFrame(60);
			screff->start(ScreenEffect::MODE_FADE_IN);
		}
		if(!screff->isWorking())
		{
			// 確認メッセージ表示
			wndSysMsg->setKind(MessageWindow::KIND_YESNO);
			wndSysMsg->showMessage(GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_TITLE_CONFIRM_TRANS_SAVES_FROM), 0);

			// 次のステップへ
			step = STEP_RUN;
		}
		if(touchp->isTouching())
		{
			// スキップ
			screff->setMaxFrame(15);	// 最大フレーム強制書き換え
		}
		break;
		
	case STEP_RUN:
		if(wndSysMsg->isDone())
		{
			// メッセージウインドウを閉じる
			wndSysMsg->showWindow(false);

			// Yes のときのみセーブデータ転送処理
			if(wndSysMsg->getResult()->isClickYesButton())	// インポートの確認メッセージを「Yes」で返答した
			{
				bool isImportSucceeded = false;
				
				#if defined(_WINDOWS)
				#elif defined(_IOS)
					// セーブデータをインポート
					VcString namePasteBoard(NAME_TRANS_SAVES_PB);
					VcString nameFiles[] =
					{
						VcString(NAME_TRANS_SAVES_FILE_1),
						VcString(NAME_TRANS_SAVES_FILE_2),
						VcString(NAME_TRANS_SAVES_FILE_3),
						VcString(NAME_TRANS_SAVES_FILE_4)
					};
					VcString keyFiles[] =
					{
						VcString(KEY_TRANS_SAVES_FILE_1),
						VcString(KEY_TRANS_SAVES_FILE_2),
						VcString(KEY_TRANS_SAVES_FILE_3),
						VcString(KEY_TRANS_SAVES_FILE_4)
					};
					VcString nameTransSavesPb(NAME_TRANS_SAVES_PB);
					isImportSucceeded = OsDepModIos::transferSavesFrom(&nameTransSavesPb, nameFiles, keyFiles, NUM_TRANS_SAVES_FILES);
				#endif

				if(isImportSucceeded)
				{
					// 再びメッセージ表示「セーブデータの移行を完了しました。」
					wndSysMsg->setKind(MessageWindow::KIND_TALK);
					wndSysMsg->showMessage(GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_TITLE_TRANS_SAVES_DONE), 0);
					// システムコンフィグをロードする
					SaveSys* svsys = Game::getGame()->getSaveSys(); ASSERT(svsys != 0L);
					svsys->loadSysConf();
				}
				else
				{
					// 次のステップへ
					step = STEP_OUT;
				}
			}
			else if(wndSysMsg->getResult()->isClickNoButton())	// インポートの確認メッセージを「No」で返答した
			{
				#if defined(_WINDOWS)
				#elif defined(_IOS)
					// ペーストボードを削除する
					VcString nameTransSavesPb(NAME_TRANS_SAVES_PB);
					OsDepModIos::removeTransferSavesPasterboard(&nameTransSavesPb);
				#endif
				
				// 次のステップへ
				step = STEP_OUT;
			}
			else	// Yes,No 以外のメッセージ後
			{
				// 次のステップへ
				step = STEP_OUT;
			}
		}
		break;
		
	case STEP_OUT:
		if(param->_ecntStep == 1)
		{
			// フェードアウト開始
			ColorU8 colBlack(0, 0, 0, 255);
			screff->setColor1(&colBlack);
			screff->setMaxFrame(_isSkipLogo ? 15.0f : 60.0f);
			screff->start(ScreenEffect::MODE_FADE_OUT);
		}
		else if(!screff->isWorking())
		{
			// メニュー再作成要求
			_isRecreateMenuReserved = true;
			
			// 次のステップへ
			res->setDone();
		}
		break;
			
	case STEP_END:
		break;
	}
	
	return step;
}

/*---------------------------------------------------------------------*//**
	タイトル画面演出
**//*---------------------------------------------------------------------*/
int TitleRole::execCommand_stable(ExecRes* res, const ExecCtx* ec, const CmdParam* param)
{
	const int	STEP_IN				= 1;
	const int	STEP_RUN			= 2;
	const int	STEP_OUT			= 3;
	const int	STEP_CONFIRM		= 4;
	const int	STEP_WAIT_OPEN_URL	= 5;
	const int	STEP_END			= 999;
	
	int step = param->_step;

	GameExecCtx* gec = (GameExecCtx*)ec;
	ScreenEffect* screff = Game::getGame()->getWholeScreenEffect();
	TouchPanel* ui = gec->getTouchPanel();
	GameSysMsgWindow* wndSysMsg = (GameSysMsgWindow*)Game::getGame()->getGui()->getSysMsgWindow();
	
	switch(step)
	{
	case STEP_IN:
		if(param->_ecntStep == 1)
		{
			// フェードイン
			//if(!_isResumeStart)
			{
				ColorU8 colBlack(0, 0, 0, 255);
				screff->setColor1(&colBlack);
				screff->setMaxFrame(60);
				screff->start(ScreenEffect::MODE_FADE_IN);
			}
			
			// 広告バナー ON
			Game::getGame()->getOsDepMod()->showAdBanner(true);

			// 曲の再生
			Game::getGame()->getSoundManager()->playBgm(GameSoundDef::BGM_101_TITLE, true);

			// メニューを開く
			_menu->showMenu(_msprm);
		}
		if(!screff->isWorking())
		{
			step = STEP_RUN;
		}
		break;

	case STEP_RUN:
		if(wndSysMsg->isShowWindow())
		{
			step = STEP_CONFIRM;
		}
		else if(_menu->isDone() && !_isRecreateMenuReserved)
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
			Game::getGame()->getSoundManager()->playUiSe(GameSoundDef::SE_GAME_START, false);
			
			// 広告バナー OFF
			Game::getGame()->getOsDepMod()->showAdBanner(false);

			// フェードアウト
			ColorU8 colBlack(0, 0, 0, 255);
			screff->setColor1(&colBlack);
			screff->setMaxFrame((_entergame == EG_NEWGAME) ? 90.0f : 60.0f);
			screff->start(ScreenEffect::MODE_FADE_OUT);
		}
		if(!screff->isWorking())
		{
			screff->end(0);

			// 次のモードへ
			bool isSuccess = false;
			switch(_entergame)
			{
			case EG_NEWGAME:
				isSuccess = Game::getGame()->enterGame(Game::EGKIND_NEW, 0, 0);
				break;
			case EG_RESTARTGAME_1:
				isSuccess = Game::getGame()->enterGame(Game::EGKIND_RESTART, 1, 0);
				break;
			case EG_RESTARTGAME_2:
				isSuccess = Game::getGame()->enterGame(Game::EGKIND_RESTART, 2, 0);
				break;
			case EG_RESUME:
				isSuccess = Game::getGame()->enterGame(Game::EGKIND_LOAD, SaveSys::SVDID_RESUME, 0);
				break;
			case EG_LOAD_1:
				isSuccess = Game::getGame()->enterGame(Game::EGKIND_LOAD, 1, 0);
				break;
			case EG_LOAD_2:
				isSuccess = Game::getGame()->enterGame(Game::EGKIND_LOAD, 2, 0);
				break;
			}

			if(isSuccess)
			{
				// 終了
				res->setDone();
			}
			else
			{
				// ここでエラー表示？

				// メニューを再度開いて戻る
				_menu->showMenu(_msprm);
				step = STEP_RUN;

				// フェードイン
				ColorU8 colBlack(0, 0, 0, 255);
				screff->setColor1(&colBlack);
				screff->setMaxFrame(15);
				screff->start(ScreenEffect::MODE_FADE_IN);
			}
		}
		break;

	case STEP_CONFIRM:
		{
			if(wndSysMsg->isDone())
			{
				// メッセージウインドウを閉じる
				wndSysMsg->showWindow(false);
				
				// Yes のときのみセーブデータ転送処理
				if(wndSysMsg->getResult()->isClickYesButton())
				{
					#if defined(_WINDOWS)
					#elif defined(_IOS)
						// セーブデータをエクスポートする
						VcString nameFiles[] =
						{
							VcString(NAME_TRANS_SAVES_FILE_1),
							VcString(NAME_TRANS_SAVES_FILE_2),
							VcString(NAME_TRANS_SAVES_FILE_3),
							VcString(NAME_TRANS_SAVES_FILE_4)
						};
						VcString keyFiles[] =
						{
							VcString(KEY_TRANS_SAVES_FILE_1),
							VcString(KEY_TRANS_SAVES_FILE_2),
							VcString(KEY_TRANS_SAVES_FILE_3),
							VcString(KEY_TRANS_SAVES_FILE_4)
						};
						VcString nameTransSavesPb(NAME_TRANS_SAVES_PB);
						if(OsDepModIos::transferSavesTo(&nameTransSavesPb, nameFiles, keyFiles, NUM_TRANS_SAVES_FILES))
						{
							// 次のステップへ
							step = STEP_WAIT_OPEN_URL;
						}
						else
						{
							#if defined(_WINDOWS)
							#elif defined(_IOS)
								// ペーストボードを削除する
								OsDepModIos::removeTransferSavesPasterboard(&nameTransSavesPb);
							#endif
							// 失敗メッセージ表示「セーブデータの移行に失敗しました。」
							wndSysMsg->setKind(MessageWindow::KIND_TALK);
							wndSysMsg->showMessage(GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_TITLE_TRANS_SAVES_FAILED), 0);
						}
					#endif
				}
				else
				{
					// メニューを再度開く
					_menu->showMenu(_msprm);
					// 元のステップへ戻る
					step = STEP_RUN;
				}
			}
		}
		break;

	case STEP_WAIT_OPEN_URL:
		if(param->_ecntStep == 1)
		{
			// フェードアウト
			ColorU8 colBlack(0, 0, 0, 255);
			screff->setColor1(&colBlack);
			screff->setMaxFrame(30);
			screff->start(ScreenEffect::MODE_FADE_OUT);
		}
		else if(!screff->isWorking() && isTransferSavesPasterboardValid())	// ペーストボード反映までに時間がかかる？対策（効果不明）
		{
			#if defined(_WINDOWS)
			#elif defined(_IOS)
				CcString cstr(URLQUERY_TRANS_SAVES_BOOT);
				if(!OsDepModIos::runApp(PRODDEFID_V01, &cstr))
				{
					// スクリーンエフェクト終了
					screff->end(0);
					// URL オープン失敗メッセージ表示「有料版 ver1.1.1 以上が必要です」							
					wndSysMsg->setKind(MessageWindow::KIND_TALK);
					wndSysMsg->showMessage(GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_TITLE_REQUIRE_PAID_VER_111), 0);
					// 確認ステップへ戻る
					step = STEP_CONFIRM;
				}
			#endif
		}
		else if(param->_ecntStep > 300)	// 失敗．タイムアウト
		{
			// スクリーンエフェクト終了
			screff->end(0);
			#if defined(_WINDOWS)
			#elif defined(_IOS)
				// ペーストボードを削除する
				VcString nameTransSavesPb(NAME_TRANS_SAVES_PB);
				OsDepModIos::removeTransferSavesPasterboard(&nameTransSavesPb);
			#endif
			// 失敗メッセージ表示「セーブデータの移行に失敗しました。」
			wndSysMsg->setKind(MessageWindow::KIND_TALK);
			wndSysMsg->showMessage(GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_TITLE_TRANS_SAVES_FAILED), 0);
			// 確認ステップへ戻る
			step = STEP_CONFIRM;
		}
		break;
			
	case STEP_END:
		break;
	}

	// 子パネル制御
	PointF32 ptTouch(S32_MIN, S32_MIN);
	PointF32 ptClick(S32_MIN, S32_MIN);
	bool isTouch = !ui->isReacted() ? ui->getTouchPosition(&ptTouch) : false;
	bool isClick = !ui->isReacted() ? /*ui->isClickRelease(&ptClick)*/ui->isTapRelease(&ptClick, 0L) : false;
	for(int i = 0; i < NUM_PNL; i++)
	{
		if(isClick)
		{
			if(_pnl[i]->hit(&ptClick, true))
			{
				switch(i)
				{
				case PNL_HP:
					// ホームページへジャンプ
					{
						VcString url(Env_getLangId() == etk::LANGID_EN ? "http://www.sourcerer.jp/en/" : "http://www.sourcerer.jp/");
						Game::getGame()->getOsDepMod()->gotoHp(&url);
					}
					break;
#if ENABLE_PSNS
				case PNL_PSNS_SCORE:
					Game::getGame()->getPsns()->showPsnsScreen(PsnsBase::SK_SCORE);
					break;
				case PNL_PSNS_TROPHY:
					Game::getGame()->getPsns()->showPsnsScreen(PsnsBase::SK_TROPHY);
					break;
#endif	// #if ENABLE_PSNS
				}

				ui->setReacted(true);
			}
		}
		else
		{
			_pnl[i]->hit(&ptTouch, isTouch);
		}
		_pnl[i]->exec(res, ec);
	}

	// PSNS の情報更新によるボタンなどの更新
	#if ENABLE_PSNS
		if((gec->getBodyExecCount() & 31) == 0)
		{
			bool isValidPsns = Game::getGame()->getPsns()->isValidUserAccount();
			bool isValidBtn = _pnl[PNL_PSNS_SCORE]->isEnable();
			if(isValidPsns != isValidBtn)
			{
				_pnl[PNL_PSNS_SCORE]->setEnable(isValidPsns);
				_pnl[PNL_PSNS_TROPHY]->setEnable(isValidPsns);
				updateMenu();
			}
		}
	#endif	// #if ENABLE_PSNS

	return step;
}

/*---------------------------------------------------------------------*//**
	コマンド描画
**//*---------------------------------------------------------------------*/
void TitleRole::drawCommand(const RenderCtx* rc, const CmdParam* param)
{
	if(param->_ecntBegun == 0)	{	return;	}	// 0 フレーム目は描画しない

	Renderer* rdr = rc->getRenderer();
	ColorU8 col;
	RectF32 vtx, uv;

	// 背景を描画する
	switch(param->_cid)
	{
	case LOGO:
		if(_texCorpLogo != 0L)	{	rdr->bindTexture(_texCorpLogo);	}	// テクスチャをバインド
		RendererUtils::draw2dTextureToScreenFitLongSide(rdr, Game::getGame()->getTfw()->getView(), Gcalc::calcTexUvNoAa(&uv, UV_CORP_LOGO), ColorU8::setout(&col, 0, 0, 0, 255));
		break;
	case PASTE_SAVES:
		if(_texTitle != 0L)		{	rdr->bindTexture(_texTitle);	}	// テクスチャをバインド
		RendererUtils::draw2dTextureToScreenFitShortSide(rdr, Game::getGame()->getTfw()->getView(), Gcalc::calcTexUvNoAa(&uv, UV_TITLE_BG));
		break;
	case STABLE:
		if(_texTitle != 0L)		{	rdr->bindTexture(_texTitle);	}	// テクスチャをバインド

		// 背景
		RendererUtils::draw2dTextureToScreenFitShortSide(rdr, Game::getGame()->getTfw()->getView(), Gcalc::calcTexUvNoAa(&uv, UV_TITLE_BG));
		// ロゴ
		RendererUtils::draw2dTextureRect(rdr, RectF32::setout(&vtx, (Game::getGame()->getLogicalWidth() - W_TITLE_LOGO) * 0.5f, 0.0f, W_TITLE_LOGO, H_TITLE_LOGO), Gcalc::calcTexUvNoAa(&uv, UV_TITLE_LOGO));
		// Copyright
		RendererUtils::draw2dTextureRect(rdr, RectF32::setout(&vtx, (Game::getGame()->getLogicalWidth() - W_COPYRIGHT) * 0.5f, Game::getGame()->getLogicalHeight() - H_COPYRIGHT - 4.0f, W_COPYRIGHT, H_COPYRIGHT), Gcalc::calcTexUvNoAa(&uv, UV_COPYRIGHT));

		// バージョン文字列描画
		rdr->setSolidColor(138, 148, 109, 255);	//rdr->setSolidColor(116, 165, 116, 255);
		EasyStringDrawer::draw(((GameRenderCtx*)rc)->getFontSet()->getFont(GameFontSet::EN), _strVersion, 0.0f, Game::getGame()->getLogicalHeight() - 42.0f, Game::getGame()->getLogicalWidth() - 4.0f, 12.0f, 10, EasyStringDrawer::F_ALIGN_RIGHT, rc);
		// 省リソース動作モード表示
		if(TFW_IS_FLAG(Game::getGame()->getExFlags(), EtkBody::EXF_SAVE_RESOURCE_RUN))
		{
			rdr->setSolidColor(255, 255, 255, 127);
			EasyStringDrawer::draw(((GameRenderCtx*)rc)->getFontSet()->getFont(GameFontSet::EN), "* resources saving mode *", 4.0f, Game::getGame()->getLogicalHeight() - 42.0f, Game::getGame()->getLogicalWidth() - 4.0f, 12.0f, 10, EasyStringDrawer::F_ALIGN_CENTER, rc);
		}
		// 子パネル描画
		for(int i = 0; i < NUM_PNL; i++)
		{
			if(_pnl[i] != 0)	{	_pnl[i]->draw(rc);	}
		}
		// iAd Test
		#if 0
			const int W_IAD = 480;
			const int H_IAD = 32;
			rdr->setSolidColor(0, 0, 0, 255);
			RendererUtils::draw2dRect(rdr, (f32)((Game::getGame()->getLogicalWidth() - W_IAD) / 2), (f32)(Game::getGame()->getLogicalHeight() - H_IAD), (f32)W_IAD, (f32)H_IAD);
		#endif
		break;
	}
}

/*---------------------------------------------------------------------*//**
	メニューノードクリック通知
**//*---------------------------------------------------------------------*/
bool TitleRole::onClickNode(const ExecCtx* ec, MenuTreeNode* mtnodeCall, MenuTreeNode* mtnodeParent, void* objCreateParam, void* objShowParam)
{
	const VcString* cmd = mtnodeCall->getName();
	if( cmd->equals("NewGame") )
	{
		_entergame = EG_NEWGAME;
	}
	else if( cmd->equals("RestartGame01") )
	{
		_entergame = EG_RESTARTGAME_1;
	}
	else if( cmd->equals("RestartGame02") )
	{
		_entergame = EG_RESTARTGAME_2;
	}
	else if( cmd->equals("ResumeGame") )
	{
		_entergame = EG_RESUME;
	}
	else if( cmd->equals("LoadGame01") )
	{
		_entergame = EG_LOAD_1;
	}
	else if( cmd->equals("LoadGame02") )
	{
		_entergame = EG_LOAD_2;
	}
	else if( cmd->equals("TransferSavesTo") )
	{
		// 確認メッセージ表示
		GameSysMsgWindow* wndSysMsg = (GameSysMsgWindow*)Game::getGame()->getGui()->getSysMsgWindow();
		wndSysMsg->setKind(MessageWindow::KIND_YESNO);
		wndSysMsg->showMessage(GameFixedStrTbl::getString(GameFixedStrTbl::SID_GUI_TITLE_CONFIRM_TRANS_SAVES_TO), 0);
	}
	else if( cmd->equals("TransferSavesFrom") )
	{
	}
	else if( cmd->equals("Scr_Orien_M90") )
	{
		SaveSys* svsys = Game::getGame()->getSaveSys(); ASSERT(svsys != 0L);
		SysConf* sysconf = svsys->getSysConf(); ASSERT(sysconf != 0L);
		sysconf->setScreenOrientation(false);	// 設定する
		svsys->saveSysConf();					// ファイルに保存する

		MenuTreeNode* mtnodeSrM90 = _menu->findMenuTreeNode(MTNODEID_SCRROT_M90); ASSERT(mtnodeSrM90 != 0L);
		MenuTreeNode* mtnodeSrP90 = _menu->findMenuTreeNode(MTNODEID_SCRROT_P90); ASSERT(mtnodeSrP90 != 0L);
		mtnodeSrM90->setChecked(true);
		mtnodeSrP90->setChecked(false);
	}
	else if( cmd->equals("Scr_Orien_P90") )
	{
		SaveSys* svsys = Game::getGame()->getSaveSys(); ASSERT(svsys != 0L);
		SysConf* sysconf = svsys->getSysConf(); ASSERT(sysconf != 0L);
		sysconf->setScreenOrientation(true);	// 設定する
		svsys->saveSysConf();					// ファイルに保存する

		MenuTreeNode* mtnodeSrM90 = _menu->findMenuTreeNode(MTNODEID_SCRROT_M90); ASSERT(mtnodeSrM90 != 0L);
		MenuTreeNode* mtnodeSrP90 = _menu->findMenuTreeNode(MTNODEID_SCRROT_P90); ASSERT(mtnodeSrP90 != 0L);
		mtnodeSrM90->setChecked(false);
		mtnodeSrP90->setChecked(true);
	}
	else if( cmd->equals("Language_English") )
	{
		SaveSys* svsys = Game::getGame()->getSaveSys(); ASSERT(svsys != 0L);
		SysConf* sysconf = svsys->getSysConf(); ASSERT(sysconf != 0L);
		sysconf->setLanguage(LANGID_EN);		// 設定する
		svsys->saveSysConf();					// ファイルに保存する

		_isRecreateMenuReserved = true;			// メニュー再生成要求
	}
	else if( cmd->equals("Language_Japanese") )
	{
		SaveSys* svsys = Game::getGame()->getSaveSys(); ASSERT(svsys != 0L);
		SysConf* sysconf = svsys->getSysConf(); ASSERT(sysconf != 0L);
		sysconf->setLanguage(LANGID_JP);		// 設定する
		svsys->saveSysConf();					// ファイルに保存する

		_isRecreateMenuReserved = true;			// メニュー再生成要求
	}
#if 0
	else if( cmd->equals("SayMsg_Pause") )
	{
		SaveSys* svsys = Game::getGame()->getSaveSys(); ASSERT(svsys != 0L);
		SysConf* sysconf = svsys->getSysConf(); ASSERT(sysconf != 0L);
		sysconf->setEnableWaitMovieMessage(true);	// ポーズを有効に
		svsys->saveSysConf();					// ファイルに保存する

		MenuTreeNode* mtnodeSayPause = _menu->findMenuTreeNode(MTNODEID_SAYMSG_PAUSE); ASSERT(mtnodeSayPause != 0L);
		MenuTreeNode* mtnodeSayNowait = _menu->findMenuTreeNode(MTNODEID_SAYMSG_NOWAIT); ASSERT(mtnodeSayNowait != 0L);
		mtnodeSayPause->setChecked(true);
		mtnodeSayNowait->setChecked(false);
	}
	else if( cmd->equals("SayMsg_NoWait") )
	{
		SaveSys* svsys = Game::getGame()->getSaveSys(); ASSERT(svsys != 0L);
		SysConf* sysconf = svsys->getSysConf(); ASSERT(sysconf != 0L);
		sysconf->setEnableWaitMovieMessage(false);	// ポーズを無効に（スキップを有効に）
		svsys->saveSysConf();					// ファイルに保存する

		MenuTreeNode* mtnodeSayPause = _menu->findMenuTreeNode(MTNODEID_SAYMSG_PAUSE); ASSERT(mtnodeSayPause != 0L);
		MenuTreeNode* mtnodeSayNowait = _menu->findMenuTreeNode(MTNODEID_SAYMSG_NOWAIT); ASSERT(mtnodeSayNowait != 0L);
		mtnodeSayPause->setChecked(false);
		mtnodeSayNowait->setChecked(true);
	}
#endif
#if 0
	else if( cmd->equals("Music_Stereo") )
	{
		// ステレオに設定
		Game::getGame()->getSoundManager()->setStereoMusic(true);
		// 曲の再生
		Game::getGame()->getSoundManager()->playBgm(GameSoundDef::BGM_TITLE, -1, true);
		_isRecreateMenuReserved = true;
	}
	else if( cmd->equals("Music_Mono") )
	{
		// モノラルに設定
		Game::getGame()->getSoundManager()->setStereoMusic(false);
		// 曲の再生
		Game::getGame()->getSoundManager()->playBgm(GameSoundDef::BGM_TITLE, -1, true);
		_isRecreateMenuReserved = true;
	}
#endif
	else if( cmd->equals("MultiTasking_Fast") )
	{
		SaveSys* svsys = Game::getGame()->getSaveSys(); ASSERT(svsys != 0L);
		SysConf* sysconf = svsys->getSysConf(); ASSERT(sysconf != 0L);
		sysconf->setEcoMultiTasking(false);		// 設定する
		svsys->saveSysConf();					// ファイルに保存する

		MenuTreeNode* mtnodeFast = _menu->findMenuTreeNode(MTNODEID_MULTITASKING_FAST);	ASSERT(mtnodeFast != 0L);
		mtnodeFast->setChecked(!sysconf->isEcoMultiTasking());
		MenuTreeNode* mtnodeEco = _menu->findMenuTreeNode(MTNODEID_MULTITASKING_ECO);	ASSERT(mtnodeEco != 0L);
		mtnodeEco->setChecked(sysconf->isEcoMultiTasking());
	}
	else if( cmd->equals("MultiTasking_Eco") )
	{
		SaveSys* svsys = Game::getGame()->getSaveSys(); ASSERT(svsys != 0L);
		SysConf* sysconf = svsys->getSysConf(); ASSERT(sysconf != 0L);
		sysconf->setEcoMultiTasking(true);		// 設定する
		svsys->saveSysConf();					// ファイルに保存する

		MenuTreeNode* mtnodeFast = _menu->findMenuTreeNode(MTNODEID_MULTITASKING_FAST);	ASSERT(mtnodeFast != 0L);
		mtnodeFast->setChecked(!sysconf->isEcoMultiTasking());
		MenuTreeNode* mtnodeEco = _menu->findMenuTreeNode(MTNODEID_MULTITASKING_ECO);	ASSERT(mtnodeEco != 0L);
		mtnodeEco->setChecked(sysconf->isEcoMultiTasking());
	}
#if defined(_ENLIGHT_PRPZL01)
	else if( cmd->equals("StartGame") )
	{
		_entergame = EG_NEWGAME;
	}
#endif

	return true;
}

//==========================================================================
// TitleRole::MenuFuncTable_Title メソッド

/*---------------------------------------------------------------------*//**
	表示通知
**//*---------------------------------------------------------------------*/
void TitleRole::MenuFuncTable_Title::onShow(Menu* menu, MenuTreeNode* mtnodeShow, void* objCreateParam, void* objShowParam)
{
}

/*---------------------------------------------------------------------*//**
	非表示通知
**//*---------------------------------------------------------------------*/
void TitleRole::MenuFuncTable_Title::onHide(Menu* menu, void* objCreateParam, void* objShowParam)
{
}

/*---------------------------------------------------------------------*//**
	呼び出し通知
**//*---------------------------------------------------------------------*/
bool TitleRole::MenuFuncTable_Title::onCall(Menu* menu, const ExecCtx* ec, MenuTreeNode* mtnodeCall, MenuTreeNode* mtnodeParent, void* objCreateParam, void* objShowParam)
{
	GameMenuShowParam* sprm = (GameMenuShowParam*)objShowParam;	ASSERT(sprm != 0L);
	TitleRole* own = (TitleRole*)sprm->_ownerRef;	ASSERT(own != 0L);
	return own->onClickNode(ec, mtnodeCall, mtnodeParent, objCreateParam, objShowParam);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
