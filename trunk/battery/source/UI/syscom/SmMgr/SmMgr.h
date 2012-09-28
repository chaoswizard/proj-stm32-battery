#ifndef __SM_MGR_H__
#define __SM_MGR_H__
#include "bits_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SM_INVALID_NODE   (0)

#define DEFINE_SM_NODE_MAP(name, enter, proc, exit)  \
    struct SM_NODE_PROC_TAB name = {\
    &enter,\
    &proc,\
    &exit,\
}

#define DECLARE_SM_NODE_MAP(name)  \
    extern struct SM_NODE_PROC_TAB name
    

#define SMNODE_IS_VALID(v)        ((v) > SM_INVALID_NODE)
#define SMNODE_IS_INVALID(v)      ((v) == SM_INVALID_NODE)

//MUST exit current node 
#define SM_EXIT_CUR      (0x01)
//if new node exist ,MUST exit old node 
#define SM_EXIT_NEW      (0x02)
//New Node MUST exit  when Trans Next 
#define SM_HOLD_NEW      (0x04)

typedef u_int8  SM_NODE_HANDLE;
typedef u_int8  SM_NODE_COUNT;

#define SM_PROC_RET_DFT     SM_PROC_RET_STAY
#define SM_PROC_RET_FINISH  SM_PROC_RET_BYPASS

enum SM_PROC_RET {
    SM_PROC_RET_STAY = 0,
    SM_PROC_RET_BYPASS,
};

// node func map
struct SM_NODE_PROC_TAB {
    void (*enter)(SM_NODE_HANDLE parent, SM_NODE_HANDLE me);
    enum SM_PROC_RET (*handle)(SM_NODE_HANDLE me, void *param);
    void (*exit)(SM_NODE_HANDLE me, SM_NODE_HANDLE next);
};

// history node func map
struct SM_NODE_ITEM {
    SM_NODE_HANDLE node;
    void (*suspend)(SM_NODE_HANDLE me, SM_NODE_HANDLE child);
    void (*resume)(SM_NODE_HANDLE child, SM_NODE_HANDLE me);
};


struct SM_NODE_MAP {
    SM_NODE_HANDLE node;
    struct SM_NODE_PROC_TAB *proc;
};


struct SMMGR_INIT_PARAM {
    struct SM_NODE_MAP  *nodeMap;
    SM_NODE_COUNT nodeCount;
    struct SM_NODE_ITEM *nodeStack;
    SM_NODE_COUNT nodeStackSize;
};


handle_t SmMgr_Open(struct SMMGR_INIT_PARAM *initParam);
void SmMgr_Close(handle_t handle);
SM_NODE_HANDLE SmMgr_Proc(handle_t handle, void *param);
SM_NODE_HANDLE SmMgr_Return(handle_t handle, SM_NODE_COUNT retLvl);
u_int8 SmMgr_Trans(handle_t handle, SM_NODE_HANDLE newNode, u_int8 quitIfSameCur);
SM_NODE_HANDLE SmMgr_GetCurrent(handle_t handle);
SM_NODE_HANDLE SmMgr_GetParent(handle_t handle);
void SmMgr_RegSuspend(handle_t handle, void (*suspend)(SM_NODE_HANDLE me, SM_NODE_HANDLE child));
void SmMgr_RegResume(handle_t handle, void (*resume)(SM_NODE_HANDLE child, SM_NODE_HANDLE me));
u_int8 SmMgr_IsInHistory(handle_t handle, SM_NODE_HANDLE name);

#ifdef __cplusplus
}
#endif


#endif
