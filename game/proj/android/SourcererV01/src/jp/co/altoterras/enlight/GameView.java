package jp.co.altoterras.enlight;

import android.content.Context;
import android.graphics.PixelFormat;
import android.opengl.GLSurfaceView;
//import android.util.AttributeSet;
import android.util.Log;
//import android.view.KeyEvent;
import android.view.MotionEvent;
import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.opengles.GL10;

/**
 *　ゲーム ビュー
　*/
class GameView extends GLSurfaceView
{
	public static String TAG = "Enlight";
	private static final boolean DEBUG = false;

	public GameView(Context context)
	{
		super(context);
		init(false, 16, 0);
	}

	public GameView(Context context, boolean translucent, int depth, int stencil)
	{
		super(context);
		init(translucent, depth, stencil);
	}

	private void init(boolean translucent, int depth, int stencil)
	{
		// 半透明時のサーフェイスフォーマット変更
		if (translucent)
		{
			this.getHolder().setFormat(PixelFormat.TRANSLUCENT);
		}

		// コンテキスト ファクトリーのセットアップ
		setEGLContextFactory(new ContextFactory());

		// コンフィグ選択を設定する
		setEGLConfigChooser(translucent ? new ConfigChooser(8, 8, 8, 8, depth, stencil) : new ConfigChooser(5, 6, 5, 0, depth, stencil));

		// レンダラーの設定
		setRenderer(new Renderer(this));
	}

	private static class ContextFactory implements GLSurfaceView.EGLContextFactory
	{
		private static int EGL_CONTEXT_CLIENT_VERSION = 0x3098;
		public EGLContext createContext(EGL10 egl, EGLDisplay display, EGLConfig eglConfig)
		{
			Log.w(TAG, "creating OpenGL ES 1.0 context");
			checkEglError("Before eglCreateContext", egl);
			int[] attrib_list = { EGL_CONTEXT_CLIENT_VERSION, 1, EGL10.EGL_NONE };
			EGLContext context = egl.eglCreateContext(display, eglConfig, EGL10.EGL_NO_CONTEXT, attrib_list);
			checkEglError("After eglCreateContext", egl);
			return context;
		}

		public void destroyContext(EGL10 egl, EGLDisplay display, EGLContext context)
		{
			egl.eglDestroyContext(display, context);
		}
	}

	private static void checkEglError(String prompt, EGL10 egl)
	{
		int error;
		while ((error = egl.eglGetError()) != EGL10.EGL_SUCCESS)
		{
			Log.e(TAG, String.format("%s: EGL error: 0x%x", prompt, error));
		}
	}

	private static class ConfigChooser implements GLSurfaceView.EGLConfigChooser
	{
		public ConfigChooser(int r, int g, int b, int a, int depth, int stencil)
		{
			_sizeRed = r;
			_sizeGreen = g;
			_sizeBlue = b;
			_sizeAlpha = a;
			_sizeDepth = depth;
			_sizeStencil = stencil;
		}

		private static int[] s_configAttribs =
		{
			EGL10.EGL_RED_SIZE, 4,
			EGL10.EGL_GREEN_SIZE, 4,
			EGL10.EGL_BLUE_SIZE, 4,
			EGL10.EGL_DEPTH_SIZE, 4,
			EGL10.EGL_RENDERABLE_TYPE, 4,
			EGL10.EGL_NONE
		};

		public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display)
		{
			// 最小限マッチするコンフィグを取得する
			int[] num_config = new int[1];
			egl.eglChooseConfig(display, s_configAttribs, null, 0, num_config);

			int numConfigs = num_config[0];

			if (numConfigs <= 0)
			{
				throw new IllegalArgumentException("No configs match configSpec");
			}

			// 最小限マッチする EGL コンフィグｍの配列を作成する
			EGLConfig[] configs = new EGLConfig[numConfigs];
			egl.eglChooseConfig(display, s_configAttribs, configs, numConfigs, num_config);

			if(DEBUG)
			{
				printConfigs(egl, display, configs);
			}
			return chooseConfig(egl, display, configs);
		}

