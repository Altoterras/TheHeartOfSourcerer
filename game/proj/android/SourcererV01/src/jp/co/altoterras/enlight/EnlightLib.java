package jp.co.altoterras.enlight;

public class EnlightLib
{
		static
		{
			System.loadLibrary("sourcerer01");
		}

		/**
		* @param width the current view width
		* @param height the current view height
		*/
		public static native void init(android.content.Context context, int width, int height);
		public static native void step();
		public static native void enterBackground();
		public static native void leaveBackground();
		public static native void touch(int action, int cntTouch, float x1, float y1, float x2, float y2);
}
