#include "gui.h"

extern application_instance_t application;

void gui_draw_keyboard(HWND window, unsigned  int x, unsigned int y)
{
    HDC context;
    context = GetDC(window);

    POINT pos;
    MoveToEx(context, x, y, &pos);
    HPEN pen;
    pen = CreatePen(PS_SOLID,1,0x00FF00);
    SelectObject(context,pen);
    LineTo(context,200,200);

    ReleaseDC(window, context);
    return;
}

void gui_create_window_layout(application_instance_t *instance)
{
    CreateWindowEx(0, "STATIC", "Simple MIDI synth", WS_CHILD| WS_VISIBLE, 0 , 0 , 200, 30, instance->h_main_window, NULL, instance->hi_main_window, NULL);
    instance->h_device_counter=CreateWindowEx(0, "STATIC", "Device not connected", WS_CHILD | WS_VISIBLE, 0 , 100 , 200, 30, instance->h_main_window, NULL, instance->hi_main_window, NULL);
    CreateWindowEx(0, "BUTTON", "Connect", WS_CHILD | WS_VISIBLE, 0, 20, 70, 30, instance->h_main_window, (HMENU) CONNECT_BUTTON_ID, instance->hi_main_window, NULL );

    instance->h_volume_slider = CreateWindowEx(0,"SCROLLBAR",NULL,WS_CHILD|WS_VISIBLE|SBS_VERT, GUI_VOLUME_X_POS,20,20,100,instance->h_main_window,(HMENU)VOLUME_SLIDER_ID,instance->hi_main_window,NULL );
    CreateWindowEx(0, "STATIC", "Volume", WS_CHILD| WS_VISIBLE, GUI_VOLUME_X_POS , 0 , 20, 20, instance->h_main_window, NULL, instance->hi_main_window, NULL);

    application.s_volume_slider.cbSize = sizeof application.s_volume_slider;
    application.s_volume_slider.fMask = SIF_ALL;
    application.s_volume_slider.nMin = 0;
    application.s_volume_slider.nMax = 1000;
    application.s_volume_slider.nPage = 100;
    application.s_volume_slider.nPos = 0;
    SetScrollInfo (application.h_volume_slider, SB_CTL, &application.s_volume_slider, TRUE);



}


void gui_indicator_update(application_instance_t *instance)
{
    //instance->device_count = midiInGetNumDevs();
    DestroyWindow(instance->h_device_counter);
    char buffer[64];
    sprintf(buffer, "Device connected", instance->device_count);
    instance->h_device_counter=CreateWindowEx(0, "STATIC", buffer, WS_CHILD | WS_VISIBLE, 0 , 100 , 100, 30, instance->h_main_window, NULL, instance->hi_main_window, NULL);

    ShowWindow(instance->h_main_window, instance->i_cmdshow);
    UpdateWindow(instance->h_main_window);
}

INT gui_register_class_instance(HINSTANCE hinstance, INT cmdshow, WNDPROC * callback)
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
    wnd_class_instance.lpfnWndProc = callback;
    wnd_class_instance.hIcon = LoadIcon(NULL,IDI_APPLICATION);
    wnd_class_instance.hCursor = LoadCursor(NULL,IDC_ARROW);
    wnd_class_instance.hbrBackground = (HBRUSH)(COLOR_WINDOW | 1);
    wnd_class_instance.lpszMenuName = NULL;
    wnd_class_instance.hIconSm = LoadIcon(NULL,IDI_APPLICATION);

    if(!RegisterClassEx(&wnd_class_instance))
    {
        return -1;
    }
    return 0;
}

