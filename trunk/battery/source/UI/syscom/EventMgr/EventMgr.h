#ifndef __EVENT_MGR_H__
#define __EVENT_MGR_H__
#include "SmMgr.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef u_int8 EVENT_NODE_HANDLE;
typedef u_int8 EVENT_NODE_COUNT;

#define MSG_IS_VALID(msg)      (((msg) > EVENT_INVALID) && ((msg) < EVENT_END))

#define MSG_IS_KEY(msg)        (((msg) >= EVENT_KEY_NUM_0) && ((msg) <= EVENT_KEY_COMBIN))
#define MSG_IS_SM(msg)         (((msg) >= EVENT_SM_INIT) && ((msg) <= EVENT_SM_EXIT))
#define MSG_IS_SYS(msg)        (((msg) >= EVENT_SYS_INIT) && ((msg) <= EVENT_SYS_EXIT))
#define MSG_IS_CANCLE(msg)     ((msg) == EVENT_KEY_ESC)
#define MSG_IS_ENTER(msg)      ((msg) == EVENT_KEY_OK)
#define MSG_IS_STOP(msg)       ((msg) == EVENT_KEY_STOP)
#define MSG_IS_START(msg)      ((msg) == EVENT_KEY_RUN)
#define MSG_IS_ENTRY(msg)      ((msg) == EVENT_SM_ENTRY)

#define MSG_IS_DIGIT_KEY(msg)  (((msg) >= EVENT_KEY_NUM_0) && ((msg) <= EVENT_KEY_NUM_9))

//----------------------------------------------------------------------
union SM_EVENT_PARAM {
    /* if msg == EVENT_SM_INIT */
    //struct {SM_NODE_HANDLE me;} init;
    /* if msg == EVENT_SM_TRANS */
    struct {SM_NODE_HANDLE target; u_int8 quitCurrent;} trans;
    /* if msg == EVENT_SM_RETURN */
    struct {SM_NODE_COUNT retLvl;} ret;
    /* if msg == EVENT_SM_EXIT */
    //struct {SM_NODE_HANDLE me;} exit;
};
//----------------------------------------------------------------------
union KEY_EVENT_PARAM {
    /* if msg == EVENT_KEY_NUM_X */
    struct {u_int8 val;} digit;
    /* if msg == EVENT_KEY_COMBIN */
    struct {u_int8 keyNum; EVENT_NODE_HANDLE keySeq[2];} combin;
};
//----------------------------------------------------------------------

union EVENT_PARAM {
    union SM_EVENT_PARAM  sm;
    union KEY_EVENT_PARAM key;
};

struct EVENT_NODE_ITEM {
    EVENT_NODE_HANDLE  sig;
    union EVENT_PARAM  param;
};

enum {
    EVENT_INVALID = 0,

    // system
    EVENT_SYS_INIT,
    EVENT_SYS_POWERON,
    EVENT_SYS_POWEROFF,
    EVENT_SYS_TIMER0,
    EVENT_SYS_EXIT,
    
    // sm
    EVENT_SM_INIT,// one sm only init once
    EVENT_SM_ENTRY,//allow muti times entry
    EVENT_SM_TRANS,
    EVENT_SM_RETURN,
    EVENT_SM_EXIT,

    // key
    EVENT_KEY_NUM_0,
    EVENT_KEY_NUM_1,
    EVENT_KEY_NUM_2,
    EVENT_KEY_NUM_3,
    EVENT_KEY_NUM_4,
    EVENT_KEY_NUM_5,
    EVENT_KEY_NUM_6,
    EVENT_KEY_NUM_7,
    EVENT_KEY_NUM_8,
    EVENT_KEY_NUM_9,
    EVENT_KEY_ADD,
    EVENT_KEY_SUB,
    EVENT_KEY_UP,
    EVENT_KEY_DOWN,
    EVENT_KEY_LEFT,
    EVENT_KEY_RIGHT,
    EVENT_KEY_OK,
    EVENT_KEY_ESC,
    EVENT_KEY_RUN,
    EVENT_KEY_STOP,
    EVENT_KEY_RESET,
    EVENT_KEY_COMBIN,
    
    EVENT_END,
};


//==========================================================

struct EVTMGR_INIT_PARAM {
    struct EVENT_NODE_ITEM *buffer;
    EVENT_NODE_COUNT        len;
    void (*r_lock)(void);
    void (*r_unlock)(void);
    void (*w_lock)(void);
    void (*w_unlock)(void);
};

enum  EVENT_OPT_MODE {
    EVENT_OPT_MODE_FIFO,
    EVENT_OPT_MODE_STACK,
};

handle_t EventMgr_Open(struct EVTMGR_INIT_PARAM *initParam);
void EventMgr_Close(handle_t handle);
EVENT_NODE_COUNT EventMgr_Read(handle_t handle, struct EVENT_NODE_ITEM *event, enum  EVENT_OPT_MODE mode);
EVENT_NODE_COUNT EventMgr_Write(handle_t handle, struct EVENT_NODE_ITEM *event, enum  EVENT_OPT_MODE mode);


#ifdef __cplusplus
}
#endif

#endif
