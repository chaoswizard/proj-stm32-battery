#include "common_define.h"
#include "EventMgr.h"

#define EVTMGR_DEBUG        MY_DEBUG
#define EVTMGR_TRACE(s,n)   MY_DEBUG(s":%d", n)


struct EVENT_MGR_CTRL {
    struct EVTMGR_INIT_PARAM cfg;
    EVENT_NODE_COUNT  head;
    EVENT_NODE_COUNT  tail;
    EVENT_NODE_COUNT  freeCnt;
    EVENT_NODE_COUNT  readyCnt;
};

static struct EVENT_MGR_CTRL gEvtMgrInstance;


HANDLE EventMgr_Open(struct EVTMGR_INIT_PARAM *initParam)
{
    struct EVENT_MGR_CTRL *pEvtMgr;

    pEvtMgr = &gEvtMgrInstance;
    memset(pEvtMgr, 0, sizeof(struct EVENT_MGR_CTRL));

    memcpy(&pEvtMgr->cfg, initParam, sizeof(struct EVTMGR_INIT_PARAM));

    pEvtMgr->head = pEvtMgr->tail = 0;
    pEvtMgr->freeCnt = pEvtMgr->cfg.len;
    pEvtMgr->readyCnt = 0;

    return (HANDLE)pEvtMgr;
}

void EventMgr_Close(HANDLE handle)
{
    struct EVENT_MGR_CTRL *pCtrl = (struct EVENT_MGR_CTRL *)handle;


    memset(pCtrl, 0, sizeof(struct EVENT_MGR_CTRL));
}

EVENT_NODE_COUNT EventMgr_Write(HANDLE handle, struct EVENT_NODE_ITEM *event, enum  EVENT_OPT_MODE mode)
{
    struct EVENT_MGR_CTRL *pCtrl = (struct EVENT_MGR_CTRL *)handle;
    EVENT_NODE_COUNT cur;

    if (0 == pCtrl->freeCnt)
    {
        return 0;
    }

    cur = pCtrl->tail;
    pCtrl->tail++;
    if (pCtrl->tail >= pCtrl->cfg.len)
    {
        pCtrl->tail = 0;
    }
    
    if (event)
    {
        memcpy(&pCtrl->cfg.buffer[cur], event, sizeof(struct EVENT_NODE_ITEM));
    }

    cur = pCtrl->freeCnt;
    
    if (pCtrl->cfg.w_lock) (pCtrl->cfg.w_lock)();
    pCtrl->readyCnt++;
    pCtrl->freeCnt--;
    if (pCtrl->cfg.w_unlock) (pCtrl->cfg.w_unlock)();

    return cur;
}

EVENT_NODE_COUNT EventMgr_Read(HANDLE handle, struct EVENT_NODE_ITEM *event, enum  EVENT_OPT_MODE mode)
{
    struct EVENT_MGR_CTRL *pCtrl = (struct EVENT_MGR_CTRL *)handle;
    EVENT_NODE_COUNT cur;
    
    if (0 == pCtrl->readyCnt)
    {
        return 0;
    }

    cur = pCtrl->head;
    pCtrl->head++;
    if (pCtrl->head >= pCtrl->cfg.len)
    {
        pCtrl->head = 0;
    }
    
    if (event)
    {
        memcpy(event, &pCtrl->cfg.buffer[cur], sizeof(struct EVENT_NODE_ITEM));
    }

    cur = pCtrl->readyCnt;
    
    if (pCtrl->cfg.r_lock) (pCtrl->cfg.r_lock)();
    pCtrl->readyCnt--;
    pCtrl->freeCnt++;
    if (pCtrl->cfg.r_unlock) (pCtrl->cfg.r_unlock)();
    
    return cur;
}


