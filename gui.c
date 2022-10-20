#include "gui.h"

extern gui_application_instance_t application;


int flat_table[] = {0,1,0,1,0,0,1,0,1,0,1,0};

void gui_create_window_layout(gui_application_instance_t *instance)
{
    CreateWindowEx(0, "STATIC", "Simple MIDI synth", WS_CHILD| WS_VISIBLE, 180 , 0 , 200, 30, instance->h_main_window, NULL, instance->hi_main_window, NULL);
    instance->h_device_counter=CreateWindowEx(0, "STATIC", "Disconnected!", WS_CHILD | WS_VISIBLE, 25 , 50 , 100, 30, instance->h_main_window, NULL, instance->hi_main_window, NULL);
    CreateWindowEx(0, "BUTTON", "Connect", WS_CHILD | WS_VISIBLE, 25, 20, 70, 30, instance->h_main_window, (HMENU) CONNECT_BUTTON_ID, instance->hi_main_window, NULL );

    //instance->controls.volume_slider.handle = CreateWindowEx(0,"SCROLLBAR",NULL,WS_CHILD|WS_VISIBLE|SBS_VERT, GUI_VOLUME_X_POS,20,20,100,instance->h_main_window,(HMENU)VOLUME_SLIDER_ID,instance->hi_main_window,NULL );
    //CreateWindowEx(0, "STATIC", "Volume", WS_CHILD| WS_VISIBLE, GUI_VOLUME_X_POS , 0 , 20, 20, instance->h_main_window, NULL, instance->hi_main_window, NULL);
    CreateWindowEx( 0, "BUTTON", "MIDI", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 20, 0, 100, 80,
                    application.h_main_window, NULL, application.hi_main_window, NULL );
    CreateWindowEx( 0, "BUTTON", "Envelope", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 20, 110, 130, 150,
                    application.h_main_window, NULL, application.hi_main_window, NULL );

    POINT pos;
    pos.x = GUI_VOLUME_X_POS;
    pos.y = 20;
    instance->controls.volume_slider.handle = gui_create_slider(&application,&application.controls.volume_slider.scroll,pos,"Vol");
    pos.x = 40;
    pos.y = GUI_ENVELOPE_Y_POS;
    instance->controls.attack_slider.handle = gui_create_slider(&application,&application.controls.attack_slider.scroll,pos,"Attack");
    pos.x = 90;
    pos.y = GUI_ENVELOPE_Y_POS;
    instance->controls.decay_slider.handle = gui_create_slider(&application,&application.controls.decay_slider.scroll,pos,"Decay");



//
//    application.controls.volume_slider.scroll.cbSize = sizeof application.controls.volume_slider.scroll;
//    application.controls.volume_slider.scroll.fMask = SIF_ALL;
//    application.controls.volume_slider.scroll.nMin = 0;
//    application.controls.volume_slider.scroll.nMax = 1000;
//    application.controls.volume_slider.scroll.nPage = 0;
//    application.controls.volume_slider.scroll.nPos = 0;
  //  SetScrollInfo (application.controls.volume_slider.handle, SB_CTL, &application.controls.volume_slider.scroll, TRUE);



}


