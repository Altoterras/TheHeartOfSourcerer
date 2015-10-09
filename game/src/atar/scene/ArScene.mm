/***********************************************************************//**
 *	ArScene.cpp
 *	Enlight AtAr Application
 *
 *	Created by Ryoutarou Kishi on 2013/06/05.
 *	Copyright 2013 Altoterras Corporation. All rights reserved.
 *
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Base

// Self
#include "ArScene.h"

// Friends
#include "ArScreen.h"
#include "../body/AtArBody.h"
#include "../body/LoadingScreen.h"
#include "../body/MaterialManager.h"
#include "../body/MovieFileTextureIos.h"
#include "../body/VideoCameraIos.h"
#include "../camera/ArCam.h"
#include "../file/AtArFileIdDef.h"
#include "../gui/AtArGui.h"
#include "../gui/IngameFront.h"
#import "../../ios/EnlightAppDelegate.h"
#include "../../ios/OsDepModIos.h"

// External
#include "../../etk/body/EtkExecCtx.h"
#include "../../etk/body/OsDepMod.h"
#include "../../etk/sg/CamSgnc.h"
#include "../../etk/sg/SorSgm.h"
#include "../../tfw/Tfw.h"
#include "../../tfw/debug/ScreenTrace.h"
#include "../../tfw/file/File.h"
#if defined(_ANDROID)
#include "../../tfw/file/FileAndroidAsset.h"
#endif
#include "../../tfw/g3d/sg/node/implement/ModelLoader.h"
#include "../../tfw/g3d/sg/node/implement/ShapeModel.h"
#include "../../tfw/gcmn/Gcalc.h"
#include "../../tfw/gcmn/Renderer.h"
#include "../../tfw/gcmn/RendererUtils.h"
#include "../../tfw/gcmn/View.h"
#include "../../tfw/gui/Panel.h"
#include "../../tfw/image/PngImg.h"
#include "../../tfw/lib/Rect.h"
#include "../../tfw/lib/Color.h"
#include "../../tfw/string/VcString.h"
#include "../../tfw/ui/TouchPanel.h"
#include "../../tfw/ui/MotionController.h"

// Library
#import <AudioToolbox/AudioServices.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#import <QuartzCore/CALayer.h>

#if defined(assert)
	#undef assert
#endif

////////////////////////////////////////////////////////////////////////////
// マクロ

ATAR_BEGIN_NS

#define JUDGE_GRAY				(1)				// グレースケールで判定する
#define FCNT_PREF_MAX			(30.0f * 2)		// 演出フレームの長さ
#define NUM_FEATURES			(180)			// 抽出ポイント数
#define TEST_RETRY				(0)				// リトライ画面のテスト
#define CNT_DESTRUCTION_MAX		(4)				// 破壊カウンタの最大値
#define FCNT_DEST_PERF_UI		(30.0f)			// 破壊演出フレーム数
#define FCNT_DEST_MAX			(30.0f * 10.0f)	// 破壊演出フレーム最大数

// テクスチャ表示位置
#define RECT_TAP_YES_MOTION		(EtkBody::getBody()->getLogicalWidth() * 3.0f / 8.0f) - 22.0f, (EtkBody::getBody()->getLogicalHeight() * 3.0f / 4.0f) - 44.44f, 44.0f, 88.88f
#define RECT_SHAKE_YES_MOTION	(EtkBody::getBody()->getLogicalWidth() * 5.0f / 8.0f) - 36.0f, (EtkBody::getBody()->getLogicalHeight() * 3.0f / 4.0f) -  34.3f, 72.0f, 68.6f
#define RECT_TAP_NO_MOTION		(EtkBody::getBody()->getLogicalWidth() * 0.5f) - 22.0f, (EtkBody::getBody()->getLogicalHeight() * 3.0f / 4.0f) - 44.44f, 44.0f, 88.88f
#define RECT_BACK			5, 3, 48, 48

// テクスチャ UV 値
#define W_GUI_TEX			(1024.0f)
#define H_GUI_TEX			(1024.0f)
#define UV_TAP				404.0f, 404.0f, 220.0f, 444.0f
#define UV_SHAKE			654.0f, 404.0f, 360.0f, 343.0f
#define UV_BACK_N			0.0f,   0.0f, 200.0f, 200.0f
#define UV_BACK_H			0.0f, 200.0f, 200.0f, 200.0f

////////////////////////////////////////////////////////////////////////////
// クラス

//==========================================================================
// MovieFileTextureIosDelegater クラス

ATAR_END_NS

/*---------------------------------------------------------------------*//**
 * ビデオカメラデリゲートクラス
 *
**//*---------------------------------------------------------------------*/
@interface ArSceneDelegater : NSObject <UIAlertViewDelegate>
{
	atar::ArScene* _own;
	UIAlertView* _alert;
}

@property atar::ArScene* own;
@property UIAlertView* alert;

@end

@implementation ArSceneDelegater

@synthesize own = _own;
@synthesize alert = _alert;

/*---------------------------------------------------------------------*//**
	アラートビューのボタンクリックハンドラ
**//*---------------------------------------------------------------------*/
- (void)alertView:(UIAlertView*)alertView didDismissWithButtonIndex:(NSInteger)index
{
	if(_own != nil)
	{
		_own->onClickAlertButton(index);
	}
	_alert = nil;
}

@end	// @implementation ArSceneDelegater

ATAR_BEGIN_NS

//==========================================================================
// ArSceneInner クラス

/*---------------------------------------------------------------------*//**
 * Objective-C クラス内包クラス
 *
**//*---------------------------------------------------------------------*/
class ArSceneInner
{
public:
	ArSceneDelegater* _delg;
	
	ArSceneInner(ArScene* own)
		: _delg(nil)
	{
		_delg = [[ArSceneDelegater alloc] init];
		_delg.own = own;
	}

	~ArSceneInner()
	{
		_delg.own = nil;
		
		// 終了時にアラートダイアログがまだ出ているようなのであれば、消しておく
		if(_delg.alert != nil)
		{
			[_delg.alert dismissWithClickedButtonIndex:0 animated:NO];
		}
	
		_delg = nil;
	}
};

//==========================================================================
// ArSys クラス

/*---------------------------------------------------------------------*//**
 *	AR システム
 *
**//*---------------------------------------------------------------------*/
class ArRender : public SgNode
{
	//======================================================================
	// 変数
public:
	ArScene* _own;
	
