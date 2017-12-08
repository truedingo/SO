#include "header.h"

int main(){
	int fd;
	char buffer[MAX];
	
	while(1){

		scanf("%s", buffer);
		fd = open(PIPE_NAME, O_WRONLY);
		write(fd, buffer, sizeof(buffer));

	}
}