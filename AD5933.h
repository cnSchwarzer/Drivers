#pragma once

#include "I2CProtocol.h"

class AD5933
{
public:
	enum WorkModes
	{
		InitializeWithStartFrequency = 0b0001,
		StartFrequencySweep = 0b0010,
		IncrementFrequency = 0b0011,
		RepeatFrequency = 0b0100,
		MeasureTemperature = 0b1001,
		PowerSave = 0b1010,
		StandBy = 0b1011
	};
	enum OutputVoltage
	{
		Vpp2_0 = 0b00,
		Vpp1_0 = 0b11,
		Vpp0_4 = 0b10,
		Vpp0_2 = 0b01
	};
	enum PGAGain
	{
		PGAGain_X5 = 0,
		PGAGain_X1 = 1
	};
	enum ClockSource
	{
		Internal = 0,
		External = 1
	};
	enum SettingCycleTimes
	{
		SettingCycle_X1 = 0b00,
		SettingCycle_X2 = 0b01,
		SettingCycle_X4 = 0b11
	};
	enum StatusCode
	{
		TemperatureValid = 0x1,
		RealAndImaginaryValid = 0x2,
		FrequencySweepComplete = 0x4
	};

private:
	I2CProtocol* i2c;
	int mainClock = 16000000;
	uint16_t controlRegister = 0xA000;

	const uint8_t address = 0xD;
	const uint8_t cmdBlockRead = 0b10100000;
	const uint8_t cmdBlockWrite = 0b10100001;
	const uint8_t cmdSetPointer = 0b10110000;
	
public:
	AD5933(I2CProtocol* i2c, int mainClock = 16000000) : mainClock(mainClock), i2c(i2c)
	{ 
	}

	void SetPointerAddress(uint8_t ptr)
	{
		i2c->BeginTransmission(address);
		i2c->Write(cmdSetPointer);
		i2c->Write(ptr);
		i2c->EndTransmission(true);
	}
	void BlockRead(uint8_t* buffer, uint8_t length)
	{
		i2c->BeginTransmission(address);
		i2c->Write(cmdBlockRead);
		i2c->Write(length);
		i2c->EndTransmission(false);
		i2c->RequestFrom(address, length);
		for (int i = 0; i < length; ++i)
		{
			buffer[i] = i2c->Read();
		}
	}
	void BlockWrite(uint8_t* buffer, uint8_t length)
	{
		i2c->BeginTransmission(address);
		i2c->Write(cmdBlockWrite);
		i2c->Write(length);
		for (int i = 0; i < length; ++i)
		{
			i2c->Write(buffer[i]);
		}
		i2c->EndTransmission(true);
	}

	uint8_t ReadRegister(uint8_t ptr)
	{
		SetPointerAddress(ptr);
		i2c->RequestFrom(address, 1);
		return i2c->Read();
	}
	void WriteRegister(uint8_t ptr, uint8_t value)
	{
		i2c->BeginTransmission(address);
		i2c->Write(ptr);
		i2c->Write(value);
		i2c->EndTransmission(true);
	}

	void SetStartFrequency(double value)
	{
		value = value * 536870912.0 / mainClock;
		uint32_t code = value;
		code &= 0xFFFFFF;

		SetPointerAddress(0x82);
		uint8_t data[] = { code >> 16, code >> 8, code };
		BlockWrite(data, 3);
	}
	void SetFrequencyIncrement(double value)
	{
		value = value * 536870912.0 / mainClock;
		uint32_t code = value;
		code &= 0xFFFFFF;

		SetPointerAddress(0x85);
		uint8_t data[] = { code >> 16, code >> 8, code };
		BlockWrite(data, 3);
	}
	void SetIncrementCount(int count)
	{
		if (count > 511) count = 511;
		count &= 0b111111111;

		SetPointerAddress(0x88);
		uint8_t data[] = { count >> 8, count };
		BlockWrite(data, 2);
	}
	void SetSettingCycles(uint16_t cycles, SettingCycleTimes times)
	{
		uint16_t code = cycles;
		code |= (times << 9);
		SetPointerAddress(0x8A);
		uint8_t data[] = { code >> 8, code };
		BlockWrite(data, 2);
	}

	void SetWorkMode(WorkModes mode)
	{
		uint8_t m = mode;
		controlRegister &= 0b0000111111111111;
		controlRegister |= m << 12;
		WriteRegister(0x80, controlRegister >> 8);
	}
	void SetOutputVoltage(OutputVoltage vol)
	{
		uint8_t v = vol;
		controlRegister &= 0b1111100111111111;
		controlRegister |= v << 9;
		WriteRegister(0x80, controlRegister >> 8);
	}
	void SetPGAGain(PGAGain gain)
	{
		uint8_t g = gain;
		controlRegister &= 0b1111111011111111;
		controlRegister |= g << 8;
		WriteRegister(0x80, controlRegister >> 8);
	}
	void SetClockSource(ClockSource clock)
	{
		uint8_t c = clock;
		controlRegister &= 0b1111111111110111;
		controlRegister |= c << 3;
		WriteRegister(0x81, controlRegister);
	}

	uint8_t GetStatus()
	{
		return ReadRegister(0x8F);
	}
	uint16_t GetReal()
	{
		uint8_t buf[2] = { 0 };
		SetPointerAddress(0x94);
		BlockRead(buf, 2);
		uint16_t value = buf[1];
		value |= (buf[0] << 8);
		return value;
	}
	uint16_t GetImaginary()
	{
		uint8_t buf[2] = { 0 };
		SetPointerAddress(0x96);
		BlockRead(buf, 2);
		uint16_t value = buf[1];
		value |= (buf[0] << 8);
		return value;
	}
	uint16_t GetTemperature()
	{
		uint8_t buf[2] = { 0 };
		SetPointerAddress(0x92);
		BlockRead(buf, 2);
		uint16_t value = buf[1];
		value |= (buf[0] << 8);
		return value;
	}
	void GetReadAndImaginary(uint16_t& real, uint16_t& imaginary)
	{
		uint8_t buf[4] = { 0 };
		SetPointerAddress(0x94);
		BlockRead(buf, 4);
		real = buf[1];
		real |= (buf[0] << 8);
		imaginary = buf[3];
		imaginary |= (buf[2] << 8);
	}

	void Reset()
	{  
		uint8_t val = (controlRegister | 0b10000);
		WriteRegister(0x81, val);
	}
};