	ArRender(ArScene* own)
	{
		_own = own;
	}
	
	bool create()
	{
		if(!SgNode::create(SorSgm::SGNTYPE_DIRECT))
		{
			return false;
		}
		setGeneralParam(SorSgm::EK_OPCTY_NOFOG);
		return true;
	}
	
	void destroy()
	{
	}

	void render(const RenderCtx* rc)
	{
		_own->renderBySg(rc);
	}
};

/*---------------------------------------------------------------------*//**
 *	AR システム
 *
**//*---------------------------------------------------------------------*/
class ArSys
{
	//======================================================================
	// クラス
public:
	/*-----------------------------------------------------------------*//**
	 *	AR 学習画像
	 *
	**//*-----------------------------------------------------------------*/
	class ArTrain
	{
	public:
		VcString _id;
		PngImg _png;
		cv::Mat _mtxDescriptors;
		cv::vector<cv::KeyPoint> _vkpKeypoints;
		cv::vector<cv::DMatch> _matches;
		f32 _distanceThreshold;
		s32 _vote;	// 認識ポイント
		s32 _hold;	// ホールドカウント
		
		ArTrain()
			: _distanceThreshold(0.0f)
			, _vote(0)
			, _hold(0)
		{
		}
	};
	
	/*-----------------------------------------------------------------*//**
	 *	AR 議題画像
	 *
	**//*-----------------------------------------------------------------*/
	class ArQuery
	{
	public:
		cv::Mat _mtxDescriptors;
		cv::vector<cv::KeyPoint> _vkpKeypoints;
		
		ArQuery()
		{
		}
	};

	//======================================================================
	// 定数
private:
	static const s32 HOLD_MAX	= 2;
	static const s32 NOVOTE_MAX	= 3;

	//======================================================================
	// 変数
public:
	cv::OrbFeatureDetector* _detector;				// ORB 特徴点検出器
	cv::OrbDescriptorExtractor* _extractor;			// ORB 特徴量抽出機
	
	ArTrain* _trains;			// 学習画像配列
	s32 _numTrain;				// 学習画像数
	ArQuery* _query;			// 対象画像
	
	s32 _idxMatchedTrain;		// 一致した学習画像
	s32 _idxCandidatedTrain;	// 候補の学習画像

	s32 _cntRecognition;		// 認識カウンタ
	s32 _cntNoVote;				// 無投票カウンタ

	//======================================================================
	// メソッド
public:
	/*-----------------------------------------------------------------*//**
		コンストラクタ
	**//*-----------------------------------------------------------------*/
	ArSys()
		: _detector(0L)
		, _extractor(0L)
		, _trains(0L)
		, _numTrain(0)
		, _query(0L)
		, _idxMatchedTrain(-1)
		, _idxCandidatedTrain(-1)
		, _cntRecognition(0)
		, _cntNoVote(0)
	{
	}
	
	/*-----------------------------------------------------------------*//**
		デストラクタ
	**//*-----------------------------------------------------------------*/
	~ArSys()
	{
		ASSERT(_detector == 0L);
		ASSERT(_extractor == 0L);
		ASSERT(_trains == 0L);
		ASSERT(_query == 0L);
	}
	
	/*-----------------------------------------------------------------*//**
		作成
	**//*-----------------------------------------------------------------*/
	bool create(MaterialManager* mtrlmng, s32 numFeatures)
	{
		// ORB 特徴点検出器・特徴量抽出機の作成
		_detector = new cv::OrbFeatureDetector(numFeatures);
		_extractor = new cv::OrbDescriptorExtractor();
		
		// AR 素材数をカウント
		s32 cntAr = 0;
		for(int i = 0; i < mtrlmng->getArMaterialMaxNum(); i++)
		{
			const MaterialManager::ArMaterialInfo* ari = mtrlmng->getArMaterial(i);
			if(ari->_id->getLength() <= 0)
			{
				continue;
			}
			cntAr++;
		}
		
		// 学習画像配列作成
		_numTrain = cntAr;
		_trains = new ArTrain[_numTrain];
		
		// 対象画像の作成
		_query = new ArQuery();
		
		// ドキュメントパス取得
		VcString pathDocs;
		EtkBody::getBody()->getOsDepMod()->getDocumentDirPath(&pathDocs, false);

		// 学習画像の読み込み
		cntAr = 0;
		for(int i = 0; i < mtrlmng->getArMaterialMaxNum(); i++)
		{
			const MaterialManager::ArMaterialInfo* ari = mtrlmng->getArMaterial(i);
			const VcString* id = ari->_id;
			if((id == 0L) || (id->getLength() <= 0))
			{
				continue;
			}
			ArTrain* train = &_trains[cntAr];
			
			// 学習画像の PNG を読み込む
			VcString pathPng(pathDocs);
			pathPng += *id + ".png";
			u16 cflagPng = PngImg::CFLAG_SAVE_BITS;
			#if defined(_DEBUG)
				 cflagPng |= PngImg::CFLAG_GLTEXIMAGE;
			#endif
			if(!train->_png.create(&pathPng, cflagPng))
			{
				continue;
			}
			
			// OpenCV に学習させる
			int width = (int)train->_png.getWidth();
			int height = (int)train->_png.getHeight();
			u8* bits = train->_png.getBits();
			u32 bpp = train->_png.getBitPerPixel();
			try
			{
				int type = CV_8UC4;
				switch(bpp)
				{
				case 32:	type = CV_8UC4;	break;
				case 24:	type = CV_8UC3;	break;
				case 16:	type = CV_8UC2;	break;
				case 8:		type = CV_8UC1;	break;
				default:	ASSERT(false);	continue;	// 対応できないフォーマット
				}
				cv::Mat mtxRgba(height, width, type, bits);
				#if JUDGE_GRAY
					cv::Mat mtxGray(height, width, CV_8UC1);
					cv::cvtColor(mtxRgba, mtxGray, CV_RGBA2GRAY);
					_detector->detect(mtxGray, train->_vkpKeypoints);
					_extractor->compute(mtxGray, train->_vkpKeypoints, train->_mtxDescriptors);
				#else
					_detector->detect(mtxRgba, train->_vkpKeypoints);
					_extractor->compute(mtxRgba, train->_vkpKeypoints, train->_mtxDescriptors);
				#endif
			}
			catch(cv::Exception ex)
			{
				ex.formatMessage();
				TRACELN(ex.msg.c_str());
			}
			train->_png.deleteBits();
			
			// 情報のコピー
			train->_id = *id;
			train->_distanceThreshold = ari->_distanceThreshold;
			
			cntAr++;
		}
	
		// その他の初期化
		_idxMatchedTrain = -1;
		_idxCandidatedTrain = -1;
		_cntRecognition = 0;
		
		return true;
	}
	