		public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display, EGLConfig[] configs)
		{
			for(EGLConfig config : configs)
			{
				// 必要なステンシル及び深度サイズのコンフィグを求める
				int d = findConfigAttrib(egl, display, config, EGL10.EGL_DEPTH_SIZE, 0);
				int s = findConfigAttrib(egl, display, config, EGL10.EGL_STENCIL_SIZE, 0);
				if (d < _sizeDepth || s < _sizeStencil)
				{
					continue;
				}

				// RGBA に正確にマッチするコンフィグを求める
				int r = findConfigAttrib(egl, display, config, EGL10.EGL_RED_SIZE, 0);
				int g = findConfigAttrib(egl, display, config, EGL10.EGL_GREEN_SIZE, 0);
				int b = findConfigAttrib(egl, display, config, EGL10.EGL_BLUE_SIZE, 0);
				int a = findConfigAttrib(egl, display, config, EGL10.EGL_ALPHA_SIZE, 0);
				if (r == _sizeRed && g == _sizeGreen && b == _sizeBlue && a == _sizeAlpha)
				{
					return config;
				}
			}
			return null;
		}

		private int findConfigAttrib(EGL10 egl, EGLDisplay display, EGLConfig config, int attribute, int defaultValue)
		{
			if (egl.eglGetConfigAttrib(display, config, attribute, _atrbval))
			{
				return _atrbval[0];
			}
			return defaultValue;
		}

		private void printConfigs(EGL10 egl, EGLDisplay display, EGLConfig[] configs)
		{
			int numConfigs = configs.length;
			Log.w(TAG, String.format("%d configurations", numConfigs));
			for (int i = 0; i < numConfigs; i++)
			{
				Log.w(TAG, String.format("Configuration %d:\n", i));
				printConfig(egl, display, configs[i]);
			}
		}

		private void printConfig(EGL10 egl, EGLDisplay display, EGLConfig config)
		{
			int[] attributes =
			{
					EGL10.EGL_BUFFER_SIZE,
					EGL10.EGL_ALPHA_SIZE,
					EGL10.EGL_BLUE_SIZE,
					EGL10.EGL_GREEN_SIZE,
					EGL10.EGL_RED_SIZE,
					EGL10.EGL_DEPTH_SIZE,
					EGL10.EGL_STENCIL_SIZE,
					EGL10.EGL_CONFIG_CAVEAT,
					EGL10.EGL_CONFIG_ID,
					EGL10.EGL_LEVEL,
					EGL10.EGL_MAX_PBUFFER_HEIGHT,
					EGL10.EGL_MAX_PBUFFER_PIXELS,
					EGL10.EGL_MAX_PBUFFER_WIDTH,
					EGL10.EGL_NATIVE_RENDERABLE,
					EGL10.EGL_NATIVE_VISUAL_ID,
					EGL10.EGL_NATIVE_VISUAL_TYPE,
					0x3030, // EGL10.EGL_PRESERVED_RESOURCES,
					EGL10.EGL_SAMPLES,
					EGL10.EGL_SAMPLE_BUFFERS,
					EGL10.EGL_SURFACE_TYPE,
					EGL10.EGL_TRANSPARENT_TYPE,
					EGL10.EGL_TRANSPARENT_RED_VALUE,
					EGL10.EGL_TRANSPARENT_GREEN_VALUE,
					EGL10.EGL_TRANSPARENT_BLUE_VALUE,
					0x3039, // EGL10.EGL_BIND_TO_TEXTURE_RGB,
					0x303A, // EGL10.EGL_BIND_TO_TEXTURE_RGBA,
					0x303B, // EGL10.EGL_MIN_SWAP_INTERVAL,
					0x303C, // EGL10.EGL_MAX_SWAP_INTERVAL,
					EGL10.EGL_LUMINANCE_SIZE,
					EGL10.EGL_ALPHA_MASK_SIZE,
					EGL10.EGL_COLOR_BUFFER_TYPE,
					EGL10.EGL_RENDERABLE_TYPE,
					0x3042 // EGL10.EGL_CONFORMANT
			};
			String[] names =
			{
					"EGL_BUFFER_SIZE",
					"EGL_ALPHA_SIZE",
					"EGL_BLUE_SIZE",
					"EGL_GREEN_SIZE",
					"EGL_RED_SIZE",
					"EGL_DEPTH_SIZE",
					"EGL_STENCIL_SIZE",
					"EGL_CONFIG_CAVEAT",
					"EGL_CONFIG_ID",
					"EGL_LEVEL",
					"EGL_MAX_PBUFFER_HEIGHT",
					"EGL_MAX_PBUFFER_PIXELS",
					"EGL_MAX_PBUFFER_WIDTH",
					"EGL_NATIVE_RENDERABLE",
					"EGL_NATIVE_VISUAL_ID",
					"EGL_NATIVE_VISUAL_TYPE",
					"EGL_PRESERVED_RESOURCES",
					"EGL_SAMPLES",
					"EGL_SAMPLE_BUFFERS",
					"EGL_SURFACE_TYPE",
					"EGL_TRANSPARENT_TYPE",
					"EGL_TRANSPARENT_RED_VALUE",
					"EGL_TRANSPARENT_GREEN_VALUE",
					"EGL_TRANSPARENT_BLUE_VALUE",
					"EGL_BIND_TO_TEXTURE_RGB",
					"EGL_BIND_TO_TEXTURE_RGBA",
					"EGL_MIN_SWAP_INTERVAL",
					"EGL_MAX_SWAP_INTERVAL",
					"EGL_LUMINANCE_SIZE",
					"EGL_ALPHA_MASK_SIZE",
					"EGL_COLOR_BUFFER_TYPE",
					"EGL_RENDERABLE_TYPE",
					"EGL_CONFORMANT"
			};
			int[] value = new int[1];
			for (int i = 0; i < attributes.length; i++)
			{
				int attribute = attributes[i];
				String name = names[i];
				if ( egl.eglGetConfigAttrib(display, config, attribute, value))
				{
					Log.w(TAG, String.format("  %s: %d\n", name, value[0]));
				}
				else
				{
					// Log.w(TAG, String.format("  %s: failed\n", name));
					while (egl.eglGetError() != EGL10.EGL_SUCCESS);
				}
			}
		}

		protected int _sizeRed;
		protected int _sizeGreen;
		protected int _sizeBlue;
		protected int _sizeAlpha;
		protected int _sizeDepth;
		protected int _sizeStencil;
		private int[] _atrbval = new int[1];
	}

	private static class Renderer implements GLSurfaceView.Renderer
	{
		public Renderer(android.view.View viewOwn)
		{
			_viewOwn = viewOwn;
		}		
		
		public void onDrawFrame(GL10 gl)
		{
			EnlightLib.step();
		}

		public void onSurfaceChanged(GL10 gl, int width, int height)
		{
			EnlightLib.init(_viewOwn.getContext(), width, height);
		}

		public void onSurfaceCreated(GL10 gl, EGLConfig config)
		{
			// Do nothing.
		}
		
		private android.view.View _viewOwn;
	}

	@Override public void onPause()
	{
		EnlightLib.enterBackground();
		super.onPause();
	}
	
	@Override public void onResume()
	{
		super.onResume();
		EnlightLib.leaveBackground();
	}
	
	@Override public boolean onTouchEvent(MotionEvent event)
	{
		int count = event.getPointerCount();
		if(count >= 2)
		{
			EnlightLib.touch(event.getAction(), 2, event.getX(0), event.getY(0), event.getX(1), event.getY(1));
		}
		else if(count >= 1)
		{
			EnlightLib.touch(event.getAction(), 1, event.getX(0), event.getY(0), 0, 0);
		}
		else
		{
			EnlightLib.touch(event.getAction(), 0, 0, 0, 0, 0);
		}
		return true;
	}
}
