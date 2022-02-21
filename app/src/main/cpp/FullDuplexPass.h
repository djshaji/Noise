/*
 * Copyright 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SAMPLES_FULLDUPLEXPASS_H
#define SAMPLES_FULLDUPLEXPASS_H

#include <dlfcn.h>
#include "ladspa.h"
#include "FullDuplexStream.h"

class FullDuplexPass : public FullDuplexStream {
public:
    void (*connect_port [2])(LADSPA_Handle Instance,
                         unsigned long Port,
                         LADSPA_Data * DataLocation);
    void (*run [2])(LADSPA_Handle Instance,
                unsigned long SampleCount);


    void * handle [2] ;
    unsigned long mode = 0 ;
    const LADSPA_Descriptor * descriptor [2] ;
    virtual oboe::DataCallbackResult
    onBothStreamsReady(
            std::shared_ptr<oboe::AudioStream> inputStream,
            const void *inputData,
            int   numInputFrames,
            std::shared_ptr<oboe::AudioStream> outputStream,
            void *outputData,
            int   numOutputFrames) {
        // This code assumes the data format for both streams is Float.
        const float *inputFloats = static_cast<const float *>(inputData);
        float *outputFloats = static_cast<float *>(outputData);

        // It also assumes the channel count for each stream is the same.
        int32_t samplesPerFrame = outputStream->getChannelCount();
        int32_t numInputSamples = numInputFrames * samplesPerFrame;
        int32_t numOutputSamples = numOutputFrames * samplesPerFrame;

        // It is possible that there may be fewer input than output samples.
        int32_t samplesToProcess = std::min(numInputSamples, numOutputSamples);

        for (int32_t i = 0; i < samplesToProcess; i++) {
            *outputFloats++ = *inputFloats++ * 0.95; // do some arbitrary processing
        }

        /*
        if (handle == NULL) {
            handle = descriptor -> instantiate (descriptor, 48000) ;
        }
        */
        LADSPA_Data amplitude = 1 ;
        unsigned long bharti = mode ;
//            if (connect_port [bharti] == NULL) continue;
//            LOGD("plugin %d", bharti);
        if (bharti) {
//            LOGD("connecting ports 0, 1");
            LADSPA_Data freq = 440 ;
            connect_port[bharti](handle[bharti], 0, (LADSPA_Data *) &freq);
            connect_port[bharti](handle[bharti], 1, (LADSPA_Data *) &amplitude);
        } else {
            connect_port[bharti](handle [bharti], 0, (LADSPA_Data *) &amplitude);
        }

//        LOGD("connecting port %lu", bharti+1);
        connect_port[bharti](handle[bharti], bharti+1, (LADSPA_Data *) outputData);
//        LOGD("running plugin") ;
        run[bharti](handle[bharti], samplesToProcess);

        // If there are fewer input samples then clear the rest of the buffer.
        int32_t samplesLeft = numOutputSamples - numInputSamples;
        for (int32_t i = 0; i < samplesLeft; i++) {
            *outputFloats++ = 0.0; // silence
        }

//        OUT ;
        return oboe::DataCallbackResult::Continue;
    }
};
#endif //SAMPLES_FULLDUPLEXPASS_H
