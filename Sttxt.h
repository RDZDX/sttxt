#ifndef _VRE_APP_WIZARDTEMPLATE_
#define	_VRE_APP_WIZARDTEMPLATE_

#include "vmsys.h"
#include "vmio.h"
#include "vmgraph.h"
#include "vmchset.h"
#include "vmstdlib.h"
#include "stdio.h"
//#include "ResID.h"
//#include "vm4res.h"
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
void vertical_scrolling_ucs2_text(VMWSTR ucs2_string);
void create_app_txt_filename(VMWSTR text);
void create_auto_filename(VMWSTR text);
void create_auto_full_path_name(VMWSTR result, VMWSTR fname);

#endif