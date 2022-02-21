package com.shajikhan.ladspa.module;

public class NativeLib {

    // Used to load the 'module' library on application startup.
    static {
        System.loadLibrary("module");
    }

    /**
     * A native method that is implemented by the 'module' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();
}