package jp.co.altoterras.sourcerer01;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager.NameNotFoundException;

import com.google.android.vending.expansion.downloader.DownloaderClientMarshaller;

public class EnlightAlarmReceiver extends BroadcastReceiver
{
	@Override public void onReceive(Context context, Intent intent)
	{
		try
		{
			DownloaderClientMarshaller.startDownloadServiceIfRequired(context, intent, EnlightDownloaderService.class);
		}
		catch(NameNotFoundException e)
		{
			e.printStackTrace();
		}       
	}
}
