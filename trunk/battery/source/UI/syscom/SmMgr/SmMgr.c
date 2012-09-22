#include "common_define.h"
#include "SmMgr.h"

#define SMMGR_DEBUG        MY_DEBUG
#define SMMGR_TRACE(s,n)   MY_DEBUG(s":%d", n)

struct SM_MGR_CTRL {
    struct SMMGR_INIT_PARAM cfg;
    SM_NODE_COUNT currentIdx;
};

static struct SM_MGR_CTRL gSmMgrInstance;

static struct SM_NODE_PROC_TAB *SmMgr_GetFuncTab(struct SM_MGR_CTRL *pCtrl, SM_NODE_HANDLE Node);
static SM_NODE_HANDLE SmMgr_Pop(struct SM_MGR_CTRL *pCtrl, u_int8 isResume);
static SM_NODE_COUNT SmMgr_Push(struct SM_MGR_CTRL *pCtrl, SM_NODE_HANDLE newNode, u_int8 suspendCur);

u_int8 SmMgr_IsInHistory(handle_t handle, SM_NODE_HANDLE name)
{
   struct SM_MGR_CTRL *pCtrl = (struct SM_MGR_CTRL *)handle;
   s_int32 i;
   
   for (i=0; i<= pCtrl->currentIdx; i++)
   {
        if (name == pCtrl->cfg.nodeStack[i].node)
        {
            return 1;
        }
   }

   return 0;
}
SM_NODE_HANDLE SmMgr_GetCurrent(handle_t handle)
{
    struct SM_MGR_CTRL *pCtrl = (struct SM_MGR_CTRL *)handle;
    
    if (pCtrl->currentIdx < pCtrl->cfg.nodeStackSize)
    {
        return (pCtrl->cfg.nodeStack[pCtrl->currentIdx].node);
    }
    else
    {
        return SM_INVALID_NODE;
    }
}

SM_NODE_HANDLE SmMgr_GetParent(handle_t handle)
{
    struct SM_MGR_CTRL *pCtrl = (struct SM_MGR_CTRL *)handle;

    if ((pCtrl->currentIdx > 0) && (pCtrl->currentIdx < pCtrl->cfg.nodeStackSize))
    {
        return (pCtrl->cfg.nodeStack[pCtrl->currentIdx - 1].node);
    }
    else
    {
        return SM_INVALID_NODE;
    }
}

static struct SM_NODE_PROC_TAB *SmMgr_GetFuncTab(struct SM_MGR_CTRL *pCtrl, SM_NODE_HANDLE Node)
{
    s_int32 i;
    
    for (i=0; i<pCtrl->cfg.nodeCount; i++)
    {
        if (Node == pCtrl->cfg.nodeMap[i].node)
        {
            return pCtrl->cfg.nodeMap[i].proc;
        }
    }

    return NULL;
}


static SM_NODE_COUNT SmMgr_Push(struct SM_MGR_CTRL *pCtrl, SM_NODE_HANDLE newNode, u_int8 suspendCur)
{
    struct SM_NODE_PROC_TAB *tab;
    struct SM_NODE_ITEM  *current;

    if (pCtrl->currentIdx >= (pCtrl->cfg.nodeStackSize - 1))
    {
        SMMGR_TRACE("SmFull", pCtrl->currentIdx);
        return pCtrl->currentIdx;
    }
    
    // first push to bottom
    if ((0 == pCtrl->currentIdx) && (SMNODE_IS_INVALID(pCtrl->cfg.nodeStack[0].node)))
    {
        current = NULL;
    }
    else 
    {
        current = &(pCtrl->cfg.nodeStack[pCtrl->currentIdx]);
        //here can occus a rePush node case,  need process later ????
        // maybe need  skip the case current == newNode ?????
        pCtrl->currentIdx++;
    }

    // suspend current node
    if (suspendCur && current && current->suspend)
    {
        (current->suspend)(current->node, newNode);
    }
    
    tab = SmMgr_GetFuncTab(pCtrl, newNode);
    //enter new node
    if (tab && tab->enter) 
    {
        (tab->enter)(current ? current->node : SM_INVALID_NODE, newNode);
    }

    //add new node to stack top
    pCtrl->cfg.nodeStack[pCtrl->currentIdx].node    = newNode;
    
    return pCtrl->currentIdx;
}

