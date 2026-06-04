#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"
#include "zpc_zxc_Headfile.h"

#define C30D_HWT101_TEST			0
#define C30D_WHEEL_STEPPER_TEST	0
#define C30D_STEPPER_TEST_RPM		60
#define C30D_POSITION_WHEEL_TEST	0
#define C30D_POSITION_TEST_STEP		1600
#define C30D_CHASSIS_TEST			1
#define C30D_CHASSIS_TEST_MM		300
#define C30D_CHASSIS_TEST_RPM		60
#define C30D_CHASSIS_TEST_ACC		100
#define C30D_USE_OLED		1

#if C30D_HWT101_TEST
static void C30D_FormatFixedLine(char *Line,float Value)
{
	int32_t Scaled;
	int32_t Whole;
	uint8_t Frac;

	if(Value>=0)
	{
		Scaled=(int32_t)(Value*10.0f+0.5f);
		Line[4]='+';
	}
	else
	{
		Scaled=(int32_t)(-Value*10.0f+0.5f);
		Line[4]='-';
	}

	Whole=Scaled/10;
	Frac=(uint8_t)(Scaled%10);
	if(Whole>9999)Whole=9999;

	Line[5]=(char)('0'+Whole/1000%10);
	Line[6]=(char)('0'+Whole/100%10);
	Line[7]=(char)('0'+Whole/10%10);
	Line[8]=(char)('0'+Whole%10);
	Line[10]=(char)('0'+Frac);
}
#endif

#if C30D_WHEEL_STEPPER_TEST || C30D_POSITION_WHEEL_TEST
static void C30D_WaitNextKey(FlagStatus *LastKeyStatus)
{
	while(Key_Status==*LastKeyStatus);
	*LastKeyStatus=Key_Status;
	Delay_ms(200);
}
#endif

