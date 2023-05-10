package ind.thk.onestone;

import java.io.File;
import java.io.FileOutputStream;

import android.app.Activity;
import android.app.NativeActivity;
import android.content.SharedPreferences;

public class MainActivity extends NativeActivity {
	void CreateFile()
	{
		SharedPreferences pref=getSharedPreferences("ind.thk.onestone.preference",Activity.MODE_PRIVATE);
		File f=new File(getFilesDir()+"/lvc");
		if(f.exists())
		{
			return;
		}
		FileOutputStream fos=null;
		pref=getSharedPreferences("ind.thk.onestone.preference", Activity.MODE_PRIVATE);
		try{
			fos=new FileOutputStream(f);
			byte[] bbuf=new byte[40];
			bbuf[0]=(byte)(400>>8);
			bbuf[1]=(byte)(400&255);
			fos.write(bbuf, 0, 2);
			int now=1;
			int i,cur;
			for(now=0;now<10;++now)
			{
				for(i=0;i<40;++i)
				{
					cur=now*40+i+1;
					bbuf[i]=(byte)pref.getInt("lv"+cur, 0);
				}
				fos.write(bbuf,0,40);
			}
		}catch(Exception e){}
	}
	void syncLevelFile()
	{
		
	}
}
