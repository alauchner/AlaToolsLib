// alaTools.h
#pragma once

#include <arduino.h>

struct EepConfigDataT
{
	int dummy;
	int magic; // Data in EEPROM valid, am Ende, damit bei Aenderung struct ungueltig
};

bool EepLoadConfig();
void EepSaveConfig();
bool ThingspeakSend(String s, String api_key);
tm GetNowTm();
time_t GetNowTimeT();
String tm2TimeString(tm);
String UtcTime2LocalTimeString(time_t);
void WifiBegin(const String ssid, const String password);

extern tm bootTime;
extern EepConfigDataT eep_config;

//****************************************************** */
// Serial DEBUG Messages Managemnet
#if (!defined(SERIAL_DEBUG) || SERIAL_DEBUG)
#pragma message("FormattingSerialDebug ENABLED")

/*
 * d, i	int as a signed decimal number. '%d' and '%i' are synonymous for output, but are different when used with scanf() for input (where using %i will interpret a number as hexadecimal if it's preceded by 0x, and octal if it's preceded by 0.)
 * u	decimal unsigned int.
 * l	long as a signed decimal number.
 * ul	decimal unsigned long.
 * f, F	double in normal (fixed-point) notation. 'f' and 'F' only differs in how the strings for an infinite number or NaN are printed ('inf', 'infinity' and 'nan' for 'f', 'INF', 'INFINITY' and 'NAN' for 'F').
 * e, E	double value in standard form ([-]d.ddd e[+/-]ddd). An E conversion uses the letter E (rather than e) to introduce the exponent. The exponent always contains at least two digits; if the value is zero, the exponent is 00. In Windows, the exponent contains three digits by default, e.g. 1.5e002, but this can be altered by Microsoft-specific _set_output_format function.
 * g, G	double in either normal or exponential notation, whichever is more appropriate for its magnitude. 'g' uses lower-case letters, 'G' uses upper-case letters. This type differs slightly from fixed-point notation in that insignificant zeroes to the right of the decimal point are not included. Also, the decimal point is not included on whole numbers.
 * x, X	unsigned int as a hexadecimal number. 'x' uses lower-case letters and 'X' uses upper-case. Prefix with 'l' to print long numbers.
 * o	unsigned int in octal. Prefix with 'l' to print long numbers.
 * s	null-terminated string.
 * c	char (character).
 * p	void * (pointer to void) in an implementation-defined format.
 * a, A	double in hexadecimal notation, starting with "0x" or "0X". 'a' uses lower-case letters, 'A' uses upper-case letters.
 * n	Print nothing, but write number of characters successfully written so far into an integer pointer parameter.
 * %	a literal '%' character (this type doesn't accept any flags, width, precision or length).
 */

#define DEBUG(format, ...)             \
	{                                  \
		printf(format, ##__VA_ARGS__); \
		fflush(stdout);                \
		Serial.println();              \
	}

#else
#pragma message("FormattingSerialDebug DISABLED")
#define DEBUG(format, ...)
#endif // SERIAL_DEBUG