/***********************************************************************//**
 *	SoundMngr.cpp
 *	Envelopment Terras Kit
 *
 *	Created by Ryoutarou Kishi on 2009/10/09.
 *	Copyright 2009 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "SoundMngr.h"

// Friends

// External
#include "../../tfw/Tfw.h"
#include "../../tfw/collection/List.h"
#include "../../tfw/sound/Music.h"
#include "../../tfw/sound/OpenAl.h"
#include "../../tfw/sound/Se.h"
#include "../../tfw/sound/Sound.h"
#include "../../tfw/file/File.h"
#include "../../tfw/file/IdFile.h"
#include "../../tfw/file/PackedIdFile.h"
#include "../../tfw/string/VcString.h"
#include "../../tfw/tbf/TbfDefine.h"

ETK_BEGIN_NS

////////////////////////////////////////////////////////////////////////////
// 定数

// SE の最大同時発音数
#define NUM_SE_POLY_MAX			(31)
// SE の最大定義数
#define NUM_SE_DEF_MAX			(141)
// OpenAL バッファの数
#define NUM_WAVE_BUF			(144)

// SE のファイル ID 定義
#define SE_FID_UI_START			(1101010)
#define SE_FID_UI_END			(1102009)
#define SE_FID_INGAME_START		(1102010)
#define SE_FID_INGAME_END		(1200000)

#define DIST_3D_REF				(1000.0f)	// 10m
#define DIST_3D_MAX				(10000.0f)	// 100m

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// SoundMngr 定数

const f32 SoundMngr::VOLUME_BGM_DEFAULT		= 1.0f;
const f32 SoundMngr::VOLUME_UI_SE_DEFAULT	= 0.5f;
#if defined(_WINDOWS)
const f32 SoundMngr::VOLUME_3D_SE_DEFAULT	= 0.9f;
const f32 SoundMngr::VOLUME_3D_SE_MIN		= 0.0f;
const f32 SoundMngr::VOLUME_3D_SE_MAX		= 1.0f;
#else
const f32 SoundMngr::VOLUME_3D_SE_DEFAULT	= 0.5f;
const f32 SoundMngr::VOLUME_3D_SE_MIN		= 0.0f;
const f32 SoundMngr::VOLUME_3D_SE_MAX		= 0.6f;
#endif

//==========================================================================
// SoundMngr メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// アクセサ

/*---------------------------------------------------------------------*//**
	現在再生中の曲 ID を得る
**//*---------------------------------------------------------------------*/
u16 SoundMngr::getCurrentPlayMusicId() const
{
	if(_sound == 0L)	{	return 0;	}	// サウンドの構築に失敗している

	if(!_sound->getMusic()->isPlaying()) { return 0; }
	return _sound->getMusic()->getCurrentMusicId();
}

