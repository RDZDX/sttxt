#include "Sttxt.h"

vm_sms_msg_data_t *message_data = NULL;

VMINT ncharacter_height = -1;
VMINT nscreen_width = -1;
VMINT nscreen_height = -1;
VMINT filledDsplByLines = 0;

VMUINT8* screenbuf = NULL;

VMWCHAR f_wname[100];
VMWCHAR f_wname1[100];
VMFILE f_write;
VMUINT nwrite;
VMBOOL trigeris = VM_FALSE;

void vm_main(void) {

    VMWCHAR file_name[100];
    layer_hdl[0] = -1;
    vm_reg_sysevt_callback(handle_sysevt);
    vm_reg_keyboard_callback(handle_keyevt);
    vm_font_set_font_size(VM_SMALL_FONT);
    ncharacter_height = vm_graphic_get_character_height();
    nscreen_width = vm_graphic_get_screen_width();
    nscreen_height = vm_graphic_get_screen_height();
    create_app_txt_filename(file_name);
    create_auto_full_path_name(f_wname, file_name);

}

void handle_sysevt(VMINT message, VMINT param) {

    switch (message) {
        case VM_MSG_CREATE:
        case VM_MSG_ACTIVE:
            layer_hdl[0] = vm_graphic_create_layer(0, 0, vm_graphic_get_screen_width(), vm_graphic_get_screen_height(), -1);
            vm_graphic_set_clip(0, 0, vm_graphic_get_screen_width(), vm_graphic_get_screen_height());
            screenbuf = vm_graphic_get_layer_buffer(layer_hdl[0]);
            vm_switch_power_saving_mode(turn_off_mode);
            mre_draw_black_rectangle();
            break;

        case VM_MSG_PAINT:
            vm_switch_power_saving_mode(turn_off_mode);
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
        mre_read_sms();
        trigeris = VM_FALSE;
    }

    if (event == VM_KEY_EVENT_UP && keycode == VM_KEY_NUM1) {
        trigeris = VM_TRUE;
    }

}

void mre_draw_black_rectangle(void) {

    vm_graphic_fill_rect(screenbuf, 0, 0, vm_graphic_get_screen_width(), vm_graphic_get_screen_height(), VM_COLOR_BLACK, VM_COLOR_BLACK);
    vm_graphic_flush_layer(layer_hdl, 1);
    filledDsplByLines = 0;
}

void mre_read_sms() {

    VMUINT16 msg_index = 0;
    VMINT16 message_id;
    VMINT16 sms_k;
    int i;

    sms_k = vm_sms_get_box_size(VM_SMS_BOX_INBOX);

    if (sms_k == 0){return;}

    message_data = vm_malloc(sizeof(vm_sms_msg_data_t));
    memset(message_data, 0, sizeof(vm_sms_msg_data_t));
    message_data->content_buff = (VMINT8 *)vm_malloc((500 + 1) * 2);  //1sms size 160 ascii GSM-7 | 70 UCS-2 ? ; 1sms 153 Total 459 | 1sms 70 Total 201 ???
    memset(message_data->content_buff, 0, (500 + 1) * 2);
    message_data->content_buff_size = 500 * 2;

    for (i = 0; i<sms_k; ++i){

        message_id = vm_sms_get_msg_id(VM_SMS_BOX_INBOX, i);
        vm_sms_read_msg(message_id, 0, message_data, mre_sms_read_callback_function, NULL);
    }

    vm_free(message_data->content_buff);
    vm_free(message_data);
    //message_data = NULL;
}

static void mre_sms_read_callback_function(vm_sms_callback_t *callback_data) {

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
    VMCHAR ascii_data[1002]; //1sms size 160 ascii GSM-7 | 70 UCS-2 ? ; 1sms 153 Total 459 | 1sms 70 Total 201 ???

    vm_chset_convert(VM_CHSET_UTF16LE, VM_CHSET_UTF8, (VMSTR)message_data->content_buff, ascii_data, vm_wstrlen((VMWSTR)message_data->content_buff) + 1);
    //vm_chset_convert(VM_CHSET_UCS2, VM_CHSET_UTF8, (VMSTR)message_data->content_buff, ascii_data, vm_wstrlen((VMWSTR)message_data->content_buff) + 1);

    display_text_line_w(screenbuf, (VMWSTR)message_data->content_buff, 0, filledDsplByLines, nscreen_width, nscreen_height, 2, 1, VM_COLOR_WHITE);

    if (trigeris == VM_TRUE) {
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
       vm_file_write(f_write, "\n", 1, &nwrite);
    }

    vm_file_close(f_write);

}

void create_app_txt_filename(VMWSTR text) {

    VMWCHAR fullPath[100];
    VMWCHAR appName[100];
    VMWCHAR wfile_extension[8];
    VMCHAR file_extension[4] = "txt";

    vm_get_exec_filename(fullPath);
    vm_get_filename(fullPath, appName);
    vm_ascii_to_ucs2(wfile_extension, (strlen(file_extension) + 1) * 2, file_extension);
    vm_wstrncpy(text, appName, vm_wstrlen(appName) - 3);
    vm_wstrcat(text, wfile_extension);

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

VMINT string_width(VMWCHAR *whead, VMWCHAR *wtail) {

	VMWCHAR * wtemp = NULL;
	VMINT width = 0;
	if (whead == NULL || wtail == NULL)
	return 0;
	wtemp = (VMWCHAR *)vm_malloc((wtail - whead) * 2 + 2);
	
	if (wtemp == NULL)
	return 0;
	memset(wtemp, 0, (wtail - whead) * 2 + 2);
	memcpy(wtemp, whead, (wtail - whead) * 2);

	width = vm_graphic_get_string_width(wtemp);
	vm_free(wtemp);
	return width;
}

void display_text_line_w(VMUINT8 *disp_buf, VMWSTR str, VMINT x, VMINT y, VMINT width, VMINT height, VMINT betlines, VMINT startLine, VMINT color) {

	VMWCHAR *ucshead;
	VMWCHAR *ucstail;
	VMBOOL is_end = VM_FALSE;
	VMINT nheight = y; 
	VMINT nline_height ;
	VMINT nlines = 0;

        if (y == 0) {mre_draw_black_rectangle();}

	if (str == NULL||disp_buf==NULL||betlines < 0) {return;}

	nline_height = ncharacter_height + betlines;
	
	ucshead = str;
	ucstail = ucshead + 1;
	
	while(is_end == VM_FALSE)

	{
		if (nheight > height) {
                        mre_draw_black_rectangle();
                        nheight = 0;
		}
		while (1)
		{
			if (string_width(ucshead,ucstail)<=width)
			{
				ucstail ++;
			}
			else
			{
				nlines++;
				ucstail --;
				break;
			}
			if (0==vm_wstrlen(ucstail))
			{
				is_end = VM_TRUE;
				nlines++;
				break;
			}
		}
		if ( nlines >= startLine)
		{
			vm_graphic_textout(disp_buf, x, nheight, ucshead, (ucstail-ucshead), (VMUINT16)(color));
                        vm_graphic_flush_layer(layer_hdl, 1);
			nheight += nline_height;
                        filledDsplByLines = nheight;
		}
		ucshead = ucstail;
		ucstail ++;
	}
}