	/*-----------------------------------------------------------------*//**
		破棄
	**//*-----------------------------------------------------------------*/
	void destroy()
	{
		delete _query;
		_query = 0L;
	
		if(_trains != 0L)
		{
			delete[] _trains;
			_trains = 0L;
		}
		_numTrain = 0;
		
		delete _detector;
		_detector = 0L;
		
		delete _extractor;
		_extractor = 0L;
	}
	
	/*-----------------------------------------------------------------*//**
		認識処理
	**//*-----------------------------------------------------------------*/
	void recognize(u8* bitsQueryBmp, s32 widthQueryBmp, s32 heightQueryBmp)
	{
		// 認識
		if(_cntRecognition < _numTrain)	// 認識フレーム
		{
			// OpenCV による画像の解析
			////TRACELN("{ArSys::recognize} begin opencv proc --- 2 ---");
			cv::Mat mtxBgra(heightQueryBmp, widthQueryBmp, CV_8UC4, bitsQueryBmp);
			#if JUDGE_GRAY
				cv::Mat mtxGray(heightQueryBmp, widthQueryBmp, CV_8UC1);
				cv::cvtColor(mtxBgra, mtxGray, CV_BGRA2GRAY, 1);
				_detector->detect(mtxGray, _query->_vkpKeypoints);
				_extractor->compute(mtxGray, _query->_vkpKeypoints, _query->_mtxDescriptors);
			#else
				cv::Mat mtxRgba(heightQueryBmp, widthQueryBmp, CV_8UC4);
				cv::cvtColor(mtxBgra, mtxRgba, CV_BGRA2RGBA, 0);
				_detector->detect(mtxRgba, _query->_vkpKeypoints);
				_extractor->compute(mtxRgba, _query->_vkpKeypoints, _query->_mtxDescriptors);
			#endif
			////TRACELN("{ArSys::recognize} end opencv proc --- 2 ---");
	
			// 画像の認識処理
			ArTrain* train = &_trains[_cntRecognition];
			train->_matches.clear();
			cv::BFMatcher matcher(cv::NORM_HAMMING, true);
			try
			{
				matcher.match(_query->_mtxDescriptors, train->_mtxDescriptors, train->_matches);
			}
			catch(cv::Exception ex)
			{
				ex.formatMessage();
				TRACELN(ex.msg.c_str());
			}
			
			// 認識ポイントの計算
			train->_vote = 0;
			for(int im = 0; im < train->_matches.size(); im++)
			{
				if(train->_matches[im].distance < train->_distanceThreshold)
				{
					train->_vote++;
				}
			}
		}
		
		// 判定
		if(_cntRecognition == (_numTrain - 1))	// 最終認識フレーム
		{
			// 最も認識ポイントが大きかったものを採用
			s32 idxImageMax = -1;
			s32 votesMax = 0;
			for(int itrain = 0; itrain < _numTrain; itrain++)
			{
				if(_trains[itrain]._vote > votesMax)
				{
					idxImageMax = itrain;
					votesMax = _trains[itrain]._vote;
				}
			}
			// マッチした特徴点の数が全体の 5% より少なければ未検出とする
			if(idxImageMax != -1)
			{
				int r = _trains[idxImageMax]._mtxDescriptors.rows;
				f32 similarity = votesMax / (f32)_trains[idxImageMax]._mtxDescriptors.rows * 100.0f;
				if(similarity < 5.0f)
				{
					idxImageMax = -1;
				}
			}
			
			if(idxImageMax != -1)
			{
				_trains[idxImageMax]._hold++;	// ホールドカウントを増加
				
				// ホールドカウントを達成したものを採用
				idxImageMax = -1;
				s32 holdMax = 0;
				for(int itrain = 0; itrain < _numTrain; itrain++)
				{
					if(_trains[itrain]._hold > holdMax)
					{
						holdMax = _trains[itrain]._hold;
						idxImageMax = itrain;
						break;
					}
				}
				if(idxImageMax != -1)
				{
					_idxCandidatedTrain = idxImageMax;
					if(holdMax >= HOLD_MAX)
					{
						_idxMatchedTrain = idxImageMax;
					}
				}
				
				// 無投票カウンタのリセット
				_cntNoVote = 0;
			}
			else
			{
				// 無投票カウンタによるホールドリセット
				_cntNoVote++;
				if(_cntNoVote == NOVOTE_MAX)
				{
					for(int itrain = 0; itrain < _numTrain; itrain++)
					{
						_trains[itrain]._hold = 0;
					}
				}
			}
		}

		// 次の認識へ
		_cntRecognition++;
		if(_cntRecognition >= _numTrain)
		{
			_cntRecognition = 0;
		}
	}

	/*-----------------------------------------------------------------*//**
		認識結果のクリア
	**//*-----------------------------------------------------------------*/
	void resetRecognizedResult()
	{
		for(int itrain = 0; itrain < _numTrain; itrain++)
		{
			_trains[itrain]._vote = 0;
			_trains[itrain]._hold = 0;
		}
		_idxMatchedTrain = -1;
		_idxCandidatedTrain = -1;
	}
};

//==========================================================================
// ArScene メソッド

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 外部サービス

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 内部制御

/*---------------------------------------------------------------------*//**
	コンストラクタ
**//*---------------------------------------------------------------------*/
ArScene::ArScene()
	: _inner(0L)
	, _arsys(0L)
	, _screen(0L)
	, _rnode(0L)
	, _hasMotionDevice(false)
	, _phase(PHASE_NULL)
	, _ecntPhase(0)
	, _fcntPerfFade(0.0f)
	, _cntDestruction(0)
	, _fcntPrefDestruction(0.0f)
	, _idxMatchedTrainResult(-1)
	, _camMvRef(0L)
	, _mdlarrRef(0L)
	, _numMdl(0)
	, _animid(0)
{
	for(int i = 0; i < NUM_BTN; i++)
	{
		_pnlButtons[i] = 0L;
	}
}

