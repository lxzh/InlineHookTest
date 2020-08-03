package com.lxzh123.inlinehooktest;

import android.app.Application;
import android.content.Context;
import android.util.Log;

import com.geetest.shell.Core;
//import com.lxzh123.hooker.XHooker;

public class App extends Application {

    private static Context mContext;

    @Override
    protected void attachBaseContext(Context base) {
        super.attachBaseContext(base);
        mContext = this;
        Log.d("DumpDex", "MainActivity onCreate");
        Hooker.hook();
//        Hooker.dump("com.lxzh123.inlinehooktest");
//        XHooker.getInstance().start();
    }

    @Override
    public void onCreate() {
        super.onCreate();
        startInit();
//        init();
    }

    private void startInit() {
        new Thread() {
            @Override
            public void run() {
                super.run();
                try {
                    Thread.sleep(5000);
                } catch (Exception ex) {
                    ex.printStackTrace();
                }
                Hooker.dump("com.lxzh123.inlinehooktest");
                init();
            }
        }.start();
    }

    public static void init() {
        try {
            Log.i("DumpDex", "init start");
            Core.init(mContext);
//            XHooker.getInstance().refresh(true);
        } catch (Exception ex) {
            Log.e("DumpDex", "onCreate Exception ex=" + ex.getMessage());
            ex.printStackTrace();
        }
    }
}
