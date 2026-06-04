#ifndef __STEPMOTOR_H
#define __STEPMOTOR_H

#include <stdint.h>

void StepMotor_Init(void);															//步进初始化
void StepMotor_SetSpeed(uint8_t ID,int32_t Speed);									//步进设置速度
void StepMotor_SetPosition(uint8_t ID,uint16_t Speed,uint32_t Step);				//步进设置位置
void StepMotor_SetPositionExt(uint8_t ID,uint16_t Speed,uint8_t Acc,uint32_t Step);	//步进设置位置(扩展)
void StepMotor_PositionModeBegin(void);												//步进底盘位置模式开始
void StepMotor_SetPositionNow(uint8_t ID,int32_t Step,uint16_t Speed,uint8_t Acc);	//步进立即相对位置设置
void StepMotor_SetPositionSync(uint8_t ID,int32_t Step,uint16_t Speed,uint8_t Acc);	//步进同步相对位置设置
void StepMotor_SyncMove(void);														//步进多机同步启动
void StepMotor_PositionModeEnd(void);												//步进底盘位置模式结束
void StepMotor_Callack(void);														//步进定时发送回调函数

#endif
