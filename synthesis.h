#pragma once
#include <portaudio.h>
#include <math.h>
#include "stdio.h"
#include <stdint.h>

#define SAMPLE_RATE 44100
#define FRAMES_PER_BUFFER 256


typedef struct stereo
{
    float left;
    float right;
} stereo_value_t;

typedef struct user_data
{
    stereo_value_t current_phase; //As amplitude
    float frequency;
    uint64_t time_stage;

} user_data_t;

typedef struct synthesizer
{
    PaStream * stream;
    user_data_t data;
} synthesizer_t;



typedef struct voice
{


} voice_t;

stereo_value_t syn_voice_component(user_data_t * user_data);

int syn_initialize(synthesizer_t * synthesizer);

int syn_deinitialize(synthesizer_t * synthesizer);

void syn_stereo_data_output(float ** out_stream,stereo_value_t * value); //Outputs data points of a given sample rate to the output stream

void syn_voice_generate();
void syn_voice_update();

void syn_create_channel();
void syn_delete_channel();

int pa_callback(const void * input,
                void * output,
                unsigned long frame_count,
                const PaStreamCallbackTimeInfo *time_info,
                PaStreamCallbackFlags status,
                void* user_data);

int testo(void);