/*---------------------------------------------------------------------*//**
	デストラクタ
**//*---------------------------------------------------------------------*/
ArScene::~ArScene()
{
	ASSERT(_inner == 0L);
	ASSERT(_arsys == 0L);
	ASSERT(_screen == 0L);
	ASSERT(_rnode == 0L);
	ASSERT(_mdlarrRef == 0L);
	#if defined(_DEBUG)
		for(int i = 0; i < NUM_BTN; i++)
		{
			ASSERT(_pnlButtons[i] == 0L);
		}
	#endif
}

/*---------------------------------------------------------------------*//**
	作成
**//*---------------------------------------------------------------------*/
bool ArScene::create(Texture* texGuiRef)
{
	RectS32 rect;
	RectF uv1, uv2;

	// 内包クラスの作成
	_inner = new ArSceneInner(this);

	// AR システムの作成
	_arsys = new ArSys();
	
	// AR スクリーンの作成
	_screen = new ArScreen();
	if(!_screen->create())
	{
		ASSERT(false);
		delete _screen;
		_screen = 0L;
	}
	
	// 描画システムの作成
	_rnode = new ArRender(this);
	if(!_rnode->create())
	{
		ASSERT(false);
		delete _rnode;
		_rnode = 0L;
	}

	// ボタンの作成
	if((_pnlButtons[BTN_BACK] = createButton(
		texGuiRef,
		RectS32::setout(&rect, RECT_BACK),
		Gcalc::calcTexUv(&uv1, UV_BACK_N, W_GUI_TEX, H_GUI_TEX, 1.0f),
		Gcalc::calcTexUv(&uv2, UV_BACK_H, W_GUI_TEX, H_GUI_TEX, 1.0f))) == 0L)
	{	ASSERT(false);	return false;	}
	
	// モーションデバイスが有効かを判定する
#if 0
	#if defined(_DEBUG)
		bool b1 = OsDepModIos::isAvailableMotionDevice();
		bool b2 = OsDepModIos::isAvailableAccelerometerDevice();
		bool b3 = OsDepModIos::isAvailableMagnetometerDevice();
		bool b4 = OsDepModIos::isAvailableGyroDevice();
		TRACELN("MotionDeviceAvalability: motion=%d, accelerometer=%d, magnetometer=%d, gyro=%d", b1, b2, b3, b4);
	#endif
	_hasMotionDevice = OsDepModIos::isAvailableMotionDevice();
#else
	_hasMotionDevice = true;
#endif
	
	return true;
}

/*---------------------------------------------------------------------*//**
	破棄
**//*---------------------------------------------------------------------*/
void ArScene::destroy()
{
	destroyScene();
	
	// ボタンの削除
	for(int i = 0; i < NUM_BTN; i++)
	{
		if(_pnlButtons[i] != 0L)
		{
			_pnlButtons[i]->destroy();
			delete _pnlButtons[i];
			_pnlButtons[i] = 0L;
		}
	}

	// 描画システムの削除
	if(_rnode != 0L)
	{
		_rnode->destroy();
		delete _rnode;
		_rnode = 0L;
	}
	
	// AR スクリーンの削除
	if(_screen != 0L)
	{
		_screen->destroy();
		delete _screen;
		_screen = 0L;
	}

	// AR システムの削除
	delete _arsys;
	_arsys = 0L;

	// 内包クラスの削除
	delete _inner;
	_inner = 0L;
}

/*---------------------------------------------------------------------*//**
	シーン開始
**//*---------------------------------------------------------------------*/
void ArScene::beginScene(void* param)
{
	TRACELN("{ArScene::beginScene}");
	if(!createScene())
	{
		ASSERT(false);
	}
}

/*---------------------------------------------------------------------*//**
	シーン終了
**//*---------------------------------------------------------------------*/
void ArScene::endScene(void* param)
{
	TRACELN("{ArScene::endScene}");
	destroyScene();
}

