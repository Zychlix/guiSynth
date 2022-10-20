#include <stdint.h>
#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include "synthesis.h"
#include "gui.h"





MSG message; //message queue


void CALLBACK midi_callback(HMIDIIN hMidiIn, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);



typedef struct midi_device
{
    LPHMIDIIN dev_handle;
    LPUINT dev_id;
    bool dev_is_connected;
    void* midi_callback;

} midi_device_t;

typedef struct midi_data
{
    uint8_t status;
    uint8_t note;
    uint8_t velocity;
} midi_data_t;



midi_device_t midi_keyboard_device;
keyboard_state_t gui_keyboard;
synthesizer_t synth;
gui_application_instance_t application;


void keyboard_operator(keyboard_state_t * gui_keyboard, midi_data_t * note);

INT midi_connect(midi_device_t * device);

midi_data_t midi_data_pack(DWORD instance, DWORD param1, DWORD param2);


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    gui_register_class_instance(hInstance, nCmdShow,wnd_callback);


    application.h_main_window = CreateWindowEx(WS_EX_CLIENTEDGE, application.wnd_class_name, "DenaturiX Sound 2000", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 400, 400, NULL, NULL, hInstance, NULL);

    if(!application.h_main_window)
    {
        return -1;
    }



    synth.data.pan = 0;
    synth.voice_main.master_volume= 1.f;
    synth.voice_main.volume = 1.f;

    synth.voice_main.attack = 100;
    synth.voice_main.decay = 10;
    synth.voice_main.sustain = 0.8;
    synth.voice_main.release = 10;

    syn_initialize(&synth);


    gui_keyboard.keyboard_size = 24;
    for (int i = 0; i < 24; ++i) {
        gui_keyboard.notes[i] = NOTE_RELEASED;

    }

    gui_create_window_layout(&application);


    midi_keyboard_device.midi_callback = (void*)midi_callback;
    midi_keyboard_device.dev_id = 0;

    ShowWindow(application.h_main_window, nCmdShow);
    UpdateWindow(application.h_main_window);

  //  application.device_count = midiInGetNumDevs();
  //
    gui_keyboard.origin.x = 5;
    gui_keyboard.origin.y = 270;

    gui_draw_keyboard(application.h_main_window,&gui_keyboard);

    while(GetMessage(&message,NULL,0,0))
    {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    return message.wParam;
}




LRESULT CALLBACK wnd_callback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_CREATE:


        case WM_VSCROLL:

            if((HWND)lParam == application.h_volume_slider) {       //our lovely volume controll handle

                SCROLLINFO si;
                ZeroMemory( & si, sizeof( si ) );

                //application.s_volume_slider.nPos = application.s_volume_slider.nTrackPos;
                si.cbSize = sizeof si;
                si.fMask = SIF_POS|SIF_PAGE | SIF_TRACKPOS;
                GetScrollInfo (application.h_volume_slider, SB_CTL, &si);
                int pos =si.nPos;
                si.nPos = si.nTrackPos;
                SetScrollInfo(application.h_volume_slider,SB_CTL, &si,1);
                synth.voice_main.master_volume= (exp((1-(pos/1000.f)))-1)/1.71;
                printf("%d \n", pos);
            }



            break;
        case WM_COMMAND:
            switch (wParam) {
                case CONNECT_BUTTON_ID:
                    if(!midi_connect(&midi_keyboard_device)){
                        gui_indicator_update(&application);
                    }

                    break;

                default:
                    break;
            }
            break;
        case WM_DESTROY:
            PostQuitMessage( 0 );
            Pa_Sleep(1000);
            syn_deinitialize(&synth);
            break;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;

        default:
            return DefWindowProc(hwnd,msg,wParam,lParam);

    }
    return 0;
}



void CALLBACK midi_callback(HMIDIIN hMidiIn, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
    switch(wMsg) {
        case MIM_OPEN:
            printf("wMsg=MIM_OPEN\n");
            break;
        case MIM_CLOSE:
            printf("wMsg=MIM_CLOSE\n");
            break;
        case MIM_DATA:
            printf("wMsg=MIM_DATA, dwInstance=%08x, dwParam1=%08x, dwParam2=%08x\n", dwInstance, dwParam1, dwParam2);
            midi_data_t data = midi_data_pack(dwInstance,dwParam1,dwParam2);
            application.device_count = data.note;

            printf("status: %d note %d velocity %d \n", data.status, data.note, data.velocity);

            if(data.status == MIDI_KEY_PRESSED)
            {
                syn_set_note(&synth,data.note);
                syn_play_note(&synth);
            }
            if(data.status == MIDI_KEY_DEPRESSED)
            {
                syn_stop_note(&synth);
            }
            keyboard_operator(&gui_keyboard, &data);
            gui_draw_keyboard(application.h_main_window,&gui_keyboard);
            break;
        case MIM_LONGDATA:
            printf("wMsg=MIM_LONGDATA\n");
            break;
        case MIM_ERROR:
            MessageBox(NULL, "Device disconnected", "MIDI error", 0);
            printf("wMsg=MIM_ERROR\n");
            break;
        case MIM_LONGERROR:
            printf("wMsg=MIM_LONGERROR\n");
            break;
        case MIM_MOREDATA:
            printf("wMsg=MIM_MOREDATA\n");
            break;
        default:
            printf("wMsg = unknown\n");
            break;
    }
    return;
}

INT midi_connect(midi_device_t * device)
{
    INT device_count = midiInGetNumDevs();

    if(device_count) {
        midiInOpen(&(device->dev_handle), 0, device->midi_callback, NULL, CALLBACK_FUNCTION);
        midiInStart(device->dev_handle);
    }
    else
    {
        MessageBox(NULL,"No midi devices detected", "Connection error",MB_ICONERROR);
        return -1;
    }
    return 0;
}

midi_data_t midi_data_pack(DWORD instance, DWORD param1, DWORD param2)
{
    midi_data_t data;
    data.status = param1;                   //As x86 is little endian, the first byte is the LSB - midi status byte
    data.note = *(((uint8_t*)&param1)+1);   //The second byte of the variable. midi note pressed
    data.velocity = *(((uint8_t*)&param1)+2); // midi velocity

    return data;
}


void keyboard_operator(keyboard_state_t * gui_keyboard, midi_data_t * note)
{
    int note_offset = 48;
    if(note->note>=48 & note->note <= 72)
    {
        if(note->status == MIDI_KEY_PRESSED)
        {
            gui_keyboard->notes[note->note - note_offset] = NOTE_PRESSED;
        }
        if(note->status == MIDI_KEY_DEPRESSED)
        {
            gui_keyboard->notes[note->note - note_offset] = NOTE_RELEASED;
        }
    }
}