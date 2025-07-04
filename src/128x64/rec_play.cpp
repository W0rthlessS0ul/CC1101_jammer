#include "rec_play.h"

void record(float frequency, int sampling)
{
    force_stop = false;
    operationTotal = RECORDINGBUFFERSIZE;
    operationCurrent = 0;
    operationProgress = 0;
    currentOperation = 1;

    ELECHOUSE_cc1101.setCCMode(0);
    ELECHOUSE_cc1101.setPktFormat(3);
    ELECHOUSE_cc1101.SetRx();
    ELECHOUSE_cc1101.setMHZ(frequency);

    pinMode(gdo0, INPUT);

    if (force_stop)
    {
        ELECHOUSE_cc1101.SetTx();
        currentOperation = 0;
        return;
    }

    recordedLength = 0;

    for (int i = 0; i < RECORDINGBUFFERSIZE && !force_stop; i++)
    {
        byte receivedbyte = 0;

        buttOK.tick();
        if (buttOK.isSingle())
        {
            force_stop = true;
            break;
        }

        for (int j = 7; j > -1 && !force_stop; j--)
        {
            bitWrite(receivedbyte, j, digitalRead(gdo0));
            delayMicroseconds(sampling);
        };

        if (force_stop)
            break;

        bigrecordingbuffer[i] = receivedbyte;
        recordedLength++;
        operationCurrent = i;
        operationProgress = (i * 100) / RECORDINGBUFFERSIZE;

        if (i % 32 == 0)
        {
            display.clearDisplay();
            display.setCursor(0, 0);
            display.println(F("Recording RAW..."));
            display.setCursor(0, 10);
            display.print("Progress: " + String(operationProgress) + "%");
            display.setCursor(0, 20);
            display.print("Length: " + String(operationCurrent) + " bytes");
            display.display();

            if (access_point == 0)
                server.handleClient();
        }
    }

    ELECHOUSE_cc1101.setCCMode(2);
    ELECHOUSE_cc1101.setPktFormat(0);
    ELECHOUSE_cc1101.SetTx();

    if (!force_stop)
    {
        operationCurrent = operationTotal;
        operationProgress = 100;
    }
    currentOperation = 0;
}

void play(float frequency, int sampling)
{
    if (recordedLength == 0)
    {
        if (access_point == 0)
        {
            server.send(200, "text/plain", "No data to play!");
        }
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println(F("No data to play!"));
        display.display();
        delay(2000);
        return;
    }

    force_stop = false;
    operationTotal = recordedLength;
    operationCurrent = 0;
    operationProgress = 0;
    currentOperation = 2;

    ELECHOUSE_cc1101.setCCMode(0);
    ELECHOUSE_cc1101.setPktFormat(3);
    ELECHOUSE_cc1101.SetTx();
    ELECHOUSE_cc1101.setMHZ(frequency);

    pinMode(gdo0, OUTPUT);

    for (int i = 1; i < recordedLength && !force_stop; i++)
    {
        byte receivedbyte = bigrecordingbuffer[i];

        buttOK.tick();
        if (buttOK.isSingle())
        {
            force_stop = true;
            break;
        }

        for (int j = 7; j > -1 && !force_stop; j--)
        {
            digitalWrite(gdo0, bitRead(receivedbyte, j));
            delayMicroseconds(sampling);
        };

        if (force_stop)
            break;

        operationCurrent = i;
        operationProgress = (i * 100) / recordedLength;

        if (i % 32 == 0)
        {
            display.clearDisplay();
            display.setCursor(0, 0);
            display.println(F("Replaying RAW..."));
            display.setCursor(0, 10);
            display.print("Progress: " + String(operationProgress) + "%");
            display.setCursor(0, 20);
            display.print("Length: " + String(operationCurrent) + " bytes");
            display.display();

            if (access_point == 0)
                server.handleClient();
        }
    }

    ELECHOUSE_cc1101.setCCMode(2);
    ELECHOUSE_cc1101.setPktFormat(0);
    ELECHOUSE_cc1101.SetTx();
    pinMode(gdo0, INPUT);

    if (!force_stop)
    {
        operationCurrent = operationTotal;
        operationProgress = 100;
    }
    currentOperation = 0;
}