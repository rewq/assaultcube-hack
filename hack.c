#define _GNU_SOURCE
#include <sys/uio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>       
#include <unistd.h>
#include <sys/types.h>
#include <linux/limits.h>

#include <stdint.h>

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
   	fgets(buff, 255, f); /* because i need better way for assault cube to parse mpas file third line */
   	fgets(buff, 255, f);
   	fgets(buff, 255, f);
   	printf("%s\n", buff);

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

long bytesToLong(unsigned char* b){

	uint64_t n = 0;

	n += ((uint64_t)b[7]<<56);
	n += ((uint64_t)b[6]<<48);
	n += ((uint64_t)b[5]<<40);
	n += ((uint64_t)b[4]<<32);
	n += ((uint64_t)b[3]<<24);
	n += ((uint64_t)b[2]<<16);
	n += ((uint64_t)b[1]<<8);
	n += ((uint64_t)b[0]<<0);

	return n;
}

int intToBytes(int n, char *buf){

	for (int i = 0; i < 8; ++i){
		buf[i] = n>>i*8;
	}

	return 0;
}

long read_long(pid_t pid, addr addy){
	struct iovec local[1];
	struct iovec remote[1];

	int nbytes = 8;
	unsigned char buf[nbytes];
	ssize_t nread;           

	local[0].iov_base = buf;
	local[0].iov_len = nbytes;

	remote[0].iov_base = addy;
	remote[0].iov_len = nbytes;

	nread = process_vm_readv(pid, local, 1, remote, 1, 0);

	for (int i = 0; i < 8; ++i)
	{
		printf("%x ", buf[i]);
	}printf("\n");

	return bytesToLong(buf);
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

 	unsigned char buf[4];
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

	char* name = "assaultcube";

	pid_t pid = getPID(name);

	if (pid == 0){
		printf("PROBLEM: Process not found.\n");
		exit(0);
	}

	printf(YEL "Found PID: \t" RESET "%i\n", pid);

	addr base_addr = getbaseaddr(pid);


    addr a_player = base_addr+0x378130;
    addr a_playerP = (addr)read_long(pid,a_player);

    addr a_player_vh = a_playerP+0x110;


    printf(YEL "Found Base Address: \t" RESET "%p\n", base_addr);
    printf(YEL "Player Pointer: \t" RESET "%p\n", a_playerP);
    printf(YEL "Player V Health: \t" RESET "%p\n", a_player_vh);

   
    if (write_int(pid,(addr)a_player_vh,1337)){
    	printf(YEL "Hacked Health" RESET "\n");
    } 


    return 0;
}

/*
0x4004d000 base addr

0x55d2b7a4b918 // not visual
0x55d2b2f1d400 // visual
// 4b6798
// 378130
*/