void gui_indicator_update(gui_application_instance_t *instance)
{
    //instance->device_count = midiInGetNumDevs();
    DestroyWindow(instance->h_device_counter);
    char buffer[64];
    sprintf(buffer, "Connected!", instance->device_count);
    instance->h_device_counter=CreateWindowEx(0, "STATIC", buffer, WS_CHILD | WS_VISIBLE, 25 , 50 , 80, 20, instance->h_main_window, NULL, instance->hi_main_window, NULL);

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

void gui_draw_keyboard(HWND window, keyboard_state_t * keyboard)
{
    HDC context;
    context = GetDC(window);
    POINT origin_point;
    origin_point.x = keyboard->origin.x;
    origin_point.y = keyboard->origin.y;

    for(int i = 0; i<keyboard->keyboard_size ; i++)      //draw chromatic keys
    {
        int current_index = i%12;
        if(flat_table[current_index])
        {

        } else {
            origin_point.x += GUI_KEYBOARD_CHROMATIC_INTERVAL;
            gui_draw_chromatic_note(context, origin_point, NOTE_CHROMATIC, keyboard->notes[i]);
        }

    }
    origin_point.x = keyboard->origin.x;
    origin_point.y = keyboard->origin.y; //Reset origin position

    for(int i = 0; i<keyboard->keyboard_size ; i++)      //draw flat keys
    {
        int current_index = i%12;
        if(flat_table[current_index])
        {
            gui_draw_chromatic_note(context, origin_point, NOTE_FLAT, keyboard->notes[i]);
        }
        else
        {
            origin_point.x += GUI_KEYBOARD_CHROMATIC_INTERVAL;
        }

    }
    //pamietaj o zwolnieniu zasobow rysunkowych
    ReleaseDC(window, context);
    return;
}


void gui_draw_chromatic_note(HDC context, POINT p, note_type_t type, note_state_t state)
{
    POINT current_loc;

    POINT pos;
    MoveToEx(context, p.x, p.y, &pos);

    HPEN pen = CreatePen(PS_SOLID,1,GUI_COLOR_BLACK);
    HBRUSH brush_black = CreateSolidBrush(GUI_COLOR_BLACK);
    HBRUSH brush_gray = CreateSolidBrush(0x808080);
    HBRUSH brush_white = CreateSolidBrush(0xFFFFFF);


    SelectObject(context,pen);

    POINT upper_left;
    POINT lower_right;

    upper_left.x = p.x;
    upper_left.y = p.y;



    if(type == NOTE_CHROMATIC) {
        lower_right.x = p.x + GUI_CHROMATIC_NOTE_WIDTH;
        lower_right.y = p.y + GUI_CHROMATIC_NOTE_HEIGHT;
    } else
    {
        upper_left.x += 14;
        upper_left.y += 0;
        lower_right.x = upper_left.x + GUI_CHROMATIC_NOTE_WIDTH/3 +2;
        lower_right.y = upper_left.y + GUI_CHROMATIC_NOTE_HEIGHT/2 -GUI_SHARP_Y_OFFSET;


    }
    if(type == NOTE_FLAT) {
        if (state == NOTE_PRESSED ) {
            SelectObject(context, brush_gray);
        } else {
            SelectObject(context, brush_black);
        }
    }
    else
    {
        if (state == NOTE_PRESSED ) {
            SelectObject(context, brush_gray);
        } else {
            SelectObject(context, brush_white);
        }
    }

//    LineTo(context, upper_left.x,upper_left.y);
//    Rectangle(context, 0,0,100,100);

    Rectangle(context,upper_left.x,upper_left.y,lower_right.x, lower_right.y);

    DeleteObject(pen);
    DeleteObject(brush_black);
    DeleteObject(brush_gray);
    DeleteObject(brush_white);

}


HWND gui_create_slider(gui_application_instance_t *instance,SCROLLINFO* bar, POINT position, const LPCSTR * name)
{
    HWND pot;

    pot = CreateWindowEx(0,"SCROLLBAR",NULL,WS_CHILD|WS_VISIBLE|SBS_VERT, position.x,position.y,20,100,instance->h_main_window,(HMENU)VOLUME_SLIDER_ID,instance->hi_main_window,NULL );
    bar->cbSize = sizeof application.controls.volume_slider.scroll;
    bar->fMask = SIF_ALL;
    bar->nMin = 0;
    bar->nMax = 1000;
    bar->nPage = 0;
    bar->nPos = 0;
    SetScrollInfo (pot, SB_CTL, bar, TRUE);

    CreateWindowEx(0, "STATIC", name, WS_CHILD| WS_VISIBLE, position.x , position.y-15 , 50, 20, instance->h_main_window, NULL, instance->hi_main_window, NULL);

    return pot;
}

float gui_get_slider_value(knob_t * knob,HWND param)
{
    SCROLLINFO * si = &knob->scroll;

    //ZeroMemory( si, sizeof( *si ) );
    //si->cbSize = sizeof *si;
    si->fMask = SIF_POS|SIF_PAGE | SIF_TRACKPOS;
    GetScrollInfo (param, SB_CTL, si);

    int pos =si->nPos;
    si->nPos = si->nTrackPos;
    SetScrollInfo(param,SB_CTL, si,1);

    return 1-(pos/1000.f);
}


