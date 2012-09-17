/***********************************************************************
 *
 * MODULE NAME:  
 * DESCRIPTION:   software timer
 * AUTHOR:     wangxi
 * DATE: 2012/09/09
 *
 ***********************************************************************/
#include "SwTmrMgr.h"
#include "common_define.h"

struct SWTMR_MGR_CTRL {
    struct SWTMR_INIT_PARAM cfg;
    struct SWTMR_NODE_ITEM *current;
    u_int32 currentTickcount;
};


static struct SWTMR_MGR_CTRL gSwTmrMgrInstance;


static void SwTmrMgr_Add(struct SWTMR_MGR_CTRL *pCtrl, struct SWTMR_NODE_ITEM *new_tmr_entry);
static struct SWTMR_NODE_ITEM *SwTmrMgr_Remove(struct SWTMR_MGR_CTRL *pCtrl, u_int8 timer_index);
static struct SWTMR_NODE_ITEM *SwTmrMgr_GetFree(struct SWTMR_MGR_CTRL *pCtrl);
static struct SWTMR_NODE_ITEM *SwTmrMgr_GetHead(struct SWTMR_MGR_CTRL *pCtrl, struct SWTMR_NODE_ITEM *reftimer);

HANDLE SwTmrMgr_Open(struct SWTMR_INIT_PARAM *initParam)
{
    struct SWTMR_MGR_CTRL *pCtrl;

    pCtrl = &gSwTmrMgrInstance;
    

    memset(pCtrl, 0, sizeof(struct SWTMR_MGR_CTRL));
    memcpy(&pCtrl->cfg, initParam, sizeof(struct SWTMR_INIT_PARAM));

    ////////////////////////////////////////////////////////////////////
    pCtrl->currentTickcount  = 0;
    pCtrl->current   = NULL;

    return (HANDLE)pCtrl;
}

void SwTmrMgr_Close(HANDLE handle)
{
    struct SWTMR_MGR_CTRL *pCtrl = (struct SWTMR_MGR_CTRL *)handle;

    if (0 == handle)
    {
        return;
    }
    
    pCtrl->currentTickcount  = 0;
    pCtrl->current   = NULL;
}

u_int32 SwTmrMgr_GetCurTickCount(HANDLE handle)
{
    struct SWTMR_MGR_CTRL *pCtrl = (struct SWTMR_MGR_CTRL *)handle;
    
    return pCtrl->currentTickcount;
}

