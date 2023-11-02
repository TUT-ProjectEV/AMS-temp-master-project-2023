#include <Arduino.h>
#include <MsTimer2.h>
#include <Wire.h>
#include "Thermistor.hpp"
#include "CAN_Temp.hpp"

#define DANGER_OUTPUT (3)

const unsigned long CAL_INTERVAL = 5000;

volatile uint8_t count = 0;
uint8_t data[8] = {0, 0, 0, 0, 0, 0, 0, 0};
Thermistor *thm = new Thermistor();
volatile uint8_t countFlag = 0;
volatile unsigned long nowTime, lastCalTime;
volatile unsigned char pastErrFlag[3] = {0, 0, 0};
volatile unsigned char dangerFlag = 0;
CAN_Temp *ACC_Temp = new CAN_Temp(ACC_ID);
volatile unsigned char printTimer = 0;
volatile unsigned char serialFlag = 0;

void handler(void);

void _init_(unsigned long time);

void runCalibration(void);

void setup()
{
    pinMode(DANGER_OUTPUT, OUTPUT);

    Wire.begin();

    Serial.begin(115200);

    Serial.println("Time,MAX,MIN,AVR");

    MsTimer2::set(100, handler);
    MsTimer2::start();

    ACC_Temp->init();
}

void loop()
{
    if (!countFlag)
    {
        serialFlag = 0;

        if (count < ecuNum)
        {
            // データリクエスト
            /*
            Serial.print("count : ");
            Serial.println(count);
            */

            Wire.requestFrom(adrs[count], sizeof(data));

            uint8_t i = 0;
            while (Wire.available())
            {
                data[i] = Wire.read();
                i++;
            }

            thm->setData(count, data, sizeof(data));

            /*
            Serial.print("raw data : ");
            Serial.println(count);
            for (char j = 0; j < thmNum; j++)
            {
                Serial.println(thm->getVal(count, j));
            }

            Serial.println("temperature");
            for (char j = 0; j < thmNum; j++)
            {
                Serial.println(thm->getTemp(count, j));
            }

            Serial.println();
            */
        }
        else
        {
            // CANバスにメッセージを流す
            ACC_Temp->sendTempMsg(1);
        }

        countFlag = 1;
        serialFlag = 1;
    }

    pastErrFlag[2] = pastErrFlag[1];
    pastErrFlag[1] = pastErrFlag[0];

    float avr = 0;
    for (int i = 0; i < ecuNum; i++)
    {
        avr += thm->getAvrTemp(i);
        ACC_Temp->setTemp(Type::MAX_TEMP, thm->getMaxTemp(i) > ACC_Temp->getTemp(Type::MAX_TEMP) ? thm->getMaxTemp(i) : ACC_Temp->getTemp(Type::MAX_TEMP));
        ACC_Temp->setTemp(Type::MIN_TEMP, thm->getMinTemp(i) < ACC_Temp->getTemp(Type::MIN_TEMP) ? thm->getMinTemp(i) : ACC_Temp->getTemp(Type::MIN_TEMP));

        if (thm->getMaxTemp(i) >= maxAllowableTemp)
        {
            pastErrFlag[0] = 1;
            break;
        }
        else if (thm->getMinTemp(i) <= minAllowableTemp)
        {
            pastErrFlag[0] = 1;
            break;
        }
        else
        {
            pastErrFlag[0] = 0;
        }
    }
    avr = avr / ((float)ecuNum);
    ACC_Temp->setTemp(Type::AVR_TEMP, avr);

    // if (!dangerFlag)
    //{
    dangerFlag = pastErrFlag[2] & pastErrFlag[1] & pastErrFlag[0];
    //}

    digitalWrite(DANGER_OUTPUT, !dangerFlag);

    nowTime = millis();

    if ((nowTime - lastCalTime) > CAL_INTERVAL)
    {
        runCalibration();
        lastCalTime = nowTime;
    }

    if (nowTime < lastCalTime)
    {
        lastCalTime = nowTime;
    }
}

void handler(void)
{
    if (countFlag)
    {
        if (count < ecuNum)
        {
            count++;
        }
        else
        {
            count = 0;
        }
        countFlag = 0;
    }
    // Serial.println(count);

    printTimer++;
    if (printTimer >= 10)
    {
        if (serialFlag)
        {
            Serial.print(millis());
            Serial.print(",");
            Serial.print(ACC_Temp->getTemp(Type::MAX_TEMP));
            Serial.print(",");
            Serial.print(ACC_Temp->getTemp(Type::MIN_TEMP));
            Serial.print(",");
            Serial.print(ACC_Temp->getTemp(Type::AVR_TEMP));
            Serial.println();
            Serial.print(thm->getVal(0, 0));
            Serial.print(",");
            Serial.print(thm->getVal(0, 1));
            Serial.print(",");
            Serial.print(thm->getVal(0, 2));
            Serial.println();
            Serial.print(thm->getTemp(0, 0));
            Serial.print(",");
            Serial.print(thm->getTemp(0, 1));
            Serial.print(",");
            Serial.print(thm->getTemp(0, 2));
            Serial.println();

            printTimer = 0;
        }
    }
}

void _init_(unsigned long time)
{

}

void runCalibration(void)
{
}