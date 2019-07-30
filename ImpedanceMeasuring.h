#pragma once

#include "AD5933.h" 

class Calibrator
{
	double calRes = 0;
	double amplitude[2] = { 0 };
	double phase[2] = { 0 };
public:
	Calibrator(double calibrationResistor) : calRes(calibrationResistor)
	{

	}
	void SetResistor(double calibrationResistor)
	{
		calRes = calibrationResistor;
	}
	void SetPoint(int idx, int16_t real, int16_t imaginary)
	{
		amplitude[idx] = 1.0 / calRes / sqrt(pow(real, 2) + pow(imaginary, 2));
		phase[idx] = RAD_TO_DEG * atan2(imaginary, real);
	}
	double GetGainFactor(double percent)
	{
		return amplitude[0] + (amplitude[1] - amplitude[0]) * percent;
	}
	double GetSystemPhase(double percent)
	{
		return phase[0] + (phase[1] - phase[0]) * percent;
	}
	double CalculateAmplitude(int16_t* readings, double percent)
	{
		return 1.0 / (sqrt(pow(readings[0], 2) + pow(readings[1], 2)) * GetGainFactor(percent));
	}
	double CalculatePhase(int16_t* readings, double percent)
	{
		return (RAD_TO_DEG * atan2(readings[1], readings[0]) - GetSystemPhase(percent));
	}
};

class ImpedanceMeasuring
{
	Calibrator* calibrator;
public: 
	AD5933* device;
	ImpedanceMeasuring(AD5933* device) : device(device)
	{
		calibrator = new Calibrator(47000);
	}
	void Calibration(double calibrationResistor, double start, double step, int count)
	{
		calibrator->SetResistor(calibrationResistor);

		//Initialize the buffer for reading.
		int16_t readings[2] = { 0x0 };

		//Firstly, we can calculate the two point value for correction.
		device->SetStartFrequency(start);
		device->SetFrequencyIncrement(step * count);
		device->SetIncrementCount(2);
		device->SetWorkMode(AD5933::WorkModes::StandBy);
		delayMicroseconds(10000);
		device->SetWorkMode(AD5933::WorkModes::InitializeWithStartFrequency);
		delayMicroseconds(10000);
		device->SetWorkMode(AD5933::WorkModes::StartFrequencySweep);
		device->SetSettingCycles(511, AD5933::SettingCycleTimes::SettingCycle_X4);

		for (int i = 0; i < 2; ++i)
		{
			while (true)
			{
				uint8_t status = device->GetStatus();
				if (status & AD5933::StatusCode::RealAndImaginaryValid)
					break;
			}
			device->GetReadAndImaginary(readings);
			device->SetWorkMode(AD5933::WorkModes::IncrementFrequency);
			calibrator->SetPoint(i, readings[0], readings[1]);
		}
		device->SetWorkMode(AD5933::WorkModes::PowerSave);
	}
	void Measure(double start, double step, int count)
	{
		//Initialize the buffer for reading.
		int16_t readings[2] = { 0x0 };

		//Setup the sweep and ready to go.
		device->SetStartFrequency(start);
		device->SetFrequencyIncrement(step);
		device->SetIncrementCount(count);
		device->SetWorkMode(AD5933::WorkModes::StandBy);
		delayMicroseconds(10000);
		device->SetWorkMode(AD5933::WorkModes::InitializeWithStartFrequency);
		delayMicroseconds(10000);
		device->SetWorkMode(AD5933::WorkModes::StartFrequencySweep);
		device->SetSettingCycles(20, AD5933::SettingCycleTimes::SettingCycle_X1); 

		for (int i = 0; i < count; ++i)
		{
			while (true)
			{
				uint8_t status = device->GetStatus();
				if (status & AD5933::StatusCode::RealAndImaginaryValid)
					break;
			}
			device->GetReadAndImaginary(readings);
			device->SetWorkMode(AD5933::WorkModes::IncrementFrequency);
			double amplitude = calibrator->CalculateAmplitude(readings, (i * 1.0) / (count - 1));
			double phase = calibrator->CalculatePhase(readings, (i * 1.0) / (count - 1));
			Serial.printf("Amplitude: %.3f \t Phase: %.3f \t %d %d %d\n ", amplitude, phase, readings[0], readings[1], i);
		}

		bool ended = (device->GetStatus() & AD5933::StatusCode::FrequencySweepComplete);
		device->SetWorkMode(AD5933::WorkModes::PowerSave);
	}
};