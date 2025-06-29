#ifndef _VRE_APP_WIZARDTEMPLATE_
#define	_VRE_APP_WIZARDTEMPLATE_

#include "vmsys.h"
#include "vmio.h"
#include "vmgraph.h"
#include "vmchset.h"
#include "vmstdlib.h"
#include "stdio.h"
#include "vmsms.h"
#include "string.h"
#include "stdint.h"
#include <time.h>
//#include "vmmm.h"

VMINT		layer_hdl[1];				////layer handle array.

void handle_sysevt(VMINT message, VMINT param);
void handle_keyevt(VMINT event, VMINT keycode);
void mre_draw_black_rectangle(void);
void mre_read_sms();
static void mre_sms_read_callback_function(vm_sms_callback_t *callback_data);
static void mre_msg_content_display(void);
void create_auto_full_path_name(VMWSTR result, VMWSTR fname);
VMINT string_width(VMWCHAR *whead, VMWCHAR *wtail);
void display_text_line_w(VMUINT8 *disp_buf,VMWSTR str, VMINT x, VMINT y, VMINT width, VMINT height,VMINT betlines,VMINT startLine, VMINT color);
void create_app_txt_filename(VMWSTR text, VMSTR extt);
void create_auto_filename(VMWSTR text, VMSTR extt);

#endif