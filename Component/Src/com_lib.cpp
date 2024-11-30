#include "com_lib.h"

namespace com_lib
{
	template <typename T> void exchange(T &a, T &b)
	{
		T temp = a;
		a = b;
		b = temp;
	}
	void Set_Bit(uint32_t &reg, uint32_t bit, bool value)
	{
		if(value)
		{
			reg |= (1 << bit);
		}
		else
		{
			reg &= ~(1 << bit);
		}
	}
	void Set_Bit(uint32_t &reg, uint32_t value)
	{
		reg |= value;
	}
	void Clear_Bit(uint32_t &reg, uint8_t bit)
	{
		reg &= ~(1 << bit);
	}
	void Clear_Bit(uint32_t &reg, uint32_t value)
	{
		reg &= ~value;
	}
	bool Get_Bit(uint32_t &reg, uint8_t bit)
	{
		return (reg & (1 << bit));
	}

	template <typename T>
	void Find_Max_Min(T *data, uint32_t len, T &max, T &min)
	{
		max = data[0];
		min = data[0];
		for(uint32_t i = 0; i < len; i++)
		{
			if(data[i] > max)
			{
				max = data[i];
			}
			else if(data[i] < min)
			{
				min = data[i];
			}
		}
	}

	template <typename T, typename T_mem, T_mem(T:: *mem)>
	void Find_Max_Min(T *address, uint8_t len, T &max, T &min)
	{
		max = address[0];
		min = address[0];
		for(uint8_t i = 0; i < len; i++)
		{
			if(address[i].*mem > max.*mem)
			{
				max = address[i];
			}
			else if(address[i].*mem < min.*mem)
			{
				min = address[i];
			}
		}
	}

	template <typename T>
	void Insertion_Sort(T *data, uint32_t len)
	{
		T temp;
		for(uint32_t i = 1; i < len; i++)
		{
			temp = data[i];
			int64_t j = i - 1;
			while(j >= 0 && data[j] > temp)
			{
				data[j + 1] = data[j];
				j--;
			}
			data[j + 1] = temp;
		}
	}

	// template <typename T, typename T_mem, T_mem(T:: *mem)>
	// void Insertion_Sort(T *address, uint8_t len)
	// {
	// 	T temp;
	// 	for(uint8_t i = 1; i < len; i++)
	// 	{
	// 		temp = address[i];
	// 		int16_t j = i - 1;
	// 		while(j >= 0 && address[j].*mem > temp.*mem)
	// 		{
	// 			address[j + 1] = address[j];
	// 			j--;
	// 		}
	// 		address[j + 1] = temp;
	// 	}
	// }
}
