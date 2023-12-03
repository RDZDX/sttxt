#include "Sttxt.h"

vm_sms_msg_data_t *message_data = NULL;
VMINT16 message_id;
int filledDsplByLines = 0;
VMUINT16 content_buffer_size;
VMINT8 *content_buff;
VMINT8 *content_buffer;
VMUINT8 *buffer;
VMINT16 sms_k;
VMWCHAR f_wname[100];
VMWCHAR f_wname1[100];
VMFILE f_write;
VMUINT nwrite;
VMINT trigeris = 0;

void vm_main(void) {

    VMWCHAR file_name[100];
    layer_hdl[0] = -1;
    vm_reg_sysevt_callback(handle_sysevt);
    vm_reg_keyboard_callback(handle_keyevt);
    vm_font_set_font_size(VM_SMALL_FONT);
    create_app_txt_filename(file_name);
    create_auto_full_path_name(f_wname, file_name);

}

void handle_sysevt(VMINT message, VMINT param) {

    switch (message) {
        case VM_MSG_CREATE:
        case VM_MSG_ACTIVE:
            layer_hdl[0] =
                vm_graphic_create_layer(0, 0, vm_graphic_get_screen_width(),
                                        vm_graphic_get_screen_height(), -1);
            vm_graphic_set_clip(0, 0, vm_graphic_get_screen_width(),
                                vm_graphic_get_screen_height());
            buffer = vm_graphic_get_layer_buffer(layer_hdl[0]);
            vm_switch_power_saving_mode(turn_off_mode);
            break;

        case VM_MSG_PAINT:
            vm_switch_power_saving_mode(turn_off_mode);
            mre_draw_black_rectangle();
            //mre_read_sms();
            break;

        case VM_MSG_INACTIVE:
            vm_switch_power_saving_mode(turn_on_mode);
            if (layer_hdl[0] != -1) vm_graphic_delete_layer(layer_hdl[0]);
            break;

        case VM_MSG_QUIT:
            if (layer_hdl[0] != -1) vm_graphic_delete_layer(layer_hdl[0]);
            break;
    }
}

void handle_keyevt(VMINT event, VMINT keycode) {

    if (event == VM_KEY_EVENT_UP && keycode == VM_KEY_RIGHT_SOFTKEY) {
        if (layer_hdl[0] != -1) {
            vm_graphic_delete_layer(layer_hdl[0]);
            layer_hdl[0] = -1;
        }
        vm_exit_app();
    }

    if (event == VM_KEY_EVENT_UP && keycode == VM_KEY_LEFT_SOFTKEY) {
        trigeris = 0;
        mre_read_sms();
    }

    if (event == VM_KEY_EVENT_UP && keycode == VM_KEY_NUM1) {
       trigeris = 1;
       mre_read_sms();
    }
}

void mre_draw_black_rectangle(void) {

    buffer = vm_graphic_get_layer_buffer(layer_hdl[0]);
    vm_graphic_fill_rect(buffer, 0, 0, vm_graphic_get_screen_width(), vm_graphic_get_screen_height(), VM_COLOR_BLACK, VM_COLOR_BLACK);
    vm_graphic_flush_layer(layer_hdl, 1);
    filledDsplByLines = 0;
}

void mre_read_sms() {

    VMUINT16 msg_index = 0;
    int i;
    void *user_data = NULL;

    sms_k = vm_sms_get_box_size(VM_SMS_BOX_INBOX);

    user_data = vm_malloc(100);
    memset(user_data, 0, 100);

    message_data = vm_malloc(sizeof(vm_sms_msg_data_t));
    memset(message_data, 0, sizeof(vm_sms_msg_data_t));
    message_data->content_buff = (VMINT8 *)vm_malloc(100);
    memset(message_data->content_buff, 0, 100);
    message_data->content_buff_size = 100;

    for (i = 0; i<sms_k; ++i){

        message_id = vm_sms_get_msg_id(VM_SMS_BOX_INBOX, i);
        vm_sms_read_msg(message_id, 0, message_data, mre_sms_read_callback_function, user_data);
    }

    vm_free(user_data);
    user_data = NULL;

    vm_free(message_data->content_buff);
    vm_free(message_data);
    //message_data = NULL;
}

static void mre_sms_read_callback_function(vm_sms_callback_t *callback_data) {

    VMWCHAR file_name[100];
    if (callback_data->result == 1) {
        if (callback_data->cause == VM_SMS_CAUSE_NO_ERROR) {
            switch (callback_data->action) {
                case VM_SMS_ACTION_NONE:
                    break;
                case VM_SMS_ACTION_READ:
                    mre_msg_content_display();
                    return;
                default:
                    break;
            }
        } else {
        }
    } else {
    }

}

