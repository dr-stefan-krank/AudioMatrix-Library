#include "AudioMatrix.h"
#include "Arduino.h"
#include <Wire.h>

const unsigned char channell_address01[6] =
    {CHANNEL1_VOLUME_STEP_01, CHANNEL2_VOLUME_STEP_01, CHANNEL3_VOLUME_STEP_01,
     CHANNEL4_VOLUME_STEP_01, CHANNEL5_VOLUME_STEP_01, CHANNEL6_VOLUME_STEP_01};

const unsigned char channell_address10[6] =
    {CHANNEL1_VOLUME_STEP_10, CHANNEL2_VOLUME_STEP_10, CHANNEL3_VOLUME_STEP_10,
     CHANNEL4_VOLUME_STEP_10, CHANNEL5_VOLUME_STEP_10, CHANNEL6_VOLUME_STEP_10};

const unsigned char PT_CHIP[] = {0x84, 0x80, 0x88};

/*!
 * @brief  AudioMatrix Initialize the MT8816AE
*/
void AudioMatrix::begin()
{
    delay(500);
    // Initialize MT8816AE

    // Set all pins to output
    pinMode(AX0, OUTPUT);
    pinMode(AX1, OUTPUT);
    pinMode(AX2, OUTPUT);
    pinMode(AX3, OUTPUT);
    pinMode(AY0, OUTPUT);
    pinMode(AY1, OUTPUT);
    pinMode(AY2, OUTPUT);
    pinMode(DATA, OUTPUT);
    pinMode(CS, OUTPUT);
    pinMode(STROBE, OUTPUT);
    pinMode(RESET, OUTPUT);

    // Enable the MT8816
    digitalWrite(CS, HIGH);

    // Reset the MT8816. All outputs off.
    digitalWrite(RESET, HIGH);
    delay(10);
    digitalWrite(RESET, LOW);

    // Initialize PT2258
    unsigned char masterVolumeValue = 0x00; //master volume 0dB
    unsigned char channelVolume = 0x00;     // channel volume 0dB

    // initialize devices
    Wire.begin(SDA, SCL);

    for (int i = 0; i < 2; i++)
    {
        writeI2CChar(PT_CHIP[i], SYSTEM_RESET);

        // set channell volumes to zero
        for (int chno = 0; chno < 6; chno++)
        {
            Wire.beginTransmission(PT_CHIP[i] >> 1); // transmit to device
            Wire.write(channell_address01[chno] | (HEX2BCD(channelVolume) & 0x0f));
            Wire.write(channell_address10[chno] | ((HEX2BCD(channelVolume) & 0xf0) >> 4));
            Wire.endTransmission(); // stop transmitting
        }

        // set the master voume
        Wire.beginTransmission(PT_CHIP[i] >> 1); // transmit to device
        Wire.write(MASTER_VOLUME_1STEP | (HEX2BCD(masterVolumeValue) & 0x0f));
        Wire.write(MASTER_VOLUME_10STEP | ((HEX2BCD(masterVolumeValue) & 0xf0) >> 4));
        Wire.endTransmission(); // stop transmitting

        // set mute off
        writeI2CChar(PT_CHIP[i], MUTE | 0x00);
    }
}

// set the ESP32 outputs for the matrix output configuration
void AudioMatrix::SetAXPins(bool ax0, bool ax1, bool ax2, bool ax3)
{
    digitalWrite(AX0, ax0);
    digitalWrite(AX1, ax1);
    digitalWrite(AX2, ax2);
    digitalWrite(AX3, ax3);
}

// set the ESP32 outputs for the matrix input configuration
void AudioMatrix::SetAYPins(bool ay0, bool ay1, bool ay2)
{
    digitalWrite(AY0, ay0);
    digitalWrite(AY1, ay1);
    digitalWrite(AY2, ay2);
}

// Calculate and set the truth table for integer to pin assignment for the audio outputs
void AudioMatrix::SetAX(int8_t number)
{
    switch (number)
    {
    case 11: // X0 to output 11
        SetAXPins(LOW, LOW, LOW, LOW);
        break;

    case 12: // X1 to output 12
        SetAXPins(HIGH, LOW, LOW, LOW);
        break;

    case 13: // X2 to output 13
        SetAXPins(LOW, HIGH, LOW, LOW);
        break;

    case 14: // X3 to output 14
        SetAXPins(HIGH, HIGH, LOW, LOW);
        break;

    case 15: // X4 to output 15
        SetAXPins(LOW, LOW, HIGH, LOW);
        break;

    case 16: // X5 to output 16
        SetAXPins(HIGH, LOW, HIGH, LOW);
        break;

    case 1: // X12 to output 1
        SetAXPins(LOW, HIGH, HIGH, LOW);
        break;

    case 2: // X13 to output 2
        SetAXPins(HIGH, HIGH, HIGH, LOW);
        break;

    case 5: // X6 to output 5
        SetAXPins(LOW, LOW, LOW, HIGH);
        break;

    case 6: // X7 to output 6
        SetAXPins(HIGH, LOW, LOW, HIGH);
        break;

    case 7: // X8 to output 7
        SetAXPins(LOW, HIGH, LOW, HIGH);
        break;

    case 8: // X9 to output 8
        SetAXPins(HIGH, HIGH, LOW, HIGH);
        break;

    case 9: // X10 to output 9
        SetAXPins(LOW, LOW, HIGH, HIGH);
        break;

    case 10: // X11 to output 10
        SetAXPins(HIGH, LOW, HIGH, HIGH);
        break;

    case 3: // X14 to output 3
        SetAXPins(LOW, HIGH, HIGH, HIGH);
        break;

    case 4: // X15 to output 4
        SetAXPins(HIGH, HIGH, HIGH, HIGH);
        break;
    }
};

