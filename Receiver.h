#ifndef RECEIVER_H
#define RECEIVER_H


#include <stdint.h>
#include "ReceiverSpiDriver.h"


class Receiver {
    public:
        void init(
            uint8_t spiClockPin,
            uint8_t spiDataPin,
            uint8_t spiSelectPin
        );

        void setFrequency(uint16_t frequency);

    private:
        ReceiverSpiDriver driver;
};


#endif
