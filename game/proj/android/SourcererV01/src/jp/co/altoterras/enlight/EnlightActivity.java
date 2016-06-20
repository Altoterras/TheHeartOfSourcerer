package jp.co.altoterras.enlight;

import com.android.vending.expansion.zipfile.ZipResourceFile;
import com.android.vending.expansion.zipfile.ZipResourceFile.ZipEntryRO;
import com.google.android.vending.expansion.downloader.DownloaderClientMarshaller;
import com.google.android.vending.expansion.downloader.DownloadProgressInfo;
import com.google.android.vending.expansion.downloader.DownloaderServiceMarshaller;
import com.google.android.vending.expansion.downloader.Helpers;
import com.google.android.vending.expansion.downloader.IDownloaderClient;
import com.google.android.vending.expansion.downloader.IDownloaderService;
import com.google.android.vending.expansion.downloader.IStub;
import android.app.Activity;
import android.app.PendingIntent;
import android.content.Intent;
import android.content.pm.PackageManager.NameNotFoundException;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Messenger;
import android.os.SystemClock;
import android.provider.Settings;
import android.util.Log;
//import android.view.WindowManager;
import android.view.View;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.TextView;
import java.io.DataInputStream;
//import java.io.File;
import java.io.IOException;
import java.util.zip.CRC32;

/**
 *　アクティビティ
　*/
public class EnlightActivity extends Activity implements IDownloaderClient
{
	// ゲームビュー
	private GameView _viewGame;

	// ダウンロードサービス
    private IDownloaderService _dsRemoteService;
    private IStub _stubDownloaderClient;
	private boolean _statePaused;
	private int _state;
	private boolean _cancelValidation;

    // UI
	private ProgressBar _widgetPb;
	private TextView _widgetStatusText;
	private TextView _widgetProgressFraction;
	private TextView _widgetProgressPercent;
	private TextView _widgetAverageSpeed;
	private TextView _widgetTimeRemaining;
	private Button _widgetPauseButton;
	private Button _widgetWiFiSettingsButton;
	private View _viewDashboard;
	private View _viewCellMessage;
	static private final float SMOOTHING_FACTOR = 0.005f;

	/**
	 *　拡張ファイル定義
	　*/
	private static class XapkFile
	{
		public final boolean _isMain;
		public final int _filever;
		public final long _filesize;
		
		XapkFile(boolean isMain, int fileVersion, long fileSize)
		{
			_isMain = isMain;
			_filever = fileVersion;
			_filesize = fileSize;
		}
	}
    private static final XapkFile[] XAPKS =
    {
		new XapkFile(
			true,			// メインファイル
			100,			// ファイルバージョン
			340694618L	),	// ファイルサイズ
		new XapkFile(
			false,			// パッチファイル
			0,				// ファイルバージョン
			0L			)	// ファイルサイズ
    };

    /**
	 *　アクティビティ作成のイベントハンドラ
	　*/
	@Override protected void onCreate(Bundle icicle)
	{
		super.onCreate(icicle);
		_viewGame = new GameView(getApplication());
 		setContentView(_viewGame);

		/** /
		setupUi();
	
		if(!checkExpansionFilesDelivered())
		{
			try
			{
				Intent launchIntent = EnlightActivity.this.getIntent();
				Intent intentToLaunchThisActivityFromNotification = new Intent(EnlightActivity.this, EnlightActivity.this.getClass());
				intentToLaunchThisActivityFromNotification.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_CLEAR_TOP);
				intentToLaunchThisActivityFromNotification.setAction(launchIntent.getAction());
				if(launchIntent.getCategories() != null)
				{
					for(String category : launchIntent.getCategories())
					{
						intentToLaunchThisActivityFromNotification.addCategory(category);
					}
				}

				// PendingIntent の作成
				PendingIntent pendingIntent = PendingIntent.getActivity(EnlightActivity.this, 0, intentToLaunchThisActivityFromNotification, PendingIntent.FLAG_UPDATE_CURRENT);
				// ダウンロード開始
				int startResult = DownloaderClientMarshaller.startDownloadServiceIfRequired(this, pendingIntent, EnlightDownloaderService.class);
				if (startResult != DownloaderClientMarshaller.NO_DOWNLOAD_REQUIRED)
				{
					// ダウンロードサービスがファイルのダウンロードを開始したら進行状況を表示する
					setupUi();
					return;
				}
			}
			catch(NameNotFoundException e)
			{
				Log.e(GameView.TAG, "Cannot find own package! MAYDAY!");
				e.printStackTrace();
			}
		}
		else
		{
			validateXapkFiles();
		}
		/**/
	}