/*---------------------------------------------------------------------*//**
	フレーム制御
**//*---------------------------------------------------------------------*/
void ArScene::exec(ExecRes* res, const ExecCtx* ec)
{
	EtkExecCtx* eec = (EtkExecCtx*)ec;
	
	// 他 GUI 表示中は何もしない
	if(eec->isOpenedLockGui())
	{
		VideoCameraIos* vcam = ((AtArBody*)AtArBody::getBody())->getVideoCamera();
		if(vcam->isCapturing())
		{
			vcam->stopCapture();	// キャプチャを止める
		}
		return;	// 抜ける
	}
	
	// ボタン UI 応答
	if(_phase == PHASE_AR)
	{
		TouchPanel* touchp = eec->getTouchPanel();
		// タッチをボタンに伝達
		PointS32 ptN, ptS;
		bool isActive = (touchp->getTouchingNum() == 1) && !touchp->isReacted();	// シングル
		touchp->getTouchPosition(&ptN);
		touchp->getTouchStartPosition(&ptS);
		for(int i = 0; i < NUM_BTN; i++)
		{
			if(_pnlButtons[i] != 0L)
			{
				_pnlButtons[i]->hit(&ptN, &ptS, isActive);
			}
		}

		// 操作
		if(touchp->isTapRelease(&ptN, &ptS) && !touchp->isReacted())
		{
			for(int i = 0; i < NUM_BTN; i++)
			{
				if(_pnlButtons[i] != 0L)
				{
					if(_pnlButtons[i]->hitTest(&ptN) && _pnlButtons[i]->hitTest(&ptS))
					{
						switch(i)
						{
						case BTN_BACK:
							// シーン移動
							res->setGeneralValue1(AtArBody::SCENE_TITLE);
							res->setDone();
							touchp->setReacted(true);
							break;
						}

						touchp->setReacted(true);	// UI 反応済み
						break;
					}
				}
			}
		}
	}

	// ボタンのフレーム処理
	for(int i = 0; i < NUM_BTN; i++)
	{
		if(_pnlButtons[i] == 0L)	{	continue;	}
		_pnlButtons[i]->exec(res, ec);
	}

#if defined(_DEBUG)
	#if 1	// モデルのアニメーション切り替え
		TouchPanel* touchp = eec->getTouchPanel();
		if(!touchp->isReacted() && touchp->isTapRelease(0L, 0L))
		{
			if(_numMdl > 0)
			{
				_animid++;
				bool isValidAnimId = false;
				for(int cnt = 1; cnt <= 2; cnt++)
				{
					for(int i = 0; i < _numMdl; i++)
					{
						if(_animid <= _mdlarrRef[i]->getMaxAnimId())
						{
							_mdlarrRef[i]->setAnimId(_animid, 30, false, true, true, false, false);
							isValidAnimId = true;
						}
					}
					if(isValidAnimId)	{	break;	}
					_animid = 1;
				}
				touchp->setReacted(true);
			}
		}
	#endif
#endif
	
	// フェイズ処理
	Phase phaseBk = _phase;
	switch(_phase)
	{
	case PHASE_AR:
		{
			if(_ecntPhase == 0)
			{
				// 3D 空間におけるスクリーン座標を算出する
				_screen->calcScreen3dVector();
			}
		
			// AR 認識
			VideoCameraIos* vcam = ((AtArBody*)AtArBody::getBody())->getVideoCamera();
			if(vcam->isCapturing())
			{
				u8* bitsCam = vcam->getBitmapBits();
				s32 widthCam = vcam->getWidth();
				s32 heightCam = vcam->getHeight();
				_arsys->recognize(bitsCam, widthCam, heightCam);
			}
			else
			{
				vcam->startCapture();	// キャプチャを再開する
			}

			// 認識による破壊カウンタを進める
			MovieFileTextureIos* mtex = ((AtArBody*)AtArBody::getBody())->getMovieFileTexture();
			if(!mtex->isPlaying() && (_arsys->_idxMatchedTrain != -1))
			{
				if(_cntDestruction == 0)
				{
					_screen->changeStatus(ArScreen::STAT_DIVIDED);
				}
				_cntDestruction++;
				_fcntPrefDestruction = 0.0f;	// 破壊演出開始
				_idxMatchedTrainResult = _arsys->_idxMatchedTrain;
				_arsys->resetRecognizedResult();
				_screen->advanceDivision((f32)_cntDestruction * _cntDestruction);
				AudioServicesPlaySystemSound(kSystemSoundID_Vibrate);	// バイブレーションをふるわせる
			}
			else if(_fcntPrefDestruction >= FCNT_DEST_MAX)
			{
				// あまりに長く進行していないと認識をリセットする
				_arsys->resetRecognizedResult();	// 認識結果リセット
				_idxMatchedTrainResult = -1;		// 最終認識結果リセット
				_cntDestruction = 0;				// 破壊カウンタリセット
				_fcntPrefDestruction = -1.0f;		// 破壊演出リセット
				
				// AR スクリーンの状態変更
				_screen->changeStatus(ArScreen::STAT_ATTRACT_ANIM);
			}

			// ビデオ再生へ
			if((_cntDestruction >= CNT_DESTRUCTION_MAX) && (_idxMatchedTrainResult != -1))
			{
				TRACELN("{ArScene::exec} play video: %d, %s", _idxMatchedTrainResult, _arsys->_trains[_idxMatchedTrainResult]._id.getRaw());
				
				// ビデオカメラ停止
				vcam->stopCapture();
				
				// 演出開始
				_screen->changeStatus(ArScreen::STAT_SPREAD_ANIM);
				
				// バイブレーションを一回ふるわせる
				///AudioServicesPlaySystemSound(kSystemSoundID_Vibrate);

				#if DOWNLOAD_MOV_FIRSTLOAD
				{
					// ムービー再正準備
					prepareMatchedMoviePlay();
					
					// 次のフェイズへ
					_phase = PHASE_MOVIE_START;
				}
				#else
				{
					MaterialManager* mtrlmng = ((AtArBody*)AtArBody::getBody())->getMaterialManager();
					if(mtrlmng->isMovieFileExists(_idxMatchedTrainResult))	// 既に映像ファイルが存在している場合
					{
						// ムービー再正準備
						prepareMatchedMoviePlay();
					
						// 次のフェイズへ
						_phase = PHASE_MOVIE_START;
					}
					else if(startMatchedMovieDownload())
					{
						// 次のフェイズへ
						_phase = PHASE_MOVIE_DOWNLOADING;
					}
					else	// ダウンロード開始にしっぱいした
					{
						// リトライするかをユーザーに尋ねる
						_inner->_delg.alert = [[UIAlertView alloc] initWithTitle:@"" message:NSLocalizedString(@"msg_retry_material_download", nil) delegate:_inner->_delg cancelButtonTitle:NSLocalizedString(@"button_retry", nil) otherButtonTitles:nil ];
						[_inner->_delg.alert addButtonWithTitle:NSLocalizedString(@"button_cancel", nil)];
						[_inner->_delg.alert show];
					
						// 次のフェイズへ
						_phase = PHASE_MOVIE_ASK_RETRY;
					}
				}
				#endif
			}
		}
		break;
	case PHASE_MOVIE_DOWNLOADING:
		{
			MaterialManager* mtrlmng = ((AtArBody*)AtArBody::getBody())->getMaterialManager();
			switch(mtrlmng->getStatus())
			{
#if !TEST_RETRY
			case MaterialManager::STATUS_SUCCEEDED_DOWNDLOAD:
				{
					// ローディングスクリーン終了
					LoadingScreen* ldscr = ((AtArBody*)AtArBody::getBody())->getLoadingScreen();
					ldscr->end();
					
					// ムービー再正準備
					prepareMatchedMoviePlay();
					
					// 次のフェイズへ
					_phase = PHASE_MOVIE_START;
				}
				break;
#else
			case MaterialManager::STATUS_SUCCEEDED_DOWNDLOAD:
#endif
			case MaterialManager::STATUS_FAILED_DOWNDLOAD:
				{
					// ローディングスクリーン終了
					LoadingScreen* ldscr = ((AtArBody*)AtArBody::getBody())->getLoadingScreen();
					ldscr->end();
					
					// リトライするかをユーザーに尋ねる
					_inner->_delg.alert = [[UIAlertView alloc] initWithTitle:@"" message:NSLocalizedString(@"msg_retry_material_download", nil) delegate:_inner->_delg cancelButtonTitle:NSLocalizedString(@"button_retry", nil) otherButtonTitles:nil ];
					[_inner->_delg.alert addButtonWithTitle:NSLocalizedString(@"button_cancel", nil)];
					[_inner->_delg.alert show];
					
					// 次のフェイズへ
					_phase = PHASE_MOVIE_ASK_RETRY;
				}
				break;
			}
		}
		break;
	case PHASE_MOVIE_START:
		{
			MovieFileTextureIos* mtex = ((AtArBody*)AtArBody::getBody())->getMovieFileTexture();
			// ムービービットマップ生成
			if(_ecntPhase == 0)
			{
				mtex->generateMovieBitmap();
			}
			// フェードカウンタを進める
			_fcntPerfFade += ec->getDeltaFrame();
			// ムービー再生開始
			if((_fcntPerfFade >= FCNT_PREF_MAX) && mtex->isPrepared())
			{
				// シーン移動
				res->setGeneralValue1(AtArBody::SCENE_MOVIE);
				res->setGeneralValue2(_idxMatchedTrainResult);
				res->setDone();
			}
		}
		break;
	default:
		break;
	}
	_ecntPhase++;
	if(phaseBk != _phase)
	{
		_ecntPhase = 0;
	}

	// 破壊演出カウンタのインクリメント
	if(_fcntPrefDestruction >= 0.0f)
	{
		_fcntPrefDestruction += ec->getDeltaFrame();
	}

	// スクリーンの処理
	_screen->exec(eec, (_fcntPerfFade < 0.0f) ? 1.0f : (_fcntPerfFade < FCNT_PREF_MAX) ? 1.0f - (_fcntPerfFade / FCNT_PREF_MAX) : 0.0f);

	// ユーザー操作で破壊を進行させる
	if(_cntDestruction > 0)
	{
		bool isReact = false;
		TouchPanel* touchp = eec->getTouchPanel();
		if(!touchp->isReacted() && touchp->isClickRelease(0L))
		{
			touchp->setReacted(true);
			isReact = true;
		}
		MotionController* moctrlr = eec->getMotionController();
		if(moctrlr->isMotionEnd())
		{
			moctrlr->setReacted(true);
			isReact = true;
		}
		if(isReact)
		{
			// 破壊進行
			_cntDestruction++;
			_screen->advanceDivision((f32)_cntDestruction * _cntDestruction);
			_fcntPrefDestruction = 0.0f;
			AudioServicesPlaySystemSound(kSystemSoundID_Vibrate);	// バイブレーションをふるわせる
		}
	}
	
#if defined(_DEBUG)
	STRACE("match=%d(%d), rcg=%d, dst=%d", _arsys->_idxMatchedTrain, _arsys->_idxCandidatedTrain, _arsys->_cntRecognition, _cntDestruction);
#endif
}

