#include <stdint.h>
#include <stdio.h>

#include "adler32.h"

#define ADLER_PRIME 65521 /* largest prime smaller than 65536 */

/* ADLER_MOD_FREQ defined by
ADLER_MAX_VALUE = ADLER_PRIME - 1
(255/2) * ADLER_MOD_FREQ^2 + (ADLER_MAX_VALUE + 255/2) * ADLER_MOD_FREQ + ADLER_MAX_VALUE < (2^32 - 1)
*/
#define ADLER_MOD_FREQ 5552 /* maximum byte delay before modulo when compute on 32 bits registers */
#define ADLER_S1_INIT 0x0001
#define ADLER_S2_INIT 0x0000

uint32_t adler32_append(uint32_t adler, const char *buffer, size_t size)
{
	uint32_t s1 = adler & 0xffff;
	uint32_t s2 = (adler >> 16) & 0xffff;
	int n;

	for (n = 0; n < size; n++)
	{
		s1 += buffer[n];
		s2 += s1;
		if ((n % ADLER_MOD_FREQ) == (ADLER_MOD_FREQ-1))
		{
			s1 %= ADLER_PRIME;
			s2 %= ADLER_PRIME;
		}
	}
	s1 %= ADLER_PRIME;
	s2 %= ADLER_PRIME;
	
	return (s2 << 16) | s1;
}

uint32_t adler32(const char *buffer, size_t size)
{
	uint32_t adler = (ADLER_S2_INIT << 16) | ADLER_S1_INIT;

	return adler32_append(adler, buffer, size);
}

static char buffer[BUFFER_SIZE];
uint32_t fnadler32(const char *file, uint32_t max_round)
{
	uint32_t adler = (ADLER_S2_INIT << 16) | ADLER_S1_INIT;
	FILE *f;
	int read;

	f = fopen(file, "rb");
	while (!feof(f) && max_round--)
	{
		read = fread(buffer, 1, sizeof(buffer), f);
		adler = adler32_append(adler, buffer, read);
	}
	fclose(f);

	return adler;
}

uint32_t fadler32(const char *file)
{
	return fnadler32(file, (uint32_t)-1);
}

