#include "gui.h"

extern gui_application_instance_t application;


void gui_create_window_layout(gui_application_instance_t *instance)
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


void gui_indicator_update(gui_application_instance_t *instance)
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

void gui_draw_keyboard(HWND window, unsigned  int x, unsigned int y)
{
    HDC context;
    context = GetDC(window);
    POINT note_pos;
    note_pos.x = x;
    note_pos.y = y;
    int chromatic_note_counter = 0;
    for(int i = 0; i<20 ; i++)      //draw chromatic keys
    {
        int current_index = i%12;
        if(current_index == 2 || current_index == 4 || current_index == 7 || current_index == 9 || current_index == 11)
        {

        } else {
            note_pos.x += GUI_KEYBOARD_CHROMATIC_INTERVAL;
            gui_draw_chromatic_note(context, note_pos, 0);
        }

    }
    note_pos.x = x; //reset the position
    note_pos.y = y;

    for(int i = 0; i<20 ; i++)      //draw flat keys
    {
        int current_index = i%12;
        if(current_index == 2 || current_index == 4 || current_index == 7 || current_index == 9 || current_index == 11)
        {
            gui_draw_chromatic_note(context, note_pos,1);
        }
        else
        {
            note_pos.x += GUI_KEYBOARD_CHROMATIC_INTERVAL;
        }

    }
    //pamietaj o zwolnieniu zasobow rysunkowych
    ReleaseDC(window, context);
    return;
}


void gui_draw_chromatic_note(HDC context, POINT p, int type)
{
    POINT current_loc;

    POINT pos;
    MoveToEx(context, p.x, p.y, &pos);
    HPEN pen;
    HBRUSH brush = CreateSolidBrush(0x000000);
    pen = CreatePen(PS_SOLID,1,GUI_COLOR_BLACK);
    SelectObject(context,pen);

    POINT upper_left;
    POINT lower_right;

    upper_left.x = p.x;
    upper_left.y = p.y;
    if(type == 0) {
        lower_right.x = p.x + GUI_CHROMATIC_NOTE_WIDTH;
        lower_right.y = p.y + GUI_CHROMATIC_NOTE_HEIGHT;
    } else
    {
        lower_right.x = p.x + GUI_CHROMATIC_NOTE_WIDTH/3+2;
        lower_right.y = p.y + GUI_CHROMATIC_NOTE_HEIGHT/2 -GUI_SHARP_Y_OFFSET;
        SelectObject(context, brush);
    }

//    LineTo(context, upper_left.x,upper_left.y);
    Rectangle(context,upper_left.x,upper_left.y,lower_right.x, lower_right.y);
    Rectangle(context, 0,0,100,100);

    DeleteObject(pen);

}

void gui_draw_sharp_note(HDC context, POINT p)
{

}