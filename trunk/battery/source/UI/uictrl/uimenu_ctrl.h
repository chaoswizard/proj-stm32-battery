#ifndef __UIMENU_CTRL_H__
#define __UIMENU_CTRL_H__
#include "uimmi_ctrl.h"


#ifdef __cplusplus
extern "C" {
#endif




void ui_menu_init(void);
void ui_menu_deinit(void);
void ui_menu_reset(void);
u_int8 ui_menu_bypass_proc(struct EVENT_NODE_ITEM *e);
u_int8 ui_menu_shotcut_proc(struct EVENT_NODE_ITEM *e);


#ifdef __cplusplus
}
#endif


#endif
