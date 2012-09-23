#ifndef __UIMMI_CONTENT_H__
#define __UIMMI_CONTENT_H__
#include "ui_com_obj.h"

#ifdef __cplusplus
extern "C" {
#endif


//主界面
UICOM_TEXT_DECLAER(ZhuJieMian);
#define   UICOM_STR_ZHUJIMIAN                    UICOM_TEXT_STR_REF(ZhuJieMian)

//通道
UICOM_TEXT_DECLAER(TongDao);
#define   UICOM_STR_TONGDAO                      UICOM_TEXT_STR_REF(TongDao)

//佛山市惠龙自动化设备有限公司
UICOM_TEXT_DECLAER(GongSiMing0);
#define   UICOM_STR_GONGSIMING0                  UICOM_TEXT_STR_REF(GongSiMing0)


UICOM_TEXT_DECLAER(GongSiMing1);
#define   UICOM_STR_GONGSIMING1                  UICOM_TEXT_STR_REF(GongSiMing1)




//电池自动化检测台
UICOM_TEXT_DECLAER(GongSiChanPinMing);
#define   UICOM_STR_CHANPINMING                  UICOM_TEXT_STR_REF(GongSiChanPinMing)


//电话:0757-23330686
UICOM_TEXT_DECLAER(GongSiDianHua);
#define   UICOM_STR_GONGSIDIANHUA                UICOM_TEXT_STR_REF(GongSiDianHua)

//保存成功!
UICOM_TEXT_DECLAER(BaoCunChengGong);
#define   UICOM_STR_BAOCUNCHENGGONG              UICOM_TEXT_STR_REF(BaoCunChengGong)


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


//以检测产品数量
UICOM_TEXT_DECLAER(YiJianCeShuLiang);
#define   UICOM_STR_YIJIANCESHULIANF                UICOM_TEXT_STR_REF(YiJianCeShuLiang)

//合格率
UICOM_TEXT_DECLAER(HeGeLv);
#define   UICOM_STR_HEGELV                          UICOM_TEXT_STR_REF(HeGeLv)

//选项设置
UICOM_TEXT_DECLAER(XuanXiangSheZhi);
#define   UICOM_STR_XUANXIANGSHEZHI                 UICOM_TEXT_STR_REF(XuanXiangSheZhi)

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


    



//合格数
UICOM_TEXT_DECLAER(HeGeShu);
#define   UICOM_STR_HEGESHU                       UICOM_TEXT_STR_REF(HeGeShu)


//计数值清零
UICOM_TEXT_DECLAER(JiShuZhiQingLing);
#define   UICOM_STR_JISHUZHIQINGLING              UICOM_TEXT_STR_REF(JiShuZhiQingLing)


//串口输出结果
UICOM_TEXT_DECLAER(ChuanKouShuChu);
#define   UICOM_STR_CHUANKOUSHUCHU                UICOM_TEXT_STR_REF(ChuanKouShuChu)


//条
UICOM_TEXT_DECLAER(DanWeiTiao);
#define   UICOM_STR_DANWEITIAO                     UICOM_TEXT_STR_REF(DanWeiTiao)

//个
UICOM_TEXT_DECLAER(DanWeiGe);
#define   UICOM_STR_DANWEIGE                       UICOM_TEXT_STR_REF(DanWeiGe)

//模式选项
UICOM_TEXT_DECLAER(MoShiXuanXiang);
#define   UICOM_STR_MOSHIXUANXAIN                  UICOM_TEXT_STR_REF(MoShiXuanXiang)


//存储选项
UICOM_TEXT_DECLAER(CunChuXUanXiang);
#define   UICOM_STR_CUNCHUXUANXIANG                 UICOM_TEXT_STR_REF(CunChuXUanXiang)


//还原出厂设置
UICOM_TEXT_DECLAER(HuanYuanChuChang);
#define   UICOM_STR_RESETFACTORY                     UICOM_TEXT_STR_REF(HuanYuanChuChang)


//查询选项
UICOM_TEXT_DECLAER(ChaXunXuanXiang);
#define   UICOM_STR_CHAXUNOPTION                     UICOM_TEXT_STR_REF(ChaXunXuanXiang)


//其他选项
UICOM_TEXT_DECLAER(QiTaXuanXiang);
#define   UICOM_STR_QITAXUANXIANG                     UICOM_TEXT_STR_REF(QiTaXuanXiang)




//按时间
UICOM_TEXT_DECLAER(AnShiJian);
#define   UICOM_STR_ANSHIJIAN                     UICOM_TEXT_STR_REF(AnShiJian)


//按电压
UICOM_TEXT_DECLAER(AnDianYa);
#define   UICOM_STR_ANDIANYA                     UICOM_TEXT_STR_REF(AnDianYa)
    
//混合模式
UICOM_TEXT_DECLAER(HunHeMoShi);
#define   UICOM_STR_HUNHEMOSHI                     UICOM_TEXT_STR_REF(HunHeMoShi)


//mV
UICOM_TEXT_DECLAER(DanWeiDianYa);
#define   UICOM_STR_DANWEIHAOFU                     UICOM_TEXT_STR_REF(DanWeiDianYa)

//min
UICOM_TEXT_DECLAER(DanWeiFenZhong);
#define   UICOM_STR_DANWEIFENZHONG                    UICOM_TEXT_STR_REF(DanWeiFenZhong)


//圆圈
UICOM_TEXT_DECLAER(YuanQuanQuan);
#define   UICOM_STR_YUANQUAN                          UICOM_TEXT_STR_REF(YuanQuanQuan)

UICOM_TEXT_DECLAER(YuanQuanQuan1);
#define   UICOM_STR_YUANQUAN1                          UICOM_TEXT_STR_REF(YuanQuanQuan1)





//最大存储量为
UICOM_TEXT_DECLAER(ZuiDaCunChuLiang);
#define   UICOM_STR_ZUIDACUNCHULIANG                  UICOM_TEXT_STR_REF(ZuiDaCunChuLiang)



//存储介质
UICOM_TEXT_DECLAER(CunChuJieZhi);
#define   UICOM_STR_CUNCHUJIEZHI                 UICOM_TEXT_STR_REF(CunChuJieZhi)


//覆盖否
UICOM_TEXT_DECLAER(FuGaiMa);
#define   UICOM_STR_FUGAIMA                      UICOM_TEXT_STR_REF(FuGaiMa)

// SPI
UICOM_TEXT_DECLAER(MediaSPI);
#define   UICOM_STR_MEDIASPI                      UICOM_TEXT_STR_REF(MediaSPI)


//组
UICOM_TEXT_DECLAER(DanWeiZu);
#define   UICOM_STR_DANWEIZU                      UICOM_TEXT_STR_REF(DanWeiZu)


//显示第
UICOM_TEXT_DECLAER(XianShiDi);
#define   UICOM_STR_XIANSHIDI                     UICOM_TEXT_STR_REF(XianShiDi)

//条起放电曲线
UICOM_TEXT_DECLAER(TiaoQiFangDian);
#define   UICOM_STR_FANGDAINQUXIAN                UICOM_TEXT_STR_REF(TiaoQiFangDian)


//一次显示曲线数
UICOM_TEXT_DECLAER(YiCiXianQuXian);
#define   UICOM_STR_YICIXIANSHISHU                UICOM_TEXT_STR_REF(YiCiXianQuXian)

//请选择可用通道，按下+，为选中，按-，为去选中
UICOM_TEXT_DECLAER(TongDaoXuanZeTip);
#define   UICOM_STR_TONGDAOXUANZETISHI                UICOM_TEXT_STR_REF(TongDaoXuanZeTip)

UICOM_TEXT_DECLAER(TongDaoXuanZeTip1);
#define   UICOM_STR_TONGDAOXUANZETISHI1                UICOM_TEXT_STR_REF(TongDaoXuanZeTip1)



//=========================================================
//测试图片
UICOM_PIC_DECLAER(smile);
#define   UICOM_BMP_WEIXIAO                      UICOM_PIC_DATA_REF(smile)


//=========================================================
#ifdef __cplusplus
}
#endif

#endif
