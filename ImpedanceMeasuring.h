#pragma once

#include "AD5933.h" 
 
class ImpedanceMeasuring
{
	AD5933* device;
public:
	ImpedanceMeasuring(AD5933* device) : device(device)
	{

	}
	void Measure(double start, double step, int count)
	{
		device->SetStartFrequency(start);
		device->SetFrequencyIncrement(step);
		device->SetIncrementCount(count);
		device->Reset();

		device->SetWorkMode(AD5933::WorkModes::InitializeWithStartFrequency);
		delayMicroseconds(10000);
		device->SetWorkMode(AD5933::WorkModes::StartFrequencySweep);

		uint16_t real = 0;
		uint16_t imaginary = 0;
		for (int i = 0; i < count; ++i)
		{
			while (!(device->GetStatus() & AD5933::StatusCode::RealAndImaginaryValid))
			{
				device->GetReadAndImaginary(real, imaginary);
			}
			device->SetWorkMode(AD5933::WorkModes::IncrementFrequency);
		}
		volatile bool ended = (device->GetStatus() & AD5933::StatusCode::FrequencySweepComplete);
		device->SetWorkMode(AD5933::WorkModes::PowerSave);
	}
};