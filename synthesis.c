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
                                 &synthesizer->data);

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
                void* user_data)
{
    user_data_t * data = (user_data_t*)user_data;
    float * out_stream = (float *)output;
    stereo_value_t data_point;

    for(int i = 0; i<FRAMES_PER_BUFFER; i++)
    {
        data_point = data->current_phase;

        syn_stereo_data_output(&out_stream, &data_point);

        data->current_phase.left = sin((double)data->time_stage*2*M_PI*data->frequency/SAMPLE_RATE);
        data->current_phase.right = data->current_phase.left;

        data->time_stage++;
    }

    return 0;
}
void syn_voice_generate()
{

}