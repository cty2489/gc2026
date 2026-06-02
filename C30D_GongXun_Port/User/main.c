#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"
#include "zpc_zxc_Headfile.h"

#define C30D_CHASSIS_TEST	1
#define C30D_USE_OLED		1

//	APB1/APB1_TIM	42MHz
//	APB2/APB2_TIM	84MHz
int main(void)
{
#if C30D_CHASSIS_TEST
	/*====================C30D底盘单独测试====================*/
	Delay_ms(100);
	Key_TIMScanInit();
#if C30D_USE_OLED
	OLED_Init();
	OLED_ShowString(1,1,"C30D Test");
	OLED_ShowString(2,1,"Press Key");
#endif
	Chassis_Init();
	
	while(Key_Status==RESET);
#if C30D_USE_OLED
	OLED_ShowString(2,1,"Key OK   ");
	Delay_ms(300);
#endif
	Chassis_Reset();
#if C30D_USE_OLED
	OLED_ShowString(2,1,"Running  ");
#endif
	
	Chassis_MoveOnce(0,80,0,120,80);	//向前慢速测试
	Delay_ms(500);
	Chassis_MoveOnce(80,0,0,120,80);	//向右平移测试
	Delay_ms(500);
	Chassis_TurnRight();				//右转90度测试
	
#if C30D_USE_OLED
	OLED_ShowString(2,1,"Finished ");
#endif
	while(1)
	{
	}
#else
	/*====================初始化====================*/
	zpc_zxc_Init();

	while(Key_Status==RESET);
	Chassis_Reset();

	/*====================起动====================*/
	zpc_zxc_StartToPlate();
	
	/*====================第一次抓取物料====================*/
	zpc_zxc_GetWuLiao(0);
	
	/*====================第一次物料盘到粗加工====================*/
	zpc_zxc_PlateToSeHuan1();
	
	/*====================第一次粗加工====================*/
	zpc_zxc_SeHuan1Process(0);
	
	/*====================第一次粗加工到成品====================*/
	zpc_zxc_SeHuan1ToSeHuan2(0);

	/*====================第一次成品====================*/
	zpc_zxc_SeHuan2Process(0);
	
	/*====================成品到物料盘====================*/
	zpc_zxc_SeHuan2ToPlate();

	/*====================第二次抓取物料====================*/
	zpc_zxc_GetWuLiao(1);
	
	/*====================第二次物料盘到粗加工====================*/
	zpc_zxc_PlateToSeHuan1();
	
	/*====================第二次粗加工====================*/
	zpc_zxc_SeHuan1Process(1);
	
	/*====================第二次粗加工到成品====================*/
	zpc_zxc_SeHuan1ToSeHuan2(1);
	
	/*====================第二次成品====================*/
	zpc_zxc_SeHuan2Process(1);
	
	/*====================回家====================*/
	zpc_zxc_GoHome();

	while(1)
	{
	}
#endif
}
