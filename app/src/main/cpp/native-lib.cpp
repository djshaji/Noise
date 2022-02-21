#include <jni.h>
#include <string>
#include <android/log.h>
#define MODULE "JNI Bridge"
#include <logging_macros.h>
#include "Engine.h"


static const int kOboeApiAAudio = 0;
static const int kOboeApiOpenSLES = 1;

static Engine * engine = nullptr;

void * handle [2] ;
LADSPA_Data amplitude = 1 ;
LADSPA_Data frequency = 440 ;
const LADSPA_Descriptor *descriptor [2];

extern "C"
void loadPlugin (void) {
    LOGD("going to load library") ;
    void * sofile = dlopen ("libmodule.so", RTLD_LAZY);
    if (sofile == NULL) {
        LOGF("failed to load shared library! %s", dlerror());
        return;
    }

    LOGD("Loaded shared library [ok]");
    LADSPA_Descriptor_Function fDescriptorFunction = (LADSPA_Descriptor_Function) dlsym(sofile, "ladspa_descriptor");
    if (fDescriptorFunction == NULL) {
        LOGF("cannot find descriptor function: %s", dlerror());
        return ;
    } else
        LOGD("Descriptor Function [ok]") ;
    descriptor [0] = fDescriptorFunction (0);
    if (descriptor [0] == NULL)
        LOGE("descriptor returned null") ;
    else
        LOGD("Descriptor [ok]");
    LOGD("loaded plugin %s", descriptor [0]->Name);
    handle [0] = descriptor [0]->instantiate(descriptor[0], 48000) ;
    LOGD("plugin instantiated [ok]");
//    LOGD("ports connected [ok]");
    engine -> mFullDuplexPass . descriptor [0] = descriptor [0] ;
    engine -> mFullDuplexPass . handle [0] = handle [0] ;
    engine -> mFullDuplexPass .connect_port [0] = descriptor [0] ->connect_port ;
    engine -> mFullDuplexPass.run [0] = descriptor  [0]-> run ;
}

extern "C"
void loadPluginSine (void) {
    LOGD("going to load library") ;
    void * sofile = dlopen ("libsine.so", RTLD_LAZY);
    if (sofile == NULL) {
        LOGF("failed to load shared library! %s", dlerror());
        return;
    }

    LOGD("Loaded shared library [ok]");
    LADSPA_Descriptor_Function fDescriptorFunction = (LADSPA_Descriptor_Function) dlsym(sofile, "ladspa_descriptor");
    if (fDescriptorFunction == NULL) {
        LOGF("cannot find descriptor function: %s", dlerror());
        return ;
    } else
        LOGD("Descriptor Function [ok]") ;
    descriptor [1] = fDescriptorFunction (0);
    if (descriptor [1] == NULL)
        LOGE("descriptor returned null") ;
    else
        LOGD("Descriptor [ok]");
    LOGD("loaded plugin %s", descriptor[1]->Name);
    handle [1] = descriptor[1]->instantiate(descriptor[1], 48000) ;
    LOGD("plugin instantiated [ok]");
//    LOGD("ports connected [ok]");
    engine -> mFullDuplexPass . descriptor[1] = descriptor [1];
    engine -> mFullDuplexPass . handle [1] = handle [1];
    engine -> mFullDuplexPass .connect_port [1] = descriptor[1] ->connect_port ;
    engine -> mFullDuplexPass.run [1] = descriptor[1] -> run ;
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_shajikhan_ladspa_noise_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_shajikhan_ladspa_noise_AudioEngine_create(JNIEnv *env, jclass clazz) {
    if (engine == nullptr) {
        engine = new Engine () ;
        loadPlugin() ;
        loadPluginSine();
    }

    return (engine != nullptr) ? JNI_TRUE : JNI_FALSE;
}
extern "C"
JNIEXPORT jboolean JNICALL
Java_com_shajikhan_ladspa_noise_AudioEngine_isAAudioRecommended(JNIEnv *env, jclass clazz) {
    if (engine == nullptr) {
        LOGE(
                "Engine is null, you must call createEngine "
                "before calling this method");
        return JNI_FALSE;
    }

    return engine->isAAudioRecommended() ? JNI_TRUE : JNI_FALSE;
}
extern "C"
JNIEXPORT jboolean JNICALL
Java_com_shajikhan_ladspa_noise_AudioEngine_setAPI(JNIEnv *env, jclass clazz, jint api_type) {
    if (engine == nullptr) {
        LOGE(
                "Engine is null, you must call createEngine "
                "before calling this method");
        return JNI_FALSE;
    }

    oboe::AudioApi audioApi;
    switch (api_type) {
        case kOboeApiAAudio:
            audioApi = oboe::AudioApi::AAudio;
            break;
        case kOboeApiOpenSLES:
            audioApi = oboe::AudioApi::OpenSLES;
            break;
        default:
            LOGE("Unknown API selection to setAPI() %d", api_type);
            return JNI_FALSE;
    }

    return engine->setAudioApi(audioApi) ? JNI_TRUE : JNI_FALSE;
}
extern "C"
JNIEXPORT jboolean JNICALL
Java_com_shajikhan_ladspa_noise_AudioEngine_setEffectOn(JNIEnv *env, jclass clazz,
                                                        jboolean is_effect_on) {
    if (engine == nullptr) {
        LOGE(
                "Engine is null, you must call createEngine before calling this "
                "method");
        return JNI_FALSE;
    }

    return engine->setEffectOn(is_effect_on) ? JNI_TRUE : JNI_FALSE;
}
extern "C"
JNIEXPORT void JNICALL
Java_com_shajikhan_ladspa_noise_AudioEngine_setRecordingDeviceId(JNIEnv *env, jclass clazz,
                                                                 jint device_id) {
    if (engine == nullptr) {
        LOGE(
                "Engine is null, you must call createEngine before calling this "
                "method");
        return;
    }

    engine->setRecordingDeviceId(device_id);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_shajikhan_ladspa_noise_AudioEngine_setPlaybackDeviceId(JNIEnv *env, jclass clazz,
                                                                jint device_id) {
    if (engine == nullptr) {
        LOGE(
                "Engine is null, you must call createEngine before calling this "
                "method");
        return;
    }

    engine->setPlaybackDeviceId(device_id);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_shajikhan_ladspa_noise_AudioEngine_delete(JNIEnv *env, jclass clazz) {
    if (engine) {
        engine->setEffectOn(false);
        delete engine;
        engine = nullptr;
    }

}
extern "C"
JNIEXPORT void JNICALL
Java_com_shajikhan_ladspa_noise_AudioEngine_native_1setDefaultStreamValues(JNIEnv *env,
                                                                           jclass clazz,
                                                                           jint default_sample_rate,
                                                                           jint default_frames_per_burst) {
    oboe::DefaultStreamValues::SampleRate = (int32_t) default_sample_rate;
    oboe::DefaultStreamValues::FramesPerBurst = (int32_t) default_frames_per_burst;
}


extern "C"
JNIEXPORT void JNICALL
Java_com_shajikhan_ladspa_noise_AudioEngine_setMode(JNIEnv *env, jclass clazz, jint mode) {
    // TODO: implement setMode()
    if (engine == nullptr) return ;
    LOGD("setting mode to %d", mode) ;
    engine -> mFullDuplexPass .mode = mode;
}