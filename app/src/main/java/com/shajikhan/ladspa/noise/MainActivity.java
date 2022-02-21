package com.shajikhan.ladspa.noise;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import android.Manifest;
import android.content.pm.PackageManager;
import android.media.AudioManager;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.RadioButton;
import android.widget.TextView;
import android.widget.Toast;

import com.shajikhan.ladspa.noise.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity
        implements ActivityCompat.OnRequestPermissionsResultCallback{
    private static final String TAG = MainActivity.class.getName();
    private static final int AUDIO_EFFECT_REQUEST = 0;
    private static final int OBOE_API_AAUDIO = 0;
    private static final int OBOE_API_OPENSL_ES=1;
    private Button toggleEffectButton;
    private boolean isPlaying = false;

    private int apiSelection = OBOE_API_AAUDIO;
    private boolean mAAudioRecommended = true;



    // Used to load the 'noise' library on application startup.
    static {
        System.loadLibrary("noise");
    }

    private ActivityMainBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());
        getSupportActionBar().hide();
        toggleEffectButton = findViewById(R.id.toggleButton);
        toggleEffectButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                toggleEffect();
            }
        });

        RadioButton sine = findViewById(R.id.sine);
        sine.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                AudioEngine.setMode(1);
            }
        });

        RadioButton noise = findViewById(R.id.noise);
        noise.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                AudioEngine.setMode(0);
            }
        });

        noise.setSelected(true);

        AudioEngine.setDefaultStreamValues(this);
//        AudioEngine.create();
    }

    public void toggleEffect() {
        if (isPlaying) {
            stopEffect();
        } else {
            AudioEngine.setAPI(apiSelection);
            startEffect();
        }
    }

    private void startEffect() {
        Log.d(TAG, "Attempting to start");

        if (!isRecordPermissionGranted()){
            requestRecordPermission();
            return;
        }

        boolean success = AudioEngine.setEffectOn(true);
        if (success) {
            isPlaying = true;
            toggleEffectButton.setText("Stop");
        } else {
            isPlaying = false;
        }
    }

    private void stopEffect() {
        Log.d(TAG, "Playing, attempting to stop");
        AudioEngine.setEffectOn(false);
        toggleEffectButton.setText("Start");
        isPlaying = false;
    }
    /*
    private int getRecordingDeviceId(){
        return ((AudioDeviceListEntry)recordingDeviceSpinner.getSelectedItem()).getId();
    }

    private int getPlaybackDeviceId(){
        return ((AudioDeviceListEntry)playbackDeviceSpinner.getSelectedItem()).getId();
    }

     */

    private boolean isRecordPermissionGranted() {
        return (ActivityCompat.checkSelfPermission(this, Manifest.permission.RECORD_AUDIO) ==
                PackageManager.PERMISSION_GRANTED);
    }

    private void requestRecordPermission(){
        ActivityCompat.requestPermissions(
                this,
                new String[]{Manifest.permission.RECORD_AUDIO},
                AUDIO_EFFECT_REQUEST);
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions,
                                           @NonNull int[] grantResults) {

        if (AUDIO_EFFECT_REQUEST != requestCode) {
            super.onRequestPermissionsResult(requestCode, permissions, grantResults);
            return;
        }

        if (grantResults.length != 1 ||
                grantResults[0] != PackageManager.PERMISSION_GRANTED) {

            // User denied the permission, without this we cannot record audio
            // Show a toast and update the status accordingly
            Toast.makeText(getApplicationContext(),
                    "User permission denied!",
                    Toast.LENGTH_LONG)
                    .show();
        } else {
            // Permission was granted, start live effect
            toggleEffect();
        }
    }

    @Override
    protected void onStart() {
        super.onStart();
        setVolumeControlStream(AudioManager.STREAM_MUSIC);
    }

    @Override
    protected void onResume() {
        super.onResume();
        AudioEngine.create();
        mAAudioRecommended = AudioEngine.isAAudioRecommended();
        AudioEngine.setAPI(apiSelection);
    }
    @Override
    protected void onPause() {
        stopEffect();
        AudioEngine.delete();
        super.onPause();
    }

    /**
     * A native method that is implemented by the 'noise' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();
}