/*---------------------------------------------------------------------*//**
	シーングラフによる 3D 描画
**//*---------------------------------------------------------------------*/
void ArScene::renderBySg(const RenderCtx* rc)
{
	MovieFileTextureIos* mtex = ((AtArBody*)AtArBody::getBody())->getMovieFileTexture();
	VideoCameraIos* vcam = ((AtArBody*)AtArBody::getBody())->getVideoCamera();
	
	// 基本描画設定
	rc->getRenderer()->setCulling(false);
	rc->getRenderer()->setEnableTexture(true);
	rc->getRenderer()->setAlphaBlend(false);
	rc->getRenderer()->setAlphaTest(false);
	rc->getRenderer()->setZWriting(false);
	rc->getRenderer()->setZTest(false);
	rc->getRenderer()->setSolidColor(255, 255, 255, 255);
	::glBindTexture(GL_TEXTURE_2D, 0);
	::glMatrixMode(GL_TEXTURE);
	::glLoadIdentity();
	::glMatrixMode(GL_MODELVIEW);
	::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	f32 tw, th;
	Vector3F vtx[4];
	Vector2F uv[4];

	// ムービーテクスチャの表示
	if(_phase == PHASE_MOVIE_START)
	{
		u8* bitsMov = mtex->getBitmapBits();	//_mtex->generateMovieBitmap();
		s32 widthMov = mtex->getWidth();
		s32 heightMov = mtex->getHeight();
		const Vector3F* vScr3d = _screen->getScreen3dVector();
		::glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthMov, heightMov, 0, GL_RGBA, GL_UNSIGNED_BYTE, bitsMov);
		f32 z = vScr3d->z();
		vtx[0].set(  vScr3d->x(),   vScr3d->y(), z);
		vtx[1].set(- vScr3d->x(),   vScr3d->y(), z);
		vtx[2].set(  vScr3d->x(), - vScr3d->y(), z);
		vtx[3].set(- vScr3d->x(), - vScr3d->y(), z);
		uv[0].set(1.0f, 1.0f);
		uv[1].set(0.0f, 1.0f);
		uv[2].set(1.0f, 0.0f);
		uv[3].set(0.0f, 0.0f);
		RendererUtils::render3dTriangles(rc->getRenderer(), vtx, 0L, uv, 0L, 4, RendererUtils::TRIANGLE_STRIP);
	}

	// スクリーンの描画
	_screen->render(
		rc,
		(_fcntPerfFade < 0.0f) ? 1.0f : (_fcntPerfFade < FCNT_PREF_MAX) ? 1.0f - (_fcntPerfFade / FCNT_PREF_MAX) : 0.0f,
		vcam->getBitmapBits(),
		vcam->getWidth(),
		vcam->getHeight());

	 // 特徴点のデバッグ表示
	#if defined(_DEBUG) && 1
	if(_phase == PHASE_AR)
	{
		for(int itrain = 0; itrain < _arsys->_numTrain; itrain++)
		{
			const ArSys::ArTrain* train = &_arsys->_trains[itrain];
			for(int im = 0; im < train->_matches.size(); im++)
			{
				cv::KeyPoint* kpQ = &_arsys->_query->_vkpKeypoints[train->_matches[im].queryIdx];
				//cv::KeyPoint* kpT = (_idxImageDetected != -1) ? &_vkpTrainKeypoints[_idxImageDetected][_matches[i].trainIdx] : 0L;
				if(train->_matches[im].distance < train->_distanceThreshold)				{	rc->getRenderer()->setSolidColor(0, 255, 0, 255);		}
				else if(train->_matches[im].distance < (train->_distanceThreshold + 10.0f))	{	rc->getRenderer()->setSolidColor(0, 127, 127, 255);	}
				else																		{	rc->getRenderer()->setSolidColor(0, 0, 255, 255);		}
				f32 x = kpQ->pt.x;
				f32 y = kpQ->pt.y;
				u32 drang = ((AtArBody*)AtArBody::getBody())->getTfw()->getView()->getDisplayFlags() & TFW_DISPF_DRANG_MASK;
				if((drang == TFW_DISPF_DRANG_0) || (drang == TFW_DISPF_DRANG_90))
				{	y = vcam->getHeight() - y;	}
				::glBindTexture(GL_TEXTURE_2D, train->_png.getGlTextureId());
				const Vector3F* vScr3d = _screen->getScreen3dVector();
				vtx[0].x() = - vScr3d->x() + ((x / (f32)vcam->getWidth()) * vScr3d->x() * 2.0f);
				vtx[0].y() = - vScr3d->y() + ((y / (f32)vcam->getHeight()) * vScr3d->y() * 2.0f);
				vtx[0].z() = vScr3d->z();
				const f32 w = 1.0f;
				vtx[1].set(vtx[0].x() + w, vtx[0].y() - w, vtx[0].z());
				vtx[2].set(vtx[0].x() - w, vtx[0].y() + w, vtx[0].z());
				vtx[3].set(vtx[0].x() + w, vtx[0].y() + w, vtx[0].z());
				vtx[0].set(vtx[0].x() - w, vtx[0].y() - w, vtx[0].z());
				uv[0].set(0.0f, 0.0f);
				uv[1].set(1.0f, 0.0f);
				uv[2].set(0.0f, 1.0f);
				uv[3].set(1.0f, 0.0f);
				RendererUtils::render3dTriangles(rc->getRenderer(), vtx, 0L, uv, 0L, 4, RendererUtils::TRIANGLE_STRIP);
				::glBindTexture(GL_TEXTURE_2D, 0);
			}
		}
	}
	rc->getRenderer()->setSolidColor(255, 255, 255, 255);
	#endif

	rc->getRenderer()->setAlphaTest(true);
	rc->getRenderer()->setZWriting(true);
	rc->getRenderer()->setZTest(true);
}