static SM_NODE_HANDLE SmMgr_Pop(struct SM_MGR_CTRL *pCtrl, u_int8 isResume)
{
    struct SM_NODE_PROC_TAB *tab;
    struct SM_NODE_ITEM  *me, *next;

    // if pop empty
    if ((pCtrl->currentIdx == 0) && (SMNODE_IS_INVALID(pCtrl->cfg.nodeStack[0].node)))
    {
        SMMGR_TRACE("SmEmpty", pCtrl->currentIdx);
        return pCtrl->currentIdx;
    }
    
    me = &(pCtrl->cfg.nodeStack[pCtrl->currentIdx]);
    //get current node map
    tab = SmMgr_GetFuncTab(pCtrl, me->node);
    if (pCtrl->currentIdx > 0)
    {
        // pop current, return old
        pCtrl->currentIdx--;
        next = &(pCtrl->cfg.nodeStack[pCtrl->currentIdx]);
    }
    else
    {
        next = NULL;
    }
    
    //leave current node
    if (tab && tab->exit) 
    {
        (tab->exit)(me->node, next ? next->node : SM_INVALID_NODE);
    }

    // if need resume old node
    if (isResume && next && (next->resume))
    {
        //resume old node
        (next->resume)(me->node, next->node);
    }

    //clear  exited  node info
    me->node    = SM_INVALID_NODE;
    me->resume  = NULL;
    me->suspend = NULL;
    
    return pCtrl->currentIdx;
}

handle_t  SmMgr_Open(struct SMMGR_INIT_PARAM *initParam)
{
    struct SM_MGR_CTRL *pSmMgr;

    pSmMgr = &gSmMgrInstance;
    memset(pSmMgr, 0, sizeof(struct SM_MGR_CTRL));

    memcpy(&pSmMgr->cfg, initParam, sizeof(struct SMMGR_INIT_PARAM));


    return (handle_t)pSmMgr;
}

void  SmMgr_Close(handle_t handle)
{
    struct SM_MGR_CTRL *pCtrl = (struct SM_MGR_CTRL *)handle;

    while (SMNODE_IS_VALID(SmMgr_GetCurrent(handle)))
    {
        SmMgr_Pop(pCtrl, 0);
    }

    memset(pCtrl, 0, sizeof(struct SM_MGR_CTRL));
}

SM_NODE_HANDLE SmMgr_Proc(handle_t handle, void *param)
{
    struct SM_NODE_PROC_TAB *tab;
    struct SM_MGR_CTRL *pCtrl = (struct SM_MGR_CTRL *)handle;
    SM_NODE_HANDLE curNode = SmMgr_GetCurrent(handle);
    
    if (SMNODE_IS_INVALID(curNode))
    {
        return SM_INVALID_NODE;
    }
    
    tab = SmMgr_GetFuncTab(pCtrl, curNode);

    if (tab && tab->handle)
    {
        (tab->handle)(curNode, param);
    }

    return curNode;
}

void SmMgr_RegSuspend(handle_t handle, void (*suspend)(SM_NODE_HANDLE me, SM_NODE_HANDLE child))
{
    struct SM_MGR_CTRL *pCtrl = (struct SM_MGR_CTRL *)handle;

    if (pCtrl->currentIdx < pCtrl->cfg.nodeStackSize)
    {
        pCtrl->cfg.nodeStack[pCtrl->currentIdx].suspend = suspend;
    }
}
void SmMgr_RegResume(handle_t handle, void (*resume)(SM_NODE_HANDLE child, SM_NODE_HANDLE me))
{
    struct SM_MGR_CTRL *pCtrl = (struct SM_MGR_CTRL *)handle;

    if (pCtrl->currentIdx < pCtrl->cfg.nodeStackSize)
    {
        pCtrl->cfg.nodeStack[pCtrl->currentIdx].resume  = resume;
    }
}


SM_NODE_HANDLE SmMgr_Return(handle_t handle, SM_NODE_COUNT retLvl)
{
    struct SM_MGR_CTRL *pCtrl = (struct SM_MGR_CTRL *)handle;

    while(retLvl && SMNODE_IS_VALID(SmMgr_GetCurrent(handle)))
    {
        SmMgr_Pop(pCtrl, (1 == retLvl));
        retLvl--;
    }

    return SmMgr_GetCurrent(handle);
}

SM_NODE_HANDLE SmMgr_Trans(handle_t handle, SM_NODE_HANDLE newNode, u_int8 quitIfSameCur)
{
    struct SM_MGR_CTRL *pCtrl = (struct SM_MGR_CTRL *)handle;
    SM_NODE_HANDLE curNode;
    u_int8 isSuspendCur;

    //if new node  has pushed , need pop child node
    if (SmMgr_IsInHistory(handle, newNode))
    {
        while (1)
        {
            curNode = SmMgr_GetCurrent(handle);
            //if current node is not same  with target nodes
            if (SMNODE_IS_VALID(curNode) && (curNode != newNode))
            {
                SmMgr_Pop(pCtrl, 0);
            }
            else
            {
                break;
            }
         }
    }
    
    if (SmMgr_GetCurrent(handle) == newNode)
    {
        if (quitIfSameCur)
        {
            SmMgr_Pop(pCtrl, 0);//close and reopen
            isSuspendCur = 0;
        }
        else
        {
            return SmMgr_GetCurrent(handle);// is current is same, just entry again
        }
    }
    else
    {
        isSuspendCur = 1;
    }
    
    SmMgr_Push(pCtrl, newNode, isSuspendCur);

    return SmMgr_GetCurrent(handle);
}

