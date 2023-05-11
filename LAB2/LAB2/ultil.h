#ifndef ULTIL_H
#define ULTIL_H

void FloatToByteArray(double dNumber, BYTE bOut[],uint8_t* nLengthTithes);
void FloatToByteArrayWithNipes(double dNumber, BYTE bOut[], uint8_t* nLengthTithes);
BYTE ascii2hex(char inASCII);
#endif // ULTIL_H
