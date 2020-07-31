package com.lxzh123.inlinehooktest;

public class Hooker {
    static {
        System.loadLibrary("hooker");
    }

    public native static void hook();
    public native static void dump(String packageName);
    public native static void printMap();
}
