#include "pch.h"
#include <intsafe.h>
#include "ultil.h"
void FloatToByteArray(double dNumber, BYTE bOut[], uint8_t* nLengthTithes)
{
	uint8_t nCountTithes = 0;
	bOut[0] = (BYTE)dNumber;
	float  nTithes = (float)(dNumber - bOut[0]);
	do
	{
		nTithes *= 10;
		if (nCountTithes++ >> 3) break;

	} while (nTithes <= 1);
	*nLengthTithes = nCountTithes;
	bOut[1] = (BYTE)nTithes;
}
void FloatToByteArrayWithNipes(double fNumber, BYTE bOut[], uint8_t* nLengthTithes) {
	uint8_t nCountTithes = 5;
	bOut[0] = (BYTE)fNumber;
	double  nTithes = (double)(fNumber - bOut[0]);

	nTithes = nTithes * pow(10, nCountTithes);
	while (nTithes > 255) {
		nTithes /= 10;
		nCountTithes--;
	}
	*nLengthTithes = nCountTithes;
	bOut[1] = (BYTE)nTithes;
}
/*
 *  DESCRIPTION
 *    Converts an ascii character ('0'..'f' or '0'..'F') to corresponding integer value.
 *    In case of invalid ascii character, return value is 0xff
 *  USAGE
 *    uint8 ascii2hex( char inASCII );
 *  PARAMETERS
 *    char inASCII  : ascii character to convert
 *  RETURN VALUE
 *    uint8         : value of inASCII, 0xff for invalid input
 */
BYTE ascii2hex(char inASCII)
{
	BYTE retHex = 0xff;

	if ((inASCII >= 48) && (inASCII <= 57))
		retHex = inASCII - 48;
	else if ((inASCII >= 65) && (inASCII <= 70))
		retHex = inASCII - 55;
	else if ((inASCII >= 97) && (inASCII <= 102))
		retHex = inASCII - 87;
	return retHex;
}
void decToHexa(uint8_t n)
{

	// char array to store hexadecimal number
	char hexaDeciNum[100];

	// counter for hexadecimal number array
	int i = 0;
	while (n != 0) {
		// temporary variable to store remainder
		int temp = 0;

		// storing remainder in temp variable.
		temp = n % 16;

		// check if temp < 10
		if (temp < 10) {
			hexaDeciNum[i] = temp + 48;
			i++;
		}
		else {
			hexaDeciNum[i] = temp + 55;
			i++;
		}

		n = n / 16;
	}

	// pr
}