/*---------------------------------------------------------------------*//**
	予約された曲 ID を得る
**//*---------------------------------------------------------------------*/
u16 SoundMngr::getReservedMusicId() const
{
	if(_sound == 0L)	{	return 0;	}	// サウンドの構築に失敗している

	return _sound->getMusic()->getReservedMusicId();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

/*---------------------------------------------------------------------*//**
	BGM 再生

	@param id 曲 ID
	@param cntLoop ループ回数（-1 で無限ループ）
	@param volume 音量（0.0f ～ 1.0f）
	@param isForceRestart 強制的に最初から再生する
	@param ctrlkeyBgm BGM 制御キー
**//*---------------------------------------------------------------------*/
void SoundMngr::playBgm(u16 bgmid, s32 cntLoop, f32 volume, bool isForceRestart, s32 frameFadeOut, s32 frameFadeIn, s32 ctrlkeyBgm)
{
	if(_sound == 0L)	{	return;	}	// サウンドの構築に失敗している

	if(_ctrlkeyBgm != 0)	// 制御制限が有効になっている場合
	{
		if(_ctrlkeyBgm != ctrlkeyBgm)	// 制御キーが合致しない場合
		{
			return;	// 制御できなかった
		}
	}

	_sound->getMusic()->play(bgmid, volume, cntLoop, isForceRestart, frameFadeOut, frameFadeIn);
}

/*---------------------------------------------------------------------*//**
	BGM 停止
**//*---------------------------------------------------------------------*/
void SoundMngr::stopBgm(s32 frameFadeOut)
{
	if(_sound == 0L)	{	return;	}	// サウンドの構築に失敗している

	_sound->getMusic()->stop(frameFadeOut);
}

/*---------------------------------------------------------------------*//**
	BGM 一時停止
**//*---------------------------------------------------------------------*/
void SoundMngr::pauseBgm()
{
	if(_sound == 0L)	{	return;	}	// サウンドの構築に失敗している

	_sound->getMusic()->pause();
}

/*---------------------------------------------------------------------*//**
	BGM 再開（一時停止した BGM の再開）
**//*---------------------------------------------------------------------*/
void SoundMngr::resumeBgm()
{
	if(_sound == 0L)	{	return;	}	// サウンドの構築に失敗している

	_sound->getMusic()->resume();
}

/*---------------------------------------------------------------------*//**
	SE 再生
**//*---------------------------------------------------------------------*/
s32 SoundMngr::playUiSe(u16 seid, bool isLoop)
{
	if(_sound == 0L)	{	return 0;	}	// サウンドの構築に失敗している

	///TRACE("{SoundMngr::playSe} play se : %d (loop=%d)\n", seid, isLoop);
	return _sound->getSe()->playUi(seid, isLoop);
}

/*---------------------------------------------------------------------*//**
	SE 再生

	@param volume ボリューム（0.0f ～ 1.0f）
**//*---------------------------------------------------------------------*/
s32 SoundMngr::playSe(u16 seid, bool isLoop, f32 volume, const Vector3F* pos, const Vector3F* vel)
{
	if(_sound == 0L)	{	return 0;	}	// サウンドの構築に失敗している

	///TRACE("{SoundMngr::playSe} play se : %d (loop=%d)\n", seid, isLoop);
	return _sound->getSe()->play(seid, isLoop, volume, pos, vel);
}

/*---------------------------------------------------------------------*//**
	SE 停止
**//*---------------------------------------------------------------------*/
void SoundMngr::stopSe(s32 sech)
{
	if(_sound == 0L)	{	return;	}	// サウンドの構築に失敗している

	_sound->getSe()->stop(sech);
}

/*---------------------------------------------------------------------*//**
	全 SE 停止
**//*---------------------------------------------------------------------*/
void SoundMngr::stopAllSe()
{
	if(_sound == 0L)	{	return;	}	// サウンドの構築に失敗している

	_sound->getSe()->stopAll();
}

/*---------------------------------------------------------------------*//**
	リスナ設定
**//*---------------------------------------------------------------------*/
void SoundMngr::setListener(const Vector3F* pos, const Vector3F* dir)
{
	if(_sound == 0L)	{	return;	}	// サウンドの構築に失敗している

	_sound->getOpenAl()->setListenerPosition(pos);
	_sound->getOpenAl()->setListenerDirection(dir);
	_sound->getMusic()->notifyChangeListenerPosition();
}

/*---------------------------------------------------------------------*//**
	ステレオ設定
**//*---------------------------------------------------------------------*/
void SoundMngr::setStereoMusic(bool isStereo)
{
	if(_sound == 0L)	{	return;	}	// サウンドの構築に失敗している

	_isStereoMusic = isStereo;

	// 曲を再登録する
	Music* music = _sound->getMusic();
	if(music->getRegisterMusicNum() > 0)
	{
		// 曲の登録を解除する
		for(int i = 0; i < _numBgm; i++)
		{
			music->unregisterMusic(_bgmarr[i]._bgmid);
		}

		// 曲を登録する
		registerMusics();
	}
}

/*---------------------------------------------------------------------*//**
	BGM を登録する
**//*---------------------------------------------------------------------*/
bool SoundMngr::registerBgms(const BgmDef* bgmarr, s32 numBgm)
{
	_bgmarr = bgmarr;
	_numBgm = numBgm;

	return registerMusics();
}

/*---------------------------------------------------------------------*//**
	インゲーム用 SE を登録する
	※便宜上、何度呼んでもよいものとする
**//*---------------------------------------------------------------------*/
bool SoundMngr::registerSeIngame()
{
	if(_isRegIngameSe)	{	return true;	}	// 登録済み

	bool isSucceeded = registerSe(SE_FID_INGAME_START, SE_FID_INGAME_END);

	_isRegIngameSe = true;	// 成否に関わらず１回のみの呼び出し
	return isSucceeded;
}

/*---------------------------------------------------------------------*//**
	制御制限を設定する

	@param ctrlkeyBgm BGM 制御キー（0 は制御制限無効化）
**//*---------------------------------------------------------------------*/
void SoundMngr::restrictControlBgm(s32 ctrlkeyBgm)
{
	_ctrlkeyBgm = ctrlkeyBgm;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
SoundMngr::SoundMngr()
	: _sound(0L)
	, _isStereoMusic(true)		// デフォルトではステレオ
	, _isRegIngameSe(false)
	, _ctrlkeyBgm(0)
	, _bgmarr(0L)
	, _numBgm(0)
{
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
SoundMngr::~SoundMngr()
{
	ASSERT(_sound == 0L);
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool SoundMngr::create()
{
	// SE を再生時に読み込むかどうか
	const bool isSeLoadOnPlay = true;

	// サウンドの作成
	Sound* sound = new Sound();
	if(!sound->create(NUM_SE_POLY_MAX, NUM_SE_DEF_MAX, NUM_WAVE_BUF, isSeLoadOnPlay))
	{
		delete sound;
		//ASSERTM(false, "Sound::create failed.");
		return false;
	}
	_sound = sound;

	// 曲を登録する
	registerMusics();

	// UI 用の SE を登録する
	registerSeUi();

	// BGM 設定
	_sound->getMusic()->setVolume(VOLUME_BGM_DEFAULT);
	// SE 設定
	_sound->getSe()->setUiVolume(VOLUME_UI_SE_DEFAULT);
	_sound->getSe()->setStandardDistance(DIST_3D_REF, DIST_3D_MAX);
	_sound->getSe()->setVolume(VOLUME_3D_SE_DEFAULT, VOLUME_3D_SE_MIN, VOLUME_3D_SE_MAX);

	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void SoundMngr::destroy()
{
	// サウンドの削除
	if(_sound != 0L)
	{
		_sound->destroy();
		delete _sound;
		_sound = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	曲を登録する
**//*---------------------------------------------------------------------*/
bool SoundMngr::registerMusics()
{
	if(_sound == 0L)	{	return false;	}	// サウンドの構築に失敗している

	VcString pathBase(File::getBasePath());
	#if defined(_DEBUG) && defined(_WINDOWS)
		pathBase.add("\\Sound");
	#endif

	Music* music = _sound->getMusic();
	ASSERT(music != 0L);
	for(int i = 0; i < _numBgm; i++)
	{
		#if defined(_IOS) || defined(_ANDROID) || defined(_WINDOWS)
		{
			VcString strFileName, cstrType;
			#if defined(_IOS)
				cstrType.set("mp4");
			#elif defined(_ANDROID) || defined(_WINDOWS)
				cstrType.set("mp3");
			#endif
			music->registerMusic(_bgmarr[i]._bgmid, &pathBase, VcString::format(&strFileName, "Bgm%d_%ss", _bgmarr[i]._bgmid, _isStereoMusic ? "s" : "m"), &cstrType, 0, S32_MAX);
		}
		#else
		{
			music->registerMusic(_bgmarr[i]._bgmid, _bgmarr[i]._fileid, WaveFormat::RIFF, _bgmarr[i]._smplLoopStart, _bgmarr[i]._smplLoopEnd);
			///music->registerMusic(_bgmarr[i]._bgmid, FILEID_FROM_BGM(_bgmarr[i]._bgmid, _isStereoMusic, false), WaveFormat::RIFF, _bgmarr[i]._smplLoopStart, _bgmarr[i]._smplLoopEnd);
			///music->registerMusic(GameSoundDef::_bgms[i]._bgmid, FILEID_FROM_BGM(GameSoundDef::_bgms[i]._bgmid, _isStereoMusic, true), WaveFormat::CAF, GameSoundDef::_bgms[i]._smplLoopStart, GameSoundDef::_bgms[i]._smplLoopEnd);
		}
		#endif
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	UI 用 SE を登録する
**//*---------------------------------------------------------------------*/
bool SoundMngr::registerSeUi()
{
	return registerSe(SE_FID_UI_START, SE_FID_UI_END);
}

/*---------------------------------------------------------------------*//**
	SE を登録する
**//*---------------------------------------------------------------------*/
bool SoundMngr::registerSe(u32 fileidRangeStart, u32 fileidRangeEnd)
{
	if(_sound == 0L)	{	return false;	}	// サウンドの構築に失敗している

	Se* se = _sound->getSe();
	const PackedIdFile* pidf = IdFile::getPackedIdFile();
	if(pidf == 0L)		{	return false;	}
	const List<TbfIncludeIdFile*>* list = pidf->getList(); ASSERT(list != 0L);

	// 範囲内の SE を登録する
	for(ListIterator<TbfIncludeIdFile*> it = list->iterator(); it.has(); it.next())
	{
		const TbfIncludeIdFile* incf = it.object();
		if((fileidRangeStart <= incf->_fileid) && (incf->_fileid <= fileidRangeEnd))
		{
			s32 platform = incf->_fileid % 10;
			u16 seid = (incf->_fileid - 1000000) / 10;
			switch(platform)
			{
			case 1:	// Windows
				#if defined(_WINDOWS) || defined(_ANDROID)
				{
					se->registerSe(seid, incf->_fileid, WaveFormat::RIFF);
				}
				#endif
				break;
			case 2:	// iOS
				#if defined(_IOS)
				{
					se->registerSe(seid, incf->_fileid, WaveFormat::CAF);
				}
				#endif
				break;
			}
		}
		else if(incf->_fileid > fileidRangeEnd)
		{
			break;
		}
	}

	return true;
}

/*---------------------------------------------------------------------*//**
	フレーム処理
**//*---------------------------------------------------------------------*/
void SoundMngr::exec(const ExecCtx* ec)
{
	if(_sound != 0L)	{	_sound->exec(ec);	}
}

////////////////////////////////////////////////////////////////////////////

ETK_END_NS
