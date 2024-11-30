#include "component.h"
#include "math.h"
#include "arm_math.h"

com_mpu6050::com_mpu6050(I2C_HandleTypeDef *hi2c)
{
	this->hi2c = hi2c;
}

com_mpu6050::com_mpu6050(I2C_HandleTypeDef *hi2c, uint8_t Address)
{
	this->hi2c = hi2c;
	this->Data.Address = Address;
}

uint8_t com_mpu6050::MPU6050_Init(void)
{
	if(Data.Address == 0)
	{
		if(MPU6050_Search() != COM_OK)
		{
			return COM_ERROR;
		}
	}

	uint8_t check = 0;
	HAL_I2C_Mem_Read(hi2c, Data.Address, 0x75, 1, &check, 1, 1000);
	if(check == 0x68)
	{
		check = 0x00;
		HAL_I2C_Mem_Write(hi2c, Data.Address, PWR_MGMT_1, 1, &check, 1, 1000);
		check = 0x07;
		HAL_I2C_Mem_Write(hi2c, Data.Address, SMPLRT_DIV, 1, &check, 1, 1000);
		check = 0x00;
		HAL_I2C_Mem_Write(hi2c, Data.Address, CONFIG, 1, &check, 1, 1000);
		check = 0x01;
		HAL_I2C_Mem_Write(hi2c, Data.Address, GYRO_CONFIG, 1, &check, 1, 1000);
		return COM_OK;
	}
	return COM_ERROR;
}

uint8_t com_mpu6050::MPU6050_Search()
{
	uint8_t check = 0;
	for(uint8_t i = 0; i < 255; i++)
	{
		if(HAL_I2C_IsDeviceReady(hi2c, i, 1, 1000) == HAL_OK)
		{
			Data.Address = i;
			return COM_OK;
		}
	}
	return COM_ERROR;
}

uint8_t com_mpu6050::Get_MPU6050_Data(MPU6050_Data_t *pData)
{
	if(pData == NULL || Data.Address == NULL) { return COM_ERROR; }
	uint8_t Buf[6], ret = COM_OK;
	pData->Address = Data.Address;

	ret += HAL_I2C_Mem_Read(hi2c, Data.Address, ACCEL_XOUT_H, 1, Buf, 6, 1000);
	Data.Accel_X = pData->Accel_X = (int16_t)(Buf[0] << 8 | Buf[1]) / 1638.4f;
	Data.Accel_Y = pData->Accel_Y = (int16_t)(Buf[2] << 8 | Buf[3]) / 1638.4f;
	Data.Accel_Z = pData->Accel_Z = (int16_t)(Buf[4] << 8 | Buf[5]) / 1638.4f;

	ret += HAL_I2C_Mem_Read(hi2c, Data.Address, GYRO_XOUT_H, 1, Buf, 6, 1000);
	Data.Gyro_X = pData->Gyro_X = (int16_t)(Buf[0] << 8 | Buf[1]) / 131.0f;
	Data.Gyro_Y = pData->Gyro_Y = (int16_t)(Buf[2] << 8 | Buf[3]) / 131.0f;
	Data.Gyro_Z = pData->Gyro_Z = (int16_t)(Buf[4] << 8 | Buf[5]) / 131.0f;

	ret += HAL_I2C_Mem_Read(hi2c, Data.Address, TEMP_OUT_H, 1, Buf, 2, 1000);
	Data.Temp = pData->Temp = (int16_t)(Buf[0] << 8 | Buf[1]) / 340.0f + 36.53f;

	if(ret != COM_OK) { return COM_ERROR; }
	return COM_OK;
}