/*---------------------------------------------------------------------*//**
	2D 描画
**//*---------------------------------------------------------------------*/
void ArScene::draw(const RenderCtx* rc)
{
	if((_phase == PHASE_AR) && (0.0f < _fcntPrefDestruction))
	{
		RectF vtx, uv;

		Texture* texGui = ((AtArGui*)AtArBody::getBody()->getGuiManager())->getGuiTexture();
		rc->getRenderer()->bindTexture(texGui);
	
		f32 alpha = ::fmodf(_fcntPrefDestruction, FCNT_DEST_PERF_UI);
		alpha = (alpha < (FCNT_DEST_PERF_UI * 0.5f)) ?
			alpha / (FCNT_DEST_PERF_UI * 0.5f) :
			(FCNT_DEST_PERF_UI - alpha) / (FCNT_DEST_PERF_UI * 0.5f);
		rc->getRenderer()->setColorAlpha(255 * alpha);
		
		if(_hasMotionDevice)
		{
			RendererUtils::draw2dTextureRect(rc->getRenderer(), RectF::setout(&vtx, RECT_TAP_YES_MOTION), Gcalc::calcTexUv(&uv, UV_TAP, W_GUI_TEX, H_GUI_TEX, 1.0f));
			RendererUtils::draw2dTextureRect(rc->getRenderer(), RectF::setout(&vtx, RECT_SHAKE_YES_MOTION), Gcalc::calcTexUv(&uv, UV_SHAKE, W_GUI_TEX, H_GUI_TEX, 1.0f));
		}
		else
		{
			RendererUtils::draw2dTextureRect(rc->getRenderer(), RectF::setout(&vtx, RECT_TAP_NO_MOTION), Gcalc::calcTexUv(&uv, UV_TAP, W_GUI_TEX, H_GUI_TEX, 1.0f));
		}
	}
	
	// ボタンの描画
	if(_phase == PHASE_AR)
	{
		if(_pnlButtons[BTN_BACK] != 0L)
		{
			_pnlButtons[BTN_BACK]->draw(rc);
		}
	}
}

/*---------------------------------------------------------------------*//**
	アラートボタンクリックハンドラ
**//*---------------------------------------------------------------------*/
void ArScene::onClickAlertButton(int index)
{
	// リトライのアラートビューであろう
	switch(index)
	{
	default:	// リトライ
		{
			// ダウンロード再度開始
			startMatchedMovieDownload();
			
			// フェイズ移行
			_phase = PHASE_MOVIE_DOWNLOADING;
		}
		break;
	case 1:		// キャンセル
		{
			// 値リセット
			_arsys->resetRecognizedResult();	// 認識結果リセット
			_idxMatchedTrainResult = -1;		// 最終認識結果リセット
			_fcntPerfFade = -1.0f;				// フェード演出リセット
			_cntDestruction = 0;				// 破壊カウンタリセット
			_fcntPrefDestruction = -1.0f;		// 破壊演出リセット

			// キャプチャ再開
			VideoCameraIos* vcam = ((AtArBody*)AtArBody::getBody())->getVideoCamera();
			vcam->startCapture();

			// AR スクリーンの状態変更
			_screen->changeStatus(ArScreen::STAT_ATTRACT_ANIM);

			// フェイズ移行
			_phase = PHASE_AR;
		}
		break;
	}
}

