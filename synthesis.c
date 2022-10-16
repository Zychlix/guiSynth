#include "synthesis.h"


int syn_initialize(synthesizer_t * synthesizer)
{
    int error = Pa_Initialize();
    if(error != paNoError) return -1;

    error = Pa_OpenDefaultStream(&synthesizer->stream,
                                 0,
                                 2,
                                 paFloat32,
                                 SAMPLE_RATE,
                                 FRAMES_PER_BUFFER,
                                 pa_callback,
                                 synthesizer); //It was here!

    if(error != paNoError) return -2;

    error = Pa_StartStream(synthesizer->stream);
    if(error != paNoError) return -3;


    return 0;
}

int syn_deinitialize(synthesizer_t * synthesizer)
{
    int error;
    Pa_StopStream(synthesizer->stream);
    Pa_CloseStream(synthesizer->stream);

    error= (synthesizer->stream);
    if(error != paNoError)
    {
        return -1;
    }
    return error;
}


void syn_stereo_data_output(float ** out_stream,stereo_value_t * value)
{
    **out_stream = value->left;
    (*out_stream)++;
    **out_stream = value->right;
    (*out_stream)++;
}


int pa_callback(const void * input,
                void * output,
                unsigned long frame_count,
                const PaStreamCallbackTimeInfo *time_info,
                PaStreamCallbackFlags status,
                void* synthesizer)
{
    synthesizer_t * synth_instance = (synthesizer_t*) synthesizer;
    user_data_t * data = &(synth_instance->data);
    float * out_stream = (float *)output;
    stereo_value_t data_point;

    for(int i = 0; i<FRAMES_PER_BUFFER; i++)
    {
        data_point = data->current_phase;

        syn_stereo_data_output(&out_stream, &data_point);

        syn_voice_generate(synth_instance);

        synth_instance->voice_main.volume *=0.99999f;

        data->time_stage++;
    }


    return 0;
}
void syn_voice_generate(synthesizer_t * synthesizer)
{
    double frequency = (double)synthesizer->data.frequency;
    float volume = synthesizer->voice_main.volume;
    int time_stage = synthesizer->data.time_stage;
    float signal;
    signal = volume * sin(frequency*time_stage*2*M_PI/SAMPLE_RATE);
    synthesizer->data.current_phase.left = (1- synthesizer->data.pan)/2 * signal;
    synthesizer->data.current_phase.right = (1+ synthesizer->data.pan)/2 * signal;
}

void syn_voice_process(synthesizer_t * synthesizer)     //add voice processing. remembera about the update func
{

}


float syn_midi_note_to_freq(unsigned int note)
{
    return 440* exp2((note-69.f)/12.f);
}

void syn_play_note(synthesizer_t * synthesizer)
{
    synthesizer->voice_main.volume = 1.0f;
    synthesizer->data.frequency = syn_midi_note_to_freq(synthesizer->voice_main.note);
    synthesizer->voice_main.note_on = 1;
}
void syn_stop_note(synthesizer_t * synthesizer)
{
    synthesizer->voice_main.note_on = 0;
}


void syn_set_note(synthesizer_t * synthesizer, unsigned int midi_note)
{
    synthesizer->voice_main.note = midi_note;
}