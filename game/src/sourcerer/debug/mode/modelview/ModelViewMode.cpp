/***********************************************************************//**
 *	ModelViewMode.h
 *	Enlight Game Application
 *
 *	Created by Ryoutarou Kishi on 2009/06/23.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// マクロ

#if defined(_DEBUG)
	#define DEBUG_STATUS_DRAWER	(1)
#else
	#define DEBUG_STATUS_DRAWER	(0)
#endif

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "ModelViewMode.h"

// Friends
#include "../../../body/Game.h"
#include "../../../body/GameExecCtx.h"
#include "../../../body/GameRenderCtx.h"
#include "../../../gui/GameFontSet.h"
#include "../../../particle/GameParticleDef.h"
#if DEBUG_STATUS_DRAWER
#include "../../../gui/GameGui.h"
#include "../../../status/CharStat.h"
#include "../../../status/EleneStat.h"
#include "../../../status/StatusDrawer.h"
#endif

// External
#include "../../../../etk/camera/ModelViewCam.h"
#include "../../../../etk/particle/ParticleSys.h"
#include "../../../../etk/sg/CamSgnc.h"
#include "../../../../etk/sg/SorSgm.h"
#include "../../../../tfw/Tfw.h"
#include "../../../../tfw/g2d/font/Font.h"
#include "../../../../tfw/g3d/sg/node/SgNodeChain.h"
#include "../../../../tfw/g3d/sg/node/implement/AnimCamera.h"
#include "../../../../tfw/g3d/sg/node/implement/ShapeModel.h"
#include "../../../../tfw/g3d/sg/node/implement/ModelLoader.h"
#include "../../../../tfw/gcmn/EasyStringDrawer.h"
#include "../../../../tfw/gcmn/Renderer.h"
#include "../../../../tfw/gcmn/RendererUtils.h"
#include "../../../../tfw/collision/Gcol.h"
#include "../../../../tfw/file/File.h"
#include "../../../../tfw/lib/Calc.h"
#include "../../../../tfw/lib/Vector.h"
#include "../../../../tfw/ui/Keyboard.h"
#include "../../../../tfw/ui/TouchPanel.h"

// Library

////////////////////////////////////////////////////////////////////////////

SRCR_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// ModelViewMode メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	カスタムイベント呼び出しハンドラ
**//*---------------------------------------------------------------------*/
void ModelViewMode::callCustomEvent(void* objRes, const VcString* strParam, const void* objParam)
{
	if(strParam->equals("GameTest::changeTestViewMode"))
	{
		// 制御モード変更
		endActMode();
		switch(_amode)
		{
		case AMODE_MODEL:
			beginActMode(AMODE_RTMV, false);
			break;
		case AMODE_RTMV:
			beginActMode(AMODE_PTCL, false);
			break;
		case AMODE_PTCL:
			beginActMode(AMODE_FONT, false);
			break;
		case AMODE_FONT:
			beginActMode(AMODE_MODEL, false);
			///this->getParentMode()->reserveMode(GameMode::MODE_GAME_BOOKVW);	// 次のデバッグモードへ
			///this->getParentMode()->reserveNextMode();	// 次のデバッグモードへ
			Game::getGame()->getGameMode()->changeEndMode((GameMode::ModeKind)getParentMode()->getNextMode());	// 次のデバッグモードへ
			break;
		}
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
ModelViewMode::ModelViewMode()
	: _mdlarrRef(0L)
	, _numMdl(0)
	, _animid(0)
	, _ptclid(0)
	, _dispCnt(0)
	, _amode(AMODE_MODEL)
	, _param(0)
	, _camMvRef(0L)
	, _camRmRef(0L)
{
	#if DEBUG_STATUS_DRAWER
		_cstatDbg = new CharStat(true);
		Elene oql(10000, 10000, 10000, 10000, 10000);
		_cstatDbg->eleneStat()->setup(&oql, &oql);
	#endif
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
ModelViewMode::~ModelViewMode()
{
	ASSERT(_mdlarrRef == 0L);
	#if DEBUG_STATUS_DRAWER
		delete _cstatDbg;
	#endif
}

/*---------------------------------------------------------------------*//**
	開始
**//*---------------------------------------------------------------------*/
bool ModelViewMode::notifyBegin(void* objParamRef)
{
	// パラメータ取得
	GameModeParam* gmparam = (GameModeParam*)objParamRef;
	if(TFW_IS_FLAG(gmparam->getFlags(), GameModeParam::F_DEBUG_MODEL_VIEW_FILE1))		{	_param = 1;	}
	else if(TFW_IS_FLAG(gmparam->getFlags(), GameModeParam::F_DEBUG_MODEL_VIEW_FILE2))	{	_param = 2;	}
	else if(TFW_IS_FLAG(gmparam->getFlags(), GameModeParam::F_DEBUG_MODEL_VIEW_FILE3))	{	_param = 3;	}
	if(gmparam != 0L)	{	gmparam->clearFlags();	}

	// 制御モード開始
	beginActMode(AMODE_MODEL, true);

	// クリアカラー設定
	Vector4F* colorClear = Game::getGame()->getTfw()->getRenderer()->clearColor();
	colorClear->set(0.5f, 0.5f, 0.5f, 1.0f);

	// 値の初期化
	_dispCnt = 0;

	return true;
}

/*---------------------------------------------------------------------*//**
	終了
**//*---------------------------------------------------------------------*/
void ModelViewMode::notifyEnd(void* objParamRef)
{
	// クリアカラー設定を戻す
	Vector4F* colorClear = Game::getGame()->getTfw()->getRenderer()->clearColor();
	colorClear->set(0.0f, 0.0f, 0.0f, 0.0f);

	// 制御モード終了
	endActMode();
}

/*---------------------------------------------------------------------*//**
	制御モード開始
**//*---------------------------------------------------------------------*/
bool ModelViewMode::beginActMode(ActMode amode, bool isFirst)
{
	SorSgm* sgm = Game::getGame()->getSceneGraph();

	// シーングラフ及びリソースプールにモデルビュー標準カメラを追加
	CamSgnc* nodecCam = (CamSgnc*)sgm->getKindChain(SorSgm::EK_CAM);
	{
		// モデルビュー専用カメラの追加
		ModelViewCam* mvcam = new ModelViewCam();
		if(!mvcam->create())
		{
			return false;
		}
		if(sgm->addNode(mvcam, true))
		{
			nodecCam->switchVisible(mvcam->getId());	// カメラを有効化
			_camMvRef = mvcam;
		}
	}

	switch(amode)
	{
	case AMODE_MODEL:
	case AMODE_RTMV:
		{
			// モデルを読み込み、リソースプールとシーングラフに追加
			VcString name;
			switch(_param)
			{
			case 0:	case 1:	name = "modelview_test";			break;
			case 2:			name = "modelview_test_t-hayashi";	break;
			case 3:			name = "modelview_test_r-kishi";	break;
			}
			if(name.getLength() > 0)
			{
				VcString filename = name + ".tbmb";
				if(File::isFileExist(&filename))
				{
					ShapeModel** mdlarr = 0L;
					s32 numMdl = 0;
					if(ModelLoader::loadModelsFromTbf(Game::getGame()->getTfw(), &name, 0, ModelLoader::LFLAG_DISPOSE_SCENE_GRAPH, (Model***)&mdlarr, &numMdl))
					{
						_mdlarrRef = mdlarr;
						_numMdl = numMdl;
						
						#if TEST_PTCL
						{
							if((param == 3) && (numMdl >= 1))
							{
								const s32 numPtcl = 75;
								s32 numMdlNew = numPtcl;
								ShapeModel** mdlarrNew = new ShapeModel*[numPtcl];

								mdlarrNew[0] = _mdlarrRef[0];
								for(int i = 1; i < numMdlNew; i++)
								{
									mdlarrNew[i] = _mdlarrRef[0]->clone();
									const f32 range = 1000.0f;
									Vector3F pos(
										(2.0f * range * ::rand() / (float)RAND_MAX) - range	,
										(2.0f * range * ::rand() / (float)RAND_MAX) - range	,
										(2.0f * range * ::rand() / (float)RAND_MAX) - range	);
									Vector3F dir;
									mdlarrNew[i]->setPosition(&pos, &dir);

									sgm->addNode(mdlarrNew[i], true);
								}

								delete[] _mdlarrRef;
								_mdlarrRef = mdlarrNew;
								_numMdl = numMdlNew;
							}
						}
						#endif

						// アニメーションセット
						_animid = 1;	// 33;
						for(int i = 0; i < _numMdl; i++)
						{
							if(_animid <= _mdlarrRef[i]->getMaxAnimId())
							{
								_mdlarrRef[i]->setAnimId(_animid, 0, true, true, true, false, false);
							}
						}

						// モデルが一つのときは強制表示（DEFTYPE_DECLARE だと表示されないので）
						if(_numMdl == 1)
						{
							_mdlarrRef[0]->setShow(true);
						}
					}
				}
			}

			if((amode == AMODE_RTMV) || isFirst)
			{
				// 更にリアルタイムムービー用のカメラを必要に応じて追加
				if((_mdlarrRef != 0L) && (_mdlarrRef[0]->getCamAnim() != 0L))	// アニメーションカメラがモデルにある場合
				{
					// クリップ無しに
					for(int i = 0; i < _numMdl; i++)
					{
						_mdlarrRef[i]->setModelFlag(Model::MFLAG_NO_CLIP, true);
					}
	
					// アニメーションカメラ
					AnimCamera* ancam = new AnimCamera();
					if(!ancam->create())
					{
						return false;
					}
					if(sgm->addNode(ancam, true))
					{
						ancam->setAnim(_mdlarrRef[0]->getCamAnim());	// アニメーションを設定
						nodecCam->switchVisible(ancam->getId());		// カメラを有効化
						_camRmRef = ancam;

						// 最初の実行時のみ、自動的にリアルタイムムービー制御モードに移行
						if(isFirst)
						{
							amode = AMODE_RTMV;
						}
					}
				}
			}
		}
		break;
	case AMODE_PTCL:
		appearNextParticle();	// パーティクル出現
		break;
	case AMODE_FONT:
		appearNextFont();
		break;
	}

	_amode = amode;
	_dispCnt = 0;	// 表示カウンタリセット

	return true;
}

/*---------------------------------------------------------------------*//**
	制御モード終了
**//*---------------------------------------------------------------------*/
void ModelViewMode::endActMode()
{
	// シーングラフのモデルを破棄
	SorSgm* sgm = Game::getGame()->getSceneGraph();
	if(_mdlarrRef != 0L)
	{
		for(int i = 0; i < _numMdl; i++)
		{
			sgm->removeNode(_mdlarrRef[i]);
		}
		delete[] _mdlarrRef;
		_mdlarrRef = 0L;
		_numMdl = 0;
	}

	// カメラを破棄
	if(_camMvRef != 0L)
	{
		sgm->removeNode(_camMvRef);
		_camMvRef = 0L;
	}
	if(_camRmRef != 0L)
	{
		sgm->removeNode(_camRmRef);
		_camRmRef = 0L;
	}

	// パーティクル破棄
	if(_ptclid != 0)
	{
		ParticleSys* ptclsys = Game::getGame()->getParticleSys();
		ptclsys->disappearParticle(_ptclid);
		_ptclid = 0;
	}
}

/*---------------------------------------------------------------------*//**
	現在のパーティクルに送る
**//*---------------------------------------------------------------------*/
void ModelViewMode::appearCurParticle()
{
	ParticleSys* ptclsys = Game::getGame()->getParticleSys();

	// 既存のパーティクルを破棄
	if(_ptclid != 0)
	{
		ParticleSys* ptclsys = Game::getGame()->getParticleSys();
		ptclsys->disappearParticle(_ptclid);
		_ptclid = 0;
	}
	
	// 新しいパーティクルを出現させる
	_ptclid = ptclsys->appearParticle(_param, 0L, GameParticleDef::CF_NO_CALC_VEL);
	if(_ptclid != 0)
	{
		_dispCnt = 0;	// 表示カウンタリセット
	}
}

/*---------------------------------------------------------------------*//**
	次のパーティクルに送る
**//*---------------------------------------------------------------------*/
void ModelViewMode::appearNextParticle()
{
	// 新しいパーティクルを出現させる
	do
	{
		if((0 <= _param) && (_param < 32767))
		{
			if(_param == 1000)	{	_param = 30000;	}	// 空乏 ID をスキップ
			_param++;
			appearCurParticle();
		}
		else
		{
			_param = 0;
		}
	}
	while(_ptclid == 0);
}

/*---------------------------------------------------------------------*//**
	前のパーティクルに送る
**//*---------------------------------------------------------------------*/
void ModelViewMode::appearPrevParticle()
{
	// 新しいパーティクルを出現させる
	do
	{
		if((0 <= _param) && (_param < 32767))
		{
			if(_param == 29999)	{	_param = 999;	}	// 空乏 ID をスキップ
			_param--;
			appearCurParticle();
		}
		else
		{
			_param = 32766;
		}
	}
	while(_ptclid == 0);
}

/*---------------------------------------------------------------------*//**
	次のフォントに送る
**//*---------------------------------------------------------------------*/
void ModelViewMode::appearNextFont()
{
	if((0 <= _param) && (_param < 2))
	{
		_param++;
	}
	else
	{
		_param = 0;
	}

	_dispCnt = 0;	// 表示カウンタリセット
}

/*---------------------------------------------------------------------*//**
	フレーム処理
**//*---------------------------------------------------------------------*/
void ModelViewMode::exec(ExecRes* res, const ExecCtx* ec)
{
	GameModeBase::exec(res, ec);

	// ユーザー操作
	TouchPanel* touchp = ((GameExecCtx*)ec)->getTouchPanel();
	Keyboard* keybrd = ((GameExecCtx*)ec)->getKeyboard();
	if(!touchp->isReacted() && touchp->isClickRelease(0L))
	{
		switch(_amode)
		{
		case AMODE_MODEL:
		{
							// アニメーション切替
							_animid++;
							bool isValidAnimId = false;
							for(int cnt = 1; cnt <= 2; cnt++)
							{
								for(int i = 0; i < _numMdl; i++)
								{
									if(_animid <= _mdlarrRef[i]->getMaxAnimId())
									{
										_mdlarrRef[i]->setAnimId(_animid, 30, true, true, true, false, false);
										isValidAnimId = true;
									}
								}
								if(isValidAnimId)	{ break; }
								_animid = 1;
							}
		}
			_dispCnt = 0;	// 表示カウンタリセット
			touchp->setReacted(true);
			break;
		case AMODE_PTCL:
			appearNextParticle();
			touchp->setReacted(true);
			break;
		case AMODE_FONT:
			appearNextFont();
			touchp->setReacted(true);
			break;
		}
	}
	if(keybrd != 0L)
	{
		switch(_amode)
		{
		case AMODE_PTCL:
			STRACE("Enter:shot, ->:next, <-:prev\n");
			if(keybrd->isTrigger('\n'))
			{
				appearCurParticle();
			}
			else if(keybrd->isRepeatTrigger(Keyboard::SYSKEY_RIGHT))
			{
				appearNextParticle();
			}
			else if(keybrd->isRepeatTrigger(Keyboard::SYSKEY_LEFT))
			{
				appearPrevParticle();
			}
		}
	}

	// モードをスクリーントレース
	if(_dispCnt < 120)
	{
		VcString stmsg;
		switch(_amode)
		{
		case AMODE_MODEL:	stmsg += ": MODELVIEW :";	break;
		case AMODE_RTMV:	stmsg += ": CAM-ANIM :";	break;
		case AMODE_PTCL:	stmsg += ": PARTICLE :";	break;
		case AMODE_FONT:	stmsg += ": FONT :";		break;
		}
		switch(_amode)
		{
		case AMODE_MODEL:
		case AMODE_RTMV:
			stmsg += VcString::format(", AnimId: %d\n", _animid);
			break;
		case AMODE_PTCL:
			stmsg += VcString::format(", ParticleDefId: %d\n", _param);
			break;
		case AMODE_FONT:
			stmsg += VcString::format(", %s\n", (_param == 0) ? "EN" : (_param == 1) ? "JP_GOTHIC" : "JP_MOVIE");
			break;
		}
		STRACE(stmsg.getRaw());
	}
	_dispCnt++;

	// キーボード動作の表示
	if(true)
	{
		for(int i = 0; i < Keyboard::NUM_KEY; i++)
		{
			if(keybrd->isOn(i))
			{
				switch(i)
				{
				case Keyboard::SYSKEY_BS:		STRACE("BS\n");		break;	// BackSpace
				case Keyboard::SYSKEY_SHIFT:	STRACE("Shift\n");	break;	// Shift
				case Keyboard::SYSKEY_CTRL:		STRACE("Ctrl\n");	break;	// Ctrl
				case Keyboard::SYSKEY_ALT:		STRACE("Alt\n");	break;	// Alt
				case Keyboard::SYSKEY_LEFT:		STRACE("Left\n");	break;	// ←
				case Keyboard::SYSKEY_RIGHT:	STRACE("Right\n");	break;	// →
				case Keyboard::SYSKEY_UP:		STRACE("Up\n");		break;	// ↑
				case Keyboard::SYSKEY_DOWN:		STRACE("Down\n");	break;	// ↓
				case Keyboard::SYSKEY_ESC:		STRACE("Esc\n");	break;	// Esc
				case Keyboard::SYSKEY_DEL:		STRACE("Delete\n");	break;	// Delete
				default:						STRACE("%c\n", i);	break;	// ASCII 文字
				}
			}
		}
	}

	// キャラクタステータス描画テスト
#if DEBUG_STATUS_DRAWER
	{
		static s32 damageEne = 0;
		static s32 damagePhys = 0;
		static s32 lfeneMax = 1000;
		static bool side = true;
		_cstatDbg->_debug_set(damageEne++, damagePhys++, lfeneMax += 2);
		//_cstatDbg->dianaFrame();
		/**/
		_cstatDbg->eleneStat()->veil(EleneStat::VEIL_WP)->elene()->addAethel(::rand() / 1000);
		_cstatDbg->eleneStat()->veil(EleneStat::VEIL_WP)->elene()->addWind(::rand() / 1000);
		_cstatDbg->eleneStat()->veil(EleneStat::VEIL_WP)->elene()->addFire(::rand() / 1000);
		_cstatDbg->eleneStat()->veil(EleneStat::VEIL_WP)->elene()->addWater(::rand() / 1000);
		_cstatDbg->eleneStat()->veil(EleneStat::VEIL_WP)->elene()->addSoil(::rand() / 1000);
		_cstatDbg->eleneStat()->veil(EleneStat::VEIL_BR)->elene()->addAethel(::rand() / 2000);
		_cstatDbg->eleneStat()->veil(EleneStat::VEIL_BR)->elene()->addWind(::rand() / 2000);
		_cstatDbg->eleneStat()->veil(EleneStat::VEIL_BR)->elene()->addFire(::rand() / 2000);
		_cstatDbg->eleneStat()->veil(EleneStat::VEIL_BR)->elene()->addWater(::rand() / 2000);
		_cstatDbg->eleneStat()->veil(EleneStat::VEIL_BR)->elene()->addSoil(::rand() / 2000);
		/**/
		if(side)
		{
			_cstatDbg->eleneStat()->cql()->addAethel(::rand() / 100);
			_cstatDbg->eleneStat()->cql()->addWind(::rand() / 100);
			_cstatDbg->eleneStat()->cql()->addFire(::rand() / 100);
			_cstatDbg->eleneStat()->cql()->addWater(::rand() / 100);
			_cstatDbg->eleneStat()->cql()->addSoil(::rand() / 100);
			if(_cstatDbg->eleneStat()->getCql()->getAethel() >= _cstatDbg->eleneStat()->getNql()->getAethel() * 3) { side = false; }
		}
		else
		{
			_cstatDbg->eleneStat()->cql()->subAethel(::rand() / 100);
			_cstatDbg->eleneStat()->cql()->subWind(::rand() / 100);
			_cstatDbg->eleneStat()->cql()->subFire(::rand() / 100);
			_cstatDbg->eleneStat()->cql()->subWater(::rand() / 100);
			_cstatDbg->eleneStat()->cql()->subSoil(::rand() / 100);
			if(_cstatDbg->eleneStat()->getCql()->getAethel() <= 0) { side = true; }
		}
		_cstatDbg->exec(ec);
	}
	#endif
}

/*---------------------------------------------------------------------*//**
	2D 描画
**//*---------------------------------------------------------------------*/
void ModelViewMode::draw(const RenderCtx* rc)
{
	GameModeBase::draw(rc);

	if(_amode == AMODE_FONT)
	{
		Font* font = 0L;
		GameFontSet* fontset = ((GameRenderCtx*)rc)->getFontSet();
		if(fontset != 0L)
		{
			switch(_param)
			{
			case 0: font = fontset->getFont(GameFontSet::EN); break;
			case 1: font = fontset->getFont(GameFontSet::JP); break;
			case 2: font = fontset->getFont(GameFontSet::JP_MOVIE); break;
			}
		}
		if(font != 0L)
		{
			VcString str;
			#if 1
				EasyStringDrawer::draw(font, VcString::setout(&str, "!\"#$%&'()*+,-./0123456789"), 10,  60, 16, rc);
				EasyStringDrawer::draw(font, VcString::setout(&str, "ABCDEFGHIJKLMNOPQRSTUVWXYZ"), 10,  80, 16, rc);
				EasyStringDrawer::draw(font, VcString::setout(&str, "abcdefghijklmnopqrstuvwxyz"), 10, 100, 16, rc);
				EasyStringDrawer::draw(font, VcString::setout(&str, ":;<=>?{|}~"), 10, 120, 16, rc);
				EasyStringDrawer::draw(font, VcString::setout(&str, "!\"#$%&'()*+,-./0123456789"), 10, 140, 32, rc);
				EasyStringDrawer::draw(font, VcString::setout(&str, "ABCDEFGHIJKLMNOPQRSTUVWXYZ"), 10, 176, 32, rc);
				EasyStringDrawer::draw(font, VcString::setout(&str, "abcdefghijklmnopqrstuvwxyz"), 10, 208, 32, rc);
				EasyStringDrawer::draw(font, VcString::setout(&str, ":;<=>?{|}~"), 10, 244, 32, rc);
			#else
				font->draw(10, 140, VcString::setout(&str, "y"), 32, rc);
			#endif
		}
	}

	// キャラクタステータス描画テスト
	#if DEBUG_STATUS_DRAWER
	{
		Texture* tex = Game::getGame()->getGui()->getGuiTexture();
		ASSERT(tex != 0L);
		StatusDrawer::drawCharStat(rc, tex, _cstatDbg, 100, 100, 80, 80, true, false);
	}
	#endif

	rc->getRenderer()->setSolidColor(255, 0, 0, 255);
	static f32 thetaStart = 0.0f;
	static f32 thetaEnd = 0.0f;
	RendererUtils::draw2dFillArc(rc->getRenderer(), 50, 50, 40, thetaStart, thetaEnd, 0.5f);
	thetaStart += 0.01f;
	thetaEnd += 0.02f;
	thetaStart = Calc::adjustAngle(thetaStart);
	thetaEnd = Calc::adjustAngle(thetaEnd);
}

////////////////////////////////////////////////////////////////////////////

SRCR_END_NS
