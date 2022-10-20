#pragma once

#include <windows.h>
#include <stdio.h>

#define NOTE_COUNT 24

#define CONNECT_BUTTON_ID 211
#define VOLUME_SLIDER_ID  212

#define MIDI_KEY_PRESSED 144
#define MIDI_KEY_DEPRESSED 128

#define GUI_VOLUME_X_POS 350

#define GUI_COLOR_BLACK 0x000000

#define GUI_CHROMATIC_NOTE_HEIGHT 60
#define GUI_CHROMATIC_NOTE_WIDTH 20
#define GUI_KEYBOARD_CHROMATIC_INTERVAL (GUI_CHROMATIC_NOTE_WIDTH+1)
#define GUI_SHARP_Y_OFFSET -10

typedef enum note_type {
    NOTE_CHROMATIC,
    NOTE_FLAT
} note_type_t;

typedef enum note_state {
    NOTE_PRESSED,
    NOTE_RELEASED
} note_state_t;

typedef struct indicator_state_struct
{
    HWND h_main_window;
    HINSTANCE hi_main_window;
    INT i_cmdshow;

    LPSTR wnd_class_name[64];

    UINT device_count;

    HWND h_device_counter;
    HWND h_volume_slider;
    SCROLLINFO s_volume_slider;
} gui_application_instance_t;

typedef struct keyboard_state
{
    note_state_t notes[NOTE_COUNT];
    unsigned int keyboard_size;
    POINT origin;
} keyboard_state_t;



LRESULT CALLBACK wnd_callback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

void gui_draw_keyboard(HWND window, keyboard_state_t * keyboard);

void gui_indicator_update(gui_application_instance_t * instance);
void gui_create_window_layout(gui_application_instance_t *instance);

INT gui_register_class_instance(HINSTANCE hinstance, INT cmdshow, WNDPROC * callback);

void gui_draw_chromatic_note(HDC context, POINT p, note_type_t type, note_state_t state);
