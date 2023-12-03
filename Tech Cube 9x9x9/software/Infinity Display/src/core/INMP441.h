#ifndef INMP441_H
#define INMP441_H

namespace I2S_MIC {

void begin(uint32_t frequency);
uint16_t loop(int32_t *buffer, uint16_t samples);

}  // namespace I2S_MIC
#endif