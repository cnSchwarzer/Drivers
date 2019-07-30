#pragma once
 

class ADF4351
{
public:
    void Init()
    { 

        for(int i = 1; i < 7; ++i)
        {
            GPIOPinTypeGPIOOutput(pinMapTable[i][0], pinMapTable[i][1]);
            digitalWrite(i, 0);
        }
        GPIOPinTypeGPIOInput(pinMapTable[0][0], pinMapTable[0][1]);
        GPIOPadConfigSet(pinMapTable[0][0], pinMapTable[0][1], GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);

        digitalWrite(6, 1);
    }
    void SetFrequency(double rfout)
    {
        double refin = 100;
        int r = 10;
        double pfdfreq = refin / r;
        double spacing = 10;
        int bandselectclock = 80;
        int optdiv = 0;

        if(rfout >= 2200) optdiv = 0;
        else if(rfout >= 1100) optdiv = 1;
        else if(rfout >= 550) optdiv = 2;
        else if(rfout >= 275) optdiv = 3;
        else if(rfout >= 137.5) optdiv = 4;
        else if(rfout >= 68.75) optdiv = 5;
        else optdiv = 6;

        int rfdiv = optdiv;
        optdiv = pow(2, rfdiv);

        double n = rfout / pfdfreq * optdiv;
        int intpart = (int)n;
        int mod = (int)round(1000 * (pfdfreq / spacing));
        int frac = (int)round((n - intpart) * mod);

        int gcdv = gcd(mod, frac);
        mod /= gcdv;
        frac /= gcdv;

        if(mod == 1) mod = 2;

        uint32_t reg5 = 0b00000000010110000000000000000101;
        uint32_t reg4 = 0b00000000100000000000000000111100;
        reg4 |= (rfdiv) << 20;
        reg4 |= ((uint32_t)(bandselectclock) << 12);
        uint32_t reg3 = 0b00000000000000000000010010110011;
        uint32_t reg2 = 0b00000000000000000000111001000010;
        reg2 |= ((uint32_t)(r) << 14);
        uint32_t reg1 = 0b00001000000000001000000000000001;
        reg1 |= (mod << 3);
        uint32_t reg0 = 0b00000000000000000000000000000000;
        reg0 |= (frac << 3);
        reg0 |= (intpart << 15);

        WriteRegister32(reg5);
        WriteRegister32(reg4);
        WriteRegister32(reg3);
        WriteRegister32(reg2);
        WriteRegister32(reg1);
        WriteRegister32(reg0);
    }
private:
    uint32_t gcd(uint32_t a, uint32_t b)
    {
        if(a == 0)return b;
        if(b == 0)return a;
        if(a>b)return gcd(a%b, b);
        else return gcd(a, b%a);
    }
    uint32_t pinMapTable[7]=
    {
         19,    //LD
         18,    //MUX
         5,     //GND
         17,    //CLK
         16,    //DAT
         4,     //LE
         0      //CE
    };
    inline void digitalWrite(unsigned int pin, int high)
    {
        if(high) GPIOPinWrite(pinMapTable[pin][0], pinMapTable[pin][1], pinMapTable[pin][1]);
        else GPIOPinWrite(pinMapTable[pin][0], pinMapTable[pin][1], 0);
    }
    inline int digitalRead(unsigned int pin)
    {
        int32_t val = GPIOPinRead(pinMapTable[pin][0], pinMapTable[pin][1]);
        return val == 0 ? 0 : 1;
    }
    inline void Pulse(unsigned int pin)
    {
        digitalWrite(pin, 0);
        digitalWrite(pin, 1);
    }
    inline void Write(uint8_t value)
    {
        digitalWrite(3, 1);
        for(int i = 0; i < 8; ++i)
        {
            digitalWrite(4, (value << i) & 0x80);
            Pulse(3);
        }
    }
    inline void WriteRegister32(uint32_t value)
    {
        digitalWrite(5, 0);
        Write(value >> 24);
        Write(value >> 16);
        Write(value >> 8);
        Write(value);
        Pulse(5);
    }
};
