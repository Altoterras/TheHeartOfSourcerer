/***********************************************************************//**
	*	main.cpp
	*	Enlight Android Application
	*
	*	Created by Ryoutarou Kishi on 2012/12/10.
	*	Copyright 2012 Altoterras Corporation. All rights reserved.
	*
**//***********************************************************************/

////////////////////////////////////////////////////////////////////////////
// インクルードファイル

// Friends
#include "DebugAndroid.h"
#include "OsDepModAndroid.h"
#include "PsnsAndroid.h"
#include "StopWatchAndroid.h"

// External
#include "../basicmodel/body/BasicModelBody.h"
#include "../etk/body/EtkBody.h"
#include "../sourcerer/body/Game.h"
#include "../tfw/file/File.h"
#include "../tfw/file/IdFile.h"
#include "../tfw/string/VcString.h"
#include "../tfw/ui/TouchPanel.h"

// Library
#include <android/sensor.h>
#include <android/log.h>
#include <android_native_app_glue.h>
#include <EGL/egl.h>
#include <GLES/gl.h>
#include <errno.h>
#include <jni.h>

using namespace app;

////////////////////////////////////////////////////////////////////////////
// マクロ

#define LOGI(...)((void)__android_log_print(ANDROID_LOG_INFO, "sourcerer01", __VA_ARGS__))
#define LOGW(...)((void)__android_log_print(ANDROID_LOG_WARN, "sourcerer01", __VA_ARGS__))

#if defined(_DEBUG)
	#define DBGTRACE	LOGI
#else
	#define DBGTRACE	//
#endif

#define ENABLE_APP				(1)		// アプリケーション有効
#define ENABLE_NATIVE_ACTIVITY	(0)		// NativeActivity 有効

////////////////////////////////////////////////////////////////////////////
// 定数

// フレーム値
#define RENDER_SPF		(60.0f)		// １秒間の描画フレーム数
#define LOGIC_SPF		(30.0f)		// １秒間で進む論理フレーム数

// 圧縮ファイル展開用リードバッファのサイズ
#define SIZE_UNCOMPRESS_READ_BUFFER_1		(5 * 1024 * 1024)	// 5MB
#define SIZE_UNCOMPRESS_READ_BUFFER_2		(12 * 1024 * 1024)	// 12MB

// サポートするタッチポイントの数
#define NUM_TOUCH_MAX						(2)

////////////////////////////////////////////////////////////////////////////
// 構造体

/*---------------------------------------------------------------------*//**
 *	セーブされる状態
 *
**//*---------------------------------------------------------------------*/
struct saved_state
{
#if !ENABLE_APP	// サンプルコード
	float angle;
	int32_t x;
	int32_t y;
#endif
};

/*---------------------------------------------------------------------*//**
 *	アプリケーションの状態変数
 *
**//*---------------------------------------------------------------------*/
struct engine
{
	struct android_app* _aapp;

	ASensorManager* _sensorManager;
	const ASensor* _accelerometerSensor;
	ASensorEventQueue* _sensorEventQueue;

	int _animating;
	EGLDisplay _display;
	EGLSurface _surface;
	EGLContext _context;
	int32_t _width;
	int32_t _height;
	struct saved_state _state;

#if ENABLE_APP
	StopWatchAndroid* _swFrame;
	OsDepModAndroid* _osdep;
	PsnsAndroid* _psns;
	etk::EtkBody* _body;
#endif
};

////////////////////////////////////////////////////////////////////////////
// グローバル変数

#if !ENABLE_NATIVE_ACTIVITY
struct engine g_engine;
#endif

////////////////////////////////////////////////////////////////////////////
// 関数宣言

extern "C"
{

JNIEXPORT void JNICALL Java_jp_co_altoterras_enlight_EnlightLib_init(JNIEnv* env, jobject jobjThis, jobject jobjActivity, jint width, jint height);
JNIEXPORT void JNICALL Java_jp_co_altoterras_enlight_EnlightLib_step(JNIEnv* env, jobject jobjThis);
JNIEXPORT void JNICALL Java_jp_co_altoterras_enlight_EnlightLib_enterBackground(JNIEnv* env, jobject jobjThis);
JNIEXPORT void JNICALL Java_jp_co_altoterras_enlight_EnlightLib_leaveBackground(JNIEnv* env, jobject jobjThis);
JNIEXPORT void JNICALL Java_jp_co_altoterras_enlight_EnlightLib_touch(JNIEnv* env, jobject jobjThis, jint action, jint cntTouch, jfloat x1, jfloat y1, jfloat x2, jfloat y2);

};	// extern "C"

static int initApp(struct engine* eng, JNIEnv* jnienv, jobject clazzActivity);
static int doApp(struct engine* eng);
static int updateApp(struct engine* eng);
static int cleanupApp(struct engine* eng);
static void touchApp(struct engine* eng, int action, int cntTouch, float x1, float y1, float x2, float y2);

#if defined(_DEBUG)
static bool test_getFilesDir(struct engine* eng);
static bool test_encodeString(struct engine* eng);
static bool test_openFile(struct engine* eng);
static bool test_openUrl(struct engine* eng);
static bool test_clipboard(struct engine* eng);
#endif

#if ENABLE_APP
// OpenAL 補助
jint JNI_OnLoad(JavaVM* vm, void* reserved);
#endif

////////////////////////////////////////////////////////////////////////////
// 関数定義