//	APB1/APB1_TIM	42MHz
//	APB2/APB2_TIM	84MHz
int main(void)
{
#if C30D_HWT101_TEST
	/*====================C30D HWT101单独测试====================*/
	FlagStatus LastKeyStatus=Key_Status;
	char YawLine[]="Yaw:+0000.0   ";
	char WzLine[] ="Wz :+0000.0   ";
	char StatusLine[]="A:0 W:0 KeyZero";

	Delay_ms(100);
	Key_TIMScanInit();
#if C30D_USE_OLED
	OLED_Init();
	OLED_ShowString(1,1,"HWT101 Test");
	OLED_ShowString(2,1,YawLine);
	OLED_ShowString(3,1,WzLine);
	OLED_ShowString(4,1,StatusLine);
#endif
	HWT101_Init();

	while(1)
	{
		C30D_FormatFixedLine(YawLine,HWT101_Yaw);
		C30D_FormatFixedLine(WzLine,HWT101_Yaw_W);
		StatusLine[2]=HWT101_AngleRxFlag?'1':'0';
		StatusLine[6]=HWT101_wRxFlag?'1':'0';

		if(Key_Status!=LastKeyStatus)
		{
			LastKeyStatus=Key_Status;
			HWT101_AngleCheck();
#if C30D_USE_OLED
			OLED_ShowString(4,1,"Yaw Zeroed     ");
#endif
			Delay_ms(300);
		}

#if C30D_USE_OLED
		OLED_ShowString(2,1,YawLine);
		OLED_ShowString(3,1,WzLine);
		OLED_ShowString(4,1,StatusLine);
#endif
		Delay_ms(100);
	}
#elif C30D_WHEEL_STEPPER_TEST
	/*====================C30D四轮张大头步进逐个测试====================*/
	uint8_t MotorID=1;
	FlagStatus LastKeyStatus=Key_Status;
	char Line1[]="Wheel ID1 Test";
	char ForwardLine[]="ID1 Forward  ";
	char StopLine[]="ID1 Stop     ";
	char ReverseLine[]="ID1 Reverse  ";
	char DoneLine[]="ID1 Done     ";

	Delay_ms(100);
	Key_TIMScanInit();
#if C30D_USE_OLED
	OLED_Init();
	OLED_ShowString(1,1,"Wheel Test");
	OLED_ShowString(2,1,"Press Key");
#endif
	StepMotor_Init();

	while(1)
	{
		Line1[8]='0'+MotorID;
#if C30D_USE_OLED
		OLED_ShowString(1,1,Line1);
		OLED_ShowString(2,1,"Press Key   ");
#endif
		C30D_WaitNextKey(&LastKeyStatus);

#if C30D_USE_OLED
		ForwardLine[2]='0'+MotorID;
		OLED_ShowString(2,1,ForwardLine);
#endif
		StepMotor_SetSpeed(MotorID,C30D_STEPPER_TEST_RPM);
		Delay_ms(2000);

#if C30D_USE_OLED
		StopLine[2]='0'+MotorID;
		OLED_ShowString(2,1,StopLine);
#endif
		StepMotor_SetSpeed(MotorID,0);
		Delay_ms(800);

#if C30D_USE_OLED
		ReverseLine[2]='0'+MotorID;
		OLED_ShowString(2,1,ReverseLine);
#endif
		StepMotor_SetSpeed(MotorID,-C30D_STEPPER_TEST_RPM);
		Delay_ms(2000);

		StepMotor_SetSpeed(MotorID,0);
#if C30D_USE_OLED
		DoneLine[2]='0'+MotorID;
		OLED_ShowString(2,1,DoneLine);
#endif
		Delay_ms(800);

		MotorID++;
		if(MotorID>4)MotorID=1;
	}
#elif C30D_POSITION_WHEEL_TEST
	/*====================C30D单轮位置模式逐个测试====================*/
	uint8_t MotorID=1;
	FlagStatus LastKeyStatus=Key_Status;
	char Line1[]="Pos ID1 Test ";
	char ForwardLine[]="ID1 Pos +   ";
	char ReverseLine[]="ID1 Pos -   ";
	char DoneLine[]="ID1 Done    ";

	Delay_ms(100);
	Key_TIMScanInit();
#if C30D_USE_OLED
	OLED_Init();
	OLED_ShowString(1,1,"Pos Wheel Test");
	OLED_ShowString(2,1,"Press Key");
#endif
	StepMotor_Init();

	while(1)
	{
		Line1[6]='0'+MotorID;
#if C30D_USE_OLED
		OLED_ShowString(1,1,Line1);
		OLED_ShowString(2,1,"Press Key   ");
#endif
		C30D_WaitNextKey(&LastKeyStatus);

#if C30D_USE_OLED
		ForwardLine[2]='0'+MotorID;
		OLED_ShowString(2,1,ForwardLine);
#endif
		StepMotor_PositionModeBegin();
		Delay_ms(20);
		StepMotor_SetPositionSync(MotorID,C30D_POSITION_TEST_STEP,40,80);
		Delay_ms(10);
		StepMotor_SyncMove();
		Delay_ms(2200);

#if C30D_USE_OLED
		ReverseLine[2]='0'+MotorID;
		OLED_ShowString(2,1,ReverseLine);
#endif
		StepMotor_SetPositionSync(MotorID,-C30D_POSITION_TEST_STEP,40,80);
		Delay_ms(10);
		StepMotor_SyncMove();
		Delay_ms(2200);
		StepMotor_PositionModeEnd();

#if C30D_USE_OLED
		DoneLine[2]='0'+MotorID;
		OLED_ShowString(2,1,DoneLine);
#endif
		Delay_ms(800);

		MotorID++;
		if(MotorID>4)MotorID=1;
	}
#elif C30D_CHASSIS_TEST
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

	/*--- 位置模式标定测试: 300mm小方形路径 ---*/
#if C30D_USE_OLED
	OLED_ShowString(2,1,"Pos Forward");
#endif
	Chassis_MoveDistance(0,C30D_CHASSIS_TEST_MM,C30D_CHASSIS_TEST_RPM,C30D_CHASSIS_TEST_ACC);
	Delay_ms(500);

#if C30D_USE_OLED
	OLED_ShowString(2,1,"Pos Left   ");
#endif
	Chassis_MoveDistance(-C30D_CHASSIS_TEST_MM,0,C30D_CHASSIS_TEST_RPM,C30D_CHASSIS_TEST_ACC);
	Delay_ms(500);

#if C30D_USE_OLED
	OLED_ShowString(2,1,"Pos Back   ");
#endif
	Chassis_MoveDistance(0,-C30D_CHASSIS_TEST_MM,C30D_CHASSIS_TEST_RPM,C30D_CHASSIS_TEST_ACC);
	Delay_ms(500);

#if C30D_USE_OLED
	OLED_ShowString(2,1,"Pos Right  ");
#endif
	Chassis_MoveDistance(C30D_CHASSIS_TEST_MM,0,C30D_CHASSIS_TEST_RPM,C30D_CHASSIS_TEST_ACC);

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
