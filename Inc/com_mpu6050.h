#ifndef __COM_MPU6050_H
#define __COM_MPU6050_H

#include "component.h"

struct MPU6050_Data_t
{
	// 地址
	uint8_t Address;
	// 角速度
	float Accel_X;
	float Accel_Y;
	float Accel_Z;
	// 角度
	float Gyro_X;
	float Gyro_Y;
	float Gyro_Z;
	// 温度
	float Temp;
};

class com_mpu6050
{
public:
	com_mpu6050(I2C_HandleTypeDef *hi2c);
	com_mpu6050(I2C_HandleTypeDef *hi2c, uint8_t Address);

	uint8_t MPU6050_Init(void);

	uint8_t Get_MPU6050_Data(MPU6050_Data_t *pData);
private:
	uint8_t MPU6050_Search(void);
	I2C_HandleTypeDef *hi2c;
	MPU6050_Data_t Data;
};

class Kalman_IMU_c
{// 卡尔曼滤波器的状态变量
public:
	float Yaw;              //偏航角
	float Pitch;            //俯仰角
	float Roll;             //翻滚角

	void Calc(float gx, float gy, float gz, float ax, float ay, float az);

	struct Kalman_Data_t
	{
		float angle; // 角度
		float bias; // 偏差
		float rate; // 角速度
		float P[2][2] = { 1,0,0,1 }; // 协方差矩阵

		void Kalman_Update(float newAngle, float newRate, float dt);
	};
private:

	Kalman_Data_t Yaw_Data;
	Kalman_Data_t Pitch_Data;
	Kalman_Data_t Roll_Data;
};


class Quaternion_IMU_c
{
public:
	Quaternion_IMU_c() :q0(1)
	{}
	float Yaw;              //偏航角
	float Pitch;            //俯仰角
	float Roll;             //翻滚角
	void Calc(float gx, float gy, float gz, float ax, float ay, float az);

	Kalman_IMU_c::Kalman_Data_t Yaw_Kalman;
	Kalman_IMU_c::Kalman_Data_t Pitch_Kalman;
	Kalman_IMU_c::Kalman_Data_t Roll_Kalman;
private:
	float Ki = 100.0f;
	float Kp = 0.002f;
	float halfT = 0.005f;        // 采样周期的一半

	float q0;
	float q1;
	float q2;
	float q3;          // 四元数的元素，代表估计方向

	float exInt;
	float eyInt;
	float ezInt;        // 按比例缩小积分误差
};

//address
#define SMPLRT_DIV  0x19  // 采样率分频，典型值：0x07(125Hz) */
#define EXT_SYNC_SET 
#define CONFIG  0x1A  // 低通滤波频率，典型值：0x06(5Hz) */
#define GYRO_CONFIG  0x1B  // 陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s) */
#define ACCEL_CONFIG  0x1C  // 加速计自检、测量范围及高通滤波频率，典型值：0x01(不自检，2G，5Hz) */

#define ACCEL_XOUT_H  0x3B  // 存储最近的X轴、Y轴、Z轴加速度感应器的测量值 */
#define ACCEL_XOUT_L  0x3C
#define ACCEL_YOUT_H  0x3D
#define ACCEL_YOUT_L  0x3E
#define ACCEL_ZOUT_H  0x3F
#define ACCEL_ZOUT_L  0x40

#define TEMP_OUT_H  0x41  // 存储的最近温度传感器的测量值 */
#define TEMP_OUT_L  0x42

#define GYRO_XOUT_H  0x43  // 存储最近的X轴、Y轴、Z轴陀螺仪感应器的测量值 */
#define GYRO_XOUT_L  0x44
#define GYRO_YOUT_H  0x45
#define GYRO_YOUT_L  0x46
#define GYRO_ZOUT_H  0x47
#define GYRO_ZOUT_L  0x48

#define PWR_MGMT_1  0x6B   // 电源管理，典型值：0x00(正常启用) */
#define WHO_AM_I  0x75 	// IIC地址寄存器(默认数值0x68，只读) */
#define MPU6050_ADDR  0xD0	// MPU6050手册上的地址，这里也可以使用serch函数去搜索

#endif