#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void p(){
	printf("puta do amores\n");
}

int main(){
	signal(SIGALRM,p);
	alarm(2);
	sleep(5);
	printf("putinha\n");
	return 0;
}