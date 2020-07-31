package com.lxzh123.hooker;

public class XHooker {
    private static final XHooker ourInstance = new XHooker();

    static {
        System.loadLibrary("xhook");
        System.loadLibrary("xhooker");
    }

    public static XHooker getInstance() {
        return ourInstance;
    }

    private XHooker() {
    }

    public native void start();

    public native void load();

    public native int refresh(boolean async);
}
