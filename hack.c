#define _GNU_SOURCE
#include <sys/uio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>       
#include <unistd.h>
#include <sys/types.h>
#include <linux/limits.h>

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

typedef void *addr; // Using void pointers to reference and compare memory addresses

void print_bytes(char* buffer, ssize_t len){

  for (int i = 0; i < len; ++i){
    printf("%X ", buffer[i] & 0xFF);
  }
  printf("\n");

  return;
}

pid_t getPID(char* procname){

	pid_t pid;
	char line[10];

	char str[35];
	strcpy(str, "pidof  ");
	strcat(str, procname);

	FILE *cmd = popen(str, "r");

	fgets(line, 10, cmd);

	pid = strtoul(line, NULL, 10);

	pclose(cmd);

	return pid;
}

void privcheck(){

	uid_t uid=getuid(), euid=geteuid();
	if (uid != 0 || uid!=euid) {
		printf("PROBLEM: Need to run as root.\n");
		exit(0);
	}

	return;
}

addr getbaseaddr(pid_t pid){

	char fname[PATH_MAX];
    FILE *f;

    sprintf(fname, "/proc/%i/maps", pid);
    f = fopen(fname, "r");
	
    char buff[255];
   	fgets(buff, 255, f);

  	long long unsigned int base_addr;

  	sscanf(buff,"%Lx",&base_addr);

	return (addr)base_addr;
}


ssize_t read_bytes(pid_t pid, addr mem_addr, char* buf) {

}

int bytesToInt(unsigned char* b){
	int val = 0;
	int j = 0;
	for (int i = 0; i < 4; ++i)
	{
		val += (b[i] & 0xFF) << (8*j);
		++j;
	}
	return val;
}

int intToBytes(int n, char *buf){

	for (int i = 0; i < 4; ++i){
		buf[i] = n>>i*8;
	}

	return 0;
}

int read_int(pid_t pid, addr addy){
	struct iovec local[1];
	struct iovec remote[1];

	int nbytes = 4;
	char buf[nbytes];
	ssize_t nread;           

	local[0].iov_base = buf;
	local[0].iov_len = nbytes;

	remote[0].iov_base = addy;
	remote[0].iov_len = nbytes;

	nread = process_vm_readv(pid, local, 1, remote, 1, 0);

	return bytesToInt(buf);
}

int write_int(pid_t pid, addr addy, int n){

 	char buf[4];
 	intToBytes(1337,buf);

	struct iovec local[1];
	struct iovec remote[1];

	ssize_t nread;           

	local[0].iov_base = buf;
	local[0].iov_len = 4;

	remote[0].iov_base = addy;
	remote[0].iov_len = 4;

	nread = process_vm_writev(pid, local, 1, remote, 1, 0);

	return nread;
}


int main(void){

	privcheck(); // Make sure we are running as root (process_vm_readv and writev require this)

	char* name = "structloop";

	pid_t pid = getPID(name);

	if (pid == 0){
		printf("PROBLEM: Process not found.\n");
		exit(0);
	}

	printf(YEL "Found PID: \t" RESET "%i\n", pid);

	addr base_addr = getbaseaddr(pid);

    printf(YEL "Found Base Address: \t" RESET "%p\n", base_addr);

    addr offset_addr = base_addr+0x201014;

    printf(YEL "Offset Address: \t" RESET "%p\n", offset_addr);


    int y = read_int(pid,offset_addr);
    printf("Y: %i\n", y);

    int s = write_int(pid,offset_addr,1337);
    printf("S: %i\n", s);

/*
	// 1337 decimal in hex is 0x539
	buf[0] = 0x39;
	buf[1] = 0x05;
	buf[2] = 0x00;
	buf[3] = 0x00;
56075b6015d8
56075a02b6c0


56075a02b5b0 + 0x110
=
B0 B5 2 5A 7 56 0 0 




558070b75f88
55806f59f6c0


55806f59f5b0  + 0x110 = 55806f59f6c0
B0 F5 59 6F 80 55 0 0

	nread = process_vm_writev(pid, local, 1, remote, 1, 0);

	printf("%li bytes written to %p\n", nread,(void *) offset_addr);

	print_bytes(buf,sizeof(buf)/sizeof(char));  */

    return 0; // Success -- we're done!
}
