#ifndef __EVENT_MGR_H__
#define __EVENT_MGR_H__
#include "SmMgr.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef u_int8 EVENT_NODE_HANDLE;
typedef u_int8 EVENT_NODE_COUNT;

#define MSG_IS_VALID(msg)      (((msg) > EVENT_INVALID) && ((msg) < EVENT_END))

#define MSG_IS_SM(msg)         (((msg) >= EVENT_SM_INIT) && ((msg) <= EVENT_SM_EXIT))
#define MSG_IS_SYS(msg)        (((msg) >= EVENT_SYS_INIT) && ((msg) <= EVENT_SYS_EXIT))
#define MSG_IS_USR(msg)        ((msg) == EVENT_USR_MMI)
#define MSG_IS_CHAR(msg)       ((msg) == EVENT_KEY_CHAR)
#define MSG_IS_CANCLE(msg)     ((msg) == EVENT_KEY_ESC)
#define MSG_IS_ENTER(msg)      ((msg) == EVENT_KEY_OK)
#define MSG_IS_STOP(msg)       ((msg) == EVENT_KEY_STOP)
#define MSG_IS_START(msg)      ((msg) == EVENT_KEY_RUN)
#define MSG_IS_ENTRY(msg)      ((msg) == EVENT_SM_ENTRY)
//----------------------------------------------------------------------
#define SM_EVENT_TRANS_INIT(p, t, m)     {(p).sm.cfg.trans.target = (t);(p).sm.cfg.trans.mode = (m);}
#define SM_EVENT_RETUEN_INIT(p, l)       ((p).sm.cfg.ret.retLvl = (l))

#define SM_EVENT_TRANS_TARGET(p)         ((p).sm.cfg.trans.target)
#define SM_EVENT_TRANS_MODE(p)           ((p).sm.cfg.trans.mode)
#define SM_EVENT_RETURN_LVL(p)           ((p).sm.cfg.ret.retLvl)



struct SM_EVENT_PARAM {
    union {
        /* if msg == EVENT_SM_INIT */
        //struct {SM_NODE_HANDLE me;} init;
        /* if msg == EVENT_SM_TRANS */
        struct {SM_NODE_HANDLE target; u_int8 mode;} trans;
        /* if msg == EVENT_SM_RETURN */
        struct {SM_NODE_COUNT retLvl;} ret;
        /* if msg == EVENT_SM_EXIT */
        //struct {SM_NODE_HANDLE me;} exit;
    } cfg;
};
//----------------------------------------------------------------------
#define KEYMODE_DIGIT_CHAR    0x1
#define KEYMODE_NORMAL_CHAR   0x2

#define KEY_EVENT_DIGIT_INIT(p, val) {\
    (p).key.mode =KEYMODE_DIGIT_CHAR;(p).key.cfg.ch = (val);}

#define KEY_EVENT_IS_DIGIT(p)   ((p).key.mode == KEYMODE_DIGIT_CHAR)
#define KEY_EVENT_CHAR(p)       ((p).key.cfg.ch)
#define KEY_EVENT_DIGIT(p)      (KEY_EVENT_CHAR(p) - '0')


struct KEY_EVENT_PARAM {
    u_int8 mode;
    union {
        /* if msg == EVENT_KEY_NUM_X */
        u_int16 ch;
        /* if msg == EVENT_KEY_COMBIN */
        struct {u_int8 keyNum; EVENT_NODE_HANDLE keySeq[2];} combin;
    }cfg;
};
//----------------------------------------------------------------------
#define USR_EVENT_MMI_INIT(p, val)        ((p).usr.cfg.mmi.param = (u_int16)(val))
#define USR_EVENT_MMI(p)                  ((p).usr.cfg.mmi.param)

struct USR_EVENT_PARAM {
    union {
        struct {u_int16 param;} mmi;
    }cfg;
};


//----------------------------------------------------------------------
struct SYS_EVENT_PARAM {
    union {
        struct {u_int16 param;} ad;
    }cfg;
};

#define SYS_EVENT_AD_INIT(p, val)        ((p).sys.cfg.ad.param = (u_int16)(val))
#define SYS_EVENT_AD(p)                         ((p).sys.cfg.ad.param)

//----------------------------------------------------------------------

union EVENT_PARAM {
    struct SM_EVENT_PARAM  sm;
    struct KEY_EVENT_PARAM key;
    struct USR_EVENT_PARAM usr;
    struct SYS_EVENT_PARAM sys;
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
    EVENT_SYS_HW_AD,
    EVENT_SYS_HW_SQ_START,
    EVENT_SYS_HW_SQ_END,
    EVENT_SYS_EXIT,
    
    // sm
    EVENT_SM_INIT,// one sm only init once
    EVENT_SM_ENTRY,//allow muti times entry
    EVENT_SM_TRANS,
    EVENT_SM_RETURN,
    EVENT_SM_EXIT,

    // key
    EVENT_KEY_CHAR,
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
    
    // USR
    EVENT_USR_MMI,
    
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