	/**
	 *　アクティビティ破棄のイベントハンドラ
	　*/
	@Override protected void onDestroy()
	{
		this._cancelValidation = true;
		super.onDestroy();
	}

	/**
	 *　アクティビティ一時停止のイベントハンドラ		
	　*/
	@Override protected void onPause()
	{
		super.onPause();
		_viewGame.onPause();
	}

	/**
	 *　アクティビティ再開のイベントハンドラ
	　*/
	@Override protected void onResume()
	{
		super.onResume();
		_viewGame.onResume();
	}
	
	/**
	 * アクティビティ開始
	 */
	@Override protected void onStart()
	{
		if(null != _stubDownloaderClient)
		{
			_stubDownloaderClient.connect(this);
		}
		super.onStart();
	}

	/**
	 * アクティビティ停止
	 */
	@Override protected void onStop()
	{
		if(null != _stubDownloaderClient)
		{
			_stubDownloaderClient.disconnect(this);
		}
		super.onStop();
	}

	/**
	 *　ダウンロードサービス接続のイベントハンドラ
	　*/
	@Override public void onServiceConnected(Messenger m)
	{
		_dsRemoteService = DownloaderServiceMarshaller.CreateProxy(m);
		_dsRemoteService.onClientUpdated(_stubDownloaderClient.getMessenger());
	}

	/**
	 *　ダウンロードサービス状態変更のイベントハンドラ
	　*/
	@Override public void onDownloadStateChanged(int newState)
	{
		if (_state != newState)
		{
			_state = newState;
			if(_widgetStatusText != null)
			{
				_widgetStatusText.setText(Helpers.getDownloaderStringResourceIDFromState(newState));
			}
		}
			
		boolean showDashboard = true;
		boolean showCellMessage = false;
		boolean paused;
		boolean indeterminate;
		switch (newState)
		{
		case IDownloaderClient.STATE_IDLE:
			paused = false;
			indeterminate = true;
			break;
		case IDownloaderClient.STATE_CONNECTING:
		case IDownloaderClient.STATE_FETCHING_URL:
			showDashboard = true;
			paused = false;
			indeterminate = true;
			break;
		case IDownloaderClient.STATE_DOWNLOADING:
			paused = false;
			showDashboard = true;
			indeterminate = false;
			break;
		case IDownloaderClient.STATE_FAILED_CANCELED:
		case IDownloaderClient.STATE_FAILED:
		case IDownloaderClient.STATE_FAILED_FETCHING_URL:
		case IDownloaderClient.STATE_FAILED_UNLICENSED:
			paused = true;
			showDashboard = false;
			indeterminate = false;
			break;
		case IDownloaderClient.STATE_PAUSED_NEED_CELLULAR_PERMISSION:
		case IDownloaderClient.STATE_PAUSED_WIFI_DISABLED_NEED_CELLULAR_PERMISSION:
			showDashboard = false;
			paused = true;
			indeterminate = false;
			showCellMessage = true;
			break;
		case IDownloaderClient.STATE_PAUSED_BY_REQUEST:
			paused = true;
			indeterminate = false;
			break;
		case IDownloaderClient.STATE_PAUSED_ROAMING:
		case IDownloaderClient.STATE_PAUSED_SDCARD_UNAVAILABLE:
			paused = true;
			indeterminate = false;
			break;
		case IDownloaderClient.STATE_COMPLETED:
			showDashboard = false;
			paused = false;
			indeterminate = false;
			validateXapkFiles();
			return;
		default:
			paused = true;
			indeterminate = true;
			showDashboard = true;
		}
		
		if((_viewDashboard != null) && (_viewCellMessage != null) && (_widgetPb != null))
		{
			int newDashboardVisibility = showDashboard ? View.VISIBLE : View.GONE;
			if(_viewDashboard.getVisibility() != newDashboardVisibility)
			{
				_viewDashboard.setVisibility(newDashboardVisibility);
			}
			int cellMessageVisibility = showCellMessage ? View.VISIBLE : View.GONE;
			if(_viewCellMessage.getVisibility() != cellMessageVisibility)
			{
				_viewCellMessage.setVisibility(cellMessageVisibility);
			}
			_widgetPb.setIndeterminate(indeterminate);
		}
		setButtonPausedState(paused);
	}

