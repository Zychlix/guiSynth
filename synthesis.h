#pragma once
#include <portaudio.h>
#include <math.h>
#include "stdio.h"
#include <stdint.h>

#define SAMPLE_RATE 44100
#define FRAMES_PER_BUFFER 256

/*
 * Michal Zychla 2022
 * A simple additive synthesizer library using Port Audio
 */

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
    float pan;

} user_data_t;

typedef struct voice
{
    float volume;
    int note;
    int note_on;

    float attack;
    float decay;
    float sustain;
    float release;

} voice_t;

typedef struct synthesizer
{
    PaStream * stream;
    user_data_t data;
    voice_t voice_main;
} synthesizer_t;





stereo_value_t syn_voice_component(user_data_t * user_data);

int syn_initialize(synthesizer_t * synthesizer);

int syn_deinitialize(synthesizer_t * synthesizer);

void syn_stereo_data_output(float ** out_stream,stereo_value_t * value); //Outputs data points of a given sample rate to the output stream

void syn_voice_generate(synthesizer_t * synthesizer);
void syn_envelope_processor(synthesizer_t * synthesizer);
void syn_voice_process(synthesizer_t * synthesizer);

float syn_midi_note_to_freq(unsigned int note);

void syn_set_note(synthesizer_t * synthesizer, unsigned int midi_note);
void syn_play_note(synthesizer_t * synthesizer);
void syn_stop_note(synthesizer_t * synthesizer);

void syn_voice_update();
void syn_create_channel();
void syn_delete_channel();

int pa_callback(const void * input,
                void * output,
                unsigned long frame_count,
                const PaStreamCallbackTimeInfo *time_info,
                PaStreamCallbackFlags status,
                void* synthesizer);
