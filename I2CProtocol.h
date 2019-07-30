#pragma once
#include <Wire.h>

#define I2C_DEBUG 0

class I2CProtocol
{
private:
	TwoWire* wire;
public:
	I2CProtocol(TwoWire* wire)
	{
		this->wire = wire; 
	}
	uint8_t Read()
	{
		return wire->read();
	}
	void Read(uint8_t address, uint8_t* buffer, uint32_t size, bool stop = true, uint32_t* read = nullptr)
	{
		wire->readTransmission(address, buffer, size, stop, read);
	}
	void ReadBytes(uint8_t* buffer, int n)
	{
#if I2C_DEBUG
		int val = 
#endif

		wire->readBytes(buffer, n);

#if I2C_DEBUG
		Serial.printf("Bytes Read: %d\n", val);
#endif 
	}
	void RequestFrom(uint8_t address, uint8_t nbytes, bool stop = true)
	{ 
		wire->requestFrom((uint16_t)address, nbytes, stop);

#if I2C_DEBUG
		char* buf = wire->getErrorText(wire->lastError());
		Serial.printf("RequestFrom: %s\n", buf);
#endif 
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