u_int32 SwTmrMgr_ExistSchedule(HANDLE handle)
{
    struct SWTMR_MGR_CTRL *pCtrl = (struct SWTMR_MGR_CTRL *)handle;
    
    if (pCtrl->current)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static struct SWTMR_NODE_ITEM *SwTmrMgr_GetFree(struct SWTMR_MGR_CTRL *pCtrl)
{
    struct SWTMR_NODE_ITEM *newEntry;
	u_int8 i;

    
	for(i=0; i< pCtrl->cfg.tmrCount; i++)
	{
        if(pCtrl->cfg.tmrList[i].flags == SWTMR_MODE_UNUSED)
        {
            newEntry = &(pCtrl->cfg.tmrList[i]);
            if(newEntry->flags == SWTMR_MODE_UNUSED)
            {      
                newEntry->tmr_id = (i+1);
                return newEntry;
            }
        }
	}
    
    return NULL;

}



static struct SWTMR_NODE_ITEM *SwTmrMgr_GetHead(struct SWTMR_MGR_CTRL *pCtrl, struct SWTMR_NODE_ITEM *reftimer)
{
    struct SWTMR_NODE_ITEM *head, *entry, *pre_entry;

    if (0 == reftimer)
    {
        return 0;
    }

    //find the latest node
    head   = reftimer;
    entry  = head;
    do
    {
        pre_entry  = entry;
        entry      = entry->next;
        if ((entry->timeToExpiry < pre_entry->timeToExpiry) &&\
            (entry->timeToExpiry < entry->next->timeToExpiry))
        {
            break;
        }
    } while(entry != head);
    
    return entry;

}


SWTMR_NODE_HANDLE SwTmrMgr_Start(HANDLE handle, u_int32 timeoutTenths, void(*callbackFunc)(void *), void *callbackArg, u_int8 isLoop)
{
    struct SWTMR_NODE_ITEM *newEntry;
    struct SWTMR_MGR_CTRL *pCtrl = (struct SWTMR_MGR_CTRL *)handle;
    u_int8 flags;

    if (isLoop) {
        flags = SWTMR_MODE_PERIODIC;
    }
    else {
        flags = SWTMR_MODE_ONESHOT;
    }
    
    newEntry = SwTmrMgr_GetFree(pCtrl);
    if (!newEntry) {
        return SWTMR_INVALID_NODE;
    }


    newEntry->flags = flags;
    newEntry->callbackFunc  = callbackFunc;
    newEntry->timeoutTenths = timeoutTenths;
    newEntry->timeToExpiry  = timeoutTenths + pCtrl->currentTickcount;
    newEntry->callbackParam = callbackArg;
    SwTmrMgr_Add(pCtrl, newEntry);
    
    return newEntry->tmr_id;
}

SWTMR_NODE_HANDLE SwTmrMgr_Change(HANDLE handle, SWTMR_NODE_COUNT tmrHdl, u_int32 timeoutTenths)
{
	struct SWTMR_NODE_ITEM *entry;
    struct SWTMR_MGR_CTRL *pCtrl = (struct SWTMR_MGR_CTRL *)handle;

    entry = SwTmrMgr_Remove(pCtrl, tmrHdl);
    if (entry)
    {
        entry->timeoutTenths = timeoutTenths;
        entry->timeToExpiry  = timeoutTenths + pCtrl->currentTickcount;
        SwTmrMgr_Add(pCtrl, entry);
        return entry->tmr_id;
    }

    return SWTMR_INVALID_NODE;
}

SWTMR_NODE_HANDLE SwTmrMgr_Stop(HANDLE handle, SWTMR_NODE_HANDLE tmrHdl)
{
	struct SWTMR_NODE_ITEM *entry;
    struct SWTMR_MGR_CTRL *pCtrl = (struct SWTMR_MGR_CTRL *)handle;


    entry = SwTmrMgr_Remove(pCtrl, tmrHdl);
    if (entry)
    {
        entry->flags = SWTMR_MODE_UNUSED;
        return entry->tmr_id;
    }
    return SWTMR_INVALID_NODE;
}

SWTMR_NODE_HANDLE SwTmrMgr_Proc(HANDLE handle)
{
   struct SWTMR_NODE_ITEM *current;
   struct SWTMR_MGR_CTRL *pCtrl = (struct SWTMR_MGR_CTRL *)handle;
   

   pCtrl->currentTickcount++;

   if (NULL == pCtrl->current)
   {
       return SWTMR_INVALID_NODE;
   }
    
   current = pCtrl->current;

/* Decrement the counter and check for expiry */
   if(pCtrl->currentTickcount >= current->timeToExpiry)
   {
        current = SwTmrMgr_Remove(pCtrl, current->tmr_id);

        if (current)
        {
            if (current->flags & SWTMR_MODE_PERIODIC) 
            {
                current->timeToExpiry = current->timeoutTenths + pCtrl->currentTickcount;
                SwTmrMgr_Add(pCtrl, current);
            } 
            else 
            {
                  current->flags = SWTMR_MODE_UNUSED; // disable the timer
            } 
            //=================================================
        }
        

        // call callback function
        if (current && current->callbackFunc)
        {
            current->callbackFunc(current->callbackParam);
            return current->tmr_id;
        }
    }

    return SWTMR_INVALID_NODE;
}

static struct SWTMR_NODE_ITEM * SwTmrMgr_Remove(struct SWTMR_MGR_CTRL *pCtrl, SWTMR_NODE_COUNT timer_index)
{
    struct SWTMR_NODE_ITEM *head, *entry, *pre_entry;

    if (NULL == pCtrl->current)
    {
        return NULL;
    }
    
    
    head = pCtrl->current;
    entry = head;
	do
    {
        pre_entry = entry;
        entry = entry->next;
        
        if(entry->tmr_id == timer_index)
        {
            pre_entry->next = entry->next;
            if (entry == entry->next)
            {
                pCtrl->current = 0;
            }
            else
            {
                pCtrl->current = SwTmrMgr_GetHead(pCtrl, pre_entry);//relocate the current to the latest node
            }
            entry->next = 0;
            return entry;
        }
    }while(entry != head);
    

    return NULL;
}

static void SwTmrMgr_Add(struct SWTMR_MGR_CTRL *pCtrl, struct SWTMR_NODE_ITEM *new_tmr_entry)
{    
    struct SWTMR_NODE_ITEM *head, *entry, *pre_entry;

    if (!new_tmr_entry)
    {
        return;
    }
    

    head = SwTmrMgr_GetHead(pCtrl, pCtrl->current);
    if (0 == head)
    {
        entry = new_tmr_entry;
        entry->next = entry;
        pCtrl->current = entry;
        
        return;
    }


    entry  = head;
    //insert the node  by the expired time
    do
    {
        pre_entry  = entry;
        entry      = entry->next;
        if ((entry->timeToExpiry >= new_tmr_entry->timeToExpiry) &&\
            (pre_entry->timeToExpiry < new_tmr_entry->timeToExpiry))
        {
            break;
        }
    } while(entry != head);

    pre_entry->next = new_tmr_entry;
    new_tmr_entry->next = entry;

    pCtrl->current = SwTmrMgr_GetHead(pCtrl, entry);//relocate the current to the latest node
}


void SwTmrMgr_Dump(HANDLE handle)
{    
    struct SWTMR_NODE_ITEM *head, *entry;
    struct SWTMR_MGR_CTRL *pCtrl = (struct SWTMR_MGR_CTRL *)handle;
    u_int32 i = 0;

    if (0 == pCtrl)
    {
        return;
    }
    
    if (NULL == pCtrl->current)
    {
        MY_DEBUG("\nNo any timer to dump\n");
        return;
    }


    head       = pCtrl->current;
    entry      = head;
    MY_DEBUG("\ndump[%X]\n", head);
    i = 0;
    do
    {
        i++;
        MY_DEBUG("\t%03d,Tmr[%d]: interval=%d, \texpire=%d, \t%x\n", 
            i,
            entry->tmr_id, 
            entry->timeoutTenths, 
            entry->timeToExpiry, 
            entry->flags);
        entry      = entry->next;
    }while(entry != head);
}



