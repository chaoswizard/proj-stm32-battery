#ifndef __UIMMI_CONTENT_H__
#define __UIMMI_CONTENT_H__
#include "ui_com_obj.h"

#ifdef __cplusplus
extern "C" {
#endif


//通道
UICOM_TEXT_DECLAER(TongDao);
#define   UICOM_STR_TONGDAO                      UICOM_TEXT_STR_REF(TongDao)


//序号
UICOM_TEXT_DECLAER(XuHao);
#define   UICOM_STR_XUHAO                        UICOM_TEXT_STR_REF(XuHao)


//当前值
UICOM_TEXT_DECLAER(ZhiDangQian);
#define   UICOM_STR_DANGQIANZHI                   UICOM_TEXT_STR_REF(ZhiDangQian)


//工作状态
UICOM_TEXT_DECLAER(GongZuoZhuangTai);
#define   UICOM_STR_GONGZUOZHUANGTAI              UICOM_TEXT_STR_REF(GongZuoZhuangTai)


//通道切换
UICOM_TEXT_DECLAER(TongDaoQieHuan);
#define   UICOM_STR_TONGDAOQIEHUAN                UICOM_TEXT_STR_REF(TongDaoQieHuan)


//放电中
UICOM_TEXT_DECLAER(FangDianZhong);
#define   UICOM_STR_FANGDIANZHONG                 UICOM_TEXT_STR_REF(FangDianZhong)


//放电结束
UICOM_TEXT_DECLAER(FangDianJieShu);
#define   UICOM_STR_FANGDIANJIESHU                UICOM_TEXT_STR_REF(FangDianJieShu)

//启用
UICOM_TEXT_DECLAER(QiYong);
#define   UICOM_STR_QIYONG                         UICOM_TEXT_STR_REF(QiYong)

//停用
UICOM_TEXT_DECLAER(TingYong);
#define   UICOM_STR_TINGYONG                       UICOM_TEXT_STR_REF(TingYong)


//确认
UICOM_TEXT_DECLAER(QueRen);
#define   UICOM_STR_QUEREN                       UICOM_TEXT_STR_REF(QueRen)


//取消
UICOM_TEXT_DECLAER(QuXiao);
#define   UICOM_STR_QUXIAO                       UICOM_TEXT_STR_REF(QuXiao)

UICOM_TEXT_DECLAER(QueDing);
#define   UICOM_STR_YES                       UICOM_TEXT_STR_REF(QueDing)

UICOM_TEXT_DECLAER(QueFou);
#define   UICOM_STR_NO                       UICOM_TEXT_STR_REF(QueFou)



//条
UICOM_TEXT_DECLAER(DanWeiTiao);
#define   UICOM_STR_DANWEITIAO                     UICOM_TEXT_STR_REF(DanWeiTiao)

//个
UICOM_TEXT_DECLAER(DanWeiGe);
#define   UICOM_STR_DANWEIGE                       UICOM_TEXT_STR_REF(DanWeiGe)


//警告查询
UICOM_TEXT_DECLAER(ChaXunJingGao);
#define   UICOM_STR_CHAXUNJINGGAO                    UICOM_TEXT_STR_REF(ChaXunJingGao)

//曲线查询
UICOM_TEXT_DECLAER(ChaXunQuXian);
#define   UICOM_STR_CHAXUNQUXIAN                    UICOM_TEXT_STR_REF(ChaXunQuXian)

//设置时间
UICOM_TEXT_DECLAER(ShiJianSheZhi);
#define   UICOM_STR_SHEZHISHIJIAN                    UICOM_TEXT_STR_REF(ShiJianSheZhi)

//已用时
UICOM_TEXT_DECLAER(ShiJianYiYong);
#define   UICOM_STR_YIYONGSHI                        UICOM_TEXT_STR_REF(ShiJianYiYong)

//当前系统时间
UICOM_TEXT_DECLAER(ShiJianXiTong);
#define   UICOM_STR_XITONGSHIJIAN                   UICOM_TEXT_STR_REF(ShiJianXiTong)


//按时间
UICOM_TEXT_DECLAER(AnShiJian);
#define   UICOM_STR_ANSHIJIAN                     UICOM_TEXT_STR_REF(AnShiJian)


//按电压
UICOM_TEXT_DECLAER(AnDianYa);
#define   UICOM_STR_ANDIANYA                     UICOM_TEXT_STR_REF(AnDianYa)

//参数设置
UICOM_TEXT_DECLAER(CanShuShiZhi);
#define   UICOM_STR_CANSHUSHIZHI                    UICOM_TEXT_STR_REF(CanShuShiZhi)


//混合模式
UICOM_TEXT_DECLAER(HunHeMoShi);
#define   UICOM_STR_HUNHEMOSHI                     UICOM_TEXT_STR_REF(HunHeMoShi)



//mV
UICOM_TEXT_DECLAER(DanWeiDianYa);
#define   UICOM_STR_DANWEIHAOFU                     UICOM_TEXT_STR_REF(DanWeiDianYa)

//V
UICOM_TEXT_DECLAER(DanWeiDianYaFu);
#define   UICOM_STR_DANWEIFU                     UICOM_TEXT_STR_REF(DanWeiDianYaFu)

//min
UICOM_TEXT_DECLAER(DanWeiFenZhong);
#define   UICOM_STR_DANWEIFENZHONG                  UICOM_TEXT_STR_REF(DanWeiFenZhong)


//工作模式
UICOM_TEXT_DECLAER(GongZuoMoShi);
#define   UICOM_STR_GONGZUIMOSHI                    UICOM_TEXT_STR_REF(GongZuoMoShi)

//跳转到
UICOM_TEXT_DECLAER(TiaoZhuanDao);
#define   UICOM_STR_TIAOZHUANDAO                    UICOM_TEXT_STR_REF(TiaoZhuanDao)

//全选
UICOM_TEXT_DECLAER(QuanXuan);
#define   UICOM_STR_QUANXUAN              UICOM_TEXT_STR_REF(QuanXuan)


//全不选
UICOM_TEXT_DECLAER(QuanBuXuan);
#define   UICOM_STR_QUANBUXUAN              UICOM_TEXT_STR_REF(QuanBuXuan)



//圆圈
UICOM_TEXT_DECLAER(YuanQuanQuan);
#define   UICOM_STR_YUANQUAN                          UICOM_TEXT_STR_REF(YuanQuanQuan)

UICOM_TEXT_DECLAER(YuanQuanQuan1);
#define   UICOM_STR_YUANQUAN1                          UICOM_TEXT_STR_REF(YuanQuanQuan1)



//组
UICOM_TEXT_DECLAER(DanWeiZu);
#define   UICOM_STR_DANWEIZU                      UICOM_TEXT_STR_REF(DanWeiZu)

//反向电压
UICOM_TEXT_DECLAER(FanXiangDianYa);
#define   UICOM_STR_FANXIANGDIANYA                      UICOM_TEXT_STR_REF(FanXiangDianYa)

//极限值下限
UICOM_TEXT_DECLAER(JiXianZhiXianXian);
#define   UICOM_STR_JIXIANZHIXIAXIAN                      UICOM_TEXT_STR_REF(JiXianZhiXianXian)

//警告内容
UICOM_TEXT_DECLAER(JingGaoNeiRong);
#define   UICOM_STR_JINGGAONEIRONG                      UICOM_TEXT_STR_REF(JingGaoNeiRong)



//悬空
UICOM_TEXT_DECLAER(JingGaoXuanKong);
#define   UICOM_STR_XUANKONG                      UICOM_TEXT_STR_REF(JingGaoXuanKong)


//反向
UICOM_TEXT_DECLAER(JingGaoFanXiang);
#define   UICOM_STR_FANXIANG                      UICOM_TEXT_STR_REF(JingGaoFanXiang)


//=========================================================
//测试图片
UICOM_PIC_DECLAER(smile);
//#define   UICOM_BMP_WEIXIAO                      UICOM_PIC_DATA_REF(smile)


//=========================================================
#ifdef __cplusplus
}
#endif

#endif
