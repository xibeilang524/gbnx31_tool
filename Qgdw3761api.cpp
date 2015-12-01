// Qgdw3761api.cpp: implementation of the CQgdw3761api class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NxMaster.h"
#include "Qgdw3761api.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


/******************************************************************************

            版权所有 (C), 2001-2015, 国网电科院通信与用电技术分公司

 ******************************************************************************
  文 件 名   : qgdw_3761_api.c
  版 本 号   : 初稿
  作    者   : liming
  生成日期   : 2013年7月29日
  最近修改   :
  功能描述   : Q/GDW 376.1 协议解析api 实现源文件
  函数列表   :
  修改历史   :
  1.日    期   : 2013年7月29日
    作    者   : liming
    修改内容   : 创建文件

******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>


// 调试开关
#define MT_DBG
 #undef  MT_DBG           // 正式版时去掉该行注释

#ifdef MT_DBG
#define DEBUG(format,...) printf("File: "__FILE__", Line: %d: "format"\n", __LINE__, ##__VA_ARGS__)  
#endif

#ifdef MT_DBG
/*****************************************************************************
 函 数 名  : smtGetErr
 功能描述  : 返回错误的类型名称
 输入参数  : 无
 输出参数  : 无
 返 回 值  : char
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年7月30日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
char * smtGetErr(eMtErr eErr)
{
    char *pStr = NULL;
    switch(eErr)
    {
        case MT_OK:
            pStr = "MT_OK";
            break;
            
        case MT_ERR_NULL:
            pStr = "MT_ERR_NULL";
            break;
            
        case MT_ERR_OUTRNG:
            pStr = "MT_ERR_OUTRNG";
            break;
            
        case MT_ERR_NONE:
            pStr = "MT_ERR_NONE";
            break;
            
        case MT_ERR_IO:
            pStr = "MT_ERR_IO";
            break;
            
        case MT_ERR_RES:
            pStr = "MT_ERR_RES";
            break;

        case MT_ERR_INIT:
            pStr = "MT_ERR_INIT";
            break;
            
        case MT_ERR_PARA:
            pStr = "MT_ERR_PARA";
            break;
            
        case MT_ERR_FN:
            pStr = "MT_ERR_FN";
            break;
            
        case MT_ERR_PN:
            pStr = "MT_ERR_PN";
            break;
            
        case MT_ERR_ADDR:
            pStr = "MT_ERR_ADDR";
            break;
            
        case MT_ERR_CTRL:
            pStr = "MT_ERR_CTRL";
            break;
            
        case MT_ERR_SEQ:
            pStr = "MT_ERR_SEQ";
            break;
            
        case MT_ERR_SEQ2CS:
            pStr = "MT_ERR_SEQ2CS";
            break;
            
         case MT_ERR_AFN:
            pStr = "MT_ERR_AFN";
            break;
            
        case MT_ERR_UNCOMP:
            pStr = "MT_ERR_UNCOMP";
            break;
            
        case MT_ERR_0x68:
            pStr = "MT_ERR_0x68";
            break;
            
        case MT_ERR_PROTO:
            pStr = "MT_ERR_PROTO";
            break;
            
        case MT_ERR_CS:
            pStr = "MT_ERR_CS";
            break;
            
        case MT_ERR_0x16:
            pStr = "MT_ERR_0x16";
            break;

        case MT_ERR_LOGIC:
            pStr = "MT_ERR_LOGIC";
            break;
           
        case MT_ERR_PACK:
            pStr = "MT_ERR_PACK";
            break;
            
        case MT_ERR_PFC:
            pStr = "MT_ERR_PFC";
            break;
            
        case MT_ERR_TIMEOUT:
            pStr = "MT_ERR_TIMEOUT";
            break;
            
        case MT_ERR_PRM:
            pStr = "MT_ERR_PRM";
            break;
            
        case MT_ERR_TEAM:
            pStr = "MT_ERR_TEAM";
            break;
            
         default:
            pStr = "Unkown Err";
            break;
    }

    return pStr;
    
}
#endif

/*****************************************************************************
 函 数 名  : MT_FREE
 功能描述  : 释放malloc申请的内存空间
 输入参数  : UINT8 *pBuf  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月22日
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
#define  MT_FREE(pBuf)\
if(pBuf != NULL)\
{\
    free(pBuf);\
    pBuf = NULL;\
}

/*******************************************************
 *  全局参数信息
 *  提供命令类型 与 Pn类型、解析函数的关系映射  
 *
{*////
BOOL      g_bMtInit = FALSE;             // 协议是否初始化
eMtRole   g_eMtRole = MT_ROLE_UNKOWN;    // 身份，主站或是从站
UINT8     g_ucMtPermitDelayMinutes = 0;  // 允许时延
UINT8     g_aucPw[MT_PW_LEN] = {0};
sMtEC     g_tEC;                         // 事件计数器 仅终端上支持

#if MT_CFG_ENCRYPT
    peMtEncryptFunc   g_peMtEncryptFunc = NULL;  // 加密接口
    peMtDecryptFunc   g_peMtDecryptFunc = NULL;  // 解密接口
#endif
///*}
    
/*******************************************************
 *  命令信息表
 *  提供命令类型 与 Pn类型、解析函数的关系映射  
 *
{*////
const sMtCmdInfor gmt_cmdinfor[] = 
{
     /*******************************
     *  (0) 确认/否认（AFN=00H）
     *
     *  上行: 本类型 
     *  下行: 本类型
     *  加密: 不需要
     *
    {*///
    {CMD_AFN_0_F1_ALL_OK,            MT_DIR_M2S, MT_PN_P0,  NULL,                  "全部确认：对收到报文中的全部数据单元标识进行确认"},
    {CMD_AFN_0_F1_ALL_OK,            MT_DIR_S2M, MT_PN_P0,  NULL,                  "全部确认：对收到报文中的全部数据单元标识进行确认"},
    {CMD_AFN_0_F2_ALL_DENY,          MT_DIR_M2S, MT_PN_P0,  NULL,                  "全部否认：对收到报文中的全部数据单元标识进行否认"},
    {CMD_AFN_0_F2_ALL_DENY,          MT_DIR_S2M, MT_PN_P0,  NULL,                  "全部否认：对收到报文中的全部数据单元标识进行否认"},
    {CMD_AFN_0_F3_ONE_BY_ONE,        MT_DIR_M2S, MT_PN_P0,  emtTrans_OneByOne,     "按数据单元标识确认和否认：对收到报文中的全部数据单元标识进行逐个确认/否认"},
    {CMD_AFN_0_F3_ONE_BY_ONE,        MT_DIR_S2M, MT_PN_P0,  emtTrans_OneByOne,     "按数据单元标识确认和否认：对收到报文中的全部数据单元标识进行逐个确认/否认"},
    ///*}

     /*******************************
     *  (1) 复位命令（AFN=01H)
     *
     *  上行: AFN 00H 确认/否认
     *  下行: 本类型
     *  加密: 需要
     *  
    {*///
    {CMD_AFN_1_F1_HARD_INIT,         MT_DIR_M2S, MT_PN_P0,  NULL,                  "硬件初始化"},
    {CMD_AFN_1_F2_DATA_INIT,         MT_DIR_M2S, MT_PN_P0,  NULL,                  "数据区初始化"},
    {CMD_AFN_1_F3_FACTORY_RESET,     MT_DIR_M2S, MT_PN_P0,  NULL,                  "参数及全体数据区初始化（即恢复至出厂配置）"},
    {CMD_AFN_1_F4_PARA_INIT,         MT_DIR_M2S, MT_PN_P0,  NULL,                  "参数（除与系统主站通信有关的）及全体数据区初始化"},
    ///*}
 
     /*******************************
     *  (2) 链路接口检测（AFN=02H）
     *
     *  上行: 本类型
     *  下行: AFN 00H 确认/否认  
     *  加密: 不需要
     *
    {*///
    {CMD_AFN_2_F1_LOG_IN,            MT_DIR_S2M, MT_PN_P0,  NULL,                  "登录"},
    {CMD_AFN_2_F2_LOG_OUT,           MT_DIR_S2M, MT_PN_P0,  NULL,                  "退出登录"},
    {CMD_AFN_2_F3_HEART_BEAT,        MT_DIR_S2M, MT_PN_P0,  NULL,                  "心跳"},
    ///*}

     /*******************************
     *  (3) 中继站命令(AFN=03H）
     *
     *  上行: 本类型
     *  下行: 本类型
     *  加密: 不需要
    {*///
    // 上行
    {CMD_AFN_3_F1_RELAY_CTRL,        MT_DIR_S2M, MT_PN_P0,  emtTrans_afn03f1,      "中继站工作状态控制"},
    {CMD_AFN_3_F2_RELAY_STAT_QUERY,  MT_DIR_S2M, MT_PN_P0,  NULL,                  "中继站工作状态查询"},
    {CMD_AFN_3_F3_RELAY_RECD_QUERY,  MT_DIR_S2M, MT_PN_P0,  NULL,                  "中继站工作状态切换记录查义"},
    {CMD_AFN_3_F4_HEART_DATA_QUERY,  MT_DIR_S2M, MT_PN_P0,  NULL,                  "中继站运行状态统计查询"},
    // 下行
    {CMD_AFN_3_F1_RELAY_CTRL,        MT_DIR_M2S, MT_PN_P0,  emtTrans_afn03f1,      "中继站工作状态控制"},
    {CMD_AFN_3_F2_RELAY_STAT_QUERY,  MT_DIR_M2S, MT_PN_P0,  NULL,                  "中继站工作状态查询"},
    {CMD_AFN_3_F3_RELAY_RECD_QUERY,  MT_DIR_M2S, MT_PN_P0,  NULL,                  "中继站工作状态切换记录查义"},
    {CMD_AFN_3_F4_HEART_DATA_QUERY,  MT_DIR_M2S, MT_PN_P0,  NULL,                  "中继站运行状态统计查询"},
    ///*}


     /*******************************
     *  (4) 设置参数(AFN=04H）
     *
     *  上行: AFN 00H 确认/否认 
     *  下行: 本类型
     *  加密: 需要
     *
    {*///
    // 组1   pn:p0
    {CMD_AFN_4_F1_TML_UP_CFG,        MT_DIR_M2S, MT_PN_P0,  emtTrans_afn04f1,      "终端上行通信口通信参数设置"},
    {CMD_AFN_4_F2_TML_WIRELESS_CFG,  MT_DIR_M2S, MT_PN_P0,  emtTrans_afn04f2,      "终端上行通信口无线中继转发设置"},
    {CMD_AFN_4_F3_MST_IP_PORT,       MT_DIR_M2S, MT_PN_P0,  emtTrans_afn04f3,      "主站IP地址和端口"},
    {CMD_AFN_4_F4_MST_PHONE_SMS,     MT_DIR_M2S, MT_PN_P0,  emtTrans_afn04f4,      "主站电话号码和短信中心号码"},
    {CMD_AFN_4_F5_TML_UP_AUTH,       MT_DIR_M2S, MT_PN_P0,  emtTrans_afn04f5,      "终端上行通信消息认证参数设置"},
    {CMD_AFN_4_F6_TEAM_ADDR,         MT_DIR_M2S, MT_PN_P0,  emtTrans_afn04f6,      "集中器组地址设置"},
    {CMD_AFN_4_F7_TML_IP_PORT,       MT_DIR_M2S, MT_PN_P0,  emtTrans_afn04f7,      "集中器IP地址和端口"},
    {CMD_AFN_4_F8_TML_UP_WAY,        MT_DIR_M2S, MT_PN_P0,  emtTrans_afn04f8,      "集中器上行通信工作方式"},

    // 组2   pn:p0
    {CMD_AFN_4_F9_TML_EVENT_CFG,     MT_DIR_M2S, MT_PN_P0,  emtTrans_afn04f9,      "终端事件记录配置设置"},
    {CMD_AFN_4_F10_TML_POWER_CFG,    MT_DIR_M2S, MT_PN_P0,  emtTrans_afn04f10,     "终端电能表/交流采样装置配置参数"},
    {CMD_AFN_4_F11_TML_PULSE_CFG,    MT_DIR_M2S, MT_PN_P0,  emtTrans_afn04f11,     "终端脉冲配置参数"},
    {CMD_AFN_4_F12_TML_STATE_INPUT,  MT_DIR_M2S, MT_PN_P0,  emtTrans_afn04f12,     "终端/集中器状态量输入参数"},
    {CMD_AFN_4_F13_TML_SIMULA_CFG,   MT_DIR_M2S, MT_PN_P0,  emtTrans_afn04f13,     "终端电压/电流模拟量配置参数"},
    {CMD_AFN_4_F14_TML_GRUP_TOTL,    MT_DIR_M2S, MT_PN_P0,  emtTrans_afn04f14,     "终端总加组配置参数"},
    {CMD_AFN_4_F15_HAVE_DIFF_EVENT,  MT_DIR_M2S, MT_PN_P0,  emtTrans_afn04f15,     "有功总电能量差动超限事件参数设置"},
    {CMD_AFN_4_F16_VPN_USER_PWD,     MT_DIR_M2S, MT_PN_P0,  emtTrans_afn04f16,     "虚拟专网用户名、密码"},

    // 组3   pn:p0
    {CMD_AFN_4_F17_TML_SAFE_VALUE,   MT_DIR_M2S, MT_PN_P0,  emtTrans_afn04f17,     "终端保安定值"},
    {CMD_AFN_4_F18_TML_PCTRL_PERD,   MT_DIR_M2S, MT_PN_P0,  emtTrans_afn04f18,     "终端功控时段"},
    {CMD_AFN_4_F19_TML_PCTRL_FACTOR, MT_DIR_M2S, MT_PN_P0,  emtTrans_afn04f19,     "终端时段功控定值浮动系数"},
    {CMD_AFN_4_F20_TML_MONTH_FACTOR, MT_DIR_M2S, MT_PN_P0,  emtTrans_afn04f20,     "终端月电能量控定值浮动系数"},
    {CMD_AFN_4_F21_TML_POWER_FACTOR, MT_DIR_M2S, MT_PN_P0,  emtTrans_afn04f21,     "终端电能量费率时段和费率数"},
    {CMD_AFN_4_F22_TML_POWER_RATE,   MT_DIR_M2S, MT_PN_P0,  emtTrans_afn04f22,     "终端电能量费率"},
    {CMD_AFN_4_F23_TML_WARNING_CFG,  MT_DIR_M2S, MT_PN_P0,  emtTrans_afn04f23,     "终端催告警参数"},

    // 组4   pn:测量点号
    {CMD_AFN_4_F25_MP_BASE_CFG,      MT_DIR_M2S, MT_PN_MP,  emtTrans_afn04f25,     "测量点基本参数"},
    {CMD_AFN_4_F26_MP_LIMIT_CFG,     MT_DIR_M2S, MT_PN_MP,  emtTrans_afn04f26,     "测量点限值参数"},
    {CMD_AFN_4_F27_MP_LOSS_CFG,      MT_DIR_M2S, MT_PN_MP,  emtTrans_afn04f27,     "测量点铜损、铁损参数"},
    {CMD_AFN_4_F28_MP_PERIOD_FACTOR, MT_DIR_M2S, MT_PN_MP,  emtTrans_afn04f28,     "测量点功率因数分段限值"},
    {CMD_AFN_4_F29_TML_METER_ID,     MT_DIR_M2S, MT_PN_MP,  emtTrans_afn04f29,     "终端当地电能表显示号"},
    {CMD_AFN_4_F30_TML_AUTO_READ,    MT_DIR_M2S, MT_PN_MP,  emtTrans_afn04f30,     "终端台区集中抄表停抄/投抄设置"},
    {CMD_AFN_4_F31_SLAVE_ADDR,       MT_DIR_M2S, MT_PN_MP,  emtTrans_afn04f31,     "载波从节点附属节点地址"},

    // 组5   pn:p0
    {CMD_AFN_4_F33_TML_READ_CFG,     MT_DIR_M2S, MT_PN_P0,  emtTrans_afn04f33,     "终端抄表运行参数设置"},
    {CMD_AFN_4_F34_CON_DOWN_CFG,     MT_DIR_M2S, MT_PN_P0,  emtTrans_afn04f34,     "集中器下行通信模拟的参数设置"},
    {CMD_AFN_4_F35_TML_READ_VIP,     MT_DIR_M2S, MT_PN_P0,  emtTrans_afn04f35,     "终端台区集中抄表重点户设置"},
    {CMD_AFN_4_F36_TML_UP_LIMIT,     MT_DIR_M2S, MT_PN_P0,  emtTrans_afn04f36,     "终端上行通信流量门限设置"},
    {CMD_AFN_4_F37_TML_CASC_CFG,     MT_DIR_M2S, MT_PN_P0,  emtTrans_afn04f37,     "终端级联通信参数"},
    {CMD_AFN_4_F38_CFG_ASK_1,        MT_DIR_M2S, MT_PN_P0,  emtTrans_afn04f38,     "1类数据配置设置(在终端支持的1类数据配置内)"},
    {CMD_AFN_4_F39_CFG_ASK_2,        MT_DIR_M2S, MT_PN_P0,  emtTrans_afn04f39,     "2类数据配置设置(在终端支持的2类数据配置内)"},

    // 组6   pn:总加组号
    {CMD_AFN_4_F41_PERIOD_VALUE,     MT_DIR_M2S, MT_PN_GT,  emtTrans_afn04f41,     "时段功控定值"},
    {CMD_AFN_4_F42_FACTORY_HOLIDAY,  MT_DIR_M2S, MT_PN_GT,  emtTrans_afn04f42,     "厂休功控参数"},
    {CMD_AFN_4_F43_SLIDE_TIME,       MT_DIR_M2S, MT_PN_GT,  emtTrans_afn04f43,     "功率控制的功率计算滑差时间"},
    {CMD_AFN_4_F44_SHUTOUT_CFG,      MT_DIR_M2S, MT_PN_GT,  emtTrans_afn04f44,     "营业报停控参数"},
    {CMD_AFN_4_F45_CTRL_TURN_CFG,    MT_DIR_M2S, MT_PN_GT,  emtTrans_afn04f45,     "功控轮次设定"},
    {CMD_AFN_4_F46_MONTH_FIX_VALUE,  MT_DIR_M2S, MT_PN_GT,  emtTrans_afn04f46,     "月电量控定值"},
    {CMD_AFN_4_F47_BUY_COUNT,        MT_DIR_M2S, MT_PN_GT,  emtTrans_afn04f47,     "购电量(费)控参数"},
    {CMD_AFN_4_F48_ELEC_TURN_CFG,    MT_DIR_M2S, MT_PN_GT,  emtTrans_afn04f48,     "电控轮次设定"},

    // 组7  pn:控制轮次
    {CMD_AFN_4_F49_WARNING_TIME,     MT_DIR_M2S, MT_PN_CT,  emtTrans_afn04f49,     "功控告警时间"},

    // 组8  pn:总加组号
    {CMD_AFN_4_F57_TML_WARN_SOUND,   MT_DIR_M2S, MT_PN_GT,  emtTrans_afn04f57,     "终端声音告警允许/禁止设置"},
    {CMD_AFN_4_F58_TML_ATUO_PROTECT, MT_DIR_M2S, MT_PN_GT,  emtTrans_afn04f58,     "终端自动保电设置"},
    {CMD_AFN_4_F59_METER_LIMIT,      MT_DIR_M2S, MT_PN_GT,  emtTrans_afn04f59,     "电能表异常判别阈值设置"},
    {CMD_AFN_4_F60_HUMOR_LIMIT,      MT_DIR_M2S, MT_PN_GT,  emtTrans_afn04f60,     "谐波限值"},
    {CMD_AFN_4_F61_DC_SIMULA,        MT_DIR_M2S, MT_PN_GT,  emtTrans_afn04f61,     "直流模拟量接入参数"},

    // 组9  pn:任务号
    {CMD_AFN_4_F65_CFG_AUTO_1,       MT_DIR_M2S, MT_PN_TK,  emtTrans_afn04f65,     "定时上报1类数据任务设置"},
    {CMD_AFN_4_F66_CFG_AUTO_2,       MT_DIR_M2S, MT_PN_TK,  emtTrans_afn04f66,     "定时上报2类数据任务设置"},
    {CMD_AFN_4_F67_GOP_AUTO_1,       MT_DIR_M2S, MT_PN_TK,  emtTrans_afn04f67,     "定时上报1类数据任务启动/停止设置"},
    {CMD_AFN_4_F68_GOP_AUTO_2,       MT_DIR_M2S, MT_PN_TK,  emtTrans_afn04f68,     "定时上报2类数据任务启动/停止设置"},

    // 组10  pn:测量点号   
    {CMD_AFN_4_F73_CAPA_CFG,         MT_DIR_M2S, MT_PN_MP,  emtTrans_afn04f73,     "电容器参数"},
    {CMD_AFN_4_F74_CAPA_RUN_CFG,     MT_DIR_M2S, MT_PN_MP,  emtTrans_afn04f74,     "电容器投切运行参数"},
    {CMD_AFN_4_F75_CAPA_PROT_PARA,   MT_DIR_M2S, MT_PN_MP,  emtTrans_afn04f75,     "电容器保护参数"},
    {CMD_AFN_4_F76_CAPA_CTRL_WAY,    MT_DIR_M2S, MT_PN_MP,  emtTrans_afn04f76,     "电容器投切控制方式"},

    // 组11 pn:直流模拟量端口号   
    {CMD_AFN_4_F81_CD_CHANGE_RATE,   MT_DIR_M2S, MT_PN_DP,  emtTrans_afn04f81,     "直流模拟量变比"},
    {CMD_AFN_4_F82_CD_LIMIT,         MT_DIR_M2S, MT_PN_DP,  emtTrans_afn04f82,     "直流模拟量限值"},
    {CMD_AFN_4_F83_CD_FREEZE_PARA,   MT_DIR_M2S, MT_PN_DP,  emtTrans_afn04f83,     "直流模拟量冻结参数"}, 
    ///*}

     /*******************************
     *  (5) 控制命令(AFN=05H）
     *
     *  上行: AFN 00H 确认/否认 
     *  下行: 本类型
     *  加密: 需要
     *
    {*///
    // 组1  pn:控制轮次   
    {CMD_AFN_5_F1_REMOTE_TURN_OFF,   MT_DIR_M2S, MT_PN_CT,  emtTrans_afn05f1,      "遥控跳闸"},
    {CMD_AFN_5_F2_PERMIT_TURN_ON,    MT_DIR_M2S, MT_PN_CT,  NULL,                  "允许合闸"},   
              
    // 组2  pn:总加组号        
    {CMD_AFN_5_F9_PERIOD_GO,         MT_DIR_M2S, MT_PN_GT,  emtTrans_afn05f9,      "时段功控投入"},
    {CMD_AFN_5_F10_HOLIDAY_GO,       MT_DIR_M2S, MT_PN_GT,  NULL,                  "厂休功控投入"},   
    {CMD_AFN_5_F11_SHUTOUT_GO,       MT_DIR_M2S, MT_PN_GT,  NULL,                  "营业报停功控投入"},
    {CMD_AFN_5_F12_DOWN_GO,          MT_DIR_M2S, MT_PN_GT,  NULL,                  "当前功率下浮控投入"},   
    {CMD_AFN_5_F15_MONTH_GO,         MT_DIR_M2S, MT_PN_GT,  emtTrans_afn05f12,     "月电控投入"},
    {CMD_AFN_5_F16_BUY_GO,           MT_DIR_M2S, MT_PN_GT,  NULL,                  "购电控投入"},   

    // 组3  pn:总加组号
    {CMD_AFN_5_F17_PERIOD_STOP,      MT_DIR_M2S, MT_PN_GT,  NULL,                  "时段功控解除"},
    {CMD_AFN_5_F18_HOLIDAY_STOP,     MT_DIR_M2S, MT_PN_GT,  NULL,                  "厂休功控解除"},   
    {CMD_AFN_5_F19_SHUTOUT_STOP,     MT_DIR_M2S, MT_PN_GT,  NULL,                  "营业报停功控解除"},
    {CMD_AFN_5_F20_DOWN_STOP,        MT_DIR_M2S, MT_PN_GT,  NULL,                  "当前功率下浮控解除"},   
    {CMD_AFN_5_F23_MONTH_STOP,       MT_DIR_M2S, MT_PN_GT,  NULL,                  "月电控解除"},
    {CMD_AFN_5_F24_BUY_STOP,         MT_DIR_M2S, MT_PN_GT,  NULL,                  "购电控解除"},   

    // 组4  pn:p0 
    {CMD_AFN_5_F25_TML_PROTECT_GO,   MT_DIR_M2S, MT_PN_P0,  emtTrans_afn05f25,     "终端电保投入"},
    {CMD_AFN_5_F26_WARN_PAY_GO,      MT_DIR_M2S, MT_PN_P0,  NULL,                  "催费告警投入"},   
    {CMD_AFN_5_F27_PERMIT_TALK_GO,   MT_DIR_M2S, MT_PN_P0,  NULL,                  "允许终端与主站通话"},
    {CMD_AFN_5_F28_TAKE_OFF_TML_GO,  MT_DIR_M2S, MT_PN_P0,  NULL,                  "终端剔除投入"},   
    {CMD_AFN_5_F29_AUTO_SAY_GO,      MT_DIR_M2S, MT_PN_P0,  NULL,                  "允许终端主动上报"},
    {CMD_AFN_5_F31_CHECK_TIME,       MT_DIR_M2S, MT_PN_P0,  emtTrans_afn05f31,     "对时命令"},   
    {CMD_AFN_5_F32_CHINESE_INFO,     MT_DIR_M2S, MT_PN_P0,  emtTrans_afn05f32,     "中文信息"},   

    // 组5 pn:p0 
    {CMD_AFN_5_F33_TML_PROTECT_STOP, MT_DIR_M2S, MT_PN_P0,  NULL,                  "终端电保解除"},
    {CMD_AFN_5_F34_WARN_PAY_STOP,    MT_DIR_M2S, MT_PN_P0,  NULL,                  "催费告警解除"},   
    {CMD_AFN_5_F35_PERMIT_TALK_STOP, MT_DIR_M2S, MT_PN_P0,  NULL,                  "禁止终端与主站通话"},
    {CMD_AFN_5_F36_TAKE_OFF_TML_STOP,MT_DIR_M2S, MT_PN_P0,  NULL,                  "终端剔除解除"},   
    {CMD_AFN_5_F37_AUTO_SAY_STOP,    MT_DIR_M2S, MT_PN_P0,  NULL,                  "禁止终端主动上报"},
    {CMD_AFN_5_F38_TML_LINK_ON,      MT_DIR_M2S, MT_PN_P0,  NULL,                  "激活终端连接主站"},   
    {CMD_AFN_5_F39_TML_LINK_OFF,     MT_DIR_M2S, MT_PN_P0,  NULL,                  "命令终端断开连接"},   

    // 组6 pn:测量点号 
    {CMD_AFN_5_F41_CAPA_CTRL_GO,     MT_DIR_M2S, MT_PN_MP,  emtTrans_afn05f41,     "电容器控制投入"},
    {CMD_AFN_5_F42_CAPA_CTRL_STOP,   MT_DIR_M2S, MT_PN_MP,  emtTrans_afn05f42,     "电容器控制解除"},   

    // 组7 pn:p0  
    {CMD_AFN_5_F49_READ_METER_STOP,  MT_DIR_M2S, MT_PN_P0,  emtTrans_afn05f49,     "命令指定通信端口暂停抄表"},
    {CMD_AFN_5_F50_READ_METER_GO,    MT_DIR_M2S, MT_PN_P0,  emtTrans_afn05f50,     "命令指定通信端口恢复抄表"},   
    {CMD_AFN_5_F51_READ_METER_REDO,  MT_DIR_M2S, MT_PN_P0,  emtTrans_afn05f51,     "命令指定通信端口重新抄表"},
    {CMD_AFN_5_F52_INIT_ROUTE,       MT_DIR_M2S, MT_PN_P0,  emtTrans_afn05f52,     "初始化指定通信端口下的全部中继路由信息"},   
    {CMD_AFN_5_F53_DELET_ALL_METER,  MT_DIR_M2S, MT_PN_P0,  emtTrans_afn05f53,     "删除指定通信端口下的全部电表"}, 
    ///*}

     /*******************************
     *  (6) 身份认证及密钥协商(AFN=06H）
     *
     *  上行: 本类型
     *  下行: 本类型
     *  加密: 不需要
     *  
    {*///
    // 上行
    {CMD_AFN_6_F1_ID_AUTH_Q,         MT_DIR_S2M, MT_PN_P0,  emtTrans_afn06f1,     "身份认证请求"},
    {CMD_AFN_6_F2_ID_AUTH_A,         MT_DIR_S2M, MT_PN_P0,  emtTrans_afn06f2,     "身份认证响应"},
    {CMD_AFN_6_F3_RAND_Q,            MT_DIR_S2M, MT_PN_P0,  emtTrans_afn06f3,     "取随机数请求"},
    {CMD_AFN_6_F4_RAND_A,            MT_DIR_S2M, MT_PN_P0,  emtTrans_afn06f4,     "取随机数响应"},

    // 下行
    {CMD_AFN_6_F1_ID_AUTH_Q,         MT_DIR_M2S, MT_PN_P0,  emtTrans_afn06f1,     "身份认证请求"},
    {CMD_AFN_6_F2_ID_AUTH_A,         MT_DIR_M2S, MT_PN_P0,  emtTrans_afn06f2,     "身份认证响应"},
    {CMD_AFN_6_F3_RAND_Q,            MT_DIR_M2S, MT_PN_P0,  emtTrans_afn06f3,     "取随机数请求"},
    {CMD_AFN_6_F4_RAND_A,            MT_DIR_M2S, MT_PN_P0,  emtTrans_afn06f4,     "取随机数响应"},
    ///*}


     /*******************************
     *  (7) 请求被级联终端主动上报(AFN=08H）
     *
     *  上行: 有主动上报的数据时, 分别用请求1类数据和请求2类数据上行报文进行应答
     *        无主动上报的数据时, AFN 00H 否认 CMD_AFN_0_F2_ALL_DENY
     *  下行: 本类型
     *  加密: 不需要 
     *
    {*///
    {CMD_AFN_8_F1_CALL_AUTO_REPT,    MT_DIR_M2S, MT_PN_P0,  NULL,                  "请求被级联终端主动上报"},
    ///*}

     /*******************************
     *  (8) 请求终端配置及信息(AFN=09H）
     *
     *  上行: 本类型
     *  下行: 本类型
     *  加密: 不需要
     *  
    {*///
    // 上行
    {CMD_AFN_9_F1_TML_VERSION,       MT_DIR_S2M, MT_PN_P0,  emtTrans_afn09f1,      "终端版本信息"},
    {CMD_AFN_9_F2_TML_INPUT_OUT,     MT_DIR_S2M, MT_PN_P0,  emtTrans_afn09f2,      "终端支持的输入、输出及通信端口配置"},
    {CMD_AFN_9_F3_TML_OTHER_CFG,     MT_DIR_S2M, MT_PN_P0,  emtTrans_afn09f3,      "终端支持的其他配置"},
    {CMD_AFN_9_F4_SUPPORT_PARA_CFG,  MT_DIR_S2M, MT_PN_P0,  emtTrans_afn09f4,      "终端支持的参数配置"},
    {CMD_AFN_9_F5_SUPPORT_CTRL_CFG,  MT_DIR_S2M, MT_PN_P0,  emtTrans_afn09f5,      "终端支持的控制配置"},
    {CMD_AFN_9_F6_SUPPORT_ASK1_CFG,  MT_DIR_S2M, MT_PN_P0,  emtTrans_afn09f6,      "终端支持的1类数据配置"},
    {CMD_AFN_9_F7_SUPPORT_ASK2_CFG,  MT_DIR_S2M, MT_PN_P0,  emtTrans_afn09f7,      "终端支持的2类数据配置"},
    {CMD_AFN_9_F8_SUPPORT_EVNT_CFG,  MT_DIR_S2M, MT_PN_P0,  emtTrans_afn09f8,      "终端支持的事件记录配置"},

    // 下行
    {CMD_AFN_9_F1_TML_VERSION,       MT_DIR_M2S, MT_PN_P0,  NULL,                  "终端版本信息"},
    {CMD_AFN_9_F2_TML_INPUT_OUT,     MT_DIR_M2S, MT_PN_P0,  NULL,                  "终端支持的输入、输出及通信端口配置"},
    {CMD_AFN_9_F3_TML_OTHER_CFG,     MT_DIR_M2S, MT_PN_P0,  NULL,                  "终端支持的其他配置"},
    {CMD_AFN_9_F4_SUPPORT_PARA_CFG,  MT_DIR_M2S, MT_PN_P0,  NULL,                  "终端支持的参数配置"},
    {CMD_AFN_9_F5_SUPPORT_CTRL_CFG,  MT_DIR_M2S, MT_PN_P0,  NULL,                  "终端支持的控制配置"},
    {CMD_AFN_9_F6_SUPPORT_ASK1_CFG,  MT_DIR_M2S, MT_PN_P0,  NULL,                  "终端支持的1类数据配置"},
    {CMD_AFN_9_F7_SUPPORT_ASK2_CFG,  MT_DIR_M2S, MT_PN_P0,  NULL,                  "终端支持的2类数据配置"},
    {CMD_AFN_9_F8_SUPPORT_EVNT_CFG,  MT_DIR_M2S, MT_PN_P0,  NULL,                  "终端支持的事件记录配置"},
    ///*}


     /*******************************
     *  (9) 查询参数(AFN=0AH）
     *
     *  上行: 本类型
     *  下行: 本类型
     *  加密: 不需要
     *  
    {*///
    /** { 上行 **/ 
    // 组1   pn:p0
    {CMD_AFN_A_F1_TML_UP_CFG,        MT_DIR_S2M, MT_PN_P0,  emtTrans_afn04f1,      "终端上行通信口通信参数设置"},
    {CMD_AFN_A_F2_TML_WIRELESS_CFG,  MT_DIR_S2M, MT_PN_P0,  emtTrans_afn04f2,      "终端上行通信口无线中继转发设置"},
    {CMD_AFN_A_F3_MST_IP_PORT,       MT_DIR_S2M, MT_PN_P0,  emtTrans_afn04f3,      "主站IP地址和端口"},
    {CMD_AFN_A_F4_MST_PHONE_SMS,     MT_DIR_S2M, MT_PN_P0,  emtTrans_afn04f4,      "主站电话号码和短信中心号码"},
    {CMD_AFN_A_F5_TML_UP_AUTH,       MT_DIR_S2M, MT_PN_P0,  emtTrans_afn04f5,      "终端上行通信消息认证参数设置"},
    {CMD_AFN_A_F6_TEAM_ADDR,         MT_DIR_S2M, MT_PN_P0,  emtTrans_afn04f6,      "集中器组地址设置"},
    {CMD_AFN_A_F7_TML_IP_PORT,       MT_DIR_S2M, MT_PN_P0,  emtTrans_afn04f7,      "集中器IP地址和端口"},
    {CMD_AFN_A_F8_TML_UP_WAY,        MT_DIR_S2M, MT_PN_P0,  emtTrans_afn04f8,      "集中器上行通信工作方式"},

    // 组2   pn:p0
    {CMD_AFN_A_F9_TML_EVENT_CFG,     MT_DIR_S2M, MT_PN_P0,  emtTrans_afn04f9,      "终端事件记录配置设置"},
    {CMD_AFN_A_F10_TML_POWER_CFG,    MT_DIR_S2M, MT_PN_P0,  emtTrans_afn04f10,     "终端电能表/交流采样装置配置参数"},
    {CMD_AFN_A_F11_TML_PULSE_CFG,    MT_DIR_S2M, MT_PN_P0,  emtTrans_afn04f11,     "终端脉冲配置参数"},
    {CMD_AFN_A_F12_TML_STATE_INPUT,  MT_DIR_S2M, MT_PN_P0,  emtTrans_afn04f12,     "终端/集中器状态量输入参数"},
    {CMD_AFN_A_F13_TML_SIMULA_CFG,   MT_DIR_S2M, MT_PN_P0,  emtTrans_afn04f13,     "终端电压/电流模拟量配置参数"},
    {CMD_AFN_A_F14_TML_GRUP_TOTL,    MT_DIR_S2M, MT_PN_P0,  emtTrans_afn04f14,     "终端总加组配置参数"},
    {CMD_AFN_A_F15_HAVE_DIFF_EVENT,  MT_DIR_S2M, MT_PN_P0,  emtTrans_afn04f15,     "有功总电能量差动超限事件参数设置"},
    {CMD_AFN_A_F16_VPN_USER_PWD,     MT_DIR_S2M, MT_PN_P0,  emtTrans_afn04f16,     "虚拟专网用户名、密码"},

    // 组3   pn:p0
    {CMD_AFN_A_F17_TML_SAFE_VALUE,   MT_DIR_S2M, MT_PN_P0,  emtTrans_afn04f17,     "终端保安定值"},
    {CMD_AFN_A_F18_TML_PCTRL_PERD,   MT_DIR_S2M, MT_PN_P0,  emtTrans_afn04f18,     "终端功控时段"},
    {CMD_AFN_A_F19_TML_PCTRL_FACTOR, MT_DIR_S2M, MT_PN_P0,  emtTrans_afn04f19,     "终端时段功控定值浮动系数"},
    {CMD_AFN_A_F20_TML_MONTH_FACTOR, MT_DIR_S2M, MT_PN_P0,  emtTrans_afn04f20,     "终端月电能量控定值浮动系数"},
    {CMD_AFN_A_F21_TML_POWER_FACTOR, MT_DIR_S2M, MT_PN_P0,  emtTrans_afn04f21,     "终端电能量费率时段和费率数"},
    {CMD_AFN_A_F22_TML_POWER_RATE,   MT_DIR_S2M, MT_PN_P0,  emtTrans_afn04f22,     "终端电能量费率"},
    {CMD_AFN_A_F23_TML_WARNING_CFG,  MT_DIR_S2M, MT_PN_P0,  emtTrans_afn04f23,     "终端催告警参数"},

    // 组4   pn:测量点号
    {CMD_AFN_A_F25_MP_BASE_CFG,      MT_DIR_S2M, MT_PN_MP,  emtTrans_afn04f25,     "测量点基本参数"},
    {CMD_AFN_A_F26_MP_LIMIT_CFG,     MT_DIR_S2M, MT_PN_MP,  emtTrans_afn04f26,     "测量点限值参数"},
    {CMD_AFN_A_F27_MP_LOSS_CFG,      MT_DIR_S2M, MT_PN_MP,  emtTrans_afn04f27,     "测量点铜损、铁损参数"},
    {CMD_AFN_A_F28_MP_PERIOD_FACTOR, MT_DIR_S2M, MT_PN_MP,  emtTrans_afn04f28,     "测量点功率因数分段限值"},
    {CMD_AFN_A_F29_TML_METER_ID,     MT_DIR_S2M, MT_PN_MP,  emtTrans_afn04f29,     "终端当地电能表显示号"},
    {CMD_AFN_A_F30_TML_AUTO_READ,    MT_DIR_S2M, MT_PN_MP,  emtTrans_afn04f30,     "终端台区集中抄表停抄/投抄设置"},
    {CMD_AFN_A_F31_SLAVE_ADDR,       MT_DIR_S2M, MT_PN_MP,  emtTrans_afn04f31,     "载波从节点附属节点地址"},

    // 组5   pn:p0
    {CMD_AFN_A_F33_TML_READ_CFG,     MT_DIR_S2M, MT_PN_P0,  emtTrans_afn04f33,     "终端抄表运行参数设置"},
    {CMD_AFN_A_F34_CON_DOWN_CFG,     MT_DIR_S2M, MT_PN_P0,  emtTrans_afn04f34,     "集中器下行通信模拟的参数设置"},
    {CMD_AFN_A_F35_TML_READ_VIP,     MT_DIR_S2M, MT_PN_P0,  emtTrans_afn04f35,     "终端台区集中抄表重点户设置"},
    {CMD_AFN_A_F36_TML_UP_LIMIT,     MT_DIR_S2M, MT_PN_P0,  emtTrans_afn04f36,     "终端上行通信流量门限设置"},
    {CMD_AFN_A_F37_TML_CASC_CFG,     MT_DIR_S2M, MT_PN_P0,  emtTrans_afn04f37,     "终端级联通信参数"},
    {CMD_AFN_A_F38_CFG_ASK_1,        MT_DIR_S2M, MT_PN_P0,  emtTrans_afn04f38,     "1类数据配置设置(在终端支持的1类数据配置内)"},
    {CMD_AFN_A_F39_CFG_ASK_2,        MT_DIR_S2M, MT_PN_P0,  emtTrans_afn04f39,     "2类数据配置设置(在终端支持的2类数据配置内)"},

    // 组6   pn:总加组号
    {CMD_AFN_A_F41_PERIOD_VALUE,     MT_DIR_S2M, MT_PN_GT,  emtTrans_afn04f41,     "时段功控定值"},
    {CMD_AFN_A_F42_FACTORY_HOLIDAY,  MT_DIR_S2M, MT_PN_GT,  emtTrans_afn04f42,     "厂休功控参数"},
    {CMD_AFN_A_F43_SLIDE_TIME,       MT_DIR_S2M, MT_PN_GT,  emtTrans_afn04f43,     "功率控制的功率计算滑差时间"},
    {CMD_AFN_A_F44_SHUTOUT_CFG,      MT_DIR_S2M, MT_PN_GT,  emtTrans_afn04f44,     "营业报停控参数"},
    {CMD_AFN_A_F45_CTRL_TURN_CFG,    MT_DIR_S2M, MT_PN_GT,  emtTrans_afn04f45,     "功控轮次设定"},
    {CMD_AFN_A_F46_MONTH_FIX_VALUE,  MT_DIR_S2M, MT_PN_GT,  emtTrans_afn04f46,     "月电量控定值"},
    {CMD_AFN_A_F47_BUY_COUNT,        MT_DIR_S2M, MT_PN_GT,  emtTrans_afn04f47,     "购电量(费)控参数"},
    {CMD_AFN_A_F48_ELEC_TURN_CFG,    MT_DIR_S2M, MT_PN_GT,  emtTrans_afn04f48,     "电控轮次设定"},

    // 组7  pn:控制轮次
    {CMD_AFN_A_F49_WARNING_TIME,     MT_DIR_S2M, MT_PN_CT,  emtTrans_afn04f49,     "功控告警时间"},

    // 组8  pn:总加组号
    {CMD_AFN_A_F57_TML_WARN_SOUND,   MT_DIR_S2M, MT_PN_GT,  emtTrans_afn04f57,     "终端声音告警允许/禁止设置"},
    {CMD_AFN_A_F58_TML_ATUO_PROTECT, MT_DIR_S2M, MT_PN_GT,  emtTrans_afn04f58,     "终端自动保电设置"},
    {CMD_AFN_A_F59_METER_LIMIT,      MT_DIR_S2M, MT_PN_GT,  emtTrans_afn04f59,     "电能表异常判别阈值设置"},
    {CMD_AFN_A_F60_HUMOR_LIMIT,      MT_DIR_S2M, MT_PN_GT,  emtTrans_afn04f60,     "谐波限值"},
    {CMD_AFN_A_F61_DC_SIMULA,        MT_DIR_S2M, MT_PN_GT,  emtTrans_afn04f61,     "直流模拟量接入参数"},

    // 组9  pn:任务号
    {CMD_AFN_A_F65_CFG_AUTO_1,       MT_DIR_S2M, MT_PN_TK,  emtTrans_afn04f65,     "定时上报1类数据任务设置"},
    {CMD_AFN_A_F66_CFG_AUTO_2,       MT_DIR_S2M, MT_PN_TK,  emtTrans_afn04f66,     "定时上报2类数据任务设置"},
    {CMD_AFN_A_F67_GOP_AUTO_1,       MT_DIR_S2M, MT_PN_TK,  emtTrans_afn04f67,     "定时上报1类数据任务启动/停止设置"},
    {CMD_AFN_A_F68_GOP_AUTO_2,       MT_DIR_S2M, MT_PN_TK,  emtTrans_afn04f68,     "定时上报2类数据任务启动/停止设置"},

    // 组10  pn:测量点号   
    {CMD_AFN_A_F73_CAPA_CFG,         MT_DIR_S2M, MT_PN_MP,  emtTrans_afn04f73,     "电容器参数"},
    {CMD_AFN_A_F74_CAPA_RUN_CFG,     MT_DIR_S2M, MT_PN_MP,  emtTrans_afn04f74,     "电容器投切运行参数"},
    {CMD_AFN_A_F75_CAPA_PROT_PARA,   MT_DIR_S2M, MT_PN_MP,  emtTrans_afn04f75,     "电容器保护参数"},
    {CMD_AFN_A_F76_CAPA_CTRL_WAY,    MT_DIR_S2M, MT_PN_MP,  emtTrans_afn04f76,     "电容器投切控制方式"},

    // 组11 pn:直流模拟量端口号   
    {CMD_AFN_A_F81_CD_CHANGE_RATE,   MT_DIR_S2M, MT_PN_DP,  emtTrans_afn04f81,     "直流模拟量变比"},
    {CMD_AFN_A_F82_CD_LIMIT,         MT_DIR_S2M, MT_PN_DP,  emtTrans_afn04f82,     "直流模拟量限值"},
    {CMD_AFN_A_F83_CD_FREEZE_PARA,   MT_DIR_S2M, MT_PN_DP,  emtTrans_afn04f83,     "直流模拟量冻结参数"}, 
    /** } 上行 **/ 

    /** { 下行 **/ 
    // 组1   pn:p0
    {CMD_AFN_A_F1_TML_UP_CFG,        MT_DIR_M2S, MT_PN_P0,  NULL,                  "终端上行通信口通信参数设置"},
    {CMD_AFN_A_F2_TML_WIRELESS_CFG,  MT_DIR_M2S, MT_PN_P0,  NULL,                  "终端上行通信口无线中继转发设置"},
    {CMD_AFN_A_F3_MST_IP_PORT,       MT_DIR_M2S, MT_PN_P0,  NULL,                  "主站IP地址和端口"},
    {CMD_AFN_A_F4_MST_PHONE_SMS,     MT_DIR_M2S, MT_PN_P0,  NULL,                  "主站电话号码和短信中心号码"},
    {CMD_AFN_A_F5_TML_UP_AUTH,       MT_DIR_M2S, MT_PN_P0,  NULL,                  "终端上行通信消息认证参数设置"},
    {CMD_AFN_A_F6_TEAM_ADDR,         MT_DIR_M2S, MT_PN_P0,  NULL,                  "集中器组地址设置"},
    {CMD_AFN_A_F7_TML_IP_PORT,       MT_DIR_M2S, MT_PN_P0,  NULL,                  "集中器IP地址和端口"},
    {CMD_AFN_A_F8_TML_UP_WAY,        MT_DIR_M2S, MT_PN_P0,  NULL,                  "集中器上行通信工作方式"},

    // 组2   pn:p0
    {CMD_AFN_A_F9_TML_EVENT_CFG,     MT_DIR_M2S, MT_PN_P0,  NULL,                  "终端事件记录配置设置"},
    {CMD_AFN_A_F10_TML_POWER_CFG,    MT_DIR_M2S, MT_PN_P0,  NULL,                  "终端电能表/交流采样装置配置参数"},
    {CMD_AFN_A_F11_TML_PULSE_CFG,    MT_DIR_M2S, MT_PN_P0,  NULL,                  "终端脉冲配置参数"},
    {CMD_AFN_A_F12_TML_STATE_INPUT,  MT_DIR_M2S, MT_PN_P0,  NULL,                  "终端/集中器状态量输入参数"},
    {CMD_AFN_A_F13_TML_SIMULA_CFG,   MT_DIR_M2S, MT_PN_P0,  NULL,                  "终端电压/电流模拟量配置参数"},
    {CMD_AFN_A_F14_TML_GRUP_TOTL,    MT_DIR_M2S, MT_PN_P0,  NULL,                  "终端总加组配置参数"},
    {CMD_AFN_A_F15_HAVE_DIFF_EVENT,  MT_DIR_M2S, MT_PN_P0,  NULL,                  "有功总电能量差动超限事件参数设置"},
    {CMD_AFN_A_F16_VPN_USER_PWD,     MT_DIR_M2S, MT_PN_P0,  NULL,                  "虚拟专网用户名、密码"},

    // 组3   pn:p0
    {CMD_AFN_A_F17_TML_SAFE_VALUE,   MT_DIR_M2S, MT_PN_P0,  NULL,                  "终端保安定值"},
    {CMD_AFN_A_F18_TML_PCTRL_PERD,   MT_DIR_M2S, MT_PN_P0,  NULL,                  "终端功控时段"},
    {CMD_AFN_A_F19_TML_PCTRL_FACTOR, MT_DIR_M2S, MT_PN_P0,  NULL,                  "终端时段功控定值浮动系数"},
    {CMD_AFN_A_F20_TML_MONTH_FACTOR, MT_DIR_M2S, MT_PN_P0,  NULL,                  "终端月电能量控定值浮动系数"},
    {CMD_AFN_A_F21_TML_POWER_FACTOR, MT_DIR_M2S, MT_PN_P0,  NULL,                  "终端电能量费率时段和费率数"},
    {CMD_AFN_A_F22_TML_POWER_RATE,   MT_DIR_M2S, MT_PN_P0,  NULL,                  "终端电能量费率"},
    {CMD_AFN_A_F23_TML_WARNING_CFG,  MT_DIR_M2S, MT_PN_P0,  NULL,                  "终端催告警参数"},

    // 组4   pn:测量点号
    {CMD_AFN_A_F25_MP_BASE_CFG,      MT_DIR_M2S, MT_PN_MP,  NULL,                  "测量点基本参数"},
    {CMD_AFN_A_F26_MP_LIMIT_CFG,     MT_DIR_M2S, MT_PN_MP,  NULL,                  "测量点限值参数"},
    {CMD_AFN_A_F27_MP_LOSS_CFG,      MT_DIR_M2S, MT_PN_MP,  NULL,                  "测量点铜损、铁损参数"},
    {CMD_AFN_A_F28_MP_PERIOD_FACTOR, MT_DIR_M2S, MT_PN_MP,  NULL,                  "测量点功率因数分段限值"},
    {CMD_AFN_A_F29_TML_METER_ID,     MT_DIR_M2S, MT_PN_MP,  NULL,                  "终端当地电能表显示号"},
    {CMD_AFN_A_F30_TML_AUTO_READ,    MT_DIR_M2S, MT_PN_MP,  NULL,                  "终端台区集中抄表停抄/投抄设置"},
    {CMD_AFN_A_F31_SLAVE_ADDR,       MT_DIR_M2S, MT_PN_MP,  NULL,                  "载波从节点附属节点地址"},

    // 组5   pn:p0
    {CMD_AFN_A_F33_TML_READ_CFG,     MT_DIR_M2S, MT_PN_P0,  NULL,                  "终端抄表运行参数设置"},
    {CMD_AFN_A_F34_CON_DOWN_CFG,     MT_DIR_M2S, MT_PN_P0,  NULL,                  "集中器下行通信模拟的参数设置"},
    {CMD_AFN_A_F35_TML_READ_VIP,     MT_DIR_M2S, MT_PN_P0,  NULL,                  "终端台区集中抄表重点户设置"},
    {CMD_AFN_A_F36_TML_UP_LIMIT,     MT_DIR_M2S, MT_PN_P0,  NULL,                  "终端上行通信流量门限设置"},
    {CMD_AFN_A_F37_TML_CASC_CFG,     MT_DIR_M2S, MT_PN_P0,  NULL,                  "终端级联通信参数"},
    {CMD_AFN_A_F38_CFG_ASK_1,        MT_DIR_M2S, MT_PN_P0,  NULL,                  "1类数据配置设置(在终端支持的1类数据配置内)"},
    {CMD_AFN_A_F39_CFG_ASK_2,        MT_DIR_M2S, MT_PN_P0,  NULL,                  "2类数据配置设置(在终端支持的2类数据配置内)"},

    // 组6   pn:总加组号
    {CMD_AFN_A_F41_PERIOD_VALUE,     MT_DIR_M2S, MT_PN_GT,  NULL,                  "时段功控定值"},
    {CMD_AFN_A_F42_FACTORY_HOLIDAY,  MT_DIR_M2S, MT_PN_GT,  NULL,                  "厂休功控参数"},
    {CMD_AFN_A_F43_SLIDE_TIME,       MT_DIR_M2S, MT_PN_GT,  NULL,                  "功率控制的功率计算滑差时间"},
    {CMD_AFN_A_F44_SHUTOUT_CFG,      MT_DIR_M2S, MT_PN_GT,  NULL,                  "营业报停控参数"},
    {CMD_AFN_A_F45_CTRL_TURN_CFG,    MT_DIR_M2S, MT_PN_GT,  NULL,                  "功控轮次设定"},
    {CMD_AFN_A_F46_MONTH_FIX_VALUE,  MT_DIR_M2S, MT_PN_GT,  NULL,                  "月电量控定值"},
    {CMD_AFN_A_F47_BUY_COUNT,        MT_DIR_M2S, MT_PN_GT,  NULL,                  "购电量(费)控参数"},
    {CMD_AFN_A_F48_ELEC_TURN_CFG,    MT_DIR_M2S, MT_PN_GT,  NULL,                  "电控轮次设定"},

    // 组7  pn:控制轮次
    {CMD_AFN_A_F49_WARNING_TIME,     MT_DIR_M2S, MT_PN_CT,  NULL,                  "功控告警时间"},

    // 组8  pn:总加组号
    {CMD_AFN_A_F57_TML_WARN_SOUND,   MT_DIR_M2S, MT_PN_GT,  NULL,                  "终端声音告警允许/禁止设置"},
    {CMD_AFN_A_F58_TML_ATUO_PROTECT, MT_DIR_M2S, MT_PN_GT,  NULL,                  "终端自动保电设置"},
    {CMD_AFN_A_F59_METER_LIMIT,      MT_DIR_M2S, MT_PN_GT,  NULL,                  "电能表异常判别阈值设置"},
    {CMD_AFN_A_F60_HUMOR_LIMIT,      MT_DIR_M2S, MT_PN_GT,  NULL,                  "谐波限值"},
    {CMD_AFN_A_F61_DC_SIMULA,        MT_DIR_M2S, MT_PN_GT,  NULL,                  "直流模拟量接入参数"},

    // 组9  pn:任务号
    {CMD_AFN_A_F65_CFG_AUTO_1,       MT_DIR_M2S, MT_PN_TK,  NULL,                  "定时上报1类数据任务设置"},
    {CMD_AFN_A_F66_CFG_AUTO_2,       MT_DIR_M2S, MT_PN_TK,  NULL,                  "定时上报2类数据任务设置"},
    {CMD_AFN_A_F67_GOP_AUTO_1,       MT_DIR_M2S, MT_PN_TK,  NULL,                  "定时上报1类数据任务启动/停止设置"},
    {CMD_AFN_A_F68_GOP_AUTO_2,       MT_DIR_M2S, MT_PN_TK,  NULL,                  "定时上报2类数据任务启动/停止设置"},

    // 组10  pn:测量点号   
    {CMD_AFN_A_F73_CAPA_CFG,         MT_DIR_M2S, MT_PN_MP,  NULL,                  "电容器参数"},
    {CMD_AFN_A_F74_CAPA_RUN_CFG,     MT_DIR_M2S, MT_PN_MP,  NULL,                  "电容器投切运行参数"},
    {CMD_AFN_A_F75_CAPA_PROT_PARA,   MT_DIR_M2S, MT_PN_MP,  NULL,                  "电容器保护参数"},
    {CMD_AFN_A_F76_CAPA_CTRL_WAY,    MT_DIR_M2S, MT_PN_MP,  NULL,                  "电容器投切控制方式"},

    // 组11 pn:直流模拟量端口号   
    {CMD_AFN_A_F81_CD_CHANGE_RATE,   MT_DIR_M2S, MT_PN_DP,  NULL,                  "直流模拟量变比"},
    {CMD_AFN_A_F82_CD_LIMIT,         MT_DIR_M2S, MT_PN_DP,  NULL,                  "直流模拟量限值"},
    {CMD_AFN_A_F83_CD_FREEZE_PARA,   MT_DIR_M2S, MT_PN_DP,  NULL,                  "直流模拟量冻结参数"}, 
    /** } 下行 **/ 
    ///*}

     /*******************************
     *  (10) 请求任务数据(AFN=0BH）
     *
     *  上行: 根据不同类型,分别用请求1类数据和请求2类数据上行报文进行应答
     *  下行: 本类型
     *  加密: 不需要  
     *  
    {*///
    {CMD_AFN_B_F1_TIMING_REPORT_1,   MT_DIR_M2S, MT_PN_TK,  NULL,                  "请求定时上报1类数据任务"},
    {CMD_AFN_B_F2_TIMING_REPORT_2,   MT_DIR_M2S, MT_PN_TK,  NULL,                  "请求定时上报2类数据任务"},
    ///*}


     /*******************************
     *  (11) 请求1类数据(AFN=0CH）
     *
     *  上行: 本类型
     *  下行: 本类型
     *  加密: 不需要
     *  
    {*///
    /** { 上行 **/ 
    // 组1   pn:p0   
    {CMD_AFN_C_F2_TML_CLOCK,         MT_DIR_S2M, MT_PN_P0,  emtTrans_afn0cf02,     "终端日历时钟"},
    {CMD_AFN_C_F3_TML_PARA_STATE,    MT_DIR_S2M, MT_PN_P0,  emtTrans_afn0cf03,     "终端参数状态"},
    {CMD_AFN_C_F4_TML_UPCOM_STATE,   MT_DIR_S2M, MT_PN_P0,  emtTrans_afn0cf04,     "终端上行通信状态"},
    {CMD_AFN_C_F5_TML_CTRL_SET,      MT_DIR_S2M, MT_PN_P0,  emtTrans_afn0cf05,     "终端控制设置状态"},
    {CMD_AFN_C_F6_TML_CTRL_STATE,    MT_DIR_S2M, MT_PN_P0,  emtTrans_afn0cf06,     "终端当前控制状态"},
    {CMD_AFN_C_F7_TML_EC_VALUE,      MT_DIR_S2M, MT_PN_P0,  emtTrans_afn0cf07,     "终端事件计数器当前值"},
    {CMD_AFN_C_F8_TML_EVNT_FLAG,     MT_DIR_S2M, MT_PN_P0,  emtTrans_afn0cf08,     "终端事件标志状态"}, 

    // 组2   pn:p0
    {CMD_AFN_C_F9_TML_STATE_BIT,     MT_DIR_S2M, MT_PN_P0,  emtTrans_afn0cf09,     "终端状态量及变位标志"},
    {CMD_AFN_C_F10_TML_MONTH_FLOW,   MT_DIR_S2M, MT_PN_P0,  emtTrans_afn0cf10,     "终端与主站当日、月通信流量"},
    {CMD_AFN_C_F11_TML_READ_METER,   MT_DIR_S2M, MT_PN_P0,  emtTrans_afn0cf11,     "终端集中抄表状态信息"},

    // 组3   pn:总加组号
    {CMD_AFN_C_F17_TOTAL_POWER_HAVE, MT_DIR_S2M, MT_PN_GT,  emtTrans_afn0cf17,     "当前总加有功功率"},
    {CMD_AFN_C_F18_TOTAL_POWER_NONE, MT_DIR_S2M, MT_PN_GT,  emtTrans_afn0cf18,     "当前总加无功功率"},
    {CMD_AFN_C_F19_DAY_POWER_HAVE,   MT_DIR_S2M, MT_PN_GT,  emtTrans_afn0cf19,     "当日总加有功电能量(总、费率 1~M)"},
    {CMD_AFN_C_F20_DAY_POWER_NONE,   MT_DIR_S2M, MT_PN_GT,  emtTrans_afn0cf20,     "当日总加无功电能量(总、费率 1~M)"},
    {CMD_AFN_C_F21_MONTH_POWER_HAVE, MT_DIR_S2M, MT_PN_GT,  emtTrans_afn0cf21,     "当月总加有功电能量(总、费率 1~M)"},
    {CMD_AFN_C_F22_MONTH_POWER_NONE, MT_DIR_S2M, MT_PN_GT,  emtTrans_afn0cf22,     "当月总加无功电能量(总、费率 1~M)"},
    {CMD_AFN_C_F23_TML_LEFT_POWER,   MT_DIR_S2M, MT_PN_GT,  emtTrans_afn0cf23,     "终端当前剩余电量(费)"},
    {CMD_AFN_C_F24_DOWN_FREZ_VALUE,  MT_DIR_S2M, MT_PN_GT,  emtTrans_afn0cf24,     "当前功率下浮控控后总加有功功率冻结值"},
           
    // 组4   pn:测量点号
    {CMD_AFN_C_F25_POWER_RATE_CUR,   MT_DIR_S2M, MT_PN_MP,  emtTrans_afn0cf25,     "当前三相及总有/无功功率、功率因数, 三相电压、电流、零序电流、视在功率"},
    {CMD_AFN_C_F26_DROP_PHASE_LAST,  MT_DIR_S2M, MT_PN_MP,  emtTrans_afn0cf26,     "A、B、C三相断相统计数据及最近一次断相记录"},
    {CMD_AFN_C_F27_METER_PROG_TIME,  MT_DIR_S2M, MT_PN_MP,  emtTrans_afn0cf27,     "电能表日历时钟、编程次数及其最近一次操作时间"},
    {CMD_AFN_C_F28_METER_WORD_CBIT,  MT_DIR_S2M, MT_PN_MP,  emtTrans_afn0cf28,     "电表运行状态字及其变位标志"},
    {CMD_AFN_C_F29_COPER_IRON_CUR,   MT_DIR_S2M, MT_PN_MP,  emtTrans_afn0cf29,     "当前铜损、铁损有功总电能示值"},
    {CMD_AFN_C_F30_COPER_IRON_LAST,  MT_DIR_S2M, MT_PN_MP,  emtTrans_afn0cf30,     "上一结算日当前铜损、铁损有功总电能示值"},
    {CMD_AFN_C_F31_POWER_CUR,        MT_DIR_S2M, MT_PN_MP,  emtTrans_afn0cf31,     "当前A、B、C三相正/反向有功电能示值、组合无功1/2电能示值"},
    {CMD_AFN_C_F32_POWER_LAST,       MT_DIR_S2M, MT_PN_MP,  emtTrans_afn0cf32,     "上一结算日A、B、C三相正/反向有功电能示值、组合无功1/2电能示值"},

    // 组5   pn:测量点号
    {CMD_AFN_C_F33_FRTH_POWR_P1P4_C, MT_DIR_S2M, MT_PN_MP,  emtTrans_afn0cf33,     "当前正向有/无功（组合无功1）电能示值、一/四象限无功电能示值（总、费率1～M，1≤M≤12）"},
    {CMD_AFN_C_F34_BACK_POWR_P2P3_C, MT_DIR_S2M, MT_PN_MP,  emtTrans_afn0cf34,     "当前反向有/无功（组合无功2）电能示值、二/三象限无功电能示值（总、费率1～M，1≤M≤12）"},
    {CMD_AFN_C_F35_FRTH_DMND_M,      MT_DIR_S2M, MT_PN_MP,  emtTrans_afn0cf35,     "当月正向有/无功最大需量及发生时间（总、费率1～M，1≤M≤12）"},
    {CMD_AFN_C_F36_BACK_DMND_M,      MT_DIR_S2M, MT_PN_MP,  emtTrans_afn0cf36,     "当月反向有/无功最大需量及发生时间（总、费率1～M，1≤M≤12）"},
    {CMD_AFN_C_F37_FRTH_POWR_P1P4_L, MT_DIR_S2M, MT_PN_MP,  emtTrans_afn0cf37,     "上月（上一结算日）正向有/无功（组合无功1）电能示值、一/四象限无功电能示值（总、费率1～M，1≤M≤12）"},
    {CMD_AFN_C_F38_BACK_POWR_P2P3_L, MT_DIR_S2M, MT_PN_MP,  emtTrans_afn0cf38,     "上月（上一结算日）反向有/无功（组合无功2）电能示值、二/三象限无功电能示值（总、费率1～M，1≤M≤12）"},
    {CMD_AFN_C_F39_FRTH_DMND_L,      MT_DIR_S2M, MT_PN_MP,  emtTrans_afn0cf39,     "上月（上一结算日）正向有/无功最大需量及发生时间（总、费率1～M，1≤M≤12）"},
    {CMD_AFN_C_F40_BACK_DMND_L,      MT_DIR_S2M, MT_PN_MP,  emtTrans_afn0cf40,     "上月（上一结算日）反向有/无功最大需量及发生时间（总、费率1～M，1≤M≤12"},

    // 组6  pn:测量点号
    {CMD_AFN_C_F41_FRTH_HAVE_POWR_D, MT_DIR_S2M, MT_PN_MP,  emtTrans_afn0cf41,     "当日正向有功电能量（总、费率1～M）"},
    {CMD_AFN_C_F42_FRTH_NONE_POWR_D, MT_DIR_S2M, MT_PN_MP,  emtTrans_afn0cf42,     "当日正向无功电能量（总、费率1～M）"},
    {CMD_AFN_C_F43_BACK_HAVE_POWR_D, MT_DIR_S2M, MT_PN_MP,  emtTrans_afn0cf43,     "当日反向有功电能量（总、费率1～M）"},
    {CMD_AFN_C_F44_BACK_NONE_POWR_D, MT_DIR_S2M, MT_PN_MP,  emtTrans_afn0cf44,     "当日反向无功电能量（总、费率1～M）"},
    {CMD_AFN_C_F45_FRTH_HAVE_POWR_M, MT_DIR_S2M, MT_PN_MP,  emtTrans_afn0cf45,     "当月正向有功电能量（总、费率1～M）"},
    {CMD_AFN_C_F46_FRTH_NONE_POWR_M, MT_DIR_S2M, MT_PN_MP,  emtTrans_afn0cf46,     "当月正向无功电能量（总、费率1～M）"},
    {CMD_AFN_C_F47_BACK_HAVE_POWR_M, MT_DIR_S2M, MT_PN_MP,  emtTrans_afn0cf47,     "当月反向有功电能量（总、费率1～M）"},
    {CMD_AFN_C_F48_BACK_NONE_POWR_M, MT_DIR_S2M, MT_PN_MP,  emtTrans_afn0cf48,     "当月反向无功电能量（总、费率1～M）"},
               
    // 组7  pn:测量点号
    {CMD_AFN_C_F49_CURT_PHASE_ANGLE, MT_DIR_S2M, MT_PN_MP,  emtTrans_afn0cf49,     "当前电压、电流相位角"},

    // 组8  pn:测量点号
    {CMD_AFN_C_F57_CURT_HARM_VALUE,  MT_DIR_S2M, MT_PN_MP,  emtTrans_afn0cf57,     "当前A、B、C三相电压、电流2～N次谐波有效值"},
    {CMD_AFN_C_F58_CURT_HARM_RATE,   MT_DIR_S2M, MT_PN_MP,  emtTrans_afn0cf58,     "当前A、B、C三相电压、电流2～N次谐波含有率"},

    // 组9  pn:测量点号
    {CMD_AFN_C_F65_CURT_CAPA_SWITCH, MT_DIR_S2M, MT_PN_MP,  emtTrans_afn0cf65,     "当前电容器投切状态"},
    {CMD_AFN_C_F66_CURT_CAPA_TIME,   MT_DIR_S2M, MT_PN_MP,  emtTrans_afn0cf66,     "当前电容器累计补偿投入时间和次数"},
    {CMD_AFN_C_F67_CURT_CAPA_POWR,   MT_DIR_S2M, MT_PN_MP,  emtTrans_afn0cf67,     "当日、当月电容器累计补偿的无功电能量"},

    // 组10  pn:直流模拟量端口号   
    {CMD_AFN_C_F73_DC_RLTM_DATA,     MT_DIR_S2M, MT_PN_MP,  emtTrans_afn0cf73,     "直流模拟量实时数据"}, 

    // 组11 pn:总加组号      
    {CMD_AFN_C_F81_HFRZ_GRUP_RATE_HAVE, MT_DIR_S2M, MT_PN_GT, emtTrans_afn0cf81,   "小时冻结总加有功功率"},
    {CMD_AFN_C_F82_HFRZ_GRUP_RATE_NONE, MT_DIR_S2M, MT_PN_GT, emtTrans_afn0cf82,   "小时冻结总加无功功率"},
    {CMD_AFN_C_F83_HFRZ_GRUP_POWR_HAVE, MT_DIR_S2M, MT_PN_GT, emtTrans_afn0cf83,   "小时冻结总加有功总电能量"},
    {CMD_AFN_C_F84_HFRZ_GRUP_POWR_NONE, MT_DIR_S2M, MT_PN_GT, emtTrans_afn0cf84,   "小时冻结总加无功总电能量"},

    // 组12 pn:测量点号
    {CMD_AFN_C_F89_HFRZ_RATE_HAVE,   MT_DIR_S2M, MT_PN_MP,    emtTrans_afn0cf89,   "小时冻结有功功率"},
    {CMD_AFN_C_F90_HFRZ_RATE_HAVE_A, MT_DIR_S2M, MT_PN_MP,    emtTrans_afn0cf90,   "小时冻结A相有功功率"},
    {CMD_AFN_C_F91_HFRZ_RATE_HAVE_B, MT_DIR_S2M, MT_PN_MP,    emtTrans_afn0cf91,   "小时冻结B相有功功率"},
    {CMD_AFN_C_F92_HFRZ_RATE_HAVE_C, MT_DIR_S2M, MT_PN_MP,    emtTrans_afn0cf92,   "小时冻结C相有功功率"},
    {CMD_AFN_C_F93_HFRZ_RATE_NONE,   MT_DIR_S2M, MT_PN_MP,    emtTrans_afn0cf93,   "小时冻结无功功率"},
    {CMD_AFN_C_F94_HFRZ_RATE_NONE_A, MT_DIR_S2M, MT_PN_MP,    emtTrans_afn0cf94,   "小时冻结A相无功功率"},
    {CMD_AFN_C_F95_HFRZ_RATE_NONE_B, MT_DIR_S2M, MT_PN_MP,    emtTrans_afn0cf95,   "小时冻结B相无功功率"},
    {CMD_AFN_C_F96_HFRZ_RATE_NONE_C, MT_DIR_S2M, MT_PN_MP,    emtTrans_afn0cf96,   "小时冻结C相无功功率"},

    // 组13 pn:测量点号
    {CMD_AFN_C_F97_HFRZ_VOLT_A,      MT_DIR_S2M, MT_PN_MP,    emtTrans_afn0cf97,   "小时冻结A相电压"},
    {CMD_AFN_C_F98_HFRZ_VOLT_B,      MT_DIR_S2M, MT_PN_MP,    emtTrans_afn0cf98,   "小时冻结B相电压"},
    {CMD_AFN_C_F99_HFRZ_VOLT_C,      MT_DIR_S2M, MT_PN_MP,    emtTrans_afn0cf99,   "小时冻结C相电压"},
    {CMD_AFN_C_F100_HFRZ_ELEC_A,     MT_DIR_S2M, MT_PN_MP,    emtTrans_afn0cf100,  "小时冻结A相电流"},
    {CMD_AFN_C_F101_HFRZ_ELEC_B,     MT_DIR_S2M, MT_PN_MP,    emtTrans_afn0cf101,  "小时冻结B相电流"},
    {CMD_AFN_C_F102_HFRZ_ELEC_C,     MT_DIR_S2M, MT_PN_MP,    emtTrans_afn0cf102,  "小时冻结C相电流"},
    {CMD_AFN_C_F103_HFRZ_ELEC_ZERO,  MT_DIR_S2M, MT_PN_MP,    emtTrans_afn0cf103,  "小时冻结零序电流"},

    // 组14 pn:测量点号
    {CMD_AFN_C_F105_HFRZ_FRTH_HAVE,  MT_DIR_S2M, MT_PN_MP,    emtTrans_afn0cf105,  "小时冻结正向有功总电能量"},
    {CMD_AFN_C_F106_HFRZ_FRTH_NONE,  MT_DIR_S2M, MT_PN_MP,    emtTrans_afn0cf106,  "小时冻结正向无功总电能量"},
    {CMD_AFN_C_F107_HFRZ_BACK_HAVE,  MT_DIR_S2M, MT_PN_MP,    emtTrans_afn0cf107,  "小时冻结反向有功总电能量"},
    {CMD_AFN_C_F108_HFRZ_BACK_NONE,  MT_DIR_S2M, MT_PN_MP,    emtTrans_afn0cf108,  "小时冻结反向无功总电能量"},
    {CMD_AFN_C_F109_HFRZ_FRTH_HAVE_S,MT_DIR_S2M, MT_PN_MP,    emtTrans_afn0cf109,  "小时冻结正向有功总电能示值"},
    {CMD_AFN_C_F110_HFRZ_FRTH_NONE_S,MT_DIR_S2M, MT_PN_MP,    emtTrans_afn0cf110,  "小时冻结正向无功总电能示值"},
    {CMD_AFN_C_F111_HFRZ_BACK_HAVE_S,MT_DIR_S2M, MT_PN_MP,    emtTrans_afn0cf111,  "小时冻结反向有功总电能示值"},
    {CMD_AFN_C_F112_HFRZ_BACK_NONE_S,MT_DIR_S2M, MT_PN_MP,    emtTrans_afn0cf112,  "小时冻结反向无功总电能示值"}, 

    // 组15 pn:测量点号
    {CMD_AFN_C_F113_HFRZ_FACT_TOTAL, MT_DIR_S2M, MT_PN_MP,    emtTrans_afn0cf113,  "小时冻结总功率因数"},
    {CMD_AFN_C_F114_HFRZ_FACT_A,     MT_DIR_S2M, MT_PN_MP,    emtTrans_afn0cf114,  "小时冻结A相功率因数"},
    {CMD_AFN_C_F115_HFRZ_FACT_B,     MT_DIR_S2M, MT_PN_MP,    emtTrans_afn0cf115,  "小时冻结B相功率因数"},
    {CMD_AFN_C_F116_HFRZ_FACT_C,     MT_DIR_S2M, MT_PN_MP,    emtTrans_afn0cf116,  "小时冻结C相功率因数"},

    // 组16 pn:测量点号
    {CMD_AFN_C_F121_HFRZ_DC_VALUE,   MT_DIR_S2M, MT_PN_MP,    emtTrans_afn0cf121,  "小时冻结直流模拟量"},

    // 组17 pn:测量点号 C表示当前current
    {CMD_AFN_C_F129_FRTH_HAVE_POWR_C, MT_DIR_S2M, MT_PN_MP,   emtTrans_afn0cf129,  "当前正向有功电能示值（总、费率1～M）"},
    {CMD_AFN_C_F130_FRTH_NONE_POWR_C, MT_DIR_S2M, MT_PN_MP,   emtTrans_afn0cf130,  "当前正向无功（组合无功1）电能示值（总、费率1～M）"},
    {CMD_AFN_C_F131_BACK_HAVE_POWR_C, MT_DIR_S2M, MT_PN_MP,   emtTrans_afn0cf131,  "当前反向有功电能示值（总、费率1～M）"},
    {CMD_AFN_C_F132_BACK_NONE_POWR_C, MT_DIR_S2M, MT_PN_MP,   emtTrans_afn0cf132,  "当前反向无功（组合无功1）电能示值（总、费率1～M）"},
    {CMD_AFN_C_F133_NONE_POWR_P1_C,   MT_DIR_S2M, MT_PN_MP,   emtTrans_afn0cf133,  "当前一象限无功电能示值（总、费率1～M）"},
    {CMD_AFN_C_F134_NONE_POWR_P2_C,   MT_DIR_S2M, MT_PN_MP,   emtTrans_afn0cf134,  "当前二象限无功电能示值（总、费率1～M）"},
    {CMD_AFN_C_F135_NONE_POWR_P3_C,   MT_DIR_S2M, MT_PN_MP,   emtTrans_afn0cf135,  "当前三象限无功电能示值（总、费率1～M）"},
    {CMD_AFN_C_F136_NONE_POWR_P4_C,   MT_DIR_S2M, MT_PN_MP,   emtTrans_afn0cf136,  "当前三象限无功电能示值（总、费率1～M）"},

    // 组18 pn:测量点号 L表示上一结算日或上月 Last Month 
    {CMD_AFN_C_F137_FRTH_HAVE_POWR_L, MT_DIR_S2M, MT_PN_MP,   emtTrans_afn0cf137,  "上月（上一结算日）正向有功电能示值（总、费率1～M）"},
    {CMD_AFN_C_F138_FRTH_NONE_POWR_L, MT_DIR_S2M, MT_PN_MP,   emtTrans_afn0cf138,  "上月（上一结算日）正向无功（组合无功1）电能示值（总、费率1～M）"},
    {CMD_AFN_C_F139_BACK_HAVE_POWR_L, MT_DIR_S2M, MT_PN_MP,   emtTrans_afn0cf139,  "上月（上一结算日）反向有功电能示值（总、费率1～M）"},
    {CMD_AFN_C_F140_BACK_NONE_POWR_L, MT_DIR_S2M, MT_PN_MP,   emtTrans_afn0cf140,  "上月（上一结算日）反向无功（组合无功1）电能示值（总、费率1～M）"},
    {CMD_AFN_C_F141_NONE_POWR_P1_L,   MT_DIR_S2M, MT_PN_MP,   emtTrans_afn0cf141,  "上月（上一结算日）一象限无功电能示值（总、费率1～M）"},
    {CMD_AFN_C_F142_NONE_POWR_P2_L,   MT_DIR_S2M, MT_PN_MP,   emtTrans_afn0cf142,  "上月（上一结算日）二象限无功电能示值（总、费率1～M）"},
    {CMD_AFN_C_F143_NONE_POWR_P3_L,   MT_DIR_S2M, MT_PN_MP,   emtTrans_afn0cf143,  "上月（上一结算日）三象限无功电能示值（总、费率1～M）"},
    {CMD_AFN_C_F144_NONE_POWR_P4_L,   MT_DIR_S2M, MT_PN_MP,   emtTrans_afn0cf144,  "上月（上一结算日）四象限无功电能示值（总、费率1～M）"},

    // 组19 pn:测量点号
    {CMD_AFN_C_F145_FRTH_HAVE_DMND_C, MT_DIR_S2M, MT_PN_MP,   emtTrans_afn0cf145,  "当月正向有功最大需量及发生时间（总、费率1～M）"},
    {CMD_AFN_C_F146_FRTH_NONE_DMND_C, MT_DIR_S2M, MT_PN_MP,   emtTrans_afn0cf146,  "当月正向无功最大需量及发生时间（总、费率1～M）"},
    {CMD_AFN_C_F147_BACK_HAVE_DMND_C, MT_DIR_S2M, MT_PN_MP,   emtTrans_afn0cf147,  "当月反向有功最大需量及发生时间（总、费率1～M）"},
    {CMD_AFN_C_F148_BACK_NONE_DMND_C, MT_DIR_S2M, MT_PN_MP,   emtTrans_afn0cf148,  "当月反向无功最大需量及发生时间（总、费率1～M）"},
    {CMD_AFN_C_F149_FRTH_HAVE_DMND_L, MT_DIR_S2M, MT_PN_MP,   emtTrans_afn0cf149,  "上月（上一结算日）正向有功最大需量及发生时间（总、费率1～M）"},
    {CMD_AFN_C_F150_FRTH_NONE_DMND_L, MT_DIR_S2M, MT_PN_MP,   emtTrans_afn0cf150,  "上月（上一结算日）正向无功最大需量及发生时间（总、费率1～M）"},
    {CMD_AFN_C_F151_BACK_HAVE_DMND_L, MT_DIR_S2M, MT_PN_MP,   emtTrans_afn0cf151,  "上月（上一结算日）反向有功最大需量及发生时间（总、费率1～M）"},
    {CMD_AFN_C_F152_BACK_NONE_DMND_L, MT_DIR_S2M, MT_PN_MP,   emtTrans_afn0cf152,  "上月（上一结算日）反向无功最大需量及发生时间（总、费率1～M）"},

    // 组20 pn:测量点号
    {CMD_AFN_C_F153_FREZ_ZONE_1,      MT_DIR_S2M, MT_PN_MP,   emtTrans_afn0cf153,  "第一时区冻结正向有功电能示值（总、费率1～M）"},
    {CMD_AFN_C_F154_FREZ_ZONE_2,      MT_DIR_S2M, MT_PN_MP,   emtTrans_afn0cf154,  "第二时区冻结正向有功电能示值（总、费率1～M）"},
    {CMD_AFN_C_F155_FREZ_ZONE_3,      MT_DIR_S2M, MT_PN_MP,   emtTrans_afn0cf155,  "第三时区冻结正向有功电能示值（总、费率1～M）"},
    {CMD_AFN_C_F156_FREZ_ZONE_4,      MT_DIR_S2M, MT_PN_MP,   emtTrans_afn0cf156,  "第四时区冻结正向有功电能示值（总、费率1～M）"},
    {CMD_AFN_C_F157_FREZ_ZONE_5,      MT_DIR_S2M, MT_PN_MP,   emtTrans_afn0cf157,  "第五时区冻结正向有功电能示值（总、费率1～M）"},
    {CMD_AFN_C_F158_FREZ_ZONE_6,      MT_DIR_S2M, MT_PN_MP,   emtTrans_afn0cf158,  "第六时区冻结正向有功电能示值（总、费率1～M）"},
    {CMD_AFN_C_F159_FREZ_ZONE_7,      MT_DIR_S2M, MT_PN_MP,   emtTrans_afn0cf159,  "第七时区冻结正向有功电能示值（总、费率1～M）"},
    {CMD_AFN_C_F160_FREZ_ZONE_8,      MT_DIR_S2M, MT_PN_MP,   emtTrans_afn0cf160,  "第八时区冻结正向有功电能示值（总、费率1～M）"},

    // 组21 pn:测量点号
    {CMD_AFN_C_F161_METR_REMOTE_CTRL, MT_DIR_S2M, MT_PN_MP,   emtTrans_afn0cf161,  "电能表远程控制通断电状态及记录"},
    {CMD_AFN_C_F165_METR_SWITCH_RECD, MT_DIR_S2M, MT_PN_MP,   emtTrans_afn0cf165,  "电能表开关操作次数及时间"},
    {CMD_AFN_C_F166_METR_MODIFY_RECD, MT_DIR_S2M, MT_PN_MP,   emtTrans_afn0cf166,  "电能表参数修改次数及时间"},
    {CMD_AFN_C_F167_METR_BUY_USE,     MT_DIR_S2M, MT_PN_MP,   emtTrans_afn0cf167,  "电能表购、用电信息"},
    {CMD_AFN_C_F168_METR_BALANCE,     MT_DIR_S2M, MT_PN_MP,   emtTrans_afn0cf168,  "电能表结算信息"},

    // 组22 pn:测量点号
    {CMD_AFN_C_F169_READ_ROUTE,       MT_DIR_S2M, MT_PN_MP,   emtTrans_afn0cf169,  "集中抄表中继路由信息"},
    {CMD_AFN_C_F170_READ_METER,       MT_DIR_S2M, MT_PN_MP,   emtTrans_afn0cf170,  "集中抄表电表抄读信息"},
    /** } 上行 **/ 

    /** { 下行 **/ 
    // 组1   pn:p0   
    {CMD_AFN_C_F2_TML_CLOCK,         MT_DIR_M2S, MT_PN_P0,  NULL,                  "终端日历时钟"},
    {CMD_AFN_C_F3_TML_PARA_STATE,    MT_DIR_M2S, MT_PN_P0,  NULL,                  "终端参数状态"},
    {CMD_AFN_C_F4_TML_UPCOM_STATE,   MT_DIR_M2S, MT_PN_P0,  NULL,                  "终端上行通信状态"},
    {CMD_AFN_C_F5_TML_CTRL_SET,      MT_DIR_M2S, MT_PN_P0,  NULL,                  "终端控制设置状态"},
    {CMD_AFN_C_F6_TML_CTRL_STATE,    MT_DIR_M2S, MT_PN_P0,  NULL,                  "终端当前控制状态"},
    {CMD_AFN_C_F7_TML_EC_VALUE,      MT_DIR_M2S, MT_PN_P0,  NULL,                  "终端事件计数器当前值"},
    {CMD_AFN_C_F8_TML_EVNT_FLAG,     MT_DIR_M2S, MT_PN_P0,  NULL,                  "终端事件标志状态"}, 

    // 组2   pn:p0
    {CMD_AFN_C_F9_TML_STATE_BIT,     MT_DIR_M2S, MT_PN_P0,  NULL,                  "终端状态量及变位标志"},
    {CMD_AFN_C_F10_TML_MONTH_FLOW,   MT_DIR_M2S, MT_PN_P0,  NULL,                  "终端与主站当日、月通信流量"},
    {CMD_AFN_C_F11_TML_READ_METER,   MT_DIR_M2S, MT_PN_P0,  NULL,                  "终端集中抄表状态信息"},

    // 组3   pn:总加组号
    {CMD_AFN_C_F17_TOTAL_POWER_HAVE, MT_DIR_M2S, MT_PN_GT,  NULL,                  "当前总加有功功率"},
    {CMD_AFN_C_F18_TOTAL_POWER_NONE, MT_DIR_M2S, MT_PN_GT,  NULL,                  "当前总加无功功率"},
    {CMD_AFN_C_F19_DAY_POWER_HAVE,   MT_DIR_M2S, MT_PN_GT,  NULL,                  "当日总加有功电能量(总、费率 1~M)"},
    {CMD_AFN_C_F20_DAY_POWER_NONE,   MT_DIR_M2S, MT_PN_GT,  NULL,                  "当日总加无功电能量(总、费率 1~M)"},
    {CMD_AFN_C_F21_MONTH_POWER_HAVE, MT_DIR_M2S, MT_PN_GT,  NULL,                  "当月总加有功电能量(总、费率 1~M)"},
    {CMD_AFN_C_F22_MONTH_POWER_NONE, MT_DIR_M2S, MT_PN_GT,  NULL,                  "当月总加无功电能量(总、费率 1~M)"},
    {CMD_AFN_C_F23_TML_LEFT_POWER,   MT_DIR_M2S, MT_PN_GT,  NULL,                  "终端当前剩余电量(费)"},
    {CMD_AFN_C_F24_DOWN_FREZ_VALUE,  MT_DIR_M2S, MT_PN_GT,  NULL,                  "当前功率下浮控控后总加有功功率冻结值"},
           
    // 组4   pn:测量点号
    {CMD_AFN_C_F25_POWER_RATE_CUR,   MT_DIR_M2S, MT_PN_MP,  NULL,                  "当前三相及总有/无功功率、功率因数, 三相电压、电流、零序电流、视在功率"},
    {CMD_AFN_C_F26_DROP_PHASE_LAST,  MT_DIR_M2S, MT_PN_MP,  NULL,                  "A、B、C三相断相统计数据及最近一次断相记录"},
    {CMD_AFN_C_F27_METER_PROG_TIME,  MT_DIR_M2S, MT_PN_MP,  NULL,                  "电能表日历时钟、编程次数及其最近一次操作时间"},
    {CMD_AFN_C_F28_METER_WORD_CBIT,  MT_DIR_M2S, MT_PN_MP,  NULL,                  "电表运行状态字及其变位标志"},
    {CMD_AFN_C_F29_COPER_IRON_CUR,   MT_DIR_M2S, MT_PN_MP,  NULL,                  "当前铜损、铁损有功总电能示值"},
    {CMD_AFN_C_F30_COPER_IRON_LAST,  MT_DIR_M2S, MT_PN_MP,  NULL,                  "上一结算日当前铜损、铁损有功总电能示值"},
    {CMD_AFN_C_F31_POWER_CUR,        MT_DIR_M2S, MT_PN_MP,  NULL,                  "当前A、B、C三相正/反向有功电能示值、组合无功1/2电能示值"},
    {CMD_AFN_C_F32_POWER_LAST,       MT_DIR_M2S, MT_PN_MP,  NULL,                  "上一结算日A、B、C三相正/反向有功电能示值、组合无功1/2电能示值"},

    // 组5   pn:测量点号
    {CMD_AFN_C_F33_FRTH_POWR_P1P4_C, MT_DIR_M2S, MT_PN_MP,  NULL,                  "当前正向有/无功（组合无功1）电能示值、一/四象限无功电能示值（总、费率1～M，1≤M≤12）"},
    {CMD_AFN_C_F34_BACK_POWR_P2P3_C, MT_DIR_M2S, MT_PN_MP,  NULL,                  "当前反向有/无功（组合无功2）电能示值、二/三象限无功电能示值（总、费率1～M，1≤M≤12）"},
    {CMD_AFN_C_F35_FRTH_DMND_M,      MT_DIR_M2S, MT_PN_MP,  NULL,                  "当月正向有/无功最大需量及发生时间（总、费率1～M，1≤M≤12）"},
    {CMD_AFN_C_F36_BACK_DMND_M,      MT_DIR_M2S, MT_PN_MP,  NULL,                  "当月反向有/无功最大需量及发生时间（总、费率1～M，1≤M≤12）"},
    {CMD_AFN_C_F37_FRTH_POWR_P1P4_L, MT_DIR_M2S, MT_PN_MP,  NULL,                  "上月（上一结算日）正向有/无功（组合无功1）电能示值、一/四象限无功电能示值（总、费率1～M，1≤M≤12）"},
    {CMD_AFN_C_F38_BACK_POWR_P2P3_L, MT_DIR_M2S, MT_PN_MP,  NULL,                  "上月（上一结算日）反向有/无功（组合无功2）电能示值、二/三象限无功电能示值（总、费率1～M，1≤M≤12）"},
    {CMD_AFN_C_F39_FRTH_DMND_L,      MT_DIR_M2S, MT_PN_MP,  NULL,                  "上月（上一结算日）正向有/无功最大需量及发生时间（总、费率1～M，1≤M≤12）"},
    {CMD_AFN_C_F40_BACK_DMND_L,      MT_DIR_M2S, MT_PN_MP,  NULL,                  "上月（上一结算日）反向有/无功最大需量及发生时间（总、费率1～M，1≤M≤12"},

    // 组6  pn:测量点号
    {CMD_AFN_C_F41_FRTH_HAVE_POWR_D, MT_DIR_M2S, MT_PN_MP,  NULL,                  "当日正向有功电能量（总、费率1～M）"},
    {CMD_AFN_C_F42_FRTH_NONE_POWR_D, MT_DIR_M2S, MT_PN_MP,  NULL,                  "当日正向无功电能量（总、费率1～M）"},
    {CMD_AFN_C_F43_BACK_HAVE_POWR_D, MT_DIR_M2S, MT_PN_MP,  NULL,                  "当日反向有功电能量（总、费率1～M）"},
    {CMD_AFN_C_F44_BACK_NONE_POWR_D, MT_DIR_M2S, MT_PN_MP,  NULL,                  "当日反向无功电能量（总、费率1～M）"},
    {CMD_AFN_C_F45_FRTH_HAVE_POWR_M, MT_DIR_M2S, MT_PN_MP,  NULL,                  "当月正向有功电能量（总、费率1～M）"},
    {CMD_AFN_C_F46_FRTH_NONE_POWR_M, MT_DIR_M2S, MT_PN_MP,  NULL,                  "当月正向无功电能量（总、费率1～M）"},
    {CMD_AFN_C_F47_BACK_HAVE_POWR_M, MT_DIR_M2S, MT_PN_MP,  NULL,                  "当月反向有功电能量（总、费率1～M）"},
    {CMD_AFN_C_F48_BACK_NONE_POWR_M, MT_DIR_M2S, MT_PN_MP,  NULL,                  "当月反向无功电能量（总、费率1～M）"},
               
    // 组7  pn:测量点号
    {CMD_AFN_C_F49_CURT_PHASE_ANGLE, MT_DIR_M2S, MT_PN_MP,  NULL,                  "当前电压、电流相位角"},

    // 组8  pn:测量点号
    {CMD_AFN_C_F57_CURT_HARM_VALUE,  MT_DIR_M2S, MT_PN_MP,  NULL,                  "当前A、B、C三相电压、电流2～N次谐波有效值"},
    {CMD_AFN_C_F58_CURT_HARM_RATE,   MT_DIR_M2S, MT_PN_MP,  NULL,                  "当前A、B、C三相电压、电流2～N次谐波含有率"},

    // 组9  pn:测量点号
    {CMD_AFN_C_F65_CURT_CAPA_SWITCH, MT_DIR_M2S, MT_PN_MP,  NULL,                  "当前电容器投切状态"},
    {CMD_AFN_C_F66_CURT_CAPA_TIME,   MT_DIR_M2S, MT_PN_MP,  NULL,                  "当前电容器累计补偿投入时间和次数"},
    {CMD_AFN_C_F67_CURT_CAPA_POWR,   MT_DIR_M2S, MT_PN_MP,  NULL,                  "当日、当月电容器累计补偿的无功电能量"},

    // 组10  pn:直流模拟量端口号   
    {CMD_AFN_C_F73_DC_RLTM_DATA,     MT_DIR_M2S, MT_PN_MP,  NULL,                  "直流模拟量实时数据"}, 

    // 组11 pn:总加组号      
    {CMD_AFN_C_F81_HFRZ_GRUP_RATE_HAVE, MT_DIR_M2S, MT_PN_GT,  NULL,                  "小时冻结总加有功功率"},
    {CMD_AFN_C_F82_HFRZ_GRUP_RATE_NONE, MT_DIR_M2S, MT_PN_GT,  NULL,                  "小时冻结总加无功功率"},
    {CMD_AFN_C_F83_HFRZ_GRUP_POWR_HAVE, MT_DIR_M2S, MT_PN_GT,  NULL,                  "小时冻结总加有功总电能量"},
    {CMD_AFN_C_F84_HFRZ_GRUP_POWR_NONE, MT_DIR_M2S, MT_PN_GT,  NULL,                  "小时冻结总加无功总电能量"},

    // 组12 pn:测量点号
    {CMD_AFN_C_F89_HFRZ_RATE_HAVE,   MT_DIR_M2S, MT_PN_MP,  NULL,                  "小时冻结有功功率"},
    {CMD_AFN_C_F90_HFRZ_RATE_HAVE_A, MT_DIR_M2S, MT_PN_MP,  NULL,                  "小时冻结A相有功功率"},
    {CMD_AFN_C_F91_HFRZ_RATE_HAVE_B, MT_DIR_M2S, MT_PN_MP,  NULL,                  "小时冻结B相有功功率"},
    {CMD_AFN_C_F92_HFRZ_RATE_HAVE_C, MT_DIR_M2S, MT_PN_MP,  NULL,                  "小时冻结C相有功功率"},
    {CMD_AFN_C_F93_HFRZ_RATE_NONE,   MT_DIR_M2S, MT_PN_MP,  NULL,                  "小时冻结无功功率"},
    {CMD_AFN_C_F94_HFRZ_RATE_NONE_A, MT_DIR_M2S, MT_PN_MP,  NULL,                  "小时冻结A相无功功率"},
    {CMD_AFN_C_F95_HFRZ_RATE_NONE_B, MT_DIR_M2S, MT_PN_MP,  NULL,                  "小时冻结B相无功功率"},
    {CMD_AFN_C_F96_HFRZ_RATE_NONE_C, MT_DIR_M2S, MT_PN_MP,  NULL,                  "小时冻结C相无功功率"},

    // 组13 pn:测量点号
    {CMD_AFN_C_F97_HFRZ_VOLT_A,      MT_DIR_M2S, MT_PN_MP,  NULL,                  "小时冻结A相电压"},
    {CMD_AFN_C_F98_HFRZ_VOLT_B,      MT_DIR_M2S, MT_PN_MP,  NULL,                  "小时冻结B相电压"},
    {CMD_AFN_C_F99_HFRZ_VOLT_C,      MT_DIR_M2S, MT_PN_MP,  NULL,                  "小时冻结C相电压"},
    {CMD_AFN_C_F100_HFRZ_ELEC_A,     MT_DIR_M2S, MT_PN_MP,  NULL,                  "小时冻结A相电流"},
    {CMD_AFN_C_F101_HFRZ_ELEC_B,     MT_DIR_M2S, MT_PN_MP,  NULL,                  "小时冻结B相电流"},
    {CMD_AFN_C_F102_HFRZ_ELEC_C,     MT_DIR_M2S, MT_PN_MP,  NULL,                  "小时冻结C相电流"},
    {CMD_AFN_C_F103_HFRZ_ELEC_ZERO,  MT_DIR_M2S, MT_PN_MP,  NULL,                  "小时冻结零序电流"},

    // 组14 pn:测量点号
    {CMD_AFN_C_F105_HFRZ_FRTH_HAVE,  MT_DIR_M2S, MT_PN_MP,  NULL,                  "小时冻结正向有功总电能量"},
    {CMD_AFN_C_F106_HFRZ_FRTH_NONE,  MT_DIR_M2S, MT_PN_MP,  NULL,                  "小时冻结正向无功总电能量"},
    {CMD_AFN_C_F107_HFRZ_BACK_HAVE,  MT_DIR_M2S, MT_PN_MP,  NULL,                  "小时冻结反向有功总电能量"},
    {CMD_AFN_C_F108_HFRZ_BACK_NONE,  MT_DIR_M2S, MT_PN_MP,  NULL,                  "小时冻结反向无功总电能量"},
    {CMD_AFN_C_F109_HFRZ_FRTH_HAVE_S,MT_DIR_M2S, MT_PN_MP,  NULL,                  "小时冻结正向有功总电能示值"},
    {CMD_AFN_C_F110_HFRZ_FRTH_NONE_S,MT_DIR_M2S, MT_PN_MP,  NULL,                  "小时冻结正向无功总电能示值"},
    {CMD_AFN_C_F111_HFRZ_BACK_HAVE_S,MT_DIR_M2S, MT_PN_MP,  NULL,                  "小时冻结反向有功总电能示值"},
    {CMD_AFN_C_F112_HFRZ_BACK_NONE_S,MT_DIR_M2S, MT_PN_MP,  NULL,                  "小时冻结反向无功总电能示值"}, 

    // 组15 pn:测量点号
    {CMD_AFN_C_F113_HFRZ_FACT_TOTAL, MT_DIR_M2S, MT_PN_MP,  NULL,                  "小时冻结总功率因数"},
    {CMD_AFN_C_F114_HFRZ_FACT_A,     MT_DIR_M2S, MT_PN_MP,  NULL,                  "小时冻结A相功率因数"},
    {CMD_AFN_C_F115_HFRZ_FACT_B,     MT_DIR_M2S, MT_PN_MP,  NULL,                  "小时冻结B相功率因数"},
    {CMD_AFN_C_F116_HFRZ_FACT_C,     MT_DIR_M2S, MT_PN_MP,  NULL,                  "小时冻结C相功率因数"},

    // 组16 pn:测量点号
    {CMD_AFN_C_F121_HFRZ_DC_VALUE,   MT_DIR_M2S, MT_PN_MP,  NULL,                  "小时冻结直流模拟量"},

    // 组17 pn:测量点号 C表示当前current
    {CMD_AFN_C_F129_FRTH_HAVE_POWR_C, MT_DIR_M2S, MT_PN_MP,  NULL,                  "当前正向有功电能示值（总、费率1～M）"},
    {CMD_AFN_C_F130_FRTH_NONE_POWR_C, MT_DIR_M2S, MT_PN_MP,  NULL,                  "当前正向无功（组合无功1）电能示值（总、费率1～M）"},
    {CMD_AFN_C_F131_BACK_HAVE_POWR_C, MT_DIR_M2S, MT_PN_MP,  NULL,                  "当前反向有功电能示值（总、费率1～M）"},
    {CMD_AFN_C_F132_BACK_NONE_POWR_C, MT_DIR_M2S, MT_PN_MP,  NULL,                  "当前反向无功（组合无功1）电能示值（总、费率1～M）"},
    {CMD_AFN_C_F133_NONE_POWR_P1_C,   MT_DIR_M2S, MT_PN_MP,  NULL,                  "当前一象限无功电能示值（总、费率1～M）"},
    {CMD_AFN_C_F134_NONE_POWR_P2_C,   MT_DIR_M2S, MT_PN_MP,  NULL,                  "当前二象限无功电能示值（总、费率1～M）"},
    {CMD_AFN_C_F135_NONE_POWR_P3_C,   MT_DIR_M2S, MT_PN_MP,  NULL,                  "当前三象限无功电能示值（总、费率1～M）"},
    {CMD_AFN_C_F136_NONE_POWR_P4_C,   MT_DIR_M2S, MT_PN_MP,  NULL,                  "当前三象限无功电能示值（总、费率1～M）"},

    // 组18 pn:测量点号 L表示上一结算日或上月 Last Month 
    {CMD_AFN_C_F137_FRTH_HAVE_POWR_L, MT_DIR_M2S, MT_PN_MP,  NULL,                  "上月（上一结算日）正向有功电能示值（总、费率1～M）"},
    {CMD_AFN_C_F138_FRTH_NONE_POWR_L, MT_DIR_M2S, MT_PN_MP,  NULL,                  "上月（上一结算日）正向无功（组合无功1）电能示值（总、费率1～M）"},
    {CMD_AFN_C_F139_BACK_HAVE_POWR_L, MT_DIR_M2S, MT_PN_MP,  NULL,                  "上月（上一结算日）反向有功电能示值（总、费率1～M）"},
    {CMD_AFN_C_F140_BACK_NONE_POWR_L, MT_DIR_M2S, MT_PN_MP,  NULL,                  "上月（上一结算日）反向无功（组合无功1）电能示值（总、费率1～M）"},
    {CMD_AFN_C_F141_NONE_POWR_P1_L,   MT_DIR_M2S, MT_PN_MP,  NULL,                  "上月（上一结算日）一象限无功电能示值（总、费率1～M）"},
    {CMD_AFN_C_F142_NONE_POWR_P2_L,   MT_DIR_M2S, MT_PN_MP,  NULL,                  "上月（上一结算日）二象限无功电能示值（总、费率1～M）"},
    {CMD_AFN_C_F143_NONE_POWR_P3_L,   MT_DIR_M2S, MT_PN_MP,  NULL,                  "上月（上一结算日）三象限无功电能示值（总、费率1～M）"},
    {CMD_AFN_C_F144_NONE_POWR_P4_L,   MT_DIR_M2S, MT_PN_MP,  NULL,                  "上月（上一结算日）四象限无功电能示值（总、费率1～M）"},

    // 组19 pn:测量点号
    {CMD_AFN_C_F145_FRTH_HAVE_DMND_C, MT_DIR_M2S, MT_PN_MP,  NULL,                  "当月正向有功最大需量及发生时间（总、费率1～M）"},
    {CMD_AFN_C_F146_FRTH_NONE_DMND_C, MT_DIR_M2S, MT_PN_MP,  NULL,                  "当月正向无功最大需量及发生时间（总、费率1～M）"},
    {CMD_AFN_C_F147_BACK_HAVE_DMND_C, MT_DIR_M2S, MT_PN_MP,  NULL,                  "当月反向有功最大需量及发生时间（总、费率1～M）"},
    {CMD_AFN_C_F148_BACK_NONE_DMND_C, MT_DIR_M2S, MT_PN_MP,  NULL,                  "当月反向无功最大需量及发生时间（总、费率1～M）"},
    {CMD_AFN_C_F149_FRTH_HAVE_DMND_L, MT_DIR_M2S, MT_PN_MP,  NULL,                  "上月（上一结算日）正向有功最大需量及发生时间（总、费率1～M）"},
    {CMD_AFN_C_F150_FRTH_NONE_DMND_L, MT_DIR_M2S, MT_PN_MP,  NULL,                  "上月（上一结算日）正向无功最大需量及发生时间（总、费率1～M）"},
    {CMD_AFN_C_F151_BACK_HAVE_DMND_L, MT_DIR_M2S, MT_PN_MP,  NULL,                  "上月（上一结算日）反向有功最大需量及发生时间（总、费率1～M）"},
    {CMD_AFN_C_F152_BACK_NONE_DMND_L, MT_DIR_M2S, MT_PN_MP,  NULL,                  "上月（上一结算日）反向无功最大需量及发生时间（总、费率1～M）"},

    // 组20 pn:测量点号
    {CMD_AFN_C_F153_FREZ_ZONE_1,      MT_DIR_M2S, MT_PN_MP,  NULL,                  "第一时区冻结正向有功电能示值（总、费率1～M）"},
    {CMD_AFN_C_F154_FREZ_ZONE_2,      MT_DIR_M2S, MT_PN_MP,  NULL,                  "第二时区冻结正向有功电能示值（总、费率1～M）"},
    {CMD_AFN_C_F155_FREZ_ZONE_3,      MT_DIR_M2S, MT_PN_MP,  NULL,                  "第三时区冻结正向有功电能示值（总、费率1～M）"},
    {CMD_AFN_C_F156_FREZ_ZONE_4,      MT_DIR_M2S, MT_PN_MP,  NULL,                  "第四时区冻结正向有功电能示值（总、费率1～M）"},
    {CMD_AFN_C_F157_FREZ_ZONE_5,      MT_DIR_M2S, MT_PN_MP,  NULL,                  "第五时区冻结正向有功电能示值（总、费率1～M）"},
    {CMD_AFN_C_F158_FREZ_ZONE_6,      MT_DIR_M2S, MT_PN_MP,  NULL,                  "第六时区冻结正向有功电能示值（总、费率1～M）"},
    {CMD_AFN_C_F159_FREZ_ZONE_7,      MT_DIR_M2S, MT_PN_MP,  NULL,                  "第七时区冻结正向有功电能示值（总、费率1～M）"},
    {CMD_AFN_C_F160_FREZ_ZONE_8,      MT_DIR_M2S, MT_PN_MP,  NULL,                  "第八时区冻结正向有功电能示值（总、费率1～M）"},

    // 组21 pn:测量点号
    {CMD_AFN_C_F161_METR_REMOTE_CTRL, MT_DIR_M2S, MT_PN_MP,  NULL,                  "电能表远程控制通断电状态及记录"},
    {CMD_AFN_C_F165_METR_SWITCH_RECD, MT_DIR_M2S, MT_PN_MP,  NULL,                  "电能表开关操作次数及时间"},
    {CMD_AFN_C_F166_METR_MODIFY_RECD, MT_DIR_M2S, MT_PN_MP,  NULL,                  "电能表参数修改次数及时间"},
    {CMD_AFN_C_F167_METR_BUY_USE,     MT_DIR_M2S, MT_PN_MP,  NULL,                  "电能表购、用电信息"},
    {CMD_AFN_C_F168_METR_BALANCE,     MT_DIR_M2S, MT_PN_MP,  NULL,                  "电能表结算信息"},

    // 组22 pn:测量点号
    {CMD_AFN_C_F169_READ_ROUTE,       MT_DIR_M2S, MT_PN_MP,  NULL,                  "集中抄表中继路由信息"},
    {CMD_AFN_C_F170_READ_METER,       MT_DIR_M2S, MT_PN_MP,  NULL,                  "集中抄表电表抄读信息"},
    /** } 下行 **/ 
    ///*}

     /*******************************
     *  (12) 请求2类数据(AFN=0DH）
     *
     *  上行: 根据不同类型,分别用请求1类数据和请求2类数据上行报文进行应答
     *  下行: 本类型
     *  加密: 不需要
     *  
    {*///
    /** { 上行 **/ 
    // 组1  电能示值、最大需量及电能量      pn:测量点号   (日冻结)    _D 表示日冻结 freeze_day
    {CMD_AFN_D_F1_FRTH_POWR_P1P4_D,    MT_DIR_S2M, MT_PN_P0,  emtTrans_afn0df1,     "正向有/无功电能示值、一/四象限无功电能示值（总、费率1～M，1≤M≤12）"},
    {CMD_AFN_D_F2_BACK_POWR_P2P3_D,    MT_DIR_S2M, MT_PN_P0,  emtTrans_afn0df2,     "反向有/无功电能示值、二/三象限无功电能示值（总、费率1～M，1≤M≤12）"},
    {CMD_AFN_D_F3_FRTH_DMND_TIME_D,    MT_DIR_S2M, MT_PN_P0,  emtTrans_afn0df3,     "正向有/无功最大需量及发生时间（总、费率1～M，1≤M≤12）"},
    {CMD_AFN_D_F4_BACK_DMND_TIME_D,    MT_DIR_S2M, MT_PN_P0,  emtTrans_afn0df4,     "反向有/无功最大需量及发生时间（总、费率1～M，1≤M≤12）"},
    {CMD_AFN_D_F5_FRTH_HAVE_POWR_D,    MT_DIR_S2M, MT_PN_P0,  emtTrans_afn0df5,     "正向有功电能量（总、费率1～M）"},
    {CMD_AFN_D_F6_FRTH_NONE_POWR_D,    MT_DIR_S2M, MT_PN_P0,  emtTrans_afn0df6,     "正向无功电能量（总、费率1～M）"},
    {CMD_AFN_D_F7_BACK_HAVE_POWR_D,    MT_DIR_S2M, MT_PN_P0,  emtTrans_afn0df7,     "反向有功电能量（总、费率1～M）"}, 
    {CMD_AFN_D_F8_BACK_NONE_POWR_D,    MT_DIR_S2M, MT_PN_P0,  emtTrans_afn0df8,     "反向无功电能量（总、费率1～M）"},

    // 组2  电能示值、最大需量 pn:测量点号 (抄表日冻结) _R 表示抄表冻结 freeze_read_meter
    {CMD_AFN_D_F9_FRTH_POWR_P1P4_R,    MT_DIR_M2S, MT_PN_MP,  emtTrans_afn0df9,     "正向有/无功电能示值、一/四象限无功电能示值（总、费率1～M，1≤M≤12）"},
    {CMD_AFN_D_F10_BACK_POWR_P2P3_R,   MT_DIR_M2S, MT_PN_MP,  emtTrans_afn0df10,    "反向有/无功电能示值、二/三象限无功电能示值（总、费率1～M，1≤M≤12）"},
    {CMD_AFN_D_F11_FRTH_DMND_TIME_R,   MT_DIR_M2S, MT_PN_MP,  emtTrans_afn0df11,    "正向有/无功最大需量及发生时间（总、费率1～M，1≤M≤12）"},
    {CMD_AFN_D_F12_BACK_DMND_TIME_R,   MT_DIR_M2S, MT_PN_MP,  emtTrans_afn0df12,    "反向有/无功最大需量及发生时间（总、费率1～M，1≤M≤12）"},
                
    // 组3  电能示值、最大需量及电能量 pn:测量点号 (月冻结) _M 表示月冻结 freeze_month
    {CMD_AFN_D_F17_FRTH_POWR_P1P4_M, MT_DIR_M2S, MT_PN_MP,    emtTrans_afn0df17,    "正向有/无功电能示值、一/四象限无功电能示值（总、费率1～M，1≤M≤12）"},
    {CMD_AFN_D_F18_BACK_POWR_P2P3_M, MT_DIR_M2S, MT_PN_MP,    emtTrans_afn0df18,    "反向有/无功电能示值、二/三象限无功电能示值（总、费率1～M，1≤M≤12"},
    {CMD_AFN_D_F19_FRTH_DMND_TIME_M, MT_DIR_M2S, MT_PN_MP,    emtTrans_afn0df19,    "正向有/无功最大需量及发生时间（总、费率1～M，1≤M≤12）"},
    {CMD_AFN_D_F20_BACK_DMND_TIME_M, MT_DIR_M2S, MT_PN_MP,    emtTrans_afn0df20,    "反向有/无功最大需量及发生时间（总、费率1～M，1≤M≤12）"},
    {CMD_AFN_D_F21_FRTH_HAVE_POWR_M, MT_DIR_M2S, MT_PN_MP,    emtTrans_afn0df21,    "正向有功电能量（总、费率1～M）"},
    {CMD_AFN_D_F22_FRTH_NONE_POWR_M, MT_DIR_M2S, MT_PN_MP,    emtTrans_afn0df22,    "正向无功电能量（总、费率1～M）"},
    {CMD_AFN_D_F23_BACK_HAVE_POWR_M, MT_DIR_M2S, MT_PN_MP,    emtTrans_afn0df23,    "正向无功电能量（总、费率1～M）"},
    {CMD_AFN_D_F24_BACK_NONE_POWR_M, MT_DIR_M2S, MT_PN_MP,    emtTrans_afn0df24,    "反向无功电能量（总、费率1～M）"},
            
    // 组4   pn:测量点号 日冻结   _D 表示日冻结 freeze_day
    {CMD_AFN_D_F25_POWR_FRZE_D,      MT_DIR_S2M, MT_PN_MP,    emtTrans_afn0df25,    "日总及分相最大有功功率及发生时间、有功功率为零时间"},
    {CMD_AFN_D_F26_DMND_FRZE_D,      MT_DIR_S2M, MT_PN_MP,    emtTrans_afn0df26,    "日总及分相最大需量及发生时间"},
    {CMD_AFN_D_F27_VOLT_FRZE_D,      MT_DIR_S2M, MT_PN_MP,    emtTrans_afn0df27,    "日电压统计数据"},
    {CMD_AFN_D_F28_UBLN_OVER_D,      MT_DIR_S2M, MT_PN_MP,    emtTrans_afn0df28,    "日不平衡度越限累计时间"},
    {CMD_AFN_D_F29_ELEC_OVER_D,      MT_DIR_S2M, MT_PN_MP,    emtTrans_afn0df29,    "日电流越限统计"},
    {CMD_AFN_D_F30_POWR_RATE_D,      MT_DIR_S2M, MT_PN_MP,    emtTrans_afn0df30,    "日视在功率越限累计时间"},
    {CMD_AFN_D_F31_LOAD_RATE_D,      MT_DIR_S2M, MT_PN_MP,    emtTrans_afn0df31,    "日负载率率统计"}, 
    {CMD_AFN_D_F32_METR_DROP_D,      MT_DIR_S2M, MT_PN_MP,    emtTrans_afn0df32,    "日电能表断相数据"},

    // 组5   pn:测量点号  _M 表示月冻结 freeze_month
    {CMD_AFN_D_F33_POWR_FRZE_M,      MT_DIR_S2M, MT_PN_MP,    emtTrans_afn0df33,    "月总及分相最大有功功率及发生时间、有功功率为零时间"},
    {CMD_AFN_D_F34_DMND_FRZE_M,      MT_DIR_S2M, MT_PN_MP,    emtTrans_afn0df34,    "月总及分相有功最大需量及发生时间"},
    {CMD_AFN_D_F35_VOLT_FRZE_M,      MT_DIR_S2M, MT_PN_MP,    emtTrans_afn0df35,    "月电压统计数据"},
    {CMD_AFN_D_F36_UBLN_OVER_M,      MT_DIR_S2M, MT_PN_MP,    emtTrans_afn0df36,    "月不平衡度越限累计时间"},
    {CMD_AFN_D_F37_ELEC_OVER_M,      MT_DIR_S2M, MT_PN_MP,    emtTrans_afn0df37,    "月电流越限统计"},
    {CMD_AFN_D_F38_POWR_RATE_M,      MT_DIR_S2M, MT_PN_MP,    emtTrans_afn0df38,    "月视在功率越限累计时间"},
    {CMD_AFN_D_F39_LOAD_RATE_M,      MT_DIR_S2M, MT_PN_MP,    emtTrans_afn0df39,    "月负载率率统计"}, 

    // 组6  pn:测量点号   _D 表示日冻结 freeze_day _M 表示月冻结 freeze_month
    {CMD_AFN_D_F41_CAPA_TIME_D,      MT_DIR_S2M, MT_PN_MP,    emtTrans_afn0df41,    "电容器投入累计时间和次数"},
    {CMD_AFN_D_F42_CAPA_NONE_D,      MT_DIR_S2M, MT_PN_MP,    emtTrans_afn0df42,    "日、月电容器累计补偿的无功电能量"},
    {CMD_AFN_D_F43_FACT_TIME_D,      MT_DIR_S2M, MT_PN_MP,    emtTrans_afn0df43,    "日功率因数区段累计时间"},
    {CMD_AFN_D_F44_FACT_TIME_M,      MT_DIR_S2M, MT_PN_MP,    emtTrans_afn0df44,    "月功率因数区段累计时间"},
    {CMD_AFN_D_F45_COPR_IRON_D,      MT_DIR_S2M, MT_PN_MP,    NULL,        "铜损、铁损有功电能示值"},
    {CMD_AFN_D_F46_COPR_IRON_M,      MT_DIR_S2M, MT_PN_MP,    NULL,        "铜损、铁损有功电能示值"},

    // 组7 终端统计数据 pn:测量点号 _D 表示日冻结 freeze_day _M 表示月冻结 freeze_month
    {CMD_AFN_D_F49_TML_ONOF_D,       MT_DIR_S2M, MT_PN_P0,    NULL,        "终端日供电时间、日复位累计次数"},
    {CMD_AFN_D_F50_TML_CTRL_D,       MT_DIR_S2M, MT_PN_P0,    NULL,        "终端日控制统计数据"},
    {CMD_AFN_D_F51_TML_ONOF_M,       MT_DIR_S2M, MT_PN_P0,    NULL,        "终端月供电时间、月复位累计次数"},
    {CMD_AFN_D_F52_TML_CTRL_M,       MT_DIR_S2M, MT_PN_P0,    NULL,        "终端月控制统计数据"},
    {CMD_AFN_D_F53_TML_FLOW_D,       MT_DIR_S2M, MT_PN_P0,    NULL,        "终端与主站日通信流量"},
    {CMD_AFN_D_F54_TML_FLOW_M,       MT_DIR_S2M, MT_PN_P0,    NULL,        "终端与主站月通信流量"},

    // 组8  总加组统计数据  pn:总加组号
    {CMD_AFN_D_F57_GRUP_RATE_D,      MT_DIR_S2M, MT_PN_GT,    NULL,        "总加组日最大、最小有功功率及其发生时间,有功功率为零日累计时间 日冻结"},
    {CMD_AFN_D_F58_GRUP_HAVE_D,      MT_DIR_S2M, MT_PN_GT,    NULL,        "总加组日累计有功电能量（总、费率1～M）日冻结"},
    {CMD_AFN_D_F59_GRUP_NONE_D,      MT_DIR_S2M, MT_PN_GT,    NULL,        "总加组日累计无功电能量（总、费率1～M）日冻结"},
    {CMD_AFN_D_F60_GRUP_RATE_M,      MT_DIR_S2M, MT_PN_GT,    NULL,        "总加组月最大、最小有功功率及其发生时间,有功功率为零月累计时间 月冻结"},
    {CMD_AFN_D_F61_GRUP_HAVE_M,      MT_DIR_S2M, MT_PN_GT,    NULL,        "总加组月累计有功电能量（总、费率1～M）月冻结"},
    {CMD_AFN_D_F62_GRUP_NONE_M,      MT_DIR_S2M, MT_PN_GT,    NULL,        "总加组月累计无功电能量（总、费率1～M）月冻结"},

    // 组9  总加组越限统计数据 pn:总加组号
    {CMD_AFN_D_F65_GRUP_RATE_OVER,   MT_DIR_S2M, MT_PN_GT,    NULL,        "总加组超功率定值的月累计时间、月累计电能量  月冻结"},
    {CMD_AFN_D_F66_GRUP_POWR_OVER,   MT_DIR_S2M, MT_PN_GT,    NULL,        "总加组超月电能量定值的月累计时间、累计电能量 月冻结"},

    // 组10 总加组曲线         pn:总加组号    
    {CMD_AFN_D_F73_CURVE_RATE_HAVE,  MT_DIR_S2M, MT_PN_GT,    NULL,        "总加组有功功率曲线"},
    {CMD_AFN_D_F74_CURVE_RATE_NONE,  MT_DIR_S2M, MT_PN_GT,    NULL,        "总加组无功功率曲线"},
    {CMD_AFN_D_F75_CURVE_POWR_HAVE,  MT_DIR_S2M, MT_PN_GT,    NULL,        "总加组有功电能量曲线"},
    {CMD_AFN_D_F76_CURVE_POWR_NONE,  MT_DIR_S2M, MT_PN_GT,    NULL,        "总加组无功电能量曲线"},

    // 组11  功率曲线 pn:测量点号      
    {CMD_AFN_D_F81_CURVE_HAVE,       MT_DIR_S2M, MT_PN_MP,  NULL,                  "有功功率曲线"},
    {CMD_AFN_D_F82_CURVE_HAVE_A,     MT_DIR_S2M, MT_PN_MP,  NULL,                  "A相有功功率曲线"},
    {CMD_AFN_D_F83_CURVE_HAVE_B,     MT_DIR_S2M, MT_PN_MP,  NULL,                  "B相有功功率曲线"},
    {CMD_AFN_D_F84_CURVE_HAVE_C,     MT_DIR_S2M, MT_PN_MP,  NULL,                  "C相有功功率曲线"},
    {CMD_AFN_D_F85_CURVE_NONE,       MT_DIR_S2M, MT_PN_MP,  NULL,                  "无功功率曲线"},
    {CMD_AFN_D_F86_CURVE_NONE_A,     MT_DIR_S2M, MT_PN_MP,  NULL,                  "A相无功功率曲线"},
    {CMD_AFN_D_F87_CURVE_NONE_B,     MT_DIR_S2M, MT_PN_MP,  NULL,                  "B相无功功率曲线"},
    {CMD_AFN_D_F88_CURVE_NONE_C,     MT_DIR_S2M, MT_PN_MP,  NULL,                  "C相无功功率曲线"},

    // 组12 电压电流曲线 pn:测量点号
    {CMD_AFN_D_F89_CURVE_VOLT_A,     MT_DIR_S2M, MT_PN_MP,  NULL,                  "A相电压曲线"},
    {CMD_AFN_D_F90_CURVE_VOLT_B,     MT_DIR_S2M, MT_PN_MP,  NULL,                  "B相电压曲线"},
    {CMD_AFN_D_F91_CURVE_VOLT_C,     MT_DIR_S2M, MT_PN_MP,  NULL,                  "C相电压曲线"},
    {CMD_AFN_D_F92_CURVE_ELEC_A,     MT_DIR_S2M, MT_PN_MP,  NULL,                  "A相电流曲线"},
    {CMD_AFN_D_F93_CURVE_ELEC_B,     MT_DIR_S2M, MT_PN_MP,  NULL,                  "B相电流曲线"},
    {CMD_AFN_D_F94_CURVE_ELEC_C,     MT_DIR_S2M, MT_PN_MP,  NULL,                  "C相电流曲线"},
    {CMD_AFN_D_F95_CURVE_ZERO_E,     MT_DIR_S2M, MT_PN_MP,  NULL,                  "零序电流曲线"},

    // 组13 总电能量、总电能示值曲线 pn:测量点号
    {CMD_AFN_D_F97_CURVE_FRTH_HAVE,  MT_DIR_S2M, MT_PN_MP,  NULL,                  "正向有功总电能量曲线"},
    {CMD_AFN_D_F98_CURVE_FRTH_NONE,  MT_DIR_S2M, MT_PN_MP,  NULL,                  "正向无功总电能量曲线"},
    {CMD_AFN_D_F98_CURVE_BACK_HAVE,  MT_DIR_S2M, MT_PN_MP,  NULL,                  "反向有功总电能量曲线"},
    {CMD_AFN_D_F100_CURVE_BACK_HAVE, MT_DIR_S2M, MT_PN_MP,  NULL,                  "反向无功总电能量曲线"},
    {CMD_AFN_D_F101_CURVE_FRTH_HAVE, MT_DIR_S2M, MT_PN_MP,  NULL,                  "正向有功总电能示值曲线"},
    {CMD_AFN_D_F102_CURVE_FRTH_NONE, MT_DIR_S2M, MT_PN_MP,  NULL,                  "正向无功总电能示值曲线"},
    {CMD_AFN_D_F103_CURVE_BACK_HAVE, MT_DIR_S2M, MT_PN_MP,  NULL,                  "反向有功总电能示值曲线"},
    {CMD_AFN_D_F104_CURVE_BACK_NONE, MT_DIR_S2M, MT_PN_MP,  NULL,                  "反向无功总电能示值曲线"},

    // 组14 功率因数、电压相位角曲线、电流相位角曲线 pn:测量点号
    {CMD_AFN_D_F105_CURVE_FACTOR_T,  MT_DIR_S2M, MT_PN_MP,  NULL,                  "总功率因数曲线"},
    {CMD_AFN_D_F106_CURVE_FACTOR_A,  MT_DIR_S2M, MT_PN_MP,  NULL,                  "A相功率因数曲线"},
    {CMD_AFN_D_F107_CURVE_FACTOR_B,  MT_DIR_S2M, MT_PN_MP,  NULL,                  "B相功率因数曲线"},
    {CMD_AFN_D_F108_CURVE_FACTOR_C,  MT_DIR_S2M, MT_PN_MP,  NULL,                  "C相功率因数曲线"},
    {CMD_AFN_D_F109_ANGLE_CURVE_VOLT,MT_DIR_S2M, MT_PN_MP,  NULL,                  "正向有功总电能示值曲线"},
    {CMD_AFN_D_F110_ANGLE_CURVE_ELEC,MT_DIR_S2M, MT_PN_MP,  NULL,                  "正向无功总电能示值曲线"},

    // 组15 谐波监测统计数据 pn:测量点号
    {CMD_AFN_D_F113_ELEC_HARM_TIME_A,MT_DIR_S2M, MT_PN_MP,  NULL,                  "A相2～19次谐波电流日最大值及发生时间 日冻结"},
    {CMD_AFN_D_F114_ELEC_HARM_TIME_B,MT_DIR_S2M, MT_PN_MP,  NULL,                  "B相2～19次谐波电流日最大值及发生时间 日冻结"},
    {CMD_AFN_D_F115_ELEC_HARM_TIME_C,MT_DIR_S2M, MT_PN_MP,  NULL,                  "C相2～19次谐波电流日最大值及发生时间 日冻结"},
    {CMD_AFN_D_F116_VOLT_HARM_TIME_A,MT_DIR_S2M, MT_PN_MP,  NULL,                  "A相2～19次谐波电压含有率及总畸变率日最大值及发生时间 日冻结"},
    {CMD_AFN_D_F117_VOLT_HARM_TIME_B,MT_DIR_S2M, MT_PN_MP,  NULL,                  "B相2～19次谐波电压含有率及总畸变率日最大值及发生时间 日冻结"},
    {CMD_AFN_D_F118_VOLT_HARM_TIME_C,MT_DIR_S2M, MT_PN_MP,  NULL,                  "C相2～19次谐波电压含有率及总畸变率日最大值及发生时间 日冻结"},

    // 组16 谐波越限统计数据 pn:测量点号
    {CMD_AFN_D_F121_HARM_OVER_A,     MT_DIR_S2M, MT_PN_MP,  NULL,                  "A相谐波越限日统计数据 日冻结"},
    {CMD_AFN_D_F122_HARM_OVER_B,     MT_DIR_S2M, MT_PN_MP,  NULL,                  "B相谐波越限日统计数据 日冻结"},
    {CMD_AFN_D_F123_HARM_OVER_C,     MT_DIR_S2M, MT_PN_MP,  NULL,                  "C相谐波越限日统计数据 日冻结"},

    // 组17 直流模拟量数据     pn:直流模拟量端口号
    {CMD_AFN_D_F129_DC_D,            MT_DIR_S2M, MT_PN_MP,  NULL,                  "直流模拟量越限日累计时间、最大/最小值及发生时间 日冻结"},
    {CMD_AFN_D_F130_DC_M,            MT_DIR_S2M, MT_PN_MP,  NULL,                  "直流模拟量越限月累计时间、最大/最小值及发生时间 月冻结"},

    // 组18 直流模拟量数据曲线 pn:测量点号
    {CMD_AFN_D_F138_DC_CURVE,        MT_DIR_S2M, MT_PN_MP,  NULL,                  "直流模拟量数据曲线"},

    // 组19 四个象限无功总电能示值曲线 pn:测量点号
    {CMD_AFN_D_F145_CURVE_PHASE_1,   MT_DIR_S2M, MT_PN_MP,  NULL,                  "一象限无功总电能示值曲线"},
    {CMD_AFN_D_F146_CURVE_PHASE_4,   MT_DIR_S2M, MT_PN_MP,  NULL,                  "四象限无功总电能示值曲线"},
    {CMD_AFN_D_F147_CURVE_PHASE_2,   MT_DIR_S2M, MT_PN_MP,  NULL,                  "二象限无功总电能示值曲线"},
    {CMD_AFN_D_F148_CURVE_PHASE_3,   MT_DIR_S2M, MT_PN_MP,  NULL,                  "三象限无功总电能示值曲线"},

    // 组20 分相电能示值 pn:测量点号
    {CMD_AFN_D_F153_FRTH_HAVE_D,     MT_DIR_S2M, MT_PN_MP,  NULL,                  "日冻结分相正向有功电能示值 日冻结"},
    {CMD_AFN_D_F154_FRTH_NONE_D,     MT_DIR_S2M, MT_PN_MP,  NULL,                  "日冻结分相正向无功电能示值 日冻结"},
    {CMD_AFN_D_F155_BACK_HAVE_D,     MT_DIR_S2M, MT_PN_MP,  NULL,                  "日冻结分相反向有功电能示值 日冻结"},
    {CMD_AFN_D_F156_BACK_NONE_D,     MT_DIR_S2M, MT_PN_MP,  NULL,                  "日冻结分相反向无功电能示值 日冻结"},
    {CMD_AFN_D_F157_FRTH_HAVE_M,     MT_DIR_S2M, MT_PN_MP,  NULL,                  "月冻结分相正向有功电能示值 月冻结"},
    {CMD_AFN_D_F158_FRTH_NONE_M,     MT_DIR_S2M, MT_PN_MP,  NULL,                  "月冻结分相正向无功电能示值 月冻结"},
    {CMD_AFN_D_F159_BACK_HAVE_M,     MT_DIR_S2M, MT_PN_MP,  NULL,                  "月冻结分相反向有功电能示值 月冻结"},
    {CMD_AFN_D_F160_BACK_NONE_M,     MT_DIR_S2M, MT_PN_MP,  NULL,                  "月冻结分相反向无功电能示值 月冻结"},

    // 组21 pn:测量点号
    {CMD_AFN_D_F161_HAVE_FRTH_D,     MT_DIR_S2M, MT_PN_MP,  NULL,                  "正向有功电能示值（总、费率1～M） 日冻结"},
    {CMD_AFN_D_F162_NONE_FRTM_D,     MT_DIR_S2M, MT_PN_MP,  NULL,                  "正向无功（组合无功1）电能示值（总、费率1～M）日冻结"},
    {CMD_AFN_D_F163_HAVE_BACK_D,     MT_DIR_S2M, MT_PN_MP,  NULL,                  "反向有功电能示值（总、费率1～M）日冻结"},
    {CMD_AFN_D_F164_NONE_BACK_D,     MT_DIR_S2M, MT_PN_MP,  NULL,                  "反向无功（组合无功1）电能示值（总、费率1～M）日冻结"},
    {CMD_AFN_D_F165_NONE_D_PHASE_1,  MT_DIR_S2M, MT_PN_MP,  NULL,                  "一象限无功电能示值（总、费率1～M）日冻结"},
    {CMD_AFN_D_F166_NONE_D_PHASE_2,  MT_DIR_S2M, MT_PN_MP,  NULL,                  "二象限无功电能示值（总、费率1～M）日冻结"},
    {CMD_AFN_D_F167_NONE_D_PHASE_3,  MT_DIR_S2M, MT_PN_MP,  NULL,                  "三象限无功电能示值（总、费率1～M）日冻结 月冻结"},
    {CMD_AFN_D_F168_NONE_D_PHASE_4,  MT_DIR_S2M, MT_PN_MP,  NULL,                  "四象限无功电能示值（总、费率1～M）日冻结 月冻结"},

    // 组22 pn:测量点号 抄表日冻结
    {CMD_AFN_D_F169_HAVE_FRTH_R,     MT_DIR_S2M, MT_PN_MP,  NULL,                  "正向有功电能示值（总、费率1～M）  抄表日冻结"},
    {CMD_AFN_D_F170_NONE_FRTM_R,     MT_DIR_S2M, MT_PN_MP,  NULL,                  "正向无功（组合无功1）电能示值（总、费率1～M） 抄表日冻结"},
    {CMD_AFN_D_F171_HAVE_BACK_R,     MT_DIR_S2M, MT_PN_MP,  NULL,                  "反向有功电能示值（总、费率1～M）  抄表日冻结"},
    {CMD_AFN_D_F172_NONE_BACK_R,     MT_DIR_S2M, MT_PN_MP,  NULL,                  "反向无功（组合无功1）电能示值（总、费率1～M）抄表日冻结"},
    {CMD_AFN_D_F173_NONE_R_PHASE_1,  MT_DIR_S2M, MT_PN_MP,  NULL,                  "一象限无功电能示值（总、费率1～M）抄表日冻结"},
    {CMD_AFN_D_F174_NONE_R_PHASE_2,  MT_DIR_S2M, MT_PN_MP,  NULL,                  "二象限无功电能示值（总、费率1～M）抄表日冻结"},
    {CMD_AFN_D_F175_NONE_R_PHASE_3,  MT_DIR_S2M, MT_PN_MP,  NULL,                  "三象限无功电能示值（总、费率1～M）抄表日冻结"},
    {CMD_AFN_D_F176_NONE_R_PHASE_4,  MT_DIR_S2M, MT_PN_MP,  NULL,                  "四象限无功电能示值（总、费率1～M）抄表日冻结"},

    // 组23 pn:测量点号
    {CMD_AFN_D_F177_HAVE_FRTH_M,     MT_DIR_S2M, MT_PN_MP,  NULL,                  "正向有功电能示值（总、费率1～M） 月冻结"},
    {CMD_AFN_D_F178_NONE_FRTM_M,     MT_DIR_S2M, MT_PN_MP,  NULL,                  "正向无功（组合无功1）电能示值（总、费率1～M）月冻结"},
    {CMD_AFN_D_F179_HAVE_BACK_M,     MT_DIR_S2M, MT_PN_MP,  NULL,                  "反向有功电能示值（总、费率1～M） 月冻结"},
    {CMD_AFN_D_F180_NONE_BACK_M,     MT_DIR_S2M, MT_PN_MP,  NULL,                  "反向无功（组合无功1）电能示值（总、费率1～M）月冻结"},
    {CMD_AFN_D_F181_NONE_M_PHASE_1,  MT_DIR_S2M, MT_PN_MP,  NULL,                  "一象限无功电能示值（总、费率1～M） 月冻结"},
    {CMD_AFN_D_F182_NONE_M_PHASE_2,  MT_DIR_S2M, MT_PN_MP,  NULL,                  "二象限无功电能示值（总、费率1～M） 月冻结"},
    {CMD_AFN_D_F183_NONE_M_PHASE_3,  MT_DIR_S2M, MT_PN_MP,  NULL,                  "三象限无功电能示值（总、费率1～M） 月冻结"},
    {CMD_AFN_D_F184_NONE_M_PHASE_4,  MT_DIR_S2M, MT_PN_MP,  NULL,                  "四象限无功电能示值（总、费率1～M） 月冻结"},

    // 组24 pn:测量点号
    {CMD_AFN_D_F185_HAVE_DMND_FRTH_D, MT_DIR_S2M, MT_PN_MP,  NULL,                  "正向有功最大需量及发生时间（总、费率1～M）日冻结"},
    {CMD_AFN_D_F186_NONE_DMND_FRTH_D, MT_DIR_S2M, MT_PN_MP,  NULL,                  "正向无功最大需量及发生时间（总、费率1～M）日冻结"},
    {CMD_AFN_D_F187_HAVE_DMND_BACK_D, MT_DIR_S2M, MT_PN_MP,  NULL,                  "反向有功最大需量及发生时间（总、费率1～M）日冻结"},
    {CMD_AFN_D_F188_NONE_DMND_BACK_D, MT_DIR_S2M, MT_PN_MP,  NULL,                  "反向无功最大需量及发生时间（总、费率1～M）日冻结"},
    {CMD_AFN_D_F189_HAVE_DMND_FRTH_R, MT_DIR_S2M, MT_PN_MP,  NULL,                  "正向有功最大需量及发生时间（总、费率1～M）抄表日冻结"},
    {CMD_AFN_D_F190_NONE_DMND_FRTH_R, MT_DIR_S2M, MT_PN_MP,  NULL,                  "正向无功最大需量及发生时间（总、费率1～M）抄表日冻结"},
    {CMD_AFN_D_F191_HAVE_DMND_BACK_R, MT_DIR_S2M, MT_PN_MP,  NULL,                  "反向有功最大需量及发生时间（总、费率1～M）抄表日冻结"},
    {CMD_AFN_D_F192_NONE_DMND_BACK_R, MT_DIR_S2M, MT_PN_MP,  NULL,                  "反向无功最大需量及发生时间（总、费率1～M）抄表日冻结"},

    // 组25 pn:测量点号
    {CMD_AFN_D_F193_HAVE_DMND_FRTH_M,  MT_DIR_S2M, MT_PN_MP,  NULL,                  "正向有功最大需量及发生时间（总、费率1～M） 月冻结"},
    {CMD_AFN_D_F194_NONE_DMND_FRTH_M,  MT_DIR_S2M, MT_PN_MP,  NULL,                  "正向无功最大需量及发生时间（总、费率1～M） 月冻结"},
    {CMD_AFN_D_F195_HAVE_DMND_BACK_M,  MT_DIR_S2M, MT_PN_MP,  NULL,                  "反向有功最大需量及发生时间（总、费率1～M） 月冻结"},
    {CMD_AFN_D_F196_NONE_DMND_BACK_M,  MT_DIR_S2M, MT_PN_MP,  NULL,                  "反向无功最大需量及发生时间（总、费率1～M） 月冻结"},

    // 组26 pn:测量点号
    {CMD_AFN_D_F201_FREZ_ZONE_1,       MT_DIR_S2M, MT_PN_MP,  NULL,                  "第一时区冻结正向有功电能示值（总、费率1～M）"},
    {CMD_AFN_D_F202_FREZ_ZONE_2,       MT_DIR_S2M, MT_PN_MP,  NULL,                  "第二时区冻结正向有功电能示值（总、费率1～M）"},
    {CMD_AFN_D_F203_FREZ_ZONE_3,       MT_DIR_S2M, MT_PN_MP,  NULL,                  "第三时区冻结正向有功电能示值（总、费率1～M）"},
    {CMD_AFN_D_F204_FREZ_ZONE_4,       MT_DIR_S2M, MT_PN_MP,  NULL,                  "第四时区冻结正向有功电能示值（总、费率1～M）"},
    {CMD_AFN_D_F205_FREZ_ZONE_5,       MT_DIR_S2M, MT_PN_MP,  NULL,                  "第五时区冻结正向有功电能示值（总、费率1～M）"},
    {CMD_AFN_D_F206_FREZ_ZONE_6,       MT_DIR_S2M, MT_PN_MP,  NULL,                  "第六时区冻结正向有功电能示值（总、费率1～M）"},
    {CMD_AFN_D_F207_FREZ_ZONE_7,       MT_DIR_S2M, MT_PN_MP,  NULL,                  "第七时区冻结正向有功电能示值（总、费率1～M）"},
    {CMD_AFN_D_F208_FREZ_ZONE_8,       MT_DIR_S2M, MT_PN_MP,  NULL,                  "第八时区冻结正向有功电能示值（总、费率1～M）"},

    // 组27 pn:测量点号
    {CMD_AFN_D_F209_METR_REMOTE_INFO,  MT_DIR_S2M, MT_PN_MP,  NULL,                  "电能表远程控制通断电状态及记录"},
    {CMD_AFN_D_F213_METR_SWITCH_INFO,  MT_DIR_S2M, MT_PN_MP,  NULL,                  "电能表开关操作次数及时间"},
    {CMD_AFN_D_F214_METR_MODIFY_INFO,  MT_DIR_S2M, MT_PN_MP,  NULL,                  "电能表参数修改次数及时间"},
    {CMD_AFN_D_F215_METR_BUY_USR,      MT_DIR_S2M, MT_PN_MP,  NULL,                  "电能表购、用电信息"},
    {CMD_AFN_D_F216_METR_BALANCE,      MT_DIR_S2M, MT_PN_MP,  NULL,                  "电能表结算信息"},

    // 组27 pn:测量点号
    {CMD_AFN_D_F217_WHITE_YAWP_CURVE,  MT_DIR_S2M, MT_PN_MP,  NULL,                  "台区集中抄表载波主节点白噪声曲线"},
    {CMD_AFN_D_F218_COLOR_YAWP_CURVE,  MT_DIR_S2M, MT_PN_MP,  NULL,                  "台区集中抄表载波主节点色噪声曲线"},
    /** } 上行 **/ 

    /** { 下行 **/ 
    // 组1  电能示值、最大需量及电能量      pn:测量点号   (日冻结)    _D 表示日冻结 freeze_day
    {CMD_AFN_D_F1_FRTH_POWR_P1P4_D,    MT_DIR_M2S, MT_PN_P0,  emtTrans_td_d,        "正向有/无功电能示值、一/四象限无功电能示值（总、费率1～M，1≤M≤12）"},
    {CMD_AFN_D_F2_BACK_POWR_P2P3_D,    MT_DIR_M2S, MT_PN_P0,  emtTrans_td_d,        "反向有/无功电能示值、二/三象限无功电能示值（总、费率1～M，1≤M≤12）"},
    {CMD_AFN_D_F3_FRTH_DMND_TIME_D,    MT_DIR_M2S, MT_PN_P0,  emtTrans_td_d,        "正向有/无功最大需量及发生时间（总、费率1～M，1≤M≤12）"},
    {CMD_AFN_D_F4_BACK_DMND_TIME_D,    MT_DIR_M2S, MT_PN_P0,  emtTrans_td_d,        "反向有/无功最大需量及发生时间（总、费率1～M，1≤M≤12）"},
    {CMD_AFN_D_F5_FRTH_HAVE_POWR_D,    MT_DIR_M2S, MT_PN_P0,  emtTrans_td_d,        "正向有功电能量（总、费率1～M）"},
    {CMD_AFN_D_F6_FRTH_NONE_POWR_D,    MT_DIR_M2S, MT_PN_P0,  emtTrans_td_d,        "正向无功电能量（总、费率1～M）"},
    {CMD_AFN_D_F7_BACK_HAVE_POWR_D,    MT_DIR_M2S, MT_PN_P0,  emtTrans_td_d,        "反向有功电能量（总、费率1～M）"}, 
    {CMD_AFN_D_F8_BACK_NONE_POWR_D,    MT_DIR_M2S, MT_PN_P0,  emtTrans_td_d,        "反向无功电能量（总、费率1～M）"},

    // 组2  电能示值、最大需量 pn:测量点号 (抄表日冻结) _R 表示抄表冻结 freeze_read_meter
    {CMD_AFN_D_F9_FRTH_POWR_P1P4_R,    MT_DIR_M2S, MT_PN_MP,  emtTrans_td_d,        "正向有/无功电能示值、一/四象限无功电能示值（总、费率1～M，1≤M≤12）"},
    {CMD_AFN_D_F10_BACK_POWR_P2P3_R,   MT_DIR_M2S, MT_PN_MP,  emtTrans_td_d,        "反向有/无功电能示值、二/三象限无功电能示值（总、费率1～M，1≤M≤12）"},
    {CMD_AFN_D_F11_FRTH_DMND_TIME_R,   MT_DIR_M2S, MT_PN_MP,  emtTrans_td_d,        "正向有/无功最大需量及发生时间（总、费率1～M，1≤M≤12）"},
    {CMD_AFN_D_F12_BACK_DMND_TIME_R,   MT_DIR_M2S, MT_PN_MP,  emtTrans_td_d,        "反向有/无功最大需量及发生时间（总、费率1～M，1≤M≤12）"},
                
    // 组3  电能示值、最大需量及电能量 pn:测量点号 (月冻结) _M 表示月冻结 freeze_month
    {CMD_AFN_D_F17_FRTH_POWR_P1P4_M, MT_DIR_M2S, MT_PN_MP,    emtTrans_td_m,        "正向有/无功电能示值、一/四象限无功电能示值（总、费率1～M，1≤M≤12）"},
    {CMD_AFN_D_F18_BACK_POWR_P2P3_M, MT_DIR_M2S, MT_PN_MP,    emtTrans_td_m,        "反向有/无功电能示值、二/三象限无功电能示值（总、费率1～M，1≤M≤12"},
    {CMD_AFN_D_F19_FRTH_DMND_TIME_M, MT_DIR_M2S, MT_PN_MP,    emtTrans_td_m,        "正向有/无功最大需量及发生时间（总、费率1～M，1≤M≤12）"},
    {CMD_AFN_D_F20_BACK_DMND_TIME_M, MT_DIR_M2S, MT_PN_MP,    emtTrans_td_m,        "反向有/无功最大需量及发生时间（总、费率1～M，1≤M≤12）"},
    {CMD_AFN_D_F21_FRTH_HAVE_POWR_M, MT_DIR_M2S, MT_PN_MP,    emtTrans_td_m,        "正向有功电能量（总、费率1～M）"},
    {CMD_AFN_D_F22_FRTH_NONE_POWR_M, MT_DIR_M2S, MT_PN_MP,    emtTrans_td_m,        "正向无功电能量（总、费率1～M）"},
    {CMD_AFN_D_F23_BACK_HAVE_POWR_M, MT_DIR_M2S, MT_PN_MP,    emtTrans_td_m,        "正向无功电能量（总、费率1～M）"},
    {CMD_AFN_D_F24_BACK_NONE_POWR_M, MT_DIR_M2S, MT_PN_MP,    emtTrans_td_m,        "反向无功电能量（总、费率1～M）"},
            
    // 组4   pn:测量点号 日冻结   _D 表示日冻结 freeze_day
    {CMD_AFN_D_F25_POWR_FRZE_D,      MT_DIR_M2S, MT_PN_P0,    emtTrans_td_d,        "日总及分相最大有功功率及发生时间、有功功率为零时间"},
    {CMD_AFN_D_F26_DMND_FRZE_D,      MT_DIR_M2S, MT_PN_P0,    emtTrans_td_d,        "日总及分相最大需量及发生时间"},
    {CMD_AFN_D_F27_VOLT_FRZE_D,      MT_DIR_M2S, MT_PN_P0,    emtTrans_td_d,        "日电压统计数据"},
    {CMD_AFN_D_F28_UBLN_OVER_D,      MT_DIR_M2S, MT_PN_P0,    emtTrans_td_d,        "日不平衡度越限累计时间"},
    {CMD_AFN_D_F29_ELEC_OVER_D,      MT_DIR_M2S, MT_PN_P0,    emtTrans_td_d,        "日电流越限统计"},
    {CMD_AFN_D_F30_POWR_RATE_D,      MT_DIR_M2S, MT_PN_P0,    emtTrans_td_d,        "日视在功率越限累计时间"},
    {CMD_AFN_D_F31_LOAD_RATE_D,      MT_DIR_M2S, MT_PN_P0,    emtTrans_td_d,        "日负载率率统计"}, 
    {CMD_AFN_D_F32_METR_DROP_D,      MT_DIR_M2S, MT_PN_P0,    emtTrans_td_d,        "日电能表断相数据"},

    // 组5   pn:测量点号  _M 表示月冻结 freeze_month
    {CMD_AFN_D_F33_POWR_FRZE_M,      MT_DIR_M2S, MT_PN_P0,    emtTrans_td_m,        "月总及分相最大有功功率及发生时间、有功功率为零时间"},
    {CMD_AFN_D_F34_DMND_FRZE_M,      MT_DIR_M2S, MT_PN_P0,    emtTrans_td_m,        "月总及分相有功最大需量及发生时间"},
    {CMD_AFN_D_F35_VOLT_FRZE_M,      MT_DIR_M2S, MT_PN_P0,    emtTrans_td_m,        "月电压统计数据"},
    {CMD_AFN_D_F36_UBLN_OVER_M,      MT_DIR_M2S, MT_PN_P0,    emtTrans_td_m,        "月不平衡度越限累计时间"},
    {CMD_AFN_D_F37_ELEC_OVER_M,      MT_DIR_M2S, MT_PN_P0,    emtTrans_td_m,        "月电流越限统计"},
    {CMD_AFN_D_F38_POWR_RATE_M,      MT_DIR_M2S, MT_PN_P0,    emtTrans_td_m,        "月视在功率越限累计时间"},
    {CMD_AFN_D_F39_LOAD_RATE_M,      MT_DIR_M2S, MT_PN_P0,    emtTrans_td_m,        "月负载率率统计"}, 

    // 组6  pn:测量点号   _D 表示日冻结 freeze_day _M 表示月冻结 freeze_month
    {CMD_AFN_D_F41_CAPA_TIME_D,      MT_DIR_M2S, MT_PN_P0,    emtTrans_td_d,        "电容器投入累计时间和次数"},
    {CMD_AFN_D_F42_CAPA_NONE_D,      MT_DIR_M2S, MT_PN_P0,    emtTrans_td_d,        "日、月电容器累计补偿的无功电能量"},
    {CMD_AFN_D_F43_FACT_TIME_D,      MT_DIR_M2S, MT_PN_P0,    emtTrans_td_d,        "日功率因数区段累计时间"},
    {CMD_AFN_D_F44_FACT_TIME_M,      MT_DIR_M2S, MT_PN_P0,    emtTrans_td_m,        "月功率因数区段累计时间"},
    {CMD_AFN_D_F45_COPR_IRON_D,      MT_DIR_M2S, MT_PN_P0,    emtTrans_td_d,        "铜损、铁损有功电能示值"},
    {CMD_AFN_D_F46_COPR_IRON_M,      MT_DIR_M2S, MT_PN_P0,    emtTrans_td_m,        "铜损、铁损有功电能示值"},

    // 组7 终端统计数据 pn:测量点号 _D 表示日冻结 freeze_day _M 表示月冻结 freeze_month
    {CMD_AFN_D_F49_TML_ONOF_D,       MT_DIR_M2S, MT_PN_P0,    emtTrans_td_d,        "终端日供电时间、日复位累计次数"},
    {CMD_AFN_D_F50_TML_CTRL_D,       MT_DIR_M2S, MT_PN_P0,    emtTrans_td_d,        "终端日控制统计数据"},
    {CMD_AFN_D_F51_TML_ONOF_M,       MT_DIR_M2S, MT_PN_P0,    emtTrans_td_m,        "终端月供电时间、月复位累计次数"},
    {CMD_AFN_D_F52_TML_CTRL_M,       MT_DIR_M2S, MT_PN_P0,    emtTrans_td_m,        "终端月控制统计数据"},
    {CMD_AFN_D_F53_TML_FLOW_D,       MT_DIR_M2S, MT_PN_P0,    emtTrans_td_d,        "终端与主站日通信流量"},
    {CMD_AFN_D_F54_TML_FLOW_M,       MT_DIR_M2S, MT_PN_P0,    emtTrans_td_m,        "终端与主站月通信流量"},

    // 组8  总加组统计数据  pn:总加组号
    {CMD_AFN_D_F57_GRUP_RATE_D,      MT_DIR_M2S, MT_PN_GT,    emtTrans_td_d,        "总加组日最大、最小有功功率及其发生时间,有功功率为零日累计时间 日冻结"},
    {CMD_AFN_D_F58_GRUP_HAVE_D,      MT_DIR_M2S, MT_PN_GT,    emtTrans_td_d,        "总加组日累计有功电能量（总、费率1～M）日冻结"},
    {CMD_AFN_D_F59_GRUP_NONE_D,      MT_DIR_M2S, MT_PN_GT,    emtTrans_td_d,        "总加组日累计无功电能量（总、费率1～M）日冻结"},
    {CMD_AFN_D_F60_GRUP_RATE_M,      MT_DIR_M2S, MT_PN_GT,    emtTrans_td_m,        "总加组月最大、最小有功功率及其发生时间,有功功率为零月累计时间 月冻结"},
    {CMD_AFN_D_F61_GRUP_HAVE_M,      MT_DIR_M2S, MT_PN_GT,    emtTrans_td_m,        "总加组月累计有功电能量（总、费率1～M）月冻结"},
    {CMD_AFN_D_F62_GRUP_NONE_M,      MT_DIR_M2S, MT_PN_GT,    emtTrans_td_m,        "总加组月累计无功电能量（总、费率1～M）月冻结"},

    // 组9  总加组越限统计数据 pn:总加组号
    {CMD_AFN_D_F65_GRUP_RATE_OVER,   MT_DIR_M2S, MT_PN_GT,    emtTrans_td_m,        "总加组超功率定值的月累计时间、月累计电能量  月冻结"},
    {CMD_AFN_D_F66_GRUP_POWR_OVER,   MT_DIR_M2S, MT_PN_GT,    emtTrans_td_m,        "总加组超月电能量定值的月累计时间、累计电能量 月冻结"},

    // 组10 总加组曲线         pn:总加组号    
    {CMD_AFN_D_F73_CURVE_RATE_HAVE,  MT_DIR_M2S, MT_PN_GT,    emtTrans_td_c,        "总加组有功功率曲线"},
    {CMD_AFN_D_F74_CURVE_RATE_NONE,  MT_DIR_M2S, MT_PN_GT,    emtTrans_td_c,        "总加组无功功率曲线"},
    {CMD_AFN_D_F75_CURVE_POWR_HAVE,  MT_DIR_M2S, MT_PN_GT,    emtTrans_td_c,        "总加组有功电能量曲线"},
    {CMD_AFN_D_F76_CURVE_POWR_NONE,  MT_DIR_M2S, MT_PN_GT,    emtTrans_td_c,        "总加组无功电能量曲线"},

    // 组11  功率曲线 pn:测量点号      
    {CMD_AFN_D_F81_CURVE_HAVE,       MT_DIR_M2S, MT_PN_MP,    emtTrans_td_c,        "有功功率曲线"},
    {CMD_AFN_D_F82_CURVE_HAVE_A,     MT_DIR_M2S, MT_PN_MP,    emtTrans_td_c,        "A相有功功率曲线"},
    {CMD_AFN_D_F83_CURVE_HAVE_B,     MT_DIR_M2S, MT_PN_MP,    emtTrans_td_c,        "B相有功功率曲线"},
    {CMD_AFN_D_F84_CURVE_HAVE_C,     MT_DIR_M2S, MT_PN_MP,    emtTrans_td_c,        "C相有功功率曲线"},
    {CMD_AFN_D_F85_CURVE_NONE,       MT_DIR_M2S, MT_PN_MP,    emtTrans_td_c,        "无功功率曲线"},
    {CMD_AFN_D_F86_CURVE_NONE_A,     MT_DIR_M2S, MT_PN_MP,    emtTrans_td_c,        "A相无功功率曲线"},
    {CMD_AFN_D_F87_CURVE_NONE_B,     MT_DIR_M2S, MT_PN_MP,    emtTrans_td_c,        "B相无功功率曲线"},
    {CMD_AFN_D_F88_CURVE_NONE_C,     MT_DIR_M2S, MT_PN_MP,    emtTrans_td_c,        "C相无功功率曲线"},

    // 组12 电压电流曲线 pn:测量点号
    {CMD_AFN_D_F89_CURVE_VOLT_A,     MT_DIR_M2S, MT_PN_MP,    emtTrans_td_c,        "A相电压曲线"},
    {CMD_AFN_D_F90_CURVE_VOLT_B,     MT_DIR_M2S, MT_PN_MP,    emtTrans_td_c,        "B相电压曲线"},
    {CMD_AFN_D_F91_CURVE_VOLT_C,     MT_DIR_M2S, MT_PN_MP,    emtTrans_td_c,        "C相电压曲线"},
    {CMD_AFN_D_F92_CURVE_ELEC_A,     MT_DIR_M2S, MT_PN_MP,    emtTrans_td_c,        "A相电流曲线"},
    {CMD_AFN_D_F93_CURVE_ELEC_B,     MT_DIR_M2S, MT_PN_MP,    emtTrans_td_c,        "B相电流曲线"},
    {CMD_AFN_D_F94_CURVE_ELEC_C,     MT_DIR_M2S, MT_PN_MP,    emtTrans_td_c,        "C相电流曲线"},
    {CMD_AFN_D_F95_CURVE_ZERO_E,     MT_DIR_M2S, MT_PN_MP,    emtTrans_td_c,        "零序电流曲线"},

    // 组13 总电能量、总电能示值曲线 pn:测量点号
    {CMD_AFN_D_F97_CURVE_FRTH_HAVE,  MT_DIR_M2S, MT_PN_MP,    emtTrans_td_c,        "正向有功总电能量曲线"},
    {CMD_AFN_D_F98_CURVE_FRTH_NONE,  MT_DIR_M2S, MT_PN_MP,    emtTrans_td_c,        "正向无功总电能量曲线"},
    {CMD_AFN_D_F98_CURVE_BACK_HAVE,  MT_DIR_M2S, MT_PN_MP,    emtTrans_td_c,        "反向有功总电能量曲线"},
    {CMD_AFN_D_F100_CURVE_BACK_HAVE, MT_DIR_M2S, MT_PN_MP,    emtTrans_td_c,        "反向无功总电能量曲线"},
    {CMD_AFN_D_F101_CURVE_FRTH_HAVE, MT_DIR_M2S, MT_PN_MP,    emtTrans_td_c,        "正向有功总电能示值曲线"},
    {CMD_AFN_D_F102_CURVE_FRTH_NONE, MT_DIR_M2S, MT_PN_MP,    emtTrans_td_c,        "正向无功总电能示值曲线"},
    {CMD_AFN_D_F103_CURVE_BACK_HAVE, MT_DIR_M2S, MT_PN_MP,    emtTrans_td_c,        "反向有功总电能示值曲线"},
    {CMD_AFN_D_F104_CURVE_BACK_NONE, MT_DIR_M2S, MT_PN_MP,    emtTrans_td_c,        "反向无功总电能示值曲线"},

    // 组14 功率因数、电压相位角曲线、电流相位角曲线 pn:测量点号
    {CMD_AFN_D_F105_CURVE_FACTOR_T,  MT_DIR_M2S, MT_PN_MP,    emtTrans_td_c,        "总功率因数曲线"},
    {CMD_AFN_D_F106_CURVE_FACTOR_A,  MT_DIR_M2S, MT_PN_MP,    emtTrans_td_c,        "A相功率因数曲线"},
    {CMD_AFN_D_F107_CURVE_FACTOR_B,  MT_DIR_M2S, MT_PN_MP,    emtTrans_td_c,        "B相功率因数曲线"},
    {CMD_AFN_D_F108_CURVE_FACTOR_C,  MT_DIR_M2S, MT_PN_MP,    emtTrans_td_c,        "C相功率因数曲线"},
    {CMD_AFN_D_F109_ANGLE_CURVE_VOLT,MT_DIR_M2S, MT_PN_MP,    emtTrans_td_c,        "正向有功总电能示值曲线"},
    {CMD_AFN_D_F110_ANGLE_CURVE_ELEC,MT_DIR_M2S, MT_PN_MP,    emtTrans_td_c,        "正向无功总电能示值曲线"},

    // 组15 谐波监测统计数据 pn:测量点号
    {CMD_AFN_D_F113_ELEC_HARM_TIME_A,MT_DIR_M2S, MT_PN_MP,    emtTrans_td_d,        "A相2～19次谐波电流日最大值及发生时间 日冻结"},
    {CMD_AFN_D_F114_ELEC_HARM_TIME_B,MT_DIR_M2S, MT_PN_MP,    emtTrans_td_d,        "B相2～19次谐波电流日最大值及发生时间 日冻结"},
    {CMD_AFN_D_F115_ELEC_HARM_TIME_C,MT_DIR_M2S, MT_PN_MP,    emtTrans_td_d,        "C相2～19次谐波电流日最大值及发生时间 日冻结"},
    {CMD_AFN_D_F116_VOLT_HARM_TIME_A,MT_DIR_M2S, MT_PN_MP,    emtTrans_td_d,        "A相2～19次谐波电压含有率及总畸变率日最大值及发生时间 日冻结"},
    {CMD_AFN_D_F117_VOLT_HARM_TIME_B,MT_DIR_M2S, MT_PN_MP,    emtTrans_td_d,        "B相2～19次谐波电压含有率及总畸变率日最大值及发生时间 日冻结"},
    {CMD_AFN_D_F118_VOLT_HARM_TIME_C,MT_DIR_M2S, MT_PN_MP,    emtTrans_td_d,        "C相2～19次谐波电压含有率及总畸变率日最大值及发生时间 日冻结"},

    // 组16 谐波越限统计数据 pn:测量点号
    {CMD_AFN_D_F121_HARM_OVER_A,     MT_DIR_M2S, MT_PN_MP,    emtTrans_td_d,        "A相谐波越限日统计数据 日冻结"},
    {CMD_AFN_D_F122_HARM_OVER_B,     MT_DIR_M2S, MT_PN_MP,    emtTrans_td_d,        "B相谐波越限日统计数据 日冻结"},
    {CMD_AFN_D_F123_HARM_OVER_C,     MT_DIR_M2S, MT_PN_MP,    emtTrans_td_d,        "C相谐波越限日统计数据 日冻结"},

    // 组17 直流模拟量数据     pn:直流模拟量端口号
    {CMD_AFN_D_F129_DC_D,            MT_DIR_M2S, MT_PN_MP,    emtTrans_td_d,        "直流模拟量越限日累计时间、最大/最小值及发生时间 日冻结"},
    {CMD_AFN_D_F130_DC_M,            MT_DIR_M2S, MT_PN_MP,    emtTrans_td_m,        "直流模拟量越限月累计时间、最大/最小值及发生时间 月冻结"},

    // 组18 直流模拟量数据曲线 pn:测量点号
    {CMD_AFN_D_F138_DC_CURVE,        MT_DIR_M2S, MT_PN_MP,    emtTrans_td_c,        "直流模拟量数据曲线"},

    // 组19 四个象限无功总电能示值曲线 pn:测量点号
    {CMD_AFN_D_F145_CURVE_PHASE_1,   MT_DIR_M2S, MT_PN_MP,    emtTrans_td_c,        "一象限无功总电能示值曲线"},
    {CMD_AFN_D_F146_CURVE_PHASE_4,   MT_DIR_M2S, MT_PN_MP,    emtTrans_td_c,        "四象限无功总电能示值曲线"},
    {CMD_AFN_D_F147_CURVE_PHASE_2,   MT_DIR_M2S, MT_PN_MP,    emtTrans_td_c,        "二象限无功总电能示值曲线"},
    {CMD_AFN_D_F148_CURVE_PHASE_3,   MT_DIR_M2S, MT_PN_MP,    emtTrans_td_c,        "三象限无功总电能示值曲线"},

    // 组20 分相电能示值 pn:测量点号
    {CMD_AFN_D_F153_FRTH_HAVE_D,     MT_DIR_M2S, MT_PN_MP,    emtTrans_td_d,        "日冻结分相正向有功电能示值 日冻结"},
    {CMD_AFN_D_F154_FRTH_NONE_D,     MT_DIR_M2S, MT_PN_MP,    emtTrans_td_d,        "日冻结分相正向无功电能示值 日冻结"},
    {CMD_AFN_D_F155_BACK_HAVE_D,     MT_DIR_M2S, MT_PN_MP,    emtTrans_td_d,        "日冻结分相反向有功电能示值 日冻结"},
    {CMD_AFN_D_F156_BACK_NONE_D,     MT_DIR_M2S, MT_PN_MP,    emtTrans_td_d,        "日冻结分相反向无功电能示值 日冻结"},
    {CMD_AFN_D_F157_FRTH_HAVE_M,     MT_DIR_M2S, MT_PN_MP,    emtTrans_td_m,        "月冻结分相正向有功电能示值 月冻结"},
    {CMD_AFN_D_F158_FRTH_NONE_M,     MT_DIR_M2S, MT_PN_MP,    emtTrans_td_m,        "月冻结分相正向无功电能示值 月冻结"},
    {CMD_AFN_D_F159_BACK_HAVE_M,     MT_DIR_M2S, MT_PN_MP,    emtTrans_td_m,        "月冻结分相反向有功电能示值 月冻结"},
    {CMD_AFN_D_F160_BACK_NONE_M,     MT_DIR_M2S, MT_PN_MP,    emtTrans_td_m,        "月冻结分相反向无功电能示值 月冻结"},

    // 组21 pn:测量点号
    {CMD_AFN_D_F161_HAVE_FRTH_D,     MT_DIR_M2S, MT_PN_MP,    emtTrans_td_d,        "正向有功电能示值（总、费率1～M） 日冻结"},
    {CMD_AFN_D_F162_NONE_FRTM_D,     MT_DIR_M2S, MT_PN_MP,    emtTrans_td_d,        "正向无功（组合无功1）电能示值（总、费率1～M）日冻结"},
    {CMD_AFN_D_F163_HAVE_BACK_D,     MT_DIR_M2S, MT_PN_MP,    emtTrans_td_d,        "反向有功电能示值（总、费率1～M）日冻结"},
    {CMD_AFN_D_F164_NONE_BACK_D,     MT_DIR_M2S, MT_PN_MP,    emtTrans_td_d,        "反向无功（组合无功1）电能示值（总、费率1～M）日冻结"},
    {CMD_AFN_D_F165_NONE_D_PHASE_1,  MT_DIR_M2S, MT_PN_MP,    emtTrans_td_d,        "一象限无功电能示值（总、费率1～M）日冻结"},
    {CMD_AFN_D_F166_NONE_D_PHASE_2,  MT_DIR_M2S, MT_PN_MP,    emtTrans_td_d,        "二象限无功电能示值（总、费率1～M）日冻结"},
    {CMD_AFN_D_F167_NONE_D_PHASE_3,  MT_DIR_M2S, MT_PN_MP,    emtTrans_td_d,        "三象限无功电能示值（总、费率1～M）日冻结 月冻结"},
    {CMD_AFN_D_F168_NONE_D_PHASE_4,  MT_DIR_M2S, MT_PN_MP,    emtTrans_td_d,        "四象限无功电能示值（总、费率1～M）日冻结 月冻结"},

    // 组22 pn:测量点号 抄表日冻结
    {CMD_AFN_D_F169_HAVE_FRTH_R,     MT_DIR_M2S, MT_PN_MP,    emtTrans_td_d,        "正向有功电能示值（总、费率1～M）  抄表日冻结"},
    {CMD_AFN_D_F170_NONE_FRTM_R,     MT_DIR_M2S, MT_PN_MP,    emtTrans_td_d,        "正向无功（组合无功1）电能示值（总、费率1～M） 抄表日冻结"},
    {CMD_AFN_D_F171_HAVE_BACK_R,     MT_DIR_M2S, MT_PN_MP,    emtTrans_td_d,        "反向有功电能示值（总、费率1～M）  抄表日冻结"},
    {CMD_AFN_D_F172_NONE_BACK_R,     MT_DIR_M2S, MT_PN_MP,    emtTrans_td_d,        "反向无功（组合无功1）电能示值（总、费率1～M）抄表日冻结"},
    {CMD_AFN_D_F173_NONE_R_PHASE_1,  MT_DIR_M2S, MT_PN_MP,    emtTrans_td_d,        "一象限无功电能示值（总、费率1～M）抄表日冻结"},
    {CMD_AFN_D_F174_NONE_R_PHASE_2,  MT_DIR_M2S, MT_PN_MP,    emtTrans_td_d,        "二象限无功电能示值（总、费率1～M）抄表日冻结"},
    {CMD_AFN_D_F175_NONE_R_PHASE_3,  MT_DIR_M2S, MT_PN_MP,    emtTrans_td_d,        "三象限无功电能示值（总、费率1～M）抄表日冻结"},
    {CMD_AFN_D_F176_NONE_R_PHASE_4,  MT_DIR_M2S, MT_PN_MP,    emtTrans_td_d,        "四象限无功电能示值（总、费率1～M）抄表日冻结"},

    // 组23 pn:测量点号
    {CMD_AFN_D_F177_HAVE_FRTH_M,     MT_DIR_M2S, MT_PN_MP,    emtTrans_td_m,        "正向有功电能示值（总、费率1～M） 月冻结"},
    {CMD_AFN_D_F178_NONE_FRTM_M,     MT_DIR_M2S, MT_PN_MP,    emtTrans_td_m,        "正向无功（组合无功1）电能示值（总、费率1～M）月冻结"},
    {CMD_AFN_D_F179_HAVE_BACK_M,     MT_DIR_M2S, MT_PN_MP,    emtTrans_td_m,        "反向有功电能示值（总、费率1～M） 月冻结"},
    {CMD_AFN_D_F180_NONE_BACK_M,     MT_DIR_M2S, MT_PN_MP,    emtTrans_td_m,        "反向无功（组合无功1）电能示值（总、费率1～M）月冻结"},
    {CMD_AFN_D_F181_NONE_M_PHASE_1,  MT_DIR_M2S, MT_PN_MP,    emtTrans_td_m,        "一象限无功电能示值（总、费率1～M） 月冻结"},
    {CMD_AFN_D_F182_NONE_M_PHASE_2,  MT_DIR_M2S, MT_PN_MP,    emtTrans_td_m,        "二象限无功电能示值（总、费率1～M） 月冻结"},
    {CMD_AFN_D_F183_NONE_M_PHASE_3,  MT_DIR_M2S, MT_PN_MP,    emtTrans_td_m,        "三象限无功电能示值（总、费率1～M） 月冻结"},
    {CMD_AFN_D_F184_NONE_M_PHASE_4,  MT_DIR_M2S, MT_PN_MP,    emtTrans_td_m,        "四象限无功电能示值（总、费率1～M） 月冻结"},

    // 组24 pn:测量点号
    {CMD_AFN_D_F185_HAVE_DMND_FRTH_D, MT_DIR_M2S, MT_PN_MP,   emtTrans_td_d,        "正向有功最大需量及发生时间（总、费率1～M）日冻结"},
    {CMD_AFN_D_F186_NONE_DMND_FRTH_D, MT_DIR_M2S, MT_PN_MP,   emtTrans_td_d,        "正向无功最大需量及发生时间（总、费率1～M）日冻结"},
    {CMD_AFN_D_F187_HAVE_DMND_BACK_D, MT_DIR_M2S, MT_PN_MP,   emtTrans_td_d,        "反向有功最大需量及发生时间（总、费率1～M）日冻结"},
    {CMD_AFN_D_F188_NONE_DMND_BACK_D, MT_DIR_M2S, MT_PN_MP,   emtTrans_td_d,        "反向无功最大需量及发生时间（总、费率1～M）日冻结"},
    {CMD_AFN_D_F189_HAVE_DMND_FRTH_R, MT_DIR_M2S, MT_PN_MP,   emtTrans_td_d,        "正向有功最大需量及发生时间（总、费率1～M）抄表日冻结"},
    {CMD_AFN_D_F190_NONE_DMND_FRTH_R, MT_DIR_M2S, MT_PN_MP,   emtTrans_td_d,        "正向无功最大需量及发生时间（总、费率1～M）抄表日冻结"},
    {CMD_AFN_D_F191_HAVE_DMND_BACK_R, MT_DIR_M2S, MT_PN_MP,   emtTrans_td_d,        "反向有功最大需量及发生时间（总、费率1～M）抄表日冻结"},
    {CMD_AFN_D_F192_NONE_DMND_BACK_R, MT_DIR_M2S, MT_PN_MP,   emtTrans_td_d,        "反向无功最大需量及发生时间（总、费率1～M）抄表日冻结"},

    // 组25 pn:测量点号
    {CMD_AFN_D_F193_HAVE_DMND_FRTH_M,  MT_DIR_M2S, MT_PN_MP,  emtTrans_td_m,        "正向有功最大需量及发生时间（总、费率1～M） 月冻结"},
    {CMD_AFN_D_F194_NONE_DMND_FRTH_M,  MT_DIR_M2S, MT_PN_MP,  emtTrans_td_m,        "正向无功最大需量及发生时间（总、费率1～M） 月冻结"},
    {CMD_AFN_D_F195_HAVE_DMND_BACK_M,  MT_DIR_M2S, MT_PN_MP,  emtTrans_td_m,        "反向有功最大需量及发生时间（总、费率1～M） 月冻结"},
    {CMD_AFN_D_F196_NONE_DMND_BACK_M,  MT_DIR_M2S, MT_PN_MP,  emtTrans_td_m,        "反向无功最大需量及发生时间（总、费率1～M） 月冻结"},

    // 组26 pn:测量点号
    {CMD_AFN_D_F201_FREZ_ZONE_1,       MT_DIR_M2S, MT_PN_MP,  emtTrans_td_m,        "第一时区冻结正向有功电能示值（总、费率1～M）"},
    {CMD_AFN_D_F202_FREZ_ZONE_2,       MT_DIR_M2S, MT_PN_MP,  emtTrans_td_m,        "第二时区冻结正向有功电能示值（总、费率1～M）"},
    {CMD_AFN_D_F203_FREZ_ZONE_3,       MT_DIR_M2S, MT_PN_MP,  emtTrans_td_m,        "第三时区冻结正向有功电能示值（总、费率1～M）"},
    {CMD_AFN_D_F204_FREZ_ZONE_4,       MT_DIR_M2S, MT_PN_MP,  emtTrans_td_m,        "第四时区冻结正向有功电能示值（总、费率1～M）"},
    {CMD_AFN_D_F205_FREZ_ZONE_5,       MT_DIR_M2S, MT_PN_MP,  emtTrans_td_m,        "第五时区冻结正向有功电能示值（总、费率1～M）"},
    {CMD_AFN_D_F206_FREZ_ZONE_6,       MT_DIR_M2S, MT_PN_MP,  emtTrans_td_m,        "第六时区冻结正向有功电能示值（总、费率1～M）"},
    {CMD_AFN_D_F207_FREZ_ZONE_7,       MT_DIR_M2S, MT_PN_MP,  emtTrans_td_m,        "第七时区冻结正向有功电能示值（总、费率1～M）"},
    {CMD_AFN_D_F208_FREZ_ZONE_8,       MT_DIR_M2S, MT_PN_MP,  emtTrans_td_m,        "第八时区冻结正向有功电能示值（总、费率1～M）"},

    // 组27 pn:测量点号
    {CMD_AFN_D_F209_METR_REMOTE_INFO,  MT_DIR_M2S, MT_PN_MP,  emtTrans_td_d,        "电能表远程控制通断电状态及记录"},
    {CMD_AFN_D_F213_METR_SWITCH_INFO,  MT_DIR_M2S, MT_PN_MP,  emtTrans_td_m,        "电能表开关操作次数及时间"},
    {CMD_AFN_D_F214_METR_MODIFY_INFO,  MT_DIR_M2S, MT_PN_MP,  emtTrans_td_m,        "电能表参数修改次数及时间"},
    {CMD_AFN_D_F215_METR_BUY_USR,      MT_DIR_M2S, MT_PN_MP,  emtTrans_td_m,        "电能表购、用电信息"},
    {CMD_AFN_D_F216_METR_BALANCE,      MT_DIR_M2S, MT_PN_MP,  emtTrans_td_m,        "电能表结算信息"},

    // 组28 pn:测量点号
    {CMD_AFN_D_F217_WHITE_YAWP_CURVE,  MT_DIR_M2S, MT_PN_MP,  emtTrans_td_c,        "台区集中抄表载波主节点白噪声曲线"},
    {CMD_AFN_D_F218_COLOR_YAWP_CURVE,  MT_DIR_M2S, MT_PN_MP,  emtTrans_td_c,        "台区集中抄表载波主节点色噪声曲线"},
    /** } 下行 **/ 
    ///*}

     /*******************************
     *  (13) 请求3类数据(AFN=0EH）
     *
     *  上行: 本类型
     *  下行: 本类型
     *  加密: 不需要  
     *  
    {*///
    // 上行
    {CMD_AFN_E_F1_EVENT_1,             MT_DIR_S2M, MT_PN_P0,  NULL,                  "请求重要事件"},
    {CMD_AFN_E_F2_EVENT_2,             MT_DIR_S2M, MT_PN_P0,  NULL,                  "请求一般事件"},
    // 下行
    {CMD_AFN_E_F1_EVENT_1,             MT_DIR_M2S, MT_PN_P0,  NULL,                  "请求重要事件"},
    {CMD_AFN_E_F2_EVENT_2,             MT_DIR_M2S, MT_PN_P0,  NULL,                  "请求一般事件"},
    ///*}

     /*******************************
     *  (14) 文件传输(AFN=0FH）
     *
     *  上行: 本类型
     *  下行: 本类型
     *  加密: 不需要  
     *  
    {*///
    // 上行
    {CMD_AFN_F_F1_TRANS_WAY,           MT_DIR_S2M, MT_PN_P0,  NULL,                  "文件传输方式1"},
    // 下行
    {CMD_AFN_F_F1_TRANS_WAY,           MT_DIR_M2S, MT_PN_P0,  NULL,                  "文件传输方式1"},
    ///*}

     /*******************************
     *  (15) 数据转发(AFN=11H）
     *
     *  上行: 本类型
     *  下行: 本类型
     *  加密: 不需要  
     *  
    {*///
    // 上行
    {CMD_AFN_10_F1_TRANSMIT,            MT_DIR_S2M, MT_PN_P0,  NULL,                  "透明转发"},
    {CMD_AFN_10_F9_TRANS_READ,          MT_DIR_S2M, MT_PN_P0,  NULL,                  "转发主站直接对终端的抄读数据命令"},
    {CMD_AFN_10_F10_TRANS_SWITCH,       MT_DIR_S2M, MT_PN_P0,  NULL,                  "转发主站直接对终端的遥控跳闸/允许合闸命令"},
    {CMD_AFN_10_F11_TRANS_POWER,        MT_DIR_S2M, MT_PN_P0,  NULL,                  "转发主站直接对终端的遥控送电命令"},
    // 下行
    {CMD_AFN_10_F1_TRANSMIT,            MT_DIR_M2S, MT_PN_P0,  NULL,                  "透明转发"},
    {CMD_AFN_10_F9_TRANS_READ,          MT_DIR_M2S, MT_PN_P0,  NULL,                  "转发主站直接对终端的抄读数据命令"},
    {CMD_AFN_10_F10_TRANS_SWITCH,       MT_DIR_M2S, MT_PN_P0,  NULL,                  "转发主站直接对终端的遥控跳闸/允许合闸命令"},
    {CMD_AFN_10_F11_TRANS_POWER,        MT_DIR_M2S, MT_PN_P0,  NULL,                  "转发主站直接对终端的遥控送电命令"},
    ///*}


     /*******************************
     *  (16) 最大命令 
     *   
     *  在此之上扩展
     *  加密: 不需要  
     *  
    {*///
    {CMD_AFN_FN_MAX,            MT_DIR_UNKOWN, MT_PN_P0,  NULL,                  "命令最大值"}
    ///*}
};
////*}

/*****************************************************************************
 函 数 名  : eMtInit
 功能描述  : 协议初始化
 输入参数  : sMtInit* sInit  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月2日 星期五
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr eMtInit(sMtInit* psInit)
{
    if(g_bMtInit == TRUE)
    {
        return MT_OK;
    }

    if(!psInit)
    {
        #ifdef MT_DBG
        DEBUG("eMtInit() pointer is null!");
        #endif
        return MT_ERR_NULL;
    }

    if(MT_ROLE_MASTER != psInit->eRole && MT_ROLE_CONTOR != psInit->eRole)
    {
        #ifdef MT_DBG
        DEBUG("eMtInit() para error!");
        #endif
        return MT_ERR_PARA;
    }

    g_ucMtPermitDelayMinutes = psInit->ucPermitDelayMinutes;
    g_eMtRole = psInit->eRole;

    #if MT_CFG_SPT_MST
    if(psInit->eRole == MT_ROLE_CONTOR)
    {
        g_tEC.ucEC1 = 0;
        g_tEC.ucEC2 = 0;
    }
    #endif

    //pw
    memcpy(g_aucPw,psInit->aucPw, MT_PW_LEN);

    // 加密与解密算法 
#if MT_CFG_ENCRYPT
    g_peMtEncryptFunc = psInit->EncryptFunc;  // 加密接口
    g_peMtDecryptFunc = psInit->DecryptFunc;  // 解密接口
#endif
    
    g_bMtInit = TRUE;
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : bmt_is_0xEE
 功能描述  : 判断usLen长的pData数据内容是否都是0xEE 
 输入参数  : UINT8* pData  
             UINT16 usLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月12日 星期一
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
BOOL   bmt_is_0xEE(UINT8* pData, UINT16 usLen)
{
    if(!pData)
    {
        return FALSE;
    }
    
    INT32 i = 0;
    for(i = 0; i < usLen; i++)
    {
        if(0xEE != pData[i])
        {
            return FALSE;
        }
    }

    return TRUE;
}

/*****************************************************************************
 函 数 名  : vmt_set_0xEE
 功能描述  : 将usLen的数据pData的内容都设置为0xEE
 输入参数  : UINT8* pData  
             UINT16 usLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月12日 星期一
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
void   vmt_set_0xEE(UINT8* pData, UINT16 usLen) 
{
    INT32 i = 0;
    
    if(!pData)
    {
        return;
    }
    
    for(i = 0; i < usLen; i++)
    {
        pData[i] = 0xEE;
    }
}

/*****************************************************************************
 函 数 名  : bmt_is_none
 功能描述  : 是否一个数据项为缺省
 输入参数  : UINT8* pData  
             UINT16 usLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月12日 星期一
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
BOOL   bmt_is_none(UINT8* pData, UINT16 usLen)
{
    return bmt_is_0xEE(pData, usLen);
}

/*****************************************************************************
 函 数 名  : vmt_set_none
 功能描述  : 将某数据项设置为缺省
 输入参数  : UINT8* pData  
             UINT16 usLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月12日 星期一
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
void   vmt_set_none(UINT8* pData, UINT16 usLen)
{
    vmt_set_0xEE(pData, usLen);
}

/*****************************************************************************
 函 数 名  : eMtGetCmdInfor
 功能描述  : 通过命令类型和报文方向获得该命令对应的相关信息
 输入参数  : eMTmd eCmd          
             eMTDir eDir         
             sMTmdInfo *psInfor  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年7月29日 星期一
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr eMtGetCmdInfor(eMtCmd eCmd, eMtDir eDir, sMtCmdInfor *psInfor)
{
    INT32 i   = 0;
    INT32 Num = 0;

    if(!psInfor)
    {
        #ifdef MT_DBG
        DEBUG("eMtGetCmdInfor() pointer is null!");
        #endif
        return MT_ERR_NULL;
    }

    #ifdef MT_DBG
    // DEBUG("[in]eMtGetCmdInfor() eCmd = 0x%X", eCmd);
    // DEBUG("[in]eMtGetCmdInfor() eDir = %s", eDir == MT_DIR_M2S ? "MT_DIR_M2S" : "MT_DIR_S2M");
    #endif
    
    Num = sizeof(gmt_cmdinfor) / sizeof(sMtCmdInfor);

    for(i = 0; i < Num; i++)
    {
        if(gmt_cmdinfor[i].eCmd == eCmd && gmt_cmdinfor[i].eDir == eDir)
        {
            psInfor->eCmd  = eCmd;
            psInfor->eDir  = eDir;
            psInfor->ePn   = gmt_cmdinfor[i].ePn;
            psInfor->pFunc = gmt_cmdinfor[i].pFunc;
            psInfor->pName = gmt_cmdinfor[i].pName;
            return MT_OK;
        }
    }

    #ifdef MT_DBG
   // DEBUG("eMtGetCmdInfor() MT_ERR_NONE!");
    #endif
    return MT_ERR_NONE;
}

/*****************************************************************************
 函 数 名  : emtGetPrm
 功能描述  : 获得某类型的报文的主动性
 输入参数  : eMtDir eDir  
             eMtAFN eAfn  
             BOOL bAuto   
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月5日 星期四
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtPRM emtGetPrm(eMtDir eDir, eMtAFN eAfn, BOOL bAuto)
{
    eMtPRM ePrm;
	
	switch(eAfn)
	{
    	case AFN_00_CONF:
    		ePrm = MT_PRM_PASIVE;
    		break;
    	
    	case AFN_01_RSET:
    		if (eDir == MT_DIR_S2M)
    		{
    			ePrm = MT_PRM_UNKWON;
    		}
    		else
    		{
    		  ePrm = MT_PRM_ACTIVE;
    		}
    	
    		break;
    	case AFN_02_LINK:
    		if (eDir == MT_DIR_S2M)
    		{
    			ePrm = MT_PRM_ACTIVE;
    		}
    		else
    		{
    			ePrm = MT_PRM_UNKWON;
    		}
    		
    		break;
    	case AFN_03_RELY:
    		ePrm = MT_PRM_PASIVE;
    		break;
    	case AFN_04_SETP:
    		if (eDir == MT_DIR_S2M)
    		{
    			ePrm = MT_PRM_UNKWON;
    		}
    		else
    		{
    			ePrm = MT_PRM_ACTIVE;
    		}
    		break;
    	case AFN_05_CTRL:
    		if (eDir == MT_DIR_S2M)
    		{
    			ePrm = MT_PRM_UNKWON;
    		}
    		else
    		{
    			ePrm = MT_PRM_ACTIVE;
    		}
    		break;
    	case AFN_06_AUTH: 

    		ePrm = MT_PRM_UNKWON;
    		break;
    	case AFN_09_CFIG:
    		if (eDir == MT_DIR_S2M)
    		{
    			ePrm = MT_PRM_UNKWON;
    		}
    		else
    		{
    			ePrm = MT_PRM_ACTIVE;
    		}
    		break;

    	case AFN_0A_GETP:
    		if (eDir == MT_DIR_S2M)
    		{
    			ePrm = MT_PRM_PASIVE;
    		}
    		else
    		{
    			ePrm = MT_PRM_ACTIVE;
    		}
    		break;


    	case AFN_0B_ASKT:
    		if (eDir == MT_DIR_S2M)
    		{
    			ePrm = MT_PRM_PASIVE;
    		}
    		else
    		{
    			ePrm = MT_PRM_ACTIVE;
    		}
    		break;


    	case AFN_0C_ASK1:
    		if (eDir == MT_DIR_S2M)
    		{
    			ePrm = MT_PRM_PASIVE;
    		}
    		else
    		{
    			ePrm = MT_PRM_ACTIVE;
    		}
    		break;

    	case AFN_0D_ASK2:
    		if (eDir == MT_DIR_S2M)
    		{
    			ePrm = MT_PRM_PASIVE;
    		}
    		else
    		{
    			ePrm = MT_PRM_ACTIVE;
    		}
    		break;

    	case AFN_0E_ASK3:
    		if (eDir == MT_DIR_S2M)
    		{
    			ePrm = MT_PRM_PASIVE;
    		}
    		else
    		{
    			ePrm = MT_PRM_ACTIVE;
    		}
    		break;


    	default:
    			ePrm = MT_PRM_UNKWON;
    		break;
	}

	if (eDir == MT_DIR_S2M && bAuto)
	{
        ePrm = MT_PRM_ACTIVE;
	}

	return ePrm;
}
/*****************************************************************************
 函 数 名  : emtIsValidPack
 功能描述  : 判断一个帧是否是一个有效的3761.1的报文
             判断一个以0x68 开头以0x16结尾的一段buffer是否是一个完整有效的376.1报文
 输入参数  : UINT8  *pOutBuf  
             UINT16 usLen     
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月9日 星期五
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtIsValidPack(const UINT8* pOutBuf, UINT16 usLen)
{
    if(!pOutBuf)
    {
       #ifdef MT_DBG
       DEBUG("emtIsValidPack() pointer is null!");
       #endif
       return MT_ERR_NULL;
    }

    UINT8  ucCheckSumP = 0;    
    UINT8  ucCheckSumC = 0;   
    UINT8  uc0x16      = 0;
    UINT16 usProtoLen  = 0; //实际应该的协议数据长度
    UINT16 usUserLen   = 0;     

    sMtfComHead *pfComHead = NULL;
    pfComHead = (sMtfComHead *)pOutBuf;

    if(0x68 != pfComHead->f68 || 0x68 != pfComHead->s68)
    {
        return MT_ERR_0x68;
    }

    if(2 != pfComHead->p10)
    {
        return MT_ERR_PROTO;
    }

    usUserLen =  ((pfComHead->L2 << 6) & 0x3FC0 ) | (pfComHead->L1 & 0x003F); 

    // 帧中的实现校验和
    ucCheckSumP =  *(UINT8*)((UINT8*)&(pfComHead->C) + usUserLen);
   
    // 计算出来的校验和
    ucCheckSumC = ucmt_get_check_sum((UINT8*)&(pfComHead->C), usUserLen);
    
    if(ucCheckSumC != ucCheckSumP)
    {
        return MT_ERR_CS;
    }

    // 协议应该的数据长度
    usProtoLen = usUserLen + MT_UN_USER_LEN;
    if(usLen < usProtoLen)
    {   
        return MT_ERR_UNCOMP;
    }

    uc0x16 = *(UINT8*)((UINT8*)&(pfComHead->C) + usUserLen + 1);

    if(uc0x16 != 0x16)
    {
        return MT_ERR_0x16;
    }

    return MT_OK;
}

/*****************************************************************************
 函 数 名  : vmtSetEC
 功能描述  : 设置事件计数器
 输入参数  : UINT8 ucEC1  
             UINT8 ucEC2  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月9日 星期五
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
void  vmtSetEC(UINT8 ucEC1, UINT8 ucEC2)
{
    g_tEC.ucEC1 = ucEC1;
    g_tEC.ucEC2 = ucEC2;
}  

/*****************************************************************************
 函 数 名  : vmtSetPw
 功能描述  : 以字符串方式设置登录密码
             如果长度不足16, 后边自动以'0'补足
 输入参数  : char *pPw  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月9日 星期五
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
void  vmtSetPw(char *pPw)
{
    if(!pPw)
    {
        return;
    }

    memset((void*)g_aucPw, '0', MT_PW_LEN);
    int len = 0, lenLeft = 0;
    len = strlen(pPw);

    if(len >= MT_PW_LEN)
    {
        memcpy((void*)g_aucPw, (void*)pPw, MT_PW_LEN); 
    }
    else
    {
        lenLeft =  MT_PW_LEN - len;
        memcpy((void*)((UINT8*)g_aucPw + lenLeft), (void*)pPw, len); 
    }
}

/*****************************************************************************
 函 数 名  : vmtSetPwBuf
 功能描述  : 以buffer的方式设置PW
 输入参数  : UINT8* buf  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月9日 星期五
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
void vmtSetPwBuf(UINT8* buf)
{
    if(!buf)
    {
        return;
    }

    memcpy((void*)g_aucPw, (void*)buf, MT_PW_LEN); 
}

/*****************************************************************************
 函 数 名  : emtWhoAmI
 功能描述  : 获得当前使用该接口的身份，主站还是从站
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月9日 星期五
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtRole emtWhoAmI()
{
    return g_eMtRole; 
}

/*****************************************************************************
 函 数 名  : emtFindValidPack
 功能描述  : 从帧缓冲区中找到第一个有效的帧的位置及长度
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月9日 星期五
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtFindValidPack(UINT8* pinBuf, UINT16 usLen, UINT16* pusFirstOff, UINT16* pusFirstLen)
{
    if(!pinBuf || !pusFirstOff || !pusFirstLen)
    {
        #ifdef MT_DBG
        DEBUG("emtFindValidPack() pointer is null!");
        #endif
        return MT_ERR_NULL;
    }
    
    eMtErr eRet = MT_OK;
    int i = 0, j = 0;
    UINT16 usValidLen = 0;

    for(i = 0; i < usLen; i++)
    {
        if(0x68 == pinBuf[i])
        {   
            // 判断下两个字节位置是不是0x68 usLen
            if(0x68 == pinBuf[i+5])
             {
                for(j = i+6; j < usLen; j++)
                {
                    if(pinBuf[j] == 0x16)
                    {    
                        usValidLen = j + 1;
                        eRet = emtIsValidPack((UINT8*)(pinBuf + i), usValidLen);

                        if(MT_OK == eRet)
                        {
                            *pusFirstOff = i;
                            *pusFirstLen = usValidLen;
                            return MT_OK;
                        }
                    }
                }

                continue;
            }
        }
    }
    
    return MT_ERR_NONE;
}

/*****************************************************************************
 函 数 名  : bmt_have_ec
 功能描述  : 此类报文中是否应该含有ec字段
 输入参数  : eMtAFN eAFN 
             eMtDir eDir  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年2月27日
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
BOOL   bmt_have_ec(eMtAFN eAFN, eMtDir eDir)
{   
    if((AFN_00_CONF == eAFN) ||    // 确认否认报文 上下行都有EC
       (AFN_02_LINK != eAFN  &&
        AFN_06_AUTH != eAFN  &&
        MT_DIR_S2M == eDir))       // 除了链路检测外的所有上行报文都有EC
    {
        return TRUE;
    }

    return FALSE;
}

/*****************************************************************************
 函 数 名  : bmt_have_pw
 功能描述  : 此报文中是否应该还有pw字段
 输入参数  : eMtAFN eAFN    
             eMtDir eDir 
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年2月27日
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
BOOL   bmt_have_pw(eMtAFN eAFN, eMtDir eDir)
{
    if(((MT_DIR_M2S == eDir)  &&
        (AFN_01_RSET == eAFN  ||
         AFN_04_SETP == eAFN  || 
         AFN_05_CTRL == eAFN  ||
         AFN_10_DATA == eAFN  ||
         AFN_0F_FILE == eAFN))||        // PW字段只出现在下行报文中
        (AFN_06_AUTH == eAFN))           // 双向身份认证报文中都有PW
    {
        return TRUE;
    }

    return FALSE;
}

/*****************************************************************************
 函 数 名  : bmt_have_tp
 功能描述  : 此报文(从 eRole 发送的报文)中是否应该还有tp字段
 输入参数  : eMtAFN eAFN    
             eMtDir eDir 
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年2月27日
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
BOOL  bmt_have_tp(eMtAFN eAFN, eMtDir eDir)
{
    if(AFN_02_LINK == eAFN &&  MT_DIR_S2M == eDir)  // 除了这种情况的所有报文中
    {
         return FALSE;
    }

    if(MT_DIR_M2S == eDir && AFN_00_CONF == eAFN)
    {
        return FALSE;
    }

    if(MT_DIR_M2S == eDir && AFN_04_SETP == eAFN)
    {
        return FALSE;
    }
    
    return TRUE;
}

/*****************************************************************************
 函 数 名  : bmt_need_con
 功能描述  : 此报文是否需要确认
 输入参数  : eMtAFN eAFN  
             eMtDir eDir  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年4月23日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
BOOL   bmt_need_con(eMtAFN eAFN, eMtDir eDir)
{
  // 该报文是否需要从动站确认
    if((MT_DIR_M2S == eDir) &&
       (AFN_01_RSET == eAFN || 
        AFN_02_LINK == eAFN || 
        AFN_04_SETP == eAFN ||
        AFN_05_CTRL == eAFN))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }  
}

/*****************************************************************************
 函 数 名  : bmt_is_p0
 功能描述  : 判断一组Pn是否都为0, 即P0
 输入参数  : UINT16 *pUsPn8  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月6日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
BOOL bmt_is_p0(UINT16 *pUsPn8)
{
    if(!pUsPn8)
    {
        return FALSE;
    }

    INT32 i = 0;
    for(i = 0; i < PN_INDEX_MAX; i++)
    {
        if(pUsPn8[i] != 0)
        {
            return FALSE;
        }
    }

    return TRUE;
}

/*****************************************************************************
 函 数 名  : ucmt_get_pn_team
 功能描述  : 获得1个Pn对应的信息点组号
 输入参数  : UINT16 usPn  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月9日 星期五
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
UINT8 ucmt_get_pn_team(UINT16 usPn)
{
    UINT8 ucTeam;
    
    if(0 == usPn)
    {
        return 0;
    }

    ucTeam = (usPn - 1)/8 + 1;
    return ucTeam;
}

/*****************************************************************************
 函 数 名  : ucmt_get_pn8_team
 功能描述  : 获得8个Pn对应的信息点组号
 输入参数  : UINT16 *pusPn  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月9日 星期五
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
UINT8 ucmt_get_pn8_team(UINT16 *pusPn)
{
    if(!pusPn)
    {
        return 0xFF;
    }

    BOOL bP0;
    bP0 = bmt_is_p0(pusPn);
    if(TRUE == bP0)
    {
        return 0;
    }

    int i = 0;
    UINT8 ucTeam;
    for(i = 0; i < 8; i++)
    {
        if(pusPn[i] != MT_PN_NONE)
        {
           ucTeam  = ucmt_get_pn_team(pusPn[i]);
           return ucTeam;
        }
    }

    return 0xFF;
}

/*****************************************************************************
 函 数 名  : bmt_in_pn8
 功能描述  : 判断某一个pn是否在一组8个pn中
 输入参数  : UINT16 usPn     
             UINT16 *pusPn8  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月17日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
BOOL bmt_in_pn8(UINT16 usPn, UINT16 *pusPn8)
{
    if((!pusPn8) || (usPn > MT_PN_MAX))
    {
        return FALSE;
    }

    INT32 i = 0;
    
    for(i = 0; i < 8; i++)
    {
        if(usPn == pusPn8[i])
        {
            return TRUE;
        }
    }

    return FALSE;
}

/*****************************************************************************
 函 数 名  : ucGetCmdFn
 功能描述  : 获得命令的FN
 输入参数  : eMtCmd eCmd  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月8日 星期四
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
UINT8  ucGetCmdFn(eMtCmd eCmd)
{
    UINT8 ucFn;
    ucFn = (UINT8)( eCmd & 0xFF);
    return ucFn;
}

/*****************************************************************************
 函 数 名  : eGetCmdAfn
 功能描述  : 通过命令字获取命令对应的AFN
 输入参数  : eMtCmd eCmd  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年5月21日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtAFN eGetCmdAfn(eMtCmd eCmd)
{
    eMtAFN eAfn;
    UINT32 ulCmd = 0;
    UINT8  ucAfn = 0;
    ulCmd = (UINT32)eCmd;   
    ucAfn = (UINT8)((ulCmd & 0x0000FF00 ) >> 8);
    eAfn  = (eMtAFN)ucAfn;
    switch(eAfn)
    {
        case AFN_00_CONF:
        case AFN_01_RSET:
        case AFN_02_LINK:
        case AFN_03_RELY:
        case AFN_04_SETP:
        case AFN_05_CTRL:
        case AFN_06_AUTH:
        case AFN_08_CASC:
        case AFN_09_CFIG:
        case AFN_0A_GETP:
        case AFN_0B_ASKT:
        case AFN_0C_ASK1:
        case AFN_0D_ASK2:
        case AFN_0E_ASK3:
        case AFN_0F_FILE:
        case AFN_10_DATA:
            break;

        default:
            eAfn = AFN_NULL;
            break;
    }
   
    return eAfn;
}

/*****************************************************************************
 函 数 名  : bmt_same_team_pn
 功能描述  : 判断一个有8个元素的Pn数组,最多8个Pn是否属于同一个信息点组
             忽略无效Pn
             如果属于同一组,将组号取出
 输入参数  : UINT16 *pUsPn8  
 输出参数  : 无
 返 回 值  : 是同一组返回真
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月6日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
BOOL bmt_same_team_pn(UINT16 *pUsPn8, UINT8* pucDa2)
{
    if(!pUsPn8)
    {
        return FALSE;
    }

    UINT16 usPn[8]   = {0};
    UINT8  ucTeam[8] = {0}; // 每个Pn对应的信息点组
    INT32  nPnNum    = 0;   // 有效Pn的个数
    INT32  i         = 0;
    BOOL   bP0       = FALSE;

    // 判断是否都是P0
    bP0 = bmt_is_p0(pUsPn8);
    if(TRUE == bP0)
    {
        if(pucDa2 != NULL)
        {
           *pucDa2 = 0;
        }
        
        return TRUE;
    }

    for(i = 0; i < 8; i++)
    {
        if(MT_PN_NONE == pUsPn8[i])
        {
            // 合法,但是无效的值
        }
        else if(pUsPn8[i] < MT_PN_MIN || pUsPn8[i] > MT_PN_MAX)
        {
            #ifdef MT_DBG
            DEBUG("bmt_same_team_pn() Pn para err!");
            #endif
            return FALSE;
        }
        // 再有一个为0则参数错误
        else if(0 == pUsPn8[i])
        {
            return FALSE;
        }
        else
        {
            usPn[nPnNum++] = pUsPn8[i];
        }
    }

    if(0 == nPnNum)
    {
        // 都是无效的值,这种情况没有意义,也定为不是一组
        return FALSE;
    }
    else if(1 == nPnNum)
    {
        //*pucTeam = (usPN[0] - 1) / 8  + 1;
        //return TRUE;
    }
    else
    {
        for(i = 0; i < nPnNum; i++)
        {
            ucTeam[i] = (usPn[i] - 1) / 8  + 1; 
        }
        
        for(i = 1; i < nPnNum; i++)
        {
            if(ucTeam[i] != ucTeam[0])
            {
                return FALSE;
            }
        }
    }

    if(pucDa2 != NULL)
    {
       *pucDa2 = (usPn[0] - 1) / 8  + 1;
    }
    
    return TRUE;
}

/*****************************************************************************
 函 数 名  : ucmt_get_fn_team
 功能描述  : 获得一个fn对应的信息类组
 输入参数  : UINT8 ucFn  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月9日 星期五
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
UINT8 ucmt_get_fn_team(UINT8 ucFn)
{
    UINT8 ucTeam = 0;
  
    // 判断是否是合法的Fn
    if(ucFn == MT_FN_NONE)
    {
        ucTeam = 0xFF;
    }
    else
    {
        if(ucFn > MT_FN_MAX || ucFn < MT_FN_MIN)
        {
            ucTeam = 0xFF;
            #ifdef MT_DBG
            DEBUG("ucmt_get_fn_team() Fn para err!");
            #endif
        }
        else
        {
            ucTeam = (ucFn - 1)/8 ;
        }
    }
    
    return ucTeam;  
}

/*****************************************************************************
 函 数 名  : ucmt_get_fn_bit
 功能描述  : 获得一个Fn对应某信息点组中的bit值
 输入参数  : UINT8 ucFn  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月12日 星期四
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
UINT8 ucmt_get_fn_bit(UINT8 ucFn)
{
    UINT8 ucBit = 0;
  
    // 判断是否是合法的Fn
    if(ucFn == MT_FN_NONE)
    {
        ucBit = 0xFF;
    }
    else
    {
        if(ucFn > MT_FN_MAX || ucFn < MT_FN_MIN)
        {
            ucBit = 0xFF;
            #ifdef MT_DBG
            DEBUG("ucmt_get_fn_bit() Fn para err ucFn = %d!", ucFn);
            #endif
        }
        else
        {
            ucBit = (0x01 << ((ucFn - 1) % 8));
        }
    }
    
    return ucBit;  
}

/*****************************************************************************
 函 数 名  : ucmt_get_fn8_team
 功能描述  : 获得8个fn对应的信息类组
 输入参数  : UINT8 *pucFn  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月9日 星期五
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
UINT8 ucmt_get_fn8_team(UINT8 *pucFn)
{
    if(!pucFn)
    {
        return 0xFF;
    }

    int i = 0; 
    UINT8 ucTeam = 0xFF;
    
    for(i = 0 ;i < 8; i++)
    {
        if(pucFn[i] != MT_FN_NONE)
        {
            ucTeam = ucmt_get_fn_team(pucFn[i]);
            break;
        }
    }

    return ucTeam;
}

/*****************************************************************************
 函 数 名  : bmt_in_fn8
 功能描述  : 判断某一个fn是否在一组8个fn中
 输入参数  : UINT8 ucFn     
             UINT8 *pucFn8  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月17日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
BOOL bmt_in_fn8(UINT8 ucFn, UINT8 *pucFn8)
{
    if((ucFn < MT_FN_MIN) || (!pucFn8) || (ucFn > MT_FN_MAX))
    {
        return FALSE;
    }

    INT32 i = 0;
    
    for(i = 0; i < 8; i++)
    {
        if(ucFn == pucFn8[i])
        {
            return TRUE;
        }
    }
    
    return FALSE;
}

/*****************************************************************************
 函 数 名  : bmt_same_team_fn
 功能描述  : 判断一个有8个元素的Fn数组,最多8个Fn是否属于同一个信息点组
             忽略无效Fn
             如果属于同一组,将组号取出
 输入参数  : UINT16 *pUcFn8  
 输出参数  : 无
 返 回 值  : 是同一组返回真
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月6日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
BOOL bmt_same_team_fn(UINT8  *pUcFn8, UINT8* pucDt2)
{
    if(!pUcFn8)
    {
        return FALSE;
    }
     
    UINT8  ucFn[8]   = {0};
    UINT8  ucTeam[8] = {0}; // 每个Pn对应的信息点组
    INT32  nFnNum    = 0;   // 有效Pn的个数
    INT32  i         = 0;

    #if 0
    #ifdef MT_DBG
    DEBUG("[in]bmt_same_team_fn()");
    for(i = 0; i < 8; i++)
    {
        printf("fn[%d] = %d\n", i, (UINT8)pUcFn8[i]);
    }
    #endif
    #endif
    
    for(i = 0; i < 8; i++)
    {
        if(MT_FN_NONE == pUcFn8[i])
        {
            // 合法,但是无效的值
        }
        else if(pUcFn8[i] < MT_FN_MIN || pUcFn8[i] > MT_FN_MAX)
        {
            #ifdef MT_DBG
            DEBUG("bmt_same_team_fn() Fn para err!");
            #endif
            return FALSE;
        }
        else
        {
            ucFn[nFnNum++] = pUcFn8[i];
        }
    }

    if(0 == nFnNum)
    {
        // 都是无效的值,这种情况没有意义,也定为不是一组
        return FALSE;
    }
    else if(1 == nFnNum)
    {
        //*pucTeam = (usPN[0] - 1) / 8  + 1;
        //return TRUE;
    }
    else
    {
        for(i = 0; i < nFnNum; i++)
        {
            ucTeam[i] = (ucFn[i] - 1) / 8 ; 
        }
        
        for(i = 1; i < nFnNum; i++)
        {
            if(ucTeam[i] != ucTeam[0])
            {
                return FALSE;
            }
        }
    }
    
    if(pucDt2 != NULL)
    { 
        *pucDt2 = (ucFn[0] - 1) / 8;
    }
    
    return TRUE;
}

/*****************************************************************************
 函 数 名  : emt_pnfn_to_dadt
 功能描述  : 数据单元标识转换函数
 输入参数  : sMtPnFn* psPnFn  
             sMtDaDt* psDaDt  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年7月30日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emt_pnfn_to_dadt(sMtPnFn* psPnFn, sMtDaDt* psDaDt)
{
    INT32  i           = 0;
    UINT8  ucDa1       = 0;
    UINT8  ucDa2       = 0;
    UINT8  ucDt1       = 0;
    UINT8  ucDt2       = 0;
    const UINT8 ucMask = 0x01;
    UINT8  ucShit      = 0;  // 位移个数
    BOOL   bSameTeam   = FALSE;
    BOOL   bPn0        = FALSE;

    // 参数检测
    if(!psPnFn || !psDaDt)
    {
        #ifdef MT_DBG
        DEBUG("emt_pnfn_to_dadt() para pointer is null !");
        #endif
        return MT_ERR_NULL;
    }

    // 判断Pn 是否是同一个组
    bSameTeam = bmt_same_team_pn(psPnFn->usPn, &ucDa2);
    if(FALSE == bSameTeam)
    {
        #ifdef MT_DBG
        DEBUG("emt_pnfn_to_dadt() Pn is not in a same team!");
        #endif
        return MT_ERR_TEAM;
    }

    // 判断Fn 是否是同一个组
    bSameTeam = bmt_same_team_fn(psPnFn->ucFn, &ucDt2);
    if(FALSE == bSameTeam)
    {
        #ifdef MT_DBG
        DEBUG("emt_pnfn_to_dadt() Fn is not in a same team!");
        #endif
        return MT_ERR_TEAM;
    }

    // 封装 Da1
    bPn0 = bmt_is_p0(psPnFn->usPn);
    if(TRUE == bPn0)
    {
         ucDa1 = 0;
    }
    else
    {
        for(i = 0; i < PN_INDEX_MAX; i++)
        {
            if(MT_PN_NONE != psPnFn->usPn[i])
            {
                ucShit = (psPnFn->usPn[i] - 1) % 8;
                ucDa1 |= (ucMask << ucShit);
            }
        }
    }

    // 封装Dt1
    for(i = 0; i < FN_INDEX_MAX; i++)
    {
        if(MT_FN_NONE != psPnFn->ucFn[i])
        {
            ucShit = (psPnFn->ucFn[i] - 1) % 8;
            ucDt1 |= (ucMask << ucShit);
        }
    }
    
    // 将合法的fn pn 封装成 DaDt
    psDaDt->ucDA1 = ucDa1;
    psDaDt->ucDA2 = ucDa2;
    psDaDt->ucDT1 = ucDt1;
    psDaDt->ucDT2 = ucDt2;

    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emt_dadt_to_pnfn
 功能描述  : 数据标识单元转换函数
 输入参数  : sMtDaDt* psDaDt  
             sMtPnFn* psPnFn  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年7月30日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emt_dadt_to_pnfn(sMtDaDt* psDaDt, sMtPnFn* psPnFn)
{
    INT32  i      = 0;
    UINT16 usPn   = 0;
    UINT8  ucFn   = 0;
    UINT8  ucMask = 0x01;
    UINT8  ucTmp  = 0;
    
    if(!psPnFn || !psDaDt)
    {
        #ifdef MT_DBG
        DEBUG("emt_dadt_to_pnfn() para pointer is null!");
        #endif
        return MT_ERR_NULL;
    }

    #if 0
    #ifdef MT_DBG
    DEBUG("[in]emt_dadt_to_pnfn()");
    DEBUG("ucDA1 = 0x%X", psDaDt->ucDA1);
    DEBUG("ucDA2 = 0x%X", psDaDt->ucDA2);
    DEBUG("ucDT1 = 0x%X", psDaDt->ucDT1);
    DEBUG("ucDT2 = 0x%X", psDaDt->ucDT2);
    #endif
    #endif 
    
    // 数据区初始化
    memset(psPnFn, 0x00, sizeof(sMtPnFn));
    
    // Fn
    if(psDaDt->ucDT2 > 30)
    {
        #ifdef MT_DBG
        DEBUG("emt_dadt_to_pnfn() para out of range! ucDT2 = %d", psDaDt->ucDT2);
        #endif
        return MT_ERR_OUTRNG;
    }
    else
    {
        for(i = 0; i < 8; i++)
        {
            ucTmp = ucMask << i;
            if(psDaDt->ucDT1 & ucTmp)
            {
                ucFn = ((psDaDt->ucDT2) * 8) + i + 1;
            }
            else
            {
                ucFn = MT_FN_NONE;
            }
            
            psPnFn->ucFn[i] = ucFn;   
        }
    }

    // P0
    if(0 == psDaDt->ucDA2)
    {
        if(0 == psDaDt->ucDA1)
        {
            for(i = 0; i < 8; i++)
            {
               psPnFn->usPn[i] = 0; 
            }
        }
        else
        {
           #ifdef MT_DBG
           DEBUG("emt_dadt_to_pnfn() para err!");
           #endif
           return MT_ERR_PARA;  
        }
    }
    // Pn
    else 
    {
        for(i = 0; i < 8; i++)
        {
            ucTmp = ucMask << i;
            if(psDaDt->ucDA1 & ucTmp)
            {
                usPn = (psDaDt->ucDA2-1) * 8 + i + 1;
            }
            else
            {
                usPn = MT_PN_NONE;
            }
            
            psPnFn->usPn[i] = usPn;   
        }
    }

    #if 0
    #ifdef MT_DBG
    DEBUG("[out]emt_dadt_to_pnfn()");
    BOOL bP0 = bmt_is_p0(psPnFn->usPn);
    if(bP0 == TRUE)
    {
         DEBUG("usPn = 0");     
    }
    else
    {
        for(i = 0; i < 8; i++)
        {
            if(psPnFn->usPn[i] == MT_PN_NONE)
            {
               DEBUG("Pn[%d] = NONE", i+1); 
            }
            else
            {
               DEBUG("Pn[%d] = %d", i+1, psPnFn->usPn[i]); 
            }
        }
    }

    for(i = 0; i < 8; i++)
    {
        if(psPnFn->ucFn[i] == MT_FN_NONE)
        {
            DEBUG("Fn[%d] = NONE", i+1);    
        }
        else
        {
            DEBUG("Fn[%d] = %d", i+1, psPnFn->ucFn[i]);    
        }
    }
    #endif
    #endif
    
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emt_add_cmdpn
 功能描述  : 向现*pucNumCmdPn个sMtCmdPn数组添加一个新的
             如果已经存在一个同样的命令与PN组,则不添加
             否则添加后最后,并计数自增
 输入参数  : sMtCmdPn* psCmdPn   
             UINT8 *pucNumCmdPn  
             sMtCmdPn sNewCmdPn
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月13日 星期五
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emt_add_cmdpn(sMtCmdPn* psCmdPn,sMtCmdPn sNewCmdPn, UINT8 *pucNumCmdPn)
{
    if(!psCmdPn || !pucNumCmdPn)
    {
        #ifdef MT_DBG
        DEBUG("emt_add_cmdpn() para pointer is null!");
        #endif
        return MT_ERR_NULL;
    }

    INT32 i   = 0;
    UINT8 ucN = 0;
    ucN = *pucNumCmdPn;

    for(i = 0; i < ucN; i++)
    {
        if(sNewCmdPn.eCmd == psCmdPn[i].eCmd)
        {
            if(sNewCmdPn.usPn == psCmdPn[i].usPn)
            {
                return MT_OK;
            }
        }
    }

    // 没有找到同样的则添加一个新的进数组
    psCmdPn[ucN].eCmd = sNewCmdPn.eCmd;
    psCmdPn[ucN].usPn = sNewCmdPn.usPn;

    *pucNumCmdPn = (ucN + 1);

    return MT_OK;
}
/*****************************************************************************
 函 数 名  : emt_pnfn_to_cmdpn
 功能描述  : 实现ucNumPnFn 个sMtPnFn结构数组 到 sMtCmdPn 结构数据的转换 
             并输出转换后数组个数
 输入参数  : eMtAFN eAfn         
             sMtPnFn* psPnFn     
             UINT8 ucNumPnFn     
            
 输出参数  : sMtCmdPn* psCmdPn   
             UINT8 *pucNumCmdPn  
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月13日 星期五
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emt_pnfn_to_cmdpn(eMtAFN eAfn, sMtPnFn* psPnFn, UINT8 ucNumPnFn,   sMtCmdPn* psCmdPn, UINT8 *pucNumCmdPn)
{
    if(!psPnFn || !psCmdPn || !pucNumCmdPn)
    {
        #ifdef MT_DBG
        DEBUG("emt_pnfn_to_cmdpn() para pointer is null!");
        #endif
        return MT_ERR_NULL;
    }

    UINT8   ucFn  = 0;
    UINT8   ucAFN = 0;
    UINT16  usCmd = 0;
    INT32   i     = 0;
    INT32   j     = 0;
    INT32   k     = 0;
    eMtCmd  eCmd  = CMD_AFN_F_UNKOWN;
    eMtErr  eRet  = MT_OK;
    sMtDaDt sDaDt;
    sMtCmdPn sNew ;
    
    switch(eAfn)
    {
        case AFN_00_CONF:
        case AFN_01_RSET:  
        case AFN_02_LINK:  
        case AFN_03_RELY:  
        case AFN_04_SETP:  
        case AFN_05_CTRL:  
        case AFN_06_AUTH:  
        case AFN_08_CASC:  
        case AFN_09_CFIG:  
        case AFN_0A_GETP:  
        case AFN_0B_ASKT:  
        case AFN_0C_ASK1:  
        case AFN_0D_ASK2:  
        case AFN_0E_ASK3:              
        case AFN_0F_FILE:              
        case AFN_10_DATA:              

            ucAFN = (UINT8)eAfn;
            break;
            
         default:
            #ifdef MT_DBG
            DEBUG("emt_pnfn_to_cmdpn() para err!");
            #endif
            return MT_ERR_PARA;  
            //break;

    }

    for(i = 0; i < ucNumPnFn; i++)
    {
        eRet = emt_pnfn_to_dadt(&(psPnFn[i]), &sDaDt);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emt_pnfn_to_cmdpn() emt_pnfn_to_dadt  err %d", eRet);
            #endif
            return eRet;  
        }

        // p0的情况
        if(0 == sDaDt.ucDA1 && 0 == sDaDt.ucDA2)
        {
            sNew.usPn = 0;

            for(j = 0; j < 8; j++)
            {
                if(sDaDt.ucDT1 & (0x01 << j))
                {
                    ucFn  = (sDaDt.ucDT2 * 8) + j + 1;
                    usCmd = (UINT16)(ucAFN << 8 | ucFn);
                    eCmd  = (eMtCmd)usCmd;
                    sNew.eCmd = eCmd;
                    (void)emt_add_cmdpn(psCmdPn ,sNew,pucNumCmdPn);
                }
            }
        }
        else
        {
            for(k = 0; k < 8; k++)
            {
                if(sDaDt.ucDA1 & (0x01 << k))
                {
                    sNew.usPn = ((sDaDt.ucDA2-1) * 8 + k + 1);
                    for(j = 0; j < 8; j++)
                    {
                        if(sDaDt.ucDT1 & (0x01 << j))
                        {
                            ucFn  = (sDaDt.ucDT2 * 8) + j + 1;
                            usCmd = (UINT16)(ucAFN << 8 | ucFn);
                            eCmd  = (eMtCmd)usCmd;
                            sNew.eCmd = eCmd;
                            (void)emt_add_cmdpn(psCmdPn ,sNew,pucNumCmdPn);
                        }
                    }
                }
            }
        }
    }
    
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emt_cmdpn_to_pnfn
 功能描述  : 实现将ucNumCmdPn个sMtCmdPn数组 转换成sMtPnFn数组
             并输出转换后的个数pucNumPnFn
 输入参数  : eMtAFN eAfn        
             sMtPnFn* psPnFn    
             UINT8 *pucNumPnFn  
             sMtCmdPn* psCmdPn  
             UINT8  ucNumCmdPn  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月13日 星期五
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emt_cmdpn_to_pnfn(eMtAFN eAfn, sMtPnFn* psPnFn, UINT8 *pucNumPnFn, sMtCmdPn* psCmdPn, UINT8  ucNumCmdPn)
{

    if(!psPnFn || !psCmdPn || !pucNumPnFn)
    {
        #ifdef MT_DBG
        DEBUG("pucNumPnFn() para pointer is null!");
        #endif
        return MT_ERR_NULL;
    }

    UINT8   ucTeamPn     = 0xFF;
    UINT8   ucTeamPnBase = 0xFF;
    UINT8   ucTeamFn     = 0xFF;
    UINT8   ucTeamFnBase = 0xFF;
    UINT8   ucFn         = 0;
    UINT8   ucNumPnFn    = 0; 
    UINT16  usPn         = 0;
    INT32   i            = 0;
    INT32   j            = 0;
    INT32   k            = 0;
    INT32   FnIndex      = 0;
    INT32   PnIndex      = 0;
    INT32   nPos         = 0;   
    BOOL    bFind        = FALSE;
    eMtCmd  eCmd         = CMD_AFN_F_UNKOWN;
    eMtAFN  eCmdAfn      = AFN_NULL;

    for(i = 0; i < ucNumCmdPn; i++)
    {
        eCmd    = psCmdPn[i].eCmd;
        usPn    = psCmdPn[i].usPn;
        ucFn    = ucGetCmdFn(eCmd); 
        eCmdAfn = eGetCmdAfn(eCmd);

        if(eCmdAfn != eAfn)  
        {
            #ifdef MT_DBG
            DEBUG("emt_cmdpn_to_pnfn() cmd is not is a same Afn");
            #endif
            return MT_ERR_TEAM;
        }

        nPos = ucNumPnFn;
        
        // 初始化PnFn组
        for(k = 0; k < 8; k++)
        {
            psPnFn[nPos].ucFn[k] = MT_FN_NONE;
            psPnFn[nPos].usPn[k] = MT_PN_NONE;
        }

        for(j = 0; j < ucNumPnFn; j++)
        {
            // 找到则nDataPos置找到的位置
            // 先判断Pn是否在同一组
            ucTeamPn     = ucmt_get_pn_team(usPn);
            ucTeamPnBase = ucmt_get_pn8_team(psPnFn[j].usPn);
            
            if(ucTeamPn == ucTeamPnBase)
            {
                // 再判断Fn是否属于同一个组
                ucTeamFn     = ucmt_get_fn_team(ucFn);
                ucTeamFnBase = ucmt_get_fn8_team(psPnFn[j].ucFn);

                if(ucTeamFn == ucTeamFnBase)
                {
                    bFind = TRUE;
                    nPos  = j;
                    break;
                }
            }
        }

         // 未找到则新启一组  
        if(FALSE == bFind)
        {
            ucNumPnFn  += 1;
        }

        if(0 == usPn)
        {
            PnIndex = 0;
            
            for(k = 0; k < 8; k++)
            {
                psPnFn[nPos].usPn[k] = 0;
            }
        }
        else
        {
            PnIndex = (usPn - 1) % 8; 
            psPnFn[nPos].usPn[PnIndex] =  usPn;
        }
      
        FnIndex = (ucFn - 1) % 8;
        psPnFn[nPos].ucFn[FnIndex] = ucFn;

        // 重置未找到状态
        bFind = FALSE;
        
    }
    
    *pucNumPnFn = ucNumPnFn;
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emt_dadt_to_cmdpn
 功能描述  : 实现ucNumDaDt 个sMtDaDt结构数组 到 sMtCmdPn 结构数据的转换 
             并输出转换后的个数
 输入参数  : eMtAFN eAfn         
             sMtDaDt* psDaDt     
             UINT8 ucNumDaDt     
             sMtCmdPn* psCmdPn   
             UINT8 *pucNumCmdPn  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月13日 星期五
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emt_dadt_to_cmdpn(eMtAFN eAfn, sMtDaDt* psDaDt, UINT8 ucNumDaDt, sMtCmdPn* psCmdPn, UINT8 *pucNumCmdPn)
{
    if(!psDaDt || !psCmdPn || !pucNumCmdPn)
    {
        #ifdef MT_DBG
        DEBUG("emt_dadt_to_cmdpn() para pointer is null!");
        #endif
        return MT_ERR_NULL;
    }

    UINT8   ucFn  = 0;
    UINT8   ucAFN = 0;
    UINT16  usCmd = 0;
    INT32   i     = 0;
    INT32   j     = 0;
    INT32   k     = 0;
    eMtCmd  eCmd  = CMD_AFN_F_UNKOWN;
    sMtDaDt sDaDt = {0};
    sMtCmdPn sNew ;
    
    switch(eAfn)
    {
        case AFN_00_CONF:
        case AFN_01_RSET:  
        case AFN_02_LINK:  
        case AFN_03_RELY:  
        case AFN_04_SETP:  
        case AFN_05_CTRL:  
        case AFN_06_AUTH:  
        case AFN_08_CASC:  
        case AFN_09_CFIG:  
        case AFN_0A_GETP:  
        case AFN_0B_ASKT:  
        case AFN_0C_ASK1:  
        case AFN_0D_ASK2:  
        case AFN_0E_ASK3:              
        case AFN_0F_FILE:              
        case AFN_10_DATA:              

            ucAFN = (UINT8)eAfn;
            break;
            
         default:
            #ifdef MT_DBG
            DEBUG("emt_pnfn_to_cmdpn() para err!");
            #endif
            return MT_ERR_PARA;  
            //break;

    }

    for(i = 0; i < ucNumDaDt; i++)
    {
        sDaDt.ucDA1 = psDaDt[i].ucDA1;
        sDaDt.ucDA2 = psDaDt[i].ucDA2;
        sDaDt.ucDT1 = psDaDt[i].ucDT1;
        sDaDt.ucDT2 = psDaDt[i].ucDT2;

        // p0的情况
        if(0 == sDaDt.ucDA1 && 0 == sDaDt.ucDA2)
        {
            sNew.usPn = 0;

            for(j = 0; j < 8; j++)
            {
                if(sDaDt.ucDT1 & (0x01 << j))
                {
                    ucFn  = (sDaDt.ucDT2 * 8) + j + 1;
                    usCmd = (UINT16)(ucAFN << 8 | ucFn);
                    eCmd  = (eMtCmd)usCmd;
                    sNew.eCmd = eCmd;
                    (void)emt_add_cmdpn(psCmdPn ,sNew,pucNumCmdPn);
                }
            }
        }
        else
        {
            for(k = 0; k < 8; k++)
            {
                if(sDaDt.ucDA1 & (0x01 << k))
                {
                    sNew.usPn = ((sDaDt.ucDA2-1) * 8 + k + 1);
                    for(j = 0; j < 8; j++)
                    {
                        if(sDaDt.ucDT1 & (0x01 << j))
                        {
                            ucFn  = (sDaDt.ucDT2 * 8) + j + 1;
                            usCmd = (UINT16)(ucAFN << 8 | ucFn);
                            eCmd  = (eMtCmd)usCmd;
                            sNew.eCmd = eCmd;
                            (void)emt_add_cmdpn(psCmdPn ,sNew,pucNumCmdPn);
                        }
                    }
                }
            }
        }
    }

    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emt_cmdpn_to_dadt
 功能描述  : 实现将ucNumCmdPn个sMtCmdPn数组 转换成sMtDaDt数组
             并输出转换后的个数pucNumDaDt
 输入参数  : eMtAFN eAfn        
             sMtDaDt* psDaDt    
             UINT8 *pucNumDaDt  
             sMtCmdPn* psCmdPn  
             UINT8  ucNumCmdPn  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月13日 星期五
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emt_cmdpn_to_dadt(eMtAFN eAfn, sMtDaDt* psDaDt, UINT8 *pucNumDaDt, sMtCmdPn* psCmdPn, UINT8  ucNumCmdPn)
{
    if(!psDaDt || !psCmdPn || !pucNumDaDt)
    {
        #ifdef MT_DBG
        DEBUG("emt_cmdpn_to_dadt() para pointer is null!");
        #endif
        return MT_ERR_NULL;
    }
    
    sMtPnFn *psPnFn   = NULL; 
    UINT8   ucNumPnFn = 0;
    INT32   i         = 0;
    eMtErr  eRet      = MT_OK;
    
    psPnFn = (sMtPnFn*)malloc(sizeof(sMtPnFn)*(ucNumCmdPn+1));
    if(!psPnFn)
    {
        #ifdef MT_DBG
        DEBUG("emt_cmdpn_to_dadt() MT_ERR_IO");
        #endif
        return MT_ERR_IO;
    }

    #if 0
    #ifdef MT_DBG
    printf("ucNumCmdPn = %d\n", ucNumCmdPn);
    for(i = 0; i < ucNumCmdPn; i++)
    {
       printf("psCmdPn[%d].eCmd = 0x%04X\n",i, psCmdPn[i].eCmd);
       printf("psCmdPn[%d].usPn = %d\n\n",i, psCmdPn[i].usPn);
    }
    #endif
    #endif
    
    eRet = emt_cmdpn_to_pnfn(eAfn, psPnFn, &ucNumPnFn, psCmdPn, ucNumCmdPn);
    if(MT_OK != eRet)
    {
        #ifdef MT_DBG
        DEBUG("emt_cmdpn_to_dadt() emt_cmdpn_to_pnfn() error = %d", eRet);
        #endif
        MT_FREE(psPnFn);
        return eRet;
    }

    #if 0
    #ifdef MT_DBG
    int j = 0;
    printf("ucNumPnFn = %d\n", ucNumPnFn);
    for(i = 0; i < ucNumPnFn; i++)
    {
        for(j = 0; j < 8; j++)
        {
            printf("psPnFn[%d].ucFn[%d] = %d\n", i,j ,psPnFn[i].ucFn[j]); 
            printf("psPnFn[%d].usPn[%d] = %d\n\n", i,j ,psPnFn[i].usPn[j]); 
            
        }
    }
    #endif
    #endif

    for(i = 0; i < ucNumPnFn; i++)
    {
        eRet = emt_pnfn_to_dadt(&(psPnFn[i]), &(psDaDt[i]));
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emt_cmdpn_to_dadt() emt_pnfn_to_dadt() error = %d", eRet);
            #endif
            MT_FREE(psPnFn);
            return eRet;
        }
    }

    *pucNumDaDt = ucNumPnFn;
    MT_FREE(psPnFn);
    return MT_OK;
}
/*****************************************************************************
 函 数 名  : bMtGetFloatSign
 功能描述  : 获得一个Float的符号位  0 1
 输入参数  : float fVal  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年6月4日
    作    者   : 李明
    修改内容   : 新生成函数

*****************************************************************************/
int    bMtGetFloatSign(float fVal)
{
    uMtFloatFmt floatFmt;
    floatFmt.v = fVal;
    return floatFmt.s.sign;
}

/*****************************************************************************
 函 数 名  : vMtSetFloatSign
 功能描述  : 设置一个浮点数Float的符号位
 输入参数  : float *pfVal  
             int sign     大于等于0为正，小于0为负  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年6月4日
    作    者   : 李明
    修改内容   : 新生成函数

*****************************************************************************/
void   vMtSetFloatSign(float *pfVal, int sign)
{
    uMtFloatFmt *pFloatFmt;
    pFloatFmt = (uMtFloatFmt *)pfVal;
    
    if(sign >= 0)
    {
        pFloatFmt->s.sign = 0;
    }
    else
    {
        pFloatFmt->s.sign = 1;
    } 
}

/*****************************************************************************
 函 数 名  : bMtGetDoubleSign
 功能描述  : 获得一个double 的符号位
 输入参数  : double dVal  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年5月30日
    作    者   : 李明
    修改内容   : 新生成函数

*****************************************************************************/
int    bMtGetDoubleSign(double dVal)
{
    uMtDoubleFmt  doubleFmt;
    doubleFmt.v = dVal;
    return doubleFmt.s.sign; 
}

/*****************************************************************************
 函 数 名  : vMtSetDoubleSign
 功能描述  : 设置浮点数的符号位
 输入参数  : int sign   >= 0 时，设置为正数， < 0时设置为负数    
 输出参数  : double *pdVal  
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年5月30日
    作    者   : 李明
    修改内容   : 新生成函数

*****************************************************************************/
void   vMtSetDoubleSign(double *pdVal, int sign)
{
    uMtDoubleFmt  *pdoubleFmt;
    pdoubleFmt = (uMtDoubleFmt*)pdVal;

    if(sign >= 0)
    {
        pdoubleFmt->s.sign = 0;
    }
    else
    {
        pdoubleFmt->s.sign = 1;
    }
}

/*****************************************************************************
 函 数 名  : emcTrans_OneByOne
 功能描述  : 逐个确认否认转换函数
 输入参数  : eMTTransDir eDir  
             void *psUser      
             void *psFrame     
             UINT16 *pusfLen   
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年6月9日
    作    者   : 李明
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_OneByOne(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    sMtOneByOne_f *psOneByOne_f = (sMtOneByOne_f *)psFrame;
    sMtOnebyOne   *psOneByOne_u = (sMtOnebyOne   *)psUser;
    INT32       i         = 0;
    INT32       j         = 0;
    INT32       k         = 0;
    INT32       fi        = 0;
    INT32       pi        = 0;
    eMtErr      eRet      = MT_OK;
    INT32       nNum      = 0;  
    INT32       nDaDtNum  = 0;      // 封装packbase的信息标识与64个数据单元组的总个数
    INT32       nDaDtPos  = 0;      // 每次应放入packbase的信息标识组的索引
    UINT16      usCMD     = 0;
    UINT16      usPn      = 0;
    UINT8       ucFn      = 0;
    eMtCmd      eCmd      = CMD_AFN_F_UNKOWN;
    eMtAFN      eAFN      = AFN_NULL;
    eMtAFN      eAFNCmd   = AFN_NULL;
    BOOL        bOK       = FALSE;
    BOOL        bFindDaDt = FALSE;  // 是否找到之前存在的组
    BOOL        bP0       = FALSE;
    INT32       nCyc      = 0;
    sMtPnFn     sPnFn;
    UINT8*      pMem      = NULL;
    sMtFnPnErr* psFnPnErr = NULL;
    #define     MT_TEAM_MAX  (50)   // 可能的最大组数

    if(!psFrame || !psUser || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_OneByOne() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    memset(&(sPnFn), 0x00, sizeof(sMtPnFn));

    // 帧侧转为用户侧
    if(MT_TRANS_F2U == eTrans)
    {
        // 解析时将其应用层数据长度
        nNum = (int)*pusfLen;
        nNum -= sizeof(UINT8);
        nNum /= sizeof(sMtOne_f); // 计算含有确认否认个数

        if(nNum > MT_FN_MAX)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_OneByOne() MT_ERR_OUTRNG");
            #endif
            return MT_ERR_OUTRNG;
        }

        eAFN = (eMtAFN)psOneByOne_f->ucAFN;
        psOneByOne_u->eAFN = eAFN;
        psOneByOne_u->ucNum = nNum;
        for(i = 0; i < nNum; i++)
        {
            bOK = psOneByOne_f->sOne[i].ucErr == 0 ? TRUE : FALSE;
            eRet = emt_dadt_to_pnfn(&(psOneByOne_f->sOne[i].sDaDt), &sPnFn);
            if(eRet != MT_OK)
            {   
                #ifdef MT_DBG
                DEBUG("Error:emt_dadt_to_pnfn() error = %d %s\n", eRet, smtGetErr(eRet));
                #endif
                return eRet;
            }

            // 是否为p0的情况
            bP0 = bmt_is_p0(sPnFn.usPn);
            if(TRUE == bP0)
            {
                nCyc = 1;
            }
            else
            {
                nCyc = PN_INDEX_MAX;
            }
            
            for(pi = 0; pi < nCyc; pi++)
            {
                if(MT_PN_NONE != sPnFn.usPn[pi])
                {
                    for(fi = 0; fi < FN_INDEX_MAX; fi++)
                    {
                        if(MT_FN_NONE != sPnFn.ucFn[fi])
                        { 
                            usCMD = (UINT16)((eAFN << 8) | sPnFn.ucFn[fi]);
                            eCmd  = (eMtCmd)usCMD; 
                            psOneByOne_u->sOne[j].bOk = bOK;
                            psOneByOne_u->sOne[j].usPn = sPnFn.usPn[pi];
                            psOneByOne_u->sOne[j].eCmd = eCmd;
                            j++;
                        }
                    }
                }            
            }
        }
    }
    else 
    // 用户侧转为帧侧
    {
        eAFN = psOneByOne_u->eAFN;
        psOneByOne_f->ucAFN  = (UINT8)psOneByOne_u->eAFN;

        pMem = (UINT8*)malloc(sizeof(sMtFnPnErr) * MT_TEAM_MAX);
        if(!pMem)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_OneByOne() MT_ERR_IO");
            #endif
            return MT_ERR_IO;
        }
            
        psFnPnErr = (sMtFnPnErr*)pMem;
            
        for(i = 0; i < psOneByOne_u->ucNum; i++)
        {
            // 判断该命令是否属于AFN的子命令
            eAFNCmd = eGetCmdAfn(psOneByOne_u->sOne[i].eCmd);
            if(eAFNCmd != eAFN)  
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_OneByOne() cmd is not is a same Afn ");
                DEBUG("eAFNCmd = %d", eAFNCmd);
                DEBUG("eAFN = %d", eAFN);
                #endif
                MT_FREE(pMem);
                return MT_ERR_TEAM;
            }

            usPn = psOneByOne_u->sOne[i].usPn;
            ucFn = ucGetCmdFn(psOneByOne_u->sOne[i].eCmd);
            bOK  = psOneByOne_u->sOne[i].bOk;

            /*
                判断该合法的数据单元标识,与数据单元是否已经填入应在位置 psBasePack
                如果重复,后者覆盖前者,Da2 与不同的Dt2组合, 
                后产生不同的标识与数据单元组,决定nDaTaNum的个数
                
            */
            nDaDtPos = nDaDtNum;
            // 初始化PnFn组
            for(k = 0; k < 8; k++)
            {
                psFnPnErr[nDaDtPos].sPnFn.ucFn[k] = MT_FN_NONE;
                psFnPnErr[nDaDtPos].sPnFn.usPn[k] = MT_PN_NONE;
            }
            
            if( nDaDtPos > MT_TEAM_MAX)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_OneByOne() MT_TEAM_MAX too small");
                #endif
                MT_FREE(pMem);
                return MT_ERR_OUTRNG;
            }
            
            for(j = 0; j < nDaDtNum; j++)
            {
                // 找到则nDataPos置找到的位置
                // 先判断Pn是否在同一组
                UINT8 ucTeamPn     = 0xFF;
                UINT8 ucTeamPnBase = 0xFF;
                ucTeamPn     = ucmt_get_pn_team(usPn);
                ucTeamPnBase = ucmt_get_pn8_team(psFnPnErr[j].sPnFn.usPn);
                
                if(ucTeamPn == ucTeamPnBase)
                {
                    // 再判断Fn是否属于同一个组
                    UINT8 ucTeamFn     = 0xFF;
                    UINT8 ucTeamFnBase = 0xFF;
                    ucTeamFn     = ucmt_get_fn_team(ucFn);
                    ucTeamFnBase = ucmt_get_fn8_team(psFnPnErr[j].sPnFn.ucFn);

                    if(ucTeamFn == ucTeamFnBase)
                    {
                         if(bOK == psFnPnErr[j].bOk)
                         {
                            bFindDaDt = TRUE;
                            nDaDtPos  = j;
                            break;
                         }                        
                    }
                }
            }

            // 未找到则nDaTaNum++
            if(FALSE == bFindDaDt)
            {
                nDaDtNum  += 1; // 新启一组  
            }
            
            // 放入 DaTa组及数据单元
            if(0 == usPn)
            {
                pi = 0;
                for(k = 0; k < 8; k++)
                {
                    psFnPnErr[nDaDtPos].sPnFn.usPn[k] = 0;
                }
            }
            else
            {
                pi = (usPn - 1) % 8; 
                psFnPnErr[nDaDtPos].sPnFn.usPn[pi] = usPn;
            }
          
            fi = (ucFn - 1) % 8;
            psFnPnErr[nDaDtPos].sPnFn.ucFn[fi] = ucFn;

            // 数据单元
            psFnPnErr[j].bOk = bOK;
            
            // 重置未找到状态
            bFindDaDt = FALSE;
        }

        //sMtFnPnErr 实现 sMtOne_f 转换
        for(i = 0; i < nDaDtNum; i++)
        {
            eRet = emt_pnfn_to_dadt(&psFnPnErr[i].sPnFn, &(psOneByOne_f->sOne[i].sDaDt));
            if(eRet != MT_OK)
            {
                #ifdef MT_DBG
                DEBUG("Error:emt_pnfn_to_dadt() error = %d %s\n", eRet, smtGetErr(eRet));
                #endif
                return eRet;
            }
                    
            // right or error
            psOneByOne_f->sOne[i].ucErr = psFnPnErr[i].bOk == TRUE ? 0 : 1;
        }
     }

    // 计算在帧侧的字节长度
    *pusfLen = sizeof(UINT8) + sizeof(sMtOne_f) * psOneByOne_u->ucNum;
 
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn03f1
 功能描述  : 中继站工作状态控制 
 对应AFN   : AFN_03_RELY 
 对应命令  : CMD_AFN_3_F1_RELAY_CTRL
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月13日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn03f1(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{

    // 计算在帧侧的字节长度
    *pusfLen = 1;
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn04f1
 功能描述  : F1：终端上行通信口通信参数设置 
             CMD_AFN_4_F1_TML_UP_CFG  
             CMD_AFN_A_F1_TML_UP_CFG 
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月14日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f1(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if(!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn04f1() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    sMtAfn04F1    *psAfn04F1   = (sMtAfn04F1*)psUser;
    sMtAfn04F1_f  *psAfn04F1_f = (sMtAfn04F1_f*)psFrame;
     
    UINT8 bs8Con = 0;
    
    if(MT_TRANS_U2F == eTrans)
    {
        psAfn04F1_f->ucRTS       = psAfn04F1->ucRTS;
        psAfn04F1_f->ucRTM       = psAfn04F1->ucRTM;
        psAfn04F1_f->ucHeartBeat = psAfn04F1->ucHeartBeat;
        psAfn04F1_f->ucSWTS1     = (psAfn04F1->usTmWaitTimeoutS & 0xFF);
        psAfn04F1_f->ucSWTS2     = ((psAfn04F1->usTmWaitTimeoutS & 0x0F00) >> 8);
        psAfn04F1_f->ucSWTS2    |= ((psAfn04F1->ucReSendTimes & 0x03) << 4);
        
        if(psAfn04F1->bAutoReportAsk_1 == TRUE)
        {
            bs8Con |= 0x01;
        }

        if(psAfn04F1->bAutoReportAsk_2 == TRUE)
        {
            bs8Con |= 0x02;
        }

        if(psAfn04F1->bAutoReportAsk_3 == TRUE)
        {
            bs8Con |= 0x04;
        }

        psAfn04F1_f->bs8Con   = bs8Con;

        #if 0
        #ifdef MT_DBG
        printf("emt_trans_Afn04F1_ConUpPortPara() user to frame\n");
        printf("ucRTS = %d\n", psAfn04F1_f->ucRTS);
        printf("ucRTM = %d\n", psAfn04F1_f->ucRTM);
        printf("ucHeartBeat = %d\n", psAfn04F1_f->ucHeartBeat);
        printf("ucSWTS1 = %x\n",  psAfn04F1_f->ucSWTS1);
        printf("ucSWTS2 = %x\n",  psAfn04F1_f->ucSWTS2);
        printf("bs8Con = %x\n",  psAfn04F1_f->bs8Con);
        #endif
        #endif
    }
    else if(MT_TRANS_F2U == eTrans)
    {
        psAfn04F1->ucRTS            = psAfn04F1_f->ucRTS;
        psAfn04F1->ucRTM            = psAfn04F1_f->ucRTM;
        psAfn04F1->ucHeartBeat      = psAfn04F1_f->ucHeartBeat;
        psAfn04F1->usTmWaitTimeoutS = (((psAfn04F1_f->ucSWTS2 & 0x0F) << 8)  | psAfn04F1_f->ucSWTS1);
        psAfn04F1->ucReSendTimes    = (psAfn04F1_f->ucSWTS2 >> 4) & 0x03;
        psAfn04F1->bAutoReportAsk_1 = ((psAfn04F1_f->bs8Con & 0x01) ? TRUE : FALSE);
        psAfn04F1->bAutoReportAsk_2 = ((psAfn04F1_f->bs8Con & 0x02) ? TRUE : FALSE);
        psAfn04F1->bAutoReportAsk_3 = ((psAfn04F1_f->bs8Con & 0x04) ? TRUE : FALSE);

        #if 0
        #ifdef MT_DBG
        printf("emtTrans_afn04f1() frame to user\n");
        printf("ucRTS = %d\n", psAfn04F1->ucRTS);
        printf("ucRTM = %d\n", psAfn04F1->ucRTM);
        printf("ucHeartBeat = %d\n", psAfn04F1->ucHeartBeat);
        printf("usTmWaitTimeoutS = %d\n", psAfn04F1->usTmWaitTimeoutS);
        printf("ucReSendTimes = %d\n", psAfn04F1->ucReSendTimes);
        printf("bAutoReportAsk_1 = %s\n", psAfn04F1->bAutoReportAsk_1 == TRUE ? "true" : "flase");
        printf("bAutoReportAsk_2 = %s\n", psAfn04F1->bAutoReportAsk_2 == TRUE ? "true" : "flase");
        printf("bAutoReportAsk_3 = %s\n", psAfn04F1->bAutoReportAsk_3 == TRUE ? "true" : "flase");
        #endif
        #endif
    }
    else
    {
        return MT_ERR_PARA;
    }
    
    *pusfLen = sizeof(sMtAfn04F1_f);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn04f2
 功能描述  :  F2：终端上行通信口无线中继转发设置 CMD_AFN_4_F2_TML_WIRELESS_CFG
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月14日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f2(eMtTrans eTrans, void* psUser, void* psFrame, UINT16* pusfLen)
{
    if(!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn04f2() pointer is null");
        #endif
        return MT_ERR_NULL;
    }
    
    UINT16 usfLen  = 0;
    int    i       = 0;
    int nAddrCount = 0;
    
    sMtAfn04F2    *psAfn04F2;
    sMtAfn04F2_f  *psAfn04F2_f;

    psAfn04F2    = (sMtAfn04F2*)psUser;
    psAfn04F2_f  = (sMtAfn04F2_f*)psFrame;
    
    if(MT_TRANS_U2F == eTrans)
    {
        if(psAfn04F2->bPermit == TRUE)
        {
            psAfn04F2_f->ucPermit = 1;
        }
        else
        {
            psAfn04F2_f->ucPermit = 0;
        }

        nAddrCount = (int)(psAfn04F2->ucAddrCount);
       
        if(nAddrCount > 16)
        {
            return MT_ERR_PARA;    // 该值不可以超过16 
        }
        else
        {
            psAfn04F2_f->ucAddrCount = psAfn04F2->ucAddrCount;
        }

        for(i = 0; i < nAddrCount; i++)
        {
            psAfn04F2_f->usAddress[i] = psAfn04F2->usAddress[i];
        }

        usfLen = 1 + nAddrCount*sizeof(UINT16);
        
    }
    else if(MT_TRANS_F2U == eTrans)
    {
        psAfn04F2->bPermit     = (psAfn04F2_f->ucPermit == 1) ? TRUE:FALSE;
        psAfn04F2->ucAddrCount = psAfn04F2_f->ucAddrCount;
        nAddrCount             = (int)(psAfn04F2_f->ucAddrCount);
        
        for(i = 0; i < nAddrCount; i++)
        {
            psAfn04F2->usAddress[i] = psAfn04F2_f->usAddress[i];
        }

        usfLen = 1 + nAddrCount*sizeof(UINT16);
    }
    else
    {
        return MT_ERR_PARA;
    }

    *pusfLen = usfLen;
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn04f3
 功能描述  : F3：主站IP地址和端口 CMD_AFN_4_F3_MST_IP_PORT CMD_AFN_A_F3_MST_IP_PORT
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月14日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f3(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if(!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn04f3() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    sMtAfn04F3   *psAfn04F3   = (sMtAfn04F3*)psUser;
    sMtAfn04F3_f *psAfn04F3_f = (sMtAfn04F3_f*)psFrame;
    
    if(MT_TRANS_U2F == eTrans)
    {
        psAfn04F3_f->ucMainIP[0] =  psAfn04F3->ulMainIP & 0xFF;
        psAfn04F3_f->ucMainIP[1] = (psAfn04F3->ulMainIP & 0xFF00)     >> 8;
        psAfn04F3_f->ucMainIP[2] = (psAfn04F3->ulMainIP & 0xFF0000)   >> 16;
        psAfn04F3_f->ucMainIP[3] = (psAfn04F3->ulMainIP & 0xFF000000) >> 24;

        psAfn04F3_f->ucBackIP[0] = psAfn04F3->ulBackIP  & 0xFF;
        psAfn04F3_f->ucBackIP[1] = (psAfn04F3->ulBackIP & 0xFF00)     >> 8;
        psAfn04F3_f->ucBackIP[2] = (psAfn04F3->ulBackIP & 0xFF0000)   >> 16;
        psAfn04F3_f->ucBackIP[3] = (psAfn04F3->ulBackIP & 0xFF000000) >> 24;

        psAfn04F3_f->usMainPort  = psAfn04F3->usMainPort;
        psAfn04F3_f->usBackPort  = psAfn04F3->usBackPort;
        memcpy(psAfn04F3_f->ucascAPN, psAfn04F3->ucascAPN, 16);

    }
    else if(MT_TRANS_F2U == eTrans)
    {
        psAfn04F3->ulMainIP  =   psAfn04F3_f->ucMainIP[0] 
                               | psAfn04F3_f->ucMainIP[1] << 8
                               | psAfn04F3_f->ucMainIP[2] << 16
                               | psAfn04F3_f->ucMainIP[3] << 24;

        psAfn04F3->ulBackIP  =   psAfn04F3_f->ucBackIP[0] 
                               | psAfn04F3_f->ucBackIP[1] << 8
                               | psAfn04F3_f->ucBackIP[2] << 16
                               | psAfn04F3_f->ucBackIP[3] << 24;

        psAfn04F3->usMainPort = psAfn04F3_f->usMainPort;
        psAfn04F3->usBackPort = psAfn04F3_f->usBackPort;
        memcpy(psAfn04F3->ucascAPN, psAfn04F3_f->ucascAPN, 16);
    }
    else
    {
        return MT_ERR_PARA;
    }
    
    *pusfLen = sizeof(sMtAfn04F3_f);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn04f4
 功能描述  : F4：主站电话号码和短信中心号码 CMD_AFN_4_F4_MST_PHONE_SMS CMD_AFN_A_F4_MST_PHONE_SMS
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月14日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f4(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    *pusfLen = sizeof(sMtAfn04F4_f);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn04f5
 功能描述  : F5：终端上行通信消息认证参数设置 CMD_AFN_4_F5_TML_UP_AUTH CMD_AFN_A_F5_TML_UP_AUTH
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月14日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f5(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    *pusfLen = sizeof(sMtAfn04F5_f);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn04f6
 功能描述  : F6：终端组地址设置 CMD_AFN_4_F6_TEAM_ADDR CMD_AFN_A_F6_TEAM_ADDR
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月14日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f6(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    *pusfLen = sizeof(sMtAfn04F6_f);
    return MT_OK;
}   

/*****************************************************************************
 函 数 名  : emtTrans_afn04f7
 功能描述  : F6：终端组地址设置 
             CMD_AFN_4_F7_TML_IP_PORT
             CMD_AFN_A_F7_TML_IP_PORT
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月14日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f7(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    *pusfLen = sizeof(sMtAfn04F7_f);
    return MT_OK;
}   

/*****************************************************************************
 函 数 名  : emtTrans_afn04f8
 功能描述  : F8：终端上行通信工作方式（以太专网或虚拟专网） 
             CMD_AFN_4_F8_TML_UP_WAY 
             CMD_AFN_A_F8_TML_UP_WAY
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月14日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f8(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    *pusfLen = sizeof(sMtAfn04F8_f);
    return MT_OK;
}     

/*****************************************************************************
 函 数 名  : emtTrans_afn04f9
 功能描述  : F9：终端事件记录配置设置 
             CMD_AFN_4_F9_TML_EVENT_CFG 
             CMD_AFN_A_F9_TML_EVENT_CFG
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月14日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f9(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    *pusfLen = sizeof(sMtAfn04F9_f);
    return MT_OK;
}     

/*****************************************************************************
 函 数 名  : emtTrans_afn04f10
 功能描述  : F10：终端电能表/交流采样装置配置参数 
             CMD_AFN_4_F10_TML_POWER_CFG 
             CMD_AFN_A_F10_TML_POWER_CFG
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月14日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f10(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if(!psUser || !psFrame || !pusfLen)
    {
         #ifdef MT_DBG
         DEBUG("emtTrans_afn04f10() pointer is null");
         #endif
         return MT_ERR_NULL;
    }

    BOOL      bNsOne    = FALSE;
    UINT8     ucTmp    = 0;
    UINT16    usCfgNum = 0;
    UINT16    usTmp    = 0;
    INT32     i        = 0;
    eMtBaudRate   eBaudRate     = MT_BAUD_DEFAULT;
    sMtAfn04F10   *psAfn04F10   = (sMtAfn04F10*)psUser;
    sMtAfn04F10_f *psAfn04F10_f = (sMtAfn04F10_f*)psFrame;
  
    if(MT_TRANS_U2F == eTrans)
    {
        usCfgNum =  psAfn04F10->usNum;
        psAfn04F10_f->usNum = usCfgNum;

        for(i = 0; i < usCfgNum; i++)
        {
            // usMeterID
            if(TRUE == psAfn04F10->sOne[i].busMeterID)
            {
                usTmp = psAfn04F10->sOne[i].usMeterID;
                if(usTmp > 2040 || usTmp < 1)
                {
                    #ifdef MT_DBG
              DEBUG("emtTrans_afn04f10() usMeterID MT_ERR_OUTRNG!");
                    #endif
                    return MT_ERR_OUTRNG;
                }

                psAfn04F10_f->sOne[i].usMeterID = usTmp;
            }
            else
            {
                vmt_set_none((UINT8*)&(psAfn04F10_f->sOne[i].usMeterID), sizeof(UINT16));
            }

            // usPn
            if(TRUE == psAfn04F10->sOne[i].busPn)
            {
                if(psAfn04F10->sOne[i].busPn > 2040 ||
                   psAfn04F10->sOne[i].busPn < 1)
                {
                    #ifdef MT_DBG
                    DEBUG("emtTrans_afn04f10() busPn MT_ERR_OUTRNG!");
                    #endif
                    return MT_ERR_OUTRNG;
                }

                psAfn04F10_f->sOne[i].usPn = psAfn04F10->sOne[i].usPn;
            }
            else
            {
                vmt_set_none((UINT8*)&(psAfn04F10_f->sOne[i].usPn), sizeof(UINT16));
            }

            // sBaudPort
            if(TRUE == psAfn04F10->sOne[i].bsBaudPort)
            {
                ucTmp = psAfn04F10->sOne[i].sBaudPort.ucPort;
                if(ucTmp > MT_METER_PORT_MAX || ucTmp < MT_METER_PORT_MIN)
                {
                    #ifdef MT_DBG
                    DEBUG("emtTrans_afn04f10() ucPort MT_ERR_OUTRNG!");
                    #endif
                    return MT_ERR_OUTRNG;
                }
                else
                {
                    psAfn04F10_f->sOne[i].sBaudPort.ucPort = psAfn04F10->sOne[i].sBaudPort.ucPort;
                }

                eBaudRate = psAfn04F10->sOne[i].sBaudPort.eBaudRate;
                switch(eBaudRate)
                {
                    case MT_BAUD_DEFAULT:
                        ucTmp = 0;
                        break;
                        
                    case MT_BAUD_600:
                        ucTmp = 1;
                        break;
                        
                    case MT_BAUD_1200:
                        ucTmp = 2;
                        break;
                        
                    case MT_BAUD_2400:
                        ucTmp = 3;
                        break;
                        
                    case MT_BAUD_4800:
                        ucTmp = 4;
                        break;
                        
                    case MT_BAUD_7200:
                        ucTmp = 5;
                        break;
                        
                    case MT_BAUD_9600:
                        ucTmp = 6;
                        break;
                        
                    case MT_BAUD_19200:
                        ucTmp = 7;
                        break;

                    default:
                        #ifdef MT_DBG
                        DEBUG("emtTrans_afn04f10() eBaudRate MT_ERR_OUTRNG!");
                        #endif
                        return MT_ERR_OUTRNG;
                       // break;
                }
                
                psAfn04F10_f->sOne[i].sBaudPort.ucBaudRate =  ucTmp;
            }
            else
            {
                vmt_set_none((UINT8*)&(psAfn04F10_f->sOne[i].sBaudPort), sizeof(sMtBaudPort_f));
            }  

            // ucProto
            if(TRUE == psAfn04F10->sOne[i].beProto)
            {
               switch(psAfn04F10->sOne[i].eProto)
               {
                   case MT_PROTO_DEFAULT:
                        ucTmp = 0;
                        break;
                        
                   case MT_PROTO_DLT645_97:
                        ucTmp = 1;
                        break;
                          
                   case MT_PROTO_DLT645_07:
                        ucTmp = 30;
                        break;
                          
                   case MT_PROTO_AC:
                        ucTmp = 2;
                        break;
                          
                   case MT_PROTO_SERIAL:
                        ucTmp = 31;
                        break;

                  default:
                        #ifdef MT_DBG
                        DEBUG("emtTrans_afn04f10() eProto MT_ERR_OUTRNG!");
                        #endif
                        return MT_ERR_OUTRNG;
                        //break;
               }

                psAfn04F10_f->sOne[i].ucProto = ucTmp;

            }
            else
            {
                vmt_set_none((UINT8*)&(psAfn04F10_f->sOne[i].ucProto), sizeof(UINT8));
            }

            // acMeterAddr
            if(TRUE == psAfn04F10->sOne[i].bacMeterAddr)
            {
                (void)emt_trans_XX_6(MT_TRANS_U2F, 
                                       (psAfn04F10->sOne[i].acMeterAddr),
                                       &(psAfn04F10_f->sOne[i].acMeterAddr));  
            }
            else
            {
                vmt_set_none((UINT8*)&(psAfn04F10_f->sOne[i].acMeterAddr), sizeof(sMtFmt_XX_6));
            }

            // acPwd
            if(TRUE == psAfn04F10->sOne[i].bacPwd)
            {
                memcpy(psAfn04F10_f->sOne[i].acPwd, psAfn04F10->sOne[i].acPwd, 6); 
            }
            else
            {
                vmt_set_none((UINT8*)&(psAfn04F10_f->sOne[i].acPwd), 6);
            }

            // ucPayRateNum
            if(TRUE == psAfn04F10->sOne[i].bucPayRateNum)
            {
                ucTmp = psAfn04F10->sOne[i].ucPayRateNum;
                if(ucTmp > MT_PAYRATE_NUM_MAX || ucTmp < MT_PAYRATE_NUM_MIN)
                {
                    #ifdef MT_DBG
                    DEBUG("emtTrans_afn04f10() PayRateNum MT_ERR_OUTRNG!");
                    #endif
                    return MT_ERR_OUTRNG;
                }
                
                psAfn04F10_f->sOne[i].ucPayRateNum = ucTmp;
            }
            else
            {
                vmt_set_none((UINT8*)&(psAfn04F10_f->sOne[i].ucPayRateNum), sizeof(UINT8));
            }

            // sDigit
            if(TRUE == psAfn04F10->sOne[i].bsDigit)
            {
                // 小数位数
                ucTmp = psAfn04F10->sOne[i].sDigit.ucDecimal;
                if(ucTmp > MT_DNUM_DECIM_MAX || ucTmp < MT_DNUM_DECIM_MIN)
                {
                    #ifdef MT_DBG
                    DEBUG("emtTrans_afn04f10() ucDecimal MT_ERR_OUTRNG!");
                    #endif
                    return MT_ERR_OUTRNG;
                }
                
                psAfn04F10_f->sOne[i].sDigit.ucDecimal = ucTmp - 1;

                // 整数位数
                ucTmp = psAfn04F10->sOne[i].sDigit.ucInteger;
                if(ucTmp > MT_DNUM_INTGR_MAX || ucTmp < MT_DNUM_INTGR_MIN)
                {
                    #ifdef MT_DBG
                    DEBUG("emtTrans_afn04f10() ucInteger MT_ERR_OUTRNG!");
                    #endif
                    return MT_ERR_OUTRNG;
                }
                
                psAfn04F10_f->sOne[i].sDigit.ucInteger = ucTmp - 4;

            }
            else
            {
                vmt_set_none((UINT8*)&(psAfn04F10_f->sOne[i].sDigit), sizeof(sMtDigitNum_f));
            }

            // acGathrAddr
            if(TRUE == psAfn04F10->sOne[i].bacGathrAddr)
            {
                (void)emt_trans_XX_6(MT_TRANS_U2F, 
                                       (psAfn04F10->sOne[i].acGathrAddr),
                                       &(psAfn04F10_f->sOne[i].acGathrAddr));  
            }
            else
            {
                vmt_set_none((UINT8*)&(psAfn04F10_f->sOne[i].acGathrAddr), sizeof(sMtFmt_XX_6));
            }

            // sUserClass
            if(TRUE == psAfn04F10->sOne[i].bsUserClass)
            {
                // 大分类
                ucTmp = psAfn04F10->sOne[i].sUserClass.ucBig;
                if(ucTmp > MT_USER_CLASS_MAX || ucTmp < MT_USER_CLASS_MIN)
                {
                    #ifdef MT_DBG
                    DEBUG("emtTrans_afn04f10() ucBig user class  MT_ERR_OUTRNG!");
                    #endif
                    return MT_ERR_OUTRNG;
                }
                
                psAfn04F10_f->sOne[i].sUserClass.ucBig = ucTmp - 1;
                
                // 小分类
                ucTmp = psAfn04F10->sOne[i].sUserClass.ucSmall;
                if(ucTmp > MT_USER_CLASS_MAX || ucTmp < MT_USER_CLASS_MIN)
                {
                    #ifdef MT_DBG
                    DEBUG("emtTrans_afn04f10() ucSmall user class  MT_ERR_OUTRNG!");
                    #endif
                    return MT_ERR_OUTRNG;
                }
                
                psAfn04F10_f->sOne[i].sUserClass.ucSmall = ucTmp - 1;
            }
            else
            {
                vmt_set_none((UINT8*)&(psAfn04F10_f->sOne[i].sUserClass), sizeof(sMtUserClass_f));
            }
        }
    }
    else if(MT_TRANS_F2U == eTrans)
    {
        usCfgNum = psAfn04F10_f->usNum;
        psAfn04F10->usNum = usCfgNum;
        for(i = 0; i < usCfgNum; i++)
        {
            // usMeterID
            usTmp = psAfn04F10_f->sOne[i].usMeterID;
            bNsOne = bmt_is_none((UINT8*)&usTmp, sizeof(UINT16));
            if(TRUE == bNsOne)
            {
                psAfn04F10->sOne[i].busMeterID = FALSE;
            }
            else
            {
                psAfn04F10->sOne[i].busMeterID = TRUE;
                if(usTmp > MT_METER_ID_MAX || usTmp < MT_METER_ID_MIN)
                {
                    #ifdef MT_DBG
                    DEBUG("emtTrans_afn04f10() usMeterID MT_ERR_OUTRNG!");
                    #endif
                    return MT_ERR_OUTRNG;
                }

                psAfn04F10->sOne[i].usMeterID = usTmp;
            }

            // usPn
            usTmp = psAfn04F10_f->sOne[i].usPn;
            bNsOne = bmt_is_none((UINT8*)&usTmp, sizeof(UINT16));
            if(TRUE == bNsOne)
            {
                psAfn04F10->sOne[i].busPn = FALSE;
            }
            else
            {
                psAfn04F10->sOne[i].busPn = TRUE;
                if(usTmp > MT_PN_MAX || usTmp < MT_PN_MIN)
                {
                    #ifdef MT_DBG
                    DEBUG("emtTrans_afn04f10() usPn MT_ERR_OUTRNG!");
                    #endif
                    return MT_ERR_OUTRNG;
                }

                psAfn04F10->sOne[i].usPn = usTmp;
            }

            // sBaudPort
            bNsOne = bmt_is_none((UINT8*)&(psAfn04F10_f->sOne[i].sBaudPort), sizeof(sMtBaudPort_f));
            if(TRUE == bNsOne)
            {
                psAfn04F10->sOne[i].bsBaudPort = FALSE;
            }
            else
            {
                psAfn04F10->sOne[i].bsBaudPort = TRUE;
                // Baud
                ucTmp = psAfn04F10_f->sOne[i].sBaudPort.ucBaudRate;
                psAfn04F10->sOne[i].sBaudPort.eBaudRate = (eMtBaudRate)ucTmp;

                // Port
                ucTmp = psAfn04F10_f->sOne[i].sBaudPort.ucPort;
                if(ucTmp > MT_METER_PORT_MAX || ucTmp < MT_METER_PORT_MIN)
                {
                    #ifdef MT_DBG
                    DEBUG("emtTrans_afn04f10() ucPort MT_ERR_OUTRNG!");
                    #endif
                    return MT_ERR_OUTRNG;
                }
                else
                {
                   psAfn04F10->sOne[i].sBaudPort.ucPort = ucTmp;
                }
            
            }

            // eProto
            ucTmp = psAfn04F10_f->sOne[i].ucProto;
            bNsOne = bmt_is_none(&ucTmp, sizeof(UINT8));
            if(TRUE == bNsOne)
            {
               psAfn04F10->sOne[i].beProto = FALSE;
            }
            else
            {
               psAfn04F10->sOne[i].beProto = TRUE;

               switch(ucTmp)
               {
                case 0:
                    psAfn04F10->sOne[i].eProto = MT_PROTO_DEFAULT;
                    break;

                case 1:
                    psAfn04F10->sOne[i].eProto = MT_PROTO_DLT645_97;
                    break;

                case 2:
                    psAfn04F10->sOne[i].eProto = MT_PROTO_AC;
                    break;

                case 30:
                    psAfn04F10->sOne[i].eProto = MT_PROTO_DLT645_07;
                    break;
                    
                case 31:
                    psAfn04F10->sOne[i].eProto = MT_PROTO_SERIAL;
                    break;

                 default:
                    #ifdef MT_DBG
                    DEBUG("emtTrans_afn04f10() eProto MT_ERR_OUTRNG!");
                    #endif
                    return MT_ERR_OUTRNG;
                    //break;
               }
            }
            
            // acMeterAddr
            bNsOne = bmt_is_none((UINT8*)&(psAfn04F10_f->sOne[i].acMeterAddr), sizeof(sMtFmt_XX_6));
            if(TRUE == bNsOne)
            {
                psAfn04F10->sOne[i].bacMeterAddr = FALSE;
            }
            else
            {
                psAfn04F10->sOne[i].bacMeterAddr = TRUE;
                (void)emt_trans_XX_6(MT_TRANS_F2U, 
                                     (psAfn04F10->sOne[i].acMeterAddr),
                                     &(psAfn04F10_f->sOne[i].acMeterAddr));  
            }

            // acPwd
            bNsOne = bmt_is_none((UINT8*)&(psAfn04F10_f->sOne[i].acPwd), 6);
            if(TRUE == bNsOne)
            {
                psAfn04F10->sOne[i].bacPwd = FALSE;
            }
            else
            {
                psAfn04F10->sOne[i].bacPwd = TRUE;
                memcpy(psAfn04F10->sOne[i].acPwd, psAfn04F10_f->sOne[i].acPwd, 6); 
            }

            // ucPayRateNum
            bNsOne = bmt_is_none(&(psAfn04F10_f->sOne[i].ucPayRateNum), sizeof(UINT8));
            if(TRUE == bNsOne)
            {
                psAfn04F10->sOne[i].bucPayRateNum = FALSE;
            }
            else
            {
                psAfn04F10->sOne[i].bucPayRateNum = TRUE;
                ucTmp = psAfn04F10_f->sOne[i].ucPayRateNum;
                if(ucTmp > MT_PAYRATE_NUM_MAX || ucTmp < MT_PAYRATE_NUM_MIN)
                {
                    #ifdef MT_DBG
                    DEBUG("emtTrans_afn04f10() PayRateNum MT_ERR_OUTRNG!");
                    #endif
                    return MT_ERR_OUTRNG;
                }
                
                psAfn04F10->sOne[i].ucPayRateNum = ucTmp;
            }
            // sDigit
            bNsOne = bmt_is_none((UINT8*)&(psAfn04F10_f->sOne[i].sDigit), sizeof(sMtDigitNum_f));
            if(TRUE == bNsOne)
            {
                psAfn04F10->sOne[i].bsDigit = FALSE;
            }
            else
            {
                psAfn04F10->sOne[i].bsDigit = TRUE;

                // 小数位数
                ucTmp = psAfn04F10_f->sOne[i].sDigit.ucDecimal + 1;
                if(ucTmp > MT_DNUM_DECIM_MAX || ucTmp < MT_DNUM_DECIM_MIN)
                {
                    #ifdef MT_DBG
                    DEBUG("emtTrans_afn04f10() ucDecimal MT_ERR_OUTRNG!");
                    #endif
                    return MT_ERR_OUTRNG;
                }
                
                psAfn04F10->sOne[i].sDigit.ucDecimal = ucTmp;

                // 整数位数
                ucTmp = psAfn04F10_f->sOne[i].sDigit.ucInteger + 4;
                if(ucTmp > MT_DNUM_INTGR_MAX || ucTmp < MT_DNUM_INTGR_MIN)
                {
                    #ifdef MT_DBG
                    DEBUG("emtTrans_afn04f10() ucInteger MT_ERR_OUTRNG!");
                    #endif
                    return MT_ERR_OUTRNG;
                }
                
                psAfn04F10->sOne[i].sDigit.ucInteger = ucTmp;
            }
            
            // acMeterAddr
            bNsOne = bmt_is_none((UINT8*)&(psAfn04F10_f->sOne[i].acGathrAddr), sizeof(sMtFmt_XX_6));
            if(TRUE == bNsOne)
            {
                psAfn04F10->sOne[i].bacGathrAddr = FALSE;
            }
            else
            {
                psAfn04F10->sOne[i].bacGathrAddr = TRUE;
                (void)emt_trans_XX_6(MT_TRANS_F2U, 
                                     (psAfn04F10->sOne[i].acGathrAddr),
                                     &(psAfn04F10_f->sOne[i].acGathrAddr));  
            }

            // sUserClass
            bNsOne = bmt_is_none((UINT8*)&(psAfn04F10_f->sOne[i].sUserClass), sizeof(sMtUserClass_f));
            if(TRUE == bNsOne)
            {
                psAfn04F10->sOne[i].bsUserClass = FALSE;
            }
            else
            {
                psAfn04F10->sOne[i].bsUserClass = TRUE;
                // 大分类
                ucTmp = psAfn04F10_f->sOne[i].sUserClass.ucBig + 1;
                if(ucTmp > MT_USER_CLASS_MAX || ucTmp < MT_USER_CLASS_MIN)
                {
                    #ifdef MT_DBG
                    DEBUG("emtTrans_afn04f10() ucBig user class  MT_ERR_OUTRNG!");
                    #endif
                    return MT_ERR_OUTRNG;
                }
                
                psAfn04F10->sOne[i].sUserClass.ucBig = ucTmp;
                
                // 小分类
                ucTmp = psAfn04F10_f->sOne[i].sUserClass.ucSmall + 1;
                if(ucTmp > MT_USER_CLASS_MAX || ucTmp < MT_USER_CLASS_MIN)
                {
                    #ifdef MT_DBG
                    DEBUG("emtTrans_afn04f10() ucSmall user class  MT_ERR_OUTRNG!");
                    #endif
                    return MT_ERR_OUTRNG;
                }
                
                psAfn04F10->sOne[i].sUserClass.ucSmall = ucTmp; 
            }
        }
    }
    else
    {
        return MT_ERR_PARA;
    }

    *pusfLen = sizeof(sMtTmlPowerCfgOne_f)* usCfgNum + sizeof(UINT16);
    return MT_OK;
}     

/*****************************************************************************
 函 数 名  : emtTrans_afn04f11
 功能描述  : F11：终端脉冲配置参数 
             CMD_AFN_4_F11_TML_PULSE_CFG 
             CMD_AFN_A_F11_TML_PULSE_CFG
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月14日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f11(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if(!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn04f11() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    UINT8 ucNum = 0;
    sMtAfn04F11_f * psAfn04F11_f = (sMtAfn04F11_f*)psFrame;
    sMtAfn04F11*    psAfn04F11   = (sMtAfn04F11*)psUser;
  
    if(MT_TRANS_U2F == eTrans)
    {
        ucNum = psAfn04F11->ucNum;

    }
    else if(MT_TRANS_F2U == eTrans)
    {
        ucNum = psAfn04F11_f->ucNum;
    }
    else
    {
        return MT_ERR_PARA;
    }

    *pusfLen = sizeof(sMtAfn04F11_f)* ucNum + sizeof(UINT8);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn04f12
 功能描述  : F12：终端状态量输入参数
             CMD_AFN_4_F12_TML_STATE_INPUT 
             CMD_AFN_A_F12_TML_STATE_INPUT
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月16日 星期五
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f12(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    *pusfLen = sizeof(sMtAfn04F12_f);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn04f13
 功能描述  : F13：终端电压/电流模拟量配置参数
             CMD_AFN_4_F13_TML_SIMULA_CFG 
             CMD_AFN_A_F13_TML_SIMULA_CFG
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月16日 星期五
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f13(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if(!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn04f13() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    UINT8 ucNum = 0;
    sMtAfn04F13_f * psAfn04F13_f = (sMtAfn04F13_f*)psFrame;
    sMtAfn04F13*    psAfn04F13   = (sMtAfn04F13*)psUser;
  
    if(MT_TRANS_U2F == eTrans)
    {
        ucNum = psAfn04F13->ucNum;

    }
    else if(MT_TRANS_F2U == eTrans)
    {
        ucNum = psAfn04F13_f->ucNum;
    }
    else
    {
        return MT_ERR_PARA;
    }

    *pusfLen = sizeof(sMtAfn04F13_f)* ucNum + sizeof(UINT8);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn04f14
 功能描述  : F14：终端总加组配置参数
             CMD_AFN_4_F14_TML_GRUP_TOTL 
             CMD_AFN_A_F14_TML_GRUP_TOTL
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月16日 星期五
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f14(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if(!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn04f14() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    UINT8  ucNum = 0;
    UINT8  ucM   = 0;
    UINT16 usLen = 0;
    INT32  i     = 0;
    INT32  j     = 0;
    sMtAfn04F14_f * psAfn04F14_f = (sMtAfn04F14_f*)psFrame;
    sMtAfn04F14*    psAfn04F14   = (sMtAfn04F14*)psUser;
  
    if(MT_TRANS_U2F == eTrans)
    {
        ucNum = psAfn04F14->ucN;
        if(ucNum > 8 || ucNum < 1)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn04f14() ucNum is err ucNum = %d", ucNum);
            #endif
            return MT_ERR_OUTRNG;
        }
        else
        {
            for(i = 0; i < ucNum; i++)
            {
                usLen += 2;
                
                ucM = psAfn04F14->sOne[i].ucM;
                if(ucM > 64)
                {
                    #ifdef MT_DBG
                    DEBUG("emtTrans_afn04f14() ucM is err ucNum = %d", ucM);
                    #endif
                    return MT_ERR_OUTRNG;
                }

                usLen += ucM;
            }
        }

        usLen += 1;
    }
    else if(MT_TRANS_F2U == eTrans)
    {
        psAfn04F14_f = psAfn04F14_f;
        j = j;
    }
    else
    {
        return MT_ERR_PARA;
    }

    *pusfLen = usLen;
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn04f15
 功能描述  : F15：有功总电能量差动越限事件参数设置
             CMD_AFN_4_F15_HAVE_DIFF_EVENT 
             CMD_AFN_A_F15_HAVE_DIFF_EVENT
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月16日 星期五
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f15(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if(!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn04f15() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    UINT8  ucNum = 0;
    UINT16 usLen = 0;
    INT32  i     = 0;
    sMtAfn04F15_f * psAfn04F15_f = (sMtAfn04F15_f*)psFrame;
    sMtAfn04F15*    psAfn04F15   = (sMtAfn04F15*)psUser;
  
    if(MT_TRANS_U2F == eTrans)
    {
        ucNum = psAfn04F15->ucN;
        if(ucNum > 8 || ucNum < 1)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn04f15() ucNum is err ucNum = %d", ucNum);
            #endif
            return MT_ERR_OUTRNG;
        }
        else
        {
            usLen += sizeof(sMtAfn04F15_f)  * ucNum;
        }

        usLen += 1;
    }
    else if(MT_TRANS_F2U == eTrans)
    {
        psAfn04F15_f = psAfn04F15_f;
        i = i;
    }
    else
    {
        return MT_ERR_PARA;
    }

    *pusfLen = usLen;
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn04f16
 功能描述  : F16：虚拟专网用户名、密码
             CMD_AFN_4_F16_VPN_USER_PWD 
             CMD_AFN_A_F16_VPN_USER_PWD
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月16日 星期五
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f16(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    *pusfLen = sizeof(sMtAfn04F16_f);
    return MT_OK;
}


/*****************************************************************************
 函 数 名  : emtTrans_afn04f17
 功能描述  : F17：终端保安定值
             CMD_AFN_4_F17_TML_SAFE_VALUE
             CMD_AFN_A_F17_TML_SAFE_VALUE
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月19日 星期一
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f17(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    eMtErr eRet = MT_OK;
    eRet = emt_trans_sXXX(eTrans, (float*)psUser, (sMtsXXX*)psFrame);
    *pusfLen = sizeof(sMtsXXX);
    return eRet;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn04f18
 功能描述  : F18：终端功控时段
             CMD_AFN_4_F18_TML_PCTRL_PERD
             CMD_AFN_A_F18_TML_PCTRL_PERD
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月19日 星期一
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f18(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    *pusfLen = sizeof(sMtPCtrl_f);  
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn04f19
 功能描述  : F19：终端时段功控定值浮动系数
             CMD_AFN_4_F19_TML_PCTRL_FACTOR
             CMD_AFN_A_F19_TML_PCTRL_FACTOR
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月19日 星期一
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f19(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    eMtErr eRet = MT_OK;
    eRet = emt_trans_sXX(eTrans, (sMtsXX*)psUser, (sMtsXX_f*)psFrame);
    *pusfLen = sizeof(sMtsXX_f);
    return eRet;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn04f20
 功能描述  : F20：终端月电能量控定值浮动系数
             CMD_AFN_4_F20_TML_MONTH_FACTOR
             CMD_AFN_A_F20_TML_MONTH_FACTOR
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月19日 星期一
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f20(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    eMtErr eRet = MT_OK;
    eRet = emt_trans_sXX(eTrans, (sMtsXX*)psUser, (sMtsXX_f*)psFrame);
    *pusfLen = sizeof(sMtsXX_f);
    return eRet;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn04f21
 功能描述  : F21：终端电能量费率时段和费率数
             CMD_AFN_4_F21_TML_POWER_FACTOR
             CMD_AFN_A_F21_TML_POWER_FACTOR
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月19日 星期一
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f21(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if(!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn04f21() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    sMtTmlPowerFactor   *psPowerFactor   = (sMtTmlPowerFactor*)psUser;
    sMtTmlPowerFactor_f *psPowerFactor_f = (sMtTmlPowerFactor_f*)psFrame;
    INT32 i =  0;

    if(MT_TRANS_U2F == eTrans)
    {
        for(i = 0; i < 48; i++)
        {
            if(psPowerFactor->ucPayRateID[i] > MT_PAYRATE_ID_MAX || 
               psPowerFactor->ucPayRateID[i] > MT_PAYRATE_ID_MIN)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn04f21() ucPayRateID is out range");
                #endif
                return MT_ERR_OUTRNG;
            }

            psPowerFactor_f->ucPayRateID[i] = psPowerFactor->ucPayRateID[i];            
        }

        if(psPowerFactor->ucPayRateNum > MT_PAYRATE_NUM_MAX || 
           psPowerFactor->ucPayRateNum > MT_PAYRATE_NUM_MIN)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn04f21() ucPayRateNum is out range");
            #endif
            return MT_ERR_OUTRNG;
        }

        psPowerFactor_f->ucPayRateNum = psPowerFactor->ucPayRateNum;     

    }
    else if(MT_TRANS_F2U == eTrans)
    {
        for(i = 0; i < 48; i++)
        {
            if(psPowerFactor_f->ucPayRateID[i] > MT_PAYRATE_ID_MAX || 
               psPowerFactor_f->ucPayRateID[i] > MT_PAYRATE_ID_MIN)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn04f21() ucPayRateID is out range");
                #endif
                return MT_ERR_OUTRNG;
            }

            psPowerFactor->ucPayRateID[i] = psPowerFactor_f->ucPayRateID[i];            
        }

        if(psPowerFactor_f->ucPayRateNum > MT_PAYRATE_NUM_MAX || 
           psPowerFactor_f->ucPayRateNum > MT_PAYRATE_NUM_MIN)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn04f21() ucPayRateNum is out range");
            #endif
            return MT_ERR_OUTRNG;
        }

        psPowerFactor->ucPayRateNum = psPowerFactor_f->ucPayRateNum; 

    }
    else
    {
        return MT_ERR_PARA;
    }

    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn04f22
 功能描述  : F22：终端电能量费率
             CMD_AFN_4_F22_TML_POWER_RATE
             CMD_AFN_A_F22_TML_POWER_RATE
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月19日 星期一
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f22(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if(!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn04f22() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    INT32         i     = 0;
    UINT8         ucM   = 0;
    eMtErr        eRet  = MT_OK;
    sMtAfn04f22   *pUsr = (sMtAfn04f22*)psUser;
    sMtAfn04f22_f *pFrm = (sMtAfn04f22_f*)psFrame;


    if(MT_TRANS_U2F == eTrans)
    {   
       ucM = pUsr->ucM;
       
        if(ucM > MT_PAYRATE_NUM_MAX || 
           ucM > MT_PAYRATE_NUM_MIN)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn04f22() PayRateNum is out range");
            #endif
            return MT_ERR_OUTRNG;
        }
        
        pFrm->ucM = ucM;
        for(i = 0; i < ucM; i++)
        {
           eRet = emt_trans_sX7(eTrans, 
                                (sMtFmt_sX7*)&(pUsr->sPayRate[i]), 
                                (sMtFmt_sX7_f*)&(pFrm->sPayRate[i]));
           if(MT_OK != eRet)
           {
                #ifdef MT_DBG
                DEBUG("emt_trans_sX7() err %d", eRet);
                #endif
                return eRet;
           }
        }
    }
    else if(MT_TRANS_F2U == eTrans)
    {
        ucM = pFrm->ucM;
       
        if(ucM > MT_PAYRATE_NUM_MAX || 
           ucM > MT_PAYRATE_NUM_MIN)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn04f22() PayRateNum is out range");
            #endif
            return MT_ERR_OUTRNG;
        }
        
        pUsr->ucM = ucM;
        for(i = 0; i < ucM; i++)
        {
           eRet = emt_trans_sX7(eTrans, 
                                (sMtFmt_sX7*)&(pUsr->sPayRate[i]), 
                                (sMtFmt_sX7_f*)&(pFrm->sPayRate[i]));
           if(MT_OK != eRet)
           {
                #ifdef MT_DBG
                DEBUG("emt_trans_sX7() err %d", eRet);
                #endif
                return eRet;
           }
        }

    }
    else
    {
        return MT_ERR_PARA;
    }

    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn04f23
 功能描述  : F23：终端催费告警参数
             CMD_AFN_4_F23_TML_WARNING_CFG
             CMD_AFN_A_F23_TML_WARNING_CFG
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月19日 星期一
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f23(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    *pusfLen = sizeof(sMtAfn04f23_f);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn04f25
 功能描述  : F25：测量点基本参数
             CMD_AFN_4_F25_MP_BASE_CFG
             CMD_AFN_A_F25_MP_BASE_CFG
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月19日 星期一
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f25(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    *pusfLen = sizeof(sMtAfn04f25_f);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0af26
 功能描述  : F26：测量点限值参数
             CMD_AFN_4_F26_MP_LIMIT_CFG
             CMD_AFN_A_F26_MP_LIMIT_CFG
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月20日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f26(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    *pusfLen = sizeof(sMtAfn04f26_f);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn04f27
 功能描述  : F27：测量点铜损、铁损参数 
             CMD_AFN_4_F27_MP_LOSS_CFG
             CMD_AFN_A_F27_MP_LOSS_CFG
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月20日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f27(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    *pusfLen = sizeof(sMtAfn04f27_f);
     return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn04f28
 功能描述  : F28：测量点功率因数分段限值 
             CMD_AFN_4_F28_MP_PERIOD_FACTOR
             CMD_AFN_A_F28_MP_PERIOD_FACTOR
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月20日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f28(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
     *pusfLen = sizeof(sMtAfn04f28_f);
     return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn04f29
 功能描述  : F29：终端当地电能表显示号  
             CMD_AFN_4_F29_TML_METER_ID
             CMD_AFN_A_F29_TML_METER_ID
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月20日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f29(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
     *pusfLen = sizeof(sMtAfn04f29_f);
     return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn04f30
 功能描述  : F30：台区集中抄表停抄/投抄设置
             CMD_AFN_4_F30_TML_AUTO_READ
             CMD_AFN_A_F30_TML_AUTO_READ
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月20日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f30(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
     *pusfLen = sizeof(sMtAfn04f30_f);
     return MT_OK;
}


/*****************************************************************************
 函 数 名  : emtTrans_afn04f31
 功能描述  : F31：载波从节点附属节点地址
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月20日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f31(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if(!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn04f31() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    UINT8 ucN;
    sMtAfn04f31   *psAfn04f31   = (sMtAfn04f31*)psUser;
    sMtAfn04f31_f *psAfn04f31_f = (sMtAfn04f31_f*)psFrame;
    
    if(MT_TRANS_U2F == eTrans)
    {
        ucN = psAfn04f31->ucN;

    }
    else if(MT_TRANS_F2U == eTrans)
    {
        ucN = psAfn04f31_f->ucN;
    }
    else
    {
        return MT_ERR_PARA;
    }


     *pusfLen = sizeof(sMtFmt12) * ucN + 1;
    return MT_OK;

}


/*****************************************************************************
 函 数 名  : emtTrans_afn04f33
 功能描述  : F33：终端抄表运行参数设置 
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月20日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f33(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if(!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn04f31() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    UINT8  ucN   = 0;
    UINT8  ucM   = 0;
    INT32  i     = 0;
    UINT16 usLen = 0;
    UINT16 usLn  = 0;
    sMtAfn04f33   *psAfn04f33   = (sMtAfn04f33*)psUser;
    sMtAfn04f33_f *psAfn04f33_f = (sMtAfn04f33_f*)psFrame;
    
    usLn = sizeof(sMtFmt18_f) + sizeof(sMtFmt19_f) + 9;
    
    if(MT_TRANS_U2F == eTrans)
    {
        ucN = psAfn04f33->ucN;
        for(i = 0; i < ucN; i++)
        {
            usLen += usLn;
            ucM = psAfn04f33->sOne[i].ucM;
            usLen += ucM * sizeof(sMtFmt19_f);
        }
    }
    else if(MT_TRANS_F2U == eTrans)
    {
        ucN = psAfn04f33_f->ucN;

        for(i = 0; i < ucN; i++)
        {
            usLen += usLn;
            ucM = psAfn04f33_f->sOne[i].ucM;
            usLen += ucM * sizeof(sMtFmt19_f);
        }
    }
    else
    {
        return MT_ERR_PARA;
    }

    *pusfLen = usLen;
    return MT_OK;

}

/*****************************************************************************
 函 数 名  : emtTrans_afn04f34
 功能描述  : F34：集中器下行通信模块的参数设置
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月20日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f34(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if(!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn04f34() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    UINT8  ucN   = 0;
    sMtAfn04f34   *psU  = (sMtAfn04f34*)psUser;
    sMtAfn04f34_f *psF  = (sMtAfn04f34_f*)psFrame;

    if(MT_TRANS_U2F == eTrans)
    {
        ucN = psU->ucN;
    }
    else if(MT_TRANS_F2U == eTrans)
    {
        ucN = psF->ucN;       
    }
    else
    {
        return MT_ERR_PARA;
    }

    *pusfLen = ucN * sizeof(sMtConDownCfgOne_f) + sizeof(UINT8);
    return MT_OK;

}

/*****************************************************************************
 函 数 名  : emtTrans_afn04f35
 功能描述  : F35：台区集中抄表重点户设置
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月20日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f35(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if(!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn04f35() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    UINT8  ucN   = 0;
    sMtAfn04f35   *psU  = (sMtAfn04f35*)psUser;
    sMtAfn04f35_f *psF  = (sMtAfn04f35_f*)psFrame;

    if(MT_TRANS_U2F == eTrans)
    {
        ucN = psU->ucN;
       
    }
    else if(MT_TRANS_F2U == eTrans)
    {
        ucN = psF->ucN;

       
    }
    else
    {
        return MT_ERR_PARA;
    }

    *pusfLen = ucN * sizeof(sMtTmlReadVipOne_f) + sizeof(UINT8);
    return MT_OK;

}

/*****************************************************************************
 函 数 名  : emtTrans_afn04f36
 功能描述  : F36：终端上行通信流量门限设置
             CMD_AFN_4_F36_TML_UP_LIMIT
             CMD_AFN_A_F36_TML_UP_LIMIT
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月20日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f36(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    *pusfLen = sizeof(UINT32);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn04f37
 功能描述  : F37：终端级联通信参数
             CMD_AFN_4_F37_TML_CASC_CFG
             CMD_AFN_A_F37_TML_CASC_CFG
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月20日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f37(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if(!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn04f37() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    UINT8  ucN   = 0;
    sMtAfn04f37   *psU  = (sMtAfn04f37*)psUser;
    sMtAfn04f37_f *psF  = (sMtAfn04f37_f*)psFrame;

    if(MT_TRANS_U2F == eTrans)
    {
        ucN = psU->ucN;
       
    }
    else if(MT_TRANS_F2U == eTrans)
    {
        ucN = psF->ucN;

    }
    else
    {
        return MT_ERR_PARA;
    }

     *pusfLen = ucN * sizeof(sMtCascOne_f) + 7 * sizeof(UINT8);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn04f38
 功能描述  : F38：1类数据配置设置 （在终端支持的1类数据配置内）
             CMD_AFN_4_F38_CFG_ASK_1
             CMD_AFN_A_F38_CFG_ASK_1
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月21日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f38(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if(!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn04f38() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    UINT8         ucFN   = 0;
    UINT8         ucN    = 0;
    UINT8         ucM    = 0;
    UINT8         ucTmp  = 0;
    UINT8         ucAfn  = 0;
    UINT8         ucFn   = 0;
    UINT8         ucTeam = 0;
    UINT8         ucTMax = 0;
    UINT8         ucBit  = 0;
    UINT16        usLen  = 0;
    UINT16        usCmd  = 0;
    INT32         i      = 0;
    INT32         m      = 0;
    INT32         n      = 0;
    eMtCmd        eCmd   = CMD_AFN_F_UNKOWN;
    eMtErr        eRet   = MT_OK;
    sMtCmdInfor   sCmdInfor;
    

    sMtAfn04f38     *psU    = (sMtAfn04f38*)psUser;
    sMtAfn04f38_f   *psF    = (sMtAfn04f38_f*)psFrame;
    sMtAsk1CfgOne_f *psAsk1CfgOne_f = NULL;
    sMtAsk1CfgOne   *psAsk1CfgOne   = NULL;
    UINT8           *pTmp   = NULL;
        
    if(MT_TRANS_U2F == eTrans)
    {
        ucTmp = psU->ucBig;
        if(MT_USER_CLASS_MIN > ucTmp || MT_USER_CLASS_MAX < ucTmp)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn04f38() ucBig is out of range ucBig = %d", ucTmp);
            #endif
            return MT_ERR_OUTRNG;
        }
    
        psF->ucBig = ucTmp;
        
        ucM = psU->ucM;
        if(MT_ASK_M_MIN > ucM || MT_ASK_M_MAX < ucM)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn04f38() ucM is out of range ucBig = %d", ucM);
            #endif
            return MT_ERR_OUTRNG;
        }

        psF->ucM = ucM;
        
        // 计算长度
        usLen = 2;
        
        psAsk1CfgOne_f = (sMtAsk1CfgOne_f *)&(psF->sOne[0]);
        pTmp = (UINT8*)&(psAsk1CfgOne_f->ucF8n[0]);

        for(m = 0; m < ucM; m++)
        {
            psAsk1CfgOne = &(psU->sOne[m]);
            psAsk1CfgOne_f->ucSmall = psAsk1CfgOne->ucSmall;
            ucFN = psAsk1CfgOne->ucFN;
            for(n = 0; n < ucFN; n++)
            {
                eCmd  = psAsk1CfgOne->eCmd[n];
                usCmd = (UINT16)eCmd;
                ucAfn = (usCmd & 0xFF00) >> 8;
                ucFn  = usCmd & 0xFF;

                if(ucAfn != AFN_0C_ASK1)
                {
                    #ifdef MT_DBG
                    DEBUG("emtTrans_afn04f38() Afn of Cmd is out of range ucAfn = %d", ucAfn);
                    #endif
                    return MT_ERR_OUTRNG;
                }

                // 求得该Fn对应的组及在该组的值
                ucTeam = ucmt_get_fn_team(ucFn);
                if(0xFF == ucTeam)
                {
                    #ifdef MT_DBG
                    DEBUG("emtTrans_afn04f38() ucTeam is out of range");
                    #endif
                    return MT_ERR_OUTRNG;
                }

                ucBit  = ucmt_get_fn_bit(ucFn);
                if(0xFF == ucBit)
                {
                    #ifdef MT_DBG
                    DEBUG("emtTrans_afn04f38() ucBit is out of range");
                    #endif
                    return MT_ERR_OUTRNG;
                }

                pTmp[ucTeam] |= ucBit;
                ucTMax = ((ucTMax > ucTeam) ?  ucTMax : ucTeam);
                
            }
            
            psAsk1CfgOne_f->ucN = (ucTMax + 1);
            pTmp  += (ucTMax + 3);

             // 计算长度
            usLen += (ucTMax + 3);

            // 确定下一个帧中变长部分指针
            psAsk1CfgOne_f = (sMtAsk1CfgOne_f*)pTmp;
    
        }       
    }
    else if(MT_TRANS_F2U == eTrans)
    {  
        ucTmp = psF->ucBig;
        if(MT_USER_CLASS_MIN > ucTmp || MT_USER_CLASS_MAX < ucTmp)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn04f38() ucBig is out of range ucBig = %d", ucTmp);
            #endif
            return MT_ERR_OUTRNG;
        }
    
        psU->ucBig = ucTmp;
        
        ucM = psF->ucM;
        if(MT_ASK_M_MIN > ucM || MT_ASK_M_MAX < ucM)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn04f38() ucM is out of range ucBig = %d", ucM);
            #endif
            return MT_ERR_OUTRNG;
        }

        psU->ucM = ucM;
        
        usLen = 2;
        pTmp  = (UINT8*)&(psF->sOne);

        for(m = 0; m < ucM; m++)
        {
           ucFN = 0;
           psAsk1CfgOne_f = (sMtAsk1CfgOne_f *)pTmp;
           psU->sOne[m].ucSmall = psAsk1CfgOne_f->ucSmall;
           
           ucN = psAsk1CfgOne_f->ucN;
           usLen += 2;
           pTmp  += 2;
           
           if(ucN > 31)
           {
              #ifdef MT_DBG
              DEBUG("emtTrans_afn04f38() ucN is out of range ucN = %d", ucN);
              #endif
              return MT_ERR_OUTRNG;
           }
           
           for(n = 0; n < ucN; n++)
           {
                ucFn = psAsk1CfgOne_f->ucF8n[n];

                if(0 == ucFn)
                {
                    continue;
                }

                for(i = 0; i < 8; i++)
                {
                    ucBit = (0x01 << i);

                    if(ucFn & ucBit)
                    {
                        // 合成命令
                        usCmd = ((AFN_0C_ASK1 << 8) | (8 * n + i + 1));
                        eCmd = (eMtCmd)usCmd;

                        //判断命令是否合法
                        eRet = eMtGetCmdInfor(eCmd, MT_DIR_S2M, &sCmdInfor);
                        if(MT_OK != eRet)
                        {
                            #ifdef MT_DBG
                            DEBUG("emtTrans_afn04f38() eCmd err = 0x%04X", eCmd);
                            #endif
                            return MT_ERR_NONE; 
                        }

                        psU->sOne[m].eCmd[ucFN++] = eCmd;
                    }
                }
            }
           
            psU->sOne[m].ucFN = ucFN;
           
            usLen += ucN;
            pTmp  += ucFN;
        }
    }
    else
    {
        return MT_ERR_PARA;
    }

    *pusfLen = usLen;
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn04f39
 功能描述  : F39：2类数据配置设置（在终端支持的2类数据配置内）
             CMD_AFN_4_F39_CFG_ASK_2
             CMD_AFN_A_F39_CFG_ASK_2
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月21日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f39(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if(!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn04f39() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    UINT8         ucFN   = 0;
    UINT8         ucN    = 0;
    UINT8         ucM    = 0;
    UINT8         ucTmp  = 0;
    UINT8         ucAfn  = 0;
    UINT8         ucFn   = 0;
    UINT8         ucTeam = 0;
    UINT8         ucTMax = 0;
    UINT8         ucBit  = 0;
    UINT16        usLen  = 0;
    UINT16        usCmd  = 0;
    INT32         i      = 0;
    INT32         m      = 0;
    INT32         n      = 0;
    eMtCmd        eCmd   = CMD_AFN_F_UNKOWN;
    eMtErr        eRet   = MT_OK;
    sMtCmdInfor   sCmdInfor;
    

    sMtAfn04f39     *psU    = (sMtAfn04f39*)psUser;
    sMtAfn04f39_f   *psF    = (sMtAfn04f39_f*)psFrame;
    sMtAsk2CfgOne_f *psAsk2CfgOne_f = NULL;
    sMtAsk2CfgOne   *psAsk2CfgOne   = NULL;
    UINT8           *pTmp   = NULL;
        
    if(MT_TRANS_U2F == eTrans)
    {
        ucTmp = psU->ucBig;
        if(MT_USER_CLASS_MIN > ucTmp || MT_USER_CLASS_MAX < ucTmp)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn04f39() ucBig is out of range ucBig = %d", ucTmp);
            #endif
            return MT_ERR_OUTRNG;
        }
    
        psF->ucBig = ucTmp;
        
        ucM = psU->ucM;
        if(MT_ASK_M_MIN > ucM || MT_ASK_M_MAX < ucM)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn04f39() ucM is out of range ucBig = %d", ucM);
            #endif
            return MT_ERR_OUTRNG;
        }

        psF->ucM = ucM;
        
        // 计算长度
        usLen = 2;
        
        psAsk2CfgOne_f = (sMtAsk2CfgOne_f *)&(psF->sOne[0]);
        pTmp = (UINT8*)&(psAsk2CfgOne_f->ucF8n[0]);

        for(m = 0; m < ucM; m++)
        {
            psAsk2CfgOne = &(psU->sOne[m]);
            psAsk2CfgOne_f->ucSmall = psAsk2CfgOne->ucSmall;
            ucFN = psAsk2CfgOne->ucFN;
            for(n = 0; n < ucFN; n++)
            {
                eCmd  = psAsk2CfgOne->eCmd[n];
                usCmd = (UINT16)eCmd;
                ucAfn = (usCmd & 0xFF00) >> 8;
                ucFn  = usCmd & 0xFF;

                if(ucAfn != AFN_0D_ASK2)
                {
                    #ifdef MT_DBG
                    DEBUG("emtTrans_afn04f39() Afn of Cmd is out of range ucAfn = %d", ucAfn);
                    #endif
                    return MT_ERR_OUTRNG;
                }

                // 求得该Fn对应的组及在该组的值
                ucTeam = ucmt_get_fn_team(ucFn);
                if(0xFF == ucTeam)
                {
                    #ifdef MT_DBG
                    DEBUG("emtTrans_afn04f39() ucTeam is out of range");
                    #endif
                    return MT_ERR_OUTRNG;
                }

                ucBit  = ucmt_get_fn_bit(ucFn);
                if(0xFF == ucBit)
                {
                    #ifdef MT_DBG
                    DEBUG("emtTrans_afn04f39() ucBit is out of range");
                    #endif
                    return MT_ERR_OUTRNG;
                }

                pTmp[ucTeam] |= ucBit;
                ucTMax = ((ucTMax > ucTeam) ?  ucTMax : ucTeam);
                
            }
            
            psAsk2CfgOne_f->ucN = (ucTMax + 1);
            pTmp  += (ucTMax + 3);

             // 计算长度
            usLen += (ucTMax + 3);

            // 确定下一个帧中变长部分指针
            psAsk2CfgOne_f = (sMtAsk2CfgOne_f*)pTmp;
    
        }       
    }
    else if(MT_TRANS_F2U == eTrans)
    {  
        ucTmp = psF->ucBig;
        if(MT_USER_CLASS_MIN > ucTmp || MT_USER_CLASS_MAX < ucTmp)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn04f39() ucBig is out of range ucBig = %d", ucTmp);
            #endif
            return MT_ERR_OUTRNG;
        }
    
        psU->ucBig = ucTmp;
        
        ucM = psF->ucM;
        if(MT_ASK_M_MIN > ucM || MT_ASK_M_MAX < ucM)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn04f39() ucM is out of range ucBig = %d", ucM);
            #endif
            return MT_ERR_OUTRNG;
        }

        psU->ucM = ucM;
        
        usLen = 2;
        pTmp  = (UINT8*)&(psF->sOne);

        for(m = 0; m < ucM; m++)
        {
           ucFN = 0;
           psAsk2CfgOne_f = (sMtAsk2CfgOne_f *)pTmp;
           psU->sOne[m].ucSmall = psAsk2CfgOne_f->ucSmall;
           
           ucN = psAsk2CfgOne_f->ucN;
           usLen += 2;
           pTmp  += 2;
           
           if(ucN > 31)
           {
              #ifdef MT_DBG
              DEBUG("emtTrans_afn04f39() ucN is out of range ucN = %d", ucN);
              #endif
              return MT_ERR_OUTRNG;
           }
           
           for(n = 0; n < ucN; n++)
           {
                ucFn = psAsk2CfgOne_f->ucF8n[n];
                printf("ucFn[%d] = 0x%02X\n", n, ucFn);

                if(0 == ucFn)
                {
                    continue;
                }

                for(i = 0; i < 8; i++)
                {
                    ucBit = (0x01 << i);

                    if(ucFn & ucBit)
                    {
                        // 合成命令
                        usCmd = ((AFN_0D_ASK2 << 8) | (8 * n + i + 1));
                        eCmd = (eMtCmd)usCmd;

                        //判断命令是否合法
                        eRet = eMtGetCmdInfor(eCmd, MT_DIR_S2M, &sCmdInfor);
                        if(MT_OK != eRet)
                        {
                            #ifdef MT_DBG
                            DEBUG("emtTrans_afn04f39() eCmd err = 0x%04X", eCmd);
                            #endif
                           // return MT_ERR_NONE; 
                        }

                        psU->sOne[m].eCmd[ucFN++] = eCmd;
                    }
                }
            }
           
            psU->sOne[m].ucFN = ucFN;
           
            usLen += ucN;
            pTmp  += ucFN;
        }
    }
    else
    {
        return MT_ERR_PARA;
    }

    *pusfLen = usLen;
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn04f41
 功能描述  : F41：时段功控定值
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月21日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f41(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
     //*pusfLen = usLen;
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn04f42
 功能描述  : F42：厂休功控参数
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月21日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f42(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
     *pusfLen = 6;
    return MT_OK;
}


/*****************************************************************************
 函 数 名  : emtTrans_afn04f42
 功能描述  : F43：功率控制的功率计算滑差时间
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月21日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f43(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
     *pusfLen = 1;
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn04f44
 功能描述  : F44：营业报停控参数
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月21日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f44(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
     *pusfLen = 8;
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn04f45
 功能描述  : F45：功控轮次设定
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月21日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f45(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
     *pusfLen = 1;
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn04f46
 功能描述  : F46：月电量控定值
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月21日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f46(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
     *pusfLen = 4;
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn04f47
 功能描述  : F47：购电量（费）控参数
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月21日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f47(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
     *pusfLen = 17;
    return MT_OK;
}


/*****************************************************************************
 函 数 名  : emtTrans_afn04f48
 功能描述  : F48：电控轮次设定
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月21日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f48(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
     *pusfLen = 1;
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn04f49
 功能描述  : F49：功控告警时间
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月21日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f49(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
     *pusfLen = 1;
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn04f57
 功能描述  : F57：终端声音告警允许∕禁止设置
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月21日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f57(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
     *pusfLen = 3;
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn04f58
 功能描述  : F58：终端自动保电参数
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月21日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f58(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
     *pusfLen = 4;
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn04f59
 功能描述  : F59：电能表异常判别阈值设定
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月21日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f59(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
     *pusfLen = 4;
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn04f60
 功能描述  : F60：谐波限值
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月21日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f60(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
     *pusfLen = 2 * (4+19*2);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn04f61
 功能描述  : F61：直流模拟量接入参数
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月21日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f61(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
     *pusfLen = 1;
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn04f65
 功能描述  : F65：定时上报1类数据任务设置
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月21日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f65(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if(!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn04f65() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    UINT8         ucTmp  = 0;
    eMtErr        eRet   = MT_OK;
    sMtAfn04f65     *psU    = (sMtAfn04f65*)psUser;
    sMtAfn04f65_f   *psF    = (sMtAfn04f65_f*)psFrame;
        
    if(MT_TRANS_U2F == eTrans)
    {
        switch(psU->eUnit)
        {
            case MT_RPT_mm:
                ucTmp = 0;
                break;
                
            case MT_RPT_HH:
                ucTmp = 1;
                break;

            case MT_RPT_DD:
                ucTmp = 2;
                break;

            case MT_RPT_MM:
                ucTmp = 3;
                break;

            default:
                #ifdef MT_DBG
                DEBUG("emtTrans_afn04f65() eUint is out range");
                #endif
                return MT_ERR_OUTRNG;    
                //break;

        }
        
        psF->ucUnit = ucTmp;

        // ucPeriod 
        ucTmp = psU->ucPeriod;
        if(MT_PRD_MIN > ucTmp ||MT_PRD_MAX < ucTmp)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn04f65() eUint is out range");
            #endif
            return MT_ERR_OUTRNG; 
        }

        psF->ucPeriod = ucTmp;

        // sTime
        eRet = emt_trans_YYWWMMDDhhmmss(eTrans, &(psU->sTime), &(psF->sTime));
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn04f65() emt_trans_YYWWMMDDhhmmss is error eRet = %d", eRet);
            #endif
            return eRet; 
        }

        // ucR
        ucTmp = psU->ucR;
        if(MT_R_MIN > ucTmp || MT_R_MAX < ucTmp)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn04f65() ucR is out range");
            #endif
            return MT_ERR_OUTRNG; 
        }

        psF->ucR = ucTmp;

        // 命令标识
        eRet = emt_cmdpn_to_dadt(AFN_0C_ASK1, &(psF->sDaDt[0]),  &(psF->ucN), &(psU->sCmdPn[0]), psU->ucN); 
        if(MT_OK != eRet)
        {           
            #ifdef MT_DBG
            DEBUG("emtTrans_afn04f65() emt_cmdpn_to_dadt() error = %d", eRet);
            #endif
            return eRet;
        }

    }
    else if(MT_TRANS_F2U == eTrans)
    {
        ucTmp = psF->ucUnit;
        switch(ucTmp)
        {
            case 0:
                psU->eUnit = MT_RPT_mm;
                break;
                
            case 1:
                psU->eUnit = MT_RPT_HH;
                break;

            case 2:
                psU->eUnit = MT_RPT_DD;
                break;

            case 3:
                psU->eUnit = MT_RPT_MM;
                break;

            default:
                #ifdef MT_DBG
                DEBUG("emtTrans_afn04f65() ucUnit is out range");
                #endif
                return MT_ERR_OUTRNG;    
                //break;

        }
        
        // ucPeriod 
        ucTmp = psF->ucPeriod;
        if(MT_PRD_MIN > ucTmp ||MT_PRD_MAX < ucTmp)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn04f65() eUint is out range");
            #endif
            return MT_ERR_OUTRNG; 
        }

        psU->ucPeriod = ucTmp;

        // sTime
        eRet = emt_trans_YYWWMMDDhhmmss(eTrans, &(psU->sTime), &(psF->sTime));
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn04f65() emt_trans_YYWWMMDDhhmmss is error eRet = %d", eRet);
            #endif
            return eRet; 
        }

        // ucR
        ucTmp = psF->ucR;
        if(MT_R_MIN > ucTmp || MT_R_MAX < ucTmp)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn04f65() ucR is out range");
            #endif
            return MT_ERR_OUTRNG; 
        }

        psU->ucR = ucTmp;

        // 命令标识
        eRet = emt_dadt_to_cmdpn(AFN_0C_ASK1, &(psF->sDaDt[0]),  psF->ucN, &(psU->sCmdPn[0]), &(psU->ucN)); 
        if(MT_OK != eRet)
        {           
            #ifdef MT_DBG
            DEBUG("emtTrans_afn04f65() emt_dadt_to_cmdpn() error = %d", eRet);
            #endif
            return eRet;
        }     
    }
    else
    {
        return MT_ERR_PARA;
    }

    *pusfLen = sizeof(UINT8) * 3 + sizeof(sMtFmt01_f) +  psF->ucN * sizeof(sMtDaDt);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn04f66
 功能描述  : F66：定时上报2类数据任务设置
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月21日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f66(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if(!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn04f66() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    UINT8         ucTmp  = 0;
    eMtErr        eRet   = MT_OK;
    sMtAfn04f66     *psU    = (sMtAfn04f66*)psUser;
    sMtAfn04f66_f   *psF    = (sMtAfn04f66_f*)psFrame;
        
    if(MT_TRANS_U2F == eTrans)
    {
        switch(psU->eUnit)
        {
            case MT_RPT_mm:
                ucTmp = 0;
                break;
                
            case MT_RPT_HH:
                ucTmp = 1;
                break;

            case MT_RPT_DD:
                ucTmp = 2;
                break;

            case MT_RPT_MM:
                ucTmp = 3;
                break;

            default:
                #ifdef MT_DBG
                DEBUG("emtTrans_afn04f66() eUint is out range");
                #endif
                return MT_ERR_OUTRNG;    
                //break;

        }
        
        psF->ucUnit = ucTmp;

        // ucPeriod 
        ucTmp = psU->ucPeriod;
        if(MT_PRD_MIN > ucTmp ||MT_PRD_MAX < ucTmp)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn04f66() eUint is out range");
            #endif
            return MT_ERR_OUTRNG; 
        }

        psF->ucPeriod = ucTmp;

        // sTime
        eRet = emt_trans_YYWWMMDDhhmmss(eTrans, &(psU->sTime), &(psF->sTime));
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn04f66() emt_trans_YYWWMMDDhhmmss is error eRet = %d", eRet);
            #endif
            return eRet; 
        }

        // ucR
        ucTmp = psU->ucR;
        if(MT_R_MIN > ucTmp || MT_R_MAX < ucTmp)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn04f66() ucR is out range");
            #endif
            return MT_ERR_OUTRNG; 
        }

        psF->ucR = ucTmp;

        // 命令标识
        eRet = emt_cmdpn_to_dadt(AFN_0D_ASK2, &(psF->sDaDt[0]),  &(psF->ucN), &(psU->sCmdPn[0]), psU->ucN); 
        if(MT_OK != eRet)
        {           
            #ifdef MT_DBG
            DEBUG("emtTrans_afn04f66() emt_cmdpn_to_dadt() error = %d", eRet);
            #endif
            return eRet;
        }

    }
    else if(MT_TRANS_F2U == eTrans)
    {
        ucTmp = psF->ucUnit;
        switch(ucTmp)
        {
            case 0:
                psU->eUnit = MT_RPT_mm;
                break;
                
            case 1:
                psU->eUnit = MT_RPT_HH;
                break;

            case 2:
                psU->eUnit = MT_RPT_DD;
                break;

            case 3:
                psU->eUnit = MT_RPT_MM;
                break;

            default:
                #ifdef MT_DBG
                DEBUG("emtTrans_afn04f66() ucUnit is out range");
                #endif
                return MT_ERR_OUTRNG;    
                //break;

        }
        
        // ucPeriod 
        ucTmp = psF->ucPeriod;
        if(MT_PRD_MIN > ucTmp ||MT_PRD_MAX < ucTmp)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn04f66() eUint is out range");
            #endif
            return MT_ERR_OUTRNG; 
        }

        psU->ucPeriod = ucTmp;

        // sTime
        eRet = emt_trans_YYWWMMDDhhmmss(eTrans, &(psU->sTime), &(psF->sTime));
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn04f66() emt_trans_YYWWMMDDhhmmss is error eRet = %d", eRet);
            #endif
            return eRet; 
        }

        // ucR
        ucTmp = psF->ucR;
        if(MT_R_MIN > ucTmp || MT_R_MAX < ucTmp)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn04f66() ucR is out range");
            #endif
            return MT_ERR_OUTRNG; 
        }

        psU->ucR = ucTmp;

        // 命令标识
        eRet = emt_dadt_to_cmdpn(AFN_0D_ASK2, &(psF->sDaDt[0]),  psF->ucN, &(psU->sCmdPn[0]), &(psU->ucN)); 
        if(MT_OK != eRet)
        {           
            #ifdef MT_DBG
            DEBUG("emtTrans_afn04f66() emt_dadt_to_cmdpn() error = %d", eRet);
            #endif
            return eRet;
        }     
    }
    else
    {
        return MT_ERR_PARA;
    }

    *pusfLen = sizeof(UINT8) * 3 + sizeof(sMtFmt01_f) +  psF->ucN * sizeof(sMtDaDt);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn04f67
 功能描述  : F67：定时上报1类数据任务启动/停止设置
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月21日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f67(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if(!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn04f67() pointer is null");
        #endif
        return MT_ERR_NULL;
    }
    
    sMtAfn04f67     *psU    = (sMtAfn04f67*)psUser;
    sMtAfn04f67_f   *psF    = (sMtAfn04f67_f*)psFrame;
    
    if(MT_TRANS_U2F == eTrans)
    {
        switch(psU->eGo)
        {
            case MT_AUTO_GO:
                psF->ucGo = 0x55;
                break;
                
            case MT_AUTO_STOP:
                psF->ucGo = 0xAA;
                break;

            default:
                 #ifdef MT_DBG
                 DEBUG("emtTrans_afn04f67() eGo is out range");
                 #endif
                 return MT_ERR_OUTRNG;   
                 //break;
        }
    }
    else if(MT_TRANS_F2U == eTrans)
    {
        if(0x55 == psF->ucGo)
        {
            psU->eGo = MT_AUTO_GO;
        }
        else if(0xAA == psF->ucGo)
        {
            psU->eGo = MT_AUTO_STOP;
        }   
        else
        {
            psU->eGo = MT_AUTO_UNKOWN;
        }
    }
    else
    {
       return MT_ERR_PARA;
    }
      
    *pusfLen = sizeof(sMtAfn04f67_f);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn04f68
 功能描述  : F68：定时上报2类数据任务启动/停止设置
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月21日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f68(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if(!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn04f68() pointer is null");
        #endif
        return MT_ERR_NULL;
    }
    
    sMtAfn04f68     *psU    = (sMtAfn04f68*)psUser;
    sMtAfn04f68_f   *psF    = (sMtAfn04f68_f*)psFrame;
    
    if(MT_TRANS_U2F == eTrans)
    {
        switch(psU->eGo)
        {
            case MT_AUTO_GO:
                psF->ucGo = 0x55;
                break;
                
            case MT_AUTO_STOP:
                psF->ucGo = 0xAA;
                break;

            default:
                 #ifdef MT_DBG
                 DEBUG("emtTrans_afn04f68() eGo is out range");
                 #endif
                 return MT_ERR_OUTRNG;   
                 //break;
        }
    }
    else if(MT_TRANS_F2U == eTrans)
    {
        if(0x55 == psF->ucGo)
        {
            psU->eGo = MT_AUTO_GO;
        }
        else if(0xAA == psF->ucGo)
        {
            psU->eGo = MT_AUTO_STOP;
        }   
        else
        {
            psU->eGo = MT_AUTO_UNKOWN;
        }
    }
    else
    {
       return MT_ERR_PARA;
    }
      
    *pusfLen = sizeof(sMtAfn04f68_f);
    return MT_OK;
}


/*****************************************************************************
 函 数 名  : emtTrans_afn04f73
 功能描述  : F73：电容器参数
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月21日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f73(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
     *pusfLen = 16 * 3;
    return MT_OK;
}


/*****************************************************************************
 函 数 名  : emtTrans_afn04f74
 功能描述  : F74：电容器投切运行参数
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月21日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f74(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
     *pusfLen = 10;
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn04f75
 功能描述  : F75：电容器保护参数
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月21日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f75(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
     *pusfLen = 16;
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn04f76
 功能描述  : F76：电容器投切控制方式
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月21日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f76(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
     *pusfLen = 1;
    return MT_OK;
}


/*****************************************************************************
 函 数 名  : emtTrans_afn04f81
 功能描述  : F81：直流模拟量输入变比
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月21日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f81(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
     *pusfLen = 4;
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn04f82
 功能描述  : F82：直流模拟量限值
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月21日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f82(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
     *pusfLen = 4;
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn04f83
 功能描述  : F83：直流模拟量冻结参数
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月21日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn04f83(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
     *pusfLen = 1;
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn05f1
 功能描述  : F1：遥控跳闸
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月21日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn05f1(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    *pusfLen = sizeof(sMtRemoteTurnOff_f);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn05f9
 功能描述  : F9：时段功控投入
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月21日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn05f9(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    *pusfLen = sizeof(sMtAfn05F9_f);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn05f12
 功能描述  : F12：当前功率下浮控投入
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月21日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn05f12(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    *pusfLen = sizeof(sMtAfn05F12_f);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn05f12
 功能描述  : F25：终端保电投入 CMD_AFN_5_F25_TML_PROTECT_GO
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月21日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn05f25(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    *pusfLen = 1;
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn05f31
 功能描述  : F31：对时命令
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月21日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn05f31(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if (!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn05f31() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    eMtErr              eRet = MT_OK;
    sMtYYWWMMDDhhmmss   *psU = (sMtYYWWMMDDhhmmss*)psUser;
    sMtYYWWMMDDhhmmss_f *psF = (sMtYYWWMMDDhhmmss_f*)psFrame;

    if (MT_TRANS_F2U == eTrans) // 帧侧到用户侧
    {

    }
    else if ( MT_TRANS_U2F == eTrans)  // 用户侧到帧侧
    {

    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn05f31() para error!");
        #endif
        return MT_ERR_PARA;
    }

    eRet = emt_trans_YYWWMMDDhhmmss(eTrans, psU, psF);
    if(MT_OK != eRet)
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_YYWWMMDDhhmmss() para error = %d", eRet);
        #endif
        return eRet;
    }
    
 // 计算在帧侧的字节长度
 *pusfLen = sizeof(sMtYYWWMMDDhhmmss_f);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn05f32
 功能描述  : F32：中文信息
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月21日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn05f32(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if (!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn05f32() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    //eMtErr        eRet  = MT_OK;
    UINT8         ucLen = 0;
    sMtAfn05F32   *psU  = (sMtAfn05F32*)psUser;
    sMtAfn05F32_f *psF  = (sMtAfn05F32_f*)psFrame;

    if (MT_TRANS_F2U == eTrans) // 帧侧到用户侧
    {
        ucLen = psF->ucLen;

    }
    else if ( MT_TRANS_U2F == eTrans)  // 用户侧到帧侧
    {
        ucLen = psU->ucLen;
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn05f32() para error!");
        #endif
        return MT_ERR_PARA;
    }
    
    // 计算在帧侧的字节长度
    *pusfLen = sizeof(UINT8) * (2 + ucLen);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn05f41
 功能描述  : F41：电容器控制投入
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月22日 星期四
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn05f41(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{ 
    // 计算在帧侧的字节长度
    *pusfLen = 2;
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn05f42
 功能描述  : F42：电容器控制切除
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月22日 星期四
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn05f42(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{ 
    // 计算在帧侧的字节长度
    *pusfLen = 2;
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn05f49
 功能描述  : F49：命令指定通信端口暂停抄表
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月22日 星期四
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn05f49(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{ 
    // 计算在帧侧的字节长度
    *pusfLen = 2;
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn05f50
 功能描述  : F50：命令指定通信端口恢复抄表
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月22日 星期四
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn05f50(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{ 
    // 计算在帧侧的字节长度
    *pusfLen = 2;
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn05f51
 功能描述  : F51：命令指定通信端口重新抄表
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月22日 星期四
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn05f51(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{ 
    // 计算在帧侧的字节长度
    *pusfLen = 2;
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn05f52
 功能描述  : F52：初始化指定通信端口下的全部中继路由信息
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月22日 星期四
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn05f52(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{ 
    // 计算在帧侧的字节长度
    *pusfLen = 2;
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn05f53
 功能描述  : F53：删除指定通信端口下的全部电表
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月22日 星期四
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn05f53(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{ 
    // 计算在帧侧的字节长度
    *pusfLen = 2;
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn06f1
 功能描述  : F1：身份认证请求
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月23日 星期五
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn06f1(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{ 
    // 计算在帧侧的字节长度
    *pusfLen = 16;
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn06f2
 功能描述  : F2：身份认证响应
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月23日 星期五
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn06f2(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{ 
    // 计算在帧侧的字节长度
    *pusfLen = 16;
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn06f3
 功能描述  : F3：取随机数
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月23日 星期五
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn06f3(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{ 
    // 计算在帧侧的字节长度
    *pusfLen = 16;
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn06f4
 功能描述  : F4：取随机数响应
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月23日 星期五
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn06f4(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{ 
    // 计算在帧侧的字节长度
    *pusfLen = 16;
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn09f1
 功能描述  : F1：终端版本信息
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月23日 星期五
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn09f1(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if(!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn09f1() pointer is null");
        #endif
        return MT_ERR_NULL;
    }
 
    sMtAfn09F1_f * psF  = (sMtAfn09F1_f*)psFrame;
    sMtAfn09F1*    psU  = (sMtAfn09F1*)psUser;
    eMtErr         eRet = MT_OK; 
  
    if(MT_TRANS_U2F == eTrans)
    {
        memcpy(psF->ucFactoryID, psU->ucFactoryID, 4);
        memcpy(psF->ucDeviceID, psU->ucDeviceID, 8);
        memcpy(psF->ucSoftwareID, psU->ucSoftwareID, 4);
        memcpy(psF->ucInfoCodeCap, psU->ucInfoCodeCap, 11);
        memcpy(psF->ucProtolID, psU->ucProtolID, 4);
        memcpy(psF->ucHardWareID, psU->ucHardWareID, 4);
        eRet = emt_trans_YYMMDD(eTrans, &(psU->sDateSoftware), &(psF->sDateSoftware));
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn09f1() emt_trans_YYMMDD() error = %d", eRet);
            #endif
            return eRet;
        }

        eRet = emt_trans_YYMMDD(eTrans, &(psU->sDateHardware), &(psF->sDateHardware));
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn09f1() emt_trans_YYMMDD() error = %d", eRet);
            #endif
            return eRet;
        }
    }
    else if(MT_TRANS_F2U == eTrans)
    {
        memcpy(psU->ucFactoryID, psF->ucFactoryID, 4);
        memcpy(psU->ucDeviceID, psF->ucDeviceID, 8);
        memcpy(psU->ucSoftwareID, psF->ucSoftwareID, 4);
        memcpy(psU->ucInfoCodeCap, psF->ucInfoCodeCap, 11);
        memcpy(psU->ucProtolID, psF->ucProtolID, 4);
        memcpy(psU->ucHardWareID, psF->ucHardWareID, 4);
        eRet = emt_trans_YYMMDD(eTrans, &(psU->sDateSoftware), &(psF->sDateSoftware));
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn09f1() emt_trans_YYMMDD() error = %d", eRet);
            #endif
            return eRet;
        }

        eRet = emt_trans_YYMMDD(eTrans, &(psU->sDateHardware), &(psF->sDateHardware));
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn09f1() emt_trans_YYMMDD() error = %d", eRet);
            #endif
            return eRet;
        }
    }
    else
    {
        return MT_ERR_PARA;
    }

    *pusfLen = sizeof(sMtAfn09F1_f);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn09f2
 功能描述  : F2：终端支持的输入、输出及通信端口配置
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月23日 星期五
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn09f2(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if(!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn09f2() pointer is null");
        #endif
        return MT_ERR_NULL;
    }
 
    sMtAfn09F2_f * psF   = (sMtAfn09F2_f*)psFrame;
    sMtAfn09F2*    psU   = (sMtAfn09F2*)psUser;
    UINT8          ucTmp = 0;
    UINT16         usTmp = 0;
    INT32          i     = 0;
    UINT8          ucN   = 0;
    
  
    if(MT_TRANS_U2F == eTrans)
    {
        // ucPulseInputNum
        ucTmp = psU->ucPulseInputNum;
        if(ucTmp > MT_CIRCLE_MAX)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn09f2() PulseInputNum is out range %d", ucTmp);
            #endif            
            return MT_ERR_OUTRNG;
        }

        psF->ucPulseInputNum = ucTmp;

        // ucSwitchInputNum
        ucTmp = psU->ucSwitchInputNum;
        if(ucTmp > MT_CIRCLE_MAX)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn09f2() SwitchInputNum is out range %d", ucTmp);
            #endif            
            return MT_ERR_OUTRNG;
        }

        psF->ucSwitchInputNum = ucTmp;

        // ucDcNum
        ucTmp = psU->ucDcNum;
        if(ucTmp > MT_CIRCLE_MAX)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn09f2() ucDcNum is out range %d", ucTmp);
            #endif            
            return MT_ERR_OUTRNG;
        }

        psF->ucDcNum = ucTmp;

        // ucSwitchOutputNum
        ucTmp = psU->ucSwitchOutputNum;
        if(ucTmp > MT_CIRCLE_MAX)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn09f2() ucSwitchOutputNum is out range %d", ucTmp);
            #endif            
            return MT_ERR_OUTRNG;
        }

        psF->ucSwitchOutputNum = ucTmp;

        // usReadMeterNum
        usTmp = psU->usReadMeterNum;
        if(usTmp > MT_READ_METER_MAX)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn09f2() usReadMeterNum is out range %d", usTmp);
            #endif            
            return MT_ERR_OUTRNG;
        }

        psF->usReadMeterNum = usTmp;

        // usTmlRecvBuf
        usTmp = psU->usTmlRecvBuf;
        if(usTmp > MT_SEND_REC_MAX || usTmp < MT_SEND_REC_MIN)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn09f2() usTmlRecvBuf is out range %d", usTmp);
            #endif            
            return MT_ERR_OUTRNG;
        }

        psF->usTmlRecvBuf = usTmp;

        // usTmlSendBuf
        usTmp = psU->usTmlSendBuf;
        if(usTmp > MT_SEND_REC_MAX || usTmp < MT_SEND_REC_MIN)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn09f2() usTmlSendBuf is out range %d", usTmp);
            #endif            
            return MT_ERR_OUTRNG;
        }

        psF->usTmlSendBuf = usTmp;

        // MAC地址
        psF->sMacAddr = psU->sMacAddr;

        // PortNum
        ucTmp = psU->ucPortNum;
        if(ucTmp > MT_PORT_ID_MAX )
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn09f2() ucPortNum is out range %d", ucTmp);
            #endif            
            return MT_ERR_OUTRNG;
        }

        psF->ucPortNum = ucTmp;
        ucN            = ucTmp;
        // sOne
        for(i = 0; i < ucN; i++)
        {
            // port
           ucTmp = psU->sPort[i].ucPort;
           if(ucTmp > MT_PORT_ID_MAX || ucTmp < MT_PORT_ID_MIN)
           {
               #ifdef MT_DBG
               DEBUG("emtTrans_afn09f2() ucPort is out range %d", ucTmp);
               #endif            
               return MT_ERR_OUTRNG;
           }

           psF->sPort[i].sWord.ucPort = ucTmp;

           // eInter
           switch(psU->sPort[i].eInter)
           {
            case MT_INTER_RS485:
                psF->sPort[i].sWord.eInter = 0;
                break;
                
            case MT_INTER_RS232:
                psF->sPort[i].sWord.eInter = 1;
                break;
                
            case MT_INTER_SERIW:
                psF->sPort[i].sWord.eInter = 2;
                break;
                
           default:
                #ifdef MT_DBG
                DEBUG("emtTrans_afn09f2() eInter is out range %d", psU->sPort[i].eInter);
                #endif            
                return MT_ERR_OUTRNG;
                //break;
           }

           // bStandard
           if(TRUE == psU->sPort[i].bStandard)
           {
               psF->sPort[i].sWord.bStandard = 0;
           }
           else
           {
               psF->sPort[i].sWord.bStandard = 1;
           }

           // eUse
           switch(psU->sPort[i].eUse)
           {
            case MT_PORR_USE_EXP:
                psF->sPort[i].sWord.eUse = 0;
                break;     
                
            case MT_PORR_USE_CNG:
                psF->sPort[i].sWord.eUse = 1;
                break;
                
            case MT_PORR_USE_GTH:
                psF->sPort[i].sWord.eUse = 2;
                break;

            case MT_PORR_USE_SHR:
                psF->sPort[i].sWord.eUse = 3;
                break;   

                default:
                #ifdef MT_DBG
                DEBUG("emtTrans_afn09f2() eUse is out range %d", psU->sPort[i].eUse);
                #endif  
                return MT_ERR_OUTRNG;
                // break;
            }

            // ulBaud
            psF->sPort[i].ulBaud =  psU->sPort[i].ulBaud;

            //usDevNum
            usTmp = psU->sPort[i].usDevNum;
            if(usTmp > MT_READ_METER_MAX )
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn09f2() usDevNum is out range %d", usTmp);
                #endif            
                return MT_ERR_OUTRNG;
            }

            psF->sPort[i].usDevNum = usTmp;

            // usRecvBuf
            usTmp = psU->sPort[i].usRecvBuf;
            if(usTmp > MT_SEND_REC_MAX || usTmp < MT_SEND_REC_MIN)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn09f2() usRecvBuf is out range %d", usTmp);
                #endif            
                return MT_ERR_OUTRNG;
            }

            psF->sPort[i].usRecvBuf = usTmp;

            // usSendBuf
            usTmp = psU->sPort[i].usSendBuf;
            if(usTmp > MT_SEND_REC_MAX || usTmp < MT_SEND_REC_MIN)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn09f2() usSendBuf is out range %d", usTmp);
                #endif            
                return MT_ERR_OUTRNG;
            }

            psF->sPort[i].usSendBuf = usTmp;


        }
        
    }
    else if(MT_TRANS_F2U == eTrans)
    {
        // ucPulseInputNum
        ucTmp = psF->ucPulseInputNum;
        if(ucTmp > MT_CIRCLE_MAX)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn09f2() PulseInputNum is out range %d", ucTmp);
            #endif            
            return MT_ERR_OUTRNG;
        }

        psU->ucPulseInputNum = ucTmp;

        // ucSwitchInputNum
        ucTmp = psF->ucSwitchInputNum;
        if(ucTmp > MT_CIRCLE_MAX)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn09f2() SwitchInputNum is out range %d", ucTmp);
            #endif            
            return MT_ERR_OUTRNG;
        }

        psU->ucSwitchInputNum = ucTmp;

        // ucDcNum
        ucTmp = psF->ucDcNum;
        if(ucTmp > MT_CIRCLE_MAX)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn09f2() ucDcNum is out range %d", ucTmp);
            #endif            
            return MT_ERR_OUTRNG;
        }

        psU->ucDcNum = ucTmp;

        // ucSwitchOutputNum
        ucTmp = psF->ucSwitchOutputNum;
        if(ucTmp > MT_CIRCLE_MAX)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn09f2() ucSwitchOutputNum is out range %d", ucTmp);
            #endif            
            return MT_ERR_OUTRNG;
        }

        psU->ucSwitchOutputNum = ucTmp;

        // usReadMeterNum
        usTmp = psF->usReadMeterNum;
        if(usTmp > MT_READ_METER_MAX)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn09f2() usReadMeterNum is out range %d", usTmp);
            #endif            
            return MT_ERR_OUTRNG;
        }

        psU->usReadMeterNum = usTmp;

        // usTmlRecvBuf
        usTmp = psF->usTmlRecvBuf;
        if(usTmp > MT_SEND_REC_MAX || usTmp < MT_SEND_REC_MIN)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn09f2() usTmlRecvBuf is out range %d", usTmp);
            #endif            
            return MT_ERR_OUTRNG;
        }

        psU->usTmlRecvBuf = usTmp;

        // usTmlSendBuf
        usTmp = psF->usTmlSendBuf;
        if(usTmp > MT_SEND_REC_MAX || usTmp < MT_SEND_REC_MIN)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn09f2() usTmlSendBuf is out range %d", usTmp);
            #endif            
            return MT_ERR_OUTRNG;
        }

        psU->usTmlSendBuf = usTmp;

        // MAC地址
        psU->sMacAddr = psF->sMacAddr;

        // PortNum
        ucTmp = psF->ucPortNum;
        if(ucTmp > MT_PORT_ID_MAX )
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn09f2() ucPortNum is out range %d", ucTmp);
            #endif            
            return MT_ERR_OUTRNG;
        }

        psU->ucPortNum = ucTmp;
        ucN            = ucTmp;
        // sOne
        for(i = 0; i < ucN; i++)
        {
            // port
           ucTmp = psF->sPort[i].sWord.ucPort;
           if(ucTmp > MT_PORT_ID_MAX || ucTmp < MT_PORT_ID_MIN)
           {
               #ifdef MT_DBG
               DEBUG("emtTrans_afn09f2() ucPort is out range %d", ucTmp);
               #endif            
               return MT_ERR_OUTRNG;
           }

           psU->sPort[i].ucPort = ucTmp;

           // eInter
           switch(psF->sPort[i].sWord.eInter)
           {
            case 0:
                psU->sPort[i].eInter = MT_INTER_RS485;
                break;
                
            case 1:
                psU->sPort[i].eInter = MT_INTER_RS232;
                break;
                
            case 2:
                psU->sPort[i].eInter = MT_INTER_SERIW;
                break;
                
           default:
                #ifdef MT_DBG
                DEBUG("emtTrans_afn09f2() eInter is out range %d", psF->sPort[i].sWord.eInter);
                #endif            
                return MT_ERR_OUTRNG;
                //break;
           }

           // bStandard
           if(psF->sPort[i].sWord.bStandard == 0)
           {
               psU->sPort[i].bStandard = TRUE;
           }
           else
           {
               psU->sPort[i].bStandard = FALSE;
           }

           // eUse
           switch(psF->sPort[i].sWord.eUse)
           {
            case 0:
                psU->sPort[i].eUse = MT_PORR_USE_EXP;
                break;     
                
            case 1:
                psU->sPort[i].eUse = MT_PORR_USE_CNG;
                break;
                
            case 2:
                psU->sPort[i].eUse = MT_PORR_USE_GTH;
                break;

            case 3:              
                psU->sPort[i].eUse = MT_PORR_USE_SHR;
                break;   

                default:
                #ifdef MT_DBG
                DEBUG("emtTrans_afn09f2() eUse is out range %d", psF->sPort[i].sWord.eUse);
                #endif  
                return MT_ERR_OUTRNG;
                // break;
            }

            // ulBaud
            psU->sPort[i].ulBaud =  psF->sPort[i].ulBaud;

            //usDevNum
            usTmp = psF->sPort[i].usDevNum;
            if(usTmp > MT_READ_METER_MAX )
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn09f2() usDevNum is out range %d", usTmp);
                #endif            
                return MT_ERR_OUTRNG;
            }

            psU->sPort[i].usDevNum = usTmp;

            // usRecvBuf
            usTmp = psF->sPort[i].usRecvBuf;
            if(usTmp > MT_SEND_REC_MAX || usTmp < MT_SEND_REC_MIN)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn09f2() usRecvBuf is out range %d", usTmp);
                #endif            
                return MT_ERR_OUTRNG;
            }

            psU->sPort[i].usRecvBuf = usTmp;

            // usSendBuf
            usTmp = psF->sPort[i].usSendBuf;
            if(usTmp > MT_SEND_REC_MAX || usTmp < MT_SEND_REC_MIN)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn09f2() usSendBuf is out range %d", usTmp);
                #endif            
                return MT_ERR_OUTRNG;
            }

            psU->sPort[i].usSendBuf = usTmp;

        }
    }
    else
    {
        return MT_ERR_PARA;
    }

    *pusfLen = 17 +  sizeof(sMtPortOne_f) * ucN;
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn09f3
 功能描述  : F3：终端支持的其他配置
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月26日 星期一
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn09f3(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if(!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn09f3() pointer is null");
        #endif
        return MT_ERR_NULL;
    }
 
    sMtAfn09F3_f * psF   = (sMtAfn09F3_f*)psFrame;
    sMtAfn09F3*    psU   = (sMtAfn09F3*)psUser;
    UINT8          ucTmp = 0;
    UINT16         usTmp = 0;
    INT32          i     = 0;
    eMtErr         eRet  = MT_OK;
    
  
    if(MT_TRANS_U2F == eTrans)
    {
        // usMp
        usTmp = psU->usMp;
        if(usTmp > MT_PN_MAX)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn09f3() usMp is out range %d", usTmp);
            #endif 
            return MT_ERR_OUTRNG;
        }

        psF->usMp = usTmp;

        // ucGroup
        ucTmp = psU->ucGroup;
        if(ucTmp > MT_GROUP_MAX)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn09f3() ucGroup is out range %d", ucTmp);
            #endif 
            return MT_ERR_OUTRNG;
        }

        psF->ucGroup = ucTmp;

        // ucTask
        ucTmp = psU->ucTask;
        if(ucTmp > MT_TASK_MAX)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn09f3() ucTask is out range %d", ucTmp);
            #endif 
            return MT_ERR_OUTRNG;
        }

        psF->ucTask = ucTmp;

        // ucTask
        ucTmp = psU->ucTeam;
        if(ucTmp > 8)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn09f3() ucTeam is out range %d", ucTmp);
            #endif 
            return MT_ERR_OUTRNG;
        }

        psF->ucTeam = ucTmp;

        // eFrezMp
        eRet = emt_trans_fmt_freeze(eTrans, &(psU->eFrezMp), &ucTmp);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn09f3() emt_trans_fmt_freeze() error %d", eRet);
            #endif 
            return eRet;
        }
        
        psF->eFrezMp = ucTmp;

        // eFrezP
        eRet = emt_trans_fmt_freeze(eTrans, &(psU->eFrezP), &ucTmp);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn09f3() emt_trans_fmt_freeze() error %d", eRet);
            #endif 
            return eRet;
        }

        psF->eFrezP = ucTmp;
        
        // eFrezQ
        eRet = emt_trans_fmt_freeze(eTrans, &(psU->eFrezQ), &ucTmp);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn09f3() emt_trans_fmt_freeze() error %d", eRet);
            #endif 
            return eRet;
        }
        
        psF->eFrezQ = ucTmp;
        
        // eFrezPt
        eRet = emt_trans_fmt_freeze(eTrans, &(psU->eFrezPt), &ucTmp);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn09f3() emt_trans_fmt_freeze() error %d", eRet);
            #endif 
            return eRet;
        }
        
        psF->eFrezPt = ucTmp;
        
        // eFrezQt
        eRet = emt_trans_fmt_freeze(eTrans, &(psU->eFrezQt), &ucTmp);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn09f3() emt_trans_fmt_freeze() error %d", eRet);
            #endif 
            return eRet;
        }
        psF->eFrezQt = ucTmp;

        // ucDay
        ucTmp = psU->ucDay;
        if(ucTmp > MT_DAY_MAX)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn09f3() ucDay is out range %d", ucTmp);
            #endif 
            return MT_ERR_OUTRNG;
        }

        psF->ucDay = ucTmp;
        
        // ucMonth
        ucTmp = psU->ucMonth;
        if(ucTmp > MT_MONTH_MAX)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn09f3() ucMonth is out range %d", ucTmp);
            #endif 
            return MT_ERR_OUTRNG;
        }

        psF->ucMonth = ucTmp;

        // ucPCtrl
        ucTmp = psU->ucPCtrl;
        if(ucTmp > MT_PCTRL_MAX)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn09f3() ucDay is out range %d", ucTmp);
            #endif 
            return MT_ERR_OUTRNG;
        }

        psF->ucPCtrl = ucTmp;
        
        // ucHarm
        ucTmp = psU->ucHarm;
        if(ucTmp > MT_HARM_MAX)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn09f3() ucHarm is out range %d", ucTmp);
            #endif 
            return MT_ERR_OUTRNG;
        }

        psF->ucHarm = ucTmp;
        
        // ucCapa
        ucTmp = psU->ucCapa;
        if(ucTmp > MT_CAPA_MAX)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn09f3() ucCapa is out range %d", ucTmp);
            #endif 
            return MT_ERR_OUTRNG;
        }

        psF->ucCapa = ucTmp;

        // ucVip
        ucTmp = psU->ucVip;
        if(ucTmp > MT_CAPA_MAX)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn09f3() ucVip is out range %d", ucTmp);
            #endif 
            return MT_ERR_OUTRNG;
        }

        psF->ucVip = ucTmp;

        // bigclass
        usTmp = 0;
        for(i = 0; i < 16; i++)
        {
            if(TRUE == psU->bBig[i])
            {
                usTmp |= (0x01 << i);
            }
        }

        psF->bBig = usTmp;

        // ucSmall
        for(i = 0; i < 16; i++)
        {
            ucTmp = psU->ucSmall[i];
            if(ucTmp > MT_USER_CLASS_MAX)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn09f3() ucSmall is out range %d", ucTmp);
                #endif 
                return MT_ERR_OUTRNG;
            }

            psF->ucSmall[i] = ucTmp;
        }
    }
    else if(MT_TRANS_F2U == eTrans)
    {
        // usMp
        usTmp = psF->usMp;
        if(usTmp > MT_PN_MAX)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn09f3() usMp is out range %d", usTmp);
            #endif 
            return MT_ERR_OUTRNG;
        }

        psU->usMp = usTmp;

        // ucGroup
        ucTmp = psF->ucGroup;
        if(ucTmp > MT_GROUP_MAX)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn09f3() ucGroup is out range %d", ucTmp);
            #endif 
            return MT_ERR_OUTRNG;
        }

        psU->ucGroup = ucTmp;

        // ucTask
        ucTmp = psF->ucTask;
        if(ucTmp > MT_TASK_MAX)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn09f3() ucTask is out range %d", ucTmp);
            #endif 
            return MT_ERR_OUTRNG;
        }

        psU->ucTask = ucTmp;

        // ucTask
        ucTmp = psF->ucTeam;
        if(ucTmp > 8)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn09f3() ucTeam is out range %d", ucTmp);
            #endif 
            return MT_ERR_OUTRNG;
        }

        psU->ucTeam = ucTmp;

        // eFrezMp
        ucTmp = psF->eFrezMp;
        eRet = emt_trans_fmt_freeze(eTrans, &(psU->eFrezMp), &ucTmp);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn09f3() emt_trans_fmt_freeze() error %d", eRet);
            #endif 
            return eRet;
        }
        

        // eFrezP
        ucTmp = psF->eFrezP;
        eRet = emt_trans_fmt_freeze(eTrans, &(psU->eFrezP), &ucTmp);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn09f3() emt_trans_fmt_freeze() error %d", eRet);
            #endif 
            return eRet;
        }

        
        // eFrezQ
        ucTmp = psF->eFrezQ;
        eRet = emt_trans_fmt_freeze(eTrans, &(psU->eFrezQ), &ucTmp);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn09f3() emt_trans_fmt_freeze() error %d", eRet);
            #endif 
            return eRet;
        }
        
        // eFrezPt
        ucTmp = psF->eFrezPt;
        eRet = emt_trans_fmt_freeze(eTrans, &(psU->eFrezPt), &ucTmp);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn09f3() emt_trans_fmt_freeze() error %d", eRet);
            #endif 
            return eRet;
        }
                
        // eFrezQt
        ucTmp = psF->eFrezQt;
        eRet = emt_trans_fmt_freeze(eTrans, &(psU->eFrezQt), &ucTmp);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn09f3() emt_trans_fmt_freeze() error %d", eRet);
            #endif 
            return eRet;
        }

        // ucDay
        ucTmp = psF->ucDay;
        if(ucTmp > MT_DAY_MAX)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn09f3() ucDay is out range %d", ucTmp);
            #endif 
            return MT_ERR_OUTRNG;
        }

        psU->ucDay = ucTmp;
        
        // ucMonth
        ucTmp = psF->ucMonth;
        if(ucTmp > MT_MONTH_MAX)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn09f3() ucMonth is out range %d", ucTmp);
            #endif 
            return MT_ERR_OUTRNG;
        }

        psU->ucMonth = ucTmp;

        // ucPCtrl
        ucTmp = psF->ucPCtrl;
        if(ucTmp > MT_PCTRL_MAX)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn09f3() ucDay is out range %d", ucTmp);
            #endif 
            return MT_ERR_OUTRNG;
        }

        psU->ucPCtrl = ucTmp;
        
        // ucHarm
        ucTmp = psF->ucHarm;
        if(ucTmp > MT_HARM_MAX)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn09f3() ucHarm is out range %d", ucTmp);
            #endif 
            return MT_ERR_OUTRNG;
        }

        psU->ucHarm = ucTmp;
        
        // ucCapa
        ucTmp = psF->ucCapa;
        if(ucTmp > MT_CAPA_MAX)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn09f3() ucCapa is out range %d", ucTmp);
            #endif 
            return MT_ERR_OUTRNG;
        }

        psU->ucCapa = ucTmp;

        // ucVip
        ucTmp = psF->ucVip;
        if(ucTmp > MT_CAPA_MAX)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn09f3() ucVip is out range %d", ucTmp);
            #endif 
            return MT_ERR_OUTRNG;
        }

        psU->ucVip = ucTmp;

        // bigclass
        usTmp = psF->bBig;
        for(i = 0; i < 16; i++)
        {
            if(usTmp & (0x01 << i))
            {
               psU->bBig[i] = TRUE;
            }
        }

        // ucSmall
        for(i = 0; i < 16; i++)
        {
            ucTmp = psF->ucSmall[i];
            if(ucTmp > MT_USER_CLASS_MAX)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn09f3() ucSmall is out range %d", ucTmp);
                #endif 
                return MT_ERR_OUTRNG;
            }

            psU->ucSmall[i] = ucTmp;
        }
    }
    else
    {
        return MT_ERR_PARA;
    }

    *pusfLen = sizeof(sMtAfn09F3_f);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn09f4_ast
 功能描述  : F4：终端支持的参数配置 辅助函数
 输入参数  : eMtCmd eCmd      
             UINT8 *pArray    
             pucTeam 所属的信息组 
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : emtTrans_afn09f4
 
 修改历史      :
  1.日    期   : 2013年8月26日 星期一
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn09f4_ast(eMtTrans eTrans, eMtCmd eCmd, UINT8 *pArray, UINT8 *pucTeam)
{
    if(!pArray || !pucTeam)
    {
      return MT_ERR_NULL;
    }

    UINT8 ucN    = 0;
    UINT8 ucF    = 0;
    UINT8 ucSift = 0;
    
    switch(eCmd)
    {
        case CMD_AFN_4_F1_TML_UP_CFG:
        case CMD_AFN_4_F2_TML_WIRELESS_CFG:      
        case CMD_AFN_4_F3_MST_IP_PORT:
        case CMD_AFN_4_F4_MST_PHONE_SMS:    
        case CMD_AFN_4_F5_TML_UP_AUTH:
        case CMD_AFN_4_F6_TEAM_ADDR:    
        case CMD_AFN_4_F7_TML_IP_PORT:
        case CMD_AFN_4_F8_TML_UP_WAY:   
        case CMD_AFN_4_F9_TML_EVENT_CFG:
        case CMD_AFN_4_F10_TML_POWER_CFG:
        case CMD_AFN_4_F11_TML_PULSE_CFG:
        case CMD_AFN_4_F12_TML_STATE_INPUT:
        case CMD_AFN_4_F13_TML_SIMULA_CFG:
        case CMD_AFN_4_F14_TML_GRUP_TOTL:
        case CMD_AFN_4_F15_HAVE_DIFF_EVENT:
        case CMD_AFN_4_F16_VPN_USER_PWD:
        case CMD_AFN_4_F17_TML_SAFE_VALUE:    
        case CMD_AFN_4_F18_TML_PCTRL_PERD:
        case CMD_AFN_4_F19_TML_PCTRL_FACTOR:  
        case CMD_AFN_4_F20_TML_MONTH_FACTOR:
        case CMD_AFN_4_F21_TML_POWER_FACTOR:  
        case CMD_AFN_4_F22_TML_POWER_RATE:
        case CMD_AFN_4_F23_TML_WARNING_CFG:  
        case CMD_AFN_4_F25_MP_BASE_CFG:
        case CMD_AFN_4_F26_MP_LIMIT_CFG:
        case CMD_AFN_4_F27_MP_LOSS_CFG:
        case CMD_AFN_4_F28_MP_PERIOD_FACTOR:  
        case CMD_AFN_4_F29_TML_METER_ID:
        case CMD_AFN_4_F30_TML_AUTO_READ:
        case CMD_AFN_4_F31_SLAVE_ADDR:
        case CMD_AFN_4_F33_TML_READ_CFG:
        case CMD_AFN_4_F34_CON_DOWN_CFG:
        case CMD_AFN_4_F35_TML_READ_VIP:
        case CMD_AFN_4_F36_TML_UP_LIMIT:
        case CMD_AFN_4_F37_TML_CASC_CFG:
        case CMD_AFN_4_F38_CFG_ASK_1:
        case CMD_AFN_4_F39_CFG_ASK_2:
        case CMD_AFN_4_F41_PERIOD_VALUE:
        case CMD_AFN_4_F42_FACTORY_HOLIDAY:
        case CMD_AFN_4_F43_SLIDE_TIME:
        case CMD_AFN_4_F44_SHUTOUT_CFG:
        case CMD_AFN_4_F45_CTRL_TURN_CFG:
        case CMD_AFN_4_F46_MONTH_FIX_VALUE:
        case CMD_AFN_4_F47_BUY_COUNT:
        case CMD_AFN_4_F48_ELEC_TURN_CFG:
        case CMD_AFN_4_F49_WARNING_TIME:
        case CMD_AFN_4_F57_TML_WARN_SOUND:
        case CMD_AFN_4_F58_TML_ATUO_PROTECT:
        case CMD_AFN_4_F59_METER_LIMIT:
        case CMD_AFN_4_F60_HUMOR_LIMIT:
        case CMD_AFN_4_F61_DC_SIMULA:
        case CMD_AFN_4_F65_CFG_AUTO_1:
        case CMD_AFN_4_F66_CFG_AUTO_2:
        case CMD_AFN_4_F67_GOP_AUTO_1:
        case CMD_AFN_4_F68_GOP_AUTO_2:
        case CMD_AFN_4_F73_CAPA_CFG:
        case CMD_AFN_4_F74_CAPA_RUN_CFG:
        case CMD_AFN_4_F75_CAPA_PROT_PARA:
        case CMD_AFN_4_F76_CAPA_CTRL_WAY:
        case CMD_AFN_4_F81_CD_CHANGE_RATE:
        case CMD_AFN_4_F82_CD_LIMIT:
        case CMD_AFN_4_F83_CD_FREEZE_PARA:

        ucF = (UINT8)(eCmd & 0xFF);
        break;

        default:
        #ifdef MT_DBG
        DEBUG("emtTrans_afn09f4_ast() para cfg para is out range %X", eCmd);
        #endif 
        return MT_ERR_OUTRNG;
        //break;
    }

    if(MT_TRANS_U2F == eTrans)
    {
        ucSift       = (ucF - 1) % 8;
        ucN          = ucF / 8;
        *pucTeam     = ucN + 1;
        pArray[ucN] |=  (0x01 << ucSift);
    }

    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn09f4
 功能描述  : F4：终端支持的参数配置
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : eMtErr
 调用函数  : emtTrans_afn09f4_ast
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月26日 星期一
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn09f4(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if(!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn09f4() pointer is null");
        #endif
        return MT_ERR_NULL;
    }
 
    sMtAfn09F4_f * psF    = (sMtAfn09F4_f*)psFrame;
    sMtAfn09F4*    psU    = (sMtAfn09F4*)psUser;
    eMtErr         eRet   = MT_OK;
    eMtCmd         eCmd   = CMD_AFN_F_UNKOWN;
    UINT8         *pFlag  = NULL;
    UINT8          ucN    = 0;
    UINT8          ucNTmp = 0;
    UINT8          ucFnN  = 0;
    UINT8          ucTmp  = 0;
    INT32          i      = 0;
    INT32          j      = 0;    
    
    if(MT_TRANS_U2F == eTrans)
    {      
        pFlag = (UINT8*)&(psF->ucFlag[0]);
        for(i = 0; i < psU->ucNum; i++)
        {
            eCmd = psU->eCfgCmd[i];
            eRet = emtTrans_afn09f4_ast(eTrans, eCmd, pFlag, &ucNTmp);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn09f4() emtTrans_afn09f4_ast() error %d", eRet);
                #endif 
                return eRet;
            }

            // 取个数最大值
            ucN = (ucN > ucNTmp) ? ucN : ucNTmp;
        }
        
        psF->ucTeam = ucN;
    }
    else if(MT_TRANS_F2U == eTrans)
    {
        ucN = psF->ucTeam;
        
        for(i = 0; i < ucN; i++)
        {
            ucTmp = psF->ucFlag[i];

            for(j = 0; j < 8; j++)
            {
                if(ucTmp & (0x01 << j))
                {
                   eCmd = (eMtCmd)((8 * i + j + 1) | 0x0400);
                   eRet = emtTrans_afn09f4_ast(eTrans, eCmd, &ucNTmp, &ucNTmp);
                   if(MT_OK != eRet)
                   {
                       #ifdef MT_DBG
                       DEBUG("emtTrans_afn09f4() emtTrans_afn09f4_ast() error %d", eRet);
                       #endif 
                       return eRet;
                   }

                   psU->eCfgCmd[ucFnN++] = eCmd;
                }
            }
        }

       psU->ucNum =  ucFnN;
    }
    else
    {
        return MT_ERR_PARA;
    }

    *pusfLen = sizeof(UINT8) * (ucN + 1);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn09f5_ast
 功能描述  : F5：终端支持的控制配置 辅助函数
 输入参数  : eMtCmd eCmd      
             UINT8 *pArray    
             pucTeam 所属的信息组 
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : emtTrans_afn09f5
 
 修改历史      :
  1.日    期   : 2013年8月26日 星期一
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn09f5_ast(eMtTrans eTrans, eMtCmd eCmd, UINT8 *pArray, UINT8 *pucTeam)
{
    if(!pArray || !pucTeam)
    {
      return MT_ERR_NULL;
    }

    UINT8 ucN    = 0;
    UINT8 ucF    = 0;
    UINT8 ucSift = 0;
    
    switch(eCmd)
    {
        case CMD_AFN_5_F1_REMOTE_TURN_OFF:
        case CMD_AFN_5_F2_PERMIT_TURN_ON:      
        case CMD_AFN_5_F9_PERIOD_GO:
        case CMD_AFN_5_F10_HOLIDAY_GO:    
        case CMD_AFN_5_F11_SHUTOUT_GO:
        case CMD_AFN_5_F12_DOWN_GO:    
        case CMD_AFN_5_F15_MONTH_GO:
        case CMD_AFN_5_F16_BUY_GO:   
        case CMD_AFN_5_F17_PERIOD_STOP:
        case CMD_AFN_5_F18_HOLIDAY_STOP:
        case CMD_AFN_5_F19_SHUTOUT_STOP:
        case CMD_AFN_5_F20_DOWN_STOP:
        case CMD_AFN_5_F23_MONTH_STOP:
        case CMD_AFN_5_F24_BUY_STOP:
        case CMD_AFN_5_F25_TML_PROTECT_GO:
        case CMD_AFN_5_F26_WARN_PAY_GO:
        case CMD_AFN_5_F27_PERMIT_TALK_GO:    
        case CMD_AFN_5_F28_TAKE_OFF_TML_GO:
        case CMD_AFN_5_F29_AUTO_SAY_GO:  
        case CMD_AFN_5_F31_CHECK_TIME:
        case CMD_AFN_5_F32_CHINESE_INFO:  
        case CMD_AFN_5_F33_TML_PROTECT_STOP:
        case CMD_AFN_5_F34_WARN_PAY_STOP:  
        case CMD_AFN_5_F35_PERMIT_TALK_STOP:
        case CMD_AFN_5_F36_TAKE_OFF_TML_STOP:
        case CMD_AFN_5_F37_AUTO_SAY_STOP:
        case CMD_AFN_5_F38_TML_LINK_ON:
        case CMD_AFN_5_F39_TML_LINK_OFF:  
        case CMD_AFN_5_F41_CAPA_CTRL_GO:
        case CMD_AFN_5_F42_CAPA_CTRL_STOP:
        case CMD_AFN_5_F49_READ_METER_STOP:
        case CMD_AFN_5_F50_READ_METER_GO:
        case CMD_AFN_5_F51_READ_METER_REDO:
        case CMD_AFN_5_F52_INIT_ROUTE:
        case CMD_AFN_5_F53_DELET_ALL_METER:

        ucF = (UINT8)(eCmd & 0xFF);
        break;

        default:
        #ifdef MT_DBG
        DEBUG("emtTrans_afn09f5_ast() para cfg para is out range %X", eCmd);
        #endif 
        return MT_ERR_OUTRNG;
        //break;
    }

    if(MT_TRANS_U2F == eTrans)
    {
        ucSift       = (ucF - 1) % 8;
        ucN          = ucF / 8;
        *pucTeam     = ucN + 1;
        pArray[ucN] |=  (0x01 << ucSift);
    }
    
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn09f5
 功能描述  : F5：终端支持的控制配置
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : eMtErr
 调用函数  : emtTrans_afn09f5_ast
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月26日 星期一
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn09f5(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if(!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn09f5() pointer is null");
        #endif
        return MT_ERR_NULL;
    }
 
    sMtAfn09F5_f * psF    = (sMtAfn09F5_f*)psFrame;
    sMtAfn09F5*    psU    = (sMtAfn09F5*)psUser;
    eMtErr         eRet   = MT_OK;
    eMtCmd         eCmd   = CMD_AFN_F_UNKOWN;
    UINT8         *pFlag  = NULL;
    UINT8          ucN    = 0;
    UINT8          ucNTmp = 0;
    UINT8          ucFnN  = 0;
    UINT8          ucTmp  = 0;
    INT32          i      = 0;
    INT32          j      = 0;    
    
    if(MT_TRANS_U2F == eTrans)
    {      
        pFlag = (UINT8*)&(psF->ucFlag[0]);
        for(i = 0; i < psU->ucNum; i++)
        {
            eCmd = psU->eCfgCmd[i];
            eRet = emtTrans_afn09f5_ast(eTrans, eCmd, pFlag, &ucNTmp);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn09f5() emtTrans_afn09f5_ast() error %d", eRet);
                #endif 
                return eRet;
            }

            // 取个数最大值
            ucN = (ucN > ucNTmp) ? ucN : ucNTmp;
        }

        psF->ucTeam = ucN;

    }
    else if(MT_TRANS_F2U == eTrans)
    {
        ucN = psF->ucTeam;
        
        for(i = 0; i < ucN; i++)
        {
            ucTmp = psF->ucFlag[i];

            for(j = 0; j < 8; j++)
            {
                if(ucTmp & (0x01 << j))
                {
                   eCmd = (eMtCmd)((8 * i + j + 1) | 0x0500);
                   printf("cmd = %X\n", eCmd);
                   eRet = emtTrans_afn09f5_ast(eTrans, eCmd, &ucNTmp, &ucNTmp);
                   if(MT_OK != eRet)
                   {
                       #ifdef MT_DBG
                       DEBUG("emtTrans_afn09f5() emtTrans_afn09f5_ast() error %d", eRet);
                       #endif 
                       return eRet;
                   }

                   psU->eCfgCmd[ucFnN++] = eCmd;
                }
            }
        }

       psU->ucNum =  ucFnN;
    }
    else
    {
        return MT_ERR_PARA;
    }

    *pusfLen = sizeof(UINT8) * (ucN + 1);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn09f6_ast
 功能描述  : F6：终端支持的1类数据配置 辅助函数
 输入参数  : eMtCmd eCmd      
             UINT8 *pArray    
             pucTeam 所属的信息组 
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : emtTrans_afn09f5
 
 修改历史      :
  1.日    期   : 2013年8月26日 星期一
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn09f6_ast(eMtTrans eTrans, eMtCmd eCmd, UINT8 *pArray, UINT8 *pucTeam)
{
    if(!pArray || !pucTeam)
    {
      return MT_ERR_NULL;
    }

    UINT8 ucN    = 0;
    UINT8 ucF    = 0;
    UINT8 ucSift = 0;
    
    switch(eCmd)
    {
        case CMD_AFN_C_F2_TML_CLOCK:
        case CMD_AFN_C_F3_TML_PARA_STATE:      
        case CMD_AFN_C_F4_TML_UPCOM_STATE:
        case CMD_AFN_C_F5_TML_CTRL_SET:    
        case CMD_AFN_C_F6_TML_CTRL_STATE:
        case CMD_AFN_C_F7_TML_EC_VALUE:    
        case CMD_AFN_C_F8_TML_EVNT_FLAG:
        case CMD_AFN_C_F9_TML_STATE_BIT:   
        case CMD_AFN_C_F10_TML_MONTH_FLOW:
        case CMD_AFN_C_F11_TML_READ_METER:
        case CMD_AFN_C_F17_TOTAL_POWER_HAVE:
        case CMD_AFN_C_F18_TOTAL_POWER_NONE:
        case CMD_AFN_C_F19_DAY_POWER_HAVE:
        case CMD_AFN_C_F20_DAY_POWER_NONE:
        case CMD_AFN_C_F21_MONTH_POWER_HAVE:
        case CMD_AFN_C_F22_MONTH_POWER_NONE:
        case CMD_AFN_C_F23_TML_LEFT_POWER:    
        case CMD_AFN_C_F24_DOWN_FREZ_VALUE:
        case CMD_AFN_C_F25_POWER_RATE_CUR:  
        case CMD_AFN_C_F26_DROP_PHASE_LAST:
        case CMD_AFN_C_F27_METER_PROG_TIME:  
        case CMD_AFN_C_F28_METER_WORD_CBIT:
        case CMD_AFN_C_F29_COPER_IRON_CUR:  
        case CMD_AFN_C_F30_COPER_IRON_LAST:
        case CMD_AFN_C_F31_POWER_CUR:
        case CMD_AFN_C_F32_POWER_LAST:
        case CMD_AFN_C_F33_FRTH_POWR_P1P4_C:  
        case CMD_AFN_C_F34_BACK_POWR_P2P3_C:
        case CMD_AFN_C_F35_FRTH_DMND_M:
        case CMD_AFN_C_F36_BACK_DMND_M:
        case CMD_AFN_C_F37_FRTH_POWR_P1P4_L:
        case CMD_AFN_C_F38_BACK_POWR_P2P3_L:
        case CMD_AFN_C_F39_FRTH_DMND_L:
        case CMD_AFN_C_F40_BACK_DMND_L:
        case CMD_AFN_C_F41_FRTH_HAVE_POWR_D:
        case CMD_AFN_C_F42_FRTH_NONE_POWR_D:
        case CMD_AFN_C_F43_BACK_HAVE_POWR_D:
        case CMD_AFN_C_F44_BACK_NONE_POWR_D:
        case CMD_AFN_C_F45_FRTH_HAVE_POWR_M:
        case CMD_AFN_C_F46_FRTH_NONE_POWR_M:
        case CMD_AFN_C_F47_BACK_HAVE_POWR_M:
        case CMD_AFN_C_F48_BACK_NONE_POWR_M:
        case CMD_AFN_C_F49_CURT_PHASE_ANGLE:
        case CMD_AFN_C_F57_CURT_HARM_VALUE:
        case CMD_AFN_C_F58_CURT_HARM_RATE:
        case CMD_AFN_C_F65_CURT_CAPA_SWITCH:
        case CMD_AFN_C_F66_CURT_CAPA_TIME:
        case CMD_AFN_C_F67_CURT_CAPA_POWR:
        case CMD_AFN_C_F73_DC_RLTM_DATA:
        case CMD_AFN_C_F81_HFRZ_GRUP_RATE_HAVE:
        case CMD_AFN_C_F82_HFRZ_GRUP_RATE_NONE:
        case CMD_AFN_C_F83_HFRZ_GRUP_POWR_HAVE:
        case CMD_AFN_C_F84_HFRZ_GRUP_POWR_NONE:
        case CMD_AFN_C_F89_HFRZ_RATE_HAVE:
        case CMD_AFN_C_F90_HFRZ_RATE_HAVE_A:
        case CMD_AFN_C_F91_HFRZ_RATE_HAVE_B:
        case CMD_AFN_C_F92_HFRZ_RATE_HAVE_C:
        case CMD_AFN_C_F93_HFRZ_RATE_NONE:
        case CMD_AFN_C_F94_HFRZ_RATE_NONE_A:
        case CMD_AFN_C_F95_HFRZ_RATE_NONE_B:
        case CMD_AFN_C_F96_HFRZ_RATE_NONE_C:
        case CMD_AFN_C_F97_HFRZ_VOLT_A:
        case CMD_AFN_C_F99_HFRZ_VOLT_C:
        case CMD_AFN_C_F100_HFRZ_ELEC_A:
        case CMD_AFN_C_F101_HFRZ_ELEC_B:
        case CMD_AFN_C_F102_HFRZ_ELEC_C:
        case CMD_AFN_C_F103_HFRZ_ELEC_ZERO:
        case CMD_AFN_C_F105_HFRZ_FRTH_HAVE:
        case CMD_AFN_C_F106_HFRZ_FRTH_NONE:
        case CMD_AFN_C_F107_HFRZ_BACK_HAVE:
        case CMD_AFN_C_F108_HFRZ_BACK_NONE:
        case CMD_AFN_C_F109_HFRZ_FRTH_HAVE_S:
        case CMD_AFN_C_F110_HFRZ_FRTH_NONE_S:
        case CMD_AFN_C_F111_HFRZ_BACK_HAVE_S:
        case CMD_AFN_C_F112_HFRZ_BACK_NONE_S:
        case CMD_AFN_C_F113_HFRZ_FACT_TOTAL:
        case CMD_AFN_C_F114_HFRZ_FACT_A:
        case CMD_AFN_C_F115_HFRZ_FACT_B:
        case CMD_AFN_C_F116_HFRZ_FACT_C:
        case CMD_AFN_C_F121_HFRZ_DC_VALUE:
        case CMD_AFN_C_F129_FRTH_HAVE_POWR_C:
        case CMD_AFN_C_F130_FRTH_NONE_POWR_C:
        case CMD_AFN_C_F131_BACK_HAVE_POWR_C:
        case CMD_AFN_C_F132_BACK_NONE_POWR_C:
        case CMD_AFN_C_F133_NONE_POWR_P1_C:
        case CMD_AFN_C_F134_NONE_POWR_P2_C:
        case CMD_AFN_C_F135_NONE_POWR_P3_C:
        case CMD_AFN_C_F136_NONE_POWR_P4_C:
        case CMD_AFN_C_F137_FRTH_HAVE_POWR_L:
        case CMD_AFN_C_F138_FRTH_NONE_POWR_L:
        case CMD_AFN_C_F139_BACK_HAVE_POWR_L:
        case CMD_AFN_C_F140_BACK_NONE_POWR_L:
        case CMD_AFN_C_F141_NONE_POWR_P1_L:
        case CMD_AFN_C_F142_NONE_POWR_P2_L:
        case CMD_AFN_C_F143_NONE_POWR_P3_L:
        case CMD_AFN_C_F144_NONE_POWR_P4_L:
        case CMD_AFN_C_F145_FRTH_HAVE_DMND_C:
        case CMD_AFN_C_F146_FRTH_NONE_DMND_C:
        case CMD_AFN_C_F147_BACK_HAVE_DMND_C:
        case CMD_AFN_C_F148_BACK_NONE_DMND_C:
        case CMD_AFN_C_F149_FRTH_HAVE_DMND_L:
        case CMD_AFN_C_F150_FRTH_NONE_DMND_L:
        case CMD_AFN_C_F151_BACK_HAVE_DMND_L:
        case CMD_AFN_C_F152_BACK_NONE_DMND_L:
        case CMD_AFN_C_F153_FREZ_ZONE_1:
        case CMD_AFN_C_F154_FREZ_ZONE_2:
        case CMD_AFN_C_F155_FREZ_ZONE_3:
        case CMD_AFN_C_F156_FREZ_ZONE_4:
        case CMD_AFN_C_F157_FREZ_ZONE_5:
        case CMD_AFN_C_F158_FREZ_ZONE_6:
        case CMD_AFN_C_F159_FREZ_ZONE_7:
        case CMD_AFN_C_F160_FREZ_ZONE_8:
        case CMD_AFN_C_F161_METR_REMOTE_CTRL:
        case CMD_AFN_C_F165_METR_SWITCH_RECD:
        case CMD_AFN_C_F166_METR_MODIFY_RECD:
        case CMD_AFN_C_F167_METR_BUY_USE:
        case CMD_AFN_C_F168_METR_BALANCE:
        case CMD_AFN_C_F169_READ_ROUTE:
        case CMD_AFN_C_F170_READ_METER:

        ucF = (UINT8)(eCmd & 0xFF);
        break;

        default:
        #ifdef MT_DBG
        DEBUG("emtTrans_afn09f6_ast() para cfg para is out range %X", eCmd);
        #endif 
        return MT_ERR_OUTRNG;
        //break;
    }

    if(MT_TRANS_U2F == eTrans)
    {
        ucSift       = (ucF - 1) % 8;
        ucN          = ucF / 8 ;
        *pucTeam     = ucN + 1;
        pArray[ucN] |=  (0x01 << ucSift);
    }
    return MT_OK;
}


/*****************************************************************************
 函 数 名  : emtTrans_afn09f6
 功能描述  : F6：终端支持的1类数据配置
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : eMtErr
 调用函数  : emtTrans_afn09f5_ast
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月26日 星期一
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn09f6(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if(!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn09f6() pointer is null");
        #endif
        return MT_ERR_NULL;
    }
 
    sMtAfn09F6_f * psF    = (sMtAfn09F6_f*)psFrame;
    sMtAfn09F6*    psU    = (sMtAfn09F6*)psUser;
    eMtErr         eRet   = MT_OK;
    eMtCmd         eCmd   = CMD_AFN_F_UNKOWN;
    UINT8         *pFlag  = NULL;
    UINT8          ucN    = 0;
    UINT8          ucNTmp = 0;
    UINT8          ucFnN  = 0;
    UINT8          ucTmp  = 0;
    INT32          i      = 0;
    INT32          j      = 0;    
    
    if(MT_TRANS_U2F == eTrans)
    {        
        pFlag = (UINT8*)&(psF->ucFlag[0]);
        for(i = 0; i < psU->ucNum; i++)
        {
            eCmd = psU->eCfgCmd[i];
            eRet = emtTrans_afn09f6_ast(eTrans, eCmd, pFlag, &ucNTmp);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn09f6() emtTrans_afn09f6_ast() error %d", eRet);
                #endif 
                return eRet;
            }

            // 取个数最大值
            ucN = (ucN > ucNTmp) ? ucN : ucNTmp;
        }

        psF->ucTeam     = ucN;
        psF->ucBigClass = AFN_0C_ASK1;

    }
    else if(MT_TRANS_F2U == eTrans)
    {
        if(AFN_0C_ASK1 != psF->ucBigClass)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn09f6() ucBigClass() error %d", psF->ucBigClass);
            #endif 
            return MT_ERR_AFN;
        }
        
        ucN = psF->ucTeam;
        
        for(i = 0; i < ucN; i++)
        {
            ucTmp = psF->ucFlag[i];

            for(j = 0; j < 8; j++)
            {
                if(ucTmp & (0x01 << j))
                {
                   eCmd = (eMtCmd)((8 * i + j + 1) | 0x0C00);
                   eRet = emtTrans_afn09f6_ast(eTrans, eCmd, &ucNTmp, &ucNTmp);
                   if(MT_OK != eRet)
                   {
                       #ifdef MT_DBG
                       DEBUG("emtTrans_afn09f6() emtTrans_afn09f6_ast() error %d", eRet);
                       #endif 
                       return eRet;
                   }

                   psU->eCfgCmd[ucFnN++] = eCmd;
                }
            }
        }

       psU->ucNum =  ucFnN;
    }
    else
    {
        return MT_ERR_PARA;
    }

    *pusfLen = sizeof(UINT8) * (ucN + 2);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn09f7_ast
 功能描述  : F7：终端支持的2类数据配置 辅助函数
 输入参数  : eMtCmd eCmd      
             UINT8 *pArray    
             pucTeam 所属的信息组 
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : emtTrans_afn09f5
 
 修改历史      :
  1.日    期   : 2013年8月26日 星期一
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn09f7_ast(eMtTrans eTrans, eMtCmd eCmd, UINT8 *pArray, UINT8 *pucTeam)
{
    if(!pArray || !pucTeam)
    {
      return MT_ERR_NULL;
    }

    UINT8 ucN    = 0;
    UINT8 ucF    = 0;
    UINT8 ucSift = 0;
    
    switch(eCmd)
    {
        case CMD_AFN_D_F1_FRTH_POWR_P1P4_D:
        case CMD_AFN_D_F2_BACK_POWR_P2P3_D:
        case CMD_AFN_D_F3_FRTH_DMND_TIME_D:
        case CMD_AFN_D_F4_BACK_DMND_TIME_D:
        case CMD_AFN_D_F5_FRTH_HAVE_POWR_D:
        case CMD_AFN_D_F6_FRTH_NONE_POWR_D:
        case CMD_AFN_D_F7_BACK_HAVE_POWR_D:
        case CMD_AFN_D_F8_BACK_NONE_POWR_D:
        case CMD_AFN_D_F9_FRTH_POWR_P1P4_R:
        case CMD_AFN_D_F10_BACK_POWR_P2P3_R:  
        case CMD_AFN_D_F11_FRTH_DMND_TIME_R:   
        case CMD_AFN_D_F12_BACK_DMND_TIME_R:
        case CMD_AFN_D_F17_FRTH_POWR_P1P4_M:   
        case CMD_AFN_D_F18_BACK_POWR_P2P3_M:
        case CMD_AFN_D_F19_FRTH_DMND_TIME_M:
        case CMD_AFN_D_F20_BACK_DMND_TIME_M:
        case CMD_AFN_D_F21_FRTH_HAVE_POWR_M:
        case CMD_AFN_D_F22_FRTH_NONE_POWR_M:
        case CMD_AFN_D_F23_BACK_HAVE_POWR_M:
        case CMD_AFN_D_F24_BACK_NONE_POWR_M:              
        case CMD_AFN_D_F25_POWR_FRZE_D:  
        case CMD_AFN_D_F26_DMND_FRZE_D:  
        case CMD_AFN_D_F27_VOLT_FRZE_D:  
        case CMD_AFN_D_F28_UBLN_OVER_D: 
        case CMD_AFN_D_F29_ELEC_OVER_D:  
        case CMD_AFN_D_F30_POWR_RATE_D:  
        case CMD_AFN_D_F31_LOAD_RATE_D:  
        case CMD_AFN_D_F32_METR_DROP_D:  
        case CMD_AFN_D_F33_POWR_FRZE_M: 
        case CMD_AFN_D_F34_DMND_FRZE_M:  
        case CMD_AFN_D_F35_VOLT_FRZE_M:  
        case CMD_AFN_D_F36_UBLN_OVER_M:  
        case CMD_AFN_D_F37_ELEC_OVER_M: 
        case CMD_AFN_D_F38_POWR_RATE_M:  
        case CMD_AFN_D_F39_LOAD_RATE_M:  
        case CMD_AFN_D_F41_CAPA_TIME_D:  
        case CMD_AFN_D_F42_CAPA_NONE_D: 
        case CMD_AFN_D_F43_FACT_TIME_D: 
        case CMD_AFN_D_F44_FACT_TIME_M:  
        case CMD_AFN_D_F45_COPR_IRON_D:  
        case CMD_AFN_D_F46_COPR_IRON_M:  
        case CMD_AFN_D_F49_TML_ONOF_D:  
        case CMD_AFN_D_F50_TML_CTRL_D: 
        case CMD_AFN_D_F51_TML_ONOF_M:  
        case CMD_AFN_D_F52_TML_CTRL_M:  
        case CMD_AFN_D_F53_TML_FLOW_D:  
        case CMD_AFN_D_F54_TML_FLOW_M:  
        case CMD_AFN_D_F57_GRUP_RATE_D: 
        case CMD_AFN_D_F58_GRUP_HAVE_D:  
        case CMD_AFN_D_F59_GRUP_NONE_D:
        case CMD_AFN_D_F60_GRUP_RATE_M: 
        case CMD_AFN_D_F61_GRUP_HAVE_M:  
        case CMD_AFN_D_F62_GRUP_NONE_M:  
        case CMD_AFN_D_F65_GRUP_RATE_OVER: 
        case CMD_AFN_D_F66_GRUP_POWR_OVER: 
        case CMD_AFN_D_F73_CURVE_RATE_HAVE:  
        case CMD_AFN_D_F74_CURVE_RATE_NONE:  
        case CMD_AFN_D_F75_CURVE_POWR_HAVE:  
        case CMD_AFN_D_F76_CURVE_POWR_NONE:  
        case CMD_AFN_D_F81_CURVE_HAVE: 
        case CMD_AFN_D_F82_CURVE_HAVE_A:  
        case CMD_AFN_D_F83_CURVE_HAVE_B:  
        case CMD_AFN_D_F84_CURVE_HAVE_C:  
        case CMD_AFN_D_F85_CURVE_NONE: 
        case CMD_AFN_D_F86_CURVE_NONE_A:
        case CMD_AFN_D_F87_CURVE_NONE_B: 
        case CMD_AFN_D_F88_CURVE_NONE_C:
        case CMD_AFN_D_F89_CURVE_VOLT_A: 
        case CMD_AFN_D_F90_CURVE_VOLT_B:
        case CMD_AFN_D_F91_CURVE_VOLT_C:  
        case CMD_AFN_D_F92_CURVE_ELEC_A: 
        case CMD_AFN_D_F93_CURVE_ELEC_B:
        case CMD_AFN_D_F94_CURVE_ELEC_C:  
        case CMD_AFN_D_F95_CURVE_ZERO_E:  
        case CMD_AFN_D_F97_CURVE_FRTH_HAVE:  
        case CMD_AFN_D_F98_CURVE_FRTH_NONE:  
        case CMD_AFN_D_F98_CURVE_BACK_HAVE: 
        case CMD_AFN_D_F100_CURVE_BACK_HAVE: 
        case CMD_AFN_D_F101_CURVE_FRTH_HAVE: 
        case CMD_AFN_D_F102_CURVE_FRTH_NONE: 
        case CMD_AFN_D_F103_CURVE_BACK_HAVE:  
        case CMD_AFN_D_F104_CURVE_BACK_NONE:
        case CMD_AFN_D_F105_CURVE_FACTOR_T:
        case CMD_AFN_D_F106_CURVE_FACTOR_A:
        case CMD_AFN_D_F107_CURVE_FACTOR_B:
        case CMD_AFN_D_F108_CURVE_FACTOR_C:
        case CMD_AFN_D_F109_ANGLE_CURVE_VOLT: 
        case CMD_AFN_D_F110_ANGLE_CURVE_ELEC:
        case CMD_AFN_D_F113_ELEC_HARM_TIME_A:
        case CMD_AFN_D_F114_ELEC_HARM_TIME_B:
        case CMD_AFN_D_F115_ELEC_HARM_TIME_C: 
        case CMD_AFN_D_F116_VOLT_HARM_TIME_A:
        case CMD_AFN_D_F117_VOLT_HARM_TIME_B: 
        case CMD_AFN_D_F118_VOLT_HARM_TIME_C:
        case CMD_AFN_D_F121_HARM_OVER_A:
        case CMD_AFN_D_F122_HARM_OVER_B:  
        case CMD_AFN_D_F123_HARM_OVER_C:  
        case CMD_AFN_D_F129_DC_D: 
        case CMD_AFN_D_F130_DC_M:
        case CMD_AFN_D_F138_DC_CURVE:  
        case CMD_AFN_D_F145_CURVE_PHASE_1:  
        case CMD_AFN_D_F146_CURVE_PHASE_4: 
        case CMD_AFN_D_F147_CURVE_PHASE_2:  
        case CMD_AFN_D_F148_CURVE_PHASE_3:  
        case CMD_AFN_D_F153_FRTH_HAVE_D:  
        case CMD_AFN_D_F154_FRTH_NONE_D:  
        case CMD_AFN_D_F155_BACK_HAVE_D:  
        case CMD_AFN_D_F156_BACK_NONE_D:  
        case CMD_AFN_D_F157_FRTH_HAVE_M:  
        case CMD_AFN_D_F158_FRTH_NONE_M:  
        case CMD_AFN_D_F159_BACK_HAVE_M:     
        case CMD_AFN_D_F160_BACK_NONE_M:  
        case CMD_AFN_D_F161_HAVE_FRTH_D:  
        case CMD_AFN_D_F162_NONE_FRTM_D:  
        case CMD_AFN_D_F163_HAVE_BACK_D:  
        case CMD_AFN_D_F164_NONE_BACK_D:  
        case CMD_AFN_D_F165_NONE_D_PHASE_1: 
        case CMD_AFN_D_F166_NONE_D_PHASE_2:  
        case CMD_AFN_D_F167_NONE_D_PHASE_3:  
        case CMD_AFN_D_F168_NONE_D_PHASE_4: 
        case CMD_AFN_D_F169_HAVE_FRTH_R:  
        case CMD_AFN_D_F170_NONE_FRTM_R:  
        case CMD_AFN_D_F171_HAVE_BACK_R:  
        case CMD_AFN_D_F172_NONE_BACK_R: 
        case CMD_AFN_D_F173_NONE_R_PHASE_1:  
        case CMD_AFN_D_F174_NONE_R_PHASE_2:  
        case CMD_AFN_D_F175_NONE_R_PHASE_3:  
        case CMD_AFN_D_F176_NONE_R_PHASE_4: 
        case CMD_AFN_D_F177_HAVE_FRTH_M:  
        case CMD_AFN_D_F178_NONE_FRTM_M:  
        case CMD_AFN_D_F179_HAVE_BACK_M:  
        case CMD_AFN_D_F180_NONE_BACK_M:  
        case CMD_AFN_D_F181_NONE_M_PHASE_1: 
        case CMD_AFN_D_F182_NONE_M_PHASE_2:  
        case CMD_AFN_D_F183_NONE_M_PHASE_3:
        case CMD_AFN_D_F184_NONE_M_PHASE_4:
        case CMD_AFN_D_F185_HAVE_DMND_FRTH_D:
        case CMD_AFN_D_F186_NONE_DMND_FRTH_D:
        case CMD_AFN_D_F187_HAVE_DMND_BACK_D:
        case CMD_AFN_D_F188_NONE_DMND_BACK_D:
        case CMD_AFN_D_F189_HAVE_DMND_FRTH_R:
        case CMD_AFN_D_F190_NONE_DMND_FRTH_R:
        case CMD_AFN_D_F191_HAVE_DMND_BACK_R:
        case CMD_AFN_D_F192_NONE_DMND_BACK_R:
        case CMD_AFN_D_F193_HAVE_DMND_FRTH_M:
        case CMD_AFN_D_F194_NONE_DMND_FRTH_M:
        case CMD_AFN_D_F195_HAVE_DMND_BACK_M: 
        case CMD_AFN_D_F196_NONE_DMND_BACK_M:  
        case CMD_AFN_D_F201_FREZ_ZONE_1:  
        case CMD_AFN_D_F202_FREZ_ZONE_2:  
        case CMD_AFN_D_F203_FREZ_ZONE_3: 
        case CMD_AFN_D_F204_FREZ_ZONE_4:  
        case CMD_AFN_D_F205_FREZ_ZONE_5:  
        case CMD_AFN_D_F206_FREZ_ZONE_6: 
        case CMD_AFN_D_F207_FREZ_ZONE_7:
        case CMD_AFN_D_F208_FREZ_ZONE_8:
        case CMD_AFN_D_F209_METR_REMOTE_INFO: 
        case CMD_AFN_D_F213_METR_SWITCH_INFO:
        case CMD_AFN_D_F214_METR_MODIFY_INFO:
        case CMD_AFN_D_F215_METR_BUY_USR:
        case CMD_AFN_D_F216_METR_BALANCE:  
        case CMD_AFN_D_F217_WHITE_YAWP_CURVE:
        case CMD_AFN_D_F218_COLOR_YAWP_CURVE:

        ucF = (UINT8)(eCmd & 0xFF);
        break;

        default:
        #ifdef MT_DBG
        DEBUG("emtTrans_afn09f7_ast() para cfg para is out range %X", eCmd);
        #endif 
        return MT_ERR_OUTRNG;
        //break;
    }

    if(MT_TRANS_U2F == eTrans)
    {
        ucSift       = (ucF - 1) % 8;
        ucN          = ucF / 8 ;
        *pucTeam     = ucN + 1;
        pArray[ucN] |=  (0x01 << ucSift);
    }
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn09f7
 功能描述  : F7：终端支持的2类数据配置
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : eMtErr
 调用函数  : emtTrans_afn09f5_ast
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月26日 星期一
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn09f7(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if(!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn09f7() pointer is null");
        #endif
        return MT_ERR_NULL;
    }
 
    sMtAfn09F7_f * psF    = (sMtAfn09F7_f*)psFrame;
    sMtAfn09F7*    psU    = (sMtAfn09F7*)psUser;
    eMtErr         eRet   = MT_OK;
    eMtCmd         eCmd   = CMD_AFN_F_UNKOWN;
    UINT8         *pFlag  = NULL;
    UINT8          ucN    = 0;
    UINT8          ucNTmp = 0;
    UINT8          ucFnN  = 0;
    UINT8          ucTmp  = 0;
    INT32          i      = 0;
    INT32          j      = 0;    
    
    if(MT_TRANS_U2F == eTrans)
    {        
        pFlag = (UINT8*)&(psF->ucFlag[0]);
        for(i = 0; i < psU->ucNum; i++)
        {
            eCmd = psU->eCfgCmd[i];
            eRet = emtTrans_afn09f7_ast(eTrans, eCmd, pFlag, &ucNTmp);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn09f7() emtTrans_afn09f7_ast() error %d", eRet);
                #endif 
                return eRet;
            }

            // 取个数最大值
            ucN = (ucN > ucNTmp) ? ucN : ucNTmp;
        }

        psF->ucTeam     = ucN;
        psF->ucBigClass = AFN_0D_ASK2;

    }
    else if(MT_TRANS_F2U == eTrans)
    {
        if(AFN_0D_ASK2 != psF->ucBigClass)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn09f7() ucBigClass() error %d", psF->ucBigClass);
            #endif 
            return MT_ERR_AFN;
        }
        
        ucN = psF->ucTeam;
        
        for(i = 0; i < ucN; i++)
        {
            ucTmp = psF->ucFlag[i];

            for(j = 0; j < 8; j++)
            {
                if(ucTmp & (0x01 << j))
                {
                   eCmd = (eMtCmd)((8 * i + j) | 0x0D00);
                   eRet = emtTrans_afn09f7_ast(eTrans, eCmd, &ucNTmp, &ucNTmp);
                   if(MT_OK != eRet)
                   {
                       #ifdef MT_DBG
                       DEBUG("emtTrans_afn09f7() emtTrans_afn09f7_ast() error %d", eRet);
                       #endif 
                       return eRet;
                   }
				   
                   psU->eCfgCmd[ucFnN++] = eCmd;
                }
            }
        }

       psU->ucNum =  ucFnN;
    }
    else
    {
        return MT_ERR_PARA;
    }

    *pusfLen = sizeof(UINT8) * (ucN + 2);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn09f8
 功能描述  : F8：终端支持的事件记录配置
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月27日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn09f8(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if(!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn09f8() pointer is null");
        #endif
        return MT_ERR_NULL;
    }
 
    sMtAfn09F8_f * psF    = (sMtAfn09F8_f*)psFrame;
    sMtAfn09F8*    psU    = (sMtAfn09F8*)psUser;
    eMtErc         eErc   = MT_ERC_UNKOWN;
    UINT8          ucN    = 0;
    UINT8          ucTmp  = 0;
    INT32          i      = 0;
    INT32          j      = 0;    
    UINT8          ucSift = 0;
    
    if(MT_TRANS_U2F == eTrans)
    {
        // 初始化
        for(i = 0; i < 8; i++)
        {
            psF->ucFlag[i] = 0;
        }
        
        ucN = psU->ucNum;
        for(i = 0; i < ucN; i++)
        {
            eErc = psU->eErc[i];
            if(eErc > MT_ERC_35_UNKOWN || eErc < MT_ERC_UNKOWN)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn09f8() eErc out of range %d", eErc);
                #endif 
                return MT_ERR_OUTRNG;
            }
            
            ucTmp  = (UINT8)eErc;
            j      = ucTmp / 8;  
            ucSift = ucTmp % 8;
            psF->ucFlag[j] |= (0x01 << ucSift);
        }

    }
    else if(MT_TRANS_F2U == eTrans)
    {
        for(i = 0; i < 8; i++)
        {
            for(j = 0; j < 8; j++)
            {
                if(psF->ucFlag[i] & (0x01 << j))
                {
                    eErc = (eMtErc)(i * 8 + j + 1);
                    if(eErc > MT_ERC_35_UNKOWN || eErc < MT_ERC_UNKOWN)
                    {
                        #ifdef MT_DBG
                        DEBUG("emtTrans_afn09f8() eErc out of range %d", eErc);
                        #endif 
                        return MT_ERR_OUTRNG;
                    }
					
                    psU->eErc[ucN++] = eErc;
                    
                }
            }
        }

        psU->ucNum = ucN;
    }
    else
    {
        return MT_ERR_PARA;
    }

    *pusfLen = sizeof(UINT8) * (ucN + 2);
    return MT_OK;
}   

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf02
 功能描述  :  格式转换函数
              请求1类数据（AFN=0CH）
              F02：终端日历时钟
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月19日 星期一
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf02(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    eMtErr eRet = MT_OK;
    eRet = emt_trans_YYWWMMDDhhmmss(eTrans, (sMtUserClock *)psUser,(sMtFrmClock *)psFrame);

    *pusfLen = sizeof(sMtFrmClock);
    return eRet;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf03
 功能描述  :  格式转换函数
              请求1类数据（AFN=0CH）
              F03：终端参数状态
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月20日 星期二
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf03(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if (!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf03() pointer is null");
        #endif
        return MT_ERR_NULL;
    }
 
    sMtAfn0cF03     *psAfn0cF03   = (sMtAfn0cF03*)psUser;
    sMtAfn0cF03_f   *psAfn0cF03_f = (sMtAfn0cF03_f*)psFrame;
    INT32       i,j;

    if (MT_TRANS_F2U == eTrans) // 帧侧到用户侧
    {
        for ( i = 0; i < MT_TERMINAL_STATUS_FRM_MAX; i++)
        {
            for (j = 0; j < 8; j++)
            {
                psAfn0cF03->bParaStatus[i * 8 + j] = (BOOL)((psAfn0cF03_f->bParaStatus[i] >> j) & 0x01);
            }
        }
    }
    else if ( MT_TRANS_U2F == eTrans) // 用户侧到帧侧
    {
        memset(psAfn0cF03_f,0,sizeof(sMtAfn0cF03_f));
        for (i = 0; i < MT_TERMINAL_STATUS_USER_MAX; )
        {
            for ( j = 0; j < 8; j++,i++)
            {
                psAfn0cF03_f->bParaStatus[i / 8] |= (psAfn0cF03->bParaStatus[i] << j);
            }
        }
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf03() para error!");
        #endif
        return MT_ERR_PARA;
    }

    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0cF03_f);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf04
 功能描述  :  格式转换函数
              请求1类数据（AFN=0CH）
              F04：终端上行通信状态
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月20日 星期二
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf04(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if (!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf04() pointer is null");
        #endif
        return MT_ERR_NULL;
    }
    
    sMtAfn0cF04     *psAfn0cF04   = (sMtAfn0cF04*)psUser;
    sMtAfn0cF04_f   *psAfn0cF04_f = (sMtAfn0cF04_f*)psFrame;

    if (MT_TRANS_F2U == eTrans) // 帧侧到用户侧
    {
        psAfn0cF04->bCom = (psAfn0cF04_f->ucComStat >> 1) ^ (psAfn0cF04_f->ucComStat);
        psAfn0cF04->bUp  = (psAfn0cF04_f->ucUpStat >> 1) ^ (psAfn0cF04_f->ucUpStat);
    }else if ( MT_TRANS_U2F == eTrans) // 用户侧到帧侧
    {
        psAfn0cF04_f->ucComStat = ( psAfn0cF04->bCom == TRUE) ? MT_AFN0CF04_ENABLE : MT_AFN0CF04_DISABLE;
        psAfn0cF04_f->ucUpStat  = ( psAfn0cF04->bUp == TRUE) ? MT_AFN0CF04_ENABLE : MT_AFN0CF04_DISABLE;
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf04() para error!");
        #endif
        return MT_ERR_PARA;
    }

    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0cF04_f);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf05
 功能描述  :  格式转换函数
              请求1类数据（AFN=0CH）
              F05：终端通信状态
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月20日 星期二
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf05(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if (!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf05() pointer is null");
        #endif
        return MT_ERR_NULL;
    }
    sMtAfn0cF05     *psAfn0cF05   = (sMtAfn0cF05*)psUser;
    sMtAfn0cF05_f   *psAfn0cF05_f = (sMtAfn0cF05_f*)psFrame;
    UINT8   n = 0;
    INT32 i,j,k;

    if (MT_TRANS_F2U == eTrans) // 帧侧到用户侧
    {
        n = emtGetGroupNum(psAfn0cF05_f->ucGroupFlag);
        psAfn0cF05->bRemain = (psAfn0cF05_f->ucWarnSta & MT_AFN0CF05_REMAIN_ENABLE) ? TRUE : FALSE;
        psAfn0cF05->bDelete = (psAfn0cF05_f->ucWarnSta & MT_AFN0CF05_DELETE_ENABLE) ? TRUE : FALSE;
        psAfn0cF05->bPayWarn = (psAfn0cF05_f->ucWarnSta & MT_AFN0CF05_PAYWAR_ENABLE) ? TRUE : FALSE;
        psAfn0cF05->ucGroupNum = n;

        // 计算有效总加组
        for ( i = 0; i < MT_AFN0CF05_MAXGROUPS;i++)
        {
            psAfn0cF05->bGroup[i] = ((1 << i) & psAfn0cF05_f->ucGroupFlag) ? TRUE : FALSE;
        }

        for ( i = 0,j = 0; i < MT_AFN0CF05_MAXGROUPS;i++)
        {
            if( psAfn0cF05->bGroup[i])
            {
                psAfn0cF05->sGroup[i].ucScheme = psAfn0cF05_f->sGroup[j].ucPowerNum;

                // 计算功控时段有效标志
                for ( k = 0; k < 8;k++)
                {
                    psAfn0cF05->sGroup[i].bPowerCtrl[k]  = 
                        ((1 << k) & psAfn0cF05_f->sGroup[j].ucPowerFlag) ? TRUE : FALSE;
                }

                // 计算功控状态
                psAfn0cF05->sGroup[i].bTimeCtrl = \
                    (psAfn0cF05_f->sGroup[j].ucPowerSta & MT_AFN0CF05_TIME_CTRL_ENABLE) ? TRUE : FALSE;

                psAfn0cF05->sGroup[i].bRestCtrl = \
                    (psAfn0cF05_f->sGroup[j].ucPowerSta & MT_AFN0CF05_REST_CTRL_ENABLE) ? TRUE : FALSE;

                psAfn0cF05->sGroup[i].bStopCtrl = \
                    (psAfn0cF05_f->sGroup[j].ucPowerSta & MT_AFN0CF05_STOP_CTRL_ENABLE) ? TRUE : FALSE;

                psAfn0cF05->sGroup[i].bDownCtrl = \
                    (psAfn0cF05_f->sGroup[j].ucPowerSta & MT_AFN0CF05_DOWN_CTRL_ENABLE) ? TRUE : FALSE;

                // 计算电控状态
                psAfn0cF05->sGroup[i].bMonthCtrl = \
                    (psAfn0cF05_f->sGroup[j].ucElecSta & MT_AFN0CF05_MONTH_CTRL_ENABLE) ? TRUE : FALSE;

                psAfn0cF05->sGroup[i].bBuyCtrl = \
                    (psAfn0cF05_f->sGroup[j].ucElecSta & MT_AFN0CF05_BUY_CTRL_ENABLE) ? TRUE : FALSE;

                // 计算功控轮次状态
                for ( k = 0; k < 8;k++)
                {
                    psAfn0cF05->sGroup[i].bPowerTimeSta[k] = \
                        ( (1 << k) & psAfn0cF05_f->sGroup[j].ucPowerTimeSta) ? TRUE : FALSE;
                }

                // 计算电控轮次状态
                for ( k = 0; k < 8;k++)
                {
                    psAfn0cF05->sGroup[i].bElecTimeSta[k] = \
                        ( (1 << k) & psAfn0cF05_f->sGroup[j].ucElecTimeSta) ? TRUE : FALSE;
                }
                j++;
            }
        } 
    }else if ( MT_TRANS_U2F == eTrans) // 用户侧到帧侧
    {
        // 计算保电、剔除、催费告警投入状态
        if( psAfn0cF05->bRemain == TRUE)
        {
            psAfn0cF05_f->ucWarnSta |= MT_AFN0CF05_REMAIN_ENABLE;
        }

        if( psAfn0cF05->bDelete == TRUE)
        {
            psAfn0cF05_f->ucWarnSta |= MT_AFN0CF05_DELETE_ENABLE;
        }

        if( psAfn0cF05->bPayWarn == TRUE)
        {
            psAfn0cF05_f->ucWarnSta |= MT_AFN0CF05_PAYWAR_ENABLE;
        }

        // 计算总加组有效标志位及总加组状态
        for ( i = 0,j = 0; i < MT_AFN0CF05_MAXGROUPS;i++)
        {
            if (psAfn0cF05->bGroup[i] == TRUE)
            {
                psAfn0cF05_f->ucGroupFlag |= (1 << i);
                // 功控定值方案号
                psAfn0cF05_f->sGroup[j].ucPowerNum = psAfn0cF05->sGroup[i].ucScheme;
                
                // 功控时段有效标志位
                for ( k = 0; k < 8; k++ )
                {
                    if(psAfn0cF05->sGroup[i].bPowerCtrl[k] == TRUE)
                    {
                        psAfn0cF05_f->sGroup[j].ucPowerFlag |= (1 << k);
                    }
                }

                // 功控状态
                if ( psAfn0cF05->sGroup[i].bTimeCtrl == TRUE)
                {
                    psAfn0cF05_f->sGroup[j].ucPowerSta |= MT_AFN0CF05_TIME_CTRL_ENABLE;
                }

                if ( psAfn0cF05->sGroup[i].bRestCtrl == TRUE)
                {
                    psAfn0cF05_f->sGroup[j].ucPowerSta |= MT_AFN0CF05_REST_CTRL_ENABLE;
                }

                if ( psAfn0cF05->sGroup[i].bStopCtrl == TRUE)
                {
                    psAfn0cF05_f->sGroup[j].ucPowerSta |= MT_AFN0CF05_STOP_CTRL_ENABLE;
                }

                if ( psAfn0cF05->sGroup[i].bDownCtrl == TRUE)
                {
                    psAfn0cF05_f->sGroup[j].ucPowerSta |= MT_AFN0CF05_DOWN_CTRL_ENABLE;
                }

                 // 电控状态
                if ( psAfn0cF05->sGroup[i].bMonthCtrl == TRUE )
                {
                    psAfn0cF05_f->sGroup[j].ucElecSta |= MT_AFN0CF05_MONTH_CTRL_ENABLE;
                }

                if ( psAfn0cF05->sGroup[i].bBuyCtrl == TRUE )
                {
                    psAfn0cF05_f->sGroup[j].ucElecSta |= MT_AFN0CF05_BUY_CTRL_ENABLE;
                }

                // 功控轮次状态
                for ( k=0; k < 8; k++ )
                {
                    if (psAfn0cF05->sGroup[i].bPowerTimeSta[k] == TRUE )
                    {
                        psAfn0cF05_f->sGroup[j].ucPowerTimeSta |= (1 << k);
                    }
                }

                // 电控轮次状态
                for ( k=0; k < 8; k++ )
                {
                    if (psAfn0cF05->sGroup[i].bElecTimeSta[k] == TRUE )
                    {
                        psAfn0cF05_f->sGroup[j].ucElecTimeSta |= (1 << k);
                    }
                }
                j++;
            }

        }
        n = j;
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf05() para error!");
        #endif
        return MT_ERR_PARA;
    }

    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0cF05_f) + n * sizeof(sMtComGroupSta_f);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf06
 功能描述  :  格式转换函数
              请求1类数据（AFN=0CH）
              F06：终端控制状态
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月20日 星期二
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf06(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if (!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf06() pointer is null");
        #endif
        return MT_ERR_NULL;
    }
    
    sMtAfn0cF06     *psAfn0cF06   = (sMtAfn0cF06*)psUser;
    sMtAfn0cF06_f   *psAfn0cF06_f = (sMtAfn0cF06_f*)psFrame;
    UINT8     n = 0;
    INT32     i,j,k;
 
    if (MT_TRANS_F2U == eTrans) // 帧侧到用户侧
    {
        // 计算遥控跳闸输出状态
        n = emtGetGroupNum(psAfn0cF06_f->ucGroupFlag);
        for ( i = 0; i < 8; i++)
        {
            psAfn0cF06->bTrip[i] = ((1 << i) & psAfn0cF06_f->ucTripOutSta) ? TRUE : FALSE;
        }

        psAfn0cF06->ucPayWarn = psAfn0cF06_f->ucPayWarnSta;
        j = 0;
        for ( i = 0; i < 8; i++)
        {
            psAfn0cF06->bGroup[i] = (( 1 << i) & psAfn0cF06_f->ucGroupFlag) ? TRUE : FALSE;

            // 总加组有效
            if ( psAfn0cF06->bGroup[i] )
            {
                // 计算当前功率定值
                emt_trans_sXXX(eTrans,&(psAfn0cF06->sGroup[i].fCurPower)
                                ,&(psAfn0cF06_f->sGroup[j].sCurPower));

                // 计算当前功率下浮控浮浮动系数
                emt_trans_sXX(eTrans, &(psAfn0cF06->sGroup[i].sPowerDrift)
                    ,&(psAfn0cF06_f->sGroup[j].sPowerDrift));

                // 功控跳闸输出状态
                for ( k = 0; k < 8; k++)
                {
                     psAfn0cF06->sGroup[i].bPowerOutSta[k] = 
                        ((1 << k) & psAfn0cF06_f->sGroup[j].ucPowerOutSta) ? TRUE : FALSE;
                }

                // 月电控跳闸输出状态
                for ( k = 0; k < 8; k++)
                {
                     psAfn0cF06->sGroup[i].bMonthElecOutSta[k] = 
                        ((1 << k) & psAfn0cF06_f->sGroup[j].ucMonthElecOutSta) ? TRUE : FALSE;
                }

                // 购电控跳闸输出状态
                for ( k = 0; k < 8; k++)
                {
                     psAfn0cF06->sGroup[i].bBuyElecOutSta[k] = 
                        ((1 << k) & psAfn0cF06_f->sGroup[j].ucBuyElecOutSta) ? TRUE : FALSE;
                }

                // 获取时段控状态
                psAfn0cF06->sGroup[i].bTimeCtrlSta = \
                    (psAfn0cF06_f->sGroup[j].ucPowerWarnSta & MT_AFN0CF06_TIME_CTRL_STAT) ? TRUE : FALSE;

                // 获取厂休控状态
                psAfn0cF06->sGroup[i].bRestCtrlSta = \
                    (psAfn0cF06_f->sGroup[j].ucPowerWarnSta & MT_AFN0CF06_REST_CTRL_STAT) ? TRUE : FALSE;

                // 获取营业报停控状态
                psAfn0cF06->sGroup[i].bStopCtrlSta = \
                    (psAfn0cF06_f->sGroup[j].ucPowerWarnSta & MT_AFN0CF06_STOP_CTRL_STAT) ? TRUE : FALSE;

                // 获取当前功率下浮控状态
                psAfn0cF06->sGroup[i].bDownCtrlSta = \
                    (psAfn0cF06_f->sGroup[j].ucPowerWarnSta & MT_AFN0CF06_DOWN_CTRL_STAT) ? TRUE : FALSE;

                // 获取月电控状态
                psAfn0cF06->sGroup[i].bMonthCtrlSta = \
                    (psAfn0cF06_f->sGroup[j].ucElecWarnSta & MT_AFN0CF06_MONTH_CTRL_STAT) ? TRUE : FALSE;

                 // 获取购电控状态
                psAfn0cF06->sGroup[i].bBuyCtrlSta = \
                    (psAfn0cF06_f->sGroup[j].ucElecWarnSta & MT_AFN0CF06_BUY_CTRL_STAT) ? TRUE : FALSE;

                j++;
            }
        }
         
    }else if ( MT_TRANS_U2F == eTrans) // 用户侧到帧侧
    {
        // 设置遥控跳闸输出状态
        for(i = 0; i < 8; i++)
        {
            if ( psAfn0cF06->bTrip[i] )
            {
                psAfn0cF06_f->ucTripOutSta |= (1 << i);
            }
        }

        // 设置当前催费告警状态
        psAfn0cF06_f->ucPayWarnSta = psAfn0cF06->ucPayWarn;
        j = 0;
        for ( i = 0; i < 8 ;i++ )
        {
            if ( psAfn0cF06->bGroup[i] )
            {
                // 总加组状态
                psAfn0cF06_f->ucGroupFlag |= (1 << i);
                 // 设置当前功率定值
                emt_trans_sXXX(eTrans,&(psAfn0cF06->sGroup[i].fCurPower)
                                ,&(psAfn0cF06_f->sGroup[j].sCurPower));

                // 设置当前功率下浮控浮浮动系数
                emt_trans_sXX(eTrans, &(psAfn0cF06->sGroup[i].sPowerDrift)
                    ,&(psAfn0cF06_f->sGroup[j].sPowerDrift));

                // 设置功控跳闸输出状态
                for ( k = 0; k < 8 ; k++)
                {
                    if ( psAfn0cF06->sGroup[i].bPowerOutSta[k] )
                    {
                        psAfn0cF06_f->sGroup[j].ucPowerOutSta |= (1 << k);
                    }
                }

                // 设置月电控跳闸输出状态
                for ( k = 0; k < 8 ; k++)
                {
                    if ( psAfn0cF06->sGroup[i].bMonthElecOutSta[k] )
                    {
                        psAfn0cF06_f->sGroup[j].ucMonthElecOutSta |= (1 << k);
                    }
                }

                // 设置购电控跳闸输出状态
                for ( k = 0; k < 8 ; k++)
                {
                    if ( psAfn0cF06->sGroup[i].bBuyElecOutSta[k] )
                    {
                        psAfn0cF06_f->sGroup[j].ucBuyElecOutSta |= (1 << k);
                    }
                }

                // 时段控状态
                if ( psAfn0cF06->sGroup[i].bTimeCtrlSta )
                {
                    psAfn0cF06_f->sGroup[j].ucPowerWarnSta |= MT_AFN0CF06_TIME_CTRL_STAT;
                }

                // 厂休控状态
                if ( psAfn0cF06->sGroup[i].bRestCtrlSta )
                {
                    psAfn0cF06_f->sGroup[j].ucPowerWarnSta |= MT_AFN0CF06_REST_CTRL_STAT;
                }

                // 营业报停状态
                if ( psAfn0cF06->sGroup[i].bStopCtrlSta )
                {
                    psAfn0cF06_f->sGroup[j].ucPowerWarnSta |= MT_AFN0CF06_STOP_CTRL_STAT;
                }

                // 当前功率下浮控状态
                if ( psAfn0cF06->sGroup[i].bDownCtrlSta )
                {
                    psAfn0cF06_f->sGroup[j].ucPowerWarnSta |= MT_AFN0CF06_DOWN_CTRL_STAT;
                }

                if ( psAfn0cF06->sGroup[i].bMonthCtrlSta )
                {
                    psAfn0cF06_f->sGroup[j].ucElecWarnSta |= MT_AFN0CF06_MONTH_CTRL_STAT;
                }

                if ( psAfn0cF06->sGroup[i].bBuyCtrlSta )
                {
                    psAfn0cF06_f->sGroup[j].ucElecWarnSta |= MT_AFN0CF06_BUY_CTRL_STAT;
                }
                j++;
            }
        }
        n = j;
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf06() para error!");
        #endif
        return MT_ERR_PARA;
    }

    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0cF06_f) + n * sizeof(sMtCtrlGroupSta_f);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf07
 功能描述  :  格式转换函数
              请求1类数据（AFN=0CH）
              F07：终端事件计数器当前值
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月20日 星期二
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf07(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if (!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf07() pointer is null");
        #endif
        return MT_ERR_NULL;
    }
    
    sMtAfn0cF07     *psAfn0cF07   = (sMtAfn0cF07*)psUser;
    sMtAfn0cF07_f   *psAfn0cF07_f = (sMtAfn0cF07_f*)psFrame;

    if (MT_TRANS_F2U == eTrans) // 帧侧到用户侧
    {
        // 用户侧和帧侧数据结构一样
        psAfn0cF07->ucEc1 = psAfn0cF07_f->ucEc1;
        psAfn0cF07->ucEc2 = psAfn0cF07_f->ucEc2;
    }else if ( MT_TRANS_U2F == eTrans) // 用户侧到帧侧
    {
        // 用户侧和帧侧数据结构一样
        psAfn0cF07_f->ucEc1 = psAfn0cF07->ucEc1;
        psAfn0cF07_f->ucEc2 = psAfn0cF07->ucEc2;
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf07() para error!");
        #endif
        return MT_ERR_PARA;
    }

    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0cF07_f);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf08
 功能描述  :  格式转换函数
              请求1类数据（AFN=0CH）
              F08：终端事件标志状态
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月20日 星期二
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf08(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if (!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf08() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    sMtAfn0cF08     *psAfn0cF08   = (sMtAfn0cF08*)psUser;
    sMtAfn0cF08_f   *psAfn0cF08_f = (sMtAfn0cF08_f*)psFrame;
    INT32   i;

    if (MT_TRANS_F2U == eTrans) // 帧侧到用户侧
    {
        for( i = 0; i < MT_AFN0CF08_ERC_MAX;i++)
        {
            // ERC 1 ~ 32
            if( i < 32)
            {
                psAfn0cF08->bErc[i] = (psAfn0cF08_f->ucErcLow & (1 << i)) ? TRUE : FALSE;
            }
            else if( i >= 32 && i < 64)
            {
                psAfn0cF08->bErc[i] = ((psAfn0cF08_f->ucErcHigh) & (1 << (i - 32))) ? TRUE : FALSE;
            }
            else
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0cf08() out of max Erc");
                #endif
                return MT_ERR_OUTRNG;
            }
        }
    }else if ( MT_TRANS_U2F == eTrans) // 用户侧到帧侧
    {
        for (i = 0; i < sizeof(UINT32) * 8;i++)
        {
            if( psAfn0cF08->bErc[i] == TRUE)
            {
                psAfn0cF08_f->ucErcLow |= (1 << i);
            }
        }

        for (i = sizeof(UINT32); i < 2 * sizeof(UINT32) * 8;i++)
        {
            if( psAfn0cF08->bErc[i] == TRUE)
            {
                psAfn0cF08_f->ucErcHigh |= (1 << i);
            }
        }
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf08() para error!");
        #endif
        return MT_ERR_PARA;
    }

    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0cF08_f);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf09
 功能描述  :  格式转换函数
              请求1类数据（AFN=0CH）
              F09：终端状态量及变位标志
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月21日 星期三
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf09(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if (!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf09() pointer is null");
        #endif
        return MT_ERR_NULL;
    }
 
    if (MT_TRANS_F2U == eTrans) // 帧侧到用户侧
    {
        // 暂时不做处理，只计算数据长度 
    }else if ( MT_TRANS_U2F == eTrans) // 用户侧到帧侧
    {
        // 暂时不做处理，只计算数据长度
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf09() para error!");
        #endif
        return MT_ERR_PARA;
    }

    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0cF09_f);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf10
 功能描述  :  格式转换函数
              请求1类数据（AFN=0CH）
              F10：终端与主站当日、月通信流量
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月21日 星期三
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf10(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if (!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf10() pointer is null");
        #endif
        return MT_ERR_NULL;
    }
 
    if (MT_TRANS_F2U == eTrans) // 帧侧到用户侧
    {
        // 暂时不做处理，只计算数据长度 
    }else if ( MT_TRANS_U2F == eTrans) // 用户侧到帧侧
    {
    // 暂时不做处理，只计算数据长度
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf10() para error!");
        #endif
        return MT_ERR_PARA;
    }

    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0cF10_f);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf11
 功能描述  :  格式转换函数
              请求1类数据（AFN=0CH）
              F11： 终端集中抄表状态信息
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月21日 星期三
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf11(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if (!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf11() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    sMtAfn0cF11_f *psMtAfn0cF11_f = (sMtAfn0cF11_f *)psFrame;
    if (MT_TRANS_F2U == eTrans) // 帧侧到用户侧
    {
        // 暂时不做处理，只计算数据长度
        if (psMtAfn0cF11_f->ucBlockNum < 1 || psMtAfn0cF11_f->ucBlockNum > 31)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0cf11() ucBlockNum out of range!");
            #endif
            return MT_ERR_OUTRNG; 
        }
        // 未检查终端通信端口号(1 ~ 31)
    }
    else if ( MT_TRANS_U2F == eTrans) // 用户侧到帧侧
    {
        // 暂时不做处理，只计算数据长度
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf11() para error!");
        #endif
        return MT_ERR_PARA;
    }

    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0cF11_f) + psMtAfn0cF11_f->ucBlockNum * sizeof(sMtDataBlock);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf17
 功能描述  :  格式转换函数
              请求1类数据（AFN=0CH）
              F17：当前总加有功功率 
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月21日 星期三
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf17(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if (!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf17() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    if (MT_TRANS_F2U == eTrans) // 帧侧到用户侧
    {
        // 暂时不做处理，只计算数据长度
    }else if ( MT_TRANS_U2F == eTrans) // 用户侧到帧侧
    {
        // 暂时不做处理，只计算数据长度
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf17() para error!");
        #endif
        return MT_ERR_PARA;
    }

    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0cF17_f);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf18
 功能描述  :  格式转换函数
              请求1类数据（AFN=0CH）
              F18：当前总加无功功率 
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月21日 星期三
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf18(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if (!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf18() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    if (MT_TRANS_F2U == eTrans) // 帧侧到用户侧
    {
        // 暂时不做处理，只计算数据长度
    }else if ( MT_TRANS_U2F == eTrans) // 用户侧到帧侧
    {
        // 暂时不做处理，只计算数据长度
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf18() para error!");
        #endif
        return MT_ERR_PARA;
    }

    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0cF18_f);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf19
 功能描述  :  格式转换函数
              请求1类数据（AFN=0CH）
              F19：当日总加有功电能量(总、费率 1~M)
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月21日 星期三
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf19(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if (!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf19() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    sMtAfn0cF19_f *psMtAfn0cF19_f = (sMtAfn0cF19_f *)psFrame;
 
    if (MT_TRANS_F2U == eTrans) // 帧侧到用户侧
    {
        // 暂时不做处理，只计算数据长度
    }else if ( MT_TRANS_U2F == eTrans) // 用户侧到帧侧
    {
        // 暂时不做处理，只计算数据长度
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf19() para error!");
        #endif
        return MT_ERR_PARA;
    }

    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0cF19_f) + psMtAfn0cF19_f->ucRateNum * sizeof(sMtFmt_sX7_f);
    return MT_OK;
}


/*****************************************************************************
 函 数 名  : emtTrans_afn0cf20
 功能描述  :  格式转换函数
              请求1类数据（AFN=0CH）
              F20：当日总加无功电能量(总、费率 1~M)
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月21日 星期三
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf20(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if (!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf20() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    sMtAfn0cF20_f *psMtAfn0cF20_f = (sMtAfn0cF20_f *)psFrame;
 
    if (MT_TRANS_F2U == eTrans) // 帧侧到用户侧
    {
        // 暂时不做处理，只计算数据长度
    }else if ( MT_TRANS_U2F == eTrans) // 用户侧到帧侧
    {
        // 暂时不做处理，只计算数据长度
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf20() para error!");
        #endif
        return MT_ERR_PARA;
    }

    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0cF20_f) + psMtAfn0cF20_f->ucRateNum * sizeof(sMtFmt_sX7_f);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf21
 功能描述  :  格式转换函数
              请求1类数据（AFN=0CH）
              F21：当月总加有功电能量(总、费率 1~M)
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月21日 星期三
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf21(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    return emtTrans_afn0cf19(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf22
 功能描述  :  格式转换函数
              请求1类数据（AFN=0CH）
              F22：当月总加无功电能量(总、费率 1~M)
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月21日 星期三
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf22(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    return emtTrans_afn0cf20(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf23
 功能描述  :  格式转换函数
              请求1类数据（AFN=0CH）
              F23：终端当前剩余电量(费)
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月22日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf23(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if (!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf23() pointer is null");
        #endif
        return MT_ERR_NULL;
    }
 
    if (MT_TRANS_F2U == eTrans) // 帧侧到用户侧
    {
        // 暂时不做处理，只计算数据长度
    }else if ( MT_TRANS_U2F == eTrans) // 用户侧到帧侧
    {
        // 暂时不做处理，只计算数据长度
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf23() para error!");
        #endif
        return MT_ERR_PARA;
    }

    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0cF23_f);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf24
 功能描述  :  格式转换函数
              请求1类数据（AFN=0CH）
              F24：当前功率下浮控控后总加有功功率冻结值
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月22日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf24(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if (!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf24() pointer is null");
        #endif
        return MT_ERR_NULL;
    }
 
    if (MT_TRANS_F2U == eTrans) // 帧侧到用户侧
    {
        // 暂时不做处理，只计算数据长度
    }else if ( MT_TRANS_U2F == eTrans) // 用户侧到帧侧
    {
        // 暂时不做处理，只计算数据长度
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf24() para error!");
        #endif
        return MT_ERR_PARA;
    }

    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0cF24_f);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf25
 功能描述  :  格式转换函数
              请求1类数据（AFN=0CH）
              F25：当前三相及总有/无功功率、功率因数，三相电压、电流、零序电流、视在功率
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月12日 星期一
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf25(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if(!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf25() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    sMtAfn0cF25     *psAfn0cF25   = (sMtAfn0cF25*)psUser;
    sMtAfn0cF25_f   *psAfn0cF25_f = (sMtAfn0cF25_f*)psFrame;
    BOOL             bNone        = FALSE;
    UINT16           usLen        = 0;
    eMtErr           eRet         = MT_OK;

    // 帧侧转为用户侧
    if(MT_TRANS_F2U == eTrans)
    {
        // sReadTime
        usLen = sizeof(sMtYYMMDDhhmm_f);
        bNone = bmt_is_none((UINT8*)&(psAfn0cF25_f->sReadTime), usLen);
        if(TRUE == bNone)
        {
            psAfn0cF25->bsReadTime = FALSE;
        }
        else
        {
            psAfn0cF25->bsReadTime = TRUE;
            eRet = emt_trans_YYMMDDhhmm(eTrans, &(psAfn0cF25->sReadTime),
            &(psAfn0cF25_f->sReadTime));
            
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0cf25() trans error %d ", eRet);
                #endif
                return eRet;
            }
        }
    
        // fp
        usLen = sizeof(sMtFmt_sXX_XXXX);
        bNone = bmt_is_none((UINT8*)&(psAfn0cF25_f->fP), usLen);
        if(TRUE == bNone)
        {
            psAfn0cF25->bfP = FALSE;
        }
        else
        {
            psAfn0cF25->bfP = TRUE;
            eRet = emt_trans_sXX_XXXX(eTrans, &(psAfn0cF25->fP),
            &(psAfn0cF25_f->fP));
    
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0cf25() trans error %d ", eRet);
                #endif
                return eRet;
            }
        }
        
        // fpa
        bNone = bmt_is_none((UINT8*)&(psAfn0cF25_f->fPa), usLen);
        if(TRUE == bNone)
        {
            psAfn0cF25->bfPa = FALSE;
        }
        else
        {
            psAfn0cF25->bfPa = TRUE;
            eRet = emt_trans_sXX_XXXX(eTrans, &(psAfn0cF25->fPa), 
            &(psAfn0cF25_f->fPa));
    
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0cf25() trans error %d ", eRet);
                #endif
                return eRet;
            }
        }
        
        // fpb
        bNone = bmt_is_none((UINT8*)&(psAfn0cF25_f->fPb), usLen);
        if(TRUE == bNone)
        {
            psAfn0cF25->bfPb = FALSE;
        }
        else
        {
            psAfn0cF25->bfPb = TRUE;
            eRet = emt_trans_sXX_XXXX(eTrans, &(psAfn0cF25->fPb), 
            &(psAfn0cF25_f->fPb));
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0cf25() trans error %d ", eRet);
                #endif
                return eRet;
            }
        }
        
        // fpc
        bNone = bmt_is_none((UINT8*)&(psAfn0cF25_f->fPc), usLen);
        if(TRUE == bNone)
        {
            psAfn0cF25->bfPc = FALSE;
        }
        else
        {
            psAfn0cF25->bfPc = TRUE;
            eRet = emt_trans_sXX_XXXX(eTrans, &(psAfn0cF25->fPc), 
            &(psAfn0cF25_f->fPc));
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0cf25() trans error %d ", eRet);
                #endif
                return eRet;
            }
        }
        
        // fQ
        bNone = bmt_is_none((UINT8*)&(psAfn0cF25_f->fQ), usLen);
        if(TRUE == bNone)
        {
            psAfn0cF25->bfQ = FALSE;
        }
        else
        {
            psAfn0cF25->bfQ = TRUE;
            eRet = emt_trans_sXX_XXXX(eTrans, &(psAfn0cF25->fQ), 
            &(psAfn0cF25_f->fQ));
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0cf25() trans error %d ", eRet);
                #endif
                return eRet;
            }
        }   
        
        // fQa
        bNone = bmt_is_none((UINT8*)&(psAfn0cF25_f->fQa), usLen);
        if(TRUE == bNone)
        {
            psAfn0cF25->bfQa = FALSE;
        }
        else
        {
            psAfn0cF25->bfQa = TRUE;
            eRet = emt_trans_sXX_XXXX(eTrans, &(psAfn0cF25->fQa),
            &(psAfn0cF25_f->fQa));
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0cf25() trans error %d ", eRet);
                #endif
                return eRet;
            }
        }  
        
        // fQb
        bNone = bmt_is_none((UINT8*)&(psAfn0cF25_f->fQb),usLen);
        if(TRUE == bNone)
        {
            psAfn0cF25->bfQb = FALSE;
        }
        else
        {
            psAfn0cF25->bfQb = TRUE;
            eRet = emt_trans_sXX_XXXX(eTrans, &(psAfn0cF25->fQb), 
            &(psAfn0cF25_f->fQb));
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0cf25() trans error %d ", eRet);
                #endif
                return eRet;
            }
        }
        
        // fQc
        bNone = bmt_is_none((UINT8*)&(psAfn0cF25_f->fQc), usLen);
        if(TRUE == bNone)
        {
            psAfn0cF25->bfQc = FALSE;
        }
        else
        {
            psAfn0cF25->bfQc = TRUE;
            eRet = emt_trans_sXX_XXXX(eTrans, &(psAfn0cF25->fQc), 
            &(psAfn0cF25_f->fQc));
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0cf25() trans error %d ", eRet);
                #endif
                return eRet;
            }
        }
        
        // Pf
        usLen = sizeof(sMtFmt_sXXX_X);
        bNone = bmt_is_none((UINT8*)&(psAfn0cF25_f->fPf), usLen);
        if(TRUE == bNone)
        {
            psAfn0cF25->bfPf = FALSE;
        }
        else
        {
            psAfn0cF25->bfPf = TRUE;
            eRet = emt_trans_sXXX_X(eTrans, &(psAfn0cF25->fPf), 
            &(psAfn0cF25_f->fPf));
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0cf25() trans error %d ", eRet);
                #endif
                return eRet;
            }
        }   
        
        // Pfa
        bNone = bmt_is_none((UINT8*)&(psAfn0cF25_f->fPfa), usLen);
        if(TRUE == bNone)
        {
            psAfn0cF25->bfPfa = FALSE;
        }
        else
        {
            psAfn0cF25->bfPfa = TRUE;
            eRet = emt_trans_sXXX_X(eTrans, &(psAfn0cF25->fPfa),
            &(psAfn0cF25_f->fPfa));
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0cf25() trans error %d ", eRet);
                #endif
                return eRet;
            }
        }   
        
        // Pfb
        bNone = bmt_is_none((UINT8*)&(psAfn0cF25_f->fPfb), usLen);
        if(TRUE == bNone)
        {
            psAfn0cF25->bfPfb = FALSE;
        }
        else
        {
            psAfn0cF25->bfPfb = TRUE;
            eRet = emt_trans_sXXX_X(eTrans, &(psAfn0cF25->fPfb), 
            &(psAfn0cF25_f->fPfb));
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0cf25() trans error %d ", eRet);
                #endif
                return eRet;
            }
        }
        
        // Pfc
        bNone = bmt_is_none((UINT8*)&(psAfn0cF25_f->fPfc), usLen);
        if(TRUE == bNone)
        {
            psAfn0cF25->bfPfc = FALSE;
        }
        else
        {
            psAfn0cF25->bfPfc = TRUE;
            eRet = emt_trans_sXXX_X(eTrans, &(psAfn0cF25->fPfc), 
            &(psAfn0cF25_f->fPfc));
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0cf25() trans error %d ", eRet);
                #endif
                return eRet;
            }
        }
        
        // fUa
        usLen = sizeof(sMtFmt_XXX_X);
        bNone = bmt_is_none((UINT8*)&(psAfn0cF25_f->fUa), usLen);
        if(TRUE == bNone)
        {
            psAfn0cF25->bfUa = FALSE;
        }
        else
        {
            psAfn0cF25->bfUa = TRUE;
            eRet = emt_trans_XXX_X(eTrans, &(psAfn0cF25->fUa), 
            &(psAfn0cF25_f->fUa));
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0cf25() trans error %d ", eRet);
                #endif
                return eRet;
            }
        }     
        
        // fUb
        bNone = bmt_is_none((UINT8*)&(psAfn0cF25_f->fUb), usLen);
        if(TRUE == bNone)
        {
            psAfn0cF25->bfUb = FALSE;
        }
        else
        {
            psAfn0cF25->bfUb = TRUE;
            eRet = emt_trans_XXX_X(eTrans, &(psAfn0cF25->fUb),
            &(psAfn0cF25_f->fUb));
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0cf25() trans error %d ", eRet);
                #endif
                return eRet;
            }
        }   
        
        // fUc
        bNone = bmt_is_none((UINT8*)&(psAfn0cF25_f->fUc), usLen);
        if(TRUE == bNone)
        {
            psAfn0cF25->bfUc = FALSE;
        }
        else
        {
            psAfn0cF25->bfUc = TRUE;
            eRet = emt_trans_XXX_X(eTrans, &(psAfn0cF25->fUc), 
            &(psAfn0cF25_f->fUc));
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0cf25() trans error %d ", eRet);
                #endif
                return eRet;
            }
        }
        
        // fIa
        usLen = sizeof(sMtFmt_sXXX_XXX);
        bNone = bmt_is_none((UINT8*)&(psAfn0cF25_f->fIa), usLen);
        if(TRUE == bNone)
        {
            psAfn0cF25->bfIa = FALSE;
        }
        else
        {
            psAfn0cF25->bfIa = TRUE;
            eRet = emt_trans_sXXX_XXX(eTrans, &(psAfn0cF25->fIa), 
            &(psAfn0cF25_f->fIa));
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0cf25() trans error %d ", eRet);
                #endif
                return eRet;
            }
        }   
        
        // fIb
        bNone = bmt_is_none((UINT8*)&(psAfn0cF25_f->fIb), usLen);
        if(TRUE == bNone)
        {
            psAfn0cF25->bfIb = FALSE;
        }
        else
        {
            psAfn0cF25->bfIb = TRUE;
            eRet = emt_trans_sXXX_XXX(eTrans, &(psAfn0cF25->fIb),
            &(psAfn0cF25_f->fIb));
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0cf25() trans error %d ", eRet);
                #endif
                return eRet;
            }
        }  
        
        // fIc
        bNone = bmt_is_none((UINT8*)&(psAfn0cF25_f->fIc), usLen);
        if(TRUE == bNone)
        {
            psAfn0cF25->bfIc = FALSE;
        }
        else
        {
            psAfn0cF25->bfIc = TRUE;
            eRet = emt_trans_sXXX_XXX(eTrans, &(psAfn0cF25->fIc), 
            &(psAfn0cF25_f->fIc));
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0cf25() trans error %d ", eRet);
                #endif
                return eRet;
            }
        }
        
        // fI0
        bNone = bmt_is_none((UINT8*)&(psAfn0cF25_f->fI0), usLen);
        if(TRUE == bNone)
        {
            psAfn0cF25->bfI0 = FALSE;
        }
        else
        {
            psAfn0cF25->bfI0 = TRUE;
            eRet = emt_trans_sXXX_XXX(eTrans, &(psAfn0cF25->fI0), 
            &(psAfn0cF25_f->fI0));
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0cf25() trans error %d ", eRet);
                #endif
                return eRet;
            }
        }
        
        // fS
        usLen = sizeof(sMtFmt_sXX_XXXX);
        bNone = bmt_is_none((UINT8*)&(psAfn0cF25_f->fS), usLen);
        if(TRUE == bNone)
        {
            psAfn0cF25->bfS = FALSE;
        }
        else
        {
            psAfn0cF25->bfS = TRUE;
            eRet = emt_trans_sXX_XXXX(eTrans, &(psAfn0cF25->fS),
            &(psAfn0cF25_f->fS));
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0cf25() trans error %d ", eRet);
                #endif
                return eRet;
            }
        }   
            
        // fSa
        bNone = bmt_is_none((UINT8*)&(psAfn0cF25_f->fSa), usLen);
        if(TRUE == bNone)
        {
            psAfn0cF25->bfSa = FALSE;
        }
        else
        {
            psAfn0cF25->bfSa = TRUE;
            eRet = emt_trans_sXX_XXXX(eTrans, &(psAfn0cF25->fSa), 
            &(psAfn0cF25_f->fSa));
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0cf25() trans error %d ", eRet);
                #endif
                return eRet;
            }
        }        
           
        // fSb
        bNone = bmt_is_none((UINT8*)&(psAfn0cF25_f->fSb), usLen);
        if(TRUE == bNone)
        {
            psAfn0cF25->bfSb = FALSE;
        }
        else
        {
            psAfn0cF25->bfSb = TRUE;
            eRet = emt_trans_sXX_XXXX(eTrans, &(psAfn0cF25->fSb), 
            &(psAfn0cF25_f->fSb));
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0cf25() trans error %d ", eRet);
                #endif
                return eRet;
            }
        }
           
        // fpc
        bNone = bmt_is_none((UINT8*)&(psAfn0cF25_f->fSc), usLen);
        if(TRUE == bNone)
        {
            psAfn0cF25->bfSc = FALSE;
        }
        else
        {
            psAfn0cF25->bfSc = TRUE;
            eRet = emt_trans_sXX_XXXX(eTrans, &(psAfn0cF25->fSc), 
            &(psAfn0cF25_f->fSc));
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0cf25() trans error %d ", eRet);
                #endif
                return eRet;
            }
        } 
    }
    else if(MT_TRANS_U2F == eTrans)
    // 用户侧转为帧侧
    {
           // sReadTime
           usLen = sizeof(sMtYYMMDDhhmm_f);
           bNone = psAfn0cF25->bsReadTime;
           if(TRUE == bNone)
           {
               eRet = emt_trans_YYMMDDhhmm(eTrans, &(psAfn0cF25->sReadTime),
               &(psAfn0cF25_f->sReadTime));
               if(MT_OK != eRet)
               {
                   #ifdef MT_DBG
                   DEBUG("emtTrans_afn0cf25() trans error %d ", eRet);
                   #endif
                   return eRet;
               }
           }
           else
           {
               vmt_set_none((UINT8*)&(psAfn0cF25_f->sReadTime), usLen);
           }
   
           // fp
           usLen = sizeof(sMtFmt_sXX_XXXX);
           bNone = psAfn0cF25->bfP;
           if(TRUE == bNone)
           {
               eRet = emt_trans_sXX_XXXX(eTrans, &(psAfn0cF25->fP),
               &(psAfn0cF25_f->fP));
               if(MT_OK != eRet)
               {
                   #ifdef MT_DBG
                   DEBUG("emtTrans_afn0cf25() trans error %d ", eRet);
                   #endif
                   return eRet;
               }
           }
           else
           {
               vmt_set_none((UINT8*)&(psAfn0cF25_f->fP), usLen);
           }
           
           // fpa
           bNone = psAfn0cF25->bfPa;
           if(TRUE == bNone)
           {
               eRet = emt_trans_sXX_XXXX(eTrans, &(psAfn0cF25->fPa), 
               &(psAfn0cF25_f->fPa));
               if(MT_OK != eRet)
               {
                   #ifdef MT_DBG
                   DEBUG("emtTrans_afn0cf25() trans error %d ", eRet);
                   #endif
                   return eRet;
               }
           }
           else
           {
               vmt_set_none((UINT8*)&(psAfn0cF25_f->fPa), usLen);
           }
           
           // fpb
           bNone = psAfn0cF25->bfPb;
           if(TRUE == bNone)
           {
               eRet = emt_trans_sXX_XXXX(eTrans, &(psAfn0cF25->fPb), 
               &(psAfn0cF25_f->fPb));
               if(MT_OK != eRet)
               {
                   #ifdef MT_DBG
                   DEBUG("emtTrans_afn0cf25() trans error %d ", eRet);
                   #endif
                   return eRet;
               }
           }
           else
           {
               vmt_set_none((UINT8*)&(psAfn0cF25_f->fPb), usLen);
           }
   
           // fpc
           bNone = psAfn0cF25->bfPc;
           if(TRUE == bNone)
           {
               eRet = emt_trans_sXX_XXXX(eTrans, &(psAfn0cF25->fPc), 
               &(psAfn0cF25_f->fPc));
               if(MT_OK != eRet)
               {
                   #ifdef MT_DBG
                   DEBUG("emtTrans_afn0cf25() trans error %d ", eRet);
                   #endif
                   return eRet;
               }
           }
           else
           {
               vmt_set_none((UINT8*)&(psAfn0cF25_f->fPc), usLen);
           }
   
           // fQ
           bNone = psAfn0cF25->bfQ;
           if(TRUE == bNone)
           {
                eRet = emt_trans_sXX_XXXX(eTrans, &(psAfn0cF25->fQ), 
               &(psAfn0cF25_f->fQ));
               if(MT_OK != eRet)
               {
                   #ifdef MT_DBG
                   DEBUG("emtTrans_afn0cf25() trans error %d ", eRet);
                   #endif
                   return eRet;
               }
           }
           else
           {
               vmt_set_none((UINT8*)&(psAfn0cF25_f->fQ), usLen);
           }
           
           // fQa
           bNone = psAfn0cF25->bfQa;
           if(TRUE == bNone)
           {
               eRet = emt_trans_sXX_XXXX(eTrans, &(psAfn0cF25->fQa),
               &(psAfn0cF25_f->fQa));
               if(MT_OK != eRet)
               {
                   #ifdef MT_DBG
                   DEBUG("emtTrans_afn0cf25() trans error %d ", eRet);
                   #endif
                   return eRet;
               }
           }
           else
           {
               vmt_set_none((UINT8*)&(psAfn0cF25_f->fQa), usLen);
           }
           
           // fQb
           bNone = psAfn0cF25->bfQb;
           if(TRUE == bNone)
           {
               eRet = emt_trans_sXX_XXXX(eTrans, &(psAfn0cF25->fQb), 
               &(psAfn0cF25_f->fQb));
               if(MT_OK != eRet)
               {
                   #ifdef MT_DBG
                   DEBUG("emtTrans_afn0cf25() trans error %d ", eRet);
                   #endif
                   return eRet;
               }
           }
           else
           {
               vmt_set_none((UINT8*)&(psAfn0cF25_f->fQb), usLen);
           }
   
           // fQc
           bNone = psAfn0cF25->bfQc;
           if(TRUE == bNone)
           {
               eRet = emt_trans_sXX_XXXX(eTrans, &(psAfn0cF25->fQc), 
               &(psAfn0cF25_f->fQc));
               if(MT_OK != eRet)
               {
                   #ifdef MT_DBG
                   DEBUG("emtTrans_afn0cf25() trans error %d ", eRet);
                   #endif
                   return eRet;
               }
           }
           else
           {
               vmt_set_none((UINT8*)&(psAfn0cF25_f->fQc), usLen);
           }
   
           // Pf
           usLen = sizeof(sMtFmt_sXXX_X);
           bNone = psAfn0cF25->bfPf;
           if(TRUE == bNone)
           {
               eRet = emt_trans_sXXX_X(eTrans, &(psAfn0cF25->fPf), 
               &(psAfn0cF25_f->fPf));
               if(MT_OK != eRet)
               {
                   #ifdef MT_DBG
                   DEBUG("emtTrans_afn0cf25() trans error %d ", eRet);
                   #endif
                   return eRet;
               }
           }
           else
           {
               vmt_set_none((UINT8*)&(psAfn0cF25_f->fQc), usLen);
           }
           
           // Pfa
           bNone = psAfn0cF25->bfPfa;
           if(TRUE == bNone)
           {
               eRet = emt_trans_sXXX_X(eTrans, &(psAfn0cF25->fPfa),
               &(psAfn0cF25_f->fPfa));
               if(MT_OK != eRet)
               {
                   #ifdef MT_DBG
                   DEBUG("emtTrans_afn0cf25() trans error %d ", eRet);
                   #endif
                   return eRet;
               }
           }
           else
           {
               vmt_set_none((UINT8*)&(psAfn0cF25_f->fPfa), usLen);
           }
           
           // Pfb
           bNone = psAfn0cF25->bfPfb;
           if(TRUE == bNone)
           {
               eRet = emt_trans_sXXX_X(eTrans, &(psAfn0cF25->fPfb), 
               &(psAfn0cF25_f->fPfb));
               if(MT_OK != eRet)
               {
                   #ifdef MT_DBG
                   DEBUG("emtTrans_afn0cf25() trans error %d ", eRet);
                   #endif
                   return eRet;
               }
           }
           else
           {
               vmt_set_none((UINT8*)&(psAfn0cF25_f->fPfb), usLen);
           }
   
           // Pfc
           bNone = psAfn0cF25->bfPfc;
           if(TRUE == bNone)
           {
               eRet = emt_trans_sXXX_X(eTrans, &(psAfn0cF25->fPfc), 
               &(psAfn0cF25_f->fPfc));
               if(MT_OK != eRet)
               {
                   #ifdef MT_DBG
                   DEBUG("emtTrans_afn0cf25() trans error %d ", eRet);
                   #endif
                   return eRet;
               }
           }
           else
           {
               vmt_set_none((UINT8*)&(psAfn0cF25_f->fPfc), usLen);
           }
   
           // fUa
           usLen = sizeof(sMtFmt_XXX_X);
           bNone = psAfn0cF25->bfUa;
           if(TRUE == bNone)
           {
               eRet = emt_trans_XXX_X(eTrans, &(psAfn0cF25->fUa), 
               &(psAfn0cF25_f->fUa));
               if(MT_OK != eRet)
               {
                   #ifdef MT_DBG
                   DEBUG("emtTrans_afn0cf25() trans error %d ", eRet);
                   #endif
                   return eRet;
               }
           }
           else
           {
               vmt_set_none((UINT8*)&(psAfn0cF25_f->fUa), usLen);
           }
           
           // fUb
           bNone = psAfn0cF25->bfUb;
           if(TRUE == bNone)
           {
               eRet = emt_trans_XXX_X(eTrans, &(psAfn0cF25->fUb),
               &(psAfn0cF25_f->fUb));
               if(MT_OK != eRet)
               {
                   #ifdef MT_DBG
                   DEBUG("emtTrans_afn0cf25() trans error %d ", eRet);
                   #endif
                   return eRet;
               }
           }
           else
           {
               vmt_set_none((UINT8*)&(psAfn0cF25_f->fUb), usLen);
           }
           
           // fUc
           bNone = psAfn0cF25->bfUc;
           if(TRUE == bNone)
           {
               eRet = emt_trans_XXX_X(eTrans, &(psAfn0cF25->fUc), 
               &(psAfn0cF25_f->fUc));
               if(MT_OK != eRet)
               {
                   #ifdef MT_DBG
                   DEBUG("emtTrans_afn0cf25() trans error %d ", eRet);
                   #endif
                   return eRet;
               }
           }
           else
           {
               vmt_set_none((UINT8*)&(psAfn0cF25_f->fUc), usLen);
           }
   
           // fIa
           usLen = sizeof(sMtFmt_sXXX_XXX);
           bNone = psAfn0cF25->bfIa;
           if(TRUE == bNone)
           {
               eRet = emt_trans_sXXX_XXX(eTrans, &(psAfn0cF25->fIa), 
               &(psAfn0cF25_f->fIa));
               if(MT_OK != eRet)
               {
                   #ifdef MT_DBG
                   DEBUG("emtTrans_afn0cf25() trans error %d ", eRet);
                   #endif
                   return eRet;
               }
           }
           else
           {
               vmt_set_none((UINT8*)&(psAfn0cF25_f->fIa), usLen);
           }
           
           // fIb
           bNone = psAfn0cF25->bfIb;
           if(TRUE == bNone)
           {
               eRet = emt_trans_sXXX_XXX(eTrans, &(psAfn0cF25->fIb),
               &(psAfn0cF25_f->fIb));
               if(MT_OK != eRet)
               {
                   #ifdef MT_DBG
                   DEBUG("emtTrans_afn0cf25() trans error %d ", eRet);
                   #endif
                   return eRet;
               }
           }
           else
           {
               vmt_set_none((UINT8*)&(psAfn0cF25_f->fIb), usLen);
           }
           
           // fIc
           bNone = psAfn0cF25->bfIc;
           if(TRUE == bNone)
           {
               eRet = emt_trans_sXXX_XXX(eTrans, &(psAfn0cF25->fIc),
               &(psAfn0cF25_f->fIc));
               if(MT_OK != eRet)
               {
                   #ifdef MT_DBG
                   DEBUG("emtTrans_afn0cf25() trans error %d ", eRet);
                   #endif
                   return eRet;
               }
           }
           else
           {
               vmt_set_none((UINT8*)&(psAfn0cF25_f->fIc), usLen);
           }
   
           // fI0
           bNone = psAfn0cF25->bfI0;
           if(TRUE == bNone)
           {
               eRet = emt_trans_sXXX_XXX(eTrans, &(psAfn0cF25->fI0),
               &(psAfn0cF25_f->fI0));
               if(MT_OK != eRet)
               {
                   #ifdef MT_DBG
                   DEBUG("emtTrans_afn0cf25() trans error %d ", eRet);
                   #endif
                   return eRet;
               }
           }
           else
           {
               vmt_set_none((UINT8*)&(psAfn0cF25_f->fI0), usLen);
           }
   
           // fS
           usLen = sizeof(sMtFmt_sXX_XXXX);
           bNone = psAfn0cF25->bfS;
           if(TRUE == bNone)
           {
               eRet = emt_trans_sXX_XXXX(eTrans, &(psAfn0cF25->fS),
               &(psAfn0cF25_f->fS));
               if(MT_OK != eRet)
               {
                   #ifdef MT_DBG
                   DEBUG("emtTrans_afn0cf25() trans error %d ", eRet);
                   #endif
                   return eRet;
               }
           }
           else
           {
               vmt_set_none((UINT8*)&(psAfn0cF25_f->fS), usLen);
           }
           
           // fSa
           bNone = psAfn0cF25->bfSa;
           if(TRUE == bNone)
           {
               eRet = emt_trans_sXX_XXXX(eTrans, &(psAfn0cF25->fSa),
               &(psAfn0cF25_f->fSa));
                 if(MT_OK != eRet)
               {
                   #ifdef MT_DBG
                   DEBUG("emtTrans_afn0cf25() trans error %d ", eRet);
                   #endif
                   return eRet;
               }
           }
           else
           {
               vmt_set_none((UINT8*)&(psAfn0cF25_f->fSa), usLen);
           }
           
           // fSb
           bNone = psAfn0cF25->bfSb;
           if(TRUE == bNone)
           {
               eRet = emt_trans_sXX_XXXX(eTrans, &(psAfn0cF25->fSb),
               &(psAfn0cF25_f->fSb));
               if(MT_OK != eRet)
               {
                   #ifdef MT_DBG
                   DEBUG("emtTrans_afn0cf25() trans error %d ", eRet);
                   #endif
                   return eRet;
               }
           }
           else
           {
               vmt_set_none((UINT8*)&(psAfn0cF25_f->fSb), usLen);
           }
           
           // fpc
           bNone = psAfn0cF25->bfSc;
           if(TRUE == bNone)
           {
               eRet = emt_trans_sXX_XXXX(eTrans, &(psAfn0cF25->fSc),
               &(psAfn0cF25_f->fSc));
               if(MT_OK != eRet)
               {
                   #ifdef MT_DBG
                   DEBUG("emtTrans_afn0cf25() trans error %d ", eRet);
                   #endif
                   return eRet;
               }
           }
           else
           {
               vmt_set_none((UINT8*)&(psAfn0cF25_f->fSc), usLen);
           }
           
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf25() para error!");
        #endif
        return MT_ERR_PARA;
    }

    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0cF25_f);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf26
 功能描述  :  格式转换函数
              请求1类数据（AFN=0CH）
              F26： A、B、C三相断相统计数据及最近一次断相记录
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月22日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf26(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if (!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf26() pointer is null");
        #endif
        return MT_ERR_NULL;
    }
  
    if (MT_TRANS_F2U == eTrans) // 帧侧到用户侧
    {
        // 暂时不做处理，只计算数据长度
    }else if ( MT_TRANS_U2F == eTrans)  // 用户侧到帧侧
    {
        // 暂时不做处理，只计算数据长度
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf26() para error!");
        #endif
        return MT_ERR_PARA;
    }

    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0cF26_f);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf27
 功能描述  :  格式转换函数
              请求1类数据（AFN=0CH）
              F27：  电能表日历时钟、编程次数及其最近一次操作时?
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月23日 星期五
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf27(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if (!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf27() pointer is null");
        #endif
        return MT_ERR_NULL;
    }
  
    if (MT_TRANS_F2U == eTrans) // 帧侧到用户侧
    {
        // 暂时不做处理，只计算数据长度
    }else if ( MT_TRANS_U2F == eTrans)  // 用户侧到帧侧
    {
        // 暂时不做处理，只计算数据长度
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf27() para error!");
        #endif
        return MT_ERR_PARA;
    }

    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0cF27_f);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf28
 功能描述  :  格式转换函数
              请求1类数据（AFN=0CH）
              F28：   电表运行状态字及其变位标志
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月23日 星期五
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf28(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if (!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf28() pointer is null");
        #endif
        return MT_ERR_NULL;
    }
  
    if (MT_TRANS_F2U == eTrans) // 帧侧到用户侧
    {
        // 暂时不做处理，只计算数据长度
    }else if ( MT_TRANS_U2F == eTrans)  // 用户侧到帧侧
    {
        // 暂时不做处理，只计算数据长度
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf28() para error!");
        #endif
        return MT_ERR_PARA;
    }

    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0cF28_f);
    return MT_OK;
}


/*****************************************************************************
 函 数 名  : emtTrans_afn0cf29
 功能描述  :  格式转换函数
              请求1类数据（AFN=0CH）
              F29：   当前铜损、铁损有功总电能示值
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月23日 星期五
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf29(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if (!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf29() pointer is null");
        #endif
        return MT_ERR_NULL;
    }
  
    if (MT_TRANS_F2U == eTrans) // 帧侧到用户侧
    {
        // 暂时不做处理，只计算数据长度
    }else if ( MT_TRANS_U2F == eTrans)  // 用户侧到帧侧
    {
        // 暂时不做处理，只计算数据长度
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf29() para error!");
        #endif
        return MT_ERR_PARA;
    }

    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0cF29_f);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf30
 功能描述  :  格式转换函数
              请求1类数据（AFN=0CH）
              F30：   上一结算日铜损、铁损有功总电能示值
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月23日 星期五
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf30(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if (!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf30() pointer is null");
        #endif
        return MT_ERR_NULL;
    }
  
    if (MT_TRANS_F2U == eTrans) // 帧侧到用户侧
    {
        // 暂时不做处理，只计算数据长度
    }else if ( MT_TRANS_U2F == eTrans)  // 用户侧到帧侧
    {
        // 暂时不做处理，只计算数据长度
    }
    else  
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf30() para error!");
        #endif
        return MT_ERR_PARA;
    }

    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0cF30_f);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf31
 功能描述  : F31：当前A、B、C三相正/反向有功电能示值、组合无功1/2电能示值
             CMD_AFN_C_F31_POWER_CUR
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月20日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf31(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if (!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf31() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    sMtAfn0cf31     *psAfn0cF31   = (sMtAfn0cf31*)psUser;
    sMtAfn0cf31_f   *psAfn0cF31_f = (sMtAfn0cf31_f*)psFrame;
   
    // 终端抄表时间
    emt_trans_YYMMDDhhmm(eTrans, &(psAfn0cF31->sTime),&(psAfn0cF31_f->sTime));

    // 当前A相 正/ 反有功电能示值、组合无功1 / 2电能示值
    emt_trans_XXXXXX_XXXX(eTrans, 
    &(psAfn0cF31->sPhaseA.dFrthHavePower),&(psAfn0cF31_f->sPhaseA.dFrthHavePower));

    emt_trans_XXXXXX_XXXX(eTrans, 
    &(psAfn0cF31->sPhaseA.dBackHavePower),&(psAfn0cF31_f->sPhaseA.dBackHavePower));

    emt_trans_XXXXXX_XX(eTrans,
        &(psAfn0cF31->sPhaseA.fComNonePower1),&(psAfn0cF31_f->sPhaseA.fComNonePower1));

    emt_trans_XXXXXX_XX(eTrans,
        &(psAfn0cF31->sPhaseA.fComNonePower2),&(psAfn0cF31_f->sPhaseA.fComNonePower2));

    // 当前B相 正/ 反有功电能示值、组合无功1 / 2电能示值
    emt_trans_XXXXXX_XXXX(eTrans, 
    &(psAfn0cF31->sPhaseB.dFrthHavePower),&(psAfn0cF31_f->sPhaseB.dFrthHavePower));

    emt_trans_XXXXXX_XXXX(eTrans, 
    &(psAfn0cF31->sPhaseB.dBackHavePower),&(psAfn0cF31_f->sPhaseB.dBackHavePower));

    emt_trans_XXXXXX_XX(eTrans,
        &(psAfn0cF31->sPhaseB.fComNonePower1),&(psAfn0cF31_f->sPhaseB.fComNonePower1));

    emt_trans_XXXXXX_XX(eTrans,
        &(psAfn0cF31->sPhaseB.fComNonePower2),&(psAfn0cF31_f->sPhaseB.fComNonePower2));

     // 当前B相 正/ 反有功电能示值、组合无功1 / 2电能示值
    emt_trans_XXXXXX_XXXX(eTrans, 
    &(psAfn0cF31->sPhaseC.dFrthHavePower),&(psAfn0cF31_f->sPhaseC.dFrthHavePower));

    emt_trans_XXXXXX_XXXX(eTrans, 
    &(psAfn0cF31->sPhaseC.dBackHavePower),&(psAfn0cF31_f->sPhaseC.dBackHavePower));

    emt_trans_XXXXXX_XX(eTrans,
        &(psAfn0cF31->sPhaseC.fComNonePower1),&(psAfn0cF31_f->sPhaseC.fComNonePower1));

    emt_trans_XXXXXX_XX(eTrans,
        &(psAfn0cF31->sPhaseC.fComNonePower2),&(psAfn0cF31_f->sPhaseC.fComNonePower2));
    
    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0cf31_f);
     return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf32
 功能描述  : F32：上一结算日A、B、C三相正/反向有功电能示值、组合无功1/2电能示值
             CMD_AFN_C_F32_POWER_LAST
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月26日 星期一
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf32(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
   return emtTrans_afn0cf31(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf33
 功能描述  : F33：当前正向有/ 无功电能示值、一/ 四象武功电能示值 ( 总、费率1  ~ M, 1 <= M  <= 12)
             CMD_AFN_C_F33_FRTH_POWR_P1P4_C
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月26日 星期一
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf33(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if (!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf33() pointer is null");
        #endif
        return MT_ERR_NULL;
    }
    
    sMtAfn0cf33_f  *psMtAfn0cf33_f = (sMtAfn0cf33_f *)psFrame;
    UINT16         usLen = 0;
    if (MT_TRANS_F2U == eTrans) // 帧侧到用户侧
    {
        // 暂时不做处理，只计算数据长度
        // 帧头部
        usLen += sizeof(sMtAfn0cf33_f);     

        // 当前正向有功
        usLen += sizeof(sMtCurFrthHaveValue) + psMtAfn0cf33_f->ucRateM * sizeof(sMtFmt14_f);


        // 当前正向无功
        usLen += sizeof(sMtCurFrthNoneValue) + psMtAfn0cf33_f->ucRateM * sizeof(sMtFmt11_f);

        // 当前一象限无功电能
        usLen += sizeof(sMtCurP1NoneValue) + psMtAfn0cf33_f->ucRateM * sizeof(sMtFmt11_f);

        // 当前四象限无功电能
        usLen += sizeof(sMtCurP4NoneValue) + psMtAfn0cf33_f->ucRateM * sizeof(sMtFmt11_f);
        
    }else if ( MT_TRANS_U2F == eTrans)  // 用户侧到帧侧
    {
        // 暂时不做处理，只计算数据长度
    }
    else  
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf33() para error!");
        #endif
        return MT_ERR_PARA;
    }
    
    // 计算在帧侧的字节长度
    *pusfLen = usLen;
     return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf34
 功能描述  : F34：当前反向有/ 无功电能示值、二/ 三象无功电能示值 ( 总、费率1  ~ M, 1 <= M  <= 12)
             CMD_AFN_C_F34_BACK_POWR_P2P3_C
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月26日 星期一
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf34(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if (!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf34() pointer is null");
        #endif
        return MT_ERR_NULL;
    }
    
    sMtAfn0cf34_f *psMtAfn0cf34_f = (sMtAfn0cf34_f *)psFrame;
    UINT16        usLen = 0;
    if (MT_TRANS_F2U == eTrans) // 帧侧到用户侧
    {
        // 暂时不做处理，只计算数据长度
        // 帧头部
        usLen += sizeof(sMtAfn0cf34_f);     

        // 当前反向有功
        usLen += sizeof(sMtCurBackHaveValue) + psMtAfn0cf34_f->ucRateM * sizeof(sMtFmt14_f);

        // 当前反向无功
        usLen += sizeof(sMtCurBackNoneValue) + psMtAfn0cf34_f->ucRateM * sizeof(sMtFmt11_f);

        // 当前二象限无功电能
        usLen += sizeof(sMtP2NoneValue) + psMtAfn0cf34_f->ucRateM * sizeof(sMtFmt11_f);

        // 当前三象限无功电能
        usLen += sizeof(sMtP3NoneValue) + psMtAfn0cf34_f->ucRateM * sizeof(sMtFmt11_f);
        
    }else if ( MT_TRANS_U2F == eTrans)  // 用户侧到帧侧
    {
        // 暂时不做处理，只计算数据长度
    }
    else  
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf34() para error!");
        #endif
        return MT_ERR_PARA;
    }
    
    // 计算在帧侧的字节长度
    *pusfLen = usLen;
     return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf35
 功能描述  : F35：// 当月正向有/无功最大需量及发生时间（总、费率1～M，1≤M≤12）
             CMD_AFN_C_F35_FRTH_DMND_M
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月26日 星期一
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf35(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if (!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf35() pointer is null");
        #endif
        return MT_ERR_NULL;
    }
    
    sMtAfn0cf35_f *psMtAfn0cf35_f = (sMtAfn0cf35_f *)psFrame;
    UINT16        usLen = 0;
    if (MT_TRANS_F2U == eTrans) // 帧侧到用户侧
    {
        // 暂时不做处理，只计算数据长度
        // 帧头部
        usLen += sizeof(sMtAfn0cf35_f);     

        // 当月正向有功最大需求量
        usLen += sizeof(sMtCurFrthHaveDmnd) + psMtAfn0cf35_f->ucRateM * sizeof(sMtFmt23_f);

        // 当月正向有功最大需求量发生时间
        usLen += sizeof(sMtCurFrthHaveTime) + psMtAfn0cf35_f->ucRateM * sizeof(sMtFmt17_f);

        // 当月正向无功最大需求量
        usLen += sizeof(sMtCurFrthNoneDmnd) + psMtAfn0cf35_f->ucRateM * sizeof(sMtFmt23_f);

        // 当月正向无功最大需求量发生时间
        usLen += sizeof(sMtCurFrthNoneTime) + psMtAfn0cf35_f->ucRateM * sizeof(sMtFmt17_f);
        
    }else if ( MT_TRANS_U2F == eTrans)  // 用户侧到帧侧
    {
        // 暂时不做处理，只计算数据长度
    }
    else  
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf35() para error!");
        #endif
        return MT_ERR_PARA;
    }
    
    // 计算在帧侧的字节长度
    *pusfLen = usLen;
     return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf36
 功能描述  : F36：当月反向有/无功最大需量及发生时间（总、费率1～M，1≤M≤12）
             CMD_AFN_C_F36_BACK_DMND_M
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月28日 星期三
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf36(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if (!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf36() pointer is null");
        #endif
        return MT_ERR_NULL;
    }
    
    sMtAfn0cf36_f *psMtAfn0cf36_f = (sMtAfn0cf36_f *)psFrame;
    UINT16        usLen = 0;
    if (MT_TRANS_F2U == eTrans) // 帧侧到用户侧
    {
        // 暂时不做处理，只计算数据长度
        // 帧头部
        usLen += sizeof(sMtAfn0cf36_f);     

        // 当月反向有功最大需求量
        usLen += sizeof(sMtCurBackHaveDmnd) + psMtAfn0cf36_f->ucRateM * sizeof(sMtFmt23_f);

        // 当月反向有功最大需求量发生时间
        usLen += sizeof(sMtCurBackHaveTime) + psMtAfn0cf36_f->ucRateM * sizeof(sMtFmt17_f);

        // 当月反向无功最大需求量
        usLen += sizeof(sMtCurBackNoneDmnd) + psMtAfn0cf36_f->ucRateM * sizeof(sMtFmt23_f);

        // 当月反向无功最大需求量发生时间
        usLen += sizeof(sMtCurBackNoneTime) + psMtAfn0cf36_f->ucRateM * sizeof(sMtFmt17_f);
        
    }else if ( MT_TRANS_U2F == eTrans)  // 用户侧到帧侧
    {
        // 暂时不做处理，只计算数据长度
    }
    else  
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf36() para error!");
        #endif
        return MT_ERR_PARA;
    }
    
    // 计算在帧侧的字节长度
    *pusfLen = usLen;
     return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf37
 功能描述  : F37：上月（上一结算日）正向有/无功（组合无功1）电能示值、一/四象限无功电能示值（总、费率1～M，1≤M≤12）
             CMD_AFN_C_F37_FRTH_POWR_P1P4_L
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月28日 星期三
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf37(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if (!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf37() pointer is null");
        #endif
        return MT_ERR_NULL;
    }
    
    sMtAfn0cf37_f *psMtAfn0cf37_f = (sMtAfn0cf37_f *)psFrame;
    UINT16        usLen = 0;
    if (MT_TRANS_F2U == eTrans) // 帧侧到用户侧
    {
        // 暂时不做处理，只计算数据长度
        // 帧头部
        usLen += sizeof(sMtAfn0cf37_f);     

        // 上月( 上一结算日) 正向有功电能示值
        usLen += sizeof(sMtLastFrthHaveValue) + psMtAfn0cf37_f->ucRateM * sizeof(sMtFmt14_f);

        // 上月(上一结算日) 正向无功电能示值
        usLen += sizeof(sMtLastFrthNoneValue) + psMtAfn0cf37_f->ucRateM * sizeof(sMtFmt11_f);

        // 上月(上一结算日) 一象限无功电能示值
        usLen += sizeof(sMtLastP1NoneValue) + psMtAfn0cf37_f->ucRateM * sizeof(sMtFmt11_f);

        // 上月(上一结算日) 四象限无功电能示值
        usLen += sizeof(sMtLastP4NoneValue) + psMtAfn0cf37_f->ucRateM * sizeof(sMtFmt11_f);
        
    }else if ( MT_TRANS_U2F == eTrans)  // 用户侧到帧侧
    {
        // 暂时不做处理，只计算数据长度
    }
    else  
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf37() para error!");
        #endif
        return MT_ERR_PARA;
    }
    
    // 计算在帧侧的字节长度
    *pusfLen = usLen;
     return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf38
 功能描述  : F37：上月（上一结算日）反向有/无功（组合无功1）电能示值、一/四象限无功电能示值（总、费率1～M，1≤M≤12）
             CMD_AFN_C_F38_BACK_POWR_P2P3_L
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月28日 星期三
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf38(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if (!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf38() pointer is null");
        #endif
        return MT_ERR_NULL;
    }
    
    sMtAfn0cf38_f *psMtAfn0cf38_f = (sMtAfn0cf38_f *)psFrame;
    UINT16        usLen = 0;
    if (MT_TRANS_F2U == eTrans) // 帧侧到用户侧
    {
        // 暂时不做处理，只计算数据长度
        // 帧头部
        usLen += sizeof(sMtAfn0cf38_f);     

        // 上月( 上一结算日) 反向有功电能示值
        usLen += sizeof(sMtLastBackHaveValue) + psMtAfn0cf38_f->ucRateM * sizeof(sMtFmt14_f);

        // 上月(上一结算日) 反向无功电能示值
        usLen += sizeof(sMtLastBackNoneValue) + psMtAfn0cf38_f->ucRateM * sizeof(sMtFmt11_f);

        // 上月(上一结算日) 二象限无功电能示值
        usLen += sizeof(sMtLastP2NoneValue) + psMtAfn0cf38_f->ucRateM * sizeof(sMtFmt11_f);

        // 上月(上一结算日) 三象限无功电能示值
        usLen += sizeof(sMtLastP3NoneValue) + psMtAfn0cf38_f->ucRateM * sizeof(sMtFmt11_f);
        
    }else if ( MT_TRANS_U2F == eTrans)  // 用户侧到帧侧
    {
        // 暂时不做处理，只计算数据长度
    }
    else  
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf38() para error!");
        #endif
        return MT_ERR_PARA;
    }
    
    // 计算在帧侧的字节长度
    *pusfLen = usLen;
     return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf39
 功能描述  : F39：上月（上一结算日）正向有/无功最大需量及发生时间（总、费率1～M，1≤M≤12）
             CMD_AFN_C_F39_FRTH_DMND_L
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月26日 星期一
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf39(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if (!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf39() pointer is null");
        #endif
        return MT_ERR_NULL;
    }
    
    sMtAfn0cf39_f *psMtAfn0cf39_f = (sMtAfn0cf39_f *)psFrame;
    UINT16        usLen = 0;
    if (MT_TRANS_F2U == eTrans) // 帧侧到用户侧
    {
        // 暂时不做处理，只计算数据长度
        // 帧头部
        usLen += sizeof(sMtAfn0cf39_f);     

        // 上月（上一结算日）正向有功最大需求量
        usLen += sizeof(sMtLastFrthHaveDmnd) + psMtAfn0cf39_f->ucRateM * sizeof(sMtFmt23_f);

        // 上月（上一结算日）正向有功最大需求量发生时间
        usLen += sizeof(sMtLastFrthHaveTime) + psMtAfn0cf39_f->ucRateM * sizeof(sMtFmt17_f);

        // 上月（上一结算日）正向无功最大需求量
        usLen += sizeof(sMtLastFrthNoneDmnd) + psMtAfn0cf39_f->ucRateM * sizeof(sMtFmt23_f);

        // 上月（上一结算日）正向无功最大需求量发生时间
        usLen += sizeof(sMtLastFrthNoneTime) + psMtAfn0cf39_f->ucRateM * sizeof(sMtFmt17_f);
        
    }else if ( MT_TRANS_U2F == eTrans)  // 用户侧到帧侧
    {
        // 暂时不做处理，只计算数据长度
    }
    else  
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf39() para error!");
        #endif
        return MT_ERR_PARA;
    }
    
    // 计算在帧侧的字节长度
    *pusfLen = usLen;
     return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf40
 功能描述  : F40：上月（上一结算日）反向有/无功最大需量及发生时间（总、费率1～M，1≤M≤12）
             CMD_AFN_C_F40_BACK_DMND_L
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月28日 星期三
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf40(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if (!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf40() pointer is null");
        #endif
        return MT_ERR_NULL;
    }
    
    sMtAfn0cf40_f *psMtAfn0cf40_f = (sMtAfn0cf40_f *)psFrame;
    UINT16        usLen = 0;
    if (MT_TRANS_F2U == eTrans) // 帧侧到用户侧
    {
        // 暂时不做处理，只计算数据长度
        // 帧头部
        usLen += sizeof(sMtAfn0cf40_f);     

        // 上月（上一结算日）反向有功最大需求量
        usLen += sizeof(sMtLastBackHaveDmnd) + psMtAfn0cf40_f->ucRateM * sizeof(sMtFmt23_f);

        // 上月（上一结算日）反向有功最大需求量发生时间
        usLen += sizeof(sMtLastBackHaveTime) + psMtAfn0cf40_f->ucRateM * sizeof(sMtFmt17_f);

        // 上月（上一结算日）反向无功最大需求量
        usLen += sizeof(sMtLastBackNoneDmnd) + psMtAfn0cf40_f->ucRateM * sizeof(sMtFmt23_f);

        // 上月（上一结算日）反向无功最大需求量发生时间
        usLen += sizeof(sMtLastBackNoneTime) + psMtAfn0cf40_f->ucRateM * sizeof(sMtFmt17_f);
        
    }else if ( MT_TRANS_U2F == eTrans)  // 用户侧到帧侧
    {
        // 暂时不做处理，只计算数据长度
    }
    else  
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf40() para error!");
        #endif
        return MT_ERR_PARA;
    }
    
    // 计算在帧侧的字节长度
    *pusfLen = usLen;
     return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf41
 功能描述  : 当日正向有功电能量（总、费率1～M） CMD_AFN_C_F41_FRTH_HAVE_POWR_D
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月13日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf41(eMtTrans eTrans, void* psUser, void* psFrame, UINT16* pusfLen)
{
    if(!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf41() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    sMtAfn0cF41     *psAfn0cF41   = (sMtAfn0cF41*)psUser;
    sMtAfn0cF41_f   *psAfn0cF41_f = (sMtAfn0cF41_f*)psFrame;
    BOOL             bNone        = FALSE;
    UINT16           usLen        = 0;
    INT32            i            = 0;

    // 帧侧转为用户侧
    if(MT_TRANS_F2U == eTrans)
    {
        // 费率M
        usLen = sizeof(UINT8);
        bNone = bmt_is_none((UINT8*)&(psAfn0cF41_f->ucM), usLen);
        if(TRUE == bNone)
        {
           psAfn0cF41->bucM = FALSE;
        }
        else
        {
            psAfn0cF41->bucM = TRUE;
            psAfn0cF41->ucM = psAfn0cF41_f->ucM;
            usLen = sizeof(sMtFmt_XXXX_XXXX);
            
            if(psAfn0cF41->ucM > MT_PAY_RATE_MAX)
            {
                return MT_ERR_OUTRNG;
            }
            
            for(i = 0; i < psAfn0cF41->ucM; i++)
            {
               bNone = bmt_is_none((UINT8*)&(psAfn0cF41_f->dForthHavePowerMD[i]), usLen);
               if(TRUE == bNone)
               {
                   psAfn0cF41->bdForthHavePowerMD[i] = FALSE;
               }
               else
               {
                   psAfn0cF41->bdForthHavePowerMD[i] = TRUE;
                   emt_trans_XXXX_XXXX(eTrans, &(psAfn0cF41->dForthHavePowerMD[i]), &(psAfn0cF41_f->dForthHavePowerMD[i]));
               }
            }
        }

        // 当日正向有功总电能量
        usLen = sizeof(sMtFmt_XXXX_XXXX);
        bNone = bmt_is_none((UINT8*)&(psAfn0cF41_f->dForthHavePowerD), usLen);
        if(bNone)
        {
           psAfn0cF41->bdForthHavePowerD = FALSE;
        }
        else
        {
            psAfn0cF41->bdForthHavePowerD = TRUE;
            emt_trans_XXXX_XXXX(eTrans, &(psAfn0cF41->dForthHavePowerD), &(psAfn0cF41_f->dForthHavePowerD));
        }
    }
    else if(MT_TRANS_U2F == eTrans)
    // 用户侧转为帧侧
    {
        bNone = !psAfn0cF41->bucM;
        usLen = sizeof(UINT8);
        if(TRUE == bNone)
        {
            vmt_set_none((UINT8*)&(psAfn0cF41->ucM), usLen);
        }
        else
        {
            psAfn0cF41_f->ucM = psAfn0cF41->ucM;
            if(psAfn0cF41->ucM > MT_PAY_RATE_MAX)
            {
                return MT_ERR_OUTRNG;
            }

            for(i = 0; i < psAfn0cF41->ucM; i++)
            {
                bNone = !psAfn0cF41->bdForthHavePowerMD[i];
                usLen = sizeof(sMtFmt_XXXX_XXXX);
                if(TRUE == bNone)
                {
                  vmt_set_none((UINT8*)&(psAfn0cF41_f->dForthHavePowerMD[i]), usLen);
                }
                else
                {
                    emt_trans_XXXX_XXXX(eTrans, &(psAfn0cF41->dForthHavePowerMD[i]), &(psAfn0cF41_f->dForthHavePowerMD[i])); 
                }
            }
        }

        bNone = !psAfn0cF41->bdForthHavePowerD;
        usLen = sizeof(sMtFmt_XXXX_XXXX);
        if(TRUE == bNone)
        {
            vmt_set_none((UINT8*)&(psAfn0cF41_f->dForthHavePowerD), usLen);
        }
        else
        {
            emt_trans_XXXX_XXXX(eTrans, &(psAfn0cF41->dForthHavePowerD), &(psAfn0cF41_f->dForthHavePowerD)); 
        }
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf41() para error!");
        #endif
        return MT_ERR_PARA;
    }

    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0cF41_f);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf42
 功能描述  : 当日正向无功电能量（总、费率1～M） CMD_AFN_C_F42_FRTH_NONE_POWR_D
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月13日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf42(eMtTrans eTrans, void* psUser, void* psFrame, UINT16* pusfLen)
{
    // 数据结构一样, 用统一的转换函数
    return emtTrans_afn0cf41(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf43
 功能描述  : 当日反向有功电能量（总、费率1～M）CMD_AFN_C_F43_BACK_HAVE_POWR_D
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月13日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf43(eMtTrans eTrans, void* psUser, void* psFrame, UINT16* pusfLen)
{
    // 数据结构一样, 用统一的转换函数
    return emtTrans_afn0cf41(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf44
 功能描述  : 当日反向无功电能量（总、费率1～M）CMD_AFN_C_F44_BACK_NONE_POWR_D
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月13日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf44(eMtTrans eTrans, void* psUser, void* psFrame, UINT16* pusfLen)
{
    // 数据结构一样, 用统一的转换函数
    return emtTrans_afn0cf41(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf45
 功能描述  : 当月正向有功电能量（总、费率1～M）CMD_AFN_C_F45_FRTH_HAVE_POWR_M
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月13日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf45(eMtTrans eTrans, void* psUser, void* psFrame, UINT16* pusfLen)
{
    // 数据结构一样, 用统一的转换函数
    return emtTrans_afn0cf41(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf46
 功能描述  : 当月正向无功电能量（总、费率1～M）CMD_AFN_C_F46_FRTH_NONE_POWR_M
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月13日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf46(eMtTrans eTrans, void* psUser, void* psFrame, UINT16* pusfLen)
{
    // 数据结构一样, 用统一的转换函数
    return emtTrans_afn0cf41(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf47
 功能描述  : 当月反向有功电能量（总、费率1～M）CMD_AFN_C_F47_BACK_HAVE_POWR_M
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月13日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf47(eMtTrans eTrans, void* psUser, void* psFrame, UINT16* pusfLen)
{
    // 数据结构一样, 用统一的转换函数
    return emtTrans_afn0cf41(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf48
 功能描述  : 当月反向无功电能量（总、费率1～M）CMD_AFN_C_F48_BACK_NONE_POWR_M
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月13日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf48(eMtTrans eTrans, void* psUser, void* psFrame, UINT16* pusfLen)
{
    // 数据结构一样, 用统一的转换函数
    return emtTrans_afn0cf41(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf49
 功能描述  : F49：当前电压、电流相位角
             CMD_AFN_C_F49_CURT_PHASE_ANGLE
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月28日 星期三
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf49(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if (!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf49() pointer is null");
        #endif
        return MT_ERR_NULL;
    }
    
    if (MT_TRANS_F2U == eTrans) // 帧侧到用户侧
    {
        // 暂时不做处理，只计算数据长度
    }else if ( MT_TRANS_U2F == eTrans)  // 用户侧到帧侧
    {
        // 暂时不做处理，只计算数据长度
    }
    else  
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf49() para error!");
        #endif
        return MT_ERR_PARA;
    }
    
    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0cf49_f);
     return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf57
 功能描述  : F57：当前A、B、C三相电压、电流2～N次谐波有效值 CMD_AFN_C_F57_CURT_HARM_VALUE
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月20日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf57(eMtTrans eTrans, void* psUser, void* psFrame, UINT16* pusfLen)
{
    if(!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf57() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    sMtAfn0cF57     *psAfn0cF57   = (sMtAfn0cF57*)psUser;
    sMtAfn0cF57_f   *psAfn0cF57_f = (sMtAfn0cF57_f*)psFrame;
    BOOL             bNone        = FALSE;
    INT32            i            = 0;
    INT32            nCycMax      = 0;
    INT32            nOffset      = 0;
    UINT8            ucN          = 0;
    eMtErr           eRet         = MT_OK;
    
    // 帧侧转为用户侧
    if(MT_TRANS_F2U == eTrans)
    {
        ucN     = psAfn0cF57_f->ucN;
        if(MT_HARM_TIMES_MIN > ucN || MT_HARM_TIMES_MAX < ucN)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0cf57() ucN is out of range ucN = %d", ucN);
            #endif
            return MT_ERR_OUTRNG;
        }

        psAfn0cF57->ucN = ucN;
        nCycMax = ucN - 1;

        // ua
        for(i = 0; i < nCycMax; i++)
        {
            bNone = bmt_is_none((UINT8*)&(psAfn0cF57_f->uHarm[i]), sizeof(sMtFmt_XXX_X));
            if(TRUE == bNone)
            {
                psAfn0cF57->sUa.bfXXX_X[i] = FALSE;
            }
            else
            {
                psAfn0cF57->sUa.bfXXX_X[i] = TRUE;
                eRet = emt_trans_XXX_X(eTrans, 
                                       &(psAfn0cF57->sUa.fXXX_X[i]), 
                                       (sMtFmt07*)&(psAfn0cF57_f->uHarm[i]));
                if(MT_OK != eRet)
                {
                    #ifdef MT_DBG
                    DEBUG("emt_trans_XXX_X() error = %d", eRet);
                    #endif       
                    return eRet;
                }
            }
        }

        nOffset = nCycMax;

        // Ub
        for(i = 0; i < nCycMax; i++)
        {
            bNone = bmt_is_none((UINT8*)&(psAfn0cF57_f->uHarm[nOffset + i]), sizeof(sMtFmt_XXX_X));
            if(TRUE == bNone)
            {
                psAfn0cF57->sUb.bfXXX_X[i] = FALSE;
            }
            else
            {
                psAfn0cF57->sUb.bfXXX_X[i] = TRUE;
                eRet = emt_trans_XXX_X(eTrans, 
                                       &(psAfn0cF57->sUb.fXXX_X[i]), 
                                       (sMtFmt07*)&(psAfn0cF57_f->uHarm[nOffset + i]));
                if(MT_OK != eRet)
                {
                    #ifdef MT_DBG
                    DEBUG("emt_trans_XXX_X() error = %d", eRet);
                    #endif       
                    return eRet;
                }
            }
        }

        nOffset = 2 * nCycMax;

        // Uc
        for(i = 0; i < nCycMax; i++)
        {
            bNone = bmt_is_none((UINT8*)&(psAfn0cF57_f->uHarm[nOffset + i]), sizeof(sMtFmt_XXX_X));
            if(TRUE == bNone)
            {
                psAfn0cF57->sUc.bfXXX_X[i] = FALSE;
            }
            else
            {
                psAfn0cF57->sUc.bfXXX_X[i] = TRUE;
                eRet = emt_trans_XXX_X(eTrans, 
                                       &(psAfn0cF57->sUc.fXXX_X[i]), 
                                       (sMtFmt07*)&(psAfn0cF57_f->uHarm[nOffset + i]));
                if(MT_OK != eRet)
                {
                    #ifdef MT_DBG
                    DEBUG("emt_trans_XXX_X() error = %d", eRet);
                    #endif       
                    return eRet;
                }
            }
        }

        nOffset = 3 * nCycMax;

        // Ia
        for(i = 0; i < nCycMax; i++)
        {
            bNone = bmt_is_none((UINT8*)&(psAfn0cF57_f->uHarm[nOffset + i]), sizeof(sMtFmt_sXX_XX));
            if(TRUE == bNone)
            {
                psAfn0cF57->sIa.bfsXX_XX[i] = FALSE;
            }
            else
            {
                psAfn0cF57->sIa.bfsXX_XX[i] = TRUE;
                eRet = emt_trans_sXX_XX(eTrans, 
                                       &(psAfn0cF57->sIa.fsXX_XX[i]), 
                                       (sMtFmt06*)&(psAfn0cF57_f->uHarm[nOffset + i]));
                if(MT_OK != eRet)
                {
                    #ifdef MT_DBG
                    DEBUG("emt_trans_sXX_XX() error = %d", eRet);
                    #endif       
                    return eRet;
                }
            }
        }

        nOffset = 4 * nCycMax;
      
        // Ib
        for(i = 0; i < nCycMax; i++)
        {
            bNone = bmt_is_none((UINT8*)&(psAfn0cF57_f->uHarm[nOffset + i]), sizeof(sMtFmt_sXX_XX));
            if(TRUE == bNone)
            {
                psAfn0cF57->sIb.bfsXX_XX[i] = FALSE;
            }
            else
            {
                psAfn0cF57->sIb.bfsXX_XX[i] = TRUE;
                eRet = emt_trans_sXX_XX(eTrans, 
                                       &(psAfn0cF57->sIb.fsXX_XX[i]), 
                                       (sMtFmt06*)&(psAfn0cF57_f->uHarm[nOffset + i]));
                if(MT_OK != eRet)
                {
                    #ifdef MT_DBG
                    DEBUG("emt_trans_sXX_XX() error = %d", eRet);
                    #endif       
                    return eRet;
                }
            }
        }

        nOffset = 5 * nCycMax;

        // Ic
        for(i = 0; i < nCycMax; i++)
        {
            bNone = bmt_is_none((UINT8*)&(psAfn0cF57_f->uHarm[nOffset + i]), sizeof(sMtFmt_sXX_XX));
            if(TRUE == bNone)
            {
                psAfn0cF57->sIc.bfsXX_XX[i] = FALSE;
            }
            else
            {
                psAfn0cF57->sIc.bfsXX_XX[i] = TRUE;
                eRet = emt_trans_sXX_XX(eTrans, 
                                       &(psAfn0cF57->sIc.fsXX_XX[i]), 
                                       (sMtFmt06*)&(psAfn0cF57_f->uHarm[nOffset + i]));
                if(MT_OK != eRet)
                {
                    #ifdef MT_DBG
                    DEBUG("emt_trans_sXX_XX() error = %d", eRet);
                    #endif       
                    return eRet;
                }
            }
        }
    }
    else if(MT_TRANS_U2F == eTrans)
    // 用户侧转为帧侧
    {
        ucN = psAfn0cF57->ucN;
        if(MT_HARM_TIMES_MIN > ucN || MT_HARM_TIMES_MAX < ucN)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0cf57() ucN is out of range ucN = %d", ucN);
            #endif
            return MT_ERR_OUTRNG;
        }
        
        psAfn0cF57_f->ucN = ucN;
        nCycMax           = ucN - 1;
        // ua
        for(i = 0; i < nCycMax; i++)
        {
            bNone = !(psAfn0cF57->sUa.bfXXX_X[i]);
            if(TRUE == bNone)
            {
                vmt_set_none((UINT8*)&(psAfn0cF57_f->uHarm[i]), sizeof(sMtFmt_XXX_X));
            }
            else
            {
                eRet = emt_trans_XXX_X(eTrans, 
                                       &(psAfn0cF57->sUa.fXXX_X[i]), 
                                       (sMtFmt07*)&(psAfn0cF57_f->uHarm[i]));
                if(MT_OK != eRet)
                {
                    #ifdef MT_DBG
                    DEBUG("emt_trans_XXX_X() error = %d", eRet);
                    #endif       
                    return eRet;
                }
            }
        }

        nOffset = nCycMax;
        
        // Ub
        for(i = 0; i < nCycMax; i++)
        {
            bNone = !(psAfn0cF57->sUb.bfXXX_X[i]);
            if(TRUE == bNone)
            {
                vmt_set_none((UINT8*)&(psAfn0cF57_f->uHarm[nOffset + i]), sizeof(sMtFmt_XXX_X));
            }
            else
            {
                eRet = emt_trans_XXX_X(eTrans, 
                                       &(psAfn0cF57->sUb.fXXX_X[i]), 
                                       (sMtFmt07*)&(psAfn0cF57_f->uHarm[nOffset + i]));
                if(MT_OK != eRet)
                {
                    #ifdef MT_DBG
                    DEBUG("emt_trans_XXX_X() error = %d", eRet);
                    #endif       
                    return eRet;
                }
            }
        }
        
        nOffset = 2 * nCycMax;

        // Uc
        for(i = 0; i < nCycMax; i++)
        {
            bNone = !(psAfn0cF57->sUc.bfXXX_X[i]);
            if(TRUE == bNone)
            {
                vmt_set_none((UINT8*)&(psAfn0cF57_f->uHarm[nOffset + i]), sizeof(sMtFmt_XXX_X));
            }
            else
            {
                eRet = emt_trans_XXX_X(eTrans, 
                                       &(psAfn0cF57->sUc.fXXX_X[i]), 
                                       (sMtFmt07*)&(psAfn0cF57_f->uHarm[nOffset + i]));
                if(MT_OK != eRet)
                {
                    #ifdef MT_DBG
                    DEBUG("emt_trans_XXX_X() error = %d", eRet);
                    #endif       
                    return eRet;
                }
            }
        }
        
        nOffset = 3 * nCycMax;

        // Ia
        for(i = 0; i < nCycMax; i++)
        {
            bNone = !(psAfn0cF57->sIa.bfsXX_XX[i]);
            if(TRUE == bNone)
            {
                vmt_set_none((UINT8*)&(psAfn0cF57_f->uHarm[nOffset + i]), sizeof(sMtFmt_sXX_XX));
            }
            else
            {
                eRet = emt_trans_sXX_XX(eTrans, 
                                       &(psAfn0cF57->sIa.fsXX_XX[i]), 
                                       (sMtFmt06*)&(psAfn0cF57_f->uHarm[nOffset + i]));
                if(MT_OK != eRet)
                {
                    #ifdef MT_DBG
                    DEBUG("emt_trans_sXX_XX() error = %d", eRet);
                    #endif       
                    return eRet;
                }
            }
        }

        nOffset = 4 * nCycMax;

        // Ib
        for(i = 0; i < nCycMax; i++)
        {
            bNone = !(psAfn0cF57->sIb.bfsXX_XX[i]);
            if(TRUE == bNone)
            {
                vmt_set_none((UINT8*)&(psAfn0cF57_f->uHarm[nOffset + i]), sizeof(sMtFmt_sXX_XX));
            }
            else
            {
                eRet = emt_trans_sXX_XX(eTrans, 
                                       &(psAfn0cF57->sIb.fsXX_XX[i]), 
                                       (sMtFmt06*)&(psAfn0cF57_f->uHarm[nOffset + i]));
                if(MT_OK != eRet)
                {
                    #ifdef MT_DBG
                    DEBUG("emt_trans_sXX_XX() error = %d", eRet);
                    #endif       
                    return eRet;
                }
            }
        }

        nOffset = 5 * nCycMax;

        // Ic
        for(i = 0; i < nCycMax; i++)
        {
            bNone = !(psAfn0cF57->sIc.bfsXX_XX[i]);
            if(TRUE == bNone)
            {
                vmt_set_none((UINT8*)&(psAfn0cF57_f->uHarm[nOffset + i]), sizeof(sMtFmt_sXX_XX));
            }
            else
            {
                eRet = emt_trans_sXX_XX(eTrans, 
                                       &(psAfn0cF57->sIc.fsXX_XX[i]), 
                                       (sMtFmt06*)&(psAfn0cF57_f->uHarm[nOffset + i]));
                if(MT_OK != eRet)
                {
                    #ifdef MT_DBG
                    DEBUG("emt_trans_sXX_XX() error = %d", eRet);
                    #endif       
                    return eRet;
                }
            }
        }
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf57() para error!");
        #endif
        return MT_ERR_PARA;
    }

    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0cF57_f) + 6 * sizeof(sMtFmt_sXX_XX) * (ucN - 1);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf58
 功能描述  : F58：当前A、B、C三相电压、电流2～N次谐波含有率
 对应命令  : CMD_AFN_C_F58_CURT_HARM_RATE
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月20日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf58(eMtTrans eTrans, void* psUser, void* psFrame, UINT16* pusfLen)
{
    if(!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf58() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    sMtAfn0cF58     *psAfn0cF58   = (sMtAfn0cF58*)psUser;
    sMtAfn0cF58_f   *psAfn0cF58_f = (sMtAfn0cF58_f*)psFrame;
    UINT8            ucN          = 0;
    BOOL             bNone        = FALSE;
    eMtErr           eRet         = MT_OK;
    UINT8            i,j;
    
    // 帧侧转为用户侧
    if(MT_TRANS_F2U == eTrans)
    {
        ucN = psAfn0cF58_f->ucN;
        psAfn0cF58->ucN = ucN;
        j = 0;

        // UA
        bNone = bmt_is_none((UINT8 *)&(psAfn0cF58_f->sXXX_X[j]),sizeof(sMtFmt_sXXX_X));
        if ( bNone == TRUE) 
        {
            psAfn0cF58->sUa.sTotal.bfsXXX_X = FALSE;
        }else
        {
            psAfn0cF58->sUa.sTotal.bfsXXX_X = TRUE;
            eRet  = emt_trans_sXXX_X(eTrans, &(psAfn0cF58->sUa.sTotal.fsXXX_X)
                                      , &(psAfn0cF58_f->sXXX_X[j]));

            if ( MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emt_trans_sXXX_X() error = %d", eRet);
                #endif       
                return eRet;
            }
        }
        j++;

        for(i = 0; i < ucN - 1;i++,j++)
        {
            bNone = bmt_is_none((UINT8 *)&(psAfn0cF58_f->sXXX_X[j]),sizeof(sMtFmt_sXXX_X));
            if ( bNone == TRUE) 
            {
                psAfn0cF58->sUa.bfsXXX_X[i] = FALSE;
            }else
            {
                psAfn0cF58->sUa.bfsXXX_X[i] = TRUE;
                eRet  = emt_trans_sXXX_X(eTrans, &(psAfn0cF58->sUa.fsXXX_X[i])
                                          , &(psAfn0cF58_f->sXXX_X[j]));

                if ( MT_OK != eRet)
                {
                    #ifdef MT_DBG
                    DEBUG("emt_trans_sXXX_X() error = %d", eRet);
                    #endif       
                    return eRet;
                }
            }
        }

        // UB
        bNone = bmt_is_none((UINT8 *)&(psAfn0cF58_f->sXXX_X[j]),sizeof(sMtFmt_sXXX_X));
        if ( bNone == TRUE) 
        {
            psAfn0cF58->sUb.sTotal.bfsXXX_X = FALSE;
        }else
        {
            psAfn0cF58->sUb.sTotal.bfsXXX_X = TRUE;
            eRet  = emt_trans_sXXX_X(eTrans, &(psAfn0cF58->sUb.sTotal.fsXXX_X)
                                      , &(psAfn0cF58_f->sXXX_X[j]));

            if ( MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emt_trans_sXXX_X() error = %d", eRet);
                #endif       
                return eRet;
            }
        }
        j++;

        for(i = 0; i < ucN - 1;i++,j++)
        {
            bNone = bmt_is_none((UINT8 *)&(psAfn0cF58_f->sXXX_X[j]),sizeof(sMtFmt_sXXX_X));
            if ( bNone == TRUE) 
            {
                psAfn0cF58->sUb.bfsXXX_X[i] = FALSE;
            }else
            {
                psAfn0cF58->sUb.bfsXXX_X[i] = TRUE;
                eRet  = emt_trans_sXXX_X(eTrans, &(psAfn0cF58->sUb.fsXXX_X[i])
                                          , &(psAfn0cF58_f->sXXX_X[j]));

                if ( MT_OK != eRet)
                {
                    #ifdef MT_DBG
                    DEBUG("emt_trans_sXXX_X() error = %d", eRet);
                    #endif       
                    return eRet;
                }
            }
        }

        // UC
        bNone = bmt_is_none((UINT8 *)&(psAfn0cF58_f->sXXX_X[j]),sizeof(sMtFmt_sXXX_X));
        if ( bNone == TRUE) 
        {
            psAfn0cF58->sUc.sTotal.bfsXXX_X = FALSE;
        }else
        {
            psAfn0cF58->sUc.sTotal.bfsXXX_X = TRUE;
            eRet  = emt_trans_sXXX_X(eTrans, &(psAfn0cF58->sUc.sTotal.fsXXX_X)
                                      , &(psAfn0cF58_f->sXXX_X[j]));

            if ( MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emt_trans_sXXX_X() error = %d", eRet);
                #endif       
                return eRet;
            }
        }
        j++;

        for(i = 0; i < ucN - 1;i++,j++)
        {
            bNone = bmt_is_none((UINT8 *)&(psAfn0cF58_f->sXXX_X[j]),sizeof(sMtFmt_sXXX_X));
            if ( bNone == TRUE) 
            {
                psAfn0cF58->sUc.bfsXXX_X[i] = FALSE;
            }else
            {
                psAfn0cF58->sUc.bfsXXX_X[i] = TRUE;
                eRet  = emt_trans_sXXX_X(eTrans, &(psAfn0cF58->sUc.fsXXX_X[i])
                                          , &(psAfn0cF58_f->sXXX_X[j]));

                if ( MT_OK != eRet)
                {
                    #ifdef MT_DBG
                    DEBUG("emt_trans_sXXX_X() error = %d", eRet);
                    #endif       
                    return eRet;
                }
            }
        }

        // IA
        bNone = bmt_is_none((UINT8 *)&(psAfn0cF58_f->sXXX_X[j]),sizeof(sMtFmt_sXXX_X));
        if ( bNone == TRUE) 
        {
            psAfn0cF58->sIa.sTotal.bfsXXX_X = FALSE;
        }else
        {
            psAfn0cF58->sIa.sTotal.bfsXXX_X = TRUE;
            eRet  = emt_trans_sXXX_X(eTrans, &(psAfn0cF58->sIa.sTotal.fsXXX_X)
                                      , &(psAfn0cF58_f->sXXX_X[j]));

            if ( MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emt_trans_sXXX_X() error = %d", eRet);
                #endif       
                return eRet;
            }
        }
        j++;

        for(i = 0; i < ucN - 1;i++,j++)
        {
            bNone = bmt_is_none((UINT8 *)&(psAfn0cF58_f->sXXX_X[j]),sizeof(sMtFmt_sXXX_X));
            if ( bNone == TRUE) 
            {
                psAfn0cF58->sIa.bfsXXX_X[i] = FALSE;
            }else
            {
                psAfn0cF58->sIa.bfsXXX_X[i] = TRUE;
                eRet  = emt_trans_sXXX_X(eTrans, &(psAfn0cF58->sIa.fsXXX_X[i])
                                          , &(psAfn0cF58_f->sXXX_X[j]));

                if ( MT_OK != eRet)
                {
                    #ifdef MT_DBG
                    DEBUG("emt_trans_sXXX_X() error = %d", eRet);
                    #endif       
                    return eRet;
                }
            }
        }

        // IB
        bNone = bmt_is_none((UINT8 *)&(psAfn0cF58_f->sXXX_X[j]),sizeof(sMtFmt_sXXX_X));
        if ( bNone == TRUE) 
        {
            psAfn0cF58->sIb.sTotal.bfsXXX_X = FALSE;
        }else
        {
            psAfn0cF58->sIb.sTotal.bfsXXX_X = TRUE;
            eRet  = emt_trans_sXXX_X(eTrans, &(psAfn0cF58->sIb.sTotal.fsXXX_X)
                                      , &(psAfn0cF58_f->sXXX_X[j]));

            if ( MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emt_trans_sXXX_X() error = %d", eRet);
                #endif       
                return eRet;
            }
        }
        j++;

        for(i = 0; i < ucN - 1;i++,j++)
        {
            bNone = bmt_is_none((UINT8 *)&(psAfn0cF58_f->sXXX_X[j]),sizeof(sMtFmt_sXXX_X));
            if ( bNone == TRUE) 
            {
                psAfn0cF58->sIb.bfsXXX_X[i] = FALSE;
            }else
            {
                psAfn0cF58->sIb.bfsXXX_X[i] = TRUE;
                eRet  = emt_trans_sXXX_X(eTrans, &(psAfn0cF58->sIb.fsXXX_X[i])
                                          , &(psAfn0cF58_f->sXXX_X[j]));

                if ( MT_OK != eRet)
                {
                    #ifdef MT_DBG
                    DEBUG("emt_trans_sXXX_X() error = %d", eRet);
                    #endif       
                    return eRet;
                }
            }
        }

        // IC
        bNone = bmt_is_none((UINT8 *)&(psAfn0cF58_f->sXXX_X[j]),sizeof(sMtFmt_sXXX_X));
        if ( bNone == TRUE) 
        {
            psAfn0cF58->sIc.sTotal.bfsXXX_X = FALSE;
        }else
        {
            psAfn0cF58->sIc.sTotal.bfsXXX_X = TRUE;
            eRet  = emt_trans_sXXX_X(eTrans, &(psAfn0cF58->sIc.sTotal.fsXXX_X)
                                      , &(psAfn0cF58_f->sXXX_X[j]));

            if ( MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emt_trans_sXXX_X() error = %d", eRet);
                #endif       
                return eRet;
            }
        }
        j++;
        
        for(i = 0; i < ucN - 1;i++,j++)
        {
            bNone = bmt_is_none((UINT8 *)&(psAfn0cF58_f->sXXX_X[j]),sizeof(sMtFmt_sXXX_X));
            if ( bNone == TRUE) 
            {
                psAfn0cF58->sIc.bfsXXX_X[i] = FALSE;
            }else
            {
                psAfn0cF58->sIc.bfsXXX_X[i] = TRUE;
                eRet  = emt_trans_sXXX_X(eTrans, &(psAfn0cF58->sIc.fsXXX_X[i])
                                          , &(psAfn0cF58_f->sXXX_X[j]));

                if ( MT_OK != eRet)
                {
                    #ifdef MT_DBG
                    DEBUG("emt_trans_sXXX_X() error = %d", eRet);
                    #endif       
                    return eRet;
                }
            }
        }
    }
    else if(MT_TRANS_U2F == eTrans)
    // 用户侧转为帧侧
    {
        ucN = psAfn0cF58->ucN;
        psAfn0cF58_f->ucN = ucN;
        j = 0;

        // UA
        if ( psAfn0cF58->sUa.sTotal.bfsXXX_X == TRUE) 
        {
            eRet = emt_trans_sXXX_X(eTrans, &(psAfn0cF58->sUa.sTotal.fsXXX_X)
                                      , &(psAfn0cF58_f->sXXX_X[j]));
            if ( MT_OK != eRet )
            {
                #ifdef MT_DBG
                DEBUG("emt_trans_sXXX_X() error = %d", eRet);
                #endif       
                return eRet;
            }
        }else
        {
            vmt_set_none((UINT8*)&(psAfn0cF58_f->sXXX_X[j]), sizeof(sMtFmt_sXXX_X));;
        }
        j++;
        
        for( i = 0;i < ucN - 1;i++,j++) 
        {
            if ( psAfn0cF58->sUa.bfsXXX_X[i] == TRUE) 
            {
                eRet = emt_trans_sXXX_X(eTrans, &(psAfn0cF58->sUa.fsXXX_X[i])
                                         , &(psAfn0cF58_f->sXXX_X[j]));
                if ( MT_OK != eRet )
                {
                    #ifdef MT_DBG
                    DEBUG("emt_trans_sXXX_X() error = %d", eRet);
                    #endif       
                    return eRet;
                }
            }else
            {
                vmt_set_none((UINT8*)&(psAfn0cF58_f->sXXX_X[j]), sizeof(sMtFmt_sXXX_X));;
            }
        }

        // UB
        if ( psAfn0cF58->sUb.sTotal.bfsXXX_X == TRUE) 
        {
            eRet = emt_trans_sXXX_X(eTrans, &(psAfn0cF58->sUb.sTotal.fsXXX_X)
                                      , &(psAfn0cF58_f->sXXX_X[j]));

            if ( MT_OK != eRet )
            {
                #ifdef MT_DBG
                DEBUG("emt_trans_sXXX_X() error = %d", eRet);
                #endif       
                return eRet;
            }
        }else
        {
            vmt_set_none((UINT8*)&(psAfn0cF58_f->sXXX_X[j]), sizeof(sMtFmt_sXXX_X));;
        }
        j++;
        
        for( i = 0;i < ucN - 1;i++,j++) 
        {
             if ( psAfn0cF58->sUb.bfsXXX_X[i] == TRUE) 
            {
                eRet = emt_trans_sXXX_X(eTrans, &(psAfn0cF58->sUb.fsXXX_X[i])
                                         , &(psAfn0cF58_f->sXXX_X[j]));
                if ( MT_OK != eRet )
                {
                    #ifdef MT_DBG
                    DEBUG("emt_trans_sXXX_X() error = %d", eRet);
                    #endif       
                    return eRet;
                }
            }else
            {
                vmt_set_none((UINT8*)&(psAfn0cF58_f->sXXX_X[j]), sizeof(sMtFmt_sXXX_X));;
            }
        }

        // UC
        if ( psAfn0cF58->sUc.sTotal.bfsXXX_X == TRUE) 
        {
            eRet = emt_trans_sXXX_X(eTrans, &(psAfn0cF58->sUc.sTotal.fsXXX_X)
                                      , &(psAfn0cF58_f->sXXX_X[j]));
            if ( MT_OK != eRet )
            {
                #ifdef MT_DBG
                DEBUG("emt_trans_sXXX_X() error = %d", eRet);
                #endif       
                return eRet;
            }
        }else
        {
            vmt_set_none((UINT8*)&(psAfn0cF58_f->sXXX_X[j]), sizeof(sMtFmt_sXXX_X));;
        }
        j++;
        
        for( i = 0;i < ucN - 1;i++,j++ ) 
        {
             if ( psAfn0cF58->sUc.bfsXXX_X[i] == TRUE) 
            {
                eRet = emt_trans_sXXX_X(eTrans, &(psAfn0cF58->sUc.fsXXX_X[i])
                                         , &(psAfn0cF58_f->sXXX_X[j]));
                if ( MT_OK != eRet )
                {
                    #ifdef MT_DBG
                    DEBUG("emt_trans_sXXX_X() error = %d", eRet);
                    #endif       
                    return eRet;
                }
            }else
            {
                vmt_set_none((UINT8*)&(psAfn0cF58_f->sXXX_X[j]), sizeof(sMtFmt_sXXX_X));;
            }
        }

        // IA
        if ( psAfn0cF58->sIa.sTotal.bfsXXX_X == TRUE) 
        {
            eRet = emt_trans_sXXX_X(eTrans, &(psAfn0cF58->sIa.sTotal.fsXXX_X)
                                      , &(psAfn0cF58_f->sXXX_X[j]));

            if ( MT_OK != eRet )
            {
                #ifdef MT_DBG
                DEBUG("emt_trans_sXXX_X() error = %d", eRet);
                #endif       
                return eRet;
            }
        }else
        {
            vmt_set_none((UINT8*)&(psAfn0cF58_f->sXXX_X[j]), sizeof(sMtFmt_sXXX_X));;
        }
        j++;
        
        for( i = 0;i < ucN - 1;i++,j++ ) 
        {
            if ( psAfn0cF58->sIa.bfsXXX_X[i] == TRUE) 
            {
                eRet = emt_trans_sXXX_X(eTrans, &(psAfn0cF58->sIa.fsXXX_X[i])
                                         , &(psAfn0cF58_f->sXXX_X[j]));

                if ( MT_OK != eRet )
                {
                    #ifdef MT_DBG
                    DEBUG("emt_trans_sXXX_X() error = %d", eRet);
                    #endif       
                    return eRet;
                }
            }else
            {
                vmt_set_none((UINT8*)&(psAfn0cF58_f->sXXX_X[j]), sizeof(sMtFmt_sXXX_X));;
            }
        }

        // IB
        if ( psAfn0cF58->sIb.sTotal.bfsXXX_X == TRUE) 
        {
            eRet = emt_trans_sXXX_X(eTrans, &(psAfn0cF58->sIb.sTotal.fsXXX_X)
                                      , &(psAfn0cF58_f->sXXX_X[j]));

            if ( MT_OK != eRet )
            {
                #ifdef MT_DBG
                DEBUG("emt_trans_sXXX_X() error = %d", eRet);
                #endif       
                return eRet;
            }
        }else
        {
            vmt_set_none((UINT8*)&(psAfn0cF58_f->sXXX_X[j]), sizeof(sMtFmt_sXXX_X));;
        }
        j++;
        
        for( i = 0;i < ucN - 1;i++,j++ ) 
        {
            if ( psAfn0cF58->sIb.bfsXXX_X[i] == TRUE) 
            {
                eRet = emt_trans_sXXX_X(eTrans, &(psAfn0cF58->sIb.fsXXX_X[i])
                                         , &(psAfn0cF58_f->sXXX_X[j]));

                if ( MT_OK != eRet )
                {
                    #ifdef MT_DBG
                    DEBUG("emt_trans_sXXX_X() error = %d", eRet);
                    #endif       
                    return eRet;
                }
            }else
            {
                vmt_set_none((UINT8*)&(psAfn0cF58_f->sXXX_X[j]), sizeof(sMtFmt_sXXX_X));;
            }
        }

        // IC
        if ( psAfn0cF58->sIc.sTotal.bfsXXX_X == TRUE) 
        {
            eRet = emt_trans_sXXX_X(eTrans, &(psAfn0cF58->sIc.sTotal.fsXXX_X)
                                      , &(psAfn0cF58_f->sXXX_X[j]));

            if ( MT_OK != eRet )
            {
                #ifdef MT_DBG
                DEBUG("emt_trans_sXXX_X() error = %d", eRet);
                #endif       
                return eRet;
            }
        }else
        {
            vmt_set_none((UINT8*)&(psAfn0cF58_f->sXXX_X[j]), sizeof(sMtFmt_sXXX_X));;
        }
        j++;
        
        for( i = 0;i < ucN - 1;i++,j++ ) 
        {
            if ( psAfn0cF58->sIc.bfsXXX_X[i] == TRUE) 
            {
                eRet = emt_trans_sXXX_X(eTrans, &(psAfn0cF58->sIc.fsXXX_X[i])
                                         , &(psAfn0cF58_f->sXXX_X[j]));

                if ( MT_OK != eRet )
                {
                    #ifdef MT_DBG
                    DEBUG("emt_trans_sXXX_X() error = %d", eRet);
                    #endif       
                    return eRet;
                }
            }else
            {
                vmt_set_none((UINT8*)&(psAfn0cF58_f->sXXX_X[j]), sizeof(sMtFmt_sXXX_X));;
            }
        }
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf58() para error!");
        #endif
        return MT_ERR_PARA;
    }

    // 计算在帧侧的字节长度
    *pusfLen =sizeof(sMtAfn0cF58_f) + 6 * sizeof(sMtFmt_sXX_XX) * ucN;
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf65
 功能描述  : F65：当前电容器投切状态
             CMD_AFN_C_F65_CURT_CAPA_SWITCH
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月28日 星期三
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf65(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if (!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf65() pointer is null");
        #endif
        return MT_ERR_NULL;
    }
    
    if (MT_TRANS_F2U == eTrans) // 帧侧到用户侧
    {
        // 暂时不做处理，只计算数据长度
    }
    else if ( MT_TRANS_U2F == eTrans)  // 用户侧到帧侧
    {
        // 暂时不做处理，只计算数据长度
    }
    else  
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf65() para error!");
        #endif
        return MT_ERR_PARA;
    }
    
    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0cf65_f);
     return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf66
 功能描述  : F66：当前电容器累计补偿投入时间和次数
             CMD_AFN_C_F66_CURT_CAPA_TIME
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月28日 星期三
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf66(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if(!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf66() pointer is null");
        #endif
        return MT_ERR_NULL;
    }
    
    if(MT_TRANS_F2U == eTrans) // 帧侧到用户侧
    {
        // 暂时不做处理，只计算数据长度
    }
    else if( MT_TRANS_U2F == eTrans)  // 用户侧到帧侧
    {
        // 暂时不做处理，只计算数据长度
    }
    else  
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf66() para error!");
        #endif
        return MT_ERR_PARA;
    }
    
    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0cf66_f);
     return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf67
 功能描述  : F67：当日、当月电容器累计补偿的无功电能量
             CMD_AFN_C_F67_CURT_CAPA_POWR
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月28日 星期三
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf67(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if (!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf67() pointer is null");
        #endif
        return MT_ERR_NULL;
    }
    
    if (MT_TRANS_F2U == eTrans) // 帧侧到用户侧
    {
        // 暂时不做处理，只计算数据长度
    }
    else if ( MT_TRANS_U2F == eTrans)  // 用户侧到帧侧
    {
        // 暂时不做处理，只计算数据长度
    }
    else  
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf67() para error!");
        #endif
        return MT_ERR_PARA;
    }
    
    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0cf67_f);
     return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf73
 功能描述  : F73：直流模拟量当前数据
             CMD_AFN_C_F73_DC_RLTM_DATA
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月28日 星期三
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf73(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if (!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf73() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    eMtErr mRet = MT_OK;
    mRet = emt_trans_sXXX(eTrans, (float *)psUser, (sMtFmt02_f *)psFrame);
    
    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0cf67_f);
     return mRet;
}

/*****************************************************************************
 函 数 名  : emtGetFrzFreq
 功能描述  : 获取冻结数据频率
 输入参数  : UINT8 ucFrzFreq
 输出参数  : 无
 返 回 值  :    返回冻结数据频率，出错返回-1
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月28日 星期三
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
INT8 emtGetFrzFreq(UINT8 ucFrzFreq)
{
    switch(ucFrzFreq)
    {
        case MT_FRZ_NONE:
            return 0;           // 不冻结
            
        case MT_FRZ_15:         // 15分钟冻结一次
            return (60 / 15);   
            
        case MT_FRZ_30:    
            return (60 / 30);   // 30分钟冻结一次
            
        case MT_FRZ_60:
            return (60 / 60);   // 60分钟冻结一次
            
        case MT_FRZ_5:
            return (60 / 5);    // 5分钟冻结一次
            
        case MT_FRZ_1:
            return (60 / 1);    // 1分钟冻结一次
            
        //case MT_FRZ_UNKNOWN:
        default:
            return -1;
    }
    
    return -1;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf81
 功能描述  : F81：小时冻结总加有功功率
             CMD_AFN_C_F81_HFRZ_GRUP_RATE_HAVE
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月28日 星期三
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf81(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if (!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf81() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    sMtAfn0cf81_f *psMtAfn0cf81_f  = (sMtAfn0cf81_f *)psFrame;
    INT8           n;
    if (MT_TRANS_F2U == eTrans) // 帧侧到用户侧
    {
        // 暂时不做处理，只计算数据长度
        n = emtGetFrzFreq(psMtAfn0cf81_f->sTimeStamp.ucFrzFreq);
        if (  n < 0)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0cf81() timestamp error!");
            #endif
            return MT_ERR_LOGIC;
        }
    }
    else if ( MT_TRANS_U2F == eTrans)  // 用户侧到帧侧
    {
        // 暂时不做处理，只计算数据长度
    }
    else  
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf81() para error!");
        #endif
        return MT_ERR_PARA;
    }
    
    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0cf81_f) + n * sizeof(sMtFmt02_f);
     return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf82
 功能描述  : F82：小时冻结总加无功功率
             CMD_AFN_C_F82_HFRZ_GRUP_RATE_NONE
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月28日 星期三
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf82(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    return emtTrans_afn0cf81(eTrans, psUser, psFrame,pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf83
 功能描述  : F83：小时冻结总加有功总电能量
             CMD_AFN_C_F83_HFRZ_GRUP_POWR_HAVE
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月28日 星期三
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf83(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if (!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf83() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    sMtAfn0cf83_f *psMtAfn0cf83_f  = (sMtAfn0cf83_f *)psFrame;
    INT8    n;
    if (MT_TRANS_F2U == eTrans) // 帧侧到用户侧
    {
        // 暂时不做处理，只计算数据长度
        n = emtGetFrzFreq(psMtAfn0cf83_f->sTimeStamp.ucFrzFreq);
        if ( n < 0)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0cf83() timestamp error!");
            #endif
            return MT_ERR_LOGIC;    
        }
    }
    else if ( MT_TRANS_U2F == eTrans)  // 用户侧到帧侧
    {
        // 暂时不做处理，只计算数据长度
    }
    else  
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf83() para error!");
        #endif
        return MT_ERR_PARA;
    }
    
    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0cf83_f) + n * sizeof(sMtFmt03_f);
     return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf84
 功能描述  : F84：小时冻结总加无功总电能量
             CMD_AFN_C_F84_HFRZ_GRUP_POWR_NONE
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月28日 星期三
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf84(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
     return emtTrans_afn0cf83(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf89
 功能描述  : F89：小时冻结有功功率
             CMD_AFN_C_F89_HFRZ_RATE_HAVE
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月28日 星期三
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf89(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if (!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf89() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    sMtAfn0cf89_f *psMtAfn0cf89_f  = (sMtAfn0cf89_f *)psFrame;
    INT8    n;
    if (MT_TRANS_F2U == eTrans) // 帧侧到用户侧
    {
        // 暂时不做处理，只计算数据长度
        n = emtGetFrzFreq(psMtAfn0cf89_f->sTimeStamp.ucFrzFreq);
        if ( n < 0 )
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0cf89() timestamp error!");
            #endif
            return MT_ERR_LOGIC;  
        }
    }
    else if ( MT_TRANS_U2F == eTrans)  // 用户侧到帧侧
    {
        // 暂时不做处理，只计算数据长度
    }
    else  
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf89() para error!");
        #endif
        return MT_ERR_PARA;
    }
    
    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0cf89_f) + n * sizeof(sMtFmt09);
    
     return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf90
 功能描述  : F90：小时冻结A相有功功率
             CMD_AFN_C_F90_HFRZ_RATE_HAVE_A
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月28日 星期三
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf90(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
     return emtTrans_afn0cf89(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf91
 功能描述  : F91：小时冻结B相有功功率
             CMD_AFN_C_F90_HFRZ_RATE_HAVE_B
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月28日 星期三
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf91(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
     return emtTrans_afn0cf89(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf92
 功能描述  : F92：小时冻结C 相有功功率
             CMD_AFN_C_F90_HFRZ_RATE_HAVE_C
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月28日 星期三
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf92(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
     return emtTrans_afn0cf89(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf93
 功能描述  : F93：小时冻结无功功率
             CMD_AFN_C_F93_HFRZ_RATE_NONE
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月28日 星期三
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf93(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
     return emtTrans_afn0cf89(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf94
 功能描述  : F94：小时冻结A  相无功功率
             CMD_AFN_C_F94_HFRZ_RATE_NONE_A
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月28日 星期三
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf94(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
     return emtTrans_afn0cf89(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf95
 功能描述  : F95：小时冻结B  相无功功率
             CMD_AFN_C_F95_HFRZ_RATE_NONE_B
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月28日 星期三
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf95(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
     return emtTrans_afn0cf89(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf96
 功能描述  : F96：小时冻结C  相无功功率
             CMD_AFN_C_F96_HFRZ_RATE_NONE_C
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月28日 星期三
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf96(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
     return emtTrans_afn0cf89(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf97
 功能描述  : F97：小时冻结A 相电压
             CMD_AFN_C_F97_HFRZ_VOLT_A
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf97(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if (!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf97() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    sMtAfn0cf97_f *psMtAfn0cf97_f  = (sMtAfn0cf97_f *)psFrame;
    INT8    n;
    if (MT_TRANS_F2U == eTrans) // 帧侧到用户侧
    {
        // 暂时不做处理，只计算数据长度
        n = emtGetFrzFreq(psMtAfn0cf97_f->sTimeStamp.ucFrzFreq);
        if ( n < 0 ) 
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0cf97() timestamp error!");
            #endif
            return MT_ERR_LOGIC;
        }
    }
    else if ( MT_TRANS_U2F == eTrans)  // 用户侧到帧侧
    {
        // 暂时不做处理，只计算数据长度
    }
    else  
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf97() para error!");
        #endif
        return MT_ERR_PARA;
    }
    
    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0cf97_f) + n * sizeof(sMtFmt07);
    
     return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf98
 功能描述  : F98：小时冻结B  相电压
             CMD_AFN_C_F98_HFRZ_VOLT_B
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf98(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
     return emtTrans_afn0cf97(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf99
 功能描述  : F99：小时冻结C  相电压
             CMD_AFN_C_F99_HFRZ_VOLT_C
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf99(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
     return emtTrans_afn0cf97(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf100
 功能描述  : F100：小时冻结有功功率
             CMD_AFN_C_F100_HFRZ_ELEC_A
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf100(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if (!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf100() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    sMtAfn0cf100_f *psMtAfn0cf100_f  = (sMtAfn0cf100_f *)psFrame;
    INT8    n;
    if (MT_TRANS_F2U == eTrans) // 帧侧到用户侧
    {
        // 暂时不做处理，只计算数据长度
        n = emtGetFrzFreq(psMtAfn0cf100_f->sTimeStamp.ucFrzFreq);
        if ( n  < 0 )
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0cf100() timestamp error!");
            #endif
            return MT_ERR_LOGIC;
        }
    }
    else if ( MT_TRANS_U2F == eTrans)  // 用户侧到帧侧
    {
        // 暂时不做处理，只计算数据长度
    }
    else  
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf100() para error!");
        #endif
        return MT_ERR_PARA;
    }
    
    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0cf100_f) + n * sizeof(sMtFmt25);
    
     return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf101
 功能描述  : F101：小时冻结B  相电流
             CMD_AFN_C_F101_HFRZ_ELEC_B
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf101(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
     return emtTrans_afn0cf100(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf102
 功能描述  : F102：小时冻结C  相电流
             CMD_AFN_C_F102_HFRZ_ELEC_C
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf102(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
     return emtTrans_afn0cf100(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf103
 功能描述  : F103：小时冻结零序电流
             CMD_AFN_C_F103_HFRZ_ELEC_ZERO
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf103(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
     return emtTrans_afn0cf100(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf105
 功能描述  : F105：小时冻结正向有功总电能量
             CMD_AFN_C_F105_HFRZ_FRTH_HAVE
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf105(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if (!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf105() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    sMtAfn0cf105_f *psMtAfn0cf105_f  = (sMtAfn0cf105_f *)psFrame;
    INT8    n;
    if (MT_TRANS_F2U == eTrans) // 帧侧到用户侧
    {
        // 暂时不做处理，只计算数据长度
        n = emtGetFrzFreq(psMtAfn0cf105_f->sTimeStamp.ucFrzFreq);
        if ( n < 0 )
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0cf105() timestamp error!");
            #endif
            return MT_ERR_PARA;
        }
    }
    else if ( MT_TRANS_U2F == eTrans)  // 用户侧到帧侧
    {
        // 暂时不做处理，只计算数据长度
    }
    else  
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf105() para error!");
        #endif
        return MT_ERR_PARA;
    }
    
    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0cf105_f) + n * sizeof(sMtFmt13);
    
     return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf106
 功能描述  : F106：小时冻结正向无功总电能量
                          CMD_AFN_C_F106_HFRZ_FRTH_NONE
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf106(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
     return emtTrans_afn0cf105(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf107
 功能描述  : F107：小时冻结反向有功总电能量
              CMD_AFN_C_F107_HFRZ_BACK_HAVE
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf107(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
     return emtTrans_afn0cf105(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf108
 功能描述  : F107：小时冻结反向无功总电能量
                          CMD_AFN_C_F108_HFRZ_BACK_NONE
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf108(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
     return emtTrans_afn0cf105(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf109
 功能描述  : F109：小时冻结正向有功总电能示值
              CMD_AFN_C_F109_HFRZ_FRTH_HAVE_S
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf109(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if (!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf109() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    sMtAfn0cf109_f *psMtAfn0cf109_f  = (sMtAfn0cf109_f *)psFrame;
    INT8    n;
    if (MT_TRANS_F2U == eTrans) // 帧侧到用户侧
    {
        // 暂时不做处理，只计算数据长度
        n = emtGetFrzFreq(psMtAfn0cf109_f->sTimeStamp.ucFrzFreq);
        if ( n < 0 )
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0cf109() timestamp error!");
            #endif
            return MT_ERR_LOGIC;
        }
    }
    else if ( MT_TRANS_U2F == eTrans)  // 用户侧到帧侧
    {
        // 暂时不做处理，只计算数据长度
    }
    else  
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf109() para error!");
        #endif
        return MT_ERR_PARA;
    }
    
    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0cf109_f) + n * sizeof(sMtFmt11);
    
     return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf110
 功能描述  : F110：小时冻结正向无功总电能示值
                          CMD_AFN_C_F110_HFRZ_FRTH_NONE_S
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf110(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
     return emtTrans_afn0cf109(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf111
 功能描述  : F111：小时冻结反向有功总电能示值
              CMD_AFN_C_F111_HFRZ_BACK_HAVE_S
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf111(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
     return emtTrans_afn0cf109(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf112
 功能描述  : F112：小时冻结反向无功总电能示值
                          CMD_AFN_C_F112_HFRZ_BACK_NONE_S
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf112(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
     return emtTrans_afn0cf109(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf113
 功能描述  : F109：小时冻结总功率因数
             CMD_AFN_C_F113_HFRZ_FACT_TOTAL
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf113(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if (!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf113() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    sMtAfn0cf113_f *psMtAfn0cf113_f  = (sMtAfn0cf113_f *)psFrame;
    INT8    n;
    if (MT_TRANS_F2U == eTrans) // 帧侧到用户侧
    {
        // 暂时不做处理，只计算数据长度
        n = emtGetFrzFreq(psMtAfn0cf113_f->sTimeStamp.ucFrzFreq);
        if ( n < 0 )
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0cf113() timestamp error!");
            #endif
            return MT_ERR_LOGIC;
        }
    }
    else if ( MT_TRANS_U2F == eTrans)  // 用户侧到帧侧
    {
        // 暂时不做处理，只计算数据长度
    }
    else  
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf113() para error!");
        #endif
        return MT_ERR_PARA;
    }
    
    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0cf113_f) + n * sizeof(sMtFmt05);
    
     return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf114
 功能描述  : F114：小时冻结A相功率因数
                          CMD_AFN_C_F114_HFRZ_FACT_A
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf114(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
     return emtTrans_afn0cf113(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf115
 功能描述  : F115：小时冻结B相功率因数
             CMD_AFN_C_F115_HFRZ_FACT_B
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf115(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
     return emtTrans_afn0cf113(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf116
 功能描述  : F116：小时冻结C相功率因数
                          CMD_AFN_C_F116_HFRZ_FACT_C
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf116(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
     return emtTrans_afn0cf113(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf121
 功能描述  : F121：小时冻结直流模拟量
             CMD_AFN_C_F121_HFRZ_DC_VALUE
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf121(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    return emtTrans_afn0cf81(eTrans, psUser, psFrame,pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf129
 功能描述  : F109：当前正向有功电能示值（总、费率1～M）
                          CMD_AFN_C_F129_FRTH_HAVE_POWR_C
 输入参数  : eMtTrans eTrans  
                           void* psUser     
                           void* psFrame    
                           UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf129(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if (!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf129() pointer is null");
        #endif
        return MT_ERR_NULL;
    }
    
    sMtAfn0cf129_f *psMtAfn0cf129_f  = (sMtAfn0cf129_f *)psFrame;
    sMtAfn0cf129   *psMtAfn0cf129    = (sMtAfn0cf129 *)psUser;
    eMtErr eRet = MT_OK;
    UINT8 i,ucM; 
    
    eRet = emt_trans_YYMMDDhhmm(eTrans,&(psMtAfn0cf129->sTime) ,&(psMtAfn0cf129_f->sTime));
    if ( MT_OK != eRet )
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf129() emt_trans_YYMMDDhhmm is %d\n",eRet);
        #endif
        return eRet;
    }
    
    eRet = emt_trans_XXXXXX_XXXX(eTrans,&(psMtAfn0cf129->dTotalValue), &(psMtAfn0cf129_f->sTotalValue));
    if ( MT_OK != eRet )
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf129() emt_trans_XXXXXX_XXXX is %d\n",eRet);
        #endif
        return eRet;
    }

    if (MT_TRANS_F2U == eTrans) // 帧侧到用户侧
    {
        psMtAfn0cf129->ucM = psMtAfn0cf129_f->ucM;
        ucM = psMtAfn0cf129_f->ucM;
    }else if ( MT_TRANS_U2F == eTrans)  // 用户侧到侦测
    {
        psMtAfn0cf129_f->ucM = psMtAfn0cf129->ucM;
        ucM = psMtAfn0cf129->ucM;
    }
    else  
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf129() para error!");
        #endif
        return MT_ERR_PARA;
    }

    if (ucM < 1)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf129() ucM Out of range!");
        #endif
        return MT_ERR_OUTRNG;
    }

    for (i = 0;i < ucM; i++)
    {
        eRet = emt_trans_XXXXXX_XXXX(eTrans,&(psMtAfn0cf129->dValueItem[i]), &(psMtAfn0cf129_f->sValueItem[i]));
        if ( MT_OK != eRet )
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0cf129() emt_trans_XXXXXX_XXXX is %d\n",eRet);
            #endif
            return eRet;
        }
    }

    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0cf129_f) + (ucM - 1)* sizeof(sMtFmt14_f);
    
     return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf130
 功能描述  : F130：当前正向无功（组合无功1）电能示值（总、费率1～M）
                          CMD_AFN_C_F130_FRTH_NONE_POWR_C
 输入参数  : eMtTrans eTrans  
                           void* psUser     
                           void* psFrame    
                           UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf130(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
   if (!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf130() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    sMtAfn0cf130_f *psMtAfn0cf130_f  = (sMtAfn0cf130_f *)psFrame;
    sMtAfn0cf130   *psMatAfn0cf130     = (sMtAfn0cf130 *)psUser;
    eMtErr eRet = MT_OK;
    UINT8 i,ucM;
    

    eRet = emt_trans_YYMMDDhhmm(eTrans,&(psMatAfn0cf130->sTime) ,&(psMtAfn0cf130_f->sTime));
    if( MT_OK != eRet )
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf130() emt_trans_YYMMDDhhmm is %d",eRet);
        #endif
        return eRet;
    }
    
    eRet = emt_trans_XXXXXX_XX(eTrans,&(psMatAfn0cf130->dTotalValue), &(psMtAfn0cf130_f->sTotalValue));
    if( MT_OK != eRet )
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf130() emt_trans_XXXXXX_XX is %d",eRet);
        #endif
        return eRet;
    }
    
    if (MT_TRANS_F2U == eTrans) // 帧侧到用户侧
    {
        ucM = psMtAfn0cf130_f->ucM;
        psMatAfn0cf130->ucM = ucM;
    }else if ( MT_TRANS_U2F == eTrans)  // 用户侧到侦测
    {
        ucM = psMatAfn0cf130->ucM;
        psMtAfn0cf130_f->ucM = ucM;
    }
    else  
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf130() para error!");
        #endif
        return MT_ERR_PARA;
    }

    if (ucM < 1)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf129() ucM Out of range!");
        #endif
        return MT_ERR_OUTRNG;
    }

    
    for (i = 0;i < ucM; i++)
    {
        eRet = emt_trans_XXXXXX_XX(eTrans,&(psMatAfn0cf130->dValueItem[i]), &(psMtAfn0cf130_f->sValueItem[i]));
        if( MT_OK != eRet )
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0cf130() emt_trans_XXXXXX_XX is %d",eRet);
            #endif
            return eRet;
        }
    }
    
    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0cf130_f) + (ucM - 1)* sizeof(sMtFmt11_f);
    
     return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf131
 功能描述  : F131：当前反向有功电能示值（总、费率1～M）
                          CMD_AFN_C_F131_BACK_HAVE_POWR_C
 输入参数  : eMtTrans eTrans  
                           void* psUser     
                           void* psFrame    
                           UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf131(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{   
     return emtTrans_afn0cf129(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf132
 功能描述  : F132：当前反向无功（组合无功1）电能示值（总、费率1～M）
                          CMD_AFN_C_F132_BACK_NONE_POWR_C
 输入参数  : eMtTrans eTrans  
                           void* psUser     
                           void* psFrame    
                           UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf132(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{   
     return emtTrans_afn0cf130(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf133
 功能描述  : F133：当前一象限无功电能示值（总、费率1～M）
                          CMD_AFN_C_F133_NONE_POWR_P1_C
 输入参数  : eMtTrans eTrans  
                           void* psUser     
                           void* psFrame    
                           UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf133(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{   
     return emtTrans_afn0cf130(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf134
 功能描述  : F134：当前二象限无功电能示值（总、费率1～M）
                          CMD_AFN_C_F134_NONE_POWR_P2_C
 输入参数  : eMtTrans eTrans  
                           void* psUser     
                           void* psFrame    
                           UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf134(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{   
     return emtTrans_afn0cf130(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf135
 功能描述  : F135：当前三象限无功电能示值（总、费率1～M）
                          CMD_AFN_C_F135_NONE_POWR_P3_C
 输入参数  : eMtTrans eTrans  
                           void* psUser     
                           void* psFrame    
                           UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf135(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{   
     return emtTrans_afn0cf130(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf136
 功能描述  : F136：当前四象限无功电能示值（总、费率1～M）
                          CMD_AFN_C_F136_NONE_POWR_P4_C
 输入参数  : eMtTrans eTrans  
                           void* psUser     
                           void* psFrame    
                           UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf136(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{   
     return emtTrans_afn0cf130(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf137
 功能描述  : F137：上月（上一结算日）正向有功电能示值（总、费率1～M）
                          CMD_AFN_C_F137_FRTH_HAVE_POWR_L
 输入参数  : eMtTrans eTrans  
                           void* psUser     
                           void* psFrame    
                           UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf137(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{   
     return emtTrans_afn0cf129(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf138
 功能描述  : F138：上月（上一结算日）正向无功（组合无功1）电能示值（总、费率1～M）
             CMD_AFN_C_F138_FRTH_NONE_POWR_L
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf138(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{   
     return emtTrans_afn0cf130(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf139
 功能描述  : F139：上月（上一结算日）反向有功电能示值（总、费率1～M）
                          CMD_AFN_C_F139_BACK_HAVE_POWR_L
 输入参数  : eMtTrans eTrans  
                           void* psUser     
                           void* psFrame    
                           UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf139(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{   
     return emtTrans_afn0cf129(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf140
 功能描述  : F140：上月（上一结算日）反向无功（组合无功1）电能示值（总、费率1～M）
                          CMD_AFN_C_F140_BACK_NONE_POWR_L
 输入参数  : eMtTrans eTrans  
                           void* psUser     
                           void* psFrame    
                           UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf140(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{   
     return emtTrans_afn0cf130(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf141
 功能描述  : F141：上月（上一结算日）一象限无功电能示值（总、费率1～M）
                          CMD_AFN_C_F141_NONE_POWR_P1_L
 输入参数  : eMtTrans eTrans  
                           void* psUser     
                           void* psFrame    
                           UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf141(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{   
     return emtTrans_afn0cf130(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf142
 功能描述  : F142：上月（上一结算日）二象限无功电能示值（总、费率1～M）
                          CMD_AFN_C_F142_NONE_POWR_P2_L
 输入参数  : eMtTrans eTrans  
                           void* psUser     
                           void* psFrame    
                           UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf142(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{   
     return emtTrans_afn0cf130(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf143
 功能描述  : F143： 上月（上一结算日）三象限无功电能示值（总、费率1～M）
                          CMD_AFN_C_F143_NONE_POWR_P3_L
 输入参数  : eMtTrans eTrans  
                           void* psUser     
                           void* psFrame    
                           UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf143(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{   
     return emtTrans_afn0cf130(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf144
 功能描述  : F144： 上月（上一结算日）四象限无功电能示值（总、费率1～M）
                          CMD_AFN_C_F144_NONE_POWR_P4_L
 输入参数  : eMtTrans eTrans  
                           void* psUser     
                           void* psFrame    
                           UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf144(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{   
     return emtTrans_afn0cf130(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf145
 功能描述  : F145：当月正向有功最大需量及发生时间（总、费率1～M）
                          CMD_AFN_C_F145_FRTH_HAVE_DMND_C
 输入参数  : eMtTrans eTrans  
                           void* psUser     
                           void* psFrame    
                           UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf145(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
    if (!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf145() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    sMtAfn0cf145_f *psMtAfn0cf145_f  = (sMtAfn0cf145_f *)psFrame;
    if (MT_TRANS_F2U == eTrans) // 帧侧到用户侧
    {
        // 暂时不做处理，只计算数据长度
    }else if ( MT_TRANS_U2F == eTrans)  // 用户侧到侦测
    {
        // 暂时不做处理，只计算数据长度
    }
    else  
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf145() para error!");
        #endif
        return MT_ERR_PARA;
    }
    
    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0cf145_f) + (psMtAfn0cf145_f->ucRateM + 1) * sizeof(sMtDmndClock);
    
     return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf146
 功能描述  : F146： 当月正向无功最大需量及发生时间（总、费率1～M）
                          CMD_AFN_C_F146_FRTH_NONE_DMND_C
 输入参数  : eMtTrans eTrans  
                           void* psUser     
                           void* psFrame    
                           UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf146(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{   
     return emtTrans_afn0cf145(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf147
 功能描述  : F147： 当月反向有功最大需量及发生时间（总、费率1～M）
                          CMD_AFN_C_F147_BACK_HAVE_DMND_C
 输入参数  : eMtTrans eTrans  
                           void* psUser     
                           void* psFrame    
                           UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf147(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{   
     return emtTrans_afn0cf145(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf148
 功能描述  : F148： 当月反向无功最大需量及发生时间（总、费率1～M）
                          CMD_AFN_C_F148_BACK_NONE_DMND_C
 输入参数  : eMtTrans eTrans  
                           void* psUser     
                           void* psFrame    
                           UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf148(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{   
     return emtTrans_afn0cf145(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf149
 功能描述  : F149： 上月（上一结算日）正向有功最大需量及发生时间（总、费率1～M）
                          CMD_AFN_C_F149_FRTH_HAVE_DMND_L
 输入参数  : eMtTrans eTrans  
                           void* psUser     
                           void* psFrame    
                           UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf149(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{   
     return emtTrans_afn0cf145(eTrans, psUser, psFrame, pusfLen);
}


/*****************************************************************************
 函 数 名  : emtTrans_afn0cf150
 功能描述  : F150： 上月（上一结算日）正向无功最大需量及发生时间（总、费率1～M）
                          CMD_AFN_C_F150_FRTH_NONE_DMND_L
 输入参数  : eMtTrans eTrans  
                           void* psUser     
                           void* psFrame    
                           UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf150(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{   
     return emtTrans_afn0cf145(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf151
 功能描述  : F151： 上月（上一结算日）反向有功最大需量及发生时间（总、费率1～M）
                          CMD_AFN_C_F151_BACK_HAVE_DMND_L
 输入参数  : eMtTrans eTrans  
                           void* psUser     
                           void* psFrame    
                           UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf151(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{   
     return emtTrans_afn0cf145(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf152
 功能描述  : F152： 上月（上一结算日）反向无功最大需量及发生时间（总、费率1～M）
                          CMD_AFN_C_F152_BACK_NONE_DMND_L
 输入参数  : eMtTrans eTrans  
                           void* psUser     
                           void* psFrame    
                           UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf152(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{   
     return emtTrans_afn0cf145(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf153
 功能描述  : F153： 第一时区冻结正向有功电能示值（总、费率1～M）
                          CMD_AFN_C_F153_FREZ_ZONE_1
 输入参数  : eMtTrans eTrans  
                           void* psUser     
                           void* psFrame    
                           UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf153(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{   
     return emtTrans_afn0cf129(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf154
 功能描述  : F154： 第二时区冻结正向有功电能示值（总、费率1～M）
                          CMD_AFN_C_F154_FREZ_ZONE_2
 输入参数  : eMtTrans eTrans  
                           void* psUser     
                           void* psFrame    
                           UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf154(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{   
     return emtTrans_afn0cf129(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf155
 功能描述  : F155： 第三时区冻结正向有功电能示值（总、费率1～M）
                          CMD_AFN_C_F155_FREZ_ZONE_3
 输入参数  : eMtTrans eTrans  
                           void* psUser     
                           void* psFrame    
                           UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf155(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{   
     return emtTrans_afn0cf129(eTrans, psUser, psFrame, pusfLen);
}


/*****************************************************************************
 函 数 名  : emtTrans_afn0cf156
 功能描述  : F156： 第四时区冻结正向有功电能示值（总、费率1～M）
                          CMD_AFN_C_F156_FREZ_ZONE_4
 输入参数  : eMtTrans eTrans  
                           void* psUser     
                           void* psFrame    
                           UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf156(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{   
     return emtTrans_afn0cf129(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf157
 功能描述  : F157：第五时区冻结正向有功电能示值（总、费率1～M）
                          CMD_AFN_C_F157_FREZ_ZONE_5
 输入参数  : eMtTrans eTrans  
                           void* psUser     
                           void* psFrame    
                           UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf157(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{   
     return emtTrans_afn0cf129(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf158
 功能描述  : F158：第六时区冻结正向有功电能示值（总、费率1～M）
                          CMD_AFN_C_F158_FREZ_ZONE_6
 输入参数  : eMtTrans eTrans  
                           void* psUser     
                           void* psFrame    
                           UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf158(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{   
     return emtTrans_afn0cf129(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf159
 功能描述  : F159：第七时区冻结正向有功电能示值（总、费率1～M）
                          CMD_AFN_C_F159_FREZ_ZONE_7
 输入参数  : eMtTrans eTrans  
                           void* psUser     
                           void* psFrame    
                           UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf159(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{   
     return emtTrans_afn0cf129(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf160
 功能描述  : F160：第八时区冻结正向有功电能示值（总、费率1～M）
                          CMD_AFN_C_F160_FREZ_ZONE_8
 输入参数  : eMtTrans eTrans  
                           void* psUser     
                           void* psFrame    
                           UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf160(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{   
     return emtTrans_afn0cf129(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf161
 功能描述  : F161：电能表远程控制通断电状态及记录
                          CMD_AFN_C_F161_METR_REMOTE_CTRL
 输入参数  : eMtTrans eTrans  
                           void* psUser     
                           void* psFrame    
                           UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月30日 星期五
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf161(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{   
    if (!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf161() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    if (MT_TRANS_F2U == eTrans) // 帧侧到用户侧
    {
        // 暂时不做处理，只计算数据长度
    }else if ( MT_TRANS_U2F == eTrans)  // 用户侧到侦测
    {
        // 暂时不做处理，只计算数据长度
    }
    else  
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf161() para error!");
        #endif
        return MT_ERR_PARA;
    }
    
    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0cf161_f);
    
     return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf165
 功能描述  : F165：电能表开关操作次数及时间
                          CMD_AFN_C_F165_METR_SWITCH_RECD
 输入参数  : eMtTrans eTrans  
                           void* psUser     
                           void* psFrame    
                           UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月30日 星期五
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf165(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{   
    if (!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf165() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    sMtAfn0cf165   *psMtAfn0cf65   = (sMtAfn0cf165 *)psUser;
    sMtAfn0cf165_f *psMtAfn0cf65_f = (sMtAfn0cf165_f *)psFrame;
    eMtErr eRet = MT_OK;

    eRet = emt_trans_YYMMDDhhmm(eTrans,&(psMtAfn0cf65->sTime) ,&(psMtAfn0cf65_f->sTime));
    if ( MT_OK != eRet)
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_YYMMDDhhmm() is %d",eRet);
        #endif
        return eRet; 
    }

    eRet = emt_trans_YYMMDDhhmm(eTrans,&(psMtAfn0cf65->sOne.sClock) ,&(psMtAfn0cf65_f->sOne.sClock));
    if ( MT_OK != eRet)
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_YYMMDDhhmm() is %d",eRet);
        #endif
        return eRet; 
    }

    eRet = emt_trans_XXXX(eTrans,&(psMtAfn0cf65->sOne.usTimes) ,&(psMtAfn0cf65_f->sOne.sTimes));
    if ( MT_OK != eRet)
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_XXXX() is %d",eRet);
        #endif
        return eRet; 
    }

    eRet = emt_trans_YYMMDDhhmm(eTrans,&(psMtAfn0cf65->sOther.sClock) ,&(psMtAfn0cf65_f->sOther.sClock));
    if ( MT_OK != eRet)
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_YYMMDDhhmm() is %d",eRet);
        #endif
        return eRet; 
    }

    eRet = emt_trans_XXXX(eTrans,&(psMtAfn0cf65->sOther.usTimes) ,&(psMtAfn0cf65_f->sOther.sTimes));
    if ( MT_OK != eRet)
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_XXXX() is %d",eRet);
        #endif
        return eRet; 
    }
    
    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0cf165_f);
     return eRet;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf166
 功能描述  : F166：电能表参数修改次数及时间
                          CMD_AFN_C_F166_METR_MODIFY_RECD
 输入参数  : eMtTrans eTrans  
                           void* psUser     
                           void* psFrame    
                           UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月30日 星期五
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf166(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{
     return emtTrans_afn0cf165(eTrans, psUser, psFrame, pusfLen);
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf167
 功能描述  : F167：电能表购、用电信息
                          CMD_AFN_C_F167_METR_BUY_USE
 输入参数  : eMtTrans eTrans  
                           void* psUser     
                           void* psFrame    
                           UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月30日 星期五
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf167(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{   
    if (!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf167() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    sMtAfn0cf167   *psMtAfn0cf167   = (sMtAfn0cf167 *)psUser;
    sMtAfn0cf167_f *psMtAfn0cf167_f = (sMtAfn0cf167_f *)psFrame;
    eMtErr eRet = MT_OK;

    eRet = emt_trans_YYMMDDhhmm(eTrans,&(psMtAfn0cf167->sTime),&(psMtAfn0cf167_f->sTime));
    if ( MT_OK != eRet )
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf167() emt_trans_YYMMDDhhmm is %d",eRet);
        #endif
        return eRet;
    }

    eRet = emt_trans_XXXX(eTrans,&(psMtAfn0cf167->usBuyTimes),&(psMtAfn0cf167_f->sBuyTimes));
    if ( MT_OK != eRet )
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf167() emt_trans_XXXX is %d",eRet);
        #endif
        return eRet;
    }

    eRet = emt_trans_XXXXXX_XXXX(eTrans,&(psMtAfn0cf167->dCashLeft),&(psMtAfn0cf167_f->sCashLeft));
    if ( MT_OK != eRet )
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf167() emt_trans_XXXXXX_XXXX is %d",eRet);
        #endif
        return eRet;
    }

    eRet = emt_trans_XXXXXX_XXXX(eTrans,&(psMtAfn0cf167->dCashTotal),&(psMtAfn0cf167_f->sCashTotal));
    if ( MT_OK != eRet )
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf167() emt_trans_XXXXXX_XXXX is %d",eRet);
        #endif
        return eRet;
    }

    eRet = emt_trans_XXXXXX_XX(eTrans,&(psMtAfn0cf167->dElecFault),&(psMtAfn0cf167_f->sElecFault));
    if ( MT_OK != eRet )
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf167() emt_trans_XXXXXX_XX is %d",eRet);
        #endif
        return eRet;
    }
    
    eRet = emt_trans_XXXXXX_XX(eTrans,&(psMtAfn0cf167->dElecLeft),&(psMtAfn0cf167_f->sElecLeft));
    if ( MT_OK != eRet )
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf167() emt_trans_XXXXXX_XX is %d",eRet);
        #endif
        return eRet;
    }

    eRet = emt_trans_XXXXXX_XX(eTrans,&(psMtAfn0cf167->dElecLimit),&(psMtAfn0cf167_f->sElecLimit));
    if ( MT_OK != eRet )
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf167() emt_trans_XXXXXX_XX is %d",eRet);
        #endif
        return eRet;
    }

    eRet = emt_trans_XXXXXX_XX(eTrans,&(psMtAfn0cf167->dElecOut),&(psMtAfn0cf167_f->sElecOut));
    if ( MT_OK != eRet )
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf167() emt_trans_XXXXXX_XX is %d",eRet);
        #endif
        return eRet;
    }

    eRet = emt_trans_XXXXXX_XX(eTrans,&(psMtAfn0cf167->dElecTotal),&(psMtAfn0cf167_f->sElecTotal));
    if ( MT_OK != eRet )
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf167() emt_trans_XXXXXX_XX is %d",eRet);
        #endif
        return eRet;
    }

    eRet = emt_trans_XXXXXX_XX(eTrans,&(psMtAfn0cf167->dElecWarn),&(psMtAfn0cf167_f->sElecWarn));
    if ( MT_OK != eRet )
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf167() emt_trans_XXXXXX_XX is %d",eRet);
        #endif
        return eRet;
    }
    
    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0cf167_f);
    
     return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf168
 功能描述  : F168：电能表结算信息
                          CMD_AFN_C_F168_METR_BALANCE
 输入参数  : eMtTrans eTrans  
                           void* psUser     
                           void* psFrame    
                           UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月30日 星期五
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf168(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{   
    if (!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf168() pointer is null");
        #endif
        return MT_ERR_NULL;
    }
    
    sMtAfn0cf168_f  *psMtAfn0cf168_f = (sMtAfn0cf168_f *)psFrame;
    
    if (MT_TRANS_F2U == eTrans) // 帧侧到用户侧
    {
        // 暂时不做处理，只计算数据长度
    }else if ( MT_TRANS_U2F == eTrans)  // 用户侧到侦测
    {
        // 暂时不做处理，只计算数据长度
    }
    else  
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf168() para error!");
        #endif
        return MT_ERR_PARA;
    }
    
    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0cf168_f) + (psMtAfn0cf168_f->usRateM + 2) * sizeof(sMtFmt14);
    
     return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf169
 功能描述  : F169：集中抄表中继路由信息
                          CMD_AFN_C_F169_READ_ROUTE
 输入参数  : eMtTrans eTrans  
                           void* psUser     
                           void* psFrame    
                           UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月30日 星期五
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf169(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{   
    if (!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf169() pointer is null");
        #endif
        return MT_ERR_NULL;
    }
    
    sMtAfn0cf169_f  *psMtAfn0cf169_f = (sMtAfn0cf169_f *)psFrame;
    UINT16  usAddress = 0;
    UINT8   i;
    if (MT_TRANS_F2U == eTrans) // 帧侧到用户侧
    {
        // 暂时不做处理，只计算数据长度
    }else if ( MT_TRANS_U2F == eTrans)  // 用户侧到侦测
    {
        // 暂时不做处理，只计算数据长度
    }
    else  
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf169() para error!");
        #endif
        return MT_ERR_PARA;
    }
    
    // 计算在帧侧的字节长度
    for (i = 0; i < psMtAfn0cf169_f->ucRoutN;i++)
    {
        usAddress += psMtAfn0cf169_f->sRoute[i].m;
    }
    
    *pusfLen = sizeof(sMtAfn0cf169_f) + psMtAfn0cf169_f->ucRoutN * sizeof(sMtRoute) + usAddress * sizeof(sMtFmt12);
    
     return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0cf170
 功能描述  : F170： 集中抄表电表抄读信息
                          CMD_AFN_C_F170_READ_METER
 输入参数  : eMtTrans eTrans  
                           void* psUser     
                           void* psFrame    
                           UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月30日 星期五
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0cf170(eMtTrans eTrans,void* psUser, void* psFrame, UINT16* pusfLen)
{   
    if (!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf170() pointer is null");
        #endif
        return MT_ERR_NULL;
    }
    
    if (MT_TRANS_F2U == eTrans) // 帧侧到用户侧
    {
        // 暂时不做处理，只计算数据长度
    }else if ( MT_TRANS_U2F == eTrans)  // 用户侧到侦测
    {
        // 暂时不做处理，只计算数据长度
    }
    else  
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0cf170() para error!");
        #endif
        return MT_ERR_PARA;
    }
    
    // 计算在帧侧的字节长度 
    *pusfLen = sizeof(sMtAfn0cf170_f);
    
     return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_td_h
 功能描述  : 小时冻结类数据时标数据格式 Td_h
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_td_h(eMtTrans eTrans, void* psUser, void* psFrame, UINT16* pusfLen)
{
    if(!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_td_h() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    sMtTd_h     *psU   = (sMtTd_h*)psUser;
    sMtTd_h_f   *psF   = (sMtTd_h_f*)psFrame;
    UINT8        ucHH  = 0;

    // 帧侧转为用户侧
    if(MT_TRANS_F2U == eTrans)
    {
        ucHH = psF->ucBCD_0 + psF->ucBCD_1 * 10;

        if(ucHH > MT_TD_H_MAX || ucHH < MT_TD_H_MIN)
        {
             #ifdef MT_DBG
             DEBUG("emtTrans_td_h() ucHH () out of range %d", ucHH);
             #endif
             return MT_ERR_OUTRNG;
        }

        psU->ucHH = ucHH;
    }
    else if(MT_TRANS_U2F == eTrans)
    // 用户侧转为帧侧
    {     
        if(psU->ucHH > MT_TD_H_MAX || psU->ucHH < MT_TD_H_MIN)
        {
             #ifdef MT_DBG
             DEBUG("emtTrans_td_h() ucHH () out of range %d", psU->ucHH);
             #endif
             return MT_ERR_OUTRNG;
        }
 
        psF->ucBCD_0 = ucmt_get_bcd_0(psU->ucHH);
        psF->ucBCD_1 = ucmt_get_bcd_1(psU->ucHH);
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_td_h() para error!");
        #endif
        return MT_ERR_PARA;
    }

    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtTd_h_f);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_td_c
 功能描述  : 曲线类数据时标数据格式 Td_c
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_td_c(eMtTrans eTrans, void* psUser, void* psFrame, UINT16* pusfLen)
{
    if(!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_td_c() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    sMtTd_c     *psU   = (sMtTd_c*)psUser;
    sMtTd_c_f   *psF   = (sMtTd_c_f*)psFrame;
    eMtErr       eRet  = MT_OK;

    // 帧侧转为用户侧
    if(MT_TRANS_F2U == eTrans)
    {
        psF->ucM = psU->ucM;
        psF->ucN = psU->ucN;
        
        eRet = emt_trans_YYMMDDhhmm(eTrans, &(psU->sT), &(psF->sT));
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_td_c() emt_trans_YYMMDDhhmm() error %d", eRet);
            #endif
            return eRet;
        }
    }
    else if(MT_TRANS_U2F == eTrans)
    // 用户侧转为帧侧
    {
        psU->ucM = psF->ucM;
        psU->ucN = psF->ucN;
        
        eRet = emt_trans_YYMMDDhhmm(eTrans, &(psU->sT), &(psF->sT));
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_td_c() emt_trans_YYMMDDhhmm() error %d", eRet);
            #endif
            return eRet;
        }
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_td_c() para error!");
        #endif
        return MT_ERR_PARA;
    }

    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtTd_c_f);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_td_d
 功能描述  : 日冻结类数据时标Td_d
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_td_d(eMtTrans eTrans, void* psUser, void* psFrame, UINT16* pusfLen)
{
    eMtErr eRet = MT_OK;
    eRet =  emt_trans_YYMMDD(eTrans, (sMtYYMMDD*)psUser, (sMtYYMMDD_f*)psFrame);
    *pusfLen = sizeof(sMtTd_d_f);
    return eRet;
}

/*****************************************************************************
 函 数 名  : emtTrans_td_m
 功能描述  : 日月冻结类数据时标Td_m
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_td_m(eMtTrans eTrans, void* psUser, void* psFrame, UINT16* pusfLen)
{
    eMtErr eRet = MT_OK;
    eRet =  emt_trans_YYMM(eTrans, (sMtYYMM*)psUser, (sMtYYMM_f*)psFrame);
    *pusfLen = sizeof(sMtTd_m_f);
    return eRet;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0df1
 功能描述  : F1：日冻结正向有/无功电能示值、一/四象限无功电能示值（总、费率1～M，1≤M≤12）
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0df1(eMtTrans eTrans, void* psUser, void* psFrame, UINT16* pusfLen)
{
    if(!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0df1() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    sMtAfn0dF1     *psU      = (sMtAfn0dF1*)psUser;
    sMtAfn0dF1_f   *psF      = (sMtAfn0dF1_f*)psFrame;
    eMtErr         eRet      = MT_OK;
    UINT8          ucM       = 0;
    UINT16         usLen     = 0;
    UINT16         usTmp     = 0;
    INT32          i         = 0;
    UINT8          *pTmp     = NULL;
    sMtFmt14_f    *psFmt14_f = NULL;
    sMtFmt11_f    *psFmt11_f = NULL;
    
    // 帧侧转为用户侧
    if(MT_TRANS_F2U == eTrans)
    {
        // Td_d
        eRet = emtTrans_td_d(eTrans, &(psU->sTd_d), &(psF->sTd_d), &usTmp);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df1() emtTrans_td_d() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += usTmp;

        // sTime
        eRet = emt_trans_YYMMDDhhmm(eTrans, &(psU->sTime), &(psF->sTime));
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df1() emt_trans_YYMMDDhhmm() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += sizeof(sMtFmt15_f);

        // M
        ucM = psF->ucM;
        if(ucM < MT_M_MIN || ucM > MT_M_MAX)
        {
            #ifdef MT_DBG
            DEBUG("ucM out of range %d", ucM);
            #endif
            return MT_ERR_OUTRNG;     
        }
        
        usLen += 1;

        // psFmt14_f
        pTmp      = (UINT8*)(psF->ucPower);
        psFmt14_f = (sMtFmt14_f*)pTmp;

        // dFrthHavePowerT
        eRet = emt_trans_XXXXXX_XXXX(eTrans, &(psU->dFrthHavePowerT), psFmt14_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df1() emt_trans_XXXXXX_XXXX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt14_f);
        
        // dFrthHavePower
        for(i = 0; i < ucM; i++)
        {
            psFmt14_f = (sMtFmt14_f*)(pTmp + (i+1)*sizeof(sMtFmt14_f));
            eRet = emt_trans_XXXXXX_XXXX(eTrans, &(psU->dFrthHavePower[i]), psFmt14_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df1() emt_trans_XXXXXX_XXXX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt14_f) * ucM;

        // dFrthNonePowerT
        pTmp      = (UINT8 *)(psFmt14_f + 1);
        psFmt11_f = (sMtFmt11_f*)pTmp;
        eRet = emt_trans_XXXXXX_XX(eTrans, &(psU->dFrthNonePowerT), psFmt11_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df1() emt_trans_XXXXXX_XX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt11_f);

        // dFrthNonePower
        for(i = 0; i < ucM; i++)
        {
            psFmt11_f = (sMtFmt11_f*)(pTmp + (i+1)*sizeof(sMtFmt11_f));
            eRet = emt_trans_XXXXXX_XX(eTrans, &(psU->dFrthNonePower[i]), psFmt11_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df1() emt_trans_XXXXXX_XX() error %d", eRet);
                #endif
                return eRet;
            }
        }
        
        usLen += sizeof(sMtFmt11_f) * ucM;

        // dNonePowerP1T
        psFmt11_f += 1;
        eRet = emt_trans_XXXXXX_XX(eTrans, &(psU->dNonePowerP1T), psFmt11_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df1() emt_trans_XXXXXX_XX() error %d", eRet);
            #endif
            return eRet;
        }

         usLen += sizeof(sMtFmt11_f);

        // dFrthNonePower
        for(i = 0; i < ucM; i++)
        {
            psFmt11_f = (sMtFmt11_f*)(pTmp + (i+1)*sizeof(sMtFmt11_f));
            eRet = emt_trans_XXXXXX_XX(eTrans, &(psU->dNonePowerP1[i]), psFmt11_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df1() emt_trans_XXXXXX_XX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt11_f) * ucM;

        // dNonePowerP1T
        psFmt11_f += 1;
        eRet = emt_trans_XXXXXX_XX(eTrans, &(psU->dNonePowerP4T), psFmt11_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df1() emt_trans_XXXXXX_XX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt11_f);

        // dFrthNonePower
        for(i = 0; i < ucM; i++)
        {
            psFmt11_f = (sMtFmt11_f*)(pTmp + (i+1)*sizeof(sMtFmt11_f));
            eRet = emt_trans_XXXXXX_XX(eTrans, &(psU->dNonePowerP4[i]), psFmt11_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df1() emt_trans_XXXXXX_XX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt11_f) * ucM;

    }
    else if(MT_TRANS_U2F == eTrans)
    // 用户侧转为帧侧
    {
        // Td_d
        eRet = emtTrans_td_d(eTrans, &(psU->sTd_d), &(psF->sTd_d), &usTmp);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df1() emtTrans_td_d() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += usTmp;

        // sTime
        eRet = emt_trans_YYMMDDhhmm(eTrans, &(psU->sTime), &(psF->sTime));
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df1() emt_trans_YYMMDDhhmm() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += sizeof(sMtFmt15_f);
        
        // M
        ucM = psU->ucM;
        if(ucM < MT_M_MIN || ucM > MT_M_MAX)
        {
            #ifdef MT_DBG
            DEBUG("ucM out of range %d", ucM);
            #endif
            return MT_ERR_OUTRNG;     
        }
        
        usLen += 1;

        // psFmt14_f
        pTmp      = (UINT8*)(psF->ucPower);
        psFmt14_f = (sMtFmt14_f*)pTmp;

        // dFrthHavePowerT
        eRet = emt_trans_XXXXXX_XXXX(eTrans, &(psU->dFrthHavePowerT), psFmt14_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df1() emt_trans_XXXXXX_XXXX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt14_f);
        
        // dFrthHavePower
        for(i = 0; i < ucM; i++)
        {
            psFmt14_f = (sMtFmt14_f*)(pTmp + (i+1)*sizeof(sMtFmt14_f));
            eRet = emt_trans_XXXXXX_XXXX(eTrans, &(psU->dFrthHavePower[i]), psFmt14_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df1() emt_trans_XXXXXX_XXXX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt14_f) * ucM;

        // dFrthNonePowerT
        pTmp      = (UINT8 *)(psFmt14_f + 1);
        psFmt11_f = (sMtFmt11_f*)pTmp;
        eRet = emt_trans_XXXXXX_XX(eTrans, &(psU->dFrthNonePowerT), psFmt11_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df1() emt_trans_XXXXXX_XX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt11_f);

        // dFrthNonePower
        for(i = 0; i < ucM; i++)
        {
            psFmt11_f = (sMtFmt11_f*)(pTmp + (i+1)*sizeof(sMtFmt11_f));
            eRet = emt_trans_XXXXXX_XX(eTrans, &(psU->dFrthNonePower[i]), psFmt11_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df1() emt_trans_XXXXXX_XX() error %d", eRet);
                #endif
                return eRet;
            }
        }
        
        usLen += sizeof(sMtFmt11_f) * ucM;

        // dNonePowerP1T
        psFmt11_f += 1;
        eRet = emt_trans_XXXXXX_XX(eTrans, &(psU->dNonePowerP1T), psFmt11_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df1() emt_trans_XXXXXX_XX() error %d", eRet);
            #endif
            return eRet;
        }

         usLen += sizeof(sMtFmt11_f);

        // dFrthNonePower
        for(i = 0; i < ucM; i++)
        {
            psFmt11_f = (sMtFmt11_f*)(pTmp + (i+1)*sizeof(sMtFmt11_f));
            eRet = emt_trans_XXXXXX_XX(eTrans, &(psU->dNonePowerP1[i]), psFmt11_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df1() emt_trans_XXXXXX_XX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt11_f) * ucM;

        // dNonePowerP1T
        psFmt11_f += 1;
        eRet = emt_trans_XXXXXX_XX(eTrans, &(psU->dNonePowerP4T), psFmt11_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df1() emt_trans_XXXXXX_XX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt11_f);

        // dFrthNonePower
        for(i = 0; i < ucM; i++)
        {
            psFmt11_f = (sMtFmt11_f*)(pTmp + (i+1)*sizeof(sMtFmt11_f));
            eRet = emt_trans_XXXXXX_XX(eTrans, &(psU->dNonePowerP4[i]), psFmt11_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df1() emt_trans_XXXXXX_XX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt11_f) * ucM;

    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0df1() para error!");
        #endif
        return MT_ERR_PARA;
    }

    // 计算在帧侧的字节长度
    *pusfLen = usLen;
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0df2
 功能描述  : F2：日冻结反向有/无功电能示值、二/三象限无功电能示值（总、费率1～M，1≤M≤12）
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0df2(eMtTrans eTrans, void* psUser, void* psFrame, UINT16* pusfLen)
{
    if(!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0df2() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    sMtAfn0dF2     *psU      = (sMtAfn0dF2*)psUser;
    sMtAfn0dF2_f   *psF      = (sMtAfn0dF2_f*)psFrame;
    eMtErr         eRet      = MT_OK;
    UINT8          ucM       = 0;
    UINT16         usLen     = 0;
    UINT16         usTmp     = 0;
    INT32          i         = 0;
    UINT8          *pTmp     = NULL;
    sMtFmt14_f    *psFmt14_f = NULL;
    sMtFmt11_f    *psFmt11_f = NULL;
    
    // 帧侧转为用户侧
    if(MT_TRANS_F2U == eTrans)
    {
        // Td_d
        eRet = emtTrans_td_d(eTrans, &(psU->sTd_d), &(psF->sTd_d), &usTmp);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df2() emtTrans_td_d() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += usTmp;

        // sTime
        eRet = emt_trans_YYMMDDhhmm(eTrans, &(psU->sTime), &(psF->sTime));
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df2() emt_trans_YYMMDDhhmm() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += sizeof(sMtFmt15_f);

        // M
        ucM = psF->ucM;
        if(ucM < MT_M_MIN || ucM > MT_M_MAX)
        {
            #ifdef MT_DBG
            DEBUG("ucM out of range %d", ucM);
            #endif
            return MT_ERR_OUTRNG;     
        }
        
        usLen += 1;

        // psFmt14_f
        pTmp      = (UINT8*)(psF->ucPower);
        psFmt14_f = (sMtFmt14_f*)pTmp;

        // dBackHavePowerT
        eRet = emt_trans_XXXXXX_XXXX(eTrans, &(psU->dBackHavePowerT), psFmt14_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df2() emt_trans_XXXXXX_XXXX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt14_f);
        
        // dBackHavePower
        for(i = 0; i < ucM; i++)
        {
            psFmt14_f = (sMtFmt14_f*)(pTmp + (i+1)*sizeof(sMtFmt14_f));
            eRet = emt_trans_XXXXXX_XXXX(eTrans, &(psU->dBackHavePower[i]), psFmt14_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df2() emt_trans_XXXXXX_XXXX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt14_f) * ucM;

        // dBackNonePowerT
        pTmp      = (UINT8 *)(psFmt14_f + 1);
        psFmt11_f = (sMtFmt11_f*)pTmp;
        eRet = emt_trans_XXXXXX_XX(eTrans, &(psU->dBackNonePowerT), psFmt11_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df2() emt_trans_XXXXXX_XX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt11_f);

        // dBackNonePower
        for(i = 0; i < ucM; i++)
        {
            psFmt11_f = (sMtFmt11_f*)(pTmp + (i+1)*sizeof(sMtFmt11_f));
            eRet = emt_trans_XXXXXX_XX(eTrans, &(psU->dBackNonePower[i]), psFmt11_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df2() emt_trans_XXXXXX_XX() error %d", eRet);
                #endif
                return eRet;
            }
        }
        
        usLen += sizeof(sMtFmt11_f) * ucM;

        // dNonePowerP2T
        psFmt11_f += 1;
        eRet = emt_trans_XXXXXX_XX(eTrans, &(psU->dNonePowerP2T), psFmt11_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df2() emt_trans_XXXXXX_XX() error %d", eRet);
            #endif
            return eRet;
        }

         usLen += sizeof(sMtFmt11_f);

        // dNonePowerP2
        for(i = 0; i < ucM; i++)
        {
            psFmt11_f = (sMtFmt11_f*)(pTmp + (i+1)*sizeof(sMtFmt11_f));
            eRet = emt_trans_XXXXXX_XX(eTrans, &(psU->dNonePowerP2[i]), psFmt11_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df2() emt_trans_XXXXXX_XX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt11_f) * ucM;

        // dNonePowerP3T
        psFmt11_f += 1;
        eRet = emt_trans_XXXXXX_XX(eTrans, &(psU->dNonePowerP3T), psFmt11_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df2() emt_trans_XXXXXX_XX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt11_f);

        // dNonePowerP3
        for(i = 0; i < ucM; i++)
        {
            psFmt11_f = (sMtFmt11_f*)(pTmp + (i+1)*sizeof(sMtFmt11_f));
            eRet = emt_trans_XXXXXX_XX(eTrans, &(psU->dNonePowerP3[i]), psFmt11_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df2() emt_trans_XXXXXX_XX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt11_f) * ucM;
    }
    else if(MT_TRANS_U2F == eTrans)
    // 用户侧转为帧侧
    {
        // Td_d
        eRet = emtTrans_td_d(eTrans, &(psU->sTd_d), &(psF->sTd_d), &usTmp);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df2() emtTrans_td_d() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += usTmp;

        // sTime
        eRet = emt_trans_YYMMDDhhmm(eTrans, &(psU->sTime), &(psF->sTime));
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df2() emt_trans_YYMMDDhhmm() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += sizeof(sMtFmt15_f);
        
        // M
        ucM = psU->ucM;
        if(ucM < MT_M_MIN || ucM > MT_M_MAX)
        {
            #ifdef MT_DBG
            DEBUG("ucM out of range %d", ucM);
            #endif
            return MT_ERR_OUTRNG;     
        }
        
        usLen += 1;

        // psFmt14_f
        pTmp      = (UINT8*)(psF->ucPower);
        psFmt14_f = (sMtFmt14_f*)pTmp;

        // dBackHavePowerT
        eRet = emt_trans_XXXXXX_XXXX(eTrans, &(psU->dBackHavePowerT), psFmt14_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df2() emt_trans_XXXXXX_XXXX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt14_f);
        
        // dBackHavePower
        for(i = 0; i < ucM; i++)
        {
            psFmt14_f = (sMtFmt14_f*)(pTmp + (i+1)*sizeof(sMtFmt14_f));
            eRet = emt_trans_XXXXXX_XXXX(eTrans, &(psU->dBackHavePower[i]), psFmt14_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df2() emt_trans_XXXXXX_XXXX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt14_f) * ucM;

        // dBackNonePowerT
        pTmp      = (UINT8 *)(psFmt14_f + 1);
        psFmt11_f = (sMtFmt11_f*)pTmp;
        eRet = emt_trans_XXXXXX_XX(eTrans, &(psU->dBackNonePowerT), psFmt11_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df2() emt_trans_XXXXXX_XX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt11_f);

        // dBackNonePower
        for(i = 0; i < ucM; i++)
        {
            psFmt11_f = (sMtFmt11_f*)(pTmp + (i+1)*sizeof(sMtFmt11_f));
            eRet = emt_trans_XXXXXX_XX(eTrans, &(psU->dBackNonePower[i]), psFmt11_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df2() emt_trans_XXXXXX_XX() error %d", eRet);
                #endif
                return eRet;
            }
        }
        
        usLen += sizeof(sMtFmt11_f) * ucM;

        // dNonePowerP2T
        psFmt11_f += 1;
        eRet = emt_trans_XXXXXX_XX(eTrans, &(psU->dNonePowerP2T), psFmt11_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df2() emt_trans_XXXXXX_XX() error %d", eRet);
            #endif
            return eRet;
        }

         usLen += sizeof(sMtFmt11_f);

        // dNonePowerP2
        for(i = 0; i < ucM; i++)
        {
            psFmt11_f = (sMtFmt11_f*)(pTmp + (i+1)*sizeof(sMtFmt11_f));
            eRet = emt_trans_XXXXXX_XX(eTrans, &(psU->dNonePowerP2[i]), psFmt11_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df2() emt_trans_XXXXXX_XX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt11_f) * ucM;

        // dNonePowerP3T
        psFmt11_f += 1;
        eRet = emt_trans_XXXXXX_XX(eTrans, &(psU->dNonePowerP3T), psFmt11_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df2() emt_trans_XXXXXX_XX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt11_f);

        // dNonePowerP3
        for(i = 0; i < ucM; i++)
        {
            psFmt11_f = (sMtFmt11_f*)(pTmp + (i+1)*sizeof(sMtFmt11_f));
            eRet = emt_trans_XXXXXX_XX(eTrans, &(psU->dNonePowerP3[i]), psFmt11_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df2() emt_trans_XXXXXX_XX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt11_f) * ucM;

    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0df2() para error!");
        #endif
        return MT_ERR_PARA;
    }

    // 计算在帧侧的字节长度
    *pusfLen = usLen;
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0df3
 功能描述  : F3：日冻结正向有/无功最大需量及发生时间（总、费率1～M，1≤M≤12）
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月30日 星期五
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0df3(eMtTrans eTrans, void* psUser, void* psFrame, UINT16* pusfLen)
{
    if(!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0df3() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    sMtAfn0dF3     *psU      = (sMtAfn0dF3*)psUser;
    sMtAfn0dF3_f   *psF      = (sMtAfn0dF3_f*)psFrame;
    eMtErr         eRet      = MT_OK;
    UINT8          ucM       = 0;
    UINT16         usLen     = 0;
    UINT16         usTmp     = 0;
    INT32          i         = 0;
    UINT8          *pTmp     = NULL;
    sMtFmt23_f    *psFmt23_f = NULL;
    sMtFmt17_f    *psFmt17_f = NULL;
    
    // 帧侧转为用户侧
    if(MT_TRANS_F2U == eTrans)
    {
        // Td_d
        eRet = emtTrans_td_d(eTrans, &(psU->sTd_d), &(psF->sTd_d), &usTmp);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df3() emtTrans_td_d() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += usTmp;

        // sTime
        eRet = emt_trans_YYMMDDhhmm(eTrans, &(psU->sTime), &(psF->sTime));
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df3() emt_trans_YYMMDDhhmm() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += sizeof(sMtFmt15_f);

        // M
        ucM = psF->ucM;
        if(ucM < MT_M_MIN || ucM > MT_M_MAX)
        {
            #ifdef MT_DBG
            DEBUG("ucM out of range %d", ucM);
            #endif
            return MT_ERR_OUTRNG;     
        }
        
        usLen += 1;

        // fFrthHaveT
        pTmp      = (UINT8*)(psF->ucDemand);
        psFmt23_f = (sMtFmt23_f*)pTmp;

        // fFrthHaveT
        eRet = emt_trans_XX_XXXX(eTrans, &(psU->fFrthHaveT), psFmt23_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df3() emt_trans_XX_XXXX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt23_f);
        
        // fFrthHave
        for(i = 0; i < ucM; i++)
        {
            psFmt23_f = (sMtFmt23_f*)(pTmp + (i+1) * sizeof(sMtFmt23_f));
            eRet = emt_trans_XX_XXXX(eTrans, &(psU->fFrthHave[i]), psFmt23_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df3() emt_trans_XX_XXXX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt23_f) * ucM;

        // sTimeFrthHaveT
        pTmp      = (UINT8 *)(psFmt23_f + 1);
        psFmt17_f = (sMtFmt17_f*)pTmp;
        eRet = emt_trans_MMDDHHmm(eTrans, &(psU->sTimeFrthHaveT), psFmt17_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df3() emt_trans_MMDDHHmm() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt17_f);

        // sTimeFrthHave
        for(i = 0; i < ucM; i++)
        {
            psFmt17_f = (sMtFmt17_f*)(pTmp + (i+1)*sizeof(sMtFmt17_f));
            eRet = emt_trans_MMDDHHmm(eTrans, &(psU->sTimeFrthHave[i]), psFmt17_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df3() emt_trans_MMDDHHmm() error %d", eRet);
                #endif
                return eRet;
            }
        }
        
        usLen += sizeof(sMtFmt17_f) * ucM;

        // fFrthNoneT
        psFmt17_f += 1;
        psFmt23_f = (sMtFmt23_f*)psFmt17_f;
        eRet = emt_trans_XX_XXXX(eTrans, &(psU->fFrthNoneT), psFmt23_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df3() emt_trans_XX_XXXX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt23_f);
        
        // fFrthNone
        for(i = 0; i < ucM; i++)
        {
            psFmt23_f = (sMtFmt23_f*)(pTmp + (i+1) * sizeof(sMtFmt23_f));
            eRet = emt_trans_XX_XXXX(eTrans, &(psU->fFrthNone[i]), psFmt23_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df3() emt_trans_XX_XXXX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt23_f) * ucM;

        // sTimeFrthNoneT
        pTmp      = (UINT8 *)(psFmt23_f + 1);
        psFmt17_f = (sMtFmt17_f*)pTmp;
        eRet = emt_trans_MMDDHHmm(eTrans, &(psU->sTimeFrthNoneT), psFmt17_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df3() emt_trans_MMDDHHmm() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt17_f);

        // sTimeFrthNone
        for(i = 0; i < ucM; i++)
        {
            psFmt17_f = (sMtFmt17_f*)(pTmp + (i+1)*sizeof(sMtFmt17_f));
            eRet = emt_trans_MMDDHHmm(eTrans, &(psU->sTimeFrthNone[i]), psFmt17_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df3() emt_trans_MMDDHHmm() error %d", eRet);
                #endif
                return eRet;
            }
        }
        
        usLen += sizeof(sMtFmt17_f) * ucM;
        
    }
    else if(MT_TRANS_U2F == eTrans)
    // 用户侧转为帧侧
    {
        // Td_d
        eRet = emtTrans_td_d(eTrans, &(psU->sTd_d), &(psF->sTd_d), &usTmp);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df3() emtTrans_td_d() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += usTmp;

        // sTime
        eRet = emt_trans_YYMMDDhhmm(eTrans, &(psU->sTime), &(psF->sTime));
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df3() emt_trans_YYMMDDhhmm() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += sizeof(sMtFmt15_f);

        // M
        ucM = psU->ucM;
        if(ucM < MT_M_MIN || ucM > MT_M_MAX)
        {
            #ifdef MT_DBG
            DEBUG("ucM out of range %d", ucM);
            #endif
            return MT_ERR_OUTRNG;     
        }
        
        usLen += 1;

        // fFrthHaveT
        pTmp      = (UINT8*)(psF->ucDemand);
        psFmt23_f = (sMtFmt23_f*)pTmp;

        // fFrthHaveT
        eRet = emt_trans_XX_XXXX(eTrans, &(psU->fFrthHaveT), psFmt23_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df3() emt_trans_XX_XXXX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt23_f);
        
        // fFrthHave
        for(i = 0; i < ucM; i++)
        {
            psFmt23_f = (sMtFmt23_f*)(pTmp + (i+1) * sizeof(sMtFmt23_f));
            eRet = emt_trans_XX_XXXX(eTrans, &(psU->fFrthHave[i]), psFmt23_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df3() emt_trans_XX_XXXX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt23_f) * ucM;

        // sTimeFrthHaveT
        pTmp      = (UINT8 *)(psFmt23_f + 1);
        psFmt17_f = (sMtFmt17_f*)pTmp;
        eRet = emt_trans_MMDDHHmm(eTrans, &(psU->sTimeFrthHaveT), psFmt17_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df3() emt_trans_MMDDHHmm() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt17_f);

        // sTimeFrthHave
        for(i = 0; i < ucM; i++)
        {
            psFmt17_f = (sMtFmt17_f*)(pTmp + (i+1)*sizeof(sMtFmt17_f));
            eRet = emt_trans_MMDDHHmm(eTrans, &(psU->sTimeFrthHave[i]), psFmt17_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df3() emt_trans_MMDDHHmm() error %d", eRet);
                #endif
                return eRet;
            }
        }
        
        usLen += sizeof(sMtFmt17_f) * ucM;

        // fFrthNoneT
        psFmt17_f += 1;
        psFmt23_f = (sMtFmt23_f*)psFmt17_f;
        eRet = emt_trans_XX_XXXX(eTrans, &(psU->fFrthNoneT), psFmt23_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df3() emt_trans_XX_XXXX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt23_f);
        
        // fFrthNone
        for(i = 0; i < ucM; i++)
        {
            psFmt23_f = (sMtFmt23_f*)(pTmp + (i+1) * sizeof(sMtFmt23_f));
            eRet = emt_trans_XX_XXXX(eTrans, &(psU->fFrthNone[i]), psFmt23_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df3() emt_trans_XX_XXXX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt23_f) * ucM;

        // sTimeFrthNoneT
        pTmp      = (UINT8 *)(psFmt23_f + 1);
        psFmt17_f = (sMtFmt17_f*)pTmp;
        eRet = emt_trans_MMDDHHmm(eTrans, &(psU->sTimeFrthNoneT), psFmt17_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df3() emt_trans_MMDDHHmm() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt17_f);

        // sTimeFrthNone
        for(i = 0; i < ucM; i++)
        {
            psFmt17_f = (sMtFmt17_f*)(pTmp + (i+1)*sizeof(sMtFmt17_f));
            eRet = emt_trans_MMDDHHmm(eTrans, &(psU->sTimeFrthNone[i]), psFmt17_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df3() emt_trans_MMDDHHmm() error %d", eRet);
                #endif
                return eRet;
            }
        }
        
        usLen += sizeof(sMtFmt17_f) * ucM;
        
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0df3() para error!");
        #endif
        return MT_ERR_PARA;
    }

    // 计算在帧侧的字节长度
    *pusfLen = usLen;
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0df4
 功能描述  : F4：日冻结反向有/无功最大需量及发生时间（总、费率1～M，1≤M≤12）
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月30日 星期五
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0df4(eMtTrans eTrans, void* psUser, void* psFrame, UINT16* pusfLen)
{
    if(!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0df4() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    sMtAfn0dF4     *psU      = (sMtAfn0dF4*)psUser;
    sMtAfn0dF4_f   *psF      = (sMtAfn0dF4_f*)psFrame;
    eMtErr         eRet      = MT_OK;
    UINT8          ucM       = 0;
    UINT16         usLen     = 0;
    UINT16         usTmp     = 0;
    INT32          i         = 0;
    UINT8          *pTmp     = NULL;
    sMtFmt23_f    *psFmt23_f = NULL;
    sMtFmt17_f    *psFmt17_f = NULL;
    
    // 帧侧转为用户侧
    if(MT_TRANS_F2U == eTrans)
    {
        // Td_d
        eRet = emtTrans_td_d(eTrans, &(psU->sTd_d), &(psF->sTd_d), &usTmp);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df4() emtTrans_td_d() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += usTmp;

        // sTime
        eRet = emt_trans_YYMMDDhhmm(eTrans, &(psU->sTime), &(psF->sTime));
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df4() emt_trans_YYMMDDhhmm() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += sizeof(sMtFmt15_f);

        // M
        ucM = psF->ucM;
        if(ucM < MT_M_MIN || ucM > MT_M_MAX)
        {
            #ifdef MT_DBG
            DEBUG("ucM out of range %d", ucM);
            #endif
            return MT_ERR_OUTRNG;     
        }
        
        usLen += 1;

        // fBackHaveT
        pTmp      = (UINT8*)(psF->ucDemand);
        psFmt23_f = (sMtFmt23_f*)pTmp;

        // fBackHaveT
        eRet = emt_trans_XX_XXXX(eTrans, &(psU->fBackHaveT), psFmt23_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df4() emt_trans_XX_XXXX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt23_f);
        
        // fBackHave
        for(i = 0; i < ucM; i++)
        {
            psFmt23_f = (sMtFmt23_f*)(pTmp + (i+1) * sizeof(sMtFmt23_f));
            eRet = emt_trans_XX_XXXX(eTrans, &(psU->fBackHave[i]), psFmt23_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df4() emt_trans_XX_XXXX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt23_f) * ucM;

        // sTimeBackHaveT
        pTmp      = (UINT8 *)(psFmt23_f + 1);
        psFmt17_f = (sMtFmt17_f*)pTmp;
        eRet = emt_trans_MMDDHHmm(eTrans, &(psU->sTimeBackHaveT), psFmt17_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df4() emt_trans_MMDDHHmm() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt17_f);

        // sTimeBackHave
        for(i = 0; i < ucM; i++)
        {
            psFmt17_f = (sMtFmt17_f*)(pTmp + (i+1)*sizeof(sMtFmt17_f));
            eRet = emt_trans_MMDDHHmm(eTrans, &(psU->sTimeBackHave[i]), psFmt17_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df4() emt_trans_MMDDHHmm() error %d", eRet);
                #endif
                return eRet;
            }
        }
        
        usLen += sizeof(sMtFmt17_f) * ucM;

        // fBackNoneT
        psFmt17_f += 1;
        psFmt23_f = (sMtFmt23_f*)psFmt17_f;
        eRet = emt_trans_XX_XXXX(eTrans, &(psU->fBackNoneT), psFmt23_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df4() emt_trans_XX_XXXX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt23_f);
        
        // fBackNone
        for(i = 0; i < ucM; i++)
        {
            psFmt23_f = (sMtFmt23_f*)(pTmp + (i+1) * sizeof(sMtFmt23_f));
            eRet = emt_trans_XX_XXXX(eTrans, &(psU->fBackNone[i]), psFmt23_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df4() emt_trans_XX_XXXX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt23_f) * ucM;

        // sTimeBackNoneT
        pTmp      = (UINT8 *)(psFmt23_f + 1);
        psFmt17_f = (sMtFmt17_f*)pTmp;
        eRet = emt_trans_MMDDHHmm(eTrans, &(psU->sTimeBackNoneT), psFmt17_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df4() emt_trans_MMDDHHmm() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt17_f);

        // sTimeBackNone
        for(i = 0; i < ucM; i++)
        {
            psFmt17_f = (sMtFmt17_f*)(pTmp + (i+1)*sizeof(sMtFmt17_f));
            eRet = emt_trans_MMDDHHmm(eTrans, &(psU->sTimeBackNone[i]), psFmt17_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df4() emt_trans_MMDDHHmm() error %d", eRet);
                #endif
                return eRet;
            }
        }
        
        usLen += sizeof(sMtFmt17_f) * ucM;
        
    }
    else if(MT_TRANS_U2F == eTrans)
    // 用户侧转为帧侧
    {
        // Td_d
        eRet = emtTrans_td_d(eTrans, &(psU->sTd_d), &(psF->sTd_d), &usTmp);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df4() emtTrans_td_d() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += usTmp;

        // sTime
        eRet = emt_trans_YYMMDDhhmm(eTrans, &(psU->sTime), &(psF->sTime));
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df4() emt_trans_YYMMDDhhmm() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += sizeof(sMtFmt15_f);

        // M
        ucM = psU->ucM;
        if(ucM < MT_M_MIN || ucM > MT_M_MAX)
        {
            #ifdef MT_DBG
            DEBUG("ucM out of range %d", ucM);
            #endif
            return MT_ERR_OUTRNG;     
        }
        
        usLen += 1;

        // fBackHaveT
        pTmp      = (UINT8*)(psF->ucDemand);
        psFmt23_f = (sMtFmt23_f*)pTmp;

        // fBackHaveT
        eRet = emt_trans_XX_XXXX(eTrans, &(psU->fBackHaveT), psFmt23_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df4() emt_trans_XX_XXXX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt23_f);
        
        // fBackHave
        for(i = 0; i < ucM; i++)
        {
            psFmt23_f = (sMtFmt23_f*)(pTmp + (i+1) * sizeof(sMtFmt23_f));
            eRet = emt_trans_XX_XXXX(eTrans, &(psU->fBackHave[i]), psFmt23_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df4() emt_trans_XX_XXXX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt23_f) * ucM;

        // sTimeBackHaveT
        pTmp      = (UINT8 *)(psFmt23_f + 1);
        psFmt17_f = (sMtFmt17_f*)pTmp;
        eRet = emt_trans_MMDDHHmm(eTrans, &(psU->sTimeBackHaveT), psFmt17_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df4() emt_trans_MMDDHHmm() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt17_f);

        // sTimeBackHave
        for(i = 0; i < ucM; i++)
        {
            psFmt17_f = (sMtFmt17_f*)(pTmp + (i+1)*sizeof(sMtFmt17_f));
            eRet = emt_trans_MMDDHHmm(eTrans, &(psU->sTimeBackHave[i]), psFmt17_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df4() emt_trans_MMDDHHmm() error %d", eRet);
                #endif
                return eRet;
            }
        }
        
        usLen += sizeof(sMtFmt17_f) * ucM;

        // fBackNoneT
        psFmt17_f += 1;
        psFmt23_f = (sMtFmt23_f*)psFmt17_f;
        eRet = emt_trans_XX_XXXX(eTrans, &(psU->fBackNoneT), psFmt23_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df4() emt_trans_XX_XXXX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt23_f);
        
        // fBackNone
        for(i = 0; i < ucM; i++)
        {
            psFmt23_f = (sMtFmt23_f*)(pTmp + (i+1) * sizeof(sMtFmt23_f));
            eRet = emt_trans_XX_XXXX(eTrans, &(psU->fBackNone[i]), psFmt23_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df4() emt_trans_XX_XXXX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt23_f) * ucM;

        // sTimeBackNoneT
        pTmp      = (UINT8 *)(psFmt23_f + 1);
        psFmt17_f = (sMtFmt17_f*)pTmp;
        eRet = emt_trans_MMDDHHmm(eTrans, &(psU->sTimeBackNoneT), psFmt17_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df4() emt_trans_MMDDHHmm() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt17_f);

        // sTimeBackNone
        for(i = 0; i < ucM; i++)
        {
            psFmt17_f = (sMtFmt17_f*)(pTmp + (i+1)*sizeof(sMtFmt17_f));
            eRet = emt_trans_MMDDHHmm(eTrans, &(psU->sTimeBackNone[i]), psFmt17_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df4() emt_trans_MMDDHHmm() error %d", eRet);
                #endif
                return eRet;
            }
        }
        
        usLen += sizeof(sMtFmt17_f) * ucM;
        
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0df4() para error!");
        #endif
        return MT_ERR_PARA;
    }

    // 计算在帧侧的字节长度
    *pusfLen = usLen;
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0df5
 功能描述  : F5：日冻结正向有功电能量（总、费率1～M）
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月30日 星期五
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0df5(eMtTrans eTrans, void* psUser, void* psFrame, UINT16* pusfLen)
{
    if(!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0df5() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    sMtAfn0dF5     *psU      = (sMtAfn0dF5*)psUser;
    sMtAfn0dF5_f   *psF      = (sMtAfn0dF5_f*)psFrame;
    eMtErr         eRet      = MT_OK;
    UINT8          ucM       = 0;
    UINT16         usLen     = 0;
    UINT16         usTmp     = 0;
    INT32          i         = 0;
    sMtFmt13_f    *psFmt13_f = NULL;
    
    // 帧侧转为用户侧
    if(MT_TRANS_F2U == eTrans)
    {
        // Td_d
        eRet = emtTrans_td_d(eTrans, &(psU->sTd_d), &(psF->sTd_d), &usTmp);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df5() emtTrans_td_d() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += usTmp;

        // sTime
        eRet = emt_trans_YYMMDDhhmm(eTrans, &(psU->sTime), &(psF->sTime));
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df5() emt_trans_YYMMDDhhmm() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += sizeof(sMtFmt15_f);

        // M
        ucM = psF->ucM;
        if(ucM < MT_M_MIN || ucM > MT_M_MAX)
        {
            #ifdef MT_DBG
            DEBUG("ucM out of range %d", ucM);
            #endif
            return MT_ERR_OUTRNG;     
        }
        
        usLen += 1;

        // dFrthHaveT
        psFmt13_f = &(psF->dFrthHaveT);

        eRet = emt_trans_XXXX_XXXX(eTrans, &(psU->dFrthHaveT), psFmt13_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df5() emt_trans_XXXX_XXXX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt13_f);
        
        // dFrthHave
        for(i = 0; i < ucM; i++)
        {
            ++psFmt13_f;
            eRet = emt_trans_XXXX_XXXX(eTrans, &(psU->dFrthHave[i]), psFmt13_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df5() emt_trans_XXXX_XXXX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt13_f) * ucM;

    }
    else if(MT_TRANS_U2F == eTrans)
    // 用户侧转为帧侧
    {
        // Td_d
        eRet = emtTrans_td_d(eTrans, &(psU->sTd_d), &(psF->sTd_d), &usTmp);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df5() emtTrans_td_d() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += usTmp;

        // sTime
        eRet = emt_trans_YYMMDDhhmm(eTrans, &(psU->sTime), &(psF->sTime));
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df5() emt_trans_YYMMDDhhmm() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += sizeof(sMtFmt15_f);

        // M
        ucM = psU->ucM;
        if(ucM < MT_M_MIN || ucM > MT_M_MAX)
        {
            #ifdef MT_DBG
            DEBUG("ucM out of range %d", ucM);
            #endif
            return MT_ERR_OUTRNG;     
        }
        
        usLen += 1;

        // dFrthHaveT
        psFmt13_f = &(psF->dFrthHaveT);

        eRet = emt_trans_XXXX_XXXX(eTrans, &(psU->dFrthHaveT), psFmt13_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df5() emt_trans_XXXX_XXXX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt13_f);
        
        // dFrthHave
        for(i = 0; i < ucM; i++)
        {
            ++psFmt13_f;
            eRet = emt_trans_XXXX_XXXX(eTrans, &(psU->dFrthHave[i]), psFmt13_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df5() emt_trans_XXXX_XXXX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt13_f) * ucM;

    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0df5() para error!");
        #endif
        return MT_ERR_PARA;
    }

    // 计算在帧侧的字节长度
    *pusfLen = usLen;
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0df6
 功能描述  : F6：日冻结正向无功电能量（总、费率1～M）
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月30日 星期五
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0df6(eMtTrans eTrans, void* psUser, void* psFrame, UINT16* pusfLen)
{
    if(!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0df6() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    sMtAfn0dF6     *psU      = (sMtAfn0dF6*)psUser;
    sMtAfn0dF6_f   *psF      = (sMtAfn0dF6_f*)psFrame;
    eMtErr         eRet      = MT_OK;
    UINT8          ucM       = 0;
    UINT16         usLen     = 0;
    UINT16         usTmp     = 0;
    INT32          i         = 0;
    sMtFmt13_f    *psFmt13_f = NULL;
    
    // 帧侧转为用户侧
    if(MT_TRANS_F2U == eTrans)
    {
        // Td_d
        eRet = emtTrans_td_d(eTrans, &(psU->sTd_d), &(psF->sTd_d), &usTmp);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df6() emtTrans_td_d() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += usTmp;

        // sTime
        eRet = emt_trans_YYMMDDhhmm(eTrans, &(psU->sTime), &(psF->sTime));
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df6() emt_trans_YYMMDDhhmm() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += sizeof(sMtFmt15_f);

        // M
        ucM = psF->ucM;
        if(ucM < MT_M_MIN || ucM > MT_M_MAX)
        {
            #ifdef MT_DBG
            DEBUG("ucM out of range %d", ucM);
            #endif
            return MT_ERR_OUTRNG;     
        }
        
        usLen += 1;

        // dFrthNoneT
        psFmt13_f = &(psF->dFrthNoneT);

        eRet = emt_trans_XXXX_XXXX(eTrans, &(psU->dFrthNoneT), psFmt13_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df6() emt_trans_XXXX_XXXX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt13_f);
        
        // dFrthNone
        for(i = 0; i < ucM; i++)
        {
            ++psFmt13_f;
            eRet = emt_trans_XXXX_XXXX(eTrans, &(psU->dFrthNone[i]), psFmt13_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df6() emt_trans_XXXX_XXXX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt13_f) * ucM;

    }
    else if(MT_TRANS_U2F == eTrans)
    // 用户侧转为帧侧
    {
        // Td_d
        eRet = emtTrans_td_d(eTrans, &(psU->sTd_d), &(psF->sTd_d), &usTmp);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df6() emtTrans_td_d() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += usTmp;

        // sTime
        eRet = emt_trans_YYMMDDhhmm(eTrans, &(psU->sTime), &(psF->sTime));
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df6() emt_trans_YYMMDDhhmm() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += sizeof(sMtFmt15_f);

        // M
        ucM = psU->ucM;
        if(ucM < MT_M_MIN || ucM > MT_M_MAX)
        {
            #ifdef MT_DBG
            DEBUG("ucM out of range %d", ucM);
            #endif
            return MT_ERR_OUTRNG;     
        }
        
        usLen += 1;

        // dFrthNoneT
        psFmt13_f = &(psF->dFrthNoneT);

        eRet = emt_trans_XXXX_XXXX(eTrans, &(psU->dFrthNoneT), psFmt13_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df6() emt_trans_XXXX_XXXX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt13_f);
        
        // dFrthNone
        for(i = 0; i < ucM; i++)
        {
            ++psFmt13_f;
            eRet = emt_trans_XXXX_XXXX(eTrans, &(psU->dFrthNone[i]), psFmt13_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df6() emt_trans_XXXX_XXXX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt13_f) * ucM;

    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0df6() para error!");
        #endif
        return MT_ERR_PARA;
    }

    // 计算在帧侧的字节长度
    *pusfLen = usLen;
    return MT_OK;
}


/*****************************************************************************
 函 数 名  : emtTrans_afn0df7
 功能描述  : F7：日冻结反向有功电能量（总、费率1～M）
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月30日 星期五
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0df7(eMtTrans eTrans, void* psUser, void* psFrame, UINT16* pusfLen)
{
    if(!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0df7() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    sMtAfn0dF7     *psU      = (sMtAfn0dF7*)psUser;
    sMtAfn0dF7_f   *psF      = (sMtAfn0dF7_f*)psFrame;
    eMtErr         eRet      = MT_OK;
    UINT8          ucM       = 0;
    UINT16         usLen     = 0;
    UINT16         usTmp     = 0;
    INT32          i         = 0;
    sMtFmt13_f    *psFmt13_f = NULL;
    
    // 帧侧转为用户侧
    if(MT_TRANS_F2U == eTrans)
    {
        // Td_d
        eRet = emtTrans_td_d(eTrans, &(psU->sTd_d), &(psF->sTd_d), &usTmp);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df7() emtTrans_td_d() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += usTmp;

        // sTime
        eRet = emt_trans_YYMMDDhhmm(eTrans, &(psU->sTime), &(psF->sTime));
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df7() emt_trans_YYMMDDhhmm() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += sizeof(sMtFmt15_f);

        // M
        ucM = psF->ucM;
        if(ucM < MT_M_MIN || ucM > MT_M_MAX)
        {
            #ifdef MT_DBG
            DEBUG("ucM out of range %d", ucM);
            #endif
            return MT_ERR_OUTRNG;     
        }
        
        usLen += 1;

        // dBackHaveT
        psFmt13_f = &(psF->dBackHaveT);

        eRet = emt_trans_XXXX_XXXX(eTrans, &(psU->dBackHaveT), psFmt13_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df7() emt_trans_XXXX_XXXX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt13_f);
        
        // dBackHave
        for(i = 0; i < ucM; i++)
        {
            ++psFmt13_f;
            eRet = emt_trans_XXXX_XXXX(eTrans, &(psU->dBackHave[i]), psFmt13_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df7() emt_trans_XXXX_XXXX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt13_f) * ucM;

    }
    else if(MT_TRANS_U2F == eTrans)
    // 用户侧转为帧侧
    {
        // Td_d
        eRet = emtTrans_td_d(eTrans, &(psU->sTd_d), &(psF->sTd_d), &usTmp);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df7() emtTrans_td_d() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += usTmp;

        // sTime
        eRet = emt_trans_YYMMDDhhmm(eTrans, &(psU->sTime), &(psF->sTime));
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df7() emt_trans_YYMMDDhhmm() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += sizeof(sMtFmt15_f);

        // M
        ucM = psU->ucM;
        if(ucM < MT_M_MIN || ucM > MT_M_MAX)
        {
            #ifdef MT_DBG
            DEBUG("ucM out of range %d", ucM);
            #endif
            return MT_ERR_OUTRNG;     
        }
        
        usLen += 1;

        // dBackHaveT
        psFmt13_f = &(psF->dBackHaveT);

        eRet = emt_trans_XXXX_XXXX(eTrans, &(psU->dBackHaveT), psFmt13_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df7() emt_trans_XXXX_XXXX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt13_f);
        
        // dBackHave
        for(i = 0; i < ucM; i++)
        {
            ++psFmt13_f;
            eRet = emt_trans_XXXX_XXXX(eTrans, &(psU->dBackHave[i]), psFmt13_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df7() emt_trans_XXXX_XXXX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt13_f) * ucM;

    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0df7() para error!");
        #endif
        return MT_ERR_PARA;
    }

    // 计算在帧侧的字节长度
    *pusfLen = usLen;
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0df8
 功能描述  : F8：日冻结反向无功电能量（总、费率1～M）
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月30日 星期五
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0df8(eMtTrans eTrans, void* psUser, void* psFrame, UINT16* pusfLen)
{
    if(!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0df8() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    sMtAfn0dF8     *psU      = (sMtAfn0dF8*)psUser;
    sMtAfn0dF8_f   *psF      = (sMtAfn0dF8_f*)psFrame;
    eMtErr         eRet      = MT_OK;
    UINT8          ucM       = 0;
    UINT16         usLen     = 0;
    UINT16         usTmp     = 0;
    INT32          i         = 0;
    sMtFmt13_f    *psFmt13_f = NULL;
    
    // 帧侧转为用户侧
    if(MT_TRANS_F2U == eTrans)
    {
        // Td_d
        eRet = emtTrans_td_d(eTrans, &(psU->sTd_d), &(psF->sTd_d), &usTmp);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df8() emtTrans_td_d() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += usTmp;

        // sTime
        eRet = emt_trans_YYMMDDhhmm(eTrans, &(psU->sTime), &(psF->sTime));
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df8() emt_trans_YYMMDDhhmm() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += sizeof(sMtFmt15_f);

        // M
        ucM = psF->ucM;
        if(ucM < MT_M_MIN || ucM > MT_M_MAX)
        {
            #ifdef MT_DBG
            DEBUG("ucM out of range %d", ucM);
            #endif
            return MT_ERR_OUTRNG;     
        }
        
        usLen += 1;

        // dBackNoneT
        psFmt13_f = &(psF->dBackNoneT);

        eRet = emt_trans_XXXX_XXXX(eTrans, &(psU->dBackNoneT), psFmt13_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df8() emt_trans_XXXX_XXXX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt13_f);
        
        // dBackNone
        for(i = 0; i < ucM; i++)
        {
            ++psFmt13_f;
            eRet = emt_trans_XXXX_XXXX(eTrans, &(psU->dBackNone[i]), psFmt13_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df8() emt_trans_XXXX_XXXX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt13_f) * ucM;

    }
    else if(MT_TRANS_U2F == eTrans)
    // 用户侧转为帧侧
    {
        // Td_d
        eRet = emtTrans_td_d(eTrans, &(psU->sTd_d), &(psF->sTd_d), &usTmp);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df8() emtTrans_td_d() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += usTmp;

        // sTime
        eRet = emt_trans_YYMMDDhhmm(eTrans, &(psU->sTime), &(psF->sTime));
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df8() emt_trans_YYMMDDhhmm() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += sizeof(sMtFmt15_f);

        // M
        ucM = psU->ucM;
        if(ucM < MT_M_MIN || ucM > MT_M_MAX)
        {
            #ifdef MT_DBG
            DEBUG("ucM out of range %d", ucM);
            #endif
            return MT_ERR_OUTRNG;     
        }
        
        usLen += 1;

        // dBackNoneT
        psFmt13_f = &(psF->dBackNoneT);

        eRet = emt_trans_XXXX_XXXX(eTrans, &(psU->dBackNoneT), psFmt13_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df8() emt_trans_XXXX_XXXX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt13_f);
        
        // dBackNone
        for(i = 0; i < ucM; i++)
        {
            ++psFmt13_f;
            eRet = emt_trans_XXXX_XXXX(eTrans, &(psU->dBackNone[i]), psFmt13_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df8() emt_trans_XXXX_XXXX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt13_f) * ucM;

    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0df8() para error!");
        #endif
        return MT_ERR_PARA;
    }

    // 计算在帧侧的字节长度
    *pusfLen = usLen;
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0df9
 功能描述  : F9：抄表日冻结正向有/无功电能示值、一/四象限无功电能示值（总、费率1～M，1≤M≤12）
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月2日 星期一
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0df9(eMtTrans eTrans, void* psUser, void* psFrame, UINT16* pusfLen)
{
    eMtErr eRet = MT_OK;
    eRet =  emtTrans_afn0df1(eTrans, psUser, psFrame, pusfLen);
    return eRet;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0df10
 功能描述  : F10：抄表日冻结反向有/无功电能示值、二/三象限无功电能示值（总、费率1～M，1≤M≤12）
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月2日 星期一
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0df10(eMtTrans eTrans, void* psUser, void* psFrame, UINT16* pusfLen)
{
    eMtErr eRet = MT_OK;
    eRet =  emtTrans_afn0df2(eTrans, psUser, psFrame, pusfLen);
    return eRet;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0df11
 功能描述  : F11：抄表日冻结电能表正向有/无功最大需量及发生时间（总、费率1～M，1≤M≤12）
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月2日 星期一
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0df11(eMtTrans eTrans, void* psUser, void* psFrame, UINT16* pusfLen)
{
    eMtErr eRet = MT_OK;
    eRet =  emtTrans_afn0df3(eTrans, psUser, psFrame, pusfLen);
    return eRet;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0df12
 功能描述  : F12：抄表日冻结电能表反向有/无功最大需量及发生时间（总、费率1～M，1≤M≤12）
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月2日 星期一
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0df12(eMtTrans eTrans, void* psUser, void* psFrame, UINT16* pusfLen)
{
    eMtErr eRet = MT_OK;
    eRet =  emtTrans_afn0df4(eTrans, psUser, psFrame, pusfLen);
    return eRet;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0df17
 功能描述  : F17：月冻结正向有/无功电能示值、一/四象限无功电能示值（总、费率1～M，1≤M≤12）
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0df17(eMtTrans eTrans, void* psUser, void* psFrame, UINT16* pusfLen)
{
    if(!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0df17() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    sMtAfn0dF17    *psU       = (sMtAfn0dF17*)psUser;
    sMtAfn0dF17_f  *psF       = (sMtAfn0dF17_f*)psFrame;
    eMtErr         eRet       = MT_OK;
    UINT8          ucM        = 0;
    UINT16         usLen      = 0;
    UINT16         usTmp      = 0;
    INT32          i          = 0;
    UINT8          *pTmp      = NULL;
    sMtFmt14_f    *psFmt14_f  = NULL;
    sMtFmt11_f    *psFmt11_f  = NULL;
    
    // 帧侧转为用户侧
    if(MT_TRANS_F2U == eTrans)
    {
        // Td_d
        eRet = emtTrans_td_m(eTrans, &(psU->sTd_m), &(psF->sTd_m), &usTmp);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df17() emtTrans_td_m() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += usTmp;

        // sTime
        eRet = emt_trans_YYMMDDhhmm(eTrans, &(psU->sTime), &(psF->sTime));
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df17() emt_trans_YYMMDDhhmm() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += sizeof(sMtFmt15_f);

        // M
        ucM = psF->ucM;
        if(ucM < MT_M_MIN || ucM > MT_M_MAX)
        {
            #ifdef MT_DBG
            DEBUG("ucM out of range %d", ucM);
            #endif
            return MT_ERR_OUTRNG;     
        }
        
        usLen += 1;

        // psFmt14_f
        pTmp      = (UINT8*)(psF->ucPower);
        psFmt14_f = (sMtFmt14_f*)pTmp;

        // dFrthHavePowerT
        eRet = emt_trans_XXXXXX_XXXX(eTrans, &(psU->dFrthHavePowerT), psFmt14_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df17() emt_trans_XXXXXX_XXXX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt14_f);
        
        // dFrthHavePower
        for(i = 0; i < ucM; i++)
        {
            psFmt14_f = (sMtFmt14_f*)(pTmp + (i+1)*sizeof(sMtFmt14_f));
            eRet = emt_trans_XXXXXX_XXXX(eTrans, &(psU->dFrthHavePower[i]), psFmt14_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df17() emt_trans_XXXXXX_XXXX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt14_f) * ucM;

        // dFrthNonePowerT
        pTmp      = (UINT8 *)(psFmt14_f + 1);
        psFmt11_f = (sMtFmt11_f*)pTmp;
        eRet = emt_trans_XXXXXX_XX(eTrans, &(psU->dFrthNonePowerT), psFmt11_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df17() emt_trans_XXXXXX_XX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt11_f);

        // dFrthNonePower
        for(i = 0; i < ucM; i++)
        {
            psFmt11_f = (sMtFmt11_f*)(pTmp + (i+1)*sizeof(sMtFmt11_f));
            eRet = emt_trans_XXXXXX_XX(eTrans, &(psU->dFrthNonePower[i]), psFmt11_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df17() emt_trans_XXXXXX_XX() error %d", eRet);
                #endif
                return eRet;
            }
        }
        
        usLen += sizeof(sMtFmt11_f) * ucM;

        // dNonePowerP1T
        psFmt11_f += 1;
        eRet = emt_trans_XXXXXX_XX(eTrans, &(psU->dNonePowerP1T), psFmt11_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df17() emt_trans_XXXXXX_XX() error %d", eRet);
            #endif
            return eRet;
        }

         usLen += sizeof(sMtFmt11_f);

        // dFrthNonePower
        for(i = 0; i < ucM; i++)
        {
            psFmt11_f = (sMtFmt11_f*)(pTmp + (i+1)*sizeof(sMtFmt11_f));
            eRet = emt_trans_XXXXXX_XX(eTrans, &(psU->dNonePowerP1[i]), psFmt11_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df17() emt_trans_XXXXXX_XX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt11_f) * ucM;

        // dNonePowerP1T
        psFmt11_f += 1;
        eRet = emt_trans_XXXXXX_XX(eTrans, &(psU->dNonePowerP4T), psFmt11_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df17() emt_trans_XXXXXX_XX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt11_f);

        // dFrthNonePower
        for(i = 0; i < ucM; i++)
        {
            psFmt11_f = (sMtFmt11_f*)(pTmp + (i+1)*sizeof(sMtFmt11_f));
            eRet = emt_trans_XXXXXX_XX(eTrans, &(psU->dNonePowerP4[i]), psFmt11_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df17() emt_trans_XXXXXX_XX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt11_f) * ucM;

    }
    else if(MT_TRANS_U2F == eTrans)
    // 用户侧转为帧侧
    {
        // sTd_m
        eRet = emtTrans_td_m(eTrans, &(psU->sTd_m), &(psF->sTd_m), &usTmp);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df17() emtTrans_td_m() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += usTmp;

        // sTime
        eRet = emt_trans_YYMMDDhhmm(eTrans, &(psU->sTime), &(psF->sTime));
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df17() emt_trans_YYMMDDhhmm() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += sizeof(sMtFmt15_f);
        
        // M
        ucM = psU->ucM;
        if(ucM < MT_M_MIN || ucM > MT_M_MAX)
        {
            #ifdef MT_DBG
            DEBUG("ucM out of range %d", ucM);
            #endif
            return MT_ERR_OUTRNG;     
        }
        
        usLen += 1;

        // psFmt14_f
        pTmp      = (UINT8*)(psF->ucPower);
        psFmt14_f = (sMtFmt14_f*)pTmp;

        // dFrthHavePowerT
        eRet = emt_trans_XXXXXX_XXXX(eTrans, &(psU->dFrthHavePowerT), psFmt14_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df17() emt_trans_XXXXXX_XXXX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt14_f);
        
        // dFrthHavePower
        for(i = 0; i < ucM; i++)
        {
            psFmt14_f = (sMtFmt14_f*)(pTmp + (i+1)*sizeof(sMtFmt14_f));
            eRet = emt_trans_XXXXXX_XXXX(eTrans, &(psU->dFrthHavePower[i]), psFmt14_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df17() emt_trans_XXXXXX_XXXX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt14_f) * ucM;

        // dFrthNonePowerT
        pTmp      = (UINT8 *)(psFmt14_f + 1);
        psFmt11_f = (sMtFmt11_f*)pTmp;
        eRet = emt_trans_XXXXXX_XX(eTrans, &(psU->dFrthNonePowerT), psFmt11_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df17() emt_trans_XXXXXX_XX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt11_f);

        // dFrthNonePower
        for(i = 0; i < ucM; i++)
        {
            psFmt11_f = (sMtFmt11_f*)(pTmp + (i+1)*sizeof(sMtFmt11_f));
            eRet = emt_trans_XXXXXX_XX(eTrans, &(psU->dFrthNonePower[i]), psFmt11_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df17() emt_trans_XXXXXX_XX() error %d", eRet);
                #endif
                return eRet;
            }
        }
        
        usLen += sizeof(sMtFmt11_f) * ucM;

        // dNonePowerP1T
        psFmt11_f += 1;
        eRet = emt_trans_XXXXXX_XX(eTrans, &(psU->dNonePowerP1T), psFmt11_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df17() emt_trans_XXXXXX_XX() error %d", eRet);
            #endif
            return eRet;
        }

         usLen += sizeof(sMtFmt11_f);

        // dFrthNonePower
        for(i = 0; i < ucM; i++)
        {
            psFmt11_f = (sMtFmt11_f*)(pTmp + (i+1)*sizeof(sMtFmt11_f));
            eRet = emt_trans_XXXXXX_XX(eTrans, &(psU->dNonePowerP1[i]), psFmt11_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df17() emt_trans_XXXXXX_XX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt11_f) * ucM;

        // dNonePowerP1T
        psFmt11_f += 1;
        eRet = emt_trans_XXXXXX_XX(eTrans, &(psU->dNonePowerP4T), psFmt11_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df17() emt_trans_XXXXXX_XX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt11_f);

        // dFrthNonePower
        for(i = 0; i < ucM; i++)
        {
            psFmt11_f = (sMtFmt11_f*)(pTmp + (i+1)*sizeof(sMtFmt11_f));
            eRet = emt_trans_XXXXXX_XX(eTrans, &(psU->dNonePowerP4[i]), psFmt11_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df17() emt_trans_XXXXXX_XX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt11_f) * ucM;
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0df17() para error!");
        #endif
        return MT_ERR_PARA;
    }

    // 计算在帧侧的字节长度
    *pusfLen = usLen;
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0df18
 功能描述  : F18：月冻结反向有/无功电能示值、二/三象限无功电能示值（总、费率1～M，1≤M≤12）
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月29日 星期四
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0df18(eMtTrans eTrans, void* psUser, void* psFrame, UINT16* pusfLen)
{
    if(!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0df18() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    sMtAfn0dF18    *psU      = (sMtAfn0dF18*)psUser;
    sMtAfn0dF18_f  *psF      = (sMtAfn0dF18_f*)psFrame;
    eMtErr         eRet      = MT_OK;
    UINT8          ucM       = 0;
    UINT16         usLen     = 0;
    UINT16         usTmp     = 0;
    INT32          i         = 0;
    UINT8          *pTmp     = NULL;
    sMtFmt14_f    *psFmt14_f = NULL;
    sMtFmt11_f    *psFmt11_f = NULL;
    
    // 帧侧转为用户侧
    if(MT_TRANS_F2U == eTrans)
    {
        // Td_m
        eRet = emtTrans_td_m(eTrans, &(psU->sTd_m), &(psF->sTd_m), &usTmp);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df18() emtTrans_td_m() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += usTmp;

        // sTime
        eRet = emt_trans_YYMMDDhhmm(eTrans, &(psU->sTime), &(psF->sTime));
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df18() emt_trans_YYMMDDhhmm() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += sizeof(sMtFmt15_f);

        // M
        ucM = psF->ucM;
        if(ucM < MT_M_MIN || ucM > MT_M_MAX)
        {
            #ifdef MT_DBG
            DEBUG("ucM out of range %d", ucM);
            #endif
            return MT_ERR_OUTRNG;     
        }
        
        usLen += 1;

        // psFmt14_f
        pTmp      = (UINT8*)(psF->ucPower);
        psFmt14_f = (sMtFmt14_f*)pTmp;

        // dBackHavePowerT
        eRet = emt_trans_XXXXXX_XXXX(eTrans, &(psU->dBackHavePowerT), psFmt14_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df18() emt_trans_XXXXXX_XXXX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt14_f);
        
        // dBackHavePower
        for(i = 0; i < ucM; i++)
        {
            psFmt14_f = (sMtFmt14_f*)(pTmp + (i+1)*sizeof(sMtFmt14_f));
            eRet = emt_trans_XXXXXX_XXXX(eTrans, &(psU->dBackHavePower[i]), psFmt14_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df18() emt_trans_XXXXXX_XXXX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt14_f) * ucM;

        // dBackNonePowerT
        pTmp      = (UINT8 *)(psFmt14_f + 1);
        psFmt11_f = (sMtFmt11_f*)pTmp;
        eRet = emt_trans_XXXXXX_XX(eTrans, &(psU->dBackNonePowerT), psFmt11_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df18() emt_trans_XXXXXX_XX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt11_f);

        // dBackNonePower
        for(i = 0; i < ucM; i++)
        {
            psFmt11_f = (sMtFmt11_f*)(pTmp + (i+1)*sizeof(sMtFmt11_f));
            eRet = emt_trans_XXXXXX_XX(eTrans, &(psU->dBackNonePower[i]), psFmt11_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df18() emt_trans_XXXXXX_XX() error %d", eRet);
                #endif
                return eRet;
            }
        }
        
        usLen += sizeof(sMtFmt11_f) * ucM;

        // dNonePowerP2T
        psFmt11_f += 1;
        eRet = emt_trans_XXXXXX_XX(eTrans, &(psU->dNonePowerP2T), psFmt11_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df18() emt_trans_XXXXXX_XX() error %d", eRet);
            #endif
            return eRet;
        }

         usLen += sizeof(sMtFmt11_f);

        // dNonePowerP2
        for(i = 0; i < ucM; i++)
        {
            psFmt11_f = (sMtFmt11_f*)(pTmp + (i+1)*sizeof(sMtFmt11_f));
            eRet = emt_trans_XXXXXX_XX(eTrans, &(psU->dNonePowerP2[i]), psFmt11_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df18() emt_trans_XXXXXX_XX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt11_f) * ucM;

        // dNonePowerP3T
        psFmt11_f += 1;
        eRet = emt_trans_XXXXXX_XX(eTrans, &(psU->dNonePowerP3T), psFmt11_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df18() emt_trans_XXXXXX_XX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt11_f);

        // dNonePowerP3
        for(i = 0; i < ucM; i++)
        {
            psFmt11_f = (sMtFmt11_f*)(pTmp + (i+1)*sizeof(sMtFmt11_f));
            eRet = emt_trans_XXXXXX_XX(eTrans, &(psU->dNonePowerP3[i]), psFmt11_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df18() emt_trans_XXXXXX_XX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt11_f) * ucM;
    }
    else if(MT_TRANS_U2F == eTrans)
    // 用户侧转为帧侧
    {
        // Td_m
        eRet = emtTrans_td_m(eTrans, &(psU->sTd_m), &(psF->sTd_m), &usTmp);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df18() emtTrans_td_d() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += usTmp;

        // sTime
        eRet = emt_trans_YYMMDDhhmm(eTrans, &(psU->sTime), &(psF->sTime));
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df18() emt_trans_YYMMDDhhmm() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += sizeof(sMtFmt15_f);
        
        // M
        ucM = psU->ucM;
        if(ucM < MT_M_MIN || ucM > MT_M_MAX)
        {
            #ifdef MT_DBG
            DEBUG("ucM out of range %d", ucM);
            #endif
            return MT_ERR_OUTRNG;     
        }
        
        usLen += 1;

        // psFmt14_f
        pTmp      = (UINT8*)(psF->ucPower);
        psFmt14_f = (sMtFmt14_f*)pTmp;

        // dBackHavePowerT
        eRet = emt_trans_XXXXXX_XXXX(eTrans, &(psU->dBackHavePowerT), psFmt14_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df18() emt_trans_XXXXXX_XXXX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt14_f);
        
        // dBackHavePower
        for(i = 0; i < ucM; i++)
        {
            psFmt14_f = (sMtFmt14_f*)(pTmp + (i+1)*sizeof(sMtFmt14_f));
            eRet = emt_trans_XXXXXX_XXXX(eTrans, &(psU->dBackHavePower[i]), psFmt14_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df18() emt_trans_XXXXXX_XXXX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt14_f) * ucM;

        // dBackNonePowerT
        pTmp      = (UINT8 *)(psFmt14_f + 1);
        psFmt11_f = (sMtFmt11_f*)pTmp;
        eRet = emt_trans_XXXXXX_XX(eTrans, &(psU->dBackNonePowerT), psFmt11_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df18() emt_trans_XXXXXX_XX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt11_f);

        // dBackNonePower
        for(i = 0; i < ucM; i++)
        {
            psFmt11_f = (sMtFmt11_f*)(pTmp + (i+1)*sizeof(sMtFmt11_f));
            eRet = emt_trans_XXXXXX_XX(eTrans, &(psU->dBackNonePower[i]), psFmt11_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df18() emt_trans_XXXXXX_XX() error %d", eRet);
                #endif
                return eRet;
            }
        }
        
        usLen += sizeof(sMtFmt11_f) * ucM;

        // dNonePowerP2T
        psFmt11_f += 1;
        eRet = emt_trans_XXXXXX_XX(eTrans, &(psU->dNonePowerP2T), psFmt11_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df18() emt_trans_XXXXXX_XX() error %d", eRet);
            #endif
            return eRet;
        }

         usLen += sizeof(sMtFmt11_f);

        // dNonePowerP2
        for(i = 0; i < ucM; i++)
        {
            psFmt11_f = (sMtFmt11_f*)(pTmp + (i+1)*sizeof(sMtFmt11_f));
            eRet = emt_trans_XXXXXX_XX(eTrans, &(psU->dNonePowerP2[i]), psFmt11_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df18() emt_trans_XXXXXX_XX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt11_f) * ucM;

        // dNonePowerP3T
        psFmt11_f += 1;
        eRet = emt_trans_XXXXXX_XX(eTrans, &(psU->dNonePowerP3T), psFmt11_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df18() emt_trans_XXXXXX_XX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt11_f);

        // dNonePowerP3
        for(i = 0; i < ucM; i++)
        {
            psFmt11_f = (sMtFmt11_f*)(pTmp + (i+1)*sizeof(sMtFmt11_f));
            eRet = emt_trans_XXXXXX_XX(eTrans, &(psU->dNonePowerP3[i]), psFmt11_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df18() emt_trans_XXXXXX_XX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt11_f) * ucM;
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0df18() para error!");
        #endif
        return MT_ERR_PARA;
    }

    // 计算在帧侧的字节长度
    *pusfLen = usLen;
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0df19
 功能描述  : F3：日冻结正向有/无功最大需量及发生时间（总、费率1～M，1≤M≤12）
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月30日 星期五
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0df19(eMtTrans eTrans, void* psUser, void* psFrame, UINT16* pusfLen)
{
    if(!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0df19() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    sMtAfn0dF19    *psU      = (sMtAfn0dF19*)psUser;
    sMtAfn0dF19_f  *psF      = (sMtAfn0dF19_f*)psFrame;
    eMtErr         eRet      = MT_OK;
    UINT8          ucM       = 0;
    UINT16         usLen     = 0;
    UINT16         usTmp     = 0;
    INT32          i         = 0;
    UINT8          *pTmp     = NULL;
    sMtFmt23_f    *psFmt23_f = NULL;
    sMtFmt17_f    *psFmt17_f = NULL;
    
    // 帧侧转为用户侧
    if(MT_TRANS_F2U == eTrans)
    {
        // Td_m
        eRet = emtTrans_td_m(eTrans, &(psU->sTd_m), &(psF->sTd_m), &usTmp);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df19() emtTrans_td_m() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += usTmp;

        // sTime
        eRet = emt_trans_YYMMDDhhmm(eTrans, &(psU->sTime), &(psF->sTime));
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df19() emt_trans_YYMMDDhhmm() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += sizeof(sMtFmt15_f);

        // M
        ucM = psF->ucM;
        if(ucM < MT_M_MIN || ucM > MT_M_MAX)
        {
            #ifdef MT_DBG
            DEBUG("ucM out of range %d", ucM);
            #endif
            return MT_ERR_OUTRNG;     
        }
        
        usLen += 1;

        // fFrthHaveT
        pTmp      = (UINT8*)(psF->ucDemand);
        psFmt23_f = (sMtFmt23_f*)pTmp;

        // fFrthHaveT
        eRet = emt_trans_XX_XXXX(eTrans, &(psU->fFrthHaveT), psFmt23_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df19() emt_trans_XX_XXXX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt23_f);
        
        // fFrthHave
        for(i = 0; i < ucM; i++)
        {
            psFmt23_f = (sMtFmt23_f*)(pTmp + (i+1) * sizeof(sMtFmt23_f));
            eRet = emt_trans_XX_XXXX(eTrans, &(psU->fFrthHave[i]), psFmt23_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df19() emt_trans_XX_XXXX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt23_f) * ucM;

        // sTimeFrthHaveT
        pTmp      = (UINT8 *)(psFmt23_f + 1);
        psFmt17_f = (sMtFmt17_f*)pTmp;
        eRet = emt_trans_MMDDHHmm(eTrans, &(psU->sTimeFrthHaveT), psFmt17_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df19() emt_trans_MMDDHHmm() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt17_f);

        // sTimeFrthHave
        for(i = 0; i < ucM; i++)
        {
            psFmt17_f = (sMtFmt17_f*)(pTmp + (i+1)*sizeof(sMtFmt17_f));
            eRet = emt_trans_MMDDHHmm(eTrans, &(psU->sTimeFrthHave[i]), psFmt17_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df19() emt_trans_MMDDHHmm() error %d", eRet);
                #endif
                return eRet;
            }
        }
        
        usLen += sizeof(sMtFmt17_f) * ucM;

        // fFrthNoneT
        psFmt17_f += 1;
        psFmt23_f = (sMtFmt23_f*)psFmt17_f;
        eRet = emt_trans_XX_XXXX(eTrans, &(psU->fFrthNoneT), psFmt23_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df19() emt_trans_XX_XXXX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt23_f);
        
        // fFrthNone
        for(i = 0; i < ucM; i++)
        {
            psFmt23_f = (sMtFmt23_f*)(pTmp + (i+1) * sizeof(sMtFmt23_f));
            eRet = emt_trans_XX_XXXX(eTrans, &(psU->fFrthNone[i]), psFmt23_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df19() emt_trans_XX_XXXX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt23_f) * ucM;

        // sTimeFrthNoneT
        pTmp      = (UINT8 *)(psFmt23_f + 1);
        psFmt17_f = (sMtFmt17_f*)pTmp;
        eRet = emt_trans_MMDDHHmm(eTrans, &(psU->sTimeFrthNoneT), psFmt17_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df19() emt_trans_MMDDHHmm() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt17_f);

        // sTimeFrthNone
        for(i = 0; i < ucM; i++)
        {
            psFmt17_f = (sMtFmt17_f*)(pTmp + (i+1)*sizeof(sMtFmt17_f));
            eRet = emt_trans_MMDDHHmm(eTrans, &(psU->sTimeFrthNone[i]), psFmt17_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df19() emt_trans_MMDDHHmm() error %d", eRet);
                #endif
                return eRet;
            }
        }
        
        usLen += sizeof(sMtFmt17_f) * ucM;
        
    }
    else if(MT_TRANS_U2F == eTrans)
    // 用户侧转为帧侧
    {
        // Td_d
        eRet = emtTrans_td_m(eTrans, &(psU->sTd_m), &(psF->sTd_m), &usTmp);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df19() emtTrans_td_m() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += usTmp;

        // sTime
        eRet = emt_trans_YYMMDDhhmm(eTrans, &(psU->sTime), &(psF->sTime));
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df19() emt_trans_YYMMDDhhmm() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += sizeof(sMtFmt15_f);

        // M
        ucM = psU->ucM;
        if(ucM < MT_M_MIN || ucM > MT_M_MAX)
        {
            #ifdef MT_DBG
            DEBUG("ucM out of range %d", ucM);
            #endif
            return MT_ERR_OUTRNG;     
        }
        
        usLen += 1;

        // fFrthHaveT
        pTmp      = (UINT8*)(psF->ucDemand);
        psFmt23_f = (sMtFmt23_f*)pTmp;

        // fFrthHaveT
        eRet = emt_trans_XX_XXXX(eTrans, &(psU->fFrthHaveT), psFmt23_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df19() emt_trans_XX_XXXX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt23_f);
        
        // fFrthHave
        for(i = 0; i < ucM; i++)
        {
            psFmt23_f = (sMtFmt23_f*)(pTmp + (i+1) * sizeof(sMtFmt23_f));
            eRet = emt_trans_XX_XXXX(eTrans, &(psU->fFrthHave[i]), psFmt23_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df19() emt_trans_XX_XXXX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt23_f) * ucM;

        // sTimeFrthHaveT
        pTmp      = (UINT8 *)(psFmt23_f + 1);
        psFmt17_f = (sMtFmt17_f*)pTmp;
        eRet = emt_trans_MMDDHHmm(eTrans, &(psU->sTimeFrthHaveT), psFmt17_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df19() emt_trans_MMDDHHmm() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt17_f);

        // sTimeFrthHave
        for(i = 0; i < ucM; i++)
        {
            psFmt17_f = (sMtFmt17_f*)(pTmp + (i+1)*sizeof(sMtFmt17_f));
            eRet = emt_trans_MMDDHHmm(eTrans, &(psU->sTimeFrthHave[i]), psFmt17_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df19() emt_trans_MMDDHHmm() error %d", eRet);
                #endif
                return eRet;
            }
        }
        
        usLen += sizeof(sMtFmt17_f) * ucM;

        // fFrthNoneT
        psFmt17_f += 1;
        psFmt23_f = (sMtFmt23_f*)psFmt17_f;
        eRet = emt_trans_XX_XXXX(eTrans, &(psU->fFrthNoneT), psFmt23_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df19() emt_trans_XX_XXXX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt23_f);
        
        // fFrthNone
        for(i = 0; i < ucM; i++)
        {
            psFmt23_f = (sMtFmt23_f*)(pTmp + (i+1) * sizeof(sMtFmt23_f));
            eRet = emt_trans_XX_XXXX(eTrans, &(psU->fFrthNone[i]), psFmt23_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df19() emt_trans_XX_XXXX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt23_f) * ucM;

        // sTimeFrthNoneT
        pTmp      = (UINT8 *)(psFmt23_f + 1);
        psFmt17_f = (sMtFmt17_f*)pTmp;
        eRet = emt_trans_MMDDHHmm(eTrans, &(psU->sTimeFrthNoneT), psFmt17_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df19() emt_trans_MMDDHHmm() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt17_f);

        // sTimeFrthNone
        for(i = 0; i < ucM; i++)
        {
            psFmt17_f = (sMtFmt17_f*)(pTmp + (i+1)*sizeof(sMtFmt17_f));
            eRet = emt_trans_MMDDHHmm(eTrans, &(psU->sTimeFrthNone[i]), psFmt17_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df19() emt_trans_MMDDHHmm() error %d", eRet);
                #endif
                return eRet;
            }
        }
        
        usLen += sizeof(sMtFmt17_f) * ucM;
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0df19() para error!");
        #endif
        return MT_ERR_PARA;
    }

    // 计算在帧侧的字节长度
    *pusfLen = usLen;
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0df20
 功能描述  : F20：月冻结反向有/无功最大需量及发生时间（总、费率1～M，1≤M≤12）
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月30日 星期五
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0df20(eMtTrans eTrans, void* psUser, void* psFrame, UINT16* pusfLen)
{
    if(!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0df20() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    sMtAfn0dF20     *psU      = (sMtAfn0dF20*)psUser;
    sMtAfn0dF20_f   *psF      = (sMtAfn0dF20_f*)psFrame;
    eMtErr         eRet      = MT_OK;
    UINT8          ucM       = 0;
    UINT16         usLen     = 0;
    UINT16         usTmp     = 0;
    INT32          i         = 0;
    UINT8          *pTmp     = NULL;
    sMtFmt23_f    *psFmt23_f = NULL;
    sMtFmt17_f    *psFmt17_f = NULL;
    
    // 帧侧转为用户侧
    if(MT_TRANS_F2U == eTrans)
    {
        // Td_m
        eRet = emtTrans_td_m(eTrans, &(psU->sTd_m), &(psF->sTd_m), &usTmp);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df20() emtTrans_td_m() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += usTmp;

        // sTime
        eRet = emt_trans_YYMMDDhhmm(eTrans, &(psU->sTime), &(psF->sTime));
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df20() emt_trans_YYMMDDhhmm() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += sizeof(sMtFmt15_f);

        // M
        ucM = psF->ucM;
        if(ucM < MT_M_MIN || ucM > MT_M_MAX)
        {
            #ifdef MT_DBG
            DEBUG("ucM out of range %d", ucM);
            #endif
            return MT_ERR_OUTRNG;     
        }
        
        usLen += 1;

        // fBackHaveT
        pTmp      = (UINT8*)(psF->ucDemand);
        psFmt23_f = (sMtFmt23_f*)pTmp;

        // fBackHaveT
        eRet = emt_trans_XX_XXXX(eTrans, &(psU->fBackHaveT), psFmt23_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df20() emt_trans_XX_XXXX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt23_f);
        
        // fBackHave
        for(i = 0; i < ucM; i++)
        {
            psFmt23_f = (sMtFmt23_f*)(pTmp + (i+1) * sizeof(sMtFmt23_f));
            eRet = emt_trans_XX_XXXX(eTrans, &(psU->fBackHave[i]), psFmt23_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df20() emt_trans_XX_XXXX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt23_f) * ucM;

        // sTimeBackHaveT
        pTmp      = (UINT8 *)(psFmt23_f + 1);
        psFmt17_f = (sMtFmt17_f*)pTmp;
        eRet = emt_trans_MMDDHHmm(eTrans, &(psU->sTimeBackHaveT), psFmt17_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df20() emt_trans_MMDDHHmm() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt17_f);

        // sTimeBackHave
        for(i = 0; i < ucM; i++)
        {
            psFmt17_f = (sMtFmt17_f*)(pTmp + (i+1)*sizeof(sMtFmt17_f));
            eRet = emt_trans_MMDDHHmm(eTrans, &(psU->sTimeBackHave[i]), psFmt17_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df20() emt_trans_MMDDHHmm() error %d", eRet);
                #endif
                return eRet;
            }
        }
        
        usLen += sizeof(sMtFmt17_f) * ucM;

        // fBackNoneT
        psFmt17_f += 1;
        psFmt23_f = (sMtFmt23_f*)psFmt17_f;
        eRet = emt_trans_XX_XXXX(eTrans, &(psU->fBackNoneT), psFmt23_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df20() emt_trans_XX_XXXX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt23_f);
        
        // fBackNone
        for(i = 0; i < ucM; i++)
        {
            psFmt23_f = (sMtFmt23_f*)(pTmp + (i+1) * sizeof(sMtFmt23_f));
            eRet = emt_trans_XX_XXXX(eTrans, &(psU->fBackNone[i]), psFmt23_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df20() emt_trans_XX_XXXX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt23_f) * ucM;

        // sTimeBackNoneT
        pTmp      = (UINT8 *)(psFmt23_f + 1);
        psFmt17_f = (sMtFmt17_f*)pTmp;
        eRet = emt_trans_MMDDHHmm(eTrans, &(psU->sTimeBackNoneT), psFmt17_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df20() emt_trans_MMDDHHmm() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt17_f);

        // sTimeBackNone
        for(i = 0; i < ucM; i++)
        {
            psFmt17_f = (sMtFmt17_f*)(pTmp + (i+1)*sizeof(sMtFmt17_f));
            eRet = emt_trans_MMDDHHmm(eTrans, &(psU->sTimeBackNone[i]), psFmt17_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df20() emt_trans_MMDDHHmm() error %d", eRet);
                #endif
                return eRet;
            }
        }
        
        usLen += sizeof(sMtFmt17_f) * ucM;
        
    }
    else if(MT_TRANS_U2F == eTrans)
    // 用户侧转为帧侧
    {
        // Td_m
        eRet = emtTrans_td_m(eTrans, &(psU->sTd_m), &(psF->sTd_m), &usTmp);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df20() emtTrans_td_m() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += usTmp;

        // sTime
        eRet = emt_trans_YYMMDDhhmm(eTrans, &(psU->sTime), &(psF->sTime));
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df20() emt_trans_YYMMDDhhmm() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += sizeof(sMtFmt15_f);

        // M
        ucM = psU->ucM;
        if(ucM < MT_M_MIN || ucM > MT_M_MAX)
        {
            #ifdef MT_DBG
            DEBUG("ucM out of range %d", ucM);
            #endif
            return MT_ERR_OUTRNG;     
        }
        
        usLen += 1;

        // fBackHaveT
        pTmp      = (UINT8*)(psF->ucDemand);
        psFmt23_f = (sMtFmt23_f*)pTmp;

        // fBackHaveT
        eRet = emt_trans_XX_XXXX(eTrans, &(psU->fBackHaveT), psFmt23_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df20() emt_trans_XX_XXXX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt23_f);
        
        // fBackHave
        for(i = 0; i < ucM; i++)
        {
            psFmt23_f = (sMtFmt23_f*)(pTmp + (i+1) * sizeof(sMtFmt23_f));
            eRet = emt_trans_XX_XXXX(eTrans, &(psU->fBackHave[i]), psFmt23_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df20() emt_trans_XX_XXXX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt23_f) * ucM;

        // sTimeBackHaveT
        pTmp      = (UINT8 *)(psFmt23_f + 1);
        psFmt17_f = (sMtFmt17_f*)pTmp;
        eRet = emt_trans_MMDDHHmm(eTrans, &(psU->sTimeBackHaveT), psFmt17_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df20() emt_trans_MMDDHHmm() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt17_f);

        // sTimeBackHave
        for(i = 0; i < ucM; i++)
        {
            psFmt17_f = (sMtFmt17_f*)(pTmp + (i+1)*sizeof(sMtFmt17_f));
            eRet = emt_trans_MMDDHHmm(eTrans, &(psU->sTimeBackHave[i]), psFmt17_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df20() emt_trans_MMDDHHmm() error %d", eRet);
                #endif
                return eRet;
            }
        }
        
        usLen += sizeof(sMtFmt17_f) * ucM;

        // fBackNoneT
        psFmt17_f += 1;
        psFmt23_f = (sMtFmt23_f*)psFmt17_f;
        eRet = emt_trans_XX_XXXX(eTrans, &(psU->fBackNoneT), psFmt23_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df20() emt_trans_XX_XXXX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt23_f);
        
        // fBackNone
        for(i = 0; i < ucM; i++)
        {
            psFmt23_f = (sMtFmt23_f*)(pTmp + (i+1) * sizeof(sMtFmt23_f));
            eRet = emt_trans_XX_XXXX(eTrans, &(psU->fBackNone[i]), psFmt23_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df20() emt_trans_XX_XXXX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt23_f) * ucM;

        // sTimeBackNoneT
        pTmp      = (UINT8 *)(psFmt23_f + 1);
        psFmt17_f = (sMtFmt17_f*)pTmp;
        eRet = emt_trans_MMDDHHmm(eTrans, &(psU->sTimeBackNoneT), psFmt17_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df20() emt_trans_MMDDHHmm() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt17_f);

        // sTimeBackNone
        for(i = 0; i < ucM; i++)
        {
            psFmt17_f = (sMtFmt17_f*)(pTmp + (i+1)*sizeof(sMtFmt17_f));
            eRet = emt_trans_MMDDHHmm(eTrans, &(psU->sTimeBackNone[i]), psFmt17_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df20() emt_trans_MMDDHHmm() error %d", eRet);
                #endif
                return eRet;
            }
        }
        
        usLen += sizeof(sMtFmt17_f) * ucM;      
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0df20() para error!");
        #endif
        return MT_ERR_PARA;
    }

    // 计算在帧侧的字节长度
    *pusfLen = usLen;
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0df21
 功能描述  : F21：月冻结正向有功电能量（总、费率1～M）
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月30日 星期五
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0df21(eMtTrans eTrans, void* psUser, void* psFrame, UINT16* pusfLen)
{
    if(!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0df21() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    sMtAfn0dF21    *psU      = (sMtAfn0dF21*)psUser;
    sMtAfn0dF21_f  *psF      = (sMtAfn0dF21_f*)psFrame;
    eMtErr         eRet      = MT_OK;
    UINT8          ucM       = 0;
    UINT16         usLen     = 0;
    UINT16         usTmp     = 0;
    INT32          i         = 0;
    sMtFmt13_f    *psFmt13_f = NULL;
    
    // 帧侧转为用户侧
    if(MT_TRANS_F2U == eTrans)
    {
        // Td_m
        eRet = emtTrans_td_m(eTrans, &(psU->sTd_m), &(psF->sTd_m), &usTmp);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df21() emtTrans_td_m() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += usTmp;

        // sTime
        eRet = emt_trans_YYMMDDhhmm(eTrans, &(psU->sTime), &(psF->sTime));
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df21() emt_trans_YYMMDDhhmm() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += sizeof(sMtFmt15_f);

        // M
        ucM = psF->ucM;
        if(ucM < MT_M_MIN || ucM > MT_M_MAX)
        {
            #ifdef MT_DBG
            DEBUG("ucM out of range %d", ucM);
            #endif
            return MT_ERR_OUTRNG;     
        }
        
        usLen += 1;

        // dFrthHaveT
        psFmt13_f = &(psF->dFrthHaveT);

        eRet = emt_trans_XXXX_XXXX(eTrans, &(psU->dFrthHaveT), psFmt13_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df21() emt_trans_XXXX_XXXX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt13_f);
        
        // dFrthHave
        for(i = 0; i < ucM; i++)
        {
            ++psFmt13_f;
            eRet = emt_trans_XXXX_XXXX(eTrans, &(psU->dFrthHave[i]), psFmt13_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df21() emt_trans_XXXX_XXXX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt13_f) * ucM;
    }
    else if(MT_TRANS_U2F == eTrans)
    // 用户侧转为帧侧
    {
        // Td_m
        eRet = emtTrans_td_m(eTrans, &(psU->sTd_m), &(psF->sTd_m), &usTmp);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df21() emtTrans_td_m() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += usTmp;

        // sTime
        eRet = emt_trans_YYMMDDhhmm(eTrans, &(psU->sTime), &(psF->sTime));
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df21() emt_trans_YYMMDDhhmm() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += sizeof(sMtFmt15_f);

        // M
        ucM = psU->ucM;
        if(ucM < MT_M_MIN || ucM > MT_M_MAX)
        {
            #ifdef MT_DBG
            DEBUG("ucM out of range %d", ucM);
            #endif
            return MT_ERR_OUTRNG;     
        }
        
        usLen += 1;

        // dFrthHaveT
        psFmt13_f = &(psF->dFrthHaveT);

        eRet = emt_trans_XXXX_XXXX(eTrans, &(psU->dFrthHaveT), psFmt13_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df21() emt_trans_XXXX_XXXX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt13_f);
        
        // dFrthHave
        for(i = 0; i < ucM; i++)
        {
            ++psFmt13_f;
            eRet = emt_trans_XXXX_XXXX(eTrans, &(psU->dFrthHave[i]), psFmt13_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df21() emt_trans_XXXX_XXXX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt13_f) * ucM;
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0df21() para error!");
        #endif
        return MT_ERR_PARA;
    }

    // 计算在帧侧的字节长度
    *pusfLen = usLen;
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0df22
 功能描述  : F6：日冻结正向无功电能量（总、费率1～M）
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月30日 星期五
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0df22(eMtTrans eTrans, void* psUser, void* psFrame, UINT16* pusfLen)
{
    if(!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0df22() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    sMtAfn0dF22    *psU      = (sMtAfn0dF22*)psUser;
    sMtAfn0dF22_f  *psF      = (sMtAfn0dF22_f*)psFrame;
    eMtErr         eRet      = MT_OK;
    UINT8          ucM       = 0;
    UINT16         usLen     = 0;
    UINT16         usTmp     = 0;
    INT32          i         = 0;
    sMtFmt13_f    *psFmt13_f = NULL;
    
    // 帧侧转为用户侧
    if(MT_TRANS_F2U == eTrans)
    {
        // Td_m
        eRet = emtTrans_td_m(eTrans, &(psU->sTd_m), &(psF->sTd_m), &usTmp);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df22() emtTrans_td_m() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += usTmp;

        // sTime
        eRet = emt_trans_YYMMDDhhmm(eTrans, &(psU->sTime), &(psF->sTime));
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df22() emt_trans_YYMMDDhhmm() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += sizeof(sMtFmt15_f);

        // M
        ucM = psF->ucM;
        if(ucM < MT_M_MIN || ucM > MT_M_MAX)
        {
            #ifdef MT_DBG
            DEBUG("ucM out of range %d", ucM);
            #endif
            return MT_ERR_OUTRNG;     
        }
        
        usLen += 1;

        // dFrthNoneT
        psFmt13_f = &(psF->dFrthNoneT);

        eRet = emt_trans_XXXX_XXXX(eTrans, &(psU->dFrthNoneT), psFmt13_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df22() emt_trans_XXXX_XXXX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt13_f);
        
        // dFrthNone
        for(i = 0; i < ucM; i++)
        {
            ++psFmt13_f;
            eRet = emt_trans_XXXX_XXXX(eTrans, &(psU->dFrthNone[i]), psFmt13_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df22() emt_trans_XXXX_XXXX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt13_f) * ucM;

    }
    else if(MT_TRANS_U2F == eTrans)
    // 用户侧转为帧侧
    {
        // Td_m
        eRet = emtTrans_td_m(eTrans, &(psU->sTd_m), &(psF->sTd_m), &usTmp);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df22() emtTrans_td_m() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += usTmp;

        // sTime
        eRet = emt_trans_YYMMDDhhmm(eTrans, &(psU->sTime), &(psF->sTime));
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df22() emt_trans_YYMMDDhhmm() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += sizeof(sMtFmt15_f);

        // M
        ucM = psU->ucM;
        if(ucM < MT_M_MIN || ucM > MT_M_MAX)
        {
            #ifdef MT_DBG
            DEBUG("ucM out of range %d", ucM);
            #endif
            return MT_ERR_OUTRNG;     
        }
        
        usLen += 1;

        // dFrthNoneT
        psFmt13_f = &(psF->dFrthNoneT);

        eRet = emt_trans_XXXX_XXXX(eTrans, &(psU->dFrthNoneT), psFmt13_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df22() emt_trans_XXXX_XXXX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt13_f);
        
        // dFrthNone
        for(i = 0; i < ucM; i++)
        {
            ++psFmt13_f;
            eRet = emt_trans_XXXX_XXXX(eTrans, &(psU->dFrthNone[i]), psFmt13_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df22() emt_trans_XXXX_XXXX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt13_f) * ucM;
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0df22() para error!");
        #endif
        return MT_ERR_PARA;
    }

    // 计算在帧侧的字节长度
    *pusfLen = usLen;
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0df23
 功能描述  : F7：日冻结反向有功电能量（总、费率1～M）
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月30日 星期五
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0df23(eMtTrans eTrans, void* psUser, void* psFrame, UINT16* pusfLen)
{
    if(!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0df23() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    sMtAfn0dF23     *psU     = (sMtAfn0dF23*)psUser;
    sMtAfn0dF23_f   *psF     = (sMtAfn0dF23_f*)psFrame;
    eMtErr         eRet      = MT_OK;
    UINT8          ucM       = 0;
    UINT16         usLen     = 0;
    UINT16         usTmp     = 0;
    INT32          i         = 0;
    sMtFmt13_f    *psFmt13_f = NULL;
    
    // 帧侧转为用户侧
    if(MT_TRANS_F2U == eTrans)
    {
        // Td_m
        eRet = emtTrans_td_m(eTrans, &(psU->sTd_m), &(psF->sTd_m), &usTmp);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df23() emtTrans_td_m() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += usTmp;

        // sTime
        eRet = emt_trans_YYMMDDhhmm(eTrans, &(psU->sTime), &(psF->sTime));
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df23() emt_trans_YYMMDDhhmm() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += sizeof(sMtFmt15_f);

        // M
        ucM = psF->ucM;
        if(ucM < MT_M_MIN || ucM > MT_M_MAX)
        {
            #ifdef MT_DBG
            DEBUG("ucM out of range %d", ucM);
            #endif
            return MT_ERR_OUTRNG;     
        }
        
        usLen += 1;

        // dBackHaveT
        psFmt13_f = &(psF->dBackHaveT);

        eRet = emt_trans_XXXX_XXXX(eTrans, &(psU->dBackHaveT), psFmt13_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df23() emt_trans_XXXX_XXXX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt13_f);
        
        // dBackHave
        for(i = 0; i < ucM; i++)
        {
            ++psFmt13_f;
            eRet = emt_trans_XXXX_XXXX(eTrans, &(psU->dBackHave[i]), psFmt13_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df23() emt_trans_XXXX_XXXX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt13_f) * ucM;
    }
    else if(MT_TRANS_U2F == eTrans)
    // 用户侧转为帧侧
    {
        // Td_m
        eRet = emtTrans_td_m(eTrans, &(psU->sTd_m), &(psF->sTd_m), &usTmp);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df23() emtTrans_td_m() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += usTmp;

        // sTime
        eRet = emt_trans_YYMMDDhhmm(eTrans, &(psU->sTime), &(psF->sTime));
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df23() emt_trans_YYMMDDhhmm() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += sizeof(sMtFmt15_f);

        // M
        ucM = psU->ucM;
        if(ucM < MT_M_MIN || ucM > MT_M_MAX)
        {
            #ifdef MT_DBG
            DEBUG("ucM out of range %d", ucM);
            #endif
            return MT_ERR_OUTRNG;     
        }
        
        usLen += 1;

        // dBackHaveT
        psFmt13_f = &(psF->dBackHaveT);

        eRet = emt_trans_XXXX_XXXX(eTrans, &(psU->dBackHaveT), psFmt13_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df23() emt_trans_XXXX_XXXX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt13_f);
        
        // dBackHave
        for(i = 0; i < ucM; i++)
        {
            ++psFmt13_f;
            eRet = emt_trans_XXXX_XXXX(eTrans, &(psU->dBackHave[i]), psFmt13_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df23() emt_trans_XXXX_XXXX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt13_f) * ucM;
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0df23() para error!");
        #endif
        return MT_ERR_PARA;
    }

    // 计算在帧侧的字节长度
    *pusfLen = usLen;
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0df24
 功能描述  : F24：月冻结反向无功电能量（总、费率1～M）
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月30日 星期五
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0df24(eMtTrans eTrans, void* psUser, void* psFrame, UINT16* pusfLen)
{
    if(!psUser || !psFrame || !pusfLen)
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0df24() pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    sMtAfn0dF24    *psU      = (sMtAfn0dF24*)psUser;
    sMtAfn0dF24_f  *psF      = (sMtAfn0dF24_f*)psFrame;
    eMtErr         eRet      = MT_OK;
    UINT8          ucM       = 0;
    UINT16         usLen     = 0;
    UINT16         usTmp     = 0;
    INT32          i         = 0;
    sMtFmt13_f    *psFmt13_f = NULL;
    
    // 帧侧转为用户侧
    if(MT_TRANS_F2U == eTrans)
    {
        // Td_m
        eRet = emtTrans_td_m(eTrans, &(psU->sTd_m), &(psF->sTd_m), &usTmp);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df24() emtTrans_td_m() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += usTmp;

        // sTime
        eRet = emt_trans_YYMMDDhhmm(eTrans, &(psU->sTime), &(psF->sTime));
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df24() emt_trans_YYMMDDhhmm() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += sizeof(sMtFmt15_f);

        // M
        ucM = psF->ucM;
        if(ucM < MT_M_MIN || ucM > MT_M_MAX)
        {
            #ifdef MT_DBG
            DEBUG("ucM out of range %d", ucM);
            #endif
            return MT_ERR_OUTRNG;     
        }
        
        usLen += 1;

        // dBackNoneT
        psFmt13_f = &(psF->dBackNoneT);

        eRet = emt_trans_XXXX_XXXX(eTrans, &(psU->dBackNoneT), psFmt13_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df24() emt_trans_XXXX_XXXX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt13_f);
        
        // dBackNone
        for(i = 0; i < ucM; i++)
        {
            ++psFmt13_f;
            eRet = emt_trans_XXXX_XXXX(eTrans, &(psU->dBackNone[i]), psFmt13_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df24() emt_trans_XXXX_XXXX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt13_f) * ucM;
    }
    else if(MT_TRANS_U2F == eTrans)
    // 用户侧转为帧侧
    {
        // Td_m
        eRet = emtTrans_td_m(eTrans, &(psU->sTd_m), &(psF->sTd_m), &usTmp);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df24() emtTrans_td_m() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += usTmp;

        // sTime
        eRet = emt_trans_YYMMDDhhmm(eTrans, &(psU->sTime), &(psF->sTime));
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df24() emt_trans_YYMMDDhhmm() error %d", eRet);
            #endif
            return eRet;
        }
        
        usLen += sizeof(sMtFmt15_f);

        // M
        ucM = psU->ucM;
        if(ucM < MT_M_MIN || ucM > MT_M_MAX)
        {
            #ifdef MT_DBG
            DEBUG("ucM out of range %d", ucM);
            #endif
            return MT_ERR_OUTRNG;     
        }
        
        usLen += 1;

        // dBackNoneT
        psFmt13_f = &(psF->dBackNoneT);

        eRet = emt_trans_XXXX_XXXX(eTrans, &(psU->dBackNoneT), psFmt13_f);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtTrans_afn0df24() emt_trans_XXXX_XXXX() error %d", eRet);
            #endif
            return eRet;
        }

        usLen += sizeof(sMtFmt13_f);
        
        // dBackNone
        for(i = 0; i < ucM; i++)
        {
            ++psFmt13_f;
            eRet = emt_trans_XXXX_XXXX(eTrans, &(psU->dBackNone[i]), psFmt13_f);
            if(MT_OK != eRet)
            {
                #ifdef MT_DBG
                DEBUG("emtTrans_afn0df24() emt_trans_XXXX_XXXX() error %d", eRet);
                #endif
                return eRet;
            }
        }

        usLen += sizeof(sMtFmt13_f) * ucM;

    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emtTrans_afn0df24() para error!");
        #endif
        return MT_ERR_PARA;
    }

    // 计算在帧侧的字节长度
    *pusfLen = usLen;
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0df25
 功能描述  : F25：日冻结日总及分相最大有功功率及发生时间、有功功率为零时间
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月10日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0df25(eMtTrans eTrans, void* psUser, void* psFrame, UINT16* pusfLen)
{
    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0dF25_f);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0df26
 功能描述  : F26：日冻结日总及分相最大需量及发生时间
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月10日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0df26(eMtTrans eTrans, void* psUser, void* psFrame, UINT16* pusfLen)
{
    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0dF26_f);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0df27
 功能描述  : F27：日冻结日电压统计数据
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月10日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0df27(eMtTrans eTrans, void* psUser, void* psFrame, UINT16* pusfLen)
{
    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0dF27_f);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0df28
 功能描述  : F28：日冻结日不平衡度越限累计时间
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月10日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0df28(eMtTrans eTrans, void* psUser, void* psFrame, UINT16* pusfLen)
{
    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0dF28_f);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0df29
 功能描述  : F29：日冻结日电流越限数据
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月10日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0df29(eMtTrans eTrans, void* psUser, void* psFrame, UINT16* pusfLen)
{ 
    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0dF29_f);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0df30
 功能描述  : F30：日冻结日视在功率越限累计时间
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月10日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0df30(eMtTrans eTrans, void* psUser, void* psFrame, UINT16* pusfLen)
{
    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0dF30_f);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0df31
 功能描述  : F31：日负载率统计
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月10日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0df31(eMtTrans eTrans, void* psUser, void* psFrame, UINT16* pusfLen)
{
    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0dF31_f);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0df32
 功能描述  : F32：日冻结电能表断相数据
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月10日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0df32(eMtTrans eTrans, void* psUser, void* psFrame, UINT16* pusfLen)
{
    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0dF32_f);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0df32
 功能描述  : F33：月冻结月总及分相最大有功功率及发生时间、有功功率为零时间
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月10日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0df33(eMtTrans eTrans, void* psUser, void* psFrame, UINT16* pusfLen)
{
    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0dF33_f);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0df34
 功能描述  : F34：月冻结月总及分相有功最大需量及发生时间
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月10日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0df34(eMtTrans eTrans, void* psUser, void* psFrame, UINT16* pusfLen)
{
    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0dF34_f);
    return MT_OK;
}   

/*****************************************************************************
 函 数 名  : emtTrans_afn0df35
 功能描述  : F35：月冻结月电压统计数据
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月10日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0df35(eMtTrans eTrans, void* psUser, void* psFrame, UINT16* pusfLen)
{
    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0dF35_f);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0df36
 功能描述  : F36：月冻结月不平衡度越限累计时间
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月10日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0df36(eMtTrans eTrans, void* psUser, void* psFrame, UINT16* pusfLen)
{
    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0dF36_f);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0df37
 功能描述  : F37：月冻结月电流越限数据
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月10日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0df37(eMtTrans eTrans, void* psUser, void* psFrame, UINT16* pusfLen)
{
    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0dF37_f);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0df38
 功能描述  : F38：月冻结月视在功率越限累计时间
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月10日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0df38(eMtTrans eTrans, void* psUser, void* psFrame, UINT16* pusfLen)
{
    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0dF38_f);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0df39
 功能描述  : F39：月负载率统计
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月10日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0df39(eMtTrans eTrans, void* psUser, void* psFrame, UINT16* pusfLen)
{
    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0dF39_f);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0df41
 功能描述  : F41：日冻结电容器累计投入时间和次数
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月10日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0df41(eMtTrans eTrans, void* psUser, void* psFrame, UINT16* pusfLen)
{
    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0dF41_f);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0df42
 功能描述  : F42：日冻结日、月电容器累计补偿的无功电能量
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月10日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0df42(eMtTrans eTrans, void* psUser, void* psFrame, UINT16* pusfLen)
{
    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0dF42_f);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0df43
 功能描述  : F43：日冻结日功率因数区段累计时间
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月10日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0df43(eMtTrans eTrans, void* psUser, void* psFrame, UINT16* pusfLen)
{
    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0dF43_f);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtTrans_afn0df44
 功能描述  : F44：月冻结月功率因数区段累计时间
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年9月10日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtTrans_afn0df44(eMtTrans eTrans, void* psUser, void* psFrame, UINT16* pusfLen)
{
    // 计算在帧侧的字节长度
    *pusfLen = sizeof(sMtAfn0dF44_f);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtGetGroupNum
 功能描述  :  根据总加组有效标志位获取总加组个数
 输入参数  : eMtTrans eTrans  
             void* psUser     
             void* psFrame    
             UINT16* pusfLen  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月20日 星期二
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
UINT8 emtGetGroupNum(UINT8 ucGroupFlag)
{
    UINT8 ucRet = 0;
    UINT8 i;

    for ( i = 0; i < 8; i ++)
    {
        if( ucGroupFlag & (1 << i))
        {
            ucRet++;
        }
    }

    return ucRet;
}

/*****************************************************************************
 函 数 名  : ucmt_get_bcd_0
 功能描述  : 获得一个char类型数据的BCD低位
 输入参数  : UINT8 ucData  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月1日 星期四
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
UINT8 ucmt_get_bcd_0(UINT8 ucData)
{
    return (ucData % 10);
}

/*****************************************************************************
 函 数 名  : ucmt_get_bcd_1
 功能描述  : 获得一个char类型数据的BCD高位
 输入参数  : UINT8 ucData  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月1日 星期四
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
UINT8 ucmt_get_bcd_1(UINT8 ucData)
{
    return (ucData / 10);
}

/*****************************************************************************
 函 数 名  : emt_bcd_to_str
 功能描述  : 将长度为len的BCD码转为2*len长的字符串
 输入参数  : const UINT8 *pbcd  
             UINT8 *pstr        
             INT32 len          
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年7月31日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emt_bcd_to_str(const UINT8* pbcd, UINT8* pstr, INT32 len)
{
    INT32 i  = 0, j  = 0;
    UINT8 c1 = 0, c0 = 0;

    if(!pstr || !pbcd)
    {
        #ifdef MT_DBG
        DEBUG("emt_bcd_to_str() para pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    if(len < 0)
    { 
        #ifdef MT_DBG
        DEBUG("emt_bcd_to_str() para error");
        #endif
        return MT_ERR_PARA;
    }

    for(i = len - 1,j = 0; i >= 0; i--)
    {
        c0 = pbcd[i] & 0xF;
        c1 = (pbcd[i] >> 4) & 0xF;
        if(c1 >= 0 && c1 <= 9)
        {
            pstr[j++] = c1 + '0'; 
        }
        else
        {
            pstr[j++] = c1 + 'A';
        }

        if(c0 >= 0 && c0 <= 9)
        {
            pstr[j++] = c0 + '0';   
        }
        else
        {
            pstr[j++] = c0 + 'A';
        } 
    }

    return MT_OK;
}
    
/*****************************************************************************
 函 数 名  : emt_str_to_bcd
 功能描述  : 将长度为len的字符串转化为BCD 长度为 len/2
 输入参数  : const UINT8 *pstr  
             UINT8  *pbcd        
             INT32   len           
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年7月31日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emt_str_to_bcd(const UINT8* pstr, UINT8* pbcd, INT32 len)
{

    UINT8 tmpValue;
    INT32 i;
    INT32 j;
    INT32 m;
    INT32 sLen;
 
    if(!pstr || !pbcd)
    {
        #ifdef MT_DBG
        DEBUG("emt_str_to_bcd() para pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    if(len < 0)
    { 
         #ifdef MT_DBG
         DEBUG("emt_str_to_bcd() para error");
         #endif
         return MT_ERR_PARA;
    }

    sLen = len;
    
    for(i = 0; i < sLen; i++)
    {
        if((pstr[i] < '0')
        ||((pstr[i] > '9') && (pstr[i] < 'A')) 
        ||((pstr[i] > 'F') && (pstr[i] < 'a')) 
        ||(pstr[i] > 'f'))
        {
            sLen=i;
            break;
        }
    }

    sLen = ((sLen <= (len * 2) ) ?  sLen : sLen * 2);
    memset((void *)pbcd, 0x00, len);

    for(i = sLen-1, j = 0, m = 0; (i>=0) && (m<len); i--, j++)
    {
        if((pstr[i] >= '0') && (pstr[i] <= '9'))
        {
            tmpValue = pstr[i] - '0';
        }
        else if((pstr[i] >= 'A') && (pstr[i] <= 'F'))
        {
            tmpValue = (pstr[i] - 'A' + 0x0A);
        }
        else if((pstr[i] >= 'a') && (pstr[i] <= 'f'))
        {
            tmpValue = (pstr[i] - 'a' + 0x0A);
        }
        else
        {
            tmpValue = 0;
        }

        if((j%2) == 0)
        {
            pbcd[m] = tmpValue;  
        }
        else
        {
            pbcd[m++] |= (tmpValue << 4);
        }

        if((tmpValue == 0) && (pstr[i] != '0'))
        {
            break;
        }
    }

    return MT_OK;
}

/*****************************************************************************
 函 数 名  : bmt_big_endian
 功能描述  : 判断主机序是否为大字节序
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2012年12月20日
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
BOOL   bmt_big_endian()
{
    UINT16 usData = 0x1234;
    if(*(UINT8*)&usData == 0x12)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*****************************************************************************
 函 数 名  : emt_write_uint16_small_endian
 功能描述  : 将一个UINT16以小字节序的方式写到指定地址
 输入参数  : UINT16 usData  
             UINT8* pWrite  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2012年12月20日
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emt_write_uint16_small_endian(UINT16 usData, UINT8* pWrite)
{
    UINT8 ucByteHi = 0, ucByteLo = 0;
    BOOL bBigEndian = FALSE; 

    if(!pWrite)
    {
        #ifdef MT_DBG
        DEBUG("emt_write_uint16_small_endian() para pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    bBigEndian = bmt_big_endian();
    if(bBigEndian == TRUE)
    {
        ucByteHi    = *(UINT8*)&usData;
        ucByteLo    = *((UINT8*)&usData + 1);
        *pWrite     = ucByteLo;
        *(pWrite+1) = ucByteHi;
    }
    else
    {
        *(UINT16*)pWrite = usData;
    }
      
    return MT_OK; 
}

/*****************************************************************************
 函 数 名  : emt_write_uint32_small_endian
 功能描述  : 将一个ulong数以小字节序的方式写入指定内存
 输入参数  : UINT32 ulData  
             UINT8* pWrite  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年1月16日
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emt_write_uint32_small_endian(UINT32 ulData,   UINT8* pWrite)
{
    UINT8 *pucData;
    BOOL  bBigEndian = FALSE;
    
    if(!pWrite)
    {
        #ifdef MT_DBG
        DEBUG("emt_write_uint32_small_endian() para pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    bBigEndian = bmt_big_endian();
    if(bBigEndian == TRUE)
    {
        pucData    = (UINT8*)pWrite;
        *pucData++ = (UINT8)(ulData  & 0xFF);
        *pucData++ = (UINT8)((ulData & 0xFF00)     >> 8);
        *pucData++ = (UINT8)((ulData & 0xFF0000)   >> 16);
        *pucData++ = (UINT8)((ulData & 0xFF000000) >> 24);
    }
    else
    {
        memcpy((void*)pWrite, (void*)&ulData, sizeof(UINT32));
    }
  
    return MT_OK; 
}

/*****************************************************************************
 函 数 名  : emt_read_uint16_small_endian
 功能描述  : 以小字节方式读取一个short
 输入参数  : UINT8 *pRead     
             UINT16 *pUint16  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年7月31日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emt_read_uint16_small_endian(UINT8* pRead, UINT16* pUint16)
{
    BOOL bBigEndian = FALSE;
    UINT8 ucData[2];
    UINT16 usRet = 0;

    if(!pRead || !pUint16)
    {
        #ifdef MT_DBG
        DEBUG("emt_read_uint16_small_endian() para pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    bBigEndian = bmt_big_endian();
    if(bBigEndian == TRUE)
    {
        ucData[1] = *pRead;
        ucData[0] = *(pRead+1);
        usRet     = *(UINT16*)ucData;  
    }
    else
    {
        usRet = *(UINT16*)pRead;
    }

    *pUint16 = usRet;
    return MT_OK;

}

/*****************************************************************************
 函 数 名  : emt_read_uint32_small_endian
 功能描述  : 从指定地址位置以小字节序的方式读入一个长整数
 输入参数  : UINT8 *pRead     
             UINT32* pUint32  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年7月31日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emt_read_uint32_small_endian(UINT8* pRead, UINT32* pUint32)
{
    UINT8 ucData[4];
    UINT32 ulRet = 0;
    BOOL bBigEndian = FALSE;

    if(!pRead || !pUint32)
    {
        #ifdef MT_DBG
        DEBUG("emt_read_uint32_small_endian() para pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    bBigEndian = bmt_big_endian();
    if(bBigEndian == TRUE)
    {
        ucData[3] = *(pRead + 0);
        ucData[2] = *(pRead + 1);
        ucData[1] = *(pRead + 2);
        ucData[0] = *(pRead + 3);
        ulRet     = *(UINT32*)ucData;  
    }
    else
    {
        memcpy((void*)&ulRet, (void*)pRead, sizeof(UINT32));
    }

    *pUint32 = ulRet;
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emt_trans_address
 功能描述  : 地址域用户侧与封帧侧数据结构转换函数
 输入参数  : eMtTrans eDir           
             sMtAddress *psAddr_u    
             sMtAddress_f *psAddr_f  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年7月31日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emt_trans_address(eMtTrans eTrans, sMtAddress* psAddr_u, sMtAddress_f* psAddr_f)
{
    if(!psAddr_u || !psAddr_f)
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_address() para pointer is null!");
        #endif
        return MT_ERR_NULL;
    }

    if(MT_TRANS_U2F == eTrans)
    {
        if(psAddr_u->usTAddress == 0)
        {
            #ifdef MT_DBG
            DEBUG("emt_trans_address() terminal address error!");
            #endif  
            return MT_ERR_ADDR;
        }

        if(psAddr_u->ucMAddress > MT_MST_ADDR_MAX ||
        psAddr_u->ucMAddress < MT_MST_ADDR_MIN)
        {
            #ifdef MT_DBG
            DEBUG("emt_trans_address() masger address error!");
            #endif  
            return MT_ERR_ADDR;
        }

        (void)emt_str_to_bcd((UINT8*)(psAddr_u->acRegionCode), (UINT8*)(psAddr_f->acRegionCode), 4);
        // (void)emt_write_uint16_small_endian(psAddr_u->usTAddress, (UINT8*)&(psAddr_f->usTAddress));
        psAddr_f->usTAddress = psAddr_u->usTAddress;

        psAddr_f->ucMAddress = (psAddr_u->ucMAddress << 1);
        if(psAddr_u->bTeamAddr == TRUE)
        {
            psAddr_f->ucMAddress |= 0x01;
        }
        else
        {
            psAddr_f->ucMAddress &= 0xFE;
        }
    }
    else if(MT_TRANS_F2U == eTrans)
    {
        (void)emt_bcd_to_str((UINT8*)(psAddr_f->acRegionCode), (UINT8*)(psAddr_u->acRegionCode), 2);
        // (void)emt_read_uint16_small_endian((UINT8*)&(psAddr_f->usTAddress), (UINT16*)&(psAddr_u->usTAddress));
        psAddr_u->usTAddress = psAddr_f->usTAddress;
        psAddr_u->bTeamAddr  = (psAddr_f->ucMAddress & 0x01) ? TRUE : FALSE;
        psAddr_u->ucMAddress = (psAddr_f->ucMAddress >> 1) & 0x7F;
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_address() para error!");
        #endif
        return MT_ERR_PARA;
    }

    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emt_trans_ctrl
 功能描述  : 控制域用户侧和帧侧数据转换函数
 输入参数  : eMtTrans eDir    
             sMtCtrl *puCtrl  
             UINT8* pfCtrl    
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年7月31日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emt_trans_ctrl(eMtTrans eTrans, sMtCtrl* puCtrl, UINT8* pfCtrl)
{
    UINT8 ucfCtrl = 0;

    // 用户侧转换为帧侧
    if(MT_TRANS_U2F == eTrans)      
    {
        (puCtrl->eDir == MT_DIR_M2S)  ? (ucfCtrl &= 0x7F) : (ucfCtrl |= 0x80);
        (puCtrl->ePRM == MT_PRM_ACTIVE) ? (ucfCtrl |= 0x40) : (ucfCtrl &= 0xBF);

        if(puCtrl->eDir == MT_DIR_M2S)
        { 
            //下行报文 
            (puCtrl->bFcv     == TRUE) ? (ucfCtrl |= 0x10) : (ucfCtrl &= 0xEF);
            (puCtrl->bAcd_Fcb == TRUE) ? (ucfCtrl |= 0x10) : (ucfCtrl &= 0xEF);
        }
        else
        {
            // 上行报文
            (puCtrl->bAcd_Fcb == TRUE) ? (ucfCtrl |= 0x20) : (ucfCtrl &= 0xDF);
        }

        // 功能码
        if(puCtrl->ePRM == MT_PRM_ACTIVE)
        {
            ucfCtrl |= (puCtrl->uFCode.eFcdPrm0 & 0x0F);
        }
        else
        {
            ucfCtrl |= (puCtrl->uFCode.eFcdPrm1 & 0x0F);
        } 

        *pfCtrl = ucfCtrl;

    }
    // 帧侧转换为用户侧
    else if(MT_TRANS_F2U == eTrans)
    {
        ucfCtrl = * pfCtrl;
        if(ucfCtrl & 0x80)
        {
            // 上行
            puCtrl->eDir = MT_DIR_S2M;  
            puCtrl->bAcd_Fcb = (ucfCtrl & 0x20) ? TRUE : FALSE;
        }
        else
        {   
            // 下行
            puCtrl->eDir     = MT_DIR_M2S; 
            puCtrl->bAcd_Fcb = (ucfCtrl & 0x20) ? TRUE : FALSE;
            puCtrl->bFcv     = (ucfCtrl & 0x10) ? TRUE : FALSE;
        }

        puCtrl->ePRM = (ucfCtrl & 0x40) ? MT_PRM_ACTIVE : MT_PRM_PASIVE;
        puCtrl->uFCode.eFcdPrm0 = (eMtFcodeRpm0)(ucfCtrl & 0x0F);
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_ctrl() para error!");
        #endif
        return MT_ERR_PARA;
    }
    
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emt_get_ctrl
 功能描述  : 获得控制域用户侧信息
 输入参数  : eMtAFN eAFN      
             eMtDir eDir      
             eMtPRM ePRM 
             BOOL bAcd_Fcb
             sMtCtrl *psCtrl  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月7日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emt_get_ctrl(eMtAFN eAFN, eMtDir eDir, eMtPRM ePRM, BOOL bAcd_Fcb, sMtCtrl *psCtrl)
{
    if(!psCtrl)
    {
        #ifdef MT_DBG
        DEBUG("emt_get_ctrl() para pointer is null");
        #endif
        return MT_ERR_NULL;
    }

    if(MT_DIR_M2S != eDir && MT_DIR_S2M != eDir)
    {
        #ifdef MT_DBG
        DEBUG("emt_get_ctrl() eDir para error");
        #endif
        return MT_ERR_PARA;
    }

    if(MT_PRM_ACTIVE != ePRM && MT_PRM_PASIVE != ePRM)
    {
        #ifdef MT_DBG
        DEBUG("emt_get_ctrl() ePRM para error");
        #endif
        return MT_ERR_PARA;
    }

    psCtrl->eDir = eDir;
    psCtrl->ePRM = ePRM;

    switch(eAFN)
    {
        case AFN_00_CONF:

             if(ePRM == MT_PRM_PASIVE) //  确认否认报文只能从动站发出
             {
                  psCtrl->uFCode.eFcdPrm0 = MT_FCD_RPM0_OK; 
             }
             else
             {
                #ifdef MT_DBG
                DEBUG("emt_get_ctrl() pack logic error");
                #endif
                return MT_ERR_LOGIC;
             }
            
             if(eDir == MT_DIR_M2S)   // 下行
             {
                psCtrl->bFcv = TRUE; 
                psCtrl->bAcd_Fcb = bAcd_Fcb; 
             }
             else                     // 上行
             {
                 psCtrl->bFcv = FALSE; 
                 psCtrl->bAcd_Fcb = TRUE;
             }

            break; 
            
        case AFN_01_RSET:
            
             if(MT_DIR_M2S == eDir && ePRM == MT_PRM_ACTIVE)   // 下行
             {
                /*
                当帧计数有效位FCV=1时，FCB表示每个站连续的发送/确认或者请求/响应服务的变化位。
                FCB位用来防止信息传输的丢失和重复。
                启动站向同一从动站传输新的发送/确认或请求/响应传输服务时，
                将FCB取相反值。启动站保存每一个从动站FCB值，若超时未收到从动站的报文,
                或接收出现差错，则启动站不改变FCB的状态，重复原来的发送/确认或者请求/响应服务。
                复位命令中的FCB=0，从动站接收复位命令后将FCB置"0"。
                */
                psCtrl->bFcv            = TRUE; 
                psCtrl->bAcd_Fcb        = FALSE; 
                psCtrl->uFCode.eFcdPrm1 = MT_FCD_RPM1_RESET; 
             }
             else                    // 上行
             {
                #ifdef MT_DBG
                DEBUG("emt_get_ctrl() pack logic error");
                #endif
                return MT_ERR_LOGIC;
             }

            break;
            
        case AFN_02_LINK:
            
             if(MT_DIR_S2M == eDir && MT_PRM_ACTIVE == ePRM)    // 该命令只有上行的主动
             {
                 psCtrl->uFCode.eFcdPrm1 = MT_FCD_RPM1_LINK; 
                 psCtrl->bFcv     = FALSE;   
                 psCtrl->bAcd_Fcb = FALSE;
             }
             else                   
             {
                #ifdef MT_DBG
                DEBUG("emt_get_ctrl() pack logic error");
                #endif
                return MT_ERR_LOGIC;
             }

            break;
            
        case AFN_03_RELY:

            if(MT_DIR_M2S == eDir)  
            {
               if(MT_PRM_ACTIVE == ePRM)
               {
                   psCtrl->uFCode.eFcdPrm1 = MT_FCD_RPM1_USER; 
                   psCtrl->bFcv     = TRUE;   
                   psCtrl->bAcd_Fcb = bAcd_Fcb;
               }
               else                    
               {
                  #ifdef MT_DBG
                  DEBUG("emt_get_ctrl() pack logic error");
                  #endif
                  return MT_ERR_LOGIC;                
               }
            }
            else 
            {
                if(MT_PRM_PASIVE == ePRM)
                {
                    psCtrl->uFCode.eFcdPrm0 = MT_FCD_RPM0_USER; 
                    // 该报文没有EC
                    psCtrl->bFcv     = FALSE;   
                    psCtrl->bAcd_Fcb = FALSE;
                }
                else                    
                {
                    #ifdef MT_DBG
                    DEBUG("emt_get_ctrl() pack logic error");
                    #endif
                    return MT_ERR_LOGIC;                
                }
            }
           
            break;

        case AFN_04_SETP:
            
             if(MT_DIR_M2S == eDir && MT_PRM_ACTIVE == ePRM)   // 只有下行主动  
             {     
                psCtrl->bFcv            = TRUE; 
                psCtrl->bAcd_Fcb        = bAcd_Fcb; 
                psCtrl->uFCode.eFcdPrm1 = MT_FCD_RPM1_USER; 
             }
             else                     
             {
                #ifdef MT_DBG
                DEBUG("emt_get_ctrl() pack logic error");
                #endif
                return MT_ERR_LOGIC;   
             }

            break;

        case AFN_05_CTRL:

            if(eDir == MT_DIR_M2S && ePRM == MT_PRM_ACTIVE)   // 只有下行主动  
             {

                psCtrl->bFcv            = TRUE; 
                psCtrl->bAcd_Fcb        = bAcd_Fcb; 
                psCtrl->uFCode.eFcdPrm1 = MT_FCD_RPM1_USER; 
             }
             else                     // 上行
             {
                #ifdef MT_DBG
                DEBUG("emt_get_ctrl() pack logic error");
                #endif
                return MT_ERR_LOGIC;   
             }

            break;
            
        case AFN_06_AUTH:

            if(eDir == MT_DIR_M2S)    // 下行
            {              
                psCtrl->bFcv            = TRUE; 
                psCtrl->bAcd_Fcb        = bAcd_Fcb; 
            }
            else                      // 上行
            {
                psCtrl->bFcv            = FALSE; 
                psCtrl->bAcd_Fcb        = TRUE; 
            }
             
            if(ePRM == MT_PRM_ACTIVE)  //主动站
            {
                psCtrl->uFCode.eFcdPrm1 = MT_FCD_RPM1_USER;
            }
            else                       //从动站
            {
                psCtrl->uFCode.eFcdPrm0 = MT_FCD_RPM0_USER;
            }
             
            break;
            
        case AFN_08_CASC:
            
            if(MT_DIR_M2S == eDir)   // 下行
            {
                if(MT_PRM_ACTIVE == ePRM)
                {
                    psCtrl->bFcv            = TRUE; 
                    psCtrl->bAcd_Fcb        = bAcd_Fcb; 
                    psCtrl->uFCode.eFcdPrm1 = MT_FCD_RPM1_USER; 
                }
                else
                {
                    #ifdef MT_DBG
                    DEBUG("emt_get_ctrl() pack logic error");
                    #endif
                    return MT_ERR_LOGIC; 
                }            
            }
            else                   
            {
               if(MT_PRM_PASIVE == ePRM)
                {
                    psCtrl->bFcv            = FALSE; 
                    psCtrl->bAcd_Fcb        = FALSE; 
                    psCtrl->uFCode.eFcdPrm0 = MT_FCD_RPM0_USER; 
                }
                else
                {
                    #ifdef MT_DBG
                    DEBUG("emt_get_ctrl() pack logic error");
                    #endif
                    return MT_ERR_LOGIC; 
                } 
            }

            break;
           
        case AFN_09_CFIG:
            
             if(MT_DIR_M2S == eDir)   // 下行
            {
                if(MT_PRM_ACTIVE == ePRM)
                {
                    psCtrl->bFcv            = TRUE; 
                    psCtrl->bAcd_Fcb        = bAcd_Fcb; 
                    psCtrl->uFCode.eFcdPrm1 = MT_FCD_RPM1_USER; 
                }
                else
                {
                    #ifdef MT_DBG
                    DEBUG("emt_get_ctrl() pack logic error");
                    #endif
                    return MT_ERR_LOGIC; 
                }            
            }
            else                   
            {
               if(MT_PRM_PASIVE == ePRM)
                {
                    psCtrl->bFcv            = FALSE; 
                    psCtrl->bAcd_Fcb        = TRUE; 
                    psCtrl->uFCode.eFcdPrm0 = MT_FCD_RPM0_USER; 
                }
                else
                {
                    #ifdef MT_DBG
                    DEBUG("emt_get_ctrl() pack logic error");
                    #endif
                    return MT_ERR_LOGIC; 
                } 
            }
            
            break;
            
        case AFN_0A_GETP:

            if(MT_DIR_M2S == eDir)   // 下行
            {
                if(MT_PRM_ACTIVE == ePRM)
                {
                    psCtrl->bFcv            = TRUE; 
                    psCtrl->bAcd_Fcb        = bAcd_Fcb; 
                    psCtrl->uFCode.eFcdPrm1 = MT_FCD_RPM1_USER; 
                }
                else
                {
                    #ifdef MT_DBG
                    DEBUG("emt_get_ctrl() pack logic error");
                    #endif
                    return MT_ERR_LOGIC; 
                }            
            }
            else                   
            {
               if(MT_PRM_PASIVE == ePRM)
                {
                    psCtrl->bFcv            = FALSE; 
                    psCtrl->bAcd_Fcb        = TRUE; 
                    psCtrl->uFCode.eFcdPrm0 = MT_FCD_RPM0_USER; 
                }
                else
                {
                    #ifdef MT_DBG
                    DEBUG("emt_get_ctrl() pack logic error");
                    #endif
                    return MT_ERR_LOGIC; 
                } 
            }
            
            break;

        case AFN_0B_ASKT:
        
           if(MT_DIR_M2S == eDir)   // 下行
            {
                if(MT_PRM_ACTIVE == ePRM)
                {
                    psCtrl->bFcv            = TRUE; 
                    psCtrl->bAcd_Fcb        = bAcd_Fcb; 
                    psCtrl->uFCode.eFcdPrm1 = MT_FCD_RPM1_USER; 
                }
                else
                {
                    #ifdef MT_DBG
                    DEBUG("emt_get_ctrl() pack logic error");
                    #endif
                    return MT_ERR_LOGIC; 
                }            
            }
            else                   
            {
               if(MT_PRM_PASIVE == ePRM)
                {
                    psCtrl->bFcv            = FALSE; 
                    psCtrl->bAcd_Fcb        = FALSE;
                    psCtrl->uFCode.eFcdPrm0 = MT_FCD_RPM0_USER; 
                }
                else
                {
                    #ifdef MT_DBG
                    DEBUG("emt_get_ctrl() pack logic error");
                    #endif
                    return MT_ERR_LOGIC; 
                } 
            }
            
            break;
            
        case AFN_0C_ASK1:
        
           if(MT_DIR_M2S == eDir)   // 下行
            {
                if(MT_PRM_ACTIVE == ePRM)
                {
                    psCtrl->bFcv            = TRUE; 
                    psCtrl->bAcd_Fcb        = bAcd_Fcb; 
                    psCtrl->uFCode.eFcdPrm1 = MT_FCD_RPM1_ASK_1; 
                }
                else
                {
                    #ifdef MT_DBG
                    DEBUG("emt_get_ctrl() pack logic error");
                    #endif
                    return MT_ERR_LOGIC; 
                }            
            }
            else                   
            {
               if(MT_PRM_PASIVE == ePRM)
                {
                    psCtrl->bFcv            = FALSE; 
                    psCtrl->bAcd_Fcb        = TRUE; 
                    psCtrl->uFCode.eFcdPrm0 = MT_FCD_RPM0_USER; 
                }
                else
                {
                    #ifdef MT_DBG
                    DEBUG("emt_get_ctrl() pack logic error");
                    #endif
                    return MT_ERR_LOGIC; 
                } 
            }
            
            break;
            
        case AFN_0D_ASK2:
        
            if(MT_DIR_M2S == eDir)   // 下行
            {
                if(MT_PRM_ACTIVE == ePRM)
                {
                    psCtrl->bFcv            = TRUE; 
                    psCtrl->bAcd_Fcb        = bAcd_Fcb; 
                    psCtrl->uFCode.eFcdPrm1 = MT_FCD_RPM1_ASK_2; 
                }
                else
                {
                    #ifdef MT_DBG
                    DEBUG("emt_get_ctrl() pack logic error");
                    #endif
                    return MT_ERR_LOGIC; 
                }            
            }
            else                   
            {
                if(MT_PRM_PASIVE == ePRM)
                {
                    psCtrl->bFcv            = FALSE; 
                    psCtrl->bAcd_Fcb        = TRUE; 
                    psCtrl->uFCode.eFcdPrm0 = MT_FCD_RPM0_USER; 
                }
                else
                {
                    #ifdef MT_DBG
                    DEBUG("emt_get_ctrl() pack logic error");
                    #endif
                    return MT_ERR_LOGIC; 
                } 
            }
            
            break;
            
        case AFN_0E_ASK3:

            if(MT_DIR_M2S == eDir)   // 下行
            {
                if(MT_PRM_ACTIVE == ePRM)
                {
                    psCtrl->bFcv            = TRUE; 
                    psCtrl->bAcd_Fcb        = bAcd_Fcb; 
                    psCtrl->uFCode.eFcdPrm1 = MT_FCD_RPM1_ASK_2; 
                }
                else
                {
                    #ifdef MT_DBG
                    DEBUG("emt_get_ctrl() pack logic error");
                    #endif
                    return MT_ERR_LOGIC; 
                }            
            }
            else                   
            {
                if(MT_PRM_PASIVE == ePRM)
                {
                    psCtrl->bFcv            = FALSE; 
                    psCtrl->bAcd_Fcb        = TRUE; 
                    psCtrl->uFCode.eFcdPrm0 = MT_FCD_RPM0_USER; 
                }
                else
                {
                    #ifdef MT_DBG
                    DEBUG("emt_get_ctrl() pack logic error");
                    #endif
                    return MT_ERR_LOGIC; 
                } 
            }
            
            break; if(MT_DIR_M2S == eDir)   // 下行
            {
                if(MT_PRM_ACTIVE == ePRM)
                {
                    psCtrl->bFcv            = TRUE; 
                    psCtrl->bAcd_Fcb        = bAcd_Fcb; 
                    psCtrl->uFCode.eFcdPrm1 = MT_FCD_RPM1_ASK_2; 
                }
                else
                {
                    #ifdef MT_DBG
                    DEBUG("emt_get_ctrl() pack logic error");
                    #endif
                    return MT_ERR_LOGIC; 
                }            
            }
            else                   
            {
                if(MT_PRM_PASIVE == ePRM)
                {
                    psCtrl->bFcv            = FALSE; 
                    psCtrl->bAcd_Fcb        = TRUE; 
                    psCtrl->uFCode.eFcdPrm0 = MT_FCD_RPM0_USER; 
                }
                else
                {
                    #ifdef MT_DBG
                    DEBUG("emt_get_ctrl() pack logic error");
                    #endif
                    return MT_ERR_LOGIC; 
                } 
            }
            
            break;
            
        case AFN_0F_FILE:
        
           if(eDir == MT_DIR_M2S)   // 下行
             {
                
             }
             else                     // 上行
             {

             }

             if(ePRM == MT_PRM_ACTIVE) //主动站
             {

             }
             else                                         //从动站
             {

             }
            break;
            
        case AFN_10_DATA:
            
           if(MT_DIR_M2S == eDir)   // 下行
            {
                if(MT_PRM_ACTIVE == ePRM)
                {
                    psCtrl->bFcv            = TRUE; 
                    psCtrl->bAcd_Fcb        = bAcd_Fcb; 
                    psCtrl->uFCode.eFcdPrm1 = MT_FCD_RPM1_ASK_2; 
                }
                else
                {
                    #ifdef MT_DBG
                    DEBUG("emt_get_ctrl() pack logic error");
                    #endif
                    return MT_ERR_LOGIC; 
                }            
            }
            else                   
            {
                if(MT_PRM_PASIVE == ePRM)
                {
                    psCtrl->bFcv            = FALSE; 
                    psCtrl->bAcd_Fcb        = TRUE; 
                    psCtrl->uFCode.eFcdPrm0 = MT_FCD_RPM0_USER; 
                }
                else
                {
                    #ifdef MT_DBG
                    DEBUG("emt_get_ctrl() pack logic error");
                    #endif
                    return MT_ERR_LOGIC; 
                } 
            }
            
            break; if(MT_DIR_M2S == eDir)   // 下行
            {
                if(MT_PRM_ACTIVE == ePRM)
                {
                    psCtrl->bFcv            = TRUE; 
                    psCtrl->bAcd_Fcb        = bAcd_Fcb; 
                    psCtrl->uFCode.eFcdPrm1 = MT_FCD_RPM1_ASK_2; 
                }
                else
                {
                    #ifdef MT_DBG
                    DEBUG("emt_get_ctrl() pack logic error");
                    #endif
                    return MT_ERR_LOGIC; 
                }            
            }
            else                   
            {
                if(MT_PRM_PASIVE == ePRM)
                {
                    psCtrl->bFcv            = FALSE; 
                    psCtrl->bAcd_Fcb        = TRUE; 
                    psCtrl->uFCode.eFcdPrm0 = MT_FCD_RPM0_USER; 
                }
                else
                {
                    #ifdef MT_DBG
                    DEBUG("emt_get_ctrl() pack logic error");
                    #endif
                    return MT_ERR_LOGIC; 
                } 
            }
            
            break;

        default:
            return MT_ERR_PARA;
            //break;

    }
    
    return MT_OK;
}
/*****************************************************************************
 函 数 名  : emt_trans_seq
 功能描述  : 将SEQ字段从用户侧到帧侧转换函数
 输入参数  : eMtTrans eTrans  
             sMtSEQ *puSEQ    
             sMtSEQ_f *pfSEQ  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年7月31日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emt_trans_seq(eMtTrans eTrans, sMtSEQ *puSEQ, sMtSEQ_f *pfSEQ)
{
    if(!puSEQ || !pfSEQ)
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_seq() para pointer is null!");
        #endif
        return MT_ERR_NULL;
    }

    if(MT_TRANS_F2U == eTrans)
    {
        puSEQ->bTpv  = (pfSEQ->TPV == 1) ? TRUE : FALSE;
        puSEQ->bCon  = (pfSEQ->CON == 1) ? TRUE : FALSE;
        puSEQ->ucSeq = pfSEQ->SEQ;

        if(pfSEQ->FIR == 1)
        {
            puSEQ->ePos = (pfSEQ->FIN == 1) ? MT_POS_SIGLE : MT_POS_MUL_S;  
        }
        else
        {
            puSEQ->ePos = (pfSEQ->FIN == 1) ? MT_POS_MUL_E : MT_POS_MUL_M;
        }        
    }
    else if(MT_TRANS_U2F == eTrans)
    {
        pfSEQ->TPV = (puSEQ->bTpv == TRUE) ? 1 : 0;
        pfSEQ->CON = (puSEQ->bCon == TRUE) ? 1 : 0;
        pfSEQ->SEQ =  puSEQ->ucSeq & 0x0F;

        switch(puSEQ->ePos)
        {
            case MT_POS_SIGLE:
                pfSEQ->FIR = 1;
                pfSEQ->FIN = 1;
                break;

            case MT_POS_MUL_S:
                pfSEQ->FIR = 1;
                pfSEQ->FIN = 0;
                break;

            case MT_POS_MUL_M:
                pfSEQ->FIR = 0;
                pfSEQ->FIN = 0;
                break;

            case MT_POS_MUL_E:
                pfSEQ->FIR = 0;
                pfSEQ->FIN = 1;
                break;

            default:
                #ifdef MT_DBG
                DEBUG("emt_trans_seq() para error!");
                #endif
                return MT_ERR_PARA;
                //break;
        }
    }
    else
    {
         #ifdef MT_DBG
         DEBUG("emt_trans_seq() para error!");
         #endif
         return MT_ERR_PARA;
    }
    
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emt_trans_tp
 功能描述  : TP数据结构转换函数
 输入参数  : eMtTrans eTrans  
             sMtTP* pTP_u     
             sMtTP_f* pTP_f   
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月1日 星期四
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emt_trans_tp(eMtTrans eTrans, sMtTP* pTP_u, sMtTP_f* pTP_f)
{
    if(!pTP_u || !pTP_f)
    {
         #ifdef MT_DBG
         DEBUG("emt_trans_tp() para pointer is null!");
         #endif
         return MT_ERR_NULL;
    }

    if(MT_TRANS_U2F == eTrans)
    {   
        pTP_f->ucPFC = pTP_u->ucPFC;
        pTP_f->ucPermitDelayMinutes = pTP_u->ucPermitDelayMinutes;
        emt_trans_DDHHmmss(MT_TRANS_U2F, &(pTP_u->sDDHHmmss), &(pTP_f->sDDHHmmss));
    }
    else if(MT_TRANS_F2U == eTrans)
        {
        pTP_u->ucPFC = pTP_f->ucPFC;
        pTP_u->ucPermitDelayMinutes = pTP_f->ucPermitDelayMinutes;
        emt_trans_DDHHmmss(MT_TRANS_F2U, &(pTP_u->sDDHHmmss), &(pTP_f->sDDHHmmss));
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_tp() para error!");
        #endif
        return MT_ERR_PARA;
    }

    return MT_OK;
}

/*****************************************************************************
 函 数 名  : usmt_get_aux_len
 功能描述  : 获得当前类型的报文中附加域的总字长(ec pw tp)
 输入参数  : eMtAFN eAFN  
             eMtDir eDir  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月9日 星期五
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
UINT16 usmt_get_aux_len(eMtAFN eAFN, eMtDir eDir, BOOL bEc, BOOL bTp)
{
    UINT16 usAuxLen = 0;

    BOOL   bPw = FALSE;

    bPw = bmt_have_pw(eAFN, eDir);
  
    if(TRUE == bEc)
    {
        usAuxLen += sizeof(sMtEC);
    }
    
    if(TRUE == bPw)
    {
        usAuxLen += MT_PW_LEN;
    }

    if(TRUE == bTp)
    {
        usAuxLen += sizeof(sMtTP_f);
    }

    return usAuxLen;
}  
/*****************************************************************************
 函 数 名  : emt_get_tp
 功能描述  : 获得当前的时间戳
 输入参数  : UINT8 ucPFC   
             sMtTP *psuTp  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月7日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emt_get_tp(UINT8 ucPFC, sMtTP *psuTp)
{
    if(!psuTp)
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_tp() para pointer is null!");
        #endif
        return MT_ERR_NULL;
    }

    // 获取系统当前时间
    time_t now;
    struct tm* timenow;
    time(&now);
    timenow = localtime(&now);
    
    psuTp->sDDHHmmss.ucDD = timenow->tm_mday;
    psuTp->sDDHHmmss.ucHH = timenow->tm_hour;
    psuTp->sDDHHmmss.ucmm = timenow->tm_min;
    psuTp->sDDHHmmss.ucss = timenow->tm_sec;

    psuTp->ucPermitDelayMinutes = g_ucMtPermitDelayMinutes;
    psuTp->ucPFC = ucPFC;

    return MT_OK;
}

/*****************************************************************************
 函 数 名  : bmt_tp_timeout
 功能描述  : 以当前时间为基准判断一个tp是否超时
 输入参数  : sMtTP *psTP  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月9日 星期五
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
BOOL  bmt_tp_timeout(sMtTP *psTP)
{
    if(!psTP)
    {
        return TRUE;
    }

    time_t now;
    struct tm* timenow;
    UINT8 ucDelayMinutes;  // 发送传输延时时间
    // 获取系统当前时间
    time(&now);
    timenow = localtime(&now);
    int cDay  = 0;
    int cHour = 0;
    int cMin  = 0;
    int cSec  = 0;

    int rDay  = 0;
    int rHour = 0;
    int rMin  = 0;
    int rSec  = 0;
    
    int minutes = 0;  // 相关的秒数
    
    cDay = timenow->tm_mday;
    cHour = timenow->tm_hour;
    cMin = timenow->tm_min;
    cSec = timenow->tm_sec;

    rDay  = psTP->sDDHHmmss.ucDD;
    rHour = psTP->sDDHHmmss.ucHH;
    rMin = psTP->sDDHHmmss.ucmm;
    rSec = psTP->sDDHHmmss.ucss;
    ucDelayMinutes = psTP->ucPermitDelayMinutes;
    minutes = (cDay - rDay)*60*24 + (cHour - rHour)*60 + (cMin - rMin) * 1; 

    if(minutes < 0)
    {
        return TRUE;
    }

    if(minutes <= ucDelayMinutes)
    {
        return FALSE;
    }
     
    return TRUE;
}    

/*****************************************************************************
 函 数 名  : nMtPow
 功能描述  : 求10的N次方 (注意参数不要超过10)
 输入参数  : UINT8 exp  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年5月29日
    作    者   : 李明
    修改内容   : 新生成函数

*****************************************************************************/
int  nMtPow(UINT8 exp)
{

    if(exp > 10)
    {
        return -1;
    }

    if(exp == 0)
    {
        return 1;
    }

    return 10 * nMtPow(exp -1);
}

/*****************************************************************************
 函 数 名  : emt_trans_YYWWMMDDhhmmss
 功能描述  : 数据格式01 对于表A.1 格式 
 输入参数  : eMtTrans eTrans      
             sMtUserClock* psUser        
             sMtFrmClock* psFrame  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月21日 星期三
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emt_trans_YYWWMMDDhhmmss(eMtTrans eTrans, sMtUserClock* psUser, sMtFrmClock* psFrame)
{
    if(!psFrame || !psUser)
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_YYWWMMDDhhmmss() para pointer is null!");
        #endif
        return MT_ERR_NULL;
    }

    if (MT_TRANS_F2U == eTrans )  // 帧侧到用户侧
    {
        psUser->ucYear    = psFrame->bcd_yea_0 + psFrame->bcd_yea_1 * 10;
        psUser->ucWeek    = psFrame->bcd_wek_0;
        psUser->ucMonth   = psFrame->bcd_mon_0 + psFrame->bcd_mon_1 * 10;
        psUser->ucDay     = psFrame->bcd_day_0 + psFrame->bcd_day_1 * 10;
        psUser->ucHour    = psFrame->bcd_hur_0 + psFrame->bcd_hur_1 * 10;
        psUser->ucMinute  = psFrame->bcd_min_0 + psFrame->bcd_min_1 * 10;
        psUser->ucSecond  = psFrame->bcd_sec_0 + psFrame->bcd_sec_1 * 10;
    }
     else if (MT_TRANS_U2F == eTrans) // 用户侧到帧侧
    { 
        // 年
        psFrame->bcd_yea_0 = ucmt_get_bcd_0(psUser->ucYear);
        psFrame->bcd_yea_1 = ucmt_get_bcd_1(psUser->ucYear);

        // 月
        if (psUser->ucMonth > 12)
        {
             #ifdef MT_DBG
             DEBUG("emt_trans_YYWWMMDDhhmmss() ucMonth MT_ERR_OUTRNG!");
             #endif
             return MT_ERR_OUTRNG;
        }
        else
        {
            psFrame->bcd_mon_0 = ucmt_get_bcd_0(psUser->ucMonth);
            psFrame->bcd_mon_1 = ucmt_get_bcd_1(psUser->ucMonth);
        }

        // 星期
        if (psUser->ucWeek > 7)
        {
            #ifdef MT_DBG
            DEBUG("emt_trans_YYWWMMDDhhmmss() ucWeek MT_ERR_OUTRNG!");
            #endif
            return MT_ERR_OUTRNG;
        }
        else
        {
            psFrame->bcd_wek_0 = psUser->ucWeek;
        }

        // 日
        if (psUser->ucDay > 31)
        {
            #ifdef MT_DBG
            DEBUG("emt_trans_YYWWMMDDhhmmss() ucDay MT_ERR_OUTRNG!");
            #endif
            return MT_ERR_OUTRNG;
        }
        else
        {
            psFrame->bcd_day_0 = ucmt_get_bcd_0(psUser->ucDay);
            psFrame->bcd_day_1 = ucmt_get_bcd_1(psUser->ucDay);
        }

        // 时
        if(psUser->ucHour > 24)
        {
            #ifdef MT_DBG
            DEBUG("emt_trans_YYWWMMDDhhmmss() ucHour MT_ERR_OUTRNG!");
            #endif
            return MT_ERR_OUTRNG;
        }
        else
        {
            psFrame->bcd_hur_0 = ucmt_get_bcd_0(psUser->ucHour);
            psFrame->bcd_hur_1 = ucmt_get_bcd_1(psUser->ucHour);
        }

        // 分
        if(psUser->ucMinute > 60)
        {
            #ifdef MT_DBG
            DEBUG("emt_trans_YYWWMMDDhhmmss() ucMinute MT_ERR_OUTRNG!");
            #endif
            return MT_ERR_OUTRNG;
        }
        else
        {
            psFrame->bcd_min_0 = ucmt_get_bcd_0(psUser->ucMinute);
            psFrame->bcd_min_1 = ucmt_get_bcd_1(psUser->ucMinute);
        }

        // 秒
        if(psUser->ucSecond > 60)
        {
            #ifdef MT_DBG
            DEBUG("emt_trans_YYWWMMDDhhmmss() ucSencond MT_ERR_OUTRNG!");
            #endif
            return MT_ERR_OUTRNG;
        }
        else
        {
            psFrame->bcd_sec_0 = ucmt_get_bcd_0(psUser->ucSecond);
            psFrame->bcd_sec_1 = ucmt_get_bcd_1(psUser->ucSecond);
        }
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_YYWWMMDDhhmmss() para error!");
        #endif
        return MT_ERR_PARA;
    }

    return MT_OK; 
}

/*****************************************************************************
 函 数 名  : emt_trans_sXXX
 功能描述  : 数据格式02 对于表A.2 格式: (+/-)XXX * 10(-3~4)  
 输入参数  : eMtTrans eTrans      
             float* psUser        
             sMtFmt02_f* psFrame  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月19日 星期一
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emt_trans_sXXX(eMtTrans eTrans, float* psUser, sMtsXXX* psFrame)
{
    if(!psFrame || !psUser)
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_sXXX() para pointer is null!");
        #endif
        return MT_ERR_NULL;
    }

    sMtsXXX   *psXXX  = (sMtsXXX *)psFrame;
    float     fUser   = *psUser;
    float     fTmp    = 0.0f;
    char      cBuf[6] = {0};
    int       nExp    = 0;    //  4 3 2 1 0 -1 -2 -3   

    // 用户侧到帧侧
    if(MT_TRANS_U2F == eTrans)
    {
        // 10(-3)
        if(fUser >= -0.999f && fUser <= 0.999f)
        {
            nExp = -3;
            fTmp = fUser * 1000.0f;
            psXXX->G1 = 1;
            psXXX->G2 = 1;
            psXXX->G3 = 1;
        }
        // 10(-2)
        else if((fUser >= -9.99f && fUser < -0.999f) || 
                (fUser >  0.999f && fUser <= 9.99f))
        {
            nExp = -2;
            fTmp = fUser * 100.0f;
            psXXX->G1 = 0;
            psXXX->G2 = 1;
            psXXX->G3 = 1;
        }
        // 10(-1)
        else if((fUser >= -99.9f && fUser < -9.99f) || 
                (fUser >   9.99f && fUser <= 99.9f))
        {
            nExp = -1;
            fTmp = fUser * 10.0f;
            psXXX->G1 = 1;
            psXXX->G2 = 0;
            psXXX->G3 = 1;
        }
        // 10(0)
        else if((fUser >= -999.0f && fUser < -99.9f) || 
                (fUser >    99.9f && fUser <= 999.0f))
        {
            nExp = 0;
            fTmp = fUser;
            psXXX->G1 = 0;
            psXXX->G2 = 0;
            psXXX->G3 = 1;
        }
        // 10(1)
        else if((fUser >= -9990.0f && fUser < -999.0f) || 
                (fUser >    999.0f && fUser <= 9990.0f))
        {
            nExp = 1;
            fTmp = fUser/10.0f;

            psXXX->G1 = 1;
            psXXX->G2 = 1;
            psXXX->G3 = 0;
        }
        // 10(2)
        else if((fUser >= -99900.0f && fUser < -9990.0f) || 
                (fUser >    9990.0f && fUser <= 99900.0f))
        {
            nExp = 2;
            fTmp = fUser/100.0f;
            psXXX->G1 = 0;
            psXXX->G2 = 1;
            psXXX->G3 = 0;
        }
        // 10(3)
        else if((fUser >= -999000.0f && fUser < -99900.0f) || 
                (fUser >    99900.0f && fUser <= 999000.0f))
        {
            nExp = 3;
            fTmp = fUser/1000.0f;
            psXXX->G1 = 1;
            psXXX->G2 = 0;
            psXXX->G3 = 0;
        }
        // 10(4)
        else if((fUser >= -9990000.0f && fUser < -999000.0f) || 
                (fUser >    999000.0f && fUser <= 9990000.0f))
        {
            nExp = 4;
            fTmp = fUser/10000.0f;
            psXXX->G1 = 0;
            psXXX->G2 = 0;
            psXXX->G3 = 0;
        }
        else
        {
            #ifdef MT_DBG
            DEBUG("emt_trans_sXXX() float out range %f!", fUser);
            #endif
            return MT_ERR_OUTRNG;
        }

        // 符号
        psXXX->S = bMtGetFloatSign(fTmp);
        vMtSetFloatSign(&fTmp, 0);

        // 有效数
        sprintf(cBuf, "%03.0f", fTmp);
        psXXX->BCD_2 = cBuf[0] - '0';
        psXXX->BCD_1 = cBuf[1] - '0';
        psXXX->BCD_0 = cBuf[2] - '0';

        }
        // 帧侧到用户侧
        else if(MT_TRANS_F2U == eTrans)
        {
            if(0 == psXXX->G3)
            {
                if(0 == psXXX->G2)
                {
                    if(0 == psXXX->G1)
                    {
                        fTmp =  10000.0f;
                    }
                    else
                    {
                        fTmp =  1000.0f;
                    }
                }
                else
                {
                    if(0 == psXXX->G1)
                    {
                        fTmp =  100.0f;
                    }
                    else
                    {
                        fTmp =  10.0f;
                    }
                }
            }
            else
            {
                if(0 == psXXX->G2)
                {
                    if(0 == psXXX->G1)
                    {
                        fTmp =  1.0f;
                    }
                    else
                    {
                        fTmp =  0.1f;
                    }
                }
                else
                {
                    if(0 == psXXX->G1)
                    {
                        fTmp =  0.01f;
                    }
                    else
                    {
                        fTmp =  0.001f;
                    }
                }
            }

            // 作为符号
            nExp    = (psXXX->S == 1)  ?  -1 : 1;
            fUser   = nExp * (psXXX->BCD_0 + psXXX->BCD_1 * 10.0f + psXXX->BCD_2 *100.0f);
            fUser   = fUser * fTmp;
            *psUser = fUser;

    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_sXXX() para error!");
        #endif
        return MT_ERR_PARA;
    }

    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emt_trans_sX7
 功能描述  : 数据格式03 对于表A.4
             格式: (+/1)XXXXXXX
 输入参数  : eMtTrans eTrans    
             sMtFmt_sX7* psUser      
             sMtFmt_sX7_f* psFrame  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月12日 星期一
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emt_trans_sX7(eMtTrans eTrans, sMtFmt_sX7* psUser, sMtFmt_sX7_f* psFrame)
{
    if(!psFrame || !psUser)
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_sx7() para pointer is null!");
        #endif
        return MT_ERR_NULL;
    }

    sMtFmt_sX7      *sX7     = (sMtFmt_sX7*)psUser; 
    sMtFmt_sX7_f    *sX7_f   = (sMtFmt_sX7_f *)psFrame;
    INT32           nData    = 0;
    UINT32          ulData   = 0;
    char           ucBuf[8] = {0};
 
    // 用户侧到帧侧
    if(MT_TRANS_U2F == eTrans)
    {
        nData = sX7->nData;
        if(nData < MT_SX7_MIN || nData > MT_SX7_MAX)
        {
            #ifdef MT_DBG
            DEBUG("emt_trans_sx7() nData is out range nData = %d", nData);
            #endif
            return MT_ERR_OUTRNG;
        }

        if(nData < 0)
        {
            ulData = nData * (-1);
            sX7_f->S = 1;
        }
        else
        {
            ulData = nData;
            sX7_f->S = 0;
        }
        
        sprintf(ucBuf, "%d", ulData);

        sX7_f->rev0  = 0;
        sX7_f->rev1  = 0;
        sX7_f->BCD_6 = ucBuf[0] - '0';
        sX7_f->BCD_5 = ucBuf[1] - '0';
        sX7_f->BCD_4 = ucBuf[2] - '0';
        sX7_f->BCD_3 = ucBuf[3] - '0';
        sX7_f->BCD_2 = ucBuf[4] - '0';
        sX7_f->BCD_1 = ucBuf[5] - '0';
        sX7_f->BCD_0 = ucBuf[6] - '0';

        // 单位
        if(MT_UINT_KWH_LI == sX7->eUnit)
        {
            sX7_f->G = 0;
        }
        else if(MT_UINT_MWH_YUAN == sX7->eUnit)
        {
            sX7_f->G = 1;
        }
        else
        { 
            #ifdef MT_DBG
            DEBUG("emt_trans_sx7() nData is eUnit = %d", sX7->eUnit);
            #endif
            return MT_ERR_OUTRNG;
        }
    }
    // 帧侧到用户侧
    else if(MT_TRANS_F2U == eTrans)
    {
        // 单位
        sX7->eUnit = (sX7_f->G == 0) ? MT_UINT_KWH_LI : MT_UINT_MWH_YUAN;

        // nData
        ulData += sX7_f->BCD_0;
        ulData += sX7_f->BCD_1 * 10;
        ulData += sX7_f->BCD_2 * 100;
        ulData += sX7_f->BCD_3 * 1000;
        ulData += sX7_f->BCD_4 * 10000;
        ulData += sX7_f->BCD_5 * 100000;
        ulData += sX7_f->BCD_6 * 1000000;

        // 符号
        nData = (sX7_f->S == 1 ) ? (-1)*ulData : ulData;
        sX7->nData = nData;
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_sx7() para error!");
        #endif
        return MT_ERR_PARA;
    }

    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emt_trans_sXX
 功能描述  : 数据格式04 对于表A.5
 数据格式  : (上浮/下浮)XX (0 ~ 79)
 输入参数  : eMtTrans eTrans    
             sMtsXX* psUser     
             sMtsXX_f* psFrame  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月12日 星期一
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emt_trans_sXX(eMtTrans eTrans, sMtsXX* psUser, sMtsXX_f* psFrame)
{
    if(!psFrame || !psUser)
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_sXX() para pointer is null!");
        #endif
        return MT_ERR_NULL;
    }

    UINT8 ucTmp = 0;

    // 用户侧到帧侧
    if(MT_TRANS_U2F == eTrans)
    {
        if(MT_FLOAT_DIR_UP == psUser->eDir)
        {
            psFrame->S0 = 0;
        }
        else if(MT_FLOAT_DIR_DOWN == psUser->eDir)
        {
            psFrame->S0 = 1;
        }
        else
        {
            #ifdef MT_DBG
            DEBUG("emt_trans_sXX() para eDir out range");
            #endif
            return MT_ERR_NULL;
        }

        ucTmp =  psUser->ucValue;
        if(ucTmp > MT_FLOAT_MAX || ucTmp < MT_FLOAT_MIN)
        {
            #ifdef MT_DBG
            DEBUG("emt_trans_sXX() para ucValue out range");
            #endif
            return MT_ERR_NULL;
        }

        psFrame->BCD_0 = ucmt_get_bcd_0(ucTmp);
        psFrame->BCD_1 = ucmt_get_bcd_1(ucTmp);

    }
    // 帧侧到用户侧
    else if(MT_TRANS_F2U == eTrans)
    {
        psUser->eDir = (psFrame->S0 == 0) ? MT_FLOAT_DIR_UP : MT_FLOAT_DIR_DOWN;
        ucTmp = psFrame->BCD_0 + psFrame->BCD_1 * 10;

        if(ucTmp > MT_FLOAT_MAX || ucTmp < MT_FLOAT_MIN)
        {
            #ifdef MT_DBG
            DEBUG("emt_trans_sXX() para ucValue out range");
            #endif
            return MT_ERR_NULL;
        }

        psUser->ucValue = ucTmp;
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_sXX() para error!");
        #endif
        return MT_ERR_PARA;
    }

    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emt_trans_sXXX_X
 功能描述  : 数据格式05 对于表A.6
             格式: (+/1)XXX.X
 输入参数  : eMtTrans eTrans    
             float* psUser      
             sMtFmt05* psFrame  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月12日 星期一
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emt_trans_sXXX_X(eMtTrans eTrans, float* psUser, sMtFmt05* psFrame)
{
    if(!psFrame || !psUser)
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_sXXX_X() para pointer is null!");
        #endif
        return MT_ERR_NULL;
    }

    sMtFmt_sXXX_X    *psFmtsXXX_X;
    float             *pfXXX_X;
    float             fZhengshu;  // 整数部分

    psFmtsXXX_X    = (sMtFmt_sXXX_X *)psFrame;
    pfXXX_X        = (float *)psUser;
    float fXXX_X   = *pfXXX_X;
    int sign       = 0; 

    // 用户侧到帧侧
    if(MT_TRANS_U2F == eTrans)
    {
        if(fXXX_X > 799.9f || fXXX_X < -799.9f)
        {
            return MT_ERR_OUTRNG;
        }

        modff(fXXX_X*10, &fZhengshu);
        sign = bMtGetFloatSign(fZhengshu);
        vMtSetFloatSign(&fZhengshu, 0);

        char buf[8] = {0};
        sprintf(buf, "%04.0lf\n", fZhengshu);

        //输出结构
        psFmtsXXX_X->S       = sign;
        psFmtsXXX_X->BCD_2   = buf[0] - '0';
        psFmtsXXX_X->BCD_1   = buf[1] - '0';
        psFmtsXXX_X->BCD_0   = buf[2] - '0';
        psFmtsXXX_X->BCD_0_1 = buf[3] - '0';  

    }
    // 帧侧到用户侧
    else if(MT_TRANS_F2U == eTrans)
    {
        UINT8    bcd[4];
        int      i;
        float    valid = 0.0f;
        bcd[0] = (UINT8)psFmtsXXX_X->BCD_0_1;
        bcd[1] = (UINT8)psFmtsXXX_X->BCD_0;
        bcd[2] = (UINT8)psFmtsXXX_X->BCD_1;
        bcd[3] = (UINT8)psFmtsXXX_X->BCD_2;

        for(i = 0; i < 4; i++)
        {
            valid+= bcd[i]*nMtPow(i);
        }

        fXXX_X = valid/10.0f;
        *pfXXX_X = fXXX_X * (psFmtsXXX_X->S == 0 ? 1.0f : -1.0f);
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_sXXX_X() para error!");
        #endif
        return MT_ERR_PARA;
    }

    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emt_trans_sXX_XX
 功能描述  : 数据格式06 对于表A.7
             格式: (+/1)XX.XX
 输入参数  : eMtTrans eTrans    
             float* psUser      
             sMtFmt06* psFrame  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月12日 星期一
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emt_trans_sXX_XX(eMtTrans eTrans, float* psUser, sMtFmt06* psFrame)
{
    if(!psFrame || !psUser)
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_sXX_XX() para pointer is null!");
        #endif
        return MT_ERR_NULL;
    }

    sMtFmt_sXX_XX    *psFmtsXX_XX;
    float             *pfXX_XX;
    float             fZhengshu;  // 整数部分

    psFmtsXX_XX    = (sMtFmt_sXX_XX *)psFrame;
    pfXX_XX        = (float *)psUser;
    float fXX_XX   = *pfXX_XX;
    int sign       = 0; 

    // 用户侧到帧侧
    if(MT_TRANS_U2F == eTrans)
    {
        if(fXX_XX > 79.99f || fXX_XX < -79.99f)
        {
            return MT_ERR_OUTRNG;
        }

        modff(fXX_XX*100, &fZhengshu);
        sign = bMtGetFloatSign(fZhengshu);
        vMtSetFloatSign(&fZhengshu, 0);

        char buf[8] = {0};
        sprintf(buf, "%04.0lf\n", fZhengshu);

        //输出结构
        psFmtsXX_XX->S       = sign;
        psFmtsXX_XX->BCD_1   = buf[0] - '0';
        psFmtsXX_XX->BCD_0   = buf[1] - '0';
        psFmtsXX_XX->BCD_0_1 = buf[2] - '0';
        psFmtsXX_XX->BCD_0_2 = buf[3] - '0';  

    }
    // 帧侧到用户侧
    else if(MT_TRANS_F2U == eTrans)
    {
        UINT8    bcd[4];
        int      i;
        float    valid = 0.0f;
        bcd[0] = (UINT8)psFmtsXX_XX->BCD_0_2;
        bcd[1] = (UINT8)psFmtsXX_XX->BCD_0_1;
        bcd[2] = (UINT8)psFmtsXX_XX->BCD_0;
        bcd[3] = (UINT8)psFmtsXX_XX->BCD_1;

        for(i = 0; i < 4; i++)
        {
            valid+= bcd[i]*nMtPow(i);
        }

        fXX_XX = valid/100.0f;
        *pfXX_XX = fXX_XX * (psFmtsXX_XX->S == 0 ? 1.0f : -1.0f);
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_sXX_XX() para error!");
        #endif
        return MT_ERR_PARA;
    }

    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emt_trans_XXX_X
 功能描述  : 数据格式07 对于表A.8 格式: (+)XXX.X
 输入参数  : eMtTrans eTrans    
             float* psUser      
             sMtFmt07* psFrame  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月12日 星期一
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emt_trans_XXX_X(eMtTrans eTrans, float* psUser, sMtFmt07* psFrame)
{
    if(!psFrame || !psUser)
    {
         #ifdef MT_DBG
         DEBUG("emt_trans_XXX_X() para pointer is null!");
         #endif
         return MT_ERR_NULL;
    }

    sMtFmt_XXX_X    *psFmtXXX_X;
    float             *pfXXX_X;
    float             fZhengshu;  // 整数部分

    psFmtXXX_X    = (sMtFmt_XXX_X *)psFrame;
    pfXXX_X        = (float *)psUser;
    float fXXX_X   = *pfXXX_X;

    // 用户侧到帧侧
    if(MT_TRANS_U2F == eTrans)
    {
        if(fXXX_X > 999.9f || fXXX_X < 0.0f)
        {
            return MT_ERR_OUTRNG;
        }

        modff(fXXX_X*10, &fZhengshu);
        vMtSetFloatSign(&fZhengshu, 0);
        char buf[8] = {0};
        sprintf(buf, "%04.0lf\n", fZhengshu);

        //输出结构
        psFmtXXX_X->BCD_2   = buf[0] - '0';
        psFmtXXX_X->BCD_1   = buf[1] - '0';
        psFmtXXX_X->BCD_0   = buf[2] - '0';
        psFmtXXX_X->BCD_0_1 = buf[3] - '0';  

    }
    // 帧侧到用户侧
    else if(MT_TRANS_F2U == eTrans)
    {
        UINT8    bcd[4];
        int      i;
        float    valid = 0.0f;
        bcd[0] = (UINT8)psFmtXXX_X->BCD_0_1;
        bcd[1] = (UINT8)psFmtXXX_X->BCD_0;
        bcd[2] = (UINT8)psFmtXXX_X->BCD_1;
        bcd[3] = (UINT8)psFmtXXX_X->BCD_2;

        for(i = 0; i < 4; i++)
        {
            valid+= bcd[i]*nMtPow(i);
        }

        fXXX_X   = valid / 10.0f;
        *pfXXX_X = fXXX_X;
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_XXX_X() para error!");
        #endif
        return MT_ERR_PARA;
    }

    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emt_trans_XXXX
 功能描述  : 数据格式08 对于表A.9 格式: XXXX
 输入参数  : eMtTrans eTrans    
             UINT16* psUser      
             sMtFmt08* psFrame  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月12日 星期一
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emt_trans_XXXX(eMtTrans eTrans, UINT16* psUser, sMtFmt08* psFrame)
{
    if(!psFrame || !psUser)
    {
         #ifdef MT_DBG
         DEBUG("emt_trans_XXXX() para pointer is null!");
         #endif
         return MT_ERR_NULL;
    }

    sMtFmt08    *psFmtXXxx;
    UINT16       *pusXXxx;
   
    psFmtXXxx       = (sMtFmt08 *)psFrame;
    pusXXxx         = (UINT16 *)psUser;
    UINT16 usXXxx   = *pusXXxx;

    // 用户侧到帧侧
    if(MT_TRANS_U2F == eTrans)
    {
        if(usXXxx > (nMtPow(4) - 1) || usXXxx < 0.0f)
        {
            return MT_ERR_OUTRNG;
        }
        
        //输出结构
        psFmtXXxx->BCD_1   = usXXxx / nMtPow(3);
        usXXxx = usXXxx % nMtPow(3);
        psFmtXXxx->BCD_2   = usXXxx / nMtPow(2);
        usXXxx = usXXxx % nMtPow(2);
        psFmtXXxx->BCD_3   = usXXxx / nMtPow(1);
        usXXxx = usXXxx % nMtPow(1);
        psFmtXXxx->BCD_4 = usXXxx;
    }
    // 帧侧到用户侧
    else if(MT_TRANS_F2U == eTrans)
    {
        UINT8    bcd[4];
        int      i;
        UINT16   valid = 0;
        
        bcd[0] = (UINT8)psFmtXXxx->BCD_4;
        bcd[1] = (UINT8)psFmtXXxx->BCD_3;
        bcd[2] = (UINT8)psFmtXXxx->BCD_2;
        bcd[3] = (UINT8)psFmtXXxx->BCD_1;

        for(i = 0; i < 4; i++)
        {
            valid+= bcd[i]*nMtPow(i);
        }

        *pusXXxx = valid;
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_XXXX() para error!");
        #endif
        return MT_ERR_PARA;
    }

    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emt_trans_sXX_XXXX
 功能描述  : 数据格式09 对于表A.10
 输入参数  : eMtTrans eTrans      
             float* psUser     
             sMtFmt09* psFrame  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月12日 星期一
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emt_trans_sXX_XXXX(eMtTrans eTrans, float* psUser, sMtFmt09* psFrame)
{
    if(!psFrame || !psUser)
     {
        #ifdef MT_DBG
        DEBUG("emt_trans_sXX_XXXX() para pointer is null!");
        #endif
        return MT_ERR_NULL;
     }

    sMtFmt_sXX_XXXX   *psFmtsXX_XXXX;
    float             *pfXX_XXXX;
    float             fZhengshu;  // 整数部分

    psFmtsXX_XXXX  = (sMtFmt_sXX_XXXX *)psFrame;
    pfXX_XXXX      = (float *)psUser;
    float fXX_XXXX = *pfXX_XXXX;
    int sign       = 0; 
 
    // 用户侧到帧侧
    if(MT_TRANS_U2F == eTrans)
    {
        if(fXX_XXXX > 79.9999f || fXX_XXXX < -79.9999f)
        {
            return MT_ERR_OUTRNG;
        }

        modff(fXX_XXXX * 10000.0f, &fZhengshu);
        sign = bMtGetFloatSign(fZhengshu);
        vMtSetFloatSign(&fZhengshu, 0);
    
        char buf[8] = {0};
        sprintf(buf, "%06.0lf\n", fZhengshu);

        //输出结构
        psFmtsXX_XXXX->S       = sign;
        psFmtsXX_XXXX->BCD_1   = buf[0] - '0';
        psFmtsXX_XXXX->BCD_0   = buf[1] - '0';
        psFmtsXX_XXXX->BCD_0_1 = buf[2] - '0';
        psFmtsXX_XXXX->BCD_0_2 = buf[3] - '0';  
        psFmtsXX_XXXX->BCD_0_3 = buf[4] - '0';
        psFmtsXX_XXXX->BCD_0_4 = buf[5] - '0';
    }
    // 帧侧到用户侧
    else if(MT_TRANS_F2U == eTrans)
    {
        UINT8    bcd[6];
        int      i;
        float    valid = 0.0f;
        bcd[0] = (UINT8)psFmtsXX_XXXX->BCD_0_4;
        bcd[1] = (UINT8)psFmtsXX_XXXX->BCD_0_3;
        bcd[2] = (UINT8)psFmtsXX_XXXX->BCD_0_2;
        bcd[3] = (UINT8)psFmtsXX_XXXX->BCD_0_1;
        bcd[4] = (UINT8)psFmtsXX_XXXX->BCD_0;
        bcd[5] = (UINT8)psFmtsXX_XXXX->BCD_1;

        for(i = 0; i < 6; i++)
        {
            valid+= bcd[i]*nMtPow(i);
        }

        fXX_XXXX = valid/10000.0f;
        *pfXX_XXXX = fXX_XXXX * (psFmtsXX_XXXX->S  == 0 ? 1.0f : -1.0f);

    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_sXX_XXXX() para error!");
        #endif
        return MT_ERR_PARA;
    }

    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emt_trans_XXXXXX
 功能描述  : 数据格式10 对于表A.11 格式:  XXXXXX
 输入参数  : eMtTrans eTrans    
             float* psUser      
             sMtFmt07* psFrame  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月22日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emt_trans_XXXXXX(eMtTrans eTrans, UINT32* psUser, sMtFmt_XXXXXX* psFrame)
{
    if(!psFrame || !psUser)
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_XXXXXX() para pointer is null!");
        #endif
        return MT_ERR_NULL;
    } 

    sMtFmt_XXXXXX   *psFmtXXXXXX;
    UINT32          *puiXXXXXX;

    psFmtXXXXXX     = (sMtFmt_XXXXXX *)psFrame;
    puiXXXXXX       = (UINT32 *)psUser;
    UINT32 uiXXXXXX = *puiXXXXXX;
    
    // 用户侧到帧侧
    if(MT_TRANS_U2F == eTrans)
    {
        if((uiXXXXXX > (UINT32)(nMtPow(6) - 1)) || (uiXXXXXX < 0))
        {
            return MT_ERR_OUTRNG;
        }
        
        //输出结构
        psFmtXXXXXX->BCD_1 = uiXXXXXX / nMtPow(5);
        uiXXXXXX = uiXXXXXX % nMtPow(5);
        psFmtXXXXXX->BCD_2 = uiXXXXXX / nMtPow(4);
        uiXXXXXX = uiXXXXXX % nMtPow(4);
        psFmtXXXXXX->BCD_3 = uiXXXXXX / nMtPow(3);
        uiXXXXXX = uiXXXXXX % nMtPow(3);
        psFmtXXXXXX->BCD_4 = uiXXXXXX / nMtPow(2);
        uiXXXXXX = uiXXXXXX % nMtPow(2);
        psFmtXXXXXX->BCD_5 = uiXXXXXX / nMtPow(1);
        uiXXXXXX = uiXXXXXX % nMtPow(1);
        psFmtXXXXXX->BCD_6 = uiXXXXXX;
    }
    // 帧侧到用户侧
    else if(MT_TRANS_F2U == eTrans)
    {
        UINT8    bcd[6];
        UINT32    valid = 0;
        UINT8     i;
        bcd[0] = (UINT8)psFmtXXXXXX->BCD_6;
        bcd[1] = (UINT8)psFmtXXXXXX->BCD_5;
        bcd[2] = (UINT8)psFmtXXXXXX->BCD_4;
        bcd[3] = (UINT8)psFmtXXXXXX->BCD_3;
        bcd[4] = (UINT8)psFmtXXXXXX->BCD_2;
        bcd[5] = (UINT8)psFmtXXXXXX->BCD_1;

        for(i = 0; i < sizeof(bcd); i++)
        {
            valid += bcd[i] * nMtPow(i);
        }

        *puiXXXXXX = valid;
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_XXXXXX() para error!");
        #endif
        return MT_ERR_PARA;
    }

    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emt_trans_XX_6
 功能描述  : 数据格式12 对于表A.13  
 数据格式  : XXXXXXXXXXXX
 输入参数  : eMtTrans eTrans       
             UINT8* psUser        
             sMtFmt_XX_6* psFrame  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月15日 星期四
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emt_trans_XX_6(eMtTrans eTrans, UINT8* psUser, sMtFmt_XX_6* psFrame)
{
    if(!psFrame || !psUser)
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_XX_6() para pointer is null!");
        #endif
        return MT_ERR_NULL;
    }

    // 用户侧到帧侧
    if(MT_TRANS_U2F == eTrans)
    {
        psFrame->BCD_11    = psUser[0]  - '0';
        psFrame->BCD_10    = psUser[1]  - '0';
        psFrame->BCD_9     = psUser[2]  - '0';
        psFrame->BCD_8     = psUser[3]  - '0'; 
        psFrame->BCD_7     = psUser[4]  - '0';
        psFrame->BCD_6     = psUser[5]  - '0';
        psFrame->BCD_5     = psUser[6]  - '0';
        psFrame->BCD_4     = psUser[7]  - '0'; 
        psFrame->BCD_3     = psUser[8]  - '0';
        psFrame->BCD_2     = psUser[9]  - '0';
        psFrame->BCD_1     = psUser[10] - '0';
        psFrame->BCD_0     = psUser[11] - '0'; 
        
    }
    // 帧侧到用户侧
    else if(MT_TRANS_F2U == eTrans)
    {
        psUser[0]  = (UINT8)(psFrame->BCD_11)  + '0';
        psUser[1]  = (UINT8)(psFrame->BCD_10)  + '0';
        psUser[2]  = (UINT8)(psFrame->BCD_9)   + '0';
        psUser[3]  = (UINT8)(psFrame->BCD_8)   + '0';
        psUser[4]  = (UINT8)(psFrame->BCD_7)   + '0';
        psUser[5]  = (UINT8)(psFrame->BCD_6)   + '0';
        psUser[6]  = (UINT8)(psFrame->BCD_5)   + '0';
        psUser[7]  = (UINT8)(psFrame->BCD_4)   + '0';
        psUser[8]  = (UINT8)(psFrame->BCD_3)   + '0';
        psUser[9]  = (UINT8)(psFrame->BCD_2)   + '0';
        psUser[10] = (UINT8)(psFrame->BCD_1)   + '0';
        psUser[11] = (UINT8)(psFrame->BCD_0)   + '0';
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_XX_6() para error!");
        #endif
        return MT_ERR_PARA;
    }

    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emt_trans_sXXX_XXX
 功能描述  : 数据格式25 对于表A.26 格式: (+/1)XXX.XXX
 输入参数  : eMtTrans eTrans    
             float* psUser      
             sMtFmt25* psFrame  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月12日 星期一
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emt_trans_sXXX_XXX(eMtTrans eTrans, float* psUser, sMtFmt25* psFrame)
{
    if(!psFrame || !psUser)
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_sXXX_XXX() para pointer is null!");
        #endif
        return MT_ERR_NULL;
    }

    sMtFmt_sXXX_XXX   *psFmtsXXX_XXX;
    float             *pfXXX_XXX;
    float             fZhengshu;  // 整数部分

    psFmtsXXX_XXX  = (sMtFmt_sXXX_XXX *)psFrame;
    pfXXX_XXX      = (float *)psUser;
    float fXXX_XXX = *pfXXX_XXX;
    int sign       = 0; 
 
    // 用户侧到帧侧
    if(MT_TRANS_U2F == eTrans)
    {
        if(fXXX_XXX > 799.999f || fXXX_XXX < -799.999f)
        {
            return MT_ERR_OUTRNG;
        }

        modff(fXXX_XXX*1000.0f, &fZhengshu);
        sign = bMtGetFloatSign(fZhengshu);     // 获得符号位
        vMtSetFloatSign(&fZhengshu, 0);        // 设置为正

        char buf[8] = {0};
        sprintf(buf, "%06.0lf\n", fZhengshu);

        //输出结构
        psFmtsXXX_XXX->S       = sign;
        psFmtsXXX_XXX->BCD_2   = buf[0] - '0';
        psFmtsXXX_XXX->BCD_1   = buf[1] - '0';
        psFmtsXXX_XXX->BCD_0   = buf[2] - '0';
        psFmtsXXX_XXX->BCD_0_1 = buf[3] - '0';  
        psFmtsXXX_XXX->BCD_0_2 = buf[4] - '0';
        psFmtsXXX_XXX->BCD_0_3 = buf[5] - '0';
     }
     // 帧侧到用户侧
     else if(MT_TRANS_F2U == eTrans)
     {
        UINT8    bcd[6];
        int      i;
        float    valid = 0.0f;
        bcd[0] = (UINT8)psFmtsXXX_XXX->BCD_0_3;
        bcd[1] = (UINT8)psFmtsXXX_XXX->BCD_0_2;
        bcd[2] = (UINT8)psFmtsXXX_XXX->BCD_0_1;
        bcd[3] = (UINT8)psFmtsXXX_XXX->BCD_0;
        bcd[4] = (UINT8)psFmtsXXX_XXX->BCD_1;
        bcd[5] = (UINT8)psFmtsXXX_XXX->BCD_2;

        for(i = 0; i < 6; i++)
        {
            valid+= bcd[i]*nMtPow(i);
        }

        fXXX_XXX = valid/1000.0f;
        *pfXXX_XXX = fXXX_XXX * (psFmtsXXX_XXX->S  == 0 ? 1.0f : -1.0f);

    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_sXXX_XXX() para error!");
        #endif
        return MT_ERR_PARA;
    }

    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emt_trans_XXXXXX_XX
 功能描述  : 数据格式11 对于表A.12 格式: (+)XXXXXX.XX
 输入参数  : eMtTrans eTrans            
             float* psUser              
             sMtFmt_XXXXXX_XX* psFrame  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月12日 星期一
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emt_trans_XXXXXX_XX(eMtTrans eTrans, double* psUser, sMtFmt_XXXXXX_XX* psFrame)
{
    if(!psFrame || !psUser)
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_XXXXXX_XX() para pointer is null!");
        #endif
        return MT_ERR_NULL;
    }

    sMtFmt_XXXXXX_XX    *psFmtXXXXXX_XX;
    double              *pdXXXXXX_XX;
   
    psFmtXXXXXX_XX      = (sMtFmt_XXXXXX_XX *)psFrame;
    pdXXXXXX_XX         = (double *)psUser;
    double dXXXXXX_XX   = *pdXXXXXX_XX;
   
    // 用户侧到帧侧
    if(MT_TRANS_U2F == eTrans)
    {
        if(dXXXXXX_XX > 999999.99 || dXXXXXX_XX < -999999.99)
        {
            return MT_ERR_OUTRNG;
        }
        char buf[5] = {0};
        //输出结构
        psFmtXXXXXX_XX->BCD_5     = (UINT8)(dXXXXXX_XX / nMtPow(5));
        dXXXXXX_XX -= psFmtXXXXXX_XX->BCD_5 * nMtPow(5);
        psFmtXXXXXX_XX->BCD_4     = (UINT8)(dXXXXXX_XX / nMtPow(4));
        dXXXXXX_XX -= psFmtXXXXXX_XX->BCD_4 * nMtPow(4);
        psFmtXXXXXX_XX->BCD_3     = (UINT8)(dXXXXXX_XX / nMtPow(3));
        dXXXXXX_XX -= psFmtXXXXXX_XX->BCD_3 * nMtPow(3);
        psFmtXXXXXX_XX->BCD_2     = (UINT8)(dXXXXXX_XX / nMtPow(2));
        dXXXXXX_XX -= psFmtXXXXXX_XX->BCD_2 * nMtPow(2);
        psFmtXXXXXX_XX->BCD_1     = (UINT8)(dXXXXXX_XX / nMtPow(1));
        dXXXXXX_XX -= psFmtXXXXXX_XX->BCD_1 * nMtPow(1);
        psFmtXXXXXX_XX->BCD_0     = (UINT32)dXXXXXX_XX % nMtPow(1);
        dXXXXXX_XX -= psFmtXXXXXX_XX->BCD_0;
        sprintf(buf,"%2.2f",dXXXXXX_XX);
        psFmtXXXXXX_XX->BCD_0_1   =  buf[2] - '0';
        psFmtXXXXXX_XX->BCD_0_2   =  buf[3] - '0';  
 
     }
     // 帧侧到用户侧
     else if(MT_TRANS_F2U == eTrans)
     {
        UINT8    bcd[8];
        int      i;
        UINT32   uiInt,uiFrac;

        uiInt = 0;
        uiFrac = 0;
        
        bcd[0] = (UINT8)psFmtXXXXXX_XX->BCD_0_2;
        bcd[1] = (UINT8)psFmtXXXXXX_XX->BCD_0_1;
        bcd[2] = (UINT8)psFmtXXXXXX_XX->BCD_0;
        bcd[3] = (UINT8)psFmtXXXXXX_XX->BCD_1;
        bcd[4] = (UINT8)psFmtXXXXXX_XX->BCD_2;
        bcd[5] = (UINT8)psFmtXXXXXX_XX->BCD_3;
        bcd[6] = (UINT8)psFmtXXXXXX_XX->BCD_4;
        bcd[7] = (UINT8)psFmtXXXXXX_XX->BCD_5;
        
        for(i = 0; i < 2; i++)
        {
            uiFrac += bcd[i]*nMtPow(i);
        }

        for(; i < 8; i++)
        {
            uiInt+= bcd[i]*nMtPow(i - 2);
        }

        dXXXXXX_XX = (DOUBLE)uiInt + (DOUBLE)uiFrac/100.0;
        *pdXXXXXX_XX = dXXXXXX_XX;
     }
    else
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_XXXXXX_XX() para error!");
        #endif
        return MT_ERR_PARA;
    }

    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emt_trans_XXXX_XXXX
 功能描述  : 数据格式13 对于表A.14 格式: (+)XXXX.XXXX
 输入参数  : eMtTrans eTrans            
             float* psUser              
             sMtFmt_XXXX_XXXX* psFrame  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月12日 星期一
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emt_trans_XXXX_XXXX(eMtTrans eTrans, double* psUser, sMtFmt_XXXX_XXXX* psFrame)
{
    if(!psFrame || !psUser)
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_XXXX_XXXX() para pointer is null!");
        #endif
        return MT_ERR_NULL;
    }

    sMtFmt_XXXX_XXXX    *psFmtXXXX_XXXX;
    double              *pdXXXX_XXXX;
    double              dZhengshu;  // 整数部分

    psFmtXXXX_XXXX      = (sMtFmt_XXXX_XXXX *)psFrame;
    pdXXXX_XXXX         = (double *)psUser;
    double dXXXX_XXXX   = *pdXXXX_XXXX;
 
    // 用户侧到帧侧
    if(MT_TRANS_U2F == eTrans)
    {
        if(dXXXX_XXXX > 9999.9999 || dXXXX_XXXX < -9999.9999)
        {
            return MT_ERR_OUTRNG;
        }

        modf(dXXXX_XXXX*10000, &dZhengshu);
        vMtSetDoubleSign(&dZhengshu, 1);
        char buf[10] = {0};
        sprintf(buf, "%08.0lf\n", dZhengshu);

        //输出结构
        psFmtXXXX_XXXX->BCD_3     = buf[0] - '0';
        psFmtXXXX_XXXX->BCD_2     = buf[1] - '0';
        psFmtXXXX_XXXX->BCD_1     = buf[2] - '0';
        psFmtXXXX_XXXX->BCD_0     = buf[3] - '0';  
        psFmtXXXX_XXXX->BCD_0_1   = buf[4] - '0';  
        psFmtXXXX_XXXX->BCD_0_2   = buf[5] - '0';  
        psFmtXXXX_XXXX->BCD_0_3   = buf[6] - '0';  
        psFmtXXXX_XXXX->BCD_0_4   = buf[7] - '0';  

    }
    // 帧侧到用户侧
    else if(MT_TRANS_F2U == eTrans)
    {
        UINT8    bcd[8];
        int      i;
        double    valid = 0.0;

        bcd[0] = (UINT8)psFmtXXXX_XXXX->BCD_0_4;
        bcd[1] = (UINT8)psFmtXXXX_XXXX->BCD_0_3;
        bcd[2] = (UINT8)psFmtXXXX_XXXX->BCD_0_2;
        bcd[3] = (UINT8)psFmtXXXX_XXXX->BCD_0_1;
        bcd[4] = (UINT8)psFmtXXXX_XXXX->BCD_0;
        bcd[5] = (UINT8)psFmtXXXX_XXXX->BCD_1;
        bcd[6] = (UINT8)psFmtXXXX_XXXX->BCD_2;
        bcd[7] = (UINT8)psFmtXXXX_XXXX->BCD_3;

        for(i = 0; i < 8; i++)
        {
            valid += bcd[i]*nMtPow(i);
        }

        dXXXX_XXXX = valid/10000.0;
        *pdXXXX_XXXX = dXXXX_XXXX;
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_XXXX_XXXX() para error!");
        #endif
        return MT_ERR_PARA;
    }

    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emt_trans_XXXXXX_XXXX
 功能描述  : 数据格式14 对于表A.15 格式: (+)XXXXXX.XXXX
 输入参数  : eMtTrans eTrans            
             float* psUser              
             sMtFmt_XXXXXX_XXXX* psFrame  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月12日 星期一
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emt_trans_XXXXXX_XXXX(eMtTrans eTrans, double* psUser, sMtFmt_XXXXXX_XXXX* psFrame)
{
    if(!psFrame || !psUser)
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_XXXXXX_XXXX() para pointer is null!");
        #endif
        return MT_ERR_NULL;
    }

    sMtFmt_XXXXXX_XXXX    *psFmtXXXXXX_XXXX;
    double                *pdXXXXXX_XXXX;
  
    psFmtXXXXXX_XXXX      = (sMtFmt_XXXXXX_XXXX *)psFrame;
    pdXXXXXX_XXXX         = (double *)psUser;
    double dXXXXXX_XXXX   = *pdXXXXXX_XXXX;
 
     // 用户侧到帧侧
    if(MT_TRANS_U2F == eTrans)
    {
        if(dXXXXXX_XXXX > 999999.9999 || dXXXXXX_XXXX < -999999.9999)
        {
            return MT_ERR_OUTRNG;
        }
        CHAR buf[7] = {0};
            
        //输出结构
        psFmtXXXXXX_XXXX->BCD_5     = (UINT8)(dXXXXXX_XXXX / nMtPow(5));
        dXXXXXX_XXXX -= psFmtXXXXXX_XXXX->BCD_5 * nMtPow(5);
        psFmtXXXXXX_XXXX->BCD_4     = (UINT8)(dXXXXXX_XXXX / nMtPow(4));
        dXXXXXX_XXXX -= psFmtXXXXXX_XXXX->BCD_4 * nMtPow(4);
        psFmtXXXXXX_XXXX->BCD_3     = (UINT8)(dXXXXXX_XXXX / nMtPow(3));
        dXXXXXX_XXXX -= psFmtXXXXXX_XXXX->BCD_3 * nMtPow(3);
        psFmtXXXXXX_XXXX->BCD_2     = (UINT8)(dXXXXXX_XXXX / nMtPow(2));  
        dXXXXXX_XXXX -= psFmtXXXXXX_XXXX->BCD_2 * nMtPow(2);
        psFmtXXXXXX_XXXX->BCD_1     = (UINT8)(dXXXXXX_XXXX / nMtPow(1)); 
        dXXXXXX_XXXX -= psFmtXXXXXX_XXXX->BCD_1 * nMtPow(1);
        psFmtXXXXXX_XXXX->BCD_0     = (UINT8)dXXXXXX_XXXX % nMtPow(1);  
        dXXXXXX_XXXX -= psFmtXXXXXX_XXXX->BCD_0;
     
        sprintf(buf,"%4.4f",dXXXXXX_XXXX);
        psFmtXXXXXX_XXXX->BCD_0_1   = buf[2] - '0';
        psFmtXXXXXX_XXXX->BCD_0_2   = buf[3] - '0';
        psFmtXXXXXX_XXXX->BCD_0_3   = buf[4] - '0';  
        psFmtXXXXXX_XXXX->BCD_0_4   = buf[5] - '0';  

    }
    // 帧侧到用户侧
    else if(MT_TRANS_F2U == eTrans)
    {
        UINT8    bcd[10];
        int      i;
        UINT32   uiInt,uiFrac;

        uiInt = 0;
        uiFrac = 0;
        
        bcd[0] = (UINT8)psFmtXXXXXX_XXXX->BCD_0_4;
        bcd[1] = (UINT8)psFmtXXXXXX_XXXX->BCD_0_3;
        bcd[2] = (UINT8)psFmtXXXXXX_XXXX->BCD_0_2;
        bcd[3] = (UINT8)psFmtXXXXXX_XXXX->BCD_0_1;
        bcd[4] = (UINT8)psFmtXXXXXX_XXXX->BCD_0;
        bcd[5] = (UINT8)psFmtXXXXXX_XXXX->BCD_1;
        bcd[6] = (UINT8)psFmtXXXXXX_XXXX->BCD_2;
        bcd[7] = (UINT8)psFmtXXXXXX_XXXX->BCD_3;
        bcd[8] = (UINT8)psFmtXXXXXX_XXXX->BCD_4;
        bcd[9] = (UINT8)psFmtXXXXXX_XXXX->BCD_5;

        for(i = 0; i < 4; i++)
        {
            uiFrac += bcd[i]*nMtPow(i);
        }

        for ( ;i < 10; i++)
        {
            uiInt += bcd[i]*nMtPow(i - 4);
        }

        dXXXXXX_XXXX = (DOUBLE)uiInt + (DOUBLE)uiFrac/10000.0;
        *pdXXXXXX_XXXX = dXXXXXX_XXXX;
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_XXXXXX_XXXX() para error!");
        #endif
        return MT_ERR_PARA;
    }

    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emt_trans_YYMMDD
 功能描述  : 数据格式转换函数  数据格式20 对于表A.21 单位: 年月日
 输入参数  : eMtTrans eTrans           
             sMtYYMMDD* psUser     
             sMtYYMMDD_f* psFrame  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月12日 星期一
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emt_trans_YYMMDD(eMtTrans eTrans, sMtYYMMDD* psUser, sMtYYMMDD_f* psFrame)
{
    if(!psUser || !psFrame)
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_YYMMDD() para pointer is null!");
        #endif
        return MT_ERR_NULL;
    }
    
    if(MT_TRANS_U2F == eTrans)
    {     
        // 年
        psFrame->bcd_YY_0 = ucmt_get_bcd_0(psUser->ucYY);
        psFrame->bcd_YY_1 = ucmt_get_bcd_1(psUser->ucYY);

        // 月
        if(psUser->ucMM > 12)
        {
            #ifdef MT_DBG
            DEBUG("emt_trans_YYMMDD() ucMM MT_ERR_OUTRNG!");
            #endif
            return MT_ERR_OUTRNG;
        }
        else
        {
            psFrame->bcd_MM_0 = ucmt_get_bcd_0(psUser->ucMM);
            psFrame->bcd_MM_1 = ucmt_get_bcd_1(psUser->ucMM);
        }

        // 日
        if(psUser->ucDD > 31)
        {
            #ifdef MT_DBG
            DEBUG("emt_trans_YYMMDD() ucDD MT_ERR_OUTRNG!");
            #endif
            return MT_ERR_OUTRNG;
        }
        else
        {
            psFrame->bcd_DD_0 = ucmt_get_bcd_0(psUser->ucDD);
            psFrame->bcd_DD_1 = ucmt_get_bcd_1(psUser->ucDD);
        }       
    }
    else if(MT_TRANS_F2U == eTrans)
    {
        psUser->ucYY = psFrame->bcd_YY_0 + psFrame->bcd_YY_1 * 10;
        psUser->ucMM = psFrame->bcd_MM_0 + psFrame->bcd_MM_1 * 10;
        psUser->ucDD = psFrame->bcd_DD_0 + psFrame->bcd_DD_1 * 10;
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("ucmt_get_bcd_0() para error!");
        #endif
        return MT_ERR_PARA;
    }
        
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emt_trans_YYMM
 功能描述  : 数据格式转换函数  数据格式21 对于表A.22 单位: 年月
 输入参数  : eMtTrans eTrans           
             sMtYYMM* psUser     
             sMtYYMM_f* psFrame  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月12日 星期一
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emt_trans_YYMM(eMtTrans eTrans, sMtYYMM* psUser, sMtYYMM_f* psFrame)
{
    if(!psUser || !psFrame)
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_YYMM() para pointer is null!");
        #endif
        return MT_ERR_NULL;
    }

    if(MT_TRANS_U2F == eTrans)
    {     
        // 年
        psFrame->bcd_YY_0 = ucmt_get_bcd_0(psUser->ucYY);
        psFrame->bcd_YY_1 = ucmt_get_bcd_1(psUser->ucYY);

        // 月
        if(psUser->ucMM > 12)
        {
            #ifdef MT_DBG
            DEBUG("emt_trans_YYMM() ucMM MT_ERR_OUTRNG!");
            #endif
            return MT_ERR_OUTRNG;
        }
        else
        {
            psFrame->bcd_MM_0 = ucmt_get_bcd_0(psUser->ucMM);
            psFrame->bcd_MM_1 = ucmt_get_bcd_1(psUser->ucMM);
        }
    }
    else if(MT_TRANS_F2U == eTrans)
    {
        psUser->ucYY = psFrame->bcd_YY_0 + psFrame->bcd_YY_1 * 10;
        psUser->ucMM = psFrame->bcd_MM_0 + psFrame->bcd_MM_1 * 10;
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("ucmt_get_bcd_0() para error!");
        #endif
        return MT_ERR_PARA;
    }

    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emt_trans_YYMMDDhhmm
 功能描述  : 数据格式转换函数  数据格式15 对于表A.16 单位: 年月日时分
 输入参数  : eMtTrans eTrans           
             sMtYYMMDDhhmm* psUser     
             sMtYYMMDDhhmm_f* psFrame  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月12日 星期一
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emt_trans_YYMMDDhhmm(eMtTrans eTrans, sMtYYMMDDhhmm* psUser, sMtYYMMDDhhmm_f* psFrame)
{
    if(!psUser || !psFrame)
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_YYMMDDhhmm() para pointer is null!");
        #endif
        return MT_ERR_NULL;
    }
    
    if(MT_TRANS_U2F == eTrans)
    {     
        // 年
        psFrame->bcd_YY_0 = ucmt_get_bcd_0(psUser->ucYY);
        psFrame->bcd_YY_1 = ucmt_get_bcd_1(psUser->ucYY);

        // 月
        if(psUser->ucMM > 12)
        {
            #ifdef MT_DBG
            DEBUG("emt_trans_YYMMDDhhmm() ucMM MT_ERR_OUTRNG!");
            #endif
            return MT_ERR_OUTRNG;
        }
        else
        {
            psFrame->bcd_MM_0 = ucmt_get_bcd_0(psUser->ucMM);
            psFrame->bcd_MM_1 = ucmt_get_bcd_1(psUser->ucMM);
        }

        // 日
        if(psUser->ucDD > 31)
        {
            #ifdef MT_DBG
            DEBUG("emt_trans_YYMMDDhhmm() ucDD MT_ERR_OUTRNG!");
            #endif
            return MT_ERR_OUTRNG;
        }
        else
        {
            psFrame->bcd_DD_0 = ucmt_get_bcd_0(psUser->ucDD);
            psFrame->bcd_DD_1 = ucmt_get_bcd_1(psUser->ucDD);
        }

        // 时
        if(psUser->ucHH > 24)
        {
            #ifdef MT_DBG
            DEBUG("emt_trans_YYMMDDhhmm() ucHH MT_ERR_OUTRNG!");
            #endif
            return MT_ERR_OUTRNG;
        }
        else
        {
            psFrame->bcd_hh_0 = ucmt_get_bcd_0(psUser->ucHH);
            psFrame->bcd_hh_1 = ucmt_get_bcd_1(psUser->ucHH);
        }

        // 分
        if(psUser->ucmm > 60)
        {
            #ifdef MT_DBG
            DEBUG("emt_trans_YYMMDDhhmm() ucmm MT_ERR_OUTRNG!");
            #endif
            return MT_ERR_OUTRNG;
        }
        else
        {
            psFrame->bcd_mm_0 = ucmt_get_bcd_0(psUser->ucmm);
            psFrame->bcd_mm_1 = ucmt_get_bcd_1(psUser->ucmm);
        }
           
    }
    else if(MT_TRANS_F2U == eTrans)
    {
        psUser->ucYY = psFrame->bcd_YY_0 + psFrame->bcd_YY_1 * 10;
        psUser->ucMM = psFrame->bcd_MM_0 + psFrame->bcd_MM_1 * 10;
        psUser->ucDD = psFrame->bcd_DD_0 + psFrame->bcd_DD_1 * 10;
        psUser->ucHH = psFrame->bcd_hh_0 + psFrame->bcd_hh_1 * 10;
        psUser->ucmm = psFrame->bcd_mm_0 + psFrame->bcd_mm_1 * 10;
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("ucmt_get_bcd_0() para error!");
        #endif
        return MT_ERR_PARA;
    }
        
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emt_trans_DDHHmmss
 功能描述  : 数据格式转换函数 数据格式16 对于表A.17 单位: 日时分秒
 输入参数  : eMtTrans eTrans              
             sMtDDhhmmss* psDDhhmmss_u    
             sMtDDhhmmss_f* psDDhhmmss_f  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月1日 星期四
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emt_trans_DDHHmmss(eMtTrans eTrans, sMtDDHHmmss* psDDHHmmss_u, sMtDDHHmmss_f* psDDHHmmss_f)
{
    if(!psDDHHmmss_u || !psDDHHmmss_f)
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_DDhhmmss() para pointer is null!");
        #endif
        return MT_ERR_NULL;
    }
    
    if(MT_TRANS_U2F == eTrans)
    {     
        if(psDDHHmmss_u->ucDD > 31 || psDDHHmmss_u->ucDD < 0)
        {
            #ifdef MT_DBG
            DEBUG("emt_trans_DDhhmmss() ucDD MT_ERR_OUTRNG!");
            #endif
            return MT_ERR_OUTRNG;
        }
        else
        {
            psDDHHmmss_f->bcd_DD_0 = ucmt_get_bcd_0(psDDHHmmss_u->ucDD);
            psDDHHmmss_f->bcd_DD_1 = ucmt_get_bcd_1(psDDHHmmss_u->ucDD);
        }

        if(psDDHHmmss_u->ucHH > 24)
        {
            #ifdef MT_DBG
            DEBUG("emt_trans_DDhhmmss() ucHH MT_ERR_OUTRNG!");
            #endif
            return MT_ERR_OUTRNG;
        }
        else
        {
            psDDHHmmss_f->bcd_HH_0 = ucmt_get_bcd_0(psDDHHmmss_u->ucHH);
            psDDHHmmss_f->bcd_HH_1 = ucmt_get_bcd_1(psDDHHmmss_u->ucHH);
        }

        if(psDDHHmmss_u->ucmm > 60)
        {
            #ifdef MT_DBG
            DEBUG("emt_trans_DDhhmmss() ucHH MT_ERR_OUTRNG!");
            #endif
            return MT_ERR_OUTRNG;
        }
        else
        {
            psDDHHmmss_f->bcd_mm_0 = ucmt_get_bcd_0(psDDHHmmss_u->ucmm);
            psDDHHmmss_f->bcd_mm_1 = ucmt_get_bcd_1(psDDHHmmss_u->ucmm);
        }

        if(psDDHHmmss_u->ucss > 60)
        {
            #ifdef MT_DBG
            DEBUG("emt_trans_DDhhmmss() ucHH MT_ERR_OUTRNG!");
            #endif
            return MT_ERR_OUTRNG;
        }
        else
        {
            psDDHHmmss_f->bcd_ss_0 = ucmt_get_bcd_0(psDDHHmmss_u->ucss);
            psDDHHmmss_f->bcd_ss_1 = ucmt_get_bcd_1(psDDHHmmss_u->ucss);
        }         
    }
    else if(MT_TRANS_F2U == eTrans)
    {
        psDDHHmmss_u->ucDD = psDDHHmmss_f->bcd_DD_0 + psDDHHmmss_f->bcd_DD_1 * 10;
        psDDHHmmss_u->ucHH = psDDHHmmss_f->bcd_HH_0 + psDDHHmmss_f->bcd_HH_1 * 10;
        psDDHHmmss_u->ucmm = psDDHHmmss_f->bcd_mm_0 + psDDHHmmss_f->bcd_mm_1 * 10;
        psDDHHmmss_u->ucss = psDDHHmmss_f->bcd_ss_0 + psDDHHmmss_f->bcd_ss_1 * 10;
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_DDhhmmss() para error!");
        #endif
        return MT_ERR_PARA;
    }
        
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emt_trans_MMDDHHmm
 功能描述  : 数据格式转换函数 数据格式17 对于表A.18 单位: 月日时分
 输入参数  : eMtTrans eTrans              
             sMtMMDDHHmm* psMMDDHHmm_u  
             sMtMMDDHHmm_f* psMMDDHHmm_f
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月1日 星期四
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emt_trans_MMDDHHmm(eMtTrans eTrans, sMtMMDDHHmm* psMMDDHHmm_u, sMtMMDDHHmm_f* psMMDDHHmm_f)
{
    if(!psMMDDHHmm_u || !psMMDDHHmm_f)
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_MMDDHHmm() para pointer is null!");
        #endif
        return MT_ERR_NULL;
    }
    
    if(MT_TRANS_U2F == eTrans)
    {     

        if(psMMDDHHmm_u->ucMM > 12 || psMMDDHHmm_u->ucMM < 1 )
        {
            #ifdef MT_DBG
            DEBUG("emt_trans_MMDDHHmm() ucMM MT_ERR_OUTRNG!");
            #endif
            return MT_ERR_OUTRNG;
        }
        else
        {
            psMMDDHHmm_f->bcd_MM_0 = ucmt_get_bcd_0(psMMDDHHmm_u->ucMM);
            psMMDDHHmm_f->bcd_MM_1 = ucmt_get_bcd_1(psMMDDHHmm_u->ucMM);
        }

        if(psMMDDHHmm_u->ucDD > 31 || psMMDDHHmm_u->ucDD < 0)
        {
            #ifdef MT_DBG
            DEBUG("emt_trans_MMDDHHmm() ucDD MT_ERR_OUTRNG!");
            #endif
            return MT_ERR_OUTRNG;
        }
        else
        {
            psMMDDHHmm_f->bcd_DD_0 = ucmt_get_bcd_0(psMMDDHHmm_u->ucDD);
            psMMDDHHmm_f->bcd_DD_1 = ucmt_get_bcd_1(psMMDDHHmm_u->ucDD);
        }

        if(psMMDDHHmm_u->ucHH > 24)
        {
            #ifdef MT_DBG
            DEBUG("emt_trans_MMDDHHmm() ucHH MT_ERR_OUTRNG!");
            #endif
            return MT_ERR_OUTRNG;
        }
        else
        {
            psMMDDHHmm_f->bcd_HH_0 = ucmt_get_bcd_0(psMMDDHHmm_u->ucHH);
            psMMDDHHmm_f->bcd_HH_1 = ucmt_get_bcd_1(psMMDDHHmm_u->ucHH);
        }

        if(psMMDDHHmm_u->ucmm > 60)
        {
            #ifdef MT_DBG
            DEBUG("emt_trans_MMDDHHmm() ucmm MT_ERR_OUTRNG!");
            #endif
            return MT_ERR_OUTRNG;
        }
        else
        {
            psMMDDHHmm_f->bcd_mm_0 = ucmt_get_bcd_0(psMMDDHHmm_u->ucmm);
            psMMDDHHmm_f->bcd_mm_1 = ucmt_get_bcd_1(psMMDDHHmm_u->ucmm);
        }
    }
    else if(MT_TRANS_F2U == eTrans)
    {
        psMMDDHHmm_u->ucDD = psMMDDHHmm_f->bcd_DD_0 + psMMDDHHmm_f->bcd_DD_1 * 10;
        psMMDDHHmm_u->ucHH = psMMDDHHmm_f->bcd_HH_0 + psMMDDHHmm_f->bcd_HH_1 * 10;
        psMMDDHHmm_u->ucmm = psMMDDHHmm_f->bcd_mm_0 + psMMDDHHmm_f->bcd_mm_1 * 10;
        psMMDDHHmm_u->ucMM = psMMDDHHmm_f->bcd_MM_0 + psMMDDHHmm_f->bcd_MM_1 * 10;
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_MMDDhhmm() para error!");
        #endif
        return MT_ERR_PARA;
    }
        
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emt_trans_MMDDHHmm
 功能描述  : 数据格式转换函数 数据格式18 对于表A.19 单位: 日时分
 输入参数  : eMtTrans eTrans              
             sMtDDhhmm* psDDhhmm_u    
             sMtDDhhmm_f* psDDhhmm_f  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月1日 星期四
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emt_trans_DDHHmm(eMtTrans eTrans, sMtDDHHmm* psDDHHmm_u, sMtDDHHmm_f* psDDHHmm_f)
{
    if(!psDDHHmm_u || !psDDHHmm_f)
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_DDHHmm() para pointer is null!");
        #endif
        return MT_ERR_NULL;
    }
    
    if(MT_TRANS_U2F == eTrans)
    {     
        if(psDDHHmm_u->ucDD > 31 || psDDHHmm_u->ucDD < 0)
        {
            #ifdef MT_DBG
            DEBUG("emt_trans_DDHHmm() ucDD MT_ERR_OUTRNG!");
            #endif
            return MT_ERR_OUTRNG;
        }
        else
        {
            psDDHHmm_f->bcd_DD_0 = ucmt_get_bcd_0(psDDHHmm_u->ucDD);
            psDDHHmm_f->bcd_DD_1 = ucmt_get_bcd_1(psDDHHmm_u->ucDD);
        }

        if(psDDHHmm_u->ucHH > 24)
        {
            #ifdef MT_DBG
            DEBUG("emt_trans_DDHHmm() ucHH MT_ERR_OUTRNG!");
            #endif
            return MT_ERR_OUTRNG;
        }
        else
        {
            psDDHHmm_f->bcd_HH_0 = ucmt_get_bcd_0(psDDHHmm_u->ucHH);
            psDDHHmm_f->bcd_HH_1 = ucmt_get_bcd_1(psDDHHmm_u->ucHH);
        }

        if(psDDHHmm_u->ucmm > 60)
        {
            #ifdef MT_DBG
            DEBUG("emt_trans_DDHHmm() ucmm MT_ERR_OUTRNG!");
            #endif
            return MT_ERR_OUTRNG;
        }
        else
        {
            psDDHHmm_f->bcd_mm_0 = ucmt_get_bcd_0(psDDHHmm_u->ucmm);
            psDDHHmm_f->bcd_mm_1 = ucmt_get_bcd_1(psDDHHmm_u->ucmm);
        }
    }
    else if(MT_TRANS_F2U == eTrans)
    {
        psDDHHmm_u->ucDD = psDDHHmm_f->bcd_DD_0 + psDDHHmm_f->bcd_DD_1 * 10;
        psDDHHmm_u->ucHH = psDDHHmm_f->bcd_HH_0 + psDDHHmm_f->bcd_HH_1 * 10;
        psDDHHmm_u->ucmm = psDDHHmm_f->bcd_mm_0 + psDDHHmm_f->bcd_mm_1 * 10;
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_DDhhmm() para error!");
        #endif
        return MT_ERR_PARA;
    }
        
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emt_trans_MMHHmm
 功能描述  : 数据格式转换函数 数据格式19 对于表A.20 单位: 时分
 输入参数  : eMtTrans eTrans              
             sMthhmm* pshhmm_u    
             sMthhmm_f* pshhmm_f  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月1日 星期四
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emt_trans_HHmm(eMtTrans eTrans, sMtHHmm* psHHmm_u, sMtHHmm_f* psHHmm_f)
{
    if(!psHHmm_u || !psHHmm_f)
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_HHmm() para pointer is null!");
        #endif
        return MT_ERR_NULL;
    }
    
    if(MT_TRANS_U2F == eTrans)
    {     
        if(psHHmm_u->ucHH > 24)
        {
            #ifdef MT_DBG
            DEBUG("emt_trans_HHmm() ucHH MT_ERR_OUTRNG!");
            #endif
            return MT_ERR_OUTRNG;
        }
        else
        {
            psHHmm_f->bcd_HH_0 = ucmt_get_bcd_0(psHHmm_u->ucHH);
            psHHmm_f->bcd_HH_1 = ucmt_get_bcd_1(psHHmm_u->ucHH);
        }

        if(psHHmm_u->ucmm > 60)
        {
            #ifdef MT_DBG
            DEBUG("emt_trans_HHmm() ucmm MT_ERR_OUTRNG!");
            #endif
            return MT_ERR_OUTRNG;
        }
        else
        {
            psHHmm_f->bcd_mm_0 = ucmt_get_bcd_0(psHHmm_u->ucmm);
            psHHmm_f->bcd_mm_1 = ucmt_get_bcd_1(psHHmm_u->ucmm);
        }
    }
    else if(MT_TRANS_F2U == eTrans)
    {
        psHHmm_u->ucHH = psHHmm_f->bcd_HH_0 + psHHmm_f->bcd_HH_1 * 10;
        psHHmm_u->ucmm = psHHmm_f->bcd_mm_0 + psHHmm_f->bcd_mm_1 * 10;
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_hhmm() para error!");
        #endif
        return MT_ERR_PARA;
    }
        
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emt_trans_X_X
 功能描述  : 数据格式转换函数 数据格式22 对于表A.23 
 输入参数  : eMtTrans eTrans              
             float* psUser 
             sMtFmt22_f* psFrame  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月1日 星期四
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emt_trans_X_X(eMtTrans eTrans, float* psUser, sMtFmt22_f* psFrame)
{
    if(!psUser || !psFrame)
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_X_X() para pointer is null!");
        #endif
        return MT_ERR_NULL;
    }

    float fData = *psUser;
    char buf[4] = {0};
    
    if(MT_TRANS_U2F == eTrans)
    {
        if(fData > 9.9f || fData < 0.0f)
        {
            #ifdef MT_DBG
            DEBUG("emt_trans_X_X() fData para error! %f", fData);
            #endif
            return MT_ERR_PARA;
        }

        sprintf(buf, "%02.1f", fData);
        psFrame->BCD_0   = buf[0] - '0';
        psFrame->BCD_0_1 = buf[2] - '0';
    }
    else if(MT_TRANS_F2U == eTrans)
    {
        fData = (psFrame->BCD_0_1 + psFrame->BCD_0 * 10.0f) / 10.0f;
        if(fData > 9.9f || fData < 0.0f)
        {
            #ifdef MT_DBG
            DEBUG("emt_trans_X_X() fData para error! %f", fData);
            #endif
            return MT_ERR_PARA;
        }
        
        *psUser = fData;
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_X_X() para error!");
        #endif
        return MT_ERR_PARA;
    }
        
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emt_trans_XX_XXXX
 功能描述  : 数据格式转换函数 数据格式23 对于表A.24 
 输入参数  : eMtTrans eTrans      
             float* psUser        
             sMtFmt23_f* psFrame  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月19日 星期一
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emt_trans_XX_XXXX(eMtTrans eTrans, float* psUser, sMtFmt23_f* psFrame)
{
    if(!psFrame || !psUser)
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_XX_XXXX() para pointer is null!");
        #endif
        return MT_ERR_NULL;
    }

    sMtFmt_XX_XXXX   *psFmtXX_XXXX;
    float             *pfXX_XXXX;
    float             fZhengshu;  // 整数部分

    psFmtXX_XXXX  = (sMtFmt_XX_XXXX *)psFrame;
    pfXX_XXXX      = (float *)psUser;
    float fXX_XXXX = *pfXX_XXXX;
    int sign       = 0; 
 
    // 用户侧到帧侧
    if(MT_TRANS_U2F == eTrans)
    {
        if(fXX_XXXX > 99.9999f || fXX_XXXX < 0.0f)
        {
            return MT_ERR_OUTRNG;
        }

        modff(fXX_XXXX*10000.0f, &fZhengshu);
        sign = bMtGetFloatSign(fZhengshu);
        vMtSetFloatSign(&fZhengshu, 0);

        char buf[8] = {0};
        sprintf(buf, "%06.0lf\n", fZhengshu);

        //输出结构
        psFmtXX_XXXX->BCD_1   = buf[0] - '0';
        psFmtXX_XXXX->BCD_0   = buf[1] - '0';
        psFmtXX_XXXX->BCD_0_1 = buf[2] - '0';
        psFmtXX_XXXX->BCD_0_2 = buf[3] - '0';  
        psFmtXX_XXXX->BCD_0_3 = buf[4] - '0';
        psFmtXX_XXXX->BCD_0_4 = buf[5] - '0';
    }
    // 帧侧到用户侧
    else if(MT_TRANS_F2U == eTrans)
    {
        UINT8    bcd[6];
        int      i;
        float    valid = 0.0f;
        bcd[0] = (UINT8)psFmtXX_XXXX->BCD_0_4;
        bcd[1] = (UINT8)psFmtXX_XXXX->BCD_0_3;
        bcd[2] = (UINT8)psFmtXX_XXXX->BCD_0_2;
        bcd[3] = (UINT8)psFmtXX_XXXX->BCD_0_1;
        bcd[4] = (UINT8)psFmtXX_XXXX->BCD_0;
        bcd[5] = (UINT8)psFmtXX_XXXX->BCD_1;

        for(i = 0; i < 6; i++)
        {
        valid+= bcd[i]*nMtPow(i);
        }

        fXX_XXXX = valid/10000.0f;
        *pfXX_XXXX = fXX_XXXX;
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_XX_XXXX() para error!");
        #endif
        return MT_ERR_PARA;
    }

     return MT_OK;
}

/*****************************************************************************
 函 数 名  : emt_trans_XX_XXXX
 功能描述  : 数据格式26 对于表A.27
 输入参数  : eMtTrans eTrans      
             float* psUser        
             sMtFmt23_f* psFrame  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月19日 星期一
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emt_trans_X_XXX(eMtTrans eTrans, float* psUser, sMtFmt26* psFrame)
{
    if(!psFrame || !psUser)
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_X_XXX() para pointer is null!");
        #endif
        return MT_ERR_NULL;
    }

    sMtFmt26          *psFmtX_XXX;
    float             *pfX_XXX;
    float             fZhengshu;  // 整数部分

    psFmtX_XXX     = (sMtFmt26 *)psFrame;
    pfX_XXX        = (float *)psUser;
    float fX_XXX   = *pfX_XXX;
    int sign       = 0; 

    // 用户侧到帧侧
    if(MT_TRANS_U2F == eTrans)
    {
        if(fX_XXX > 9.999f || fX_XXX < 0.0f)
        {
            return MT_ERR_OUTRNG;
        }

        modff(fX_XXX*1000, &fZhengshu);
        sign = bMtGetFloatSign(fZhengshu);
        vMtSetFloatSign(&fZhengshu, 0);

        char buf[8] = {0};
        sprintf(buf, "%04.0lf\n", fZhengshu);

        //输出结构
        psFmtX_XXX->BCD_0   = buf[0] - '0';
        psFmtX_XXX->BCD_0_1 = buf[1] - '0';
        psFmtX_XXX->BCD_0_2 = buf[2] - '0';
        psFmtX_XXX->BCD_0_3 = buf[3] - '0';  

    }
    // 帧侧到用户侧
    else if(MT_TRANS_F2U == eTrans)
    {
        UINT8    bcd[4];
        int      i;
        float    valid = 0.0f;
        bcd[0] = (UINT8)psFmtX_XXX->BCD_0_3;
        bcd[1] = (UINT8)psFmtX_XXX->BCD_0_2;
        bcd[2] = (UINT8)psFmtX_XXX->BCD_0_1;
        bcd[3] = (UINT8)psFmtX_XXX->BCD_0;

        for(i = 0; i < 4; i++)
        {
            valid+= bcd[i]*nMtPow(i);
        }

        fX_XXX = valid/1000.0f;
        *pfX_XXX = fX_XXX;

    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_X_XXX() para error!");
        #endif
        return MT_ERR_PARA;
    }

 return MT_OK;
}

/*****************************************************************************
 函 数 名  : emt_trans_XXXXXXXX
 功能描述  : 数据格式27 对于表A.28 格式:  XXXXXXXX
 输入参数  : eMtTrans eTrans    
             float* psUser      
             sMtFmt07* psFrame  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月22日 星期四
    作    者   : liujinlong
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emt_trans_XXXXXXXX(eMtTrans eTrans, UINT32* psUser, sMtFmt_XXXXXXXX* psFrame)
{
    if(!psFrame || !psUser)
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_XXXXXXXX() para pointer is null!");
        #endif
        return MT_ERR_NULL;
    }

    sMtFmt_XXXXXXXX   *psFmtXXxxXXxx;
    UINT32            *puiXXxxXXxx;

    psFmtXXxxXXxx        = (sMtFmt_XXXXXXXX *)psFrame;
    puiXXxxXXxx          = (UINT32 *)psUser;
    UINT32 uiXXxxXXxx    = *puiXXxxXXxx;
    
    // 用户侧到帧侧
    if(MT_TRANS_U2F == eTrans)
    {
        if(uiXXxxXXxx > (UINT32)(nMtPow(8) - 1)|| uiXXxxXXxx < 0)
        {
            return MT_ERR_OUTRNG;
        }
        
        //输出结构
        psFmtXXxxXXxx->BCD_1 = uiXXxxXXxx / nMtPow(7);
        uiXXxxXXxx = uiXXxxXXxx % nMtPow(7);
        psFmtXXxxXXxx->BCD_2 = uiXXxxXXxx / nMtPow(6);
        uiXXxxXXxx = uiXXxxXXxx % nMtPow(6);
        psFmtXXxxXXxx->BCD_3 = uiXXxxXXxx / nMtPow(5);
        uiXXxxXXxx = uiXXxxXXxx % nMtPow(5);
        psFmtXXxxXXxx->BCD_4 = uiXXxxXXxx / nMtPow(4);
        uiXXxxXXxx = uiXXxxXXxx % nMtPow(4);
        psFmtXXxxXXxx->BCD_5 = uiXXxxXXxx / nMtPow(3);
        uiXXxxXXxx = uiXXxxXXxx % nMtPow(3);
        psFmtXXxxXXxx->BCD_6 = uiXXxxXXxx / nMtPow(2);
        uiXXxxXXxx = uiXXxxXXxx % nMtPow(2);
        psFmtXXxxXXxx->BCD_7 = uiXXxxXXxx / nMtPow(1);
        uiXXxxXXxx = uiXXxxXXxx % nMtPow(1);
        psFmtXXxxXXxx->BCD_8 = uiXXxxXXxx;
    }
    // 帧侧到用户侧
    else if(MT_TRANS_F2U == eTrans)
    {
        UINT8    bcd[8];
        UINT32   valid = 0;
        UINT8    i;
        bcd[0] = (UINT8)psFmtXXxxXXxx->BCD_8;
        bcd[1] = (UINT8)psFmtXXxxXXxx->BCD_7;
        bcd[2] = (UINT8)psFmtXXxxXXxx->BCD_6;
        bcd[3] = (UINT8)psFmtXXxxXXxx->BCD_5;
        bcd[4] = (UINT8)psFmtXXxxXXxx->BCD_4;
        bcd[5] = (UINT8)psFmtXXxxXXxx->BCD_3;
        bcd[6] = (UINT8)psFmtXXxxXXxx->BCD_2;
        bcd[7] = (UINT8)psFmtXXxxXXxx->BCD_1;    

        for(i = 0; i < sizeof(bcd); i++)
        {
            valid += bcd[i] * nMtPow(i);
        }

        *puiXXxxXXxx = valid;
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_XXXXXXXX() para error!");
        #endif
        return MT_ERR_PARA;
    }

    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emt_trans_fmt_freeze
 功能描述  : 附录c 冻结间隔
 输入参数  : eMtTrans eTrans     
             eMtFmtFrez* psUser  
             UINT8* psFrame      
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月26日 星期一
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emt_trans_fmt_freeze(eMtTrans eTrans, eMtFmtFrez* psUser, UINT8* psFrame)
{
    if(!psFrame || !psUser)
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_fmt_freeze() para pointer is null!");
        #endif
        return MT_ERR_NULL;
    }

    eMtFmtFrez        *psU;
    UINT8             *psF;
    eMtFmtFrez        eFrez;
    UINT8             ucFrez;

    psU               = (eMtFmtFrez *)psUser;
    eFrez             = *(psU); 
    psF               = (UINT8 *)psFrame;
    
    // 用户侧到帧侧
    if(MT_TRANS_U2F == eTrans)
    {
        switch(eFrez)
        {
            case MT_FREZ_NULL:
                ucFrez = 0;
                break;
                
            case MT_FREZ_15:
                ucFrez = 1;
                break;   
                
            case MT_FREZ_30:
                ucFrez = 2;
                break;   
                
            case MT_FREZ_60:
                ucFrez = 3;
                break;   
                
            case MT_FREZ_5:
                ucFrez = 254;
                break;            

            case MT_FREZ_1:
                ucFrez = 255;
                break;             

                default:
                #ifdef MT_DBG
                DEBUG("emt_trans_fmt_freeze() eFrez para out range %d", eFrez);
                #endif
                return MT_ERR_OUTRNG;
               // break;
        }

        *psF = ucFrez;
    }
    // 帧侧到用户侧
    else if(MT_TRANS_F2U == eTrans)
    {
        ucFrez = *psF;
        switch(ucFrez)
        {
            case 0:
                eFrez = MT_FREZ_NULL;
                break;
                
            case 1:
                eFrez = MT_FREZ_15;
                break;   
                
            case 2:
                eFrez = MT_FREZ_30;
                break;   
                
            case 3:
                eFrez = MT_FREZ_60;
                break;   
                
            case 254:
                eFrez = MT_FREZ_5;
                break;            

            case 255:
                eFrez = MT_FREZ_1;
                break;             

                default:
                #ifdef MT_DBG
                DEBUG("emt_trans_fmt_freeze() ucFrez para out range %d", ucFrez);
                #endif
                return MT_ERR_OUTRNG;
               // break;
        }

        *(psU) = eFrez;
    }
    else
    {
        #ifdef MT_DBG
        DEBUG("emt_trans_fmt_freeze() para error!");
        #endif
        return MT_ERR_PARA;
    }

    return MT_OK;
}
    
/*****************************************************************************
 函 数 名  : ucmt_get_check_sum
 功能描述  : 计算校验和函数
 输入参数  : UINT8 *pStartPos  
             UINT16 usLen      
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年1月16日
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
UINT8 ucmt_get_check_sum(UINT8 *pStartPos, UINT16 usLen)
{
    UINT8 ucCheckSum = 0;
    int i = 0;
   
    for(i = 0; i < usLen; i++)
    {
        ucCheckSum += pStartPos[i];
    }

    return ucCheckSum;
}

/*****************************************************************************
 函 数 名  : emt_pack_common
 功能描述  : 公共部分 报文封装函数   
 输入参数  : eMtAFN eAFN           
             sMtComPack *psCommon  
 输出参数  : UINT16 *pusLen     封装后的帧长    
             UINT8  *pOutBuf    封装后的帧内容
 返 回 值  : 
 调用函数  : 
 被调函数  : emtBasePack
 
 修改历史      :
  1.日    期   : 2013年8月2日 星期五
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emt_pack_common(eMtAFN eAFN, sMtComPack *psCommon,  UINT16 *pusLen, UINT8  *pOutBuf)
{    
    eMtErr eErr     = MT_OK;
    UINT16 usBufLen = 0;
    UINT8  ucCtrl   = 0;
    UINT16 usUserDataLen = 0; // 用户数据区字长 用于CS 和 长度域 
    
    UINT8 *pucSeq  = NULL;
    UINT8 *pucCtrl = NULL;   // 用于计算CS
    UINT8 *pucCS   = NULL;
    sMtfComHead *psHead = NULL;
    
    if(!psCommon || !pusLen || !pOutBuf)
    {
        #ifdef MT_DBG
        DEBUG("emt_pack_common() pointer is null!");
        #endif
        return MT_ERR_NULL;
    }

    if(g_bMtInit != TRUE)
    {
        #ifdef MT_DBG
        DEBUG("emt_pack_common() protocol is not init!");
        #endif
        return MT_ERR_INIT;
    }

    // 帧长是否超限
    if(psCommon->usSeq2CsLen > MT_SEQ2CS_MAX)
    {
        #ifdef MT_DBG
        DEBUG("emt_pack_common() para out of range!");
        #endif
        return MT_ERR_OUTRNG;
    }

    if(!(psCommon->pSeq2Cs))
    {
        #ifdef MT_DBG
        DEBUG("emt_pack_common() MT_ERR_SEQ2CS pointer is null!");
        #endif
        return MT_ERR_SEQ2CS;
    }
       
    psHead = (sMtfComHead *)pOutBuf;

    // 0x68
    psHead->f68 = 0x68;
    psHead->s68 = 0x68;

    // AFN 
    psHead->AFN  = eAFN;
    
    // 计算应用层数据的长度
    usUserDataLen  = psCommon->usSeq2CsLen + MT_CANS_LEN;  
    psHead->L1 = (usUserDataLen & 0x003F);
    psHead->L2 = (usUserDataLen & 0x3FC0) >> 6;
    
    psHead->L1_2 = (usUserDataLen & 0x003F);
    psHead->L2_2 = (usUserDataLen & 0x3FC0) >> 6;

    psHead->p10_2 = 2; 
    psHead->p10   = 2; 
    
    // 计算控制域
    eErr = emt_trans_ctrl(MT_TRANS_U2F, &(psCommon->sCtrl), &ucCtrl);
    if(eErr != MT_OK)
    {
        #ifdef MT_DBG
        DEBUG("emt_pack_common() emt_trans_ctrl failed: %s\n", smtGetErr(eErr));
        #endif 
        return MT_ERR_CTRL;
    }
  
    psHead->C = ucCtrl;

    // 地址域
    eErr = emt_trans_address(MT_TRANS_U2F, &(psCommon->sAddr), &(psHead->A));
    if(eErr != MT_OK)
    {
        #ifdef MT_DBG
        DEBUG("emt_pack_common() emt_trans_address failed: %s\n", smtGetErr(eErr));
        #endif 
        return MT_ERR_ADDR;
    }
        
    // 构造SEQ
    eErr = emt_trans_seq(MT_TRANS_U2F, &(psCommon->sSEQ), (sMtSEQ_f*)&(psHead->SEQ));
    if(eErr != MT_OK)
    {
        #ifdef MT_DBG
        DEBUG("emt_pack_common() emt_trans_seq failed: %s\n", smtGetErr(eErr));
        #endif 
        return MT_ERR_SEQ;
    }

    // 复制SEQ 到 CS 之间的数据
    pucSeq = (UINT8*)&(psHead->SEQ);
    pucSeq++;  // 取得应用层数据首地址
      
    memcpy((void*)pucSeq, (void*)(psCommon->pSeq2Cs), psCommon->usSeq2CsLen);

    // 计算CS
    pucCtrl = (UINT8*)&(psHead->C);   // 计算起始位置
    pucCS   = (UINT8*)(pucCtrl + usUserDataLen);
    
    *pucCS = ucmt_get_check_sum(pucCtrl, usUserDataLen);

    // 0x16H
    pucCS++;
    *pucCS = 0x16;    

    // 计算总长
    usBufLen = usUserDataLen + MT_UN_USER_LEN;

    // 将封装后的报文 传入回调

    // 根据返回值不同的处理
 
    // 输出长度
    *pusLen = usBufLen;

    // 输出报文数据
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtLitePack
 功能描述  : 实现基本的报文封装, 加入加密算法接口
             该处对应用层数据加密 加密后可能可能会改
             变长度,加密的数据区包括：应用层功能码、数据单元标识及数据单元部分
             应用层的数据,用上层传入的空间,这样比emtBasePack节省内存
             
 输入参数  : smtLitePack* psPack  
             UINT16* pusLen       
             UINT8* pOutBuf       
 输出参数  : 无
 返 回 值  : 
 调用函数  : emt_pack_common()
 被调函数  : emtPack()
 
 修改历史      :
  1.日    期   : 2013年8月6日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtLitePack(smtLitePack* psPack, UINT16* pusLen, UINT8* pOutBuf)
{
    if(!psPack || !pusLen || !pOutBuf)
    {
        #ifdef MT_DBG
        DEBUG("emtLitePack() pointer is null!");
        #endif
        return MT_ERR_NULL;
    }

    if(TRUE != g_bMtInit)
    {
        #ifdef MT_DBG
        DEBUG("emtLitePack() protocol is not init!");
        #endif
        return MT_ERR_INIT;
    }

    eMtErr         eRet        = MT_OK;
    eMtCmd         eCmd        = CMD_AFN_F_UNKOWN;
    eMtDir         eDir        = MT_DIR_UNKOWN;
    eMtAFN         eAFN        = AFN_NULL;
    BOOL           bSameTeam   = FALSE;
    BOOL           bP0         = FALSE;
    UINT8          ucPnNum     = 0;
    UINT16         usSeq2CsLen = 0;
    UINT16         usSeq2CsPos = 0; 
    UINT16         usfDataLen  = 0;      // 帧侧数据单元字长
    UINT16         usCMD       = 0;
    INT32          i           = 0;
    INT32          j           = 0;
    INT32          fi          = 0;
    INT32          pi          = 0;
    UINT8*         pSeq2Cs     = NULL;
    UINT8*         puData      = NULL;   // 数据单元用户侧数据
    UINT8*         pData       = NULL;   // 数据单元
    sMtDaDt*       pDaDt       = NULL;   // 数据单元标识
    sMtEC*         psEC        = NULL;
    sMtTP_f*       psfTp       = NULL;   // 帧侧Tp字段
    pMtFunc        pFunc       = NULL;
    sMtCmdInfor    sCmdInfor;   
    sMtTP          suTp;                 // 用户侧Tp字段信息
    sMtPnFn        sPnFn;
    sMtComPack     sPackCommon;

    // 加密
    #if MT_CFG_ENCRYPT
    UINT8*         pEncry_in   = NULL;  // 需要加密部分的首地址 输入 
    INT32          nLen_in     = 0;     // 明文的总字长
    UINT8*         pEncry_out  = NULL;  // 需要加密部分的首地址 输出
    INT32          nLen_out    = 0;     // 密文的总字长
    #endif
    
    memset(&(sPnFn), 0x00, sizeof(sMtPnFn));
    memset(&sPackCommon, 0x00, sizeof(sMtComPack));

    if(MT_ROLE_MASTER == g_eMtRole)
    {
        eDir = MT_DIR_M2S;
    }
    else
    {
        eDir = MT_DIR_S2M;
    }

    /* 封装sMtComPack 入参 */
    // 构建通用封包参数
    pSeq2Cs = (UINT8*)malloc(MT_SEQ2CS_BYTES_MAX);
    if(!pSeq2Cs)
    {
        #ifdef MT_DBG
        DEBUG("emtLitePack() malloc failed!");
        #endif
        return MT_ERR_IO;
    }

    // 封装变长的数据单元标识与数据单元组
    if(0 == psPack->usDataNum)
    {
        #ifdef MT_DBG
        DEBUG("emtLitePack() ucSubNum is 0!");
        #endif
        MT_FREE(pSeq2Cs);
        return MT_ERR_PROTO;
    }

    // 封装至少一个数据标识与数据单元组合
    for(i = 0; i < psPack->usDataNum; i++)
    {
        for(j = 0; j < PN_INDEX_MAX; j++)
        {
            sPnFn.usPn[j] =  psPack->sData[i].sPnFn.usPn[j];
            sPnFn.ucFn[j] =  psPack->sData[i].sPnFn.ucFn[j];
        }

        // 确保每个Pn 都属于同一个信息点组
        bSameTeam = bmt_same_team_pn(sPnFn.usPn, NULL);
        if(FALSE == bSameTeam)
        {
            #ifdef MT_DBG
            DEBUG("emtLitePack() pn is not is same team!");
            #endif
            MT_FREE(pSeq2Cs);
            return MT_ERR_TEAM;
        }
        
        // 确保每个Fn 都属于同一个信息类组
        bSameTeam = bmt_same_team_fn(psPack->sData[i].sPnFn.ucFn, NULL);
        if(FALSE == bSameTeam)
        {
            #ifdef MT_DBG
            DEBUG("emtLitePack() Fn is not is same team!");
            #endif
            MT_FREE(pSeq2Cs);
            return MT_ERR_TEAM;
        }

        // 主功能码
        eAFN = psPack->eAFN;

        // 封装数据单元标识
        pDaDt = (sMtDaDt*)(pSeq2Cs + usSeq2CsPos);
        eRet = emt_pnfn_to_dadt(&(sPnFn), pDaDt);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtLitePack() emt_pnfn_to_dadt() failed %d %s!", eRet, smtGetErr(eRet));
            #endif
            return eRet;
        }

        usSeq2CsPos +=  sizeof(sMtDaDt);
        /*
            逐个封装最多64个与Fn Pn组对应的数据单元
            数据单元为按数据单元标识所组织的数据，包括参数、命令等。
            数据组织的顺序规则：先按pn从小到大、再按Fn从小到大的次序，即：完成一个信息点pi
            的所有信息类Fn的处理后，再进行下一个pi+1的处理。
        */

        // 分两种情况
        bP0 = bmt_is_p0(psPack->sData[i].sPnFn.usPn);
        if(TRUE == bP0)
        {
             ucPnNum = 1; // 1 - p0
        }
        else
        {
             ucPnNum = PN_INDEX_MAX; // 2 - 含有最多8个Pn 
        }
        
        // 应用层数据域
        for(pi = 0; pi < ucPnNum; pi++)
        {
            if(MT_PN_NONE != psPack->sData[i].sPnFn.usPn[pi])
            {
                for(fi = 0; fi < FN_INDEX_MAX; fi++)
                {
                    if(MT_FN_NONE != psPack->sData[i].sPnFn.ucFn[fi])
                    {
                         // 合成命令码
                        usCMD = (UINT16)((eAFN << 8) | (psPack->sData[i].sPnFn.ucFn[fi]));
                        eCmd  = (eMtCmd)usCMD; 

                        // 获得命令信息
                        eRet = eMtGetCmdInfor(eCmd, eDir, &sCmdInfor);
                        if(MT_OK != eRet)
                        {
                            #ifdef MT_DBG
                            DEBUG("emtLitePack() eMtGetCmdInfor() failed %d %s!", eRet, smtGetErr(eRet));
                            #endif
                            MT_FREE(pSeq2Cs);
                            return eRet;
                        }

                        // 数据单元
                        pData = (UINT8*)(pSeq2Cs + usSeq2CsPos);
                        pFunc = sCmdInfor.pFunc;
                        if(NULL != pFunc)
                        {
                            puData = (UINT8*)(psPack->sData[i].puApp[pi][fi]);
                            eRet = pFunc(MT_TRANS_U2F, puData, pData, &usfDataLen);
                            if(MT_OK != eRet)
                            {
                                #ifdef MT_DBG
                                DEBUG("emtLitePack() transU2FpFunc() failed %d %s!", eRet, smtGetErr(eRet));
                                #endif
                                MT_FREE(pSeq2Cs);
                                return eRet;
                            }

                            usSeq2CsPos += usfDataLen;
                        }     
                    }
                }
            }
        }
    }

    // 加密
    #if MT_CFG_ENCRYPT
    if(NULL != g_peMtEncryptFunc)
    {
        pEncry_in  = pSeq2Cs;  // 这里需要再调整,请确认需要加密的部分
        nLen_in    = usSeq2CsPos;
        pEncry_out = (UINT8*)malloc(MT_SEQ2CS_BYTES_MAX);
        if(!pEncry_out)
        {
            #ifdef MT_DBG
            DEBUG("emtLitePack() malloc failed!");
            #endif
            return MT_ERR_IO; 
        }
        
        eRet = g_peMtEncryptFunc(pEncry_in, nLen_in, pEncry_out, &nLen_out);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtLitePack() encrypt failed! %d", eRet);
            #endif
            MT_FREE(pEncry_out);
            return MT_ERR_ENCRYPT;
        }

        // 替换成密文
        memcpy((void*)pSeq2Cs, (void*)pEncry_out, nLen_out); 

        // 更新长度
        usSeq2CsPos = nLen_out;

    }
    #endif

    /* 附加信息域AUX */
    // 如果有 EC
    if(TRUE == bmt_have_ec(eAFN, eDir))
    {       
        pData = (UINT8*)(pSeq2Cs + usSeq2CsPos);
        psEC = (sMtEC*)pData;
        if(MT_DIR_S2M == eDir)
        {
            psEC->ucEC1 = g_tEC.ucEC1;
            psEC->ucEC2 = g_tEC.ucEC2;
        }
        else
        {
            // 疑问: 主站的该字段如何封装 全填0 还是 0xEE
            psEC->ucEC1 = 0x0;
            psEC->ucEC2 = 0x0;
        }
       
        usSeq2CsPos += sizeof(sMtEC);
     }
    
    // 如果有 pw  
    // pw用于重要的下行报文中,由主站产生,收端验证，通过后执行(16字节)
    if(TRUE == bmt_have_pw(eAFN, eDir))    
    {
        pData = (UINT8*)(pSeq2Cs + usSeq2CsPos);
        memcpy((void*)pData, (void*)g_aucPw, MT_PW_LEN); 
        usSeq2CsPos += MT_PW_LEN;
    }

    // 如果有 TP
    if(TRUE == bmt_have_tp(eAFN, eDir))  // 除了这种情况的所有报文中
    {
        pData = (UINT8*)(pSeq2Cs + usSeq2CsPos);
        psfTp = (sMtTP_f *)pData;
        suTp  = psPack->sTP;
  
        //(void)emt_get_mtutp(psPack->ucPFC, &suTp); 由上层封装时间戳
        (void)emt_trans_tp(MT_TRANS_U2F, &suTp, psfTp);
        usSeq2CsPos += sizeof(sMtTP_f);
    }

    // 总结数据
    usSeq2CsLen             = usSeq2CsPos;
    sPackCommon.sAddr       = psPack->sAddress;
    sPackCommon.sCtrl       = psPack->sCtrl;;
    sPackCommon.sSEQ        = psPack->sSEQ;   
    sPackCommon.usSeq2CsLen = usSeq2CsLen;
    sPackCommon.pSeq2Cs     = pSeq2Cs;

    // 通过通用封包函数pack
    eRet = emt_pack_common(eAFN,  &sPackCommon, pusLen, pOutBuf);   
    if(MT_OK != eRet)
    {
        #ifdef MT_DBG
        DEBUG("emtLitePack() emt_pack_common failed:%s!", smtGetErr(eRet));
        #endif

        MT_FREE(pSeq2Cs);
        // 释放加密部分申请的内存
        #if MT_CFG_ENCRYPT
        MT_FREE(pEncry_out);
        #endif
        return eRet;
    }

    MT_FREE(pSeq2Cs);
    
    // 释放加密部分申请的内存
    #if MT_CFG_ENCRYPT
    MT_FREE(pEncry_out);
    #endif
    
    return MT_OK;
}
   
/*****************************************************************************
 函 数 名  : emtLiteUnPack
 功能描述  : 实现基本的报文解封装, 加入加密算法接口
             该处对应用层数据解密 解密后可能可能会改变长度,
             解密的数据区包括：应用层功能码、数据单元标识及数据单元部分
 输入参数  : smtLitePack *psUnpack  
             UINT8* pInBuf          
             UINT16 usLen           
 输出参数  : 无
 返 回 值  : 
 调用函数  : emtUnPack
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月6日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtLiteUnPack(smtLitePack *psUnpack, UINT8* pInBuf, UINT16 usLen)
{
    if(!psUnpack || !pInBuf)
    {
        #ifdef MT_DBG
        DEBUG("emtLiteUnPack() pointer is null!");
        #endif
        return MT_ERR_NULL;
    }

    if(TRUE != g_bMtInit)
    {
        #ifdef MT_DBG
        DEBUG("emtLiteUnPack() protocol is not init!");
        #endif
        return MT_ERR_INIT;
    }

    if(usLen < MT_FRM_LEN_MIN)
    {
        #ifdef MT_DBG
        DEBUG("emtLiteUnPack() input frame length is too short usLen = %d", usLen);
        #endif 
        return MT_ERR_UNCOMP;
    }
    
    eMtErr      eRet            = MT_OK;
    eMtDir      eDir            = MT_DIR_UNKOWN;
    eMtAFN      eAFN            = AFN_NULL;
    eMtCmd      eCmd            = CMD_AFN_F_UNKOWN;
    BOOL        bP0             = FALSE;
    BOOL        bTp             = FALSE;   
    BOOL        bEc             = FALSE;
    sMtfComHead *psHead         = NULL;
    sMtDaDt     *pDaDt          = NULL;
    UINT8       *pucTemp        = NULL;
    sMtEC       *psEC           = NULL;
    uMtApp      *puApp          = NULL;
    pMtFunc     pFunc           = NULL;
    UINT8       ucFn            = 0;
    UINT8       ucPnCycMax      = 0;        // 按Pn循环的最大值
    UINT16      usPn            = 0;
    UINT16      usUsrdLen       = 0;        //用户应用数据在帧侧的字长
    UINT16      usUsrdLenTotal  = 0;        //用户应用数据在帧侧的字长
    UINT16      usAuxLen        = 0;        // 该类型帧的EcPwTp帧侧的几个域的部字长
    UINT16      usLenUserField  = 0;        // 用户数据区长度       
    UINT16      usDataNum       = 0;     // 数据单元组的个数
    INT32       nLenUserField   = 0;        // 每次解析一个子命令数据单元后的用户数据域字长 
    INT32       i               = 0;
    INT32       fi              = 0;        // Fn的索引
    INT32       pi              = 0;        // Pn的索引

    sMtPnFn     sPnFn;
    sMtCmdInfor sCmdInfor;

    // 判断该帧是否是一个有效的帧
    eRet = emtIsValidPack(pInBuf, usLen);
    if(MT_OK != eRet)
    {
        #ifdef MT_DBG
        DEBUG("emtLiteUnPack() input is not a valid pack eRet = %d", eRet);
        #endif 
        return MT_ERR_PACK;
    }

    // 报文头
    psHead = (sMtfComHead *)pInBuf;
    usLenUserField =  ((psHead->L2 << 6) & 0x3FC0 )| (psHead->L1 & 0x003F); 
  
    // 地址域
    eRet = emt_trans_address(MT_TRANS_F2U, &(psUnpack->sAddress), &(psHead->A));
    if(MT_OK != eRet)
    {
        #ifdef MT_DBG
        DEBUG("emtLiteUnPack() emt_trans_addr() error = %d\n", eRet);
        #endif
        return eRet;
    }

    // 控制域
    eRet = emt_trans_ctrl(MT_TRANS_F2U, &(psUnpack->sCtrl), &(psHead->C));
    if(MT_OK != eRet)
    {
        #ifdef MT_DBG
        DEBUG("emtLiteUnPack() emt_trans_ctrl() error = %d\n", eRet);
        #endif
        return eRet;
    }

    //
 
    // SEQ
    eRet = emt_trans_seq(MT_TRANS_F2U, &psUnpack->sSEQ, (sMtSEQ_f*)&(psHead->SEQ));
    if(MT_OK != eRet)
    {
        #ifdef MT_DBG
        DEBUG("emtLiteUnPack() emt_trans_seq() error = %d\n", eRet);
        #endif
        return eRet;
    }

    bTp = psUnpack->sSEQ.bTpv;

    // AFN
    eAFN = (eMtAFN)(psHead->AFN);
    psUnpack->eAFN = eAFN;

    // 计算除了附加域的应用层数据字长
    eDir = psUnpack->sCtrl.eDir;
    if(eDir == MT_DIR_S2M)
    {
         bEc = psUnpack->sCtrl.bAcd_Fcb;
    }
    else
    {
         bEc = bmt_have_ec(eAFN, eDir);
    }
    
    usAuxLen = usmt_get_aux_len(eAFN, eDir, bEc, bTp);
    nLenUserField = (INT32)(usLenUserField - usAuxLen - MT_CANS_LEN);   
    
    #if 0
    #ifdef MT_DBG
    DEBUG("emtLiteUnPack() nLenUserField = %d\n", nLenUserField);
    #endif
    #endif
    
    // 应用层数据 数据单元标识与数据单元组
    pucTemp = (UINT8*)((UINT8*)&(psHead->SEQ) + 1);
    while(nLenUserField > 0)
    {
        // 初始化
        usUsrdLen = nLenUserField;
        
        // 数据单元标识
        pDaDt = (sMtDaDt*)pucTemp;
        eRet = emt_dadt_to_pnfn(pDaDt, &sPnFn);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtLiteUnPack() emt_dadt_to_pnfn() error = %d\n", eRet);
            #endif
            return eRet;
        }

        pucTemp += sizeof(sMtDaDt);
       
        bP0 = bmt_is_p0(sPnFn.usPn);
        if(TRUE == bP0)
        {  
            //P0的情况作单循环 
            ucPnCycMax = 1;
        }
        else       
        {  
            // 不是P0的情况作双重循环
            ucPnCycMax = 8;
        }

        psUnpack->sData[i].sPnFn = sPnFn;
        for(pi = 0; pi < ucPnCycMax; pi++)
        {
            usPn = sPnFn.usPn[pi];
            if(MT_PN_NONE == usPn)
            {
                // 合法但不处理, 即没有对应位置的PN
            }
            else if(usPn < MT_PN_MIN || usPn > MT_PN_MAX)
            {
                // 非法Fn
                #ifdef MT_DBG
                DEBUG("emtLiteUnPack() usPn error Pn = %d\n", usPn);
                #endif
                return MT_ERR_PARA;
            }
            else
            {
                for(fi = 0; fi < 8; fi++)
                {
                    ucFn = sPnFn.ucFn[fi];
                    if(MT_FN_NONE == ucFn)
                    {
                        // 这种情况即没有该FN, 合法但不处理
                    }
                    else if(ucFn < MT_FN_MIN || ucFn > MT_FN_MAX)
                    {
                        // 非法Fn
                        #ifdef MT_DBG
                        DEBUG("emtLiteUnPack() ucFn error Fn = %d\n", ucFn);
                        #endif
                        return MT_ERR_PARA;
                    }
                    else
                    {
                        eCmd = (eMtCmd)((eAFN << 8) | (sPnFn.ucFn[fi]));
                        eRet = eMtGetCmdInfor(eCmd, eDir, &sCmdInfor);
                        if(MT_OK != eRet)
                        {
                            #ifdef MT_DBG
                            DEBUG("emtLiteUnPack() eMtGetCmdInfor() failed %d %s!", eRet, smtGetErr(eRet));
                            #endif
                            return eRet;
                        }
         
                        pFunc = sCmdInfor.pFunc;
                        if(NULL != pFunc)
                        {
                            // 申请内存,存放应用层数据, 由调用者释放该内存, 该函数内不释放
                            puApp = (uMtApp*)malloc(sizeof(uMtApp));
                            if(!puApp)
                            {
                                #ifdef MT_DBG
                                DEBUG("emtLiteUnPack() malloc failed!");
                                #endif
                                return MT_ERR_IO; 
                            }

                            psUnpack->sData[i].puApp[pi][fi] = puApp;
                            
                            eRet = pFunc(MT_TRANS_F2U, (void*)(psUnpack->sData[i].puApp[pi][fi]), (void*)pucTemp, &usUsrdLen); 
                            if(eRet != MT_OK)
                            {
                                #ifdef MT_DBG
                                DEBUG("emtLiteUnPack() transfunc() error = %d\n", eRet);
                                #endif
                                return eRet;
                            }

                            // 数据指针后移
                            pucTemp += usUsrdLen;

                            // 计录数据总长
                            usUsrdLenTotal += usUsrdLen;
                        }
                    }
                }
            }
        }
           
        // 输出数据
        usDataNum++;
        
        // 长度计算
        nLenUserField -= sizeof(sMtDaDt);
        nLenUserField -= usUsrdLenTotal;

        // 重置计数
        usUsrdLenTotal = 0;
        i++;
    }
    
    // 如果有EC
    if(TRUE == bEc)
    {
        psEC = (sMtEC*)pucTemp;
        psUnpack->sEC.ucEC1 = psEC->ucEC1;
        psUnpack->sEC.ucEC2 = psEC->ucEC2;
        pucTemp += sizeof(sMtEC);
    }
    
    // 如果有PW
    if(TRUE == bmt_have_pw((eMtAFN)(psHead->AFN), eDir))
    {
        memcpy((void*)(psUnpack->acPW), (void*)pucTemp, MT_PW_LEN);
        pucTemp += MT_PW_LEN;
    }

    // 如果有TP
    if(TRUE == bTp)
    {
        eRet = emt_trans_tp(MT_TRANS_F2U, &(psUnpack->sTP), (sMtTP_f*)pucTemp);
        if(eRet != MT_OK)
        {
            #ifdef MT_DBG
            DEBUG("eMtUnpack() emt_trans_tp() error = %d\n", eRet);
            #endif
            return eRet;
        }

        //pucTemp += sizeof(sMtfTp);
    }

    psUnpack->usDataNum = usDataNum;
    return MT_OK;
}


/*****************************************************************************
 函 数 名  : emtPackLite
 功能描述  : 封装函数(高级接口)
 实现功能  : (1) 实现离散的信息点与信息类的自动分组
             (2) 自动取当前的时间来合成tp
             (3) 为上层屏蔽相关的参数 如功能码等
             
 输入参数  : smtPack* psPack  
             UINT16* pusLen   
             UINT8* pOutBuf   
 输出参数  : 无
 返 回 值  : 
 调用函数  : emtLitePack()
 被调函数  : emtPack()
 
 修改历史      :
  1.日    期   : 2013年8月7日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtPackLite(smtPack* psPack, UINT16* pusLen, UINT8* pOutBuf)
{
    if(!psPack || !pusLen || !pOutBuf)
    {
        #ifdef MT_DBG
        DEBUG("emtPackLite() pointer is null!");
        #endif
        return MT_ERR_NULL;
    }

    if(TRUE != g_bMtInit)
    {
        #ifdef MT_DBG
        DEBUG("emtPackLite() protocol is not init!");
        #endif
        return MT_ERR_INIT;
    }

    eMtErr         eRet         = MT_OK;
    eMtCmd         eCmd         = CMD_AFN_F_UNKOWN;
    eMtDir         eDir         = MT_DIR_UNKOWN;
    eMtAFN         eAFN         = AFN_NULL;
    eMtAFN         eAFNCmd      = AFN_NULL;  // 命令对应的AFN
    UINT8          ucTeamPn     = 0xFF;
    UINT8          ucTeamPnBase = 0xFF;
    UINT8          ucTeamFn     = 0xFF;
    UINT8          ucTeamFnBase = 0xFF;
    UINT8          ucFn         = 0;
    UINT16         usPn         = 0;
    INT32          FnIndex      = 0;
    INT32          PnIndex      = 0;
    INT32          i            = 0;
    INT32          j            = 0;
    INT32          k            = 0;
    INT32          nDaDtNum     = 0;      // 封装packbase的信息标识与64个数据单元组的总个数
    INT32          nDaDtPos     = 0;      // 每次应放入packbase的信息标识组的索引
    BOOL           bFindDaDt    = FALSE;  // 是否找到之前存在的组
    BOOL           bAcd_Fcb     = FALSE;
    BOOL           bPW          = FALSE;  // 有没有Pw 字段
    BOOL           bEC          = FALSE;  // 有没有EC 字段
    BOOL           bTP          = FALSE;  // 是否有Tp 字段
    BOOL           bInFn8       = FALSE;
    BOOL           bInPn8       = FALSE;
    UINT8*         pMemBase     = NULL;
    smtLitePack*   psLitePack   = NULL;
    sMtCtrl        sCtrl;
    sMtTP          sTp;                 // 用户侧Tp字段信息
    sMtCmdInfor    sCmdInfor;

    // 为参数早请内存
    pMemBase  = (UINT8*)malloc(MT_USER_MAX);
    if(!pMemBase)
    { 
        #ifdef MT_DBG
        DEBUG("emtPackLite() malloc failed!");
        #endif
        return MT_ERR_IO; 
    }

    psLitePack = (smtLitePack*)pMemBase;

    // 封装成 smtLitePack 参数
    eDir = psPack->eDir;
    eAFN = psPack->eAFN;

    // Afn
    psLitePack->eAFN = eAFN;

    // SEQ
    psLitePack->sSEQ.bTpv  = bmt_have_tp(eAFN, eDir);
    psLitePack->sSEQ.bCon  = bmt_need_con(eAFN, eDir);
    psLitePack->sSEQ.ePos  = psPack->ePos;
    psLitePack->sSEQ.ucSeq = psPack->ucSeq;

    // 地址域
    psLitePack->sAddress = psPack->sAddress;

    // 控制域
    bAcd_Fcb = psPack->bAcdFcb;
    
    eRet = emt_get_ctrl(eAFN, eDir, psPack->ePRM, bAcd_Fcb, &sCtrl);
    if(MT_OK != eRet)
    {
        #ifdef MT_DBG
        DEBUG("emtPackLite() emt_get_ctrl() failed! %d %s", eRet, smtGetErr(eRet));
        #endif
        MT_FREE(pMemBase);
        return eRet;
    }
    
    psLitePack->sCtrl = sCtrl;
    
    // pw 
    bPW = bmt_have_pw(eAFN, eDir);
    if(TRUE == bPW)
    {
       //memcpy(psBasePack->acPW, psPack->acPW, MT_PW_LEN);
       memcpy(psLitePack->acPW, g_aucPw, MT_PW_LEN);
    }

    // ec
    bEC = bmt_have_ec(eAFN, eDir);
    if(TRUE == bEC)
    {
        psLitePack->sEC.ucEC1 = g_tEC.ucEC1;
        psLitePack->sEC.ucEC2 = g_tEC.ucEC2;
    }

    // tp 
    bTP = bmt_have_tp(eAFN, eDir);
    if(TRUE == bTP)
    {
        (void)emt_get_tp(psPack->sTP.ucPFC, &sTp);
        psLitePack->sTP.sDDHHmmss = sTp.sDDHHmmss;
        psLitePack->sTP.ucPermitDelayMinutes = sTp.ucPermitDelayMinutes;
        // psBasePack->sTP.ucPFC = psBasePack->sTP.ucPFC;// 这个已经在封装时由用户添加
    }

    // 将离散的数据单元标识与数据单元组合成组
    for(i = 0; i < psPack->usDataNum; i++)
    {
        eCmd = psPack->sData[i].eCmd;
        
        // 判断该命令是否是合法的、可支持的
        eRet = eMtGetCmdInfor(eCmd, eDir, &sCmdInfor);
        if(eRet != MT_OK)
        {
            #ifdef MT_DBG
            DEBUG("emtPackLite() eMtGetCmdInfor() failed %d %s!", eRet, smtGetErr(eRet));
            #endif
            MT_FREE(pMemBase);
            return eRet;
        }

        // 判断该命令是否属于AFN的子命令
        eAFNCmd = eGetCmdAfn(eCmd);
        if(eAFNCmd != eAFN)  
        {
            MT_FREE(pMemBase);
            #ifdef MT_DBG
            DEBUG("emtPackLite() cmd is not is a same Afn");
            #endif
            return MT_ERR_TEAM;
        }

        usPn = psPack->sData[i].usPN;
        ucFn = ucGetCmdFn(eCmd);

        /*
            判断该合法的数据单元标识,与数据单元是否已经填入应在位置 psBasePack
            如果重复,后者覆盖前者, Da2 与不同的Dt2组合, 
            会产生不同的标识与数据单元组, 决定nDaTaNum的个数
        */
        
        nDaDtPos = nDaDtNum;
        
        // 初始化PnFn组
        for(k = 0; k < 8; k++)
        {
            psLitePack->sData[nDaDtPos].sPnFn.ucFn[k] = MT_FN_NONE;
            psLitePack->sData[nDaDtPos].sPnFn.usPn[k] = MT_PN_NONE;
        }
             
        for(j = 0; j < nDaDtNum; j++)
        {
            // 找到则nDataPos置找到的位置
            // 先判断Pn是否在同一组
            ucTeamPn     = ucmt_get_pn_team(usPn);
            ucTeamPnBase = ucmt_get_pn8_team(psLitePack->sData[j].sPnFn.usPn);
            
            if(ucTeamPn == ucTeamPnBase)
            {
                // 再判断Fn是否属于同一个组
                ucTeamFn     = ucmt_get_fn_team(ucFn);
                ucTeamFnBase = ucmt_get_fn8_team(psLitePack->sData[j].sPnFn.ucFn);
                       
                if(ucTeamFn == ucTeamFnBase)
                {
                    //bInPn8 = bmt_in_pn8(usPn, psLitePack->sData[j].sPnFn.usPn);
                    bInFn8 = bmt_in_fn8(ucFn, psLitePack->sData[j].sPnFn.ucFn);

                   // if(TRUE == bInPn8 && TRUE == bInFn8)
					if(TRUE == bInFn8)
                    {
                        bFindDaDt = TRUE;
                        nDaDtPos  = j;
                        break;
                    }
                }
            }
        }

        // 未找到则nDaTaNum++
        if(FALSE == bFindDaDt)
        {
            nDaDtNum  += 1; // 新启一组  
        }

        // 放入 DaTa组及数据单元
        if(0 == usPn)
        {
            PnIndex = 0;
            
            for(k = 0; k < 8; k++)
            {
                psLitePack->sData[nDaDtPos].sPnFn.usPn[k] = 0;
            }
        }
        else
        {
            PnIndex = (usPn - 1) % 8; 
            psLitePack->sData[nDaDtPos].sPnFn.usPn[PnIndex] = usPn;
        }
      
        FnIndex = (ucFn - 1) % 8;
        psLitePack->sData[nDaDtPos].sPnFn.ucFn[FnIndex] = ucFn;

        // 数据单元 
        psLitePack->sData[nDaDtPos].puApp[PnIndex][FnIndex] =  &(psPack->sData[i].uApp);

        // 重置未找到状态
        bFindDaDt = FALSE;
        
    }

    psLitePack->usDataNum = nDaDtNum;

    // 调用 emtLitePack()
    eRet = emtLitePack(psLitePack, pusLen , pOutBuf);
    if(MT_OK != eRet)
    {
        #ifdef MT_DBG
        DEBUG("emtPackLite() emtLitePack() failed! code : %d %s", eRet, smtGetErr(eRet));
        #endif
        MT_FREE(pMemBase);
        return eRet;
    }

    MT_FREE(pMemBase);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtUnPackLite
 功能描述  : 协议解析(高级接口)
             将emtBaseUnPack解析出来的信息smtBasePack, 封装成高级smtPack
             
             (1)将重要信息提取
             (2)将PnFn数据单元提取成离散的
             
 输入参数  : smtPack *psUnpack  
             UINT8* pInBuf      
             UINT16 usLen      
             
 输出参数  : 无
 返 回 值  : 
 调用函数  : emtLiteUnPack()
 被调函数  : emtUnPack()
 
 修改历史      :
  1.日    期   : 2013年8月9日 星期五
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtUnPackLite(smtPack *psUnpack, UINT8* pInBuf, UINT16 usLen)
{
    if(!psUnpack || !pInBuf)
    {
        #ifdef MT_DBG
        DEBUG("emtUnPackLite() pointer is null!");
        #endif
        return MT_ERR_NULL;
    }

    if(TRUE != g_bMtInit)
    {
        #ifdef MT_DBG
        DEBUG("emtUnPackLite() protocol is not init!");
        #endif
        return MT_ERR_INIT;
    }
    
    eMtErr       eRet          = MT_OK;
    eMtCmd       eCmd          = CMD_AFN_F_UNKOWN;
    eMtAFN       eAFN          = AFN_NULL;
    UINT8*       pMemBase      = NULL;
    smtLitePack* pLiteUnpack   = NULL;
    uMtApp      *puApp         = NULL;
    BOOL         bP0           = FALSE;
    INT32        i             = 0;
    INT32        j             = 0;
    INT32        fi            = 0;
    INT32        pi            = 0;
    INT32        PnCyc         = 0;
    UINT16       usPn          = 0;
    UINT8        ucFn          = 0;
    sMtCmdInfor  sCmdInfor;

    pMemBase = (UINT8*)malloc(MT_USER_MAX);
    if(!pMemBase)
    {
        #ifdef MT_DBG
        DEBUG("emtUnPackLite() malloc failed!");
        #endif
        return MT_ERR_IO;
    }
    
    pLiteUnpack = (smtLitePack*)pMemBase;

    // 调用解析函数
    eRet = emtLiteUnPack(pLiteUnpack, pInBuf, usLen);
    if(MT_OK != eRet)
    {
        #ifdef MT_DBG
        DEBUG("emtUnPackLite() emtLiteUnPack failed! %d %s", eRet, smtGetErr(eRet));
        #endif
        MT_FREE(pMemBase);
        return eRet;
    }

    // 转换数据结构
    eAFN               = pLiteUnpack->eAFN;
    psUnpack->sAddress = pLiteUnpack->sAddress;
    psUnpack->eAFN     = eAFN;
    psUnpack->eDir     = pLiteUnpack->sCtrl.eDir;
    psUnpack->ePRM     = pLiteUnpack->sCtrl.ePRM;
    psUnpack->ePos     = pLiteUnpack->sSEQ.ePos;
    psUnpack->bCon     = pLiteUnpack->sSEQ.bCon;
    psUnpack->ucSeq    = pLiteUnpack->sSEQ.ucSeq;
    psUnpack->bAcdFcb  = pLiteUnpack->sCtrl.bAcd_Fcb;
    psUnpack->bPW      = bmt_have_pw(psUnpack->eAFN, psUnpack->eDir);
    psUnpack->bEC      = bmt_have_ec(psUnpack->eAFN, psUnpack->eDir);
    psUnpack->bTP      = pLiteUnpack->sSEQ.bTpv;

    if(TRUE == psUnpack->bPW)
    {
        memcpy(psUnpack->acPW, pLiteUnpack->acPW, MT_PW_LEN);
    }
    
    if(TRUE == psUnpack->bEC)
    {
        psUnpack->sEC = pLiteUnpack->sEC;
    }

    if(TRUE == psUnpack->bTP)
    {
        psUnpack->sTP = pLiteUnpack->sTP;
    }

    // 应用层数据
    for(i = 0; i < pLiteUnpack->usDataNum; i++)
    {
        bP0 = bmt_is_p0(pLiteUnpack->sData[i].sPnFn.usPn);
        PnCyc = ((bP0 == TRUE) ? 1 : 8);
        
        for(pi = 0; pi < PnCyc; pi++)
        {
            usPn = pLiteUnpack->sData[i].sPnFn.usPn[pi];
            if(MT_PN_NONE == usPn)
            {
                // 合法但不处理, 即没有对应位置的PN
            }
            else if(usPn < MT_PN_MIN || usPn > MT_PN_MAX)
            {
                // 非法Fn
                #ifdef MT_DBG
                DEBUG("emtUnPackLite() usPn error Pn = %d\n", usPn);
                #endif
                MT_FREE(pMemBase);
                return MT_ERR_PARA;
            }
            else
            {
                #if 0
                #ifdef MT_DBG
                for(fi = 0; fi < 8; fi++)
                {   ucFn = pUnpackBase->sData[i].sPnFn.ucFn[fi];                
                    DEBUG("emtUnPack() sData[%d].sPnFn.ucFn[%d] = %d",i, fi, ucFn);
                }
                #endif
                #endif
              
                for(fi = 0; fi < 8; fi++)
                {
                    ucFn = pLiteUnpack->sData[i].sPnFn.ucFn[fi];
                    if(MT_FN_NONE == ucFn)
                    {
                        // 这种情况即没有该FN, 合法但不处理
                    }
                    else if(ucFn < MT_FN_MIN || ucFn > MT_FN_MAX)
                    {
                        // 非法Fn
                        #ifdef MT_DBG
                        DEBUG("emtUnPackLite() ucFn error Fn = %d\n", ucFn);
                        #endif
                        MT_FREE(pMemBase);
                        return MT_ERR_PARA;
                    }
                    else
                    {
                        eCmd = (eMtCmd)((eAFN << 8) | ucFn);
                        eRet = eMtGetCmdInfor(eCmd, psUnpack->eDir, &sCmdInfor);
                        if(MT_OK != eRet)
                        {
                            #ifdef MT_DBG
                            DEBUG("emtUnPackLite() eMtGetCmdInfor() failed %d %s!", eRet, smtGetErr(eRet));
                            #endif
                            MT_FREE(pMemBase);
                            return eRet;
                        }
                        
                        // 一个有效的数据单元标识组
                        psUnpack->sData[j].eCmd  = eCmd;
                        psUnpack->sData[j].usPN  = usPn;

                        if(NULL != sCmdInfor.pFunc)
                        {
                            psUnpack->sData[j].bApp = TRUE;
                            puApp = pLiteUnpack->sData[i].puApp[pi][fi];
                            if(!puApp)
                            {
                                #ifdef MT_DBG
                                DEBUG("emtUnPackLite() app date is null");
                                #endif
                                MT_FREE(pMemBase);
                                return MT_ERR_PARA;
                            }

                            memcpy((void*)&(psUnpack->sData[j].uApp), (void*)puApp, sizeof(uMtApp));
                            MT_FREE(puApp);
                        }
                        else
                        {
                            psUnpack->sData[j].bApp = FALSE;
                        }
                    
                        j++;                        
                    }
                }
            }
        }
    }

    psUnpack->usDataNum = j;
    MT_FREE(pMemBase);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtBasePack
 功能描述  : 实现基本的报文封装, 加入加密算法接口
             该处对应用层数据加密 加密后可能可能会改
             变长度,加密的数据区包括：应用层功能码、数据单元标识及数据单元部分
 输入参数  : smtBasePack* psPack  
             UINT16* pusLen       
             UINT8* pOutBuf       
 输出参数  : 无
 返 回 值  : 
 调用函数  : emt_pack_common
 被调函数  : emtPackBase
 
 修改历史      :
  1.日    期   : 2013年8月6日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtBasePack(smtBasePack* psPack, UINT16* pusLen, UINT8* pOutBuf)
{
    if(!psPack || !pusLen || !pOutBuf)
    {
        #ifdef MT_DBG
        DEBUG("emtBasePack() pointer is null!");
        #endif
        return MT_ERR_NULL;
    }

    if(TRUE != g_bMtInit)
    {
        #ifdef MT_DBG
        DEBUG("emtBasePack() protocol is not init!");
        #endif
        return MT_ERR_INIT;
    }

    eMtErr         eRet        = MT_OK;
    eMtCmd         eCmd        = CMD_AFN_F_UNKOWN;
    eMtDir         eDir        = MT_DIR_UNKOWN;
    eMtAFN         eAFN        = AFN_NULL;
    BOOL           bSameTeam   = FALSE;
    BOOL           bP0         = FALSE;
    UINT8          ucPnNum     = 0;
    UINT16         usSeq2CsLen = 0;
    UINT16         usSeq2CsPos = 0; 
    UINT16         usfDataLen  = 0;      // 帧侧数据单元字长
    UINT16         usCMD       = 0;
    INT32          i           = 0;
    INT32          j           = 0;
    INT32          fi          = 0;
    INT32          pi          = 0;
    UINT8*         pSeq2Cs     = NULL;
    UINT8*         puData      = NULL;   // 数据单元用户侧数据
    UINT8*         pData       = NULL;   // 数据单元
    sMtDaDt*       pDaDt       = NULL;   // 数据单元标识
    sMtEC*         psEC        = NULL;
    sMtTP_f*       psfTp       = NULL;   // 帧侧Tp字段
    pMtFunc        pFunc       = NULL;
    sMtCmdInfor    sCmdInfor;   
    sMtTP          suTp;                 // 用户侧Tp字段信息
    sMtPnFn        sPnFn;
    sMtComPack     sPackCommon;

    // 加密
    #if MT_CFG_ENCRYPT
    UINT8*         pEncry_in   = NULL;  // 需要加密部分的首地址 输入 
    INT32          nLen_in     = 0;     // 明文的总字长
    UINT8*         pEncry_out  = NULL;  // 需要加密部分的首地址 输出
    INT32          nLen_out    = 0;     // 密文的总字长
    #endif
    
    memset(&(sPnFn), 0x00, sizeof(sMtPnFn));
    memset(&sPackCommon, 0x00, sizeof(sMtComPack));

    if(MT_ROLE_MASTER == g_eMtRole)
    {
        eDir = MT_DIR_M2S;
    }
    else
    {
        eDir = MT_DIR_S2M;
    }

    /* 封装sMtComPack 入参 */
    // 构建通用封包参数
    pSeq2Cs = (UINT8*)malloc(MT_SEQ2CS_BYTES_MAX);
    if(!pSeq2Cs)
    {
        #ifdef MT_DBG
        DEBUG("emtBasePack() malloc failed!");
        #endif
        return MT_ERR_IO;
    }

    // 封装变长的数据单元标识与数据单元组
    if(0 == psPack->usDataNum)
    {
        #ifdef MT_DBG
        DEBUG("emtBasePack() ucSubNum is 0!");
        #endif
        MT_FREE(pSeq2Cs);
        return MT_ERR_PROTO;
    }

    // 封装至少一个数据标识与数据单元组合
    for(i = 0; i < psPack->usDataNum; i++)
    {
        for(j = 0; j < PN_INDEX_MAX; j++)
        {
            sPnFn.usPn[j] =  psPack->sData[i].sPnFn.usPn[j];
            sPnFn.ucFn[j] =  psPack->sData[i].sPnFn.ucFn[j];
        }

        // 确保每个Pn 都属于同一个信息点组
        bSameTeam = bmt_same_team_pn(sPnFn.usPn, NULL);
        if(FALSE == bSameTeam)
        {
            #ifdef MT_DBG
            DEBUG("emtBasePack() pn is not is same team!");
            #endif
            MT_FREE(pSeq2Cs);
            return MT_ERR_TEAM;
        }
        
        // 确保每个Fn 都属于同一个信息类组
        bSameTeam = bmt_same_team_fn(psPack->sData[i].sPnFn.ucFn, NULL);
        if(FALSE == bSameTeam)
        {
            #ifdef MT_DBG
            DEBUG("emtBasePack() Fn is not is same team!");
            #endif
            MT_FREE(pSeq2Cs);
            return MT_ERR_TEAM;
        }

        // 主功能码
        eAFN = psPack->eAFN;

        // 封装数据单元标识
        pDaDt = (sMtDaDt*)(pSeq2Cs + usSeq2CsPos);
        eRet = emt_pnfn_to_dadt(&(sPnFn), pDaDt);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtBasePack() emt_pnfn_to_dadt() failed %d %s!", eRet, smtGetErr(eRet));
            #endif
            return eRet;
        }

        usSeq2CsPos +=  sizeof(sMtDaDt);
        /*
            逐个封装最多64个与Fn Pn组对应的数据单元
            数据单元为按数据单元标识所组织的数据，包括参数、命令等。
            数据组织的顺序规则：先按pn从小到大、再按Fn从小到大的次序，即：完成一个信息点pi
            的所有信息类Fn的处理后，再进行下一个pi+1的处理。
        */

        // 分两种情况
        bP0 = bmt_is_p0(psPack->sData[i].sPnFn.usPn);
        if(TRUE == bP0)
        {
             ucPnNum = 1;            // 1 - p0
        }
        else
        {
             ucPnNum = PN_INDEX_MAX; // 2 - 含有最多8个Pn 
        }
        
        // 应用层数据域
        for(pi = 0; pi < ucPnNum; pi++)
        {
            if(MT_PN_NONE != psPack->sData[i].sPnFn.usPn[pi])
            {
                for(fi = 0; fi < FN_INDEX_MAX; fi++)
                {
                    if(MT_FN_NONE != psPack->sData[i].sPnFn.ucFn[fi])
                    {
                         // 合成命令码
                        usCMD = (UINT16)((eAFN << 8) | (psPack->sData[i].sPnFn.ucFn[fi]));
                        eCmd  = (eMtCmd)usCMD; 

                        // 获得命令信息
                        eRet = eMtGetCmdInfor(eCmd, eDir, &sCmdInfor);
                        if(MT_OK != eRet)
                        {
                            #ifdef MT_DBG
                            DEBUG("emtBasePack() eMtGetCmdInfor() failed %d %s!", eRet, smtGetErr(eRet));
                            #endif
                            MT_FREE(pSeq2Cs);
                            return eRet;
                        }

                        // 数据单元
                        pData = (UINT8*)(pSeq2Cs + usSeq2CsPos);
                        pFunc = sCmdInfor.pFunc;
                        if(NULL != pFunc)
                        {
                            puData = (UINT8*)&(psPack->sData[i].uApp[pi][fi]);
                            eRet = pFunc(MT_TRANS_U2F, puData, pData, &usfDataLen);
                            if(MT_OK != eRet)
                            {
                                #ifdef MT_DBG
                                DEBUG("emtBasePack() transU2FpFunc() failed %d %s!", eRet, smtGetErr(eRet));
                                #endif
                                MT_FREE(pSeq2Cs);
                                return eRet;
                            }

                            usSeq2CsPos += usfDataLen;
                        }     
                    }
                }
            }
        }
    }

    // 加密
    #if MT_CFG_ENCRYPT
    if(NULL != g_peMtEncryptFunc)
    {
        pEncry_in  = pSeq2Cs;  // 这里需要再调整,请确认需要加密的部分
        nLen_in    = usSeq2CsPos;
        pEncry_out = (UINT8*)malloc(MT_SEQ2CS_BYTES_MAX);
        if(!pEncry_out)
        {
            #ifdef MT_DBG
            DEBUG("emtBasePack() malloc failed!");
            #endif
            return MT_ERR_IO; 
        }
        
        eRet = g_peMtEncryptFunc(pEncry_in, nLen_in, pEncry_out, &nLen_out);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtBasePack() encrypt failed! %d", eRet);
            #endif
            MT_FREE(pEncry_out);
            return MT_ERR_ENCRYPT;
        }

        // 替换成密文
        memcpy((void*)pSeq2Cs, (void*)pEncry_out, nLen_out); 

        // 更新长度
        usSeq2CsPos = nLen_out;

    }
    #endif

    /* 附加信息域AUX */
    // 如果有 EC
    if(TRUE == bmt_have_ec(eAFN, eDir))
    {       
        pData = (UINT8*)(pSeq2Cs + usSeq2CsPos);
        psEC = (sMtEC*)pData;
        if(MT_DIR_S2M == eDir)
        {
            psEC->ucEC1 = g_tEC.ucEC1;
            psEC->ucEC2 = g_tEC.ucEC2;
        }
        else
        {
            // 疑问: 主站的该字段如何封装 全填0 还是 0xEE
            psEC->ucEC1 = 0x0;
            psEC->ucEC2 = 0x0;
        }
       
        usSeq2CsPos += sizeof(sMtEC);
     }
    
    // 如果有 pw  
    // pw用于重要的下行报文中,由主站产生,收端验证，通过后执行(16字节)
    if(TRUE == bmt_have_pw(eAFN, eDir))    
    {
        pData = (UINT8*)(pSeq2Cs + usSeq2CsPos);
        memcpy((void*)pData, (void*)g_aucPw, MT_PW_LEN); 
        usSeq2CsPos += MT_PW_LEN;
    }

    // 如果有 TP
    if(TRUE == bmt_have_tp(eAFN, eDir))  // 除了这种情况的所有报文中
    {
        pData = (UINT8*)(pSeq2Cs + usSeq2CsPos);
        psfTp = (sMtTP_f *)pData;
        suTp  = psPack->sTP;
  
        //(void)emt_get_mtutp(psPack->ucPFC, &suTp); 由上层封装时间戳
        (void)emt_trans_tp(MT_TRANS_U2F, &suTp, psfTp);
        usSeq2CsPos += sizeof(sMtTP_f);
    }

    // 总结数据
    usSeq2CsLen             = usSeq2CsPos;
    sPackCommon.sAddr       = psPack->sAddress;
    sPackCommon.sCtrl       = psPack->sCtrl;;
    sPackCommon.sSEQ        = psPack->sSEQ;   
    sPackCommon.usSeq2CsLen = usSeq2CsLen;
    sPackCommon.pSeq2Cs     = pSeq2Cs;

    // 通过通用封包函数pack
    eRet = emt_pack_common(eAFN,  &sPackCommon, pusLen, pOutBuf);   
    if(MT_OK != eRet)
    {
        #ifdef MT_DBG
        DEBUG("emtBasePack() emt_pack_common failed:%s!", smtGetErr(eRet));
        #endif

        MT_FREE(pSeq2Cs);
        // 释放加密部分申请的内存
        #if MT_CFG_ENCRYPT
        MT_FREE(pEncry_out);
        #endif
        return eRet;
    }

    MT_FREE(pSeq2Cs);
    
    // 释放加密部分申请的内存
    #if MT_CFG_ENCRYPT
    MT_FREE(pEncry_out);
    #endif
    
    return MT_OK;
}
         
/*****************************************************************************
 函 数 名  : emtBaseUnPack
 功能描述  : 实现基本的报文解封装, 加入加密算法接口
             该处对应用层数据解密 解密后可能可能会改变长度,
             解密的数据区包括：应用层功能码、数据单元标识及数据单元部分
 输入参数  : smtBasePack *psUnpack  
             UINT8* pInBuf          
             UINT16 usLen           
 输出参数  : 无
 返 回 值  : 
 调用函数  : emtUnPackBase
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月6日 星期二
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtBaseUnPack(smtBasePack *psUnpack, UINT8* pInBuf, UINT16 usLen)
{
    if(!psUnpack || !pInBuf)
    {
        #ifdef MT_DBG
        DEBUG("emtBaseUnPack() pointer is null!");
        #endif
        return MT_ERR_NULL;
    }

    if(TRUE != g_bMtInit)
    {
        #ifdef MT_DBG
        DEBUG("emtBaseUnPack() protocol is not init!");
        #endif
        return MT_ERR_INIT;
    }

    if(usLen < MT_FRM_LEN_MIN)
    {
        #ifdef MT_DBG
        DEBUG("emtBaseUnPack() input frame length is too short usLen = %d", usLen);
        #endif 
        return MT_ERR_UNCOMP;
    }
    
    eMtErr      eRet            = MT_OK;
    eMtDir      eDir            = MT_DIR_UNKOWN;
    eMtAFN      eAFN            = AFN_NULL;
    eMtCmd      eCmd            = CMD_AFN_F_UNKOWN;
    BOOL        bP0             = FALSE;
    BOOL        bTp             = FALSE;   
    BOOL        bEc             = FALSE;
    sMtfComHead *psHead         = NULL;
    sMtDaDt     *pDaDt          = NULL;
    UINT8       *pucTemp        = NULL;
    sMtEC       *psEC           = NULL;
    pMtFunc     pFunc           = NULL;
    UINT8       ucFn            = 0;
    UINT8       ucPnCycMax      = 0;        // 按Pn循环的最大值
    UINT16      usPn            = 0;
    UINT16      usUsrdLen       = 0;        //用户应用数据在帧侧的字长
    UINT16      usUsrdLenTotal  = 0;        //用户应用数据在帧侧的字长
    UINT16      usAuxLen        = 0;        // 该类型帧的EcPwTp帧侧的几个域的部字长
    UINT16      usLenUserField  = 0;        // 用户数据区长度       
    UINT16      usDataNum       = 0;     // 数据单元组的个数
    INT32       nLenUserField   = 0;        // 每次解析一个子命令数据单元后的用户数据域字长 
    INT32       i               = 0;
    INT32       fi              = 0;        // Fn的索引
    INT32       pi              = 0;        // Pn的索引
    sMtPnFn     sPnFn;
    sMtCmdInfor sCmdInfor;

    // 判断该帧是否是一个有效的帧
    eRet = emtIsValidPack(pInBuf, usLen);
    if(MT_OK != eRet)
    {
        #ifdef MT_DBG
        DEBUG("emtBaseUnPack() input is not a valid pack eRet = %d", eRet);
        #endif 
        return MT_ERR_PACK;
    }

    // 报文头
    psHead = (sMtfComHead *)pInBuf;
    usLenUserField =  ((psHead->L2 << 6) & 0x3FC0 )| (psHead->L1 & 0x003F); 
  
    // 地址域
    eRet = emt_trans_address(MT_TRANS_F2U, &(psUnpack->sAddress), &(psHead->A));
    if(MT_OK != eRet)
    {
        #ifdef MT_DBG
        DEBUG("emtBaseUnPack() emt_trans_addr() error = %d\n", eRet);
        #endif
        return eRet;
    }

    // 控制域
    eRet = emt_trans_ctrl(MT_TRANS_F2U, &(psUnpack->sCtrl), &(psHead->C));
    if(MT_OK != eRet)
    {
        #ifdef MT_DBG
        DEBUG("emtBaseUnPack() emt_trans_ctrl() error = %d\n", eRet);
        #endif
        return eRet;
    }

    // SEQ
    eRet = emt_trans_seq(MT_TRANS_F2U, &psUnpack->sSEQ, (sMtSEQ_f*)&(psHead->SEQ));
    if(MT_OK != eRet)
    {
        #ifdef MT_DBG
        DEBUG("emtBaseUnPack() emt_trans_seq() error = %d\n", eRet);
        #endif
        return eRet;
    }

    bTp = psUnpack->sSEQ.bTpv;
    eAFN = (eMtAFN)(psHead->AFN);
    psUnpack->eAFN = eAFN;

    // 计算除了附加域的应用层数据字长
    eDir = psUnpack->sCtrl.eDir;
    if(eDir == MT_DIR_S2M)
    {
         bEc = psUnpack->sCtrl.bAcd_Fcb;
    }
    else
    {
         bEc = bmt_have_ec(eAFN, eDir);
    }
    
    usAuxLen = usmt_get_aux_len(eAFN, eDir, bEc, bTp);
    nLenUserField = (INT32)(usLenUserField - usAuxLen - MT_CANS_LEN);   
    
    #if 0
    #ifdef MT_DBG
    DEBUG("emtBaseUnPack() nLenUserField = %d\n", nLenUserField);
    #endif
    #endif
    
    // 应用层数据 数据单元标识与数据单元组
    pucTemp = (UINT8*)((UINT8*)&(psHead->SEQ) + 1);
    while(nLenUserField > 0)
    {
        // 初始化
        usUsrdLen = nLenUserField;
        
        // 数据单元标识
        pDaDt = (sMtDaDt*)pucTemp;
        eRet = emt_dadt_to_pnfn(pDaDt, &sPnFn);
        if(MT_OK != eRet)
        {
            #ifdef MT_DBG
            DEBUG("emtBaseUnPack() emt_dadt_to_pnfn() error = %d\n", eRet);
            #endif
            return eRet;
        }

        pucTemp += sizeof(sMtDaDt);
      
        bP0 = bmt_is_p0(sPnFn.usPn);
        if(TRUE == bP0)
        {  
            //P0的情况作单循环 
            ucPnCycMax = 1;
        }
        else       
        {  
            // 不是P0的情况作双重循环
            ucPnCycMax = 8;
        }

        psUnpack->sData[i].sPnFn = sPnFn;
        for(pi = 0; pi < ucPnCycMax; pi++)
        {
            usPn = sPnFn.usPn[pi];
            if(MT_PN_NONE == usPn)
            {
                // 合法但不处理, 即没有对应位置的PN
            }
            else if(usPn < MT_PN_MIN || usPn > MT_PN_MAX)
            {
                // 非法Fn
                #ifdef MT_DBG
                DEBUG("emtBaseUnPack() usPn error Pn = %d\n", usPn);
                #endif
                return MT_ERR_PARA;
            }
            else
            {
                for(fi = 0; fi < 8; fi++)
                {
                    ucFn = sPnFn.ucFn[fi];
                    if(MT_FN_NONE == ucFn)
                    {
                        // 这种情况即没有该FN, 合法但不处理
                    }
                    else if(ucFn < MT_FN_MIN || ucFn > MT_FN_MAX)
                    {
                        // 非法Fn
                        #ifdef MT_DBG
                        DEBUG("emtBaseUnPack() ucFn error Fn = %d\n", ucFn);
                        #endif
                        return MT_ERR_PARA;
                    }
                    else
                    {
                        eCmd = (eMtCmd)((eAFN << 8) | (sPnFn.ucFn[fi]));
                        eRet = eMtGetCmdInfor(eCmd, eDir, &sCmdInfor);
                        if(MT_OK != eRet)
                        {
                            #ifdef MT_DBG
                            DEBUG("emtBaseUnPack() eMtGetCmdInfor() failed %d %s!", eRet, smtGetErr(eRet));
                            #endif
                            return eRet;
                        }
         
                        pFunc = sCmdInfor.pFunc;
                        if(NULL != pFunc)
                        {
                            eRet = pFunc(MT_TRANS_F2U, (void*)&(psUnpack->sData[i].uApp[pi][fi]), (void*)pucTemp, &usUsrdLen); 
                            if(eRet != MT_OK)
                            {
                                #ifdef MT_DBG
                                DEBUG("emtBaseUnPack() transfunc() error = %d\n", eRet);
                                #endif
                                return eRet;
                            }

                            // 数据指针后移
                            pucTemp += usUsrdLen;

                            // 计录数据总长
                            usUsrdLenTotal += usUsrdLen;
                        }
                    }
                }
            }
        }
           
        // 输出数据
        usDataNum++;
        
        // 长度计算
        nLenUserField -= sizeof(sMtDaDt);
        nLenUserField -= usUsrdLenTotal;

        // 重置计数
        usUsrdLenTotal = 0;
        i++;
    }
    
    // 如果有EC
    if(TRUE == bEc)
    {
        psEC = (sMtEC*)pucTemp;
        psUnpack->sEC.ucEC1 = psEC->ucEC1;
        psUnpack->sEC.ucEC2 = psEC->ucEC2;
        pucTemp += sizeof(sMtEC);
    }
    
    // 如果有PW
    if(TRUE == bmt_have_pw((eMtAFN)(psHead->AFN), eDir))
    {
        memcpy((void*)(psUnpack->acPW), (void*)pucTemp, MT_PW_LEN);
        pucTemp += MT_PW_LEN;
    }

    // 如果有TP
    if(TRUE == bTp)
    {
        eRet = emt_trans_tp(MT_TRANS_F2U, &(psUnpack->sTP), (sMtTP_f*)pucTemp);
        if(eRet != MT_OK)
        {
            #ifdef MT_DBG
            DEBUG("eMtUnpack() emt_trans_tp() error = %d\n", eRet);
            #endif
            return eRet;
        }

        //pucTemp += sizeof(sMtfTp);
    }

    psUnpack->usDataNum = usDataNum;
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtPackBase
 功能描述  : 封装函数(高级接口)
 实现功能  : (1) 实现离散的信息点与信息类的自动分组
             (2) 自动取当前的时间来合成tp
             (3) 为上层屏蔽相关的参数 如功能码等
             
 输入参数  : smtPack* psPack  
             UINT16* pusLen   
             UINT8* pOutBuf   
 输出参数  : 无
 返 回 值  : 
 调用函数  : emtBasePack
 被调函数  : emtPack()
 
 修改历史      :
  1.日    期   : 2013年8月7日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtPackBase(smtPack* psPack, UINT16* pusLen, UINT8* pOutBuf)
{
    if(!psPack || !pusLen || !pOutBuf)
    {
        #ifdef MT_DBG
        DEBUG("emtPack() pointer is null!");
        #endif
        return MT_ERR_NULL;
    }

    if(TRUE != g_bMtInit)
    {
        #ifdef MT_DBG
        DEBUG("emtPack() protocol is not init!");
        #endif
        return MT_ERR_INIT;
    }

    eMtErr         eRet         = MT_OK;
    eMtCmd         eCmd         = CMD_AFN_F_UNKOWN;
    eMtDir         eDir         = MT_DIR_UNKOWN;
    eMtAFN         eAFN         = AFN_NULL;
    eMtAFN         eAFNCmd      = AFN_NULL;  // 命令对应的AFN
    UINT8          ucTeamPn     = 0xFF;
    UINT8          ucTeamPnBase = 0xFF;
    UINT8          ucTeamFn     = 0xFF;
    UINT8          ucTeamFnBase = 0xFF;
    UINT8          ucFn         = 0;
    UINT16         usPn         = 0;
    INT32          FnIndex      = 0;
    INT32          PnIndex      = 0;
    INT32          i            = 0;
    INT32          j            = 0;
    INT32          k            = 0;
    INT32          nDaDtNum     = 0;      // 封装packbase的信息标识与64个数据单元组的总个数
    INT32          nDaDtPos     = 0;      // 每次应放入packbase的信息标识组的索引
    BOOL           bFindDaDt    = FALSE;  // 是否找到之前存在的组
    BOOL           bAcd_Fcb     = FALSE;
    BOOL           bPW          = FALSE;  // 有没有Pw 字段
    BOOL           bEC          = FALSE;  // 有没有EC 字段
    BOOL           bTP          = FALSE;  // 是否有Tp 字段
    BOOL           bInFn8       = FALSE;
    BOOL           bInPn8       = FALSE;
    UINT8*         pMemBase     = NULL;
    smtBasePack*   psBasePack   = NULL;
    sMtCtrl        sCtrl;
    sMtTP          sTp;                 // 用户侧Tp字段信息
    sMtCmdInfor    sCmdInfor;

    // 为参数早请内存
    pMemBase  = (UINT8*)malloc(MT_USER_MAX);
    if(!pMemBase)
    { 
        #ifdef MT_DBG
        DEBUG("emtPack() malloc failed!");
        #endif
        return MT_ERR_IO; 
    }

    psBasePack = (smtBasePack*)pMemBase;

    // 封装成 smtBasePack 参数
    eDir = psPack->eDir;
    eAFN = psPack->eAFN;

    // Afn
    psBasePack->eAFN = eAFN;

    // SEQ
    psBasePack->sSEQ.bTpv  = bmt_have_tp(eAFN, eDir);
    psBasePack->sSEQ.bCon  = bmt_need_con(eAFN, eDir);
    psBasePack->sSEQ.ePos  = psPack->ePos;
    psBasePack->sSEQ.ucSeq = psPack->ucSeq;

    // 地址域
    psBasePack->sAddress = psPack->sAddress;

    // 控制域
    bAcd_Fcb = psPack->bAcdFcb;
    
    eRet = emt_get_ctrl(eAFN, eDir, psPack->ePRM, bAcd_Fcb, &sCtrl);
    if(MT_OK != eRet)
    {
        #ifdef MT_DBG
        DEBUG("emtPack() emt_get_ctrl() failed! %d %s", eRet, smtGetErr(eRet));
        #endif
        MT_FREE(pMemBase);
        return eRet;
    }
    
    psBasePack->sCtrl = sCtrl;
    
    // pw 
    bPW = bmt_have_pw(eAFN, eDir);
    if(TRUE == bPW)
    {
       //memcpy(psBasePack->acPW, psPack->acPW, MT_PW_LEN);
       memcpy(psBasePack->acPW, g_aucPw, MT_PW_LEN);
    }

    // ec
    bEC = bmt_have_ec(eAFN, eDir);
    if(TRUE == bEC)
    {
        psBasePack->sEC.ucEC1 = g_tEC.ucEC1;
        psBasePack->sEC.ucEC2 = g_tEC.ucEC2;
    }

    // tp 
    bTP = bmt_have_tp(eAFN, eDir);
    if(TRUE == bTP)
    {
        (void)emt_get_tp(psPack->sTP.ucPFC, &sTp);
        psBasePack->sTP.sDDHHmmss = sTp.sDDHHmmss;
        psBasePack->sTP.ucPermitDelayMinutes = sTp.ucPermitDelayMinutes;
        // psBasePack->sTP.ucPFC = psBasePack->sTP.ucPFC;// 这个已经在封装时由用户添加
    }

    // 将离散的数据单元标识与数据单元组合成组
    for(i = 0; i < psPack->usDataNum; i++)
    {
        eCmd = psPack->sData[i].eCmd;
        
        // 判断该命令是否是合法的、可支持的
        eRet = eMtGetCmdInfor(eCmd, eDir, &sCmdInfor);
        if(eRet != MT_OK)
        {
            #ifdef MT_DBG
            DEBUG("emtPack() eMtGetCmdInfor() failed %d %s!", eRet, smtGetErr(eRet));
            #endif
            MT_FREE(pMemBase);
            return eRet;
        }

        // 判断该命令是否属于AFN的子命令
        eAFNCmd = eGetCmdAfn(eCmd);
        if(eAFNCmd != eAFN)  
        {
            MT_FREE(pMemBase);
            #ifdef MT_DBG
            DEBUG("emtPack() cmd is not is a same Afn");
            #endif
            return MT_ERR_TEAM;
        }

        usPn = psPack->sData[i].usPN;
        ucFn = ucGetCmdFn(eCmd);

        /*
            判断该合法的数据单元标识,与数据单元是否已经填入应在位置 psBasePack
            如果重复,后者覆盖前者, Da2 与不同的Dt2组合, 
            会产生不同的标识与数据单元组, 决定nDaTaNum的个数
        */
        
        nDaDtPos = nDaDtNum;
        
        // 初始化PnFn组
        for(k = 0; k < 8; k++)
        {
            psBasePack->sData[nDaDtPos].sPnFn.ucFn[k] = MT_FN_NONE;
            psBasePack->sData[nDaDtPos].sPnFn.usPn[k] = MT_PN_NONE;
        }
             
        for(j = 0; j < nDaDtNum; j++)
        {
            // 找到则nDataPos置找到的位置
            // 先判断Pn是否在同一组
            ucTeamPn     = ucmt_get_pn_team(usPn);
            ucTeamPnBase = ucmt_get_pn8_team(psBasePack->sData[j].sPnFn.usPn);
            
            if(ucTeamPn == ucTeamPnBase)
            {
                // 再判断Fn是否属于同一个组
                ucTeamFn     = ucmt_get_fn_team(ucFn);
                ucTeamFnBase = ucmt_get_fn8_team(psBasePack->sData[j].sPnFn.ucFn);

                if(ucTeamFn == ucTeamFnBase)
                {
                    bInPn8 = bmt_in_pn8(usPn, psBasePack->sData[j].sPnFn.usPn);
                    bInFn8 = bmt_in_fn8(ucFn, psBasePack->sData[j].sPnFn.ucFn);

                    if(TRUE == bInPn8 || TRUE == bInFn8)
                    {
                        bFindDaDt = TRUE;
                        nDaDtPos  = j;
                        break;
                    }
                }
            }
        }

        // 未找到则nDaTaNum++
        if(FALSE == bFindDaDt)
        {
            nDaDtNum  += 1; // 新启一组  
        }

        // 放入 DaTa组及数据单元
        if(0 == usPn)
        {
            PnIndex = 0;
            
            for(k = 0; k < 8; k++)
            {
                psBasePack->sData[nDaDtPos].sPnFn.usPn[k] = 0;
            }
        }
        else
        {
            PnIndex = (usPn - 1) % 8; 
            psBasePack->sData[nDaDtPos].sPnFn.usPn[PnIndex] = usPn;
        }
      
        FnIndex = (ucFn - 1) % 8;
        psBasePack->sData[nDaDtPos].sPnFn.ucFn[FnIndex] = ucFn;

        // 数据单元 
        psBasePack->sData[nDaDtPos].uApp[PnIndex][FnIndex] =  psPack->sData[i].uApp;

        // 重置未找到状态
        bFindDaDt = FALSE;
        
    }

    psBasePack->usDataNum = nDaDtNum;

    // 调用 emtBasePack()
    eRet = emtBasePack(psBasePack, pusLen , pOutBuf);
    if(MT_OK != eRet)
    {
        #ifdef MT_DBG
        DEBUG("emtPack() emtBasePack() failed! code : %d %s", eRet, smtGetErr(eRet));
        #endif
        MT_FREE(pMemBase);
        return eRet;
    }

    MT_FREE(pMemBase);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtUnPackBase
 功能描述  : 协议解析(高级接口)
             将emtBaseUnPack解析出来的信息smtBasePack, 封装成高级smtPack
             
             (1)将重要信息提取
             (2)将PnFn数据单元提取成离散的
             
 输入参数  : smtPack *psUnpack  
             UINT8* pInBuf      
             UINT16 usLen      
             
 输出参数  : 无
 返 回 值  : 
 调用函数  : emtBaseUnPack
 被调函数  : emtUnPack
 
 修改历史      :
  1.日    期   : 2013年8月9日 星期五
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtUnPackBase(smtPack *psUnpack, UINT8* pInBuf, UINT16 usLen)
{
    if(!psUnpack || !pInBuf)
    {
        #ifdef MT_DBG
        DEBUG("emtUnPack() pointer is null!");
        #endif
        return MT_ERR_NULL;
    }

    if(TRUE != g_bMtInit)
    {
        #ifdef MT_DBG
        DEBUG("emtPack() protocol is not init!");
        #endif
        return MT_ERR_INIT;
    }
    
    eMtErr       eRet          = MT_OK;
    eMtCmd       eCmd          = CMD_AFN_F_UNKOWN;
    eMtAFN       eAFN          = AFN_NULL;
    UINT8*       pMemBase      = NULL;
    smtBasePack* pUnpackBase   = NULL;
    BOOL         bP0           = FALSE;
    INT32        i             = 0;
    INT32        j             = 0;
    INT32        fi            = 0;
    INT32        pi            = 0;
    INT32        PnCyc         = 0;
    UINT16       usPn          = 0;
    UINT8        ucFn          = 0;
    sMtCmdInfor  sCmdInfor;

    pMemBase = (UINT8*)malloc(MT_USER_MAX);
    if(!pMemBase)
    {
        #ifdef MT_DBG
        DEBUG("emtUnPack() malloc failed!");
        #endif
        return MT_ERR_IO;
    }
    
    pUnpackBase = (smtBasePack*)pMemBase;

    // 调用解析函数
    eRet = emtBaseUnPack(pUnpackBase, pInBuf, usLen);
    if(MT_OK != eRet)
    {
        #ifdef MT_DBG
        DEBUG("emtUnPack() emtBaseUnPack failed! %d %s", eRet, smtGetErr(eRet));
        #endif
        MT_FREE(pMemBase);
        return eRet;
    }

    // 转换数据结构
    eAFN               = pUnpackBase->eAFN;
    psUnpack->sAddress = pUnpackBase->sAddress;
    psUnpack->eAFN     = eAFN;
    psUnpack->eDir     = pUnpackBase->sCtrl.eDir;
    psUnpack->ePRM     = pUnpackBase->sCtrl.ePRM;
    psUnpack->ePos     = pUnpackBase->sSEQ.ePos;
    psUnpack->bCon     = pUnpackBase->sSEQ.bCon;
    psUnpack->ucSeq    = pUnpackBase->sSEQ.ucSeq;
    psUnpack->bAcdFcb  = pUnpackBase->sCtrl.bAcd_Fcb;
    psUnpack->bPW      = bmt_have_pw(psUnpack->eAFN, psUnpack->eDir);
    psUnpack->bEC      = bmt_have_ec(psUnpack->eAFN, psUnpack->eDir);
    psUnpack->bTP      = pUnpackBase->sSEQ.bTpv;

    if(TRUE == psUnpack->bPW)
    {
        memcpy(psUnpack->acPW, pUnpackBase->acPW, MT_PW_LEN);
    }
    
    if(TRUE == psUnpack->bEC)
    {
        psUnpack->sEC = pUnpackBase->sEC;
    }

    if(TRUE == psUnpack->bTP)
    {
        psUnpack->sTP = pUnpackBase->sTP;
    }

    // 应用层数据
    for(i = 0; i < pUnpackBase->usDataNum; i++)
    {
        bP0 = bmt_is_p0(pUnpackBase->sData[i].sPnFn.usPn);
        PnCyc = ((bP0 == TRUE) ? 1 : 8);
        
        for(pi = 0; pi < PnCyc; pi++)
        {
            usPn = pUnpackBase->sData[i].sPnFn.usPn[pi];
            if(MT_PN_NONE == usPn)
            {
                // 合法但不处理, 即没有对应位置的PN
            }
            else if(usPn < MT_PN_MIN || usPn > MT_PN_MAX)
            {
                // 非法Fn
                #ifdef MT_DBG
                DEBUG("emtUnPack() usPn error Pn = %d\n", usPn);
                #endif
                MT_FREE(pMemBase);
                return MT_ERR_PARA;
            }
            else
            {
                #if 0
                #ifdef MT_DBG
                for(fi = 0; fi < 8; fi++)
                {   ucFn = pUnpackBase->sData[i].sPnFn.ucFn[fi];                
                    DEBUG("emtUnPack() sData[%d].sPnFn.ucFn[%d] = %d",i, fi, ucFn);
                }
                #endif
                #endif
              
                for(fi = 0; fi < 8; fi++)
                {
                    ucFn = pUnpackBase->sData[i].sPnFn.ucFn[fi];
                    if(MT_FN_NONE == ucFn)
                    {
                        // 这种情况即没有该FN, 合法但不处理
                    }
                    else if(ucFn < MT_FN_MIN || ucFn > MT_FN_MAX)
                    {
                        // 非法Fn
                        #ifdef MT_DBG
                        DEBUG("emtUnPack() ucFn error Fn = %d\n", ucFn);
                        #endif
                        MT_FREE(pMemBase);
                        return MT_ERR_PARA;
                    }
                    else
                    {
                        eCmd = (eMtCmd)((eAFN << 8) | ucFn);
                        eRet = eMtGetCmdInfor(eCmd, psUnpack->eDir, &sCmdInfor);
                        if(MT_OK != eRet)
                        {
                            #ifdef MT_DBG
                            DEBUG("emtUnPack() eMtGetCmdInfor() failed %d %s!", eRet, smtGetErr(eRet));
                            #endif
                            MT_FREE(pMemBase);
                            return eRet;
                        }
                        
                        if(NULL != sCmdInfor.pFunc)
                        {
                            psUnpack->sData[j].bApp = TRUE;
                        }
                        else
                        {
                            psUnpack->sData[j].bApp = FALSE;
                        }

                        // 一个有效的数据单元标识组
                        psUnpack->sData[j].eCmd  = eCmd;
                        psUnpack->sData[j].usPN  = usPn;
                        psUnpack->sData[j].uApp  = pUnpackBase->sData[i].uApp[pi][fi];
                        j++;
                    }
                }
            }
        }
    }

    psUnpack->usDataNum = j;
    MT_FREE(pMemBase);
    return MT_OK;
}

/*****************************************************************************
 函 数 名  : emtPack
 功能描述  : 封装函数(高级接口)
 实现功能  : (1) 实现离散的信息点与信息类的自动分组
             (2) 自动取当前的时间来合成tp
             (3) 为上层屏蔽相关的参数 如功能码等
             
 输入参数  : smtPack* psPack  
             UINT16* pusLen   
             UINT8* pOutBuf   
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : emtBasePack()
 
 修改历史      :
  1.日    期   : 2013年8月7日 星期三
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtPack(smtPack* psPack, UINT16* pusLen, UINT8* pOutBuf)
{
    eMtErr eRet = MT_OK;

    // 两种方式任选某一, emtPackBase较费内存
    // eRet = emtPackBase(psPack, pusLen, pOutBuf);
    eRet = emtPackLite(psPack, pusLen, pOutBuf);
    
    return eRet;
}

/*****************************************************************************
 函 数 名  : emtUnPack
 功能描述  : 协议解析(高级接口)
             将emtBaseUnPack解析出来的信息smtBasePack, 封装成高级smtPack
             
             (1)将重要信息提取
             (2)将PnFn数据单元提取成离散的
             
 输入参数  : smtPack *psUnpack  
             UINT8* pInBuf      
             UINT16 usLen      
             
 输出参数  : 无
 返 回 值  : 
 调用函数  : emtBaseUnPack
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年8月9日 星期五
    作    者   : liming
    修改内容   : 新生成函数

*****************************************************************************/
eMtErr emtUnPack(smtPack *psUnpack, UINT8* pInBuf, UINT16 usLen)
{
     eMtErr eRet = MT_OK;

    // 两种方式任选某一, emtUnPackBase较费内存
    //eRet = emtUnPackBase(psUnpack, pInBuf, usLen);
    eRet = emtUnPackLite(psUnpack, pInBuf, usLen);
    
    return eRet;
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CQgdw3761api::CQgdw3761api()
{

}

CQgdw3761api::~CQgdw3761api()
{

}


char * CQgdw3761api::getErrMessage( eMtErr err )
{
	char * pRet;
	switch(err)
	{
	case MT_OK:             // 无错误   
		pRet = "MT_OK";
		break;
	case MT_ERR_NULL:           // 指针为空    
        pRet = "MT_ERR_NULL";
		break;
	case MT_ERR_OUTRNG:        // 参数越界  
		pRet = "MT_ERR_OUTRNG";
		break;
	case MT_ERR_NONE:           // 不存在:没有找到  
		pRet = "MT_ERR_NONE";
		break;
	case MT_ERR_IO:         // IO错误    
		pRet = "MT_ERR_IO";
		break;
	case MT_ERR_RES:        // 资源不足
		pRet = "MT_ERR_RES";
		break;
	case MT_ERR_INIT:       // 没有初始化
		pRet = "MT_ERR_INIT";
		break;
	case MT_ERR_PARA:       // 参数错误
		pRet = "MT_ERR_PARA";
		break;
	case MT_ERR_FN:         // 指定Afn 中不支持的 Fn
		pRet = "MT_ERR_FN";
		break;
	case MT_ERR_PN:         // 指定Afn Fn 中不支持的 Pn
		pRet = "MT_ERR_PN";
		break;
	case MT_ERR_ADDR:       // 无效地址
		pRet = "MT_ERR_ADDR";
		break;
	case MT_ERR_CTRL:       // 控制域错误 
		pRet = "MT_ERR_CTRL";
		break;
	case MT_ERR_SEQ:        // SEQ域错误
		pRet = "MT_ERR_SEQ";
		break;
	case MT_ERR_SEQ2CS:     // 
		pRet = "MT_ERR_SEQ2CS";
		break;
	case MT_ERR_AFN:        // AFN 错误
		pRet = "MT_ERR_AFN";
		break;
	case MT_ERR_UNCOMP:     // 不完整的协议包
		pRet = "MT_ERR_UNCOMP";
		break;
	case MT_ERR_0x68:       // 错误的报文起始
		pRet = "MT_ERR_0x68";
		break;
	case MT_ERR_PROTO:      // 错误的协议标识
		pRet = "MT_ERR_PROTO";
		break;
	case MT_ERR_CS:         // 错误的检测和
		pRet = "MT_ERR_CS";
		break;
	case MT_ERR_0x16:       // 错误的结尾符
		pRet = "MT_ERR_0x16";
		break;
	case MT_ERR_LOGIC:      // 错误的处理逻辑
		pRet = "MT_ERR_LOGIC";
		break;
	case MT_ERR_PACK:       // 不是一个有效的包 
		pRet = "MT_ERR_PACK";
		break;
	default:
		pRet = "unkown";
		break;
		
	}
	
	return pRet;
}

/************************************************************************
函数名称：CStringToHexBuffer
函数功能：将字符串 转换向16进制的char[],并输出其长度
输入参数：strText
输出参数：pOutBuf
		  pLen
返回值：  BOOL 转换成功 true 否则 false
函数说明：

************************************************************************/
BOOL CQgdw3761api::CStringToHexBuffer( CString strText, char *pOutBuf, UINT16 *pLen )
{
	if(!pOutBuf || !pLen)
	{
		return FALSE;
	}

	// 去掉空格
	strText.Replace(" ",NULL);
	int StrLen     = 0;
	int BufLen     = 0;
	int strLenLeft = 0;
	int i          = 0;
	char *pBuf     = NULL;
	CString strHex, strTmp;

	StrLen = strText.GetLength();

	if(StrLen%2 != 0)
	{
		return FALSE;
	}
	
	BufLen = StrLen/2;
	strLenLeft = StrLen;

	for (i = 0; i < BufLen; i++)
	{
		strHex = strText.Left(2);
		pOutBuf[i] = (char)strtol(strHex, NULL, 16);
		strLenLeft -= 2;
		strText = strText.Right(strLenLeft);

	}

	*pLen = BufLen;


	return TRUE;	
}


CString CQgdw3761api::GetAfnName( eMtAFN eAfn )
{
	CString strName;
	switch(eAfn)
	{
	case AFN_00_CONF:
		strName = "AFN_00_CONF 确认否认";
		break;
	case AFN_01_RSET:
		strName = "AFN_01_RSET 复位";
		break;
	case AFN_02_LINK:
		strName = "AFN_02_LINK 接口检测";
		break;
	case AFN_03_RELY:
		strName = "AFN_03_RELY 中继站命令";
		break;
	case AFN_04_SETP:
		strName = "AFN_04_SETP 设置参数";
		break;
	case AFN_05_CTRL:
		strName = "AFN_05_CTRL 控制命令";
		break;
	case AFN_06_AUTH:
		strName = "AFN_06_AUTH 身份认证";
		break;
	case AFN_08_CASC:
		strName = "AFN_08_CASC 请求被级联终端主动上报";
		break;
	case AFN_09_CFIG:
		strName = "AFN_09_CFIG 请求终端配置";
		break;
	case AFN_0A_GETP:
		strName = "AFN_0A_GETP 查询参数";
		break;
	case AFN_0B_ASKT:
		strName = "AFN_0B_ASKT 请求任务数据";
		break;
	case AFN_0C_ASK1:
		strName = "AFN_0C_ASK1 请求1类数据 实时数据";
		break;
	case AFN_0D_ASK2:
		strName = "AFN_0D_ASK2 请求2类数据 历史数据";
		break;
	case AFN_0E_ASK3:
		strName = "AFN_0E_ASK3 请求3类数据 事件数据";
		break;
	case AFN_0F_FILE:
		strName = "AFN_0F_FILE 文件传输";
		break;
	case AFN_10_DATA:
		strName = "AFN_10_DATA 数据转发";
		break;
	default:
		break;
	}
	
	return strName;
}

CString CQgdw3761api::BufferToHex(char *pBuf, UINT16 usLen)
{
	CString strOut;
	CString strTmp;
	int len;
	len = (int)usLen;
	int i;
	for (i = 0; i < len; i++)
	{
		strTmp.Format("%02X ",(UINT8)pBuf[i]);
		strOut+= strTmp;
		
	}	
	return strOut;
}
