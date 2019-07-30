#pragma once
#include <Wire.h>

class I2CProtocol
{
private:
	TwoWire* wire;
public:
	I2CProtocol(TwoWire* wire, int sda = -1, int scl = -1, int freq = 0)
	{
		this->wire = wire;
		wire->begin(sda, scl, freq);
	}
	uint8_t Read()
	{
		wire->read();
	}
	void Read(uint8_t address, uint8_t* buffer, uint32_t size, bool stop = true, uint32_t* read = nullptr)
	{
		wire->readTransmission(address, buffer, size, stop, read);
	}
	void ReadBytes(uint8_t* buffer, int n)
	{
		wire->readBytes(buffer, n);
	}
	void RequestFrom(uint8_t address, uint8_t nbytes, bool stop = true)
	{
		wire->requestFrom((uint16_t)address, nbytes, stop);
	}
	void BeginTransmission(uint8_t address)
	{
		wire->beginTransmission(address);
	}
	void Write(uint8_t* buffer, int n)
	{
		wire->write(buffer, n);
	}
	void Write(uint8_t value)
	{
		wire->write(value);
	}
	void Write(uint8_t address, uint8_t* buffer, uint32_t n, bool stop = true)
	{
		wire->writeTransmission(address, buffer, n, stop);
	}
	void EndTransmission(bool stop)
	{
		wire->endTransmission(stop);
	}
};