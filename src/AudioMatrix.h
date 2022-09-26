#include "Arduino.h"

// Pin assignments on AudioMatrix Board
#define AX0 15
#define AX1 21
#define AX2 19
#define AX3 2
#define AY0 18
#define AY1 5
#define AY2 0
#define DATA 4
#define CS 16
#define RESET 17
#define STROBE 23
#define SCL 26
#define SDA 27

/* channel adresses */
#define  CHANNEL1_VOLUME_STEP_01  0x90
#define  CHANNEL1_VOLUME_STEP_10  0x80
#define  CHANNEL2_VOLUME_STEP_01  0x50
#define  CHANNEL2_VOLUME_STEP_10  0x40
#define  CHANNEL3_VOLUME_STEP_01  0x10
#define  CHANNEL3_VOLUME_STEP_10  0x00
#define  CHANNEL4_VOLUME_STEP_01  0x30
#define  CHANNEL4_VOLUME_STEP_10  0x20
#define  CHANNEL5_VOLUME_STEP_01  0x70
#define  CHANNEL5_VOLUME_STEP_10  0x60
#define  CHANNEL6_VOLUME_STEP_01  0xb0
#define  CHANNEL6_VOLUME_STEP_10  0xa0
#define  MASTER_VOLUME_1STEP     0xe0
#define  MASTER_VOLUME_10STEP    0xd0
//#define  MUTE                    0x08
#define  MUTE                    0xF8
#define  SYSTEM_RESET            0xc0 

class AudioMatrix {
public:
    void begin();
    void connect(int8_t input, int8_t output);
    void disconnect(int8_t input, int8_t output);
    void setVolume(int8_t channel, int8_t volume);
    void setMasterVolume(int8_t chip, unsigned char mvol);
private:
    void SetAX(int8_t number);
    void SetAXPins(bool ax0, bool ax1, bool ax2, bool ax3);
    void SetAY(int8_t number);
    void SetAYPins(bool ay0, bool ay1, bool ay2);
    void Latch();
    unsigned char HEX2BCD (unsigned char x);
    int writeI2CChar(int8_t chip, unsigned char c);
    void setMute(int8_t chip, char);
    void setChannelVolume(int8_t chip, unsigned char chvol, unsigned char chno);
};