/*---------------------------------------------------------------------*//**
	シーン作成
**//*---------------------------------------------------------------------*/
bool ArScene::createScene()
{
	MaterialManager* mtrlmng = ((AtArBody*)AtArBody::getBody())->getMaterialManager();
	
	// シーングラフ及びリソースプールにモデルビュー標準カメラを追加
	etk::SorSgm* sgm = (etk::SorSgm*)EtkBody::getBody()->getSceneGraphManager();
	CamSgnc* nodecCam = (CamSgnc*)sgm->getKindChain(etk::SorSgm::EK_CAM);
	{
		// モデルビュー専用カメラの追加
		ArCam* cam = new ArCam();
		if(!cam->create())
		{
			TRACE("{ArScene::create} create ModelViewCam failed.");
			return false;
		}
		if(sgm->addNode(cam, true))
		{
			nodecCam->switchVisible(cam->getId());	// カメラを有効化
			cam->loc()->z() = 1000.0f;				// 初期位置
			_camMvRef = cam;
		}
	}
	
	// シーングラフに描画システムを追加
	sgm->addNode(_rnode);

#if 0
	// モデル読み込み
	tfw::VcString name = "modelview_test";
	ShapeModel** mdlarr = 0L;
	s32 numMdl = 0;
	if(!ModelLoader::loadModelsFromTbf(EtkBody::getBody()->getTfw(), &name, 0, ModelLoader::LFLAG_DISPOSE_SCENE_GRAPH, (Model***)&mdlarr, &numMdl))
	{
		TRACE("{ArScene::create} load model failed. %s", name.getRaw());
	}
	_mdlarrRef = mdlarr;
	_numMdl = numMdl;

	// アニメーションセット
	_animid = 20;//1;//33;
	for(int i = 0; i < _numMdl; i++)
	{
		if(_animid <= _mdlarrRef[i]->getMaxAnimId())
		{
			_mdlarrRef[i]->setAnimId(_animid, 0, false, true, true, false, false);
		}
	}

	// モデルが一つのときは強制表示（DEFTYPE_DECLARE だと表示されないので）
	if(_numMdl == 1)
	{
		_mdlarrRef[0]->setShow(true);
	}
#else
	_mdlarrRef = 0L;
	_numMdl = 0;
#endif

	// AR システムの作成
	_arsys->create(mtrlmng, NUM_FEATURES);

	// キャプチャ開始
	VideoCameraIos* vcam = ((AtArBody*)AtArBody::getBody())->getVideoCamera();
	if((vcam != 0L) && (!vcam->isCapturing()))
	{
		vcam->startCapture();
	}
	
	// インゲームフロントの有効化
	IngameFront* igfr = ((AtArBody*)AtArBody::getBody())->getGui()->getIngameFront();
	if(igfr != 0L)
	{
		igfr->setEnable(true);
	}
	
	// AR スクリーンの初期状態設定
	_screen->changeStatus(ArScreen::STAT_ONENESS);
	
	// その他リセット
	_phase = PHASE_AR;
	_fcntPerfFade = -1.0f;
	_cntDestruction = 0;
	_fcntPrefDestruction = -1.0f;
	_idxMatchedTrainResult = -1;

	return true;
}

/*---------------------------------------------------------------------*//**
	シーン破棄
**//*---------------------------------------------------------------------*/
void ArScene::destroyScene()
{
	// インゲームフロントの無効化
	IngameFront* igfr = ((AtArBody*)AtArBody::getBody())->getGui()->getIngameFront();
	if(igfr != 0L)
	{
		igfr->setEnable(false);
	}
	
	// キャプチャ終了
	VideoCameraIos* vcam = ((AtArBody*)AtArBody::getBody())->getVideoCamera();
	if((vcam != 0L) && vcam->isCapturing())
	{
		vcam->stopCapture();
	}

	// AR スクリーンの状態リセット
	_screen->changeStatus(ArScreen::STAT_NULL);

	// AR システムの破棄
	if(_arsys != 0L)
	{
		_arsys->destroy();
	}

	// シーングラフのモデルを破棄
	etk::SorSgm* sgm = (etk::SorSgm*)EtkBody::getBody()->getSceneGraphManager();
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
}

/*---------------------------------------------------------------------*//**
	マッチした画像の映像のダウンロードを開始する
**//*---------------------------------------------------------------------*/
bool ArScene::startMatchedMovieDownload()
{
	// 念のため確認
	ASSERT((0 <= _idxMatchedTrainResult) && (_idxMatchedTrainResult < _arsys->_numTrain));
	if(!((0 <= _idxMatchedTrainResult) && (_idxMatchedTrainResult < _arsys->_numTrain)))
	{
		return false;
	}

	// フェードカウンタリセット
	_fcntPerfFade = 0.0f;

	// ダウンロード開始
	MaterialManager* mtrlmng = ((AtArBody*)AtArBody::getBody())->getMaterialManager();
	if(!mtrlmng->startOneMovieDownload(_idxMatchedTrainResult))
	{
		return false;
	}

	// ローディングスクリーン開始
	LoadingScreen* ldscr = ((AtArBody*)AtArBody::getBody())->getLoadingScreen();
	ldscr->begin();
	
	return true;
}

/*---------------------------------------------------------------------*//**
	映像の再生準備
**//*---------------------------------------------------------------------*/
void ArScene::prepareMatchedMoviePlay()
{
	// 念のため確認
	ASSERT((0 <= _idxMatchedTrainResult) && (_idxMatchedTrainResult < _arsys->_numTrain));
	if(!((0 <= _idxMatchedTrainResult) && (_idxMatchedTrainResult < _arsys->_numTrain)))
	{
		return;
	}

	// 再生準備
	MovieFileTextureIos* mtex = ((AtArBody*)AtArBody::getBody())->getMovieFileTexture();
	VcString pathMov;
	EtkBody::getBody()->getOsDepMod()->getDocumentDirPath(&pathMov, false);
	pathMov += _arsys->_trains[_idxMatchedTrainResult]._id + FEXT_MOV;
	mtex->prepareFile(&pathMov);

	// フェードカウンタリセット
	_fcntPerfFade = 0.0f;
}

/*---------------------------------------------------------------------*//**
	ボタン作成
**//*---------------------------------------------------------------------*/
Panel* ArScene::createButton(Texture* texGuiRef, const RectS32* rectPanel, const RectF* uvPanelNormal, const RectF* uvPanelHover)
{
	ColorU8 col;

	Panel* pnl = new Panel();
	if(pnl == 0L)
	{
		ASSERT(false);
		return 0L;
	}
	if(!pnl->create(rectPanel))
	{
		ASSERT(false);
		return 0L;
	}
	pnl->setStateInfo(Panel::STATE_NORMAL, texGuiRef, uvPanelNormal, ColorU8::setout(&col, 255, 255, 255, 255));
	pnl->setStateInfo(Panel::STATE_HOVER, texGuiRef, uvPanelHover, ColorU8::setout(&col, 255, 255, 255, 255));

	return pnl;
}

ATAR_END_NS