static void mre_msg_content_display(void) {

    VMWCHAR file_name[100];
    VMCHAR ascii_data[161]; //160 characters (GSM-7 encoding) or 70 characters (UCS-2 encoding) 

    content_buffer_size = message_data->content_buff_size;
    content_buffer = message_data->content_buff;

    vertical_scrolling_ucs2_text((VMWSTR)content_buffer);

    vm_ucs2_to_ascii (ascii_data, content_buffer_size, (VMWSTR)content_buffer);
    //vm_ucs2_to_ascii (ascii_data, wstrlen((VMWSTR)content_buffer) + 1, (VMWSTR)content_buffer);

    if (trigeris == 1) {
       create_auto_filename(file_name);
       create_auto_full_path_name(f_wname1, file_name);
       f_write = vm_file_open(f_wname1, MODE_CREATE_ALWAYS_WRITE, FALSE);
       if (f_write < 0){
          f_write = vm_file_open(f_wname1, MODE_APPEND, FALSE);
       }
       vm_file_write(f_write, ascii_data, strlen(ascii_data), &nwrite);
    } else {
       f_write = vm_file_open(f_wname, MODE_APPEND, FALSE);
       if (f_write < 0){
          f_write = vm_file_open(f_wname, MODE_CREATE_ALWAYS_WRITE, FALSE);
       }
       vm_file_write(f_write, ascii_data, strlen(ascii_data), &nwrite);
       vm_file_write(f_write, "\n",1, &nwrite);
    }

    vm_file_close(f_write);

}

void vertical_scrolling_ucs2_text(VMWSTR ucs2_string) {
    int max_height;
    int font_height = 16;
    int i;
    int abc = 0;

    if (filledDsplByLines == 0) {abc = 2;}
    if (filledDsplByLines == 20) {
       mre_draw_black_rectangle();
       filledDsplByLines = 0;
    }

    max_height = vm_graphic_get_screen_height();
    vm_graphic_clear_layer_bg(layer_hdl[0]);
    vm_graphic_textout_by_baseline(buffer, 0, filledDsplByLines + abc, ucs2_string, vm_wstrlen(ucs2_string), VM_COLOR_WHITE, 12);
    vm_graphic_flush_layer(layer_hdl, 1);
    filledDsplByLines += font_height;
}

void create_app_txt_filename(VMWSTR text) {

    VMWCHAR fullPath[100];
    VMWCHAR appName[100];
    VMCHAR asciiAppName[100];
    VMCHAR file_name[100];

    vm_get_exec_filename(fullPath);
    vm_get_filename(fullPath, appName);
    vm_ucs2_to_ascii(asciiAppName, wstrlen(appName) + 1, appName);
    memcpy(file_name, asciiAppName, strlen(asciiAppName) - 3);
    file_name[strlen(asciiAppName) - 3] = '\0';
    strcat(file_name, "txt");
    vm_ascii_to_ucs2(text, (strlen(file_name) + 1) * 2, file_name);

}

void create_auto_filename(VMWSTR text) {

    struct vm_time_t curr_time;
    VMCHAR fAutoFileName[100];
    int rand1 = rand() % 99;
    vm_get_time(&curr_time);
    //sprintf(fAutoFileName, "%02d%02d%02d%02d%02d.txt", curr_time.mon, curr_time.day, curr_time.hour, curr_time.min, curr_time.sec);
    sprintf(fAutoFileName, "%02d%02d%02d%02d%02d.txt", curr_time.mon, curr_time.day, curr_time.hour, rand1, curr_time.sec);
    vm_ascii_to_ucs2(text, (strlen(fAutoFileName) + 1) * 2, fAutoFileName);

}

void create_auto_full_path_name(VMWSTR result, VMWSTR fname) {

    VMINT drv;
    VMCHAR fAutoFileName[100];
    VMWCHAR wAutoFileName[100];

    if ((drv = vm_get_removable_driver()) < 0) {
       drv = vm_get_system_driver();
    }

    sprintf(fAutoFileName, "%c:\\", drv);
    vm_ascii_to_ucs2(wAutoFileName, (strlen(fAutoFileName) + 1) * 2, fAutoFileName);
    vm_wstrcat(wAutoFileName, fname);
    vm_wstrcpy(result, wAutoFileName);

}