#include <stdint.h>
#include <windows.h>
#include <stdio.h>
#include <stdbool.h>


#define TESTBUTTON_ID 211

MSG message; //message queue

LRESULT CALLBACK wnd_callback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam );  //Window event callback function

void CALLBACK Midi_callback(HMIDIIN hMidiIn, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);


typedef struct indicator_state_struct
{
    HWND h_main_window;
    HINSTANCE hi_main_window;
    INT i_cmdshow;

    LPSTR wnd_class_name[64];

    UINT device_count;
    HWND h_device_count;
} application_instance_t;

typedef struct midi_device
{
    LPHMIDIIN dev_handle;
    LPUINT dev_id;
    bool dev_is_connected;
    void* midi_callback;

} midi_device_t;


application_instance_t application;
midi_device_t keyboard;
void indicator_update(application_instance_t * instance);

INT register_class_instance(HINSTANCE hinstance, INT cmdshow);

void create_window_layout(application_instance_t *instance);

void midi_connect(midi_device_t * device);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

    register_class_instance(hInstance,nCmdShow);


    HWND main_window_handle = CreateWindowEx(WS_EX_CLIENTEDGE, application.wnd_class_name, "Window", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 400, 400, NULL, NULL, hInstance, NULL);

    application.h_main_window = main_window_handle;

    create_window_layout( &application);

    
    if(!main_window_handle)
    {
        return -1;
    }

    keyboard.midi_callback = (void*)Midi_callback;\
    keyboard.dev_id = 0;
    midi_connect(&keyboard);
//    if(res== MMSYSERR_NOERROR)
//    {
//        MessageBox(NULL,"Dziala cos","  ",0);
//    }
    ShowWindow(main_window_handle, nCmdShow);
    UpdateWindow(main_window_handle);

    application.device_count = midiInGetNumDevs();
    midiInStart(dev_handle);
    while(GetMessage(&message,NULL,0,0))
    {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    return 0;
}


INT register_class_instance(HINSTANCE hinstance, INT cmdshow)
{
    application.hi_main_window = hinstance;
    application.i_cmdshow = cmdshow;

    memcpy(application.wnd_class_name,"wndClass",sizeof "wndClass");
    WNDCLASSEX wnd_class_instance;

    wnd_class_instance.hInstance =  application.hi_main_window;
    wnd_class_instance.lpszClassName = application.wnd_class_name;

    wnd_class_instance.cbSize = sizeof wnd_class_instance;
    wnd_class_instance.cbClsExtra = 0;
    wnd_class_instance.cbWndExtra = 0;
    wnd_class_instance.style = 0;
    wnd_class_instance.lpfnWndProc = wnd_callback;
    wnd_class_instance.hIcon = LoadIcon(NULL,IDI_APPLICATION);
    wnd_class_instance.hCursor = LoadCursor(NULL,IDC_ARROW);
    wnd_class_instance.hbrBackground = (HBRUSH)(COLOR_WINDOW | 1);
    wnd_class_instance.lpszMenuName = NULL;
    wnd_class_instance.hIconSm = LoadIcon(NULL,IDI_APPLICATION);

    if(!RegisterClassEx(&wnd_class_instance))
    {
        return -1;
    }
}

LRESULT CALLBACK wnd_callback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_COMMAND:
            switch (wParam) {
                case TESTBUTTON_ID:
                    indicator_update(&application);
                    break;
                default:
                    break;
            }
            break;
        case WM_DESTROY:
            PostQuitMessage( 0 );
            break;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;

        default:
            return DefWindowProc(hwnd,msg,wParam,lParam);

    }
    return 0;
}


void create_window_layout(application_instance_t *instance)
{
    CreateWindowEx(0, "STATIC", "Simple MIDI synth", WS_CHILD| WS_VISIBLE, 0 , 0 , 200, 30, instance->h_main_window, NULL, instance->hi_main_window, NULL);
    instance->h_device_count=CreateWindowEx(0, "STATIC", "Test", WS_CHILD | WS_VISIBLE, 0 , 100 , 200, 30, instance->h_main_window, NULL, instance->hi_main_window, NULL);
 CreateWindowEx(0, "BUTTON", "Test", WS_CHILD | WS_VISIBLE, 100, 100, 150, 30, instance->h_main_window, (HMENU) TESTBUTTON_ID, instance->hi_main_window, NULL );
}


void indicator_update(application_instance_t *instance)
{
    instance->device_count = midiInGetNumDevs();
    DestroyWindow(instance->h_device_count);
    char buffer[64];
    sprintf(buffer, "liczba %d", instance->device_count);
    instance->h_device_count=CreateWindowEx(0, "STATIC", buffer, WS_CHILD | WS_VISIBLE, 0 , 100 , 200, 30, instance->h_main_window, NULL, instance->hi_main_window, NULL);

    ShowWindow(instance->h_main_window, instance->i_cmdshow);
    UpdateWindow(instance->h_main_window);
}

void CALLBACK Midi_callback(HMIDIIN hMidiIn, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
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
            break;
        case MIM_LONGDATA:
            printf("wMsg=MIM_LONGDATA\n");
            break;
        case MIM_ERROR:
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


void midi_connect(midi_device_t * device)
{
    midiInOpen(&(device->dev_handle),0,device->midi_callback,NULL,CALLBACK_FUNCTION);
    midiInStart(device->dev_handle);
}