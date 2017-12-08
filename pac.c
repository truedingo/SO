#include "header.h"

int main(){
	int fd;
	char buffer[MAX];
	char checker[MAX];

	while(1){

		scanf("%s", buffer);
		strcpy(çhecker, buffer);
		fd = open(PIPE_NAME, O_WRONLY);
		write(fd, buffer, sizeof(buffer));

	}
}