/*---------------------------------------------------------------------*//**
	EGL コンテキストの初期化
**//*---------------------------------------------------------------------*/
static int engine_init_display(struct engine* eng)
{
	// initialize OpenGL ES and EGL

	/*
		* Here specify the attributes of the desired configuration.
		* Below, we select an EGLConfig with at least 8 bits per color
		* component compatible with on-screen windows
		*/
	const EGLint attribs[] =
	{
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_BLUE_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_RED_SIZE, 8,
		EGL_NONE
	};
	EGLint w, h, dummy, format;
	EGLint numConfigs;
	EGLConfig config;
	EGLSurface surface;
	EGLContext context;

	EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	eglInitialize(display, 0, 0);

	/* Here, the application chooses the configuration it desires. In this
		* sample, we have a very simplified selection process, where we pick
		* the first EGLConfig that matches our criteria */
	eglChooseConfig(display, attribs, &config, 1, &numConfigs);

	/* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
		* guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
		* As soon as we picked a EGLConfig, we can safely reconfigure the
		* ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
	eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

	ANativeWindow_setBuffersGeometry(eng->_aapp->window, 0, 0, format);

	surface = eglCreateWindowSurface(display, config, eng->_aapp->window, NULL);
	context = eglCreateContext(display, config, NULL, NULL);

	if(eglMakeCurrent(display, surface, surface, context) == EGL_FALSE)
	{
		LOGW("Unable to eglMakeCurrent");
		return -1;
	}

	eglQuerySurface(display, surface, EGL_WIDTH, &w);
	eglQuerySurface(display, surface, EGL_HEIGHT, &h);

	eng->_display = display;
	eng->_context = context;
	eng->_surface = surface;
	eng->_width = w;
	eng->_height = h;
	eng->_animating = 1;
	#if !ENABLE_APP	// サンプルコード
		eng->_state.angle = 0;
	#endif

	// OpenGL の状態初期化
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
	glEnable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);
	glDisable(GL_DEPTH_TEST);

	return 0;
}

/*---------------------------------------------------------------------*//**
	フレーム描画
**//*---------------------------------------------------------------------*/
static void engine_draw_frame(struct engine* eng)
{
	if(eng->_display == NULL)
	{
		// No display.
		return;
	}

	#if ENABLE_APP
		// フレーム更新
		if(eng->_body != 0L)
		{
			eng->_body->renderFrame();
		}
	#else	// サンプルコード
		// Just fill the screen with a color.
		glClearColor(((float)eng->_state.x) / eng->_width, eng->_state.angle, ((float)eng->_state.y) / eng->_height, 1);
		glClear(GL_COLOR_BUFFER_BIT);
	#endif

	#if ENABLE_NATIVE_ACTIVITY
		eglSwapBuffers(eng->_display, eng->_surface);
	#endif
}

/*---------------------------------------------------------------------*//**
	EGL コンテキストの終了
**//*---------------------------------------------------------------------*/
static void engine_term_display(struct engine* eng)
{
	if(eng->_display != EGL_NO_DISPLAY)
	{
		eglMakeCurrent(eng->_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		if(eng->_context != EGL_NO_CONTEXT)
		{
			eglDestroyContext(eng->_display, eng->_context);
		}
		if(eng->_surface != EGL_NO_SURFACE)
		{
			eglDestroySurface(eng->_display, eng->_surface);
		}
		eglTerminate(eng->_display);
	}
	eng->_animating = 0;
	eng->_display = EGL_NO_DISPLAY;
	eng->_context = EGL_NO_CONTEXT;
	eng->_surface = EGL_NO_SURFACE;
}

/*---------------------------------------------------------------------*//**
	Process the next input event.
**//*---------------------------------------------------------------------*/
static int32_t engine_handle_input(struct android_app* app, AInputEvent* event)
{
	struct engine* eng = (struct engine*)app->userData;
	if(AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION)
	{
		#if ENABLE_APP
			s32 tcnt = (s32)AMotionEvent_getPointerCount(event);
			if(tcnt >= 2)
			{
				touchApp(eng, AKeyEvent_getAction(event), 2, AMotionEvent_getX(event, 0), AMotionEvent_getY(event, 0), AMotionEvent_getX(event, 1), AMotionEvent_getY(event, 1));
			}
			else if(tcnt >= 1)
			{
				touchApp(eng, AKeyEvent_getAction(event), 1, AMotionEvent_getX(event, 0), AMotionEvent_getY(event, 0), 0, 0);
			}
			else
			{
				touchApp(eng, AKeyEvent_getAction(event), 0, 0, 0, 0, 0);
			}
		#else	// サンプルコード
			///eng->_animating = 1;
			eng->_state.x = AMotionEvent_getX(event, 0);
			eng->_state.y = AMotionEvent_getY(event, 0);
		#endif
		return 1;
	}
	return 0;
}

/*---------------------------------------------------------------------*//**
	Process the next main command.
**//*---------------------------------------------------------------------*/
static void engine_handle_cmd(struct android_app* app, int32_t cmd)
{
	struct engine* eng = (struct engine*)app->userData;
	switch(cmd)
	{
	case APP_CMD_SAVE_STATE:
		// システムから現在の状態を保存するように求められたときのイベントハンドラ
		{
			eng->_aapp->savedState = malloc(sizeof(struct saved_state));
			*((struct saved_state*)eng->_aapp->savedState) = eng->_state;
			eng->_aapp->savedStateSize = sizeof(struct saved_state);
		}
		break;
	case APP_CMD_INIT_WINDOW:
		// 最初の表示がされたときのイベントハンドラ
		// アプリの初期化を行う
		if(eng->_aapp->window != NULL)
		{
			// ディスプレイ環境の初期化処理
			engine_init_display(eng);

			// OpenAL 補助
			#if ENABLE_NATIVE_ACTIVITY
				JNI_OnLoad(eng->_aapp->activity->vm, NULL);
			#endif

			// JNI 環境取得
			JNIEnv* jnienv;
			eng->_aapp->activity->vm->AttachCurrentThread(&jnienv, NULL);

			#if ENABLE_APP
				// アプリケーション初期化
				initApp(eng, jnienv, eng->_aapp->activity->clazz);
			#endif

			// アプリの最初の描画
			engine_draw_frame(eng);
		}
		break;
	case APP_CMD_TERM_WINDOW:
		// ウインドウが隠されるか閉じられたときのイベントハンドラ
		// アプリの後始末を行う
		{
			#if ENABLE_APP
				// アプリケーション後始末
				cleanupApp(eng);
			#endif

			// ディスプレイ環境の終端処理
			engine_term_display(eng);
		}
		break;
	case APP_CMD_GAINED_FOCUS:
		// アプリにフォーカスが得られ、加速度計のモニタリングを開始するときのイベントハンドラ
		if(eng->_accelerometerSensor != NULL)
		{
			ASensorEventQueue_enableSensor(eng->_sensorEventQueue, eng->_accelerometerSensor);
			// We'd like to get 60 events per second(in us).
			ASensorEventQueue_setEventRate(eng->_sensorEventQueue, eng->_accelerometerSensor,(1000L/60)*1000);
		}
		break;
	case APP_CMD_LOST_FOCUS:
		// アプリがフォーカスを失い、加速度計のモニタリングを停止するときのイベントハンドラ
		// バッテリー消費を抑制するための処理
		{
			if(eng->_accelerometerSensor != NULL)
			{
				ASensorEventQueue_disableSensor(eng->_sensorEventQueue, eng->_accelerometerSensor);
			}
			// アニメーションを停止する
			eng->_animating = 0;
			engine_draw_frame(eng);
		}
		break;
	}
}

/*---------------------------------------------------------------------*//**
	NativeActivity 実行時のエントリーポイント
**//*---------------------------------------------------------------------*/
void android_main(struct android_app* aapp)
{
	DBGTRACE("{android_main} started.\n");

	struct engine engineStackInstance;

	// Make sure glue isn't stripped.
	app_dummy();

	struct engine* eng = &engineStackInstance;
	memset(eng, 0, sizeof(engine));
	aapp->userData = eng;
	aapp->onAppCmd = engine_handle_cmd;
	aapp->onInputEvent = engine_handle_input;
	eng->_aapp = aapp;

	// 加速度計モニタリングの準備
	eng->_sensorManager = ASensorManager_getInstance();
	eng->_accelerometerSensor = ASensorManager_getDefaultSensor(eng->_sensorManager, ASENSOR_TYPE_ACCELEROMETER);
	eng->_sensorEventQueue = ASensorManager_createEventQueue(eng->_sensorManager, aapp->looper, LOOPER_ID_USER, NULL, NULL);

	if(aapp->savedState != NULL)
	{
		// 以前の保存された状態を復帰する
		eng->_state = *(struct saved_state*)aapp->savedState;
	}

	// アプリケーション実行
	doApp(eng);
}

/*---------------------------------------------------------------------*//**
	Java 実行時の初期化処理
**//*---------------------------------------------------------------------*/
JNIEXPORT void JNICALL Java_jp_co_altoterras_enlight_EnlightLib_init(JNIEnv* env, jobject jobjThis, jobject jobjActivity, jint width, jint height)
{
	DBGTRACE("{Java_jp_co_altoterras_enlight_EnlightLib_init} started.\n");

#if !ENABLE_NATIVE_ACTIVITY
	struct engine* eng = &g_engine;
	memset((void*)eng, 0, sizeof(engine));

	if((eng->_width == width) && (eng->_height == height))
	{
		return;
	}

	eng->_display = eglGetCurrentDisplay();
	eng->_context = eglGetCurrentContext();
	eng->_surface = eglGetCurrentSurface(EGL_DRAW);
	eng->_width = width;
	eng->_height = height;
	eng->_animating = 1;
	#if !ENABLE_APP	// サンプルコード
		eng->_state.angle = 0;
	#endif

	// OpenGL の状態初期化
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
	glEnable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);
	glDisable(GL_DEPTH_TEST);

	initApp(eng, env, jobjActivity);
#endif	// !ENABLE_NATIVE_ACTIVITY

	DBGTRACE("{Java_jp_co_altoterras_enlight_EnlightLib_init} end.\n");
}

/*---------------------------------------------------------------------*//**
	Java 実行時の更新・描画処理
**//*---------------------------------------------------------------------*/
JNIEXPORT void JNICALL Java_jp_co_altoterras_enlight_EnlightLib_step(JNIEnv* env, jobject jobjThis)
{
	///DBGTRACE("{Java_jp_co_altoterras_enlight_EnlightLib_step} started.\n");

#if !ENABLE_NATIVE_ACTIVITY
	struct engine* eng = &g_engine;

	updateApp(eng);
#endif	// !ENABLE_NATIVE_ACTIVITY

	///DBGTRACE("{Java_jp_co_altoterras_enlight_EnlightLib_step} end.\n");
}

/*---------------------------------------------------------------------*//**
	Java 実行時のバックグラウンドへ入る処理
**//*---------------------------------------------------------------------*/
JNIEXPORT void JNICALL Java_jp_co_altoterras_enlight_EnlightLib_enterBackground(JNIEnv* env, jobject jobjThis)
{
#if !ENABLE_NATIVE_ACTIVITY
	struct engine* eng = &g_engine;

	if(eng->_body != 0L)
	{
		eng->_body->enterBackground();
	}
#endif	// !ENABLE_NATIVE_ACTIVITY
}

/*---------------------------------------------------------------------*//**
	Java 実行時のバックグラウンドから抜ける処理
**//*---------------------------------------------------------------------*/
JNIEXPORT void JNICALL Java_jp_co_altoterras_enlight_EnlightLib_leaveBackground(JNIEnv* env, jobject jobjThis)
{
#if !ENABLE_NATIVE_ACTIVITY
	struct engine* eng = &g_engine;

	if(eng->_body != 0L)
	{
		eng->_body->leaveBackground();
	}
#endif	// !ENABLE_NATIVE_ACTIVITY
}

/*---------------------------------------------------------------------*//**
	Java 実行時のタッチイベント処理
**//*---------------------------------------------------------------------*/
JNIEXPORT void JNICALL Java_jp_co_altoterras_enlight_EnlightLib_touch(JNIEnv* env, jobject jobjThis, jint action, jint cntTouch, jfloat x1, jfloat y1, jfloat x2, jfloat y2)
{
#if !ENABLE_NATIVE_ACTIVITY
	struct engine* eng = &g_engine;

	touchApp(eng, (int)action, (int)cntTouch, (float)x1, (float)y1, (float)x2, (float)y2);
#endif	// !ENABLE_NATIVE_ACTIVITY
}

/*---------------------------------------------------------------------*//**
	アプリ実行
**//*---------------------------------------------------------------------*/
static int initApp(struct engine* eng, JNIEnv* jnienv, jobject clazzActivity)
{
	DBGTRACE("{initApp} started.\n");

	// ロケール指定
	///setlocale(LC_ALL, "japanese");

#if defined(_DEBUG) && 0
	//test_getFilesDir(engine);
	//test_encodeString(engine);
	//test_openFile(engine);
	//test_openUrl(engine);
	test_clipboard(engine);
#endif

#if ENABLE_APP
	// プロダクト定義
	VcString postfix("-a");
	sourcerer::ProductDefId proddefid = sourcerer::NUM_PRODDEFID;
	etk::EtkBody* body = 0L;
	#if defined(_ENLIGHT_V01)
		proddefid = sourcerer::PRODDEFID_V01;
		CcString prod("jp.co.altoterras.enlight");
		VcString appnam("sourcerer01");
		VcString fname("main.100.jp.co.altoterras.enlight.obb");
		body = new sourcerer::Game();
	#elif defined(_ENLIGHT_V01_LITE)
		proddefid = sourcerer::PRODDEFID_V01LITE;
		CcString prod("jp.co.altoterras.enlight");
		VcString appnam("sourcerer01");
		VcString fname("enlight1010al.dat");
		body = new sourcerer::Game();
	#elif defined(_ENLIGHT_V02)
		proddefid = sourcerer::PRODDEFID_V02;
		CcString prod("jp.co.altoterras.enlight");
		VcString appnam("sourcerer02");
		VcString fname("enlight1020a.dat");
		body = new sourcerer::Game();
	#elif defined(_ENLIGHT_PRPZL01)
		proddefid = sourcerer::PRODDEFID_V01;
		CcString prod("jp.co.altoterras.enlight");
		VcString appnam("sourcerer01");
		VcString fname("enlight1010a.dat");
		body = new sourcerer::Game();
	#elif defined(_BASICMODEL)
		proddefid = sourcerer::PRODDEFID_BASICMODEL;
		CcString prod("jp.co.altoterras.enlight");
		VcString appnam("sourcerer01");
		VcString fname("enlight1010w.dat");
		body = new basicmodel::BasicModelBody();
	#elif defined(_DIMZ)
		proddefid = sourcerer::PRODDEFID_DIMZ;
		CcString prod("jp.co.altoterras.enlight");
		VcString appnam("sourcerer01");
		VcString fname("enlight1010a.dat");
		body = new sourcerer::Game();
	#elif defined(_EDEV)
		proddefid = sourcerer::PRODDEFID_EDEV;
		CcString prod("sourcerer");
		VcString appnam("sourcerer01");
		VcString fname("enlight1010a.dat");
		body = new sourcerer::Game();
	#endif

	// デバッグモジュール初期化
	DebugAndroid::init();

	// OS モジュールの作成
	eng->_osdep = new OsDepModAndroid(jnienv, clazzActivity);

	// PSNS の作成
	eng->_psns = new PsnsAndroid();

	// ボディ生成フラグ
	u32 bcflags = etk::EtkBody::BCF_HD_RESOLUTION;

	// 表示フラグ
	u32 dispflags = TFW_DISPF_RANG_90;

	// リソースパスの取得
	VcString pathRes;
	if(!eng->_osdep->getResourceDirPath(&pathRes, prod))
	{
		return -1;
	}

	// TFW ファイルジュール初期化
	tfw::File::init(&pathRes);
	tfw::IdFile::init(&fname, &postfix, SIZE_UNCOMPRESS_READ_BUFFER_1, SIZE_UNCOMPRESS_READ_BUFFER_2, IdFile::PIFF_STORED_ZIP);
	DBGTRACE("{initApp} file-module inited. pathRes=%s, fname=%s, postfix=%s\n", pathRes.getRaw(), fname.getRaw(), postfix.getRaw());
		
	// ゲームアプリケーション作成
	if((body == 0L) || ! body->init(
		(s32)eng->_width,
		(s32)eng->_height,
		1.0f,
		dispflags,
		RENDER_SPF,
		LOGIC_SPF,
		etk::LANGID_JP,
		etk::EtkBody::SBCASE_OPEN,
		bcflags,
		eng->_osdep,
		eng->_psns	))
	{
		delete body;
		return -1;
	}
	eng->_body = body;

	// フレーム計測のためのストップウォッチを作成
	eng->_swFrame = new StopWatchAndroid();
	eng->_body->setStopWatch(eng->_swFrame);

#endif
	return 0;
}

/*---------------------------------------------------------------------*//**
	アプリ実行
**//*---------------------------------------------------------------------*/
static int doApp(struct engine* eng)
{
	DBGTRACE("{doApp} started.\n");

	// loop waiting for stuff to do.
	while(1)
	{
		// Read all pending events.
		int ident;
		int events;
		struct android_poll_source* source;

		// If not animating, we will block forever waiting for events.
		// If animating, we loop until all events are read, then continue
		// to draw the next frame of animation.
		while((ident = ALooper_pollAll(eng->_animating ? 0 : -1, NULL, &events, (void**)&source)) >= 0)
		{
			// Process this event.
			if(source != NULL)
			{
				source->process(eng->_aapp, source);
			}

			// If a sensor has data, process it now.
			if(ident == LOOPER_ID_USER)
			{
				if(eng->_accelerometerSensor != NULL)
				{
					ASensorEvent event;
					while(ASensorEventQueue_getEvents(eng->_sensorEventQueue, &event, 1) > 0)
					{
						///LOGI("accelerometer: x=%f y=%f z=%f", event.acceleration.x, event.acceleration.y, event.acceleration.z);
					}
				}
			}

			// Check if we are exiting.
			if(eng->_aapp->destroyRequested != 0)
			{
				#if ENABLE_APP
					// アプリケーション後始末
					cleanupApp(eng);
				#endif

				// ディスプレイ環境の終端処理
				engine_term_display(eng);
				return -1;
			}
		}

		if(eng->_animating)
		{
			// アプリ更新
			updateApp(eng);
		}
	}

	return 0;
}

/*---------------------------------------------------------------------*//**
	アプリ更新
**//*---------------------------------------------------------------------*/
static int updateApp(struct engine* eng)
{
	#if ENABLE_APP
		// 経過フレーム計測
		float frameElapse = 1.0f;
		if(eng->_swFrame != 0L)
		{
			float timeElapse = eng->_swFrame->getElapsedSeconds();
			if(timeElapse != 0.0f)	{	frameElapse = timeElapse * LOGIC_SPF;	}
			eng->_swFrame->start();		// この時点をフレームの開始時点とする
		}
	
		// フレーム更新
		if(eng->_body != 0L)
		{
			eng->_body->updateFrame(frameElapse);
		}
	#else	// サンプルコード
		// Done with events; draw next animation frame.
		eng->_state.angle += .01f;
		if(eng->_state.angle > 1)
		{
			eng->_state.angle = 0;
		}
	#endif

	// Drawing is throttled to the screen update rate, so there
	// is no need to do timing here.
	engine_draw_frame(eng);
}

/*---------------------------------------------------------------------*//**
	アプリ解放
**//*---------------------------------------------------------------------*/
static int cleanupApp(struct engine* eng)
{
	DBGTRACE("{cleanupApp} started.\n");

#if ENABLE_APP
	// ゲームアプリケーション解放
	if(eng->_body != 0L)
	{
		eng->_body->cleanup(etk::EtkBody::TBCASE_CLOSE);
	}
	
	// ファイルモジュールの後処理
	tfw::IdFile::cleanup();
	tfw::File::cleanup();

	// ストップウォッチの解放
	if(eng->_swFrame != NULL)
	{
		delete eng->_swFrame;
		eng->_swFrame = 0L;
	}

	// PSNS の解放
	if(eng->_psns != 0L)
	{
		delete eng->_psns;
		eng->_psns = 0L;
	}

	// OS モジュールの解放
	if(eng->_osdep != 0L)
	{
		delete eng->_osdep;
		eng->_osdep = 0L;
	}

	// ゲームアプリケーション破棄
	if(eng->_body != 0L)
	{
		delete eng->_body;
		eng->_body = NULL;
	}
#endif

	return 0;
}

/*---------------------------------------------------------------------*//**
	アプリのタッチイベント処理
**//*---------------------------------------------------------------------*/
static void touchApp(struct engine* eng, int action, int cntTouch, float x1, float y1, float x2, float y2)
{
	if(eng->_body == 0L)
	{
		return;
	}

	if(cntTouch > NUM_TOUCH_MAX)
	{
		cntTouch = NUM_TOUCH_MAX;
	}

	tfw::TouchPanel* ui = eng->_body->getTouchPanel();

	tfw::TouchPanel::Touch arrt[NUM_TOUCH_MAX];
	arrt[0].set(x1, y1);
	arrt[1].set(x2, y2);

	switch(action & AMOTION_EVENT_ACTION_MASK)
	{
	case AMOTION_EVENT_ACTION_DOWN:
	case AMOTION_EVENT_ACTION_POINTER_DOWN:
		ui->beganTouches(arrt, cntTouch);
		break;
	case AMOTION_EVENT_ACTION_UP:
	case AMOTION_EVENT_ACTION_POINTER_UP:
		ui->endedTouches(arrt, cntTouch);
		break;
	case AMOTION_EVENT_ACTION_CANCEL:
		ui->cancelledTouches(arrt, cntTouch);
		break;
	case AMOTION_EVENT_ACTION_MOVE:
		ui->movedTouches(arrt, cntTouch);
		break;
	}
}

#if defined(_DEBUG)
/*---------------------------------------------------------------------*//**
	getFilesDir テスト
**//*---------------------------------------------------------------------*/
static bool test_getFilesDir(struct engine* eng)
{
	LOGI("###*** TEST 01 ***### start...\n");

	tfw::VcString str;

	JNIEnv *jni;
	eng->_aapp->activity->vm->AttachCurrentThread(&jni, NULL);

	jclass jclsNativeActivity = jni->FindClass("android/app/NativeActivity");
	if(jclsNativeActivity == NULL)						{	LOGI("### 701 ### +2\n");	return false;	}
	jclass jclsContext = jni->FindClass("android/content/Context");
	if(jclsContext == NULL)								{	LOGI("### 702 ###\n");	return false;	}
	jclass jclsFile = jni->FindClass("java/io/File");
	if(jclsFile == NULL)									{	LOGI("### 703 ###\n");	return false;	}
	/*
	jclass jclsReflector = jni->FindClass("jp/co/altoterras/sourcerer01/Reflector");
	if(jclsReflector == NULL)							{	LOGI("### 704 ### +1\n");	return false;	}
	*/

	/*
	jmethodID jmethodClassLoader = jni->GetMethodID(jclsNativeActivity, "getClassLoader", "()Ljava/lang/ClassLoader;");
	if(jmethodClassLoader == NULL)						{	LOGI("### 801 ###\n");	return false;	}
	jobject jobjClassLoader = jni->CallObjectMethod(eng->_aapp->activity->clazz, jmethodClassLoader);
	if(jobjClassLoader == NULL)							{	LOGI("### 802 ###\n");	return false;	}
	jclass jclsClassLoader = jni->FindClass("java/lang/ClassLoader");
	if(jclsClassLoader == NULL)							{	LOGI("### 803 ###\n");	return false;	}
	jmethodID jmethodLoadClass = jni->GetMethodID(jclsClassLoader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
	if(jmethodLoadClass == NULL)							{	LOGI("### 804 ###\n");	return false;	}
	jstring strClassName = jni->NewStringUTF("jp/co/altoterras/sourcerer01/Reflector");
	if(strClassName == NULL)							{	LOGI("### 805 ###\n");	return false;	}
LOGI("### 2 ###\n");
	jclass jclsReflector = (jclass)jni->CallObjectMethod(jobjClassLoader, jmethodLoadClass, strClassName); 
	if(jclsReflector == NULL)							{	LOGI("### 806 ### +2\n");	return false;	}
LOGI("### 3 ###\n");
	jmethodID jmethodReflectorCtor = jni->GetMethodID(jclsReflector, "<init>", "()V");
	if(jmethodReflectorCtor == NULL)						{	LOGI("### 807 ### +1\n");	return false;	}
LOGI("### 4 ###\n");
	jobject jobjReflector = jni->NewObject(jclsReflector, jmethodReflectorCtor);
	if(jobjReflector == NULL)							{	LOGI("### 808 ### +1\n");	return false;	}
LOGI("### 5 ###\n");
	jmethodID jmethodTest = jni->GetMethodID(jclsReflector, "test", "();");
	if(jmethodTest == NULL)								{	LOGI("### 809 ###\n");	return false;	}
LOGI("### 6 ###\n");
	jni->CallVoidMethod(jobjReflector, jmethodTest);

	jmethodID jmethodGetApplicationContext = jni->GetMethodID(jclsContext, "getApplicationContext", "()Landroid/content/Context;");
	if(jmethodGetApplicationContext == NULL)				{	LOGI("### 904 ###\n");	return false;	}
	*/
	jmethodID jmethodGetFilesDir = jni->GetMethodID(jclsContext, "getFilesDir", "()Ljava/io/File;");
	if(jmethodGetFilesDir == NULL)						{	LOGI("### 905 ###\n");	return false;	}
	jmethodID jmethodGetPath = jni->GetMethodID(jclsFile, "getPath", "()Ljava/lang/String;");
	if(jmethodGetPath == NULL)							{	LOGI("### 906 ###\n");	return false;	}
	jmethodID jmethodGetPackageCodePath = jni->GetMethodID(jclsNativeActivity, "getPackageCodePath", "()Ljava/lang/String;");
	if(jmethodGetPackageCodePath == NULL)				{	LOGI("### 907 ###\n");	return false;	}
	jmethodID jmethodGetPackageResourcePath = jni->GetMethodID(jclsNativeActivity, "getPackageResourcePath", "()Ljava/lang/String;");
	if(jmethodGetPackageResourcePath == NULL)			{	LOGI("### 908 ###\n");	return false;	}

	/*
	jobject jobjContext = jni->CallObjectMethod(eng->_aapp->activity->clazz, jmethodGetApplicationContext);
	if(jobjContext == NULL)								{	LOGI("### 907 ###\n");	return false;	}
	jobject jobjFile = jni->CallObjectMethod(jobjContext, jmethodGetFilesDir);
	if(jobjFile == NULL)									{	LOGI("### 908 ###\n");	return false;	}
	jstring strPath = (jstring)jni->CallObjectMethod(jobjFile, jmethodGetPath);
	if(strPath == NULL)									{	LOGI("### 909 ###\n");	return false;	}
	*/
	jobject jobjFile = jni->CallObjectMethod(eng->_aapp->activity->clazz, jmethodGetFilesDir);
	if(jobjFile == NULL)								{	LOGI("### 1001 ###\n");	return false;	}
	jstring jstrFilesDirPath = (jstring)jni->CallObjectMethod(jobjFile, jmethodGetPath);
	if(jstrFilesDirPath == NULL)							{	LOGI("### 1002 ###\n");	return false;	}
	jstring jstrPackageCodePath = (jstring)jni->CallObjectMethod(eng->_aapp->activity->clazz, jmethodGetPackageCodePath);
	if(jstrPackageCodePath == NULL)						{	LOGI("### 1003 ###\n");	return false;	}
	jstring jstrPackageResourcePath = (jstring)jni->CallObjectMethod(eng->_aapp->activity->clazz, jmethodGetPackageResourcePath);
	if(jstrPackageResourcePath == NULL)					{	LOGI("### 1004 ###\n");	return false;	}

	jsize len = jni->GetStringLength(jstrFilesDirPath);
	const char* chars = jni->GetStringUTFChars(jstrFilesDirPath, NULL);
	str.set(chars, (int)len);
	jni->ReleaseStringUTFChars(jstrFilesDirPath, chars);
	LOGI("getFilesDir=%s\n", str.getRaw());

	len = jni->GetStringLength(jstrPackageCodePath);
	chars = jni->GetStringUTFChars(jstrPackageCodePath, NULL);
	str.set(chars, (int)len);
	jni->ReleaseStringUTFChars(jstrPackageCodePath, chars);
	LOGI("getPackageCodePath=%s\n", str.getRaw());

	len = jni->GetStringLength(jstrPackageResourcePath);
	chars = jni->GetStringUTFChars(jstrPackageResourcePath, NULL);
	str.set(chars, (int)len);
	jni->ReleaseStringUTFChars(jstrPackageResourcePath, chars);
	LOGI("getPackageResourcePath=%s\n", str.getRaw());

	/*
	jmethodID jmethodLoadClass = jni->GetMethodID(jclsNativeActivity, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
	if(jmethodLoadClass == NULL)							{	LOGI("### 804 ###\n");	return false;	}
	jstring strClassName = jni->NewStringUTF("jp/co/altoterras/sourcerer01/Reflector");
	if(strClassName == NULL)							{	LOGI("### 805 ###\n");	return false;	}
	jclass jclsReflector = (jclass)jni->CallObjectMethod(eng->_aapp->activity->clazz, jmethodLoadClass, strClassName); 
	if(jclsReflector == NULL)							{	LOGI("### 806 ### +2\n");	return false;	}
	*/

	LOGI("###*** TEST 01 ***### end.");

	return true;
}

/*---------------------------------------------------------------------*//**
	文字エンコード テスト
**//*---------------------------------------------------------------------*/
static bool test_encodeString(struct engine* eng)
{
	LOGI("###*** TEST 02 ***### start...\n");

	JNIEnv *jni;
	eng->_aapp->activity->vm->AttachCurrentThread(&jni, NULL);

	char bytesSjis[] = { 0x81, 0xa5, 0x88, 0xa4, 0x0 };					// Shift-JIS の"▼愛"
	char bytesUtf8[] = { 0xe2, 0x96, 0xbc, 0xe6, 0x84, 0x9b, 0x0 };		// utf8 の"▼愛"

	jboolean jb;
	int len = sizeof(bytesSjis) / sizeof(bytesSjis[0]);
	jbyteArray jbarrSjis = jni->NewByteArray(len);
	if(jbarrSjis == NULL)								{	LOGI("### 11 ###\n");	return false;	}
	jbyte* jbSjis = jni->GetByteArrayElements(jbarrSjis, &jb);
	if(jbSjis == NULL)									{	LOGI("### 12 ###\n");	return false;	}
	for(int i = 0; i < len; i++) { jbSjis[i] = bytesSjis[i]; }
	jni->ReleaseByteArrayElements(jbarrSjis, jbSjis, 0);
	len = sizeof(bytesUtf8) / sizeof(bytesUtf8[0]);
	jbyteArray jbarrUtf8 = jni->NewByteArray(len);
	if(jbarrUtf8 == NULL)								{	LOGI("### 21 ###\n");	return false;	}
	jbyte* jbUtf8 = jni->GetByteArrayElements(jbarrUtf8, &jb);
	if(jbUtf8 == NULL)									{	LOGI("### 22 ###\n");	return false;	}
	for(int i = 0; i < len; i++) { jbUtf8[i] = bytesUtf8[i]; }
	jni->ReleaseByteArrayElements(jbarrUtf8, jbUtf8, 0);

	jstring jstrEncSjis = jni->NewStringUTF("Shift_JIS");
	if(jstrEncSjis == NULL)								{	LOGI("### 51 ###\n");	return false;	}
	jstring jstrEncUtf8 = jni->NewStringUTF("UTF-8");
	if(jstrEncUtf8 == NULL)								{	LOGI("### 52 ###\n");	return false;	}

	jclass jclsString = jni->FindClass("java/lang/String");
	if(jclsString == NULL)								{	LOGI("### 101 ###\n");	return false;	}

	jmethodID jmethodStringCtor = jni->GetMethodID(jclsString, "<init>", "([BLjava/lang/String;)V");
	if(jmethodStringCtor == NULL)						{	LOGI("### 111 ###\n");	return false;	}
	jmethodID jmethodStringGetBytes = jni->GetMethodID(jclsString, "getBytes", "(Ljava/lang/String;)[B");
	if(jmethodStringGetBytes == NULL)					{	LOGI("### 112 ###\n");	return false;	}

	jstring jobjSjisStrOnUnicode = (jstring)jni->NewObject(jclsString, jmethodStringCtor, jbarrSjis, jstrEncSjis); 
	if(jobjSjisStrOnUnicode == NULL)					{	LOGI("### 201 ###\n");	return false;	}
	jstring jobjUtf8StrOnUnicode = (jstring)jni->NewObject(jclsString, jmethodStringCtor, jbarrUtf8, jstrEncUtf8); 
	if(jobjUtf8StrOnUnicode == NULL)					{	LOGI("### 202 ###\n");	return false;	}

	jbarrUtf8 = (jbyteArray)jni->CallObjectMethod(jobjSjisStrOnUnicode, jmethodStringGetBytes, jstrEncUtf8);
	if(jbarrUtf8 == NULL)								{	LOGI("### 301 ###\n");	return false;	}
	jbarrSjis = (jbyteArray)jni->CallObjectMethod(jobjUtf8StrOnUnicode, jmethodStringGetBytes, jstrEncSjis);
	if(jbarrSjis == NULL)								{	LOGI("### 302 ###\n");	return false;	}

	jbUtf8 = jni->GetByteArrayElements(jbarrUtf8, &jb);
	if(jbUtf8 == NULL)									{	LOGI("### 401 ###\n");	return false;	}
	LOGI("utf8 -> Shift-JIS...\n{");
	for(int i = 0; i < jni->GetArrayLength(jbarrUtf8); i++) { LOGI(" %x,", (unsigned char)jbUtf8[i]); }
	LOGI(" }\n");
	jni->ReleaseByteArrayElements(jbarrUtf8, jbUtf8, 0);
	jbSjis = jni->GetByteArrayElements(jbarrSjis, &jb);
	if(jbSjis == NULL)									{	LOGI("### 402 ###\n");	return false;	}
	LOGI("utf8 -> Shift-JIS...\n{");
	for(int i = 0; i < jni->GetArrayLength(jbarrSjis); i++) { LOGI(" %x,", (unsigned char)jbSjis[i]); }
	LOGI(" }\n");
	jni->ReleaseByteArrayElements(jbarrSjis, jbSjis, 0);

	/*
	// Shift-JIS -> utf8
	jstring strUtf8 = jni->NewString("漢字躁鬱");
	if(strUtf8 == NULL)		{	LOGI("### 105 ###\n");	return false;	}

	// utf8 -> Shift-JIS
	jstring strUtf8 = jni->NewStringUTF("漢字躁鬱");
	if(strUtf8 == NULL)		{	LOGI("### 105 ###\n");	return false;	}
	*/

	/*
		byte[] bytesUtf8 = strUnicode.getBytes("UTF-8");
	*/

	LOGI("###*** TEST 02 ***### end.\n");
	return true;
}

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
/*---------------------------------------------------------------------*//**
	ファイル オープン テスト
**//*---------------------------------------------------------------------*/
static bool test_openFile(struct engine* eng)
{
	LOGI("###*** TEST 03 ***### start...\n");

#if 1
	AAssetManager* astmng = eng->_aapp->activity->assetManager;

	AAsset* asset = AAssetManager_open(astmng, "a.txt", AASSET_MODE_UNKNOWN);
	if(asset == 0L)										{	LOGI("### 101 ###\n");	return false;	}

	#if 0	// AAsset_openFileDescriptor は圧縮のかからないファイルのみ
		off_t ofs, len;
		int fd = AAsset_openFileDescriptor(asset, &ofs, &len);
		if(fd < 0)										{	LOGI("### 102 ###\n");	return false;	}

		FILE* file = fdopen(fd, "rb");
		if(file < 0)									{	LOGI("### 103 ###\n");	return false;	}

		LOGI("{");
		const int SIZE_BUF = 255;
		char cbuf[SIZE_BUF + 1];
		int rdsize;
		do
		{
			rdsize = fread(cbuf, 1, SIZE_BUF, file);
			for(int i = 0; i < rdsize; i++) { LOGI(" %c,", cbuf[i]); }
		}
		while(rdsize > 0);
		LOGI(" }\n");
	#else
		LOGI("{");
		const int SIZE_BUF = 255;
		char cbuf[SIZE_BUF + 1];
		int rdsize;
		do
		{
			rdsize = AAsset_read(asset, cbuf, SIZE_BUF);
			for(int i = 0; i < rdsize; i++) { LOGI(" %c,", cbuf[i]); }
		}
		while(rdsize > 0);
		LOGI(" }\n");
	#endif

	AAsset_close(asset);

#elif 1
	AAssetManager* astmng = eng->_aapp->activity->assetManager;

	AAsset* asset = AAssetManager_open(astmng, "a.txt", AASSET_MODE_UNKNOWN);
	if(asset == 0L)										{	LOGI("### 101 ###\n");	return false;	}

	LOGI("{");
	const int SIZE_BUF = 255;
	char cbuf[SIZE_BUF + 1];
	int rdsize;
	do
	{
		rdsize = AAsset_read(asset, cbuf, SIZE_BUF);
		for(int i = 0; i < rdsize; i++) { LOGI(" %c,", cbuf[i]); }
	}
	while(rdsize > 0);
	LOGI(" }\n");

	AAsset_close(asset);

#elif 0
	JNIEnv *jni;
	eng->_aapp->activity->vm->AttachCurrentThread(&jni, NULL);

	jclass jclsNativeActivity = jni->FindClass("android/app/NativeActivity");
	if(jclsNativeActivity == NULL)						{	LOGI("### 101 ###\n");	return false;	}

	jmethodID jmethodGetAssets = jni->GetMethodID(jclsNativeActivity, "getAssets", "()Landroid/content/res/AssetManager;");
	if(jmethodGetAssets == NULL)						{	LOGI("### 102 ###\n");	return false;	}

	jobject jobjAssetMng = (jstring)jni->CallObjectMethod(eng->_aapp->activity->clazz, jmethodGetAssets);
	if(jobjAssetMng == NULL)							{	LOGI("### 103 ###\n");	return false;	}

#else
	VcString strPath;
	{
		JNIEnv *jni;
		eng->_aapp->activity->vm->AttachCurrentThread(&jni, NULL);
		jclass jclsNativeActivity = jni->FindClass("android/app/NativeActivity");
		if(jclsNativeActivity == NULL)						{	LOGI("### 701 ### +2\n");	return false;	}
		jmethodID jmethodGetPackageResourcePath = jni->GetMethodID(jclsNativeActivity, "getPackageResourcePath", "()Ljava/lang/String;");
		if(jmethodGetPackageResourcePath == NULL)			{	LOGI("### 908 ###\n");	return false;	}
		jstring jstrPackageResourcePath = (jstring)jni->CallObjectMethod(eng->_aapp->activity->clazz, jmethodGetPackageResourcePath);
		if(jstrPackageResourcePath == NULL)					{	LOGI("### 1004 ###\n");	return false;	}
		int len = jni->GetStringLength(jstrPackageResourcePath);
		const char* chars = jni->GetStringUTFChars(jstrPackageResourcePath, NULL);
		strPath.set(chars, (int)len);
		jni->ReleaseStringUTFChars(jstrPackageResourcePath, chars);
		LOGI("getPackageResourcePath=%s\n", strPath.getRaw());
	}

	const char* charsPath = strPath + "/a.txt";
	LOGI("%s\n", charsPath);
	FILE* file = fopen(charsPath, "rb");
	if(file == NULL)								{	LOGI("### 101 ###\n");	return false;	}

	LOGI("{");
	const int SIZE_BUF = 256;
	char cbuf[SIZE_BUF];
	int sizeRead;
	do
	{
		sizeRead = fread(cbuf, 1, SIZE_BUF, file);
		for(int i = 0; i < sizeRead; i++) { LOGI(" %x,", cbuf[i]); }
	}
	while(sizeRead < SIZE_BUF);
	LOGI(" }\n");

	fclose(file);
#endif

	LOGI("###*** TEST 03 ***### end.\n");
	return true;
}

/*---------------------------------------------------------------------*//**
	URL オープン テスト
**//*---------------------------------------------------------------------*/
static bool test_openUrl(struct engine* eng)
{
	LOGI("###*** TEST 04 ***### start...\n");

	JNIEnv *jni;
	eng->_aapp->activity->vm->AttachCurrentThread(&jni, NULL);

	// 各クラス取得
	jclass jclsNativeActivity = jni->GetObjectClass(eng->_aapp->activity->clazz);
	if(jclsNativeActivity == NULL)						{	LOGI("### 101 ###\n");	return false;	}
	jclass jclsUri = jni->FindClass("android/net/Uri");
	if(jclsUri == NULL)									{	LOGI("### 102 ###\n");	return false;	}
	jclass jclsIntent = jni->FindClass("android/content/Intent");
	if(jclsIntent == NULL)								{	LOGI("### 103 ###\n");	return false;	}

	// Uri uri = Uri.parse("http://....");
	jmethodID jmethodUriParse = jni->GetStaticMethodID(jclsUri, "parse", "(Ljava/lang/String;)Landroid/net/Uri;");
	if(jmethodUriParse == NULL)							{	LOGI("### 201 ###\n");	return false;	}
	jstring jstrUrl = jni->NewStringUTF("http://altoterras.co.jp/");
	if(jstrUrl == NULL)									{	LOGI("### 202 ###\n");	return false;	}
	jvalue jvalUriParseArgs[1];
	jvalUriParseArgs[0].l = jstrUrl;
	jobject jobjUri = jni->CallStaticObjectMethodA(jclsUri, jmethodUriParse, jvalUriParseArgs);
	if(jobjUri == NULL)									{	LOGI("### 203 ###\n");	return false;	}

	// Intent intent = new Intent(Intent.ACTION_VIEW, uri);
	jmethodID jmethodIntentCtor = jni->GetMethodID(jclsIntent, "<init>", "(Ljava/lang/String;)V");
	if(jmethodIntentCtor == NULL)						{	LOGI("### 301 ###\n");	return false;	}
	jfieldID jfieldActionView = jni->GetStaticFieldID(jclsIntent, "ACTION_VIEW", "Ljava/lang/String;");
	if(jfieldActionView == NULL)						{	LOGI("### 302 ###\n");	return false;	}
	jobject jobjActionView = jni->GetStaticObjectField(jclsIntent, jfieldActionView);
	if(jobjActionView == NULL)							{	LOGI("### 303 ###\n");	return false;	}
	jvalue jvalIntentCtorArgs[2];
	jvalIntentCtorArgs[0].l = jobjActionView;
	jvalIntentCtorArgs[1].l = jobjUri;
	jobject jobjIntent = jni->NewObjectA(jclsIntent, jmethodIntentCtor, jvalIntentCtorArgs);
	if(jobjIntent == NULL)								{	LOGI("### 304 ###\n");	return false;	}
    
	// startActivity(i);
	jmethodID jmethodStartActivity = jni->GetMethodID(jclsNativeActivity, "startActivity", "(Landroid/content/Intent;)V");
	if(jmethodStartActivity == NULL)					{	LOGI("### 401 ###\n");	return false;	}
	jvalue jvalAtartActivityArgs[1];
	jvalAtartActivityArgs[0].l = jobjIntent;
	jni->CallVoidMethodA(eng->_aapp->activity->clazz, jmethodStartActivity, jvalAtartActivityArgs);

	LOGI("###*** TEST 04 ***### end.\n");
	return true;
}

/*---------------------------------------------------------------------*//**
	クリップボード テスト
**//*---------------------------------------------------------------------*/
static bool test_clipboard(struct engine* eng)
{
	LOGI("###*** TEST 05 ***### start...\n");

	JNIEnv *jni;
	eng->_aapp->activity->vm->AttachCurrentThread(&jni, NULL);

	jclass jclsClipboardManager = jni->FindClass("android/text/ClipboardManager");
	if(jclsClipboardManager == NULL)					{	LOGI("### 101 ###\n");	return false;	}
	jclass jclsContext = jni->FindClass("android/content/Context");
	if(jclsContext == NULL)								{	LOGI("### 102 ###\n");	return false;	}

	jmethodID jmethodGetSystemService = jni->GetMethodID(jclsContext, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");
	if(jmethodGetSystemService == NULL)					{	LOGI("### 201 ###\n");	return false;	}
	jmethodID jmethodCmGetText = jni->GetMethodID(jclsClipboardManager, "getText", "()Ljava/lang/CharSequence;");
	if(jmethodCmGetText == NULL)						{	LOGI("### 202 ###\n");	return false;	}
	jmethodID jmethodCmSetText = jni->GetMethodID(jclsClipboardManager, "setText", "(Ljava/lang/CharSequence;)V");
	if(jmethodCmSetText == NULL)						{	LOGI("### 203 ###\n");	return false;	}

	jfieldID jfieldClipboardService = jni->GetStaticFieldID(jclsContext, "CLIPBOARD_SERVICE", "Ljava/lang/String;");
	if(jfieldClipboardService == NULL)					{	LOGI("### 301 ###\n");	return false;	}

	jobject jobjClipboardService = jni->GetStaticObjectField(jclsContext, jfieldClipboardService);
	if(jobjClipboardService == NULL)					{	LOGI("### 302 ###\n");	return false;	}

	// ClipboardManager cm = (ClipboardManager)getSystemService(CLIPBOARD_SERVICE);
	jvalue jvalGetSystemServiceArgs[1];
	jvalGetSystemServiceArgs[0].l = jobjClipboardService;
	jobject jobjClipboardManager = jni->CallObjectMethodA(eng->_aapp->activity->clazz, jmethodGetSystemService, jvalGetSystemServiceArgs);
	if(jobjClipboardManager == NULL)					{	LOGI("### 401 ###\n");	return false;	}

	// String strClipText = cm.getText();
	jstring jstrClipText = (jstring)jni->CallObjectMethod(jobjClipboardManager, jmethodCmGetText);
	if(jstrClipText == NULL)							{	LOGI("### 402 ###\n");	return false;	}
	VcString strClipText;
	int len = jni->GetStringLength(jstrClipText);
	const char* chars = jni->GetStringUTFChars(jstrClipText, NULL);
	strClipText.set(chars, (int)len);
	jni->ReleaseStringUTFChars(jstrClipText, chars);
	LOGI("{1} clip-text: %s\n", strClipText.getRaw());

	// cm.setText(strClipText);
	strClipText.set("**TEST** ABCDEFG");
	jstrClipText = jni->NewStringUTF(strClipText.getRaw());
	if(jstrClipText == NULL)							{	LOGI("### 403 ###\n");	return false;	}
	jni->CallVoidMethod(jobjClipboardManager, jmethodCmSetText, jstrClipText);

	// strClipText = cm.getText();
	jstrClipText = (jstring)jni->CallObjectMethod(jobjClipboardManager, jmethodCmGetText);
	if(jstrClipText == NULL)							{	LOGI("### 404 ###\n");	return false;	}
	len = jni->GetStringLength(jstrClipText);
	chars = jni->GetStringUTFChars(jstrClipText, NULL);
	strClipText.set(chars, (int)len);
	jni->ReleaseStringUTFChars(jstrClipText, chars);
	LOGI("{2} clip-text: %s\n", strClipText.getRaw());

	LOGI("###*** TEST 05 ***### end.\n");
	return true;
}
#endif