void Quaternion_IMU_c::Calc(float gx, float gy, float gz, float ax, float ay, float az)
{
	float norm;
	float vx, vy, vz;
	float ex, ey, ez;

	// 测量正常化
	norm = sqrt(ax * ax + ay * ay + az * az);
	ax = ax / norm;                   //单位化
	ay = ay / norm;
	az = az / norm;

	// 估计方向的重力
	vx = 2 * (q1 * q3 - q0 * q2);
	vy = 2 * (q0 * q1 + q2 * q3);
	vz = q0 * q0 - q1 * q1 - q2 * q2 + q3 * q3;

	// 错误的领域和方向传感器测量参考方向之间的交叉乘积的总和
	ex = (ay * vz - az * vy);
	ey = (az * vx - ax * vz);
	ez = (ax * vy - ay * vx);

	// 积分误差比例积分增益
	exInt = exInt + ex * Ki;
	eyInt = eyInt + ey * Ki;
	ezInt = ezInt + ez * Ki;

	// 调整后的陀螺仪测量
	gx = gx + Kp * ex + exInt;
	gy = gy + Kp * ey + eyInt;
	gz = gz + Kp * ez + ezInt;

	// 整合四元数率和正常化
	q0 += (-q1 * gx - q2 * gy - q3 * gz) * halfT;
	q1 += (q0 * gx + q2 * gz - q3 * gy) * halfT;
	q2 += (q0 * gy - q1 * gz + q3 * gx) * halfT;
	q3 += (q0 * gz + q1 * gy - q2 * gx) * halfT;

	// 正常化四元
	norm = sqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
	q0 = q0 / norm;
	q1 = q1 / norm;
	q2 = q2 / norm;
	q3 = q3 / norm;

	Pitch = asin(-2 * q1 * q3 + 2 * q0 * q2) * 57.3f;
	Roll = atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2 * q2 + 1) * 57.3f;
	Yaw = atan2(2 * (q1 * q2 + q0 * q3), q0 * q0 + q1 * q1 - q2 * q2 - q3 * q3) * 57.3f;
//todo: 测试
	Yaw_Kalman.Kalman_Update(Pitch, gx, halfT * 2);
	Pitch_Kalman.Kalman_Update(Pitch, gy, halfT * 2);
	Roll_Kalman.Kalman_Update(Roll, gz, halfT * 2);

	Yaw = Yaw_Kalman.angle;
	Pitch = Pitch_Kalman.angle;
	Roll = Roll_Kalman.angle;
}

void Kalman_IMU_c::Kalman_Data_t::Kalman_Update(float newAngle, float newRate, float dt)
{
	// 预测
	rate = newRate - bias;
	angle += dt * rate;

	P[0][0] += dt * (dt * P[1][1] - P[0][1] - P[1][0] + 1);
	P[0][1] -= dt * P[1][1];
	P[1][0] -= dt * P[1][1];
	P[1][1] += dt;

	// 更新
	float S = P[0][0] + 1;
	float K[2]; // 卡尔曼增益
	K[0] = P[0][0] / S;
	K[1] = P[1][0] / S;

	float y = newAngle - angle; // 角度差

	angle += K[0] * y;
	bias += K[1] * y;

	float P00_temp = P[0][0];
	float P01_temp = P[0][1];

	P[0][0] -= K[0] * P00_temp;
	P[0][1] -= K[0] * P01_temp;
	P[1][0] -= K[1] * P00_temp;
	P[1][1] -= K[1] * P01_temp;
}

void Kalman_IMU_c::Calc(float gx, float gy, float gz, float ax, float ay, float az)
{
	float accel_x = ax;
	float accel_y = ay;
	float accel_z = az;
	float dt = 0.01;

	//
	float accel_angle_x = atan2(accel_y, sqrt(accel_x * accel_x + accel_z * accel_z)) * 180 / PI;
	float accel_angle_y = atan2(accel_x, sqrt(accel_y * accel_y + accel_z * accel_z)) * 180 / PI;
	float accel_angle_z = atan2(accel_y, accel_x) * 180 / PI;

	//
	float Angle_X_Final = 0.98 * (accel_angle_x + gx * dt) + 0.02 * accel_angle_x;
	float Angle_Y_Final = 0.98 * (accel_angle_y + gy * dt) + 0.02 * accel_angle_y;
	float Angle_Z_Final = 0.98 * (accel_angle_z + gz * dt) + 0.02 * accel_angle_z;

	Yaw_Data.Kalman_Update(Angle_X_Final, gx, dt);
	Pitch_Data.Kalman_Update(Angle_Y_Final, gy, dt);
	Roll_Data.Kalman_Update(Angle_Z_Final, gz, dt);

	Yaw = Yaw_Data.angle;
	Pitch = Pitch_Data.angle;
	Roll = Roll_Data.angle;
}
