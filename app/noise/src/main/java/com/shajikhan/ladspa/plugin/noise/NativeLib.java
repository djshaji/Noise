package com.shajikhan.ladspa.plugin.noise;

public class NativeLib {

    // Used to load the 'noise' library on application startup.
    static {
        System.loadLibrary("noise");
    }

    /**
     * A native method that is implemented by the 'noise' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();
}