	/**
	 *　ダウンロードサービス進行状況更新のイベントハンドラ
	　*/
	@Override public void onDownloadProgress(DownloadProgressInfo progress)
	{
		if((_widgetAverageSpeed != null) && (_widgetTimeRemaining != null) && (_widgetPb != null) && (_widgetProgressPercent != null) && (_widgetProgressFraction != null))
		{
			_widgetAverageSpeed.setText(getString(R.string.kilobytes_per_second, Helpers.getSpeedString(progress.mCurrentSpeed)));
			_widgetTimeRemaining.setText(getString(R.string.time_remaining, Helpers.getTimeRemaining(progress.mTimeRemaining)));
			progress.mOverallTotal = progress.mOverallTotal;
			_widgetPb.setMax((int) (progress.mOverallTotal >> 8));
			_widgetPb.setProgress((int) (progress.mOverallProgress >> 8));
			_widgetProgressPercent.setText(Long.toString(progress.mOverallProgress * 100 / progress.mOverallTotal) + "%");
			_widgetProgressFraction.setText(Helpers.getDownloadProgressString(progress.mOverallProgress, progress.mOverallTotal));
		}
	}

	/**
	 *　拡張ファイルが配信されているかを調べる
	　*/
    private boolean checkExpansionFilesDelivered()
	{
		for(XapkFile xf : XAPKS)
		{
			if(xf._filever <= 0)	{	continue;	}
			String fileName = Helpers.getExpansionAPKFileName(this, xf._isMain, xf._filever);
			if(!Helpers.doesFileExist(this, fileName, xf._filesize, false))
			{
				return false;
			}
		}
		return true;
	}
	
	/**
	 * UI の初期化
	 */
	private void setupUi()
	{
		_stubDownloaderClient = DownloaderClientMarshaller.CreateStub(this, EnlightDownloaderService.class);
		setContentView(R.layout.xapk_downloader);

		_widgetPb = (ProgressBar) findViewById(R.id.progressBar);
		_widgetStatusText = (TextView) findViewById(R.id.statusText);
		_widgetProgressFraction = (TextView) findViewById(R.id.progressAsFraction);
		_widgetProgressPercent = (TextView) findViewById(R.id.progressAsPercentage);
		_widgetAverageSpeed = (TextView) findViewById(R.id.progressAverageSpeed);
		_widgetTimeRemaining = (TextView) findViewById(R.id.progressTimeRemaining);
		_viewDashboard = findViewById(R.id.downloaderDashboard);
		_viewCellMessage = findViewById(R.id.approveCellular);
		_widgetPauseButton = (Button) findViewById(R.id.pauseButton);
		_widgetWiFiSettingsButton = (Button) findViewById(R.id.wifiSettingsButton);

		if(_widgetPauseButton != null)
		{
			_widgetPauseButton.setOnClickListener(
				new View.OnClickListener()
				{
					@Override public void onClick(View view)
					{
						if(_statePaused)
						{
							_dsRemoteService.requestContinueDownload();
						}
						else
						{
							_dsRemoteService.requestPauseDownload();
						}
						setButtonPausedState(!_statePaused);
					}
				}
			);
		}

		if(_widgetWiFiSettingsButton != null)
		{
			_widgetWiFiSettingsButton.setOnClickListener(
				new View.OnClickListener()
				{
					@Override public void onClick(View v)
					{
						startActivity(new Intent(Settings.ACTION_WIFI_SETTINGS));
					}
				}
			);
		}

		Button resumeOnCell = (Button) findViewById(R.id.resumeOverCellular);
		if(resumeOnCell != null)
		{
			resumeOnCell.setOnClickListener(new View.OnClickListener()
				{
					@Override
					public void onClick(View view)
					{
						_dsRemoteService.setDownloadFlags(IDownloaderService.FLAGS_DOWNLOAD_OVER_CELLULAR);
						_dsRemoteService.requestContinueDownload();
						_viewCellMessage.setVisibility(View.GONE);
					}
				}
			);
		}
	}

	/**
	 * ポーズボタンの状態設定
	 */
	private void setButtonPausedState(boolean paused)
	{
		_statePaused = paused;
		if(_widgetPauseButton != null)
		{
			int stringResourceID = paused ? R.string.text_button_resume : R.string.text_button_pause;
			_widgetPauseButton.setText(stringResourceID);
		}
	}
	
