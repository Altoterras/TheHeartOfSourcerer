

package jp.co.altoterras.enlight;

import com.google.android.vending.expansion.downloader.impl.DownloaderService;

/**
 *　ダウンロードサービス実装
　*/
public class EnlightDownloaderService extends DownloaderService
{
	// ライセンスキー - Base64 エンコードの RSA 公開鍵
	private static final String BASE64_PUBLIC_KEY = "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAiQJYgnOl7CDY4SS9rldJi4Vo+YJlpCBL/Gn6ys/RHwPvzuENBYmqtyyTkt6c7ZAxLbVlBroC5/JSE6OUI+H71h9d9rnSx0euHv4WHef38aO7ee72W/6SdoKvlGEqi7iHgw7zACgk9mUL906ttdD9EcTY6xyo5rZcYFRyO+JLTyfifWCo+ZX1VJnm4ZOspCnxJZF798tXP8P3yhI7TH5Lss+NbBBWVM7WkSlUyd6pl1u3b2GmxLM1Q+KAcp0gO5TAl0ou6cNLxNw2iCajgGeUL+whyyqVHRsDSSbkzg7Bt6py9SugC0ZATeQFhqGosyZfL91yg+7jGe6clnf03JJaBwIDAQAB";
	
	// 難読化ヒント
	private static final byte[] SALT = new byte[]
	{
		2, 45,  9, -77, 43, -4, 9, 126, 1, -1, 84, -17, 53, -103, -102, 35, -1, 99, -108, -31, 
	};

	/**
	 * ライセンスキーの取得
	 */
	@Override public String getPublicKey()
	{
		return BASE64_PUBLIC_KEY;
	}

	/**
	 * 難読化ヒントの取得
	 */
	@Override public byte[] getSALT()
	{
		return SALT;
	}

	/**
	 * アラーム受診クラス名の取得
	 */
	@Override public String getAlarmReceiverClassName()
	{
		return EnlightAlarmReceiver.class.getName();
	}
}
