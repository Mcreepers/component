#ifndef __COM_LIB_H__
#define __COM_LIB_H__

#include "component.h"

namespace com_lib
{
	template <typename T> void exchange(T &a, T &b);
	void Set_Bit(uint32_t &reg, uint32_t bit, bool value);
	void Set_Bit(uint32_t &reg, uint32_t value);
	void Clear_Bit(uint32_t &reg, uint8_t bit);
	void Clear_Bit(uint32_t &reg, uint32_t value);
	bool Get_Bit(uint32_t &reg, uint8_t bit);
	template <typename T>
	void Find_Max_Min(T *data, uint32_t len, T &max, T &min);//find the max and min value
	template <typename T, typename T_mem, T_mem(T:: *mem)>
	void Find_Max_Min(T *address, uint8_t len, T &max, T &min);//find the max and min value by the member of the class
	template <typename T>
	void Insertion_Sort(T *data, uint32_t len);//sort the data
	// template <typename T, typename T_mem, T_mem(T:: *mem)>
	// void Insertion_Sort(T *address, uint8_t len);//sort the data by the member of the class
}

#endif