	private void validateXapkFiles()
	{
		AsyncTask<Object, DownloadProgressInfo, Boolean> validationTask = new AsyncTask<Object, DownloadProgressInfo, Boolean>()
		{
			@Override
			protected void onPreExecute()
			{
				if((_viewDashboard != null) && (_viewCellMessage != null) && (_widgetStatusText != null) && (_widgetPauseButton != null))
				{
					_viewDashboard.setVisibility(View.VISIBLE);
					_viewCellMessage.setVisibility(View.GONE);
					_widgetStatusText.setText(R.string.text_verifying_download);
					_widgetPauseButton.setOnClickListener(
						new View.OnClickListener()
						{
							@Override public void onClick(View view)
							{
								_cancelValidation = true;
							}
						}
					);
					_widgetPauseButton.setText(R.string.text_button_cancel_verify);
				}
				super.onPreExecute();
			}

			@Override protected Boolean doInBackground(Object... params)
			{
				for(XapkFile xf : XAPKS)
				{
					if(xf._filever <= 0)	{	continue;	}
					String fileName = Helpers.getExpansionAPKFileName(EnlightActivity.this, xf._isMain, xf._filever);
					if (!Helpers.doesFileExist(EnlightActivity.this, fileName, xf._filesize, false))
					{
						return false;
					}
					fileName = Helpers.generateSaveFileName(EnlightActivity.this, fileName);
					ZipResourceFile zrf;
					byte[] buf = new byte[1024 * 256];
					try
					{
						zrf = new ZipResourceFile(fileName);
						ZipEntryRO[] entries = zrf.getAllEntries();

						// 最初にトータル圧縮サイズを計算する
						long totalCompressedLength = 0;
						for (ZipEntryRO entry : entries)
						{
							totalCompressedLength += entry.mCompressedLength;
						}
						float averageVerifySpeed = 0;
						long totalBytesRemaining = totalCompressedLength;
						long timeRemaining;
						
						// ZIP ファイルの CRC を計算
						for (ZipEntryRO entry : entries)
						{
							if (-1 != entry.mCRC32)
							{
								long length = entry.mUncompressedLength;
								CRC32 crc = new CRC32();
								DataInputStream dis = null;
								try
								{
									dis = new DataInputStream(zrf.getInputStream(entry.mFileName));

									long startTime = SystemClock.uptimeMillis();
									while (length > 0)
									{
										// バッファに読み込み、CRC 計算
										int seek = (int) (length > buf.length ? buf.length : length);
										dis.readFully(buf, 0, seek);
										crc.update(buf, 0, seek);
										length -= seek;
										
										// 平均速度の表示
										long currentTime = SystemClock.uptimeMillis();
										long timePassed = currentTime - startTime;
										if (timePassed > 0)
										{
											float currentSpeedSample = (float)seek / (float)timePassed;
											averageVerifySpeed = (0 != averageVerifySpeed) ? (SMOOTHING_FACTOR * currentSpeedSample + (1 - SMOOTHING_FACTOR) * averageVerifySpeed) : currentSpeedSample;
											totalBytesRemaining -= seek;
											timeRemaining = (long)(totalBytesRemaining / averageVerifySpeed);
											this.publishProgress(new DownloadProgressInfo(totalCompressedLength, totalCompressedLength - totalBytesRemaining, timeRemaining, averageVerifySpeed));
										}
										startTime = currentTime;
										
										// キャンセル応答
										if (_cancelValidation)
										{
											return true;
										}
									}
									if (crc.getValue() != entry.mCRC32)
									{
										Log.e(GameView.TAG, "CRC does not match for entry: " + entry.mFileName);
										Log.e(GameView.TAG, "In file: " + entry.getZipFileName());
										return false;
									}
								}
								finally
								{
									if (null != dis)
									{
										dis.close();
									}
								}
							}
						}
					}
					catch (IOException e)
					{
						e.printStackTrace();
						return false;
					}
				}
				return true;
			}

			@Override
			protected void onProgressUpdate(DownloadProgressInfo... values)
			{
				onDownloadProgress(values[0]);
				super.onProgressUpdate(values);
			}

			@Override
			protected void onPostExecute(Boolean result)
			{
				if(result)
				{
					if((_viewDashboard != null) && (_viewCellMessage != null) && (_widgetStatusText != null) && (_widgetPauseButton != null))
					{
						_viewDashboard.setVisibility(View.VISIBLE);
						_viewCellMessage.setVisibility(View.GONE);
						_widgetStatusText.setText(R.string.text_validation_complete);
						_widgetPauseButton.setOnClickListener(
							new View.OnClickListener()
							{
								@Override public void onClick(View view)
								{
									//finish();
									setContentView(_viewGame);
								}
							}
						);
						_widgetPauseButton.setText(android.R.string.ok);
					}
				}
				else
				{
					if((_viewDashboard != null) && (_viewCellMessage != null) && (_widgetStatusText != null) && (_widgetPauseButton != null))
					{
						_viewDashboard.setVisibility(View.VISIBLE);
						_viewCellMessage.setVisibility(View.GONE);
						_widgetStatusText.setText(R.string.text_validation_failed);
						_widgetPauseButton.setOnClickListener(
							new View.OnClickListener()
							{
								@Override public void onClick(View view)
								{
									//finish();
									setContentView(_viewGame);
								}
							}
						);
						_widgetPauseButton.setText(android.R.string.cancel);
					}
				}
				super.onPostExecute(result);
			}
		};
		validationTask.execute(new Object());
	}

}
