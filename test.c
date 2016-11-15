#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(int argc, char const *argv[])
{
	unsigned char byte[] = {0xf0, 0xd2, 0xf1, 0xb2, 0xd2, 0x55, 0x0, 0x0};


	uint64_t n = 0;

	n += ((uint64_t)byte[7]<<56);
	n += ((uint64_t)byte[6]<<48);
	n += ((uint64_t)byte[5]<<40);
	n += ((uint64_t)byte[4]<<32);
	n += (byte[3]<<24);
	n += (byte[2]<<16);
	n += (byte[1]<<8);
	n += (byte[0]<<0);


	printf("%p\n", (void*)n);

	return 0;
}