// Calculate and set the truth table for integer to pin assignment for the audio inputs
void AudioMatrix::SetAY(int8_t number)
{
    switch (number)
    {
    case 1: // Audio in 1 to Y4
        SetAYPins(LOW, LOW, HIGH);
        break;
    case 2: // Audio in 2 to Y5
        SetAYPins(HIGH, LOW, HIGH);
        break;
    case 3: // Audio in 3 to Y6 
        SetAYPins(LOW, HIGH, HIGH);
        break;
    case 4: // Audio in 4 to Y7
        SetAYPins(HIGH, HIGH, HIGH);
        break;
    case 5: // Audio in 5 to Y0
        SetAYPins(LOW, LOW, LOW);
        break;
    case 6: // Audio in 6 to Y1
        SetAYPins(HIGH, LOW, LOW);
        break;
    case 7: // Audio in 7 to Y2
        SetAYPins(LOW, HIGH, LOW);
        break;
    case 8: // Audio in 8 to Y3
        SetAYPins(HIGH, HIGH, LOW);
        break;
    };
};

// Latch set pins to the matrix to make it active
void AudioMatrix::Latch()
{
    digitalWrite(STROBE, HIGH);
    delay(20);
    digitalWrite(STROBE, LOW);
};

/*!
 * @brief  AudioMatrix Connect input to output
 * @param  *input
 *         *output
 */
void AudioMatrix::connect(int8_t input, int8_t output)
{
    SetAY(input);
    SetAX(output);
    digitalWrite(DATA, HIGH);
    Latch();
};

/*!
 * @brief  AudioMatrix Disconnect input from output
 * @param  *input
 *         *output
 */
void AudioMatrix::disconnect(int8_t input, int8_t output)
{
    SetAY(input);
    SetAX(output);
    digitalWrite(DATA, LOW);
    Latch();
};

void AudioMatrix::setVolume(int8_t channel, int8_t volume)
{
    if(volume > 79 ){
        volume = 79;
    }
    switch (channel)
    {
    case 1:
        setChannelVolume(PT_CHIP[0], volume, 3);
        break;
    case 2:
        setChannelVolume(PT_CHIP[0], volume, 2);
        break;
    case 3:
        setChannelVolume(PT_CHIP[0], volume, 1);
        break;
    case 4:
        setChannelVolume(PT_CHIP[0], volume, 0);
        break;
    case 5:
        setChannelVolume(PT_CHIP[1], volume, 5);
        break;
    case 6:
        setChannelVolume(PT_CHIP[1], volume, 4);
        break;
    case 7:
        setChannelVolume(PT_CHIP[1], volume, 3);
        break;
    case 8:
        setChannelVolume(PT_CHIP[1], volume, 2);
        break;
    case 9:
        setChannelVolume(PT_CHIP[1], volume, 1);
        break;
    case 10:
        setChannelVolume(PT_CHIP[1], volume, 0);
        break;
    case 11:
        setChannelVolume(PT_CHIP[2], volume, 5);
        break;
    case 12:
        setChannelVolume(PT_CHIP[2], volume, 4);
        break;
    case 13:
        setChannelVolume(PT_CHIP[2], volume, 3);
        break;
    case 14:
        setChannelVolume(PT_CHIP[2], volume, 2);
        break;
    case 15:
        setChannelVolume(PT_CHIP[2], volume, 1);
        break;
    case 16:
        setChannelVolume(PT_CHIP[2], volume, 0);
        break;
    }
}

// helper method
unsigned char AudioMatrix::HEX2BCD(unsigned char x)
{
    unsigned char y;
    y = (x / 10) << 4;
    y = y | (x % 10);
    return (y);
}

// helper method
int AudioMatrix::writeI2CChar(int8_t chip, unsigned char c)
{
    //  shift address to right - Wire library always uses 7 bit addressing
    Wire.beginTransmission(chip >> 1); // transmit to selected device
    Wire.write(c);
    int rtnCode = Wire.endTransmission(); // stop transmitting
    return rtnCode;
}

// Set mute: 1 -> mute on, 0 -> mute off
void AudioMatrix::setMute(int8_t chip, char in_mute)
{
    writeI2CChar(chip, MUTE | in_mute);
}

// Set channel volume, attenuation range : 0 to 79dB
void AudioMatrix::setChannelVolume(int8_t chip, unsigned char chvol, unsigned char chno)
{
    Wire.beginTransmission(chip >> 1); // transmit to selected device
    Wire.write(channell_address01[chno] | (HEX2BCD(chvol) & 0x0f));
    Wire.write(channell_address10[chno] | ((HEX2BCD(chvol) & 0xf0) >> 4));
    Wire.endTransmission(); // stop transmitting
}

// Set master volume, attenuation range : 0 to 79dB
void AudioMatrix::setMasterVolume(int8_t chip, unsigned char mvol)
{
    Wire.beginTransmission(chip >> 1); // transmit to selected device
    Wire.write(MASTER_VOLUME_1STEP | (HEX2BCD(mvol) & 0x0f));
    Wire.write(MASTER_VOLUME_10STEP | ((HEX2BCD(mvol) & 0xf0) >> 4));
    Wire.endTransmission(); // stop transmitting
}