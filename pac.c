#include "header.h"

int main(){
	int fd;
	char buffer[MAX];

	if((fd = open(PIPE_NAME, O_WRONLY)) < 0){
		perror("Error opening pipe for writing: ");
	    exit(0);
	}
	while(1){

		scanf("%s", buffer);
		write(fd, buffer, sizeof(buffer));

	}
}