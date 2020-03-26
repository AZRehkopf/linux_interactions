#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

int main(void) {
	int pipe1[2], pipe2[2], nbytes, accum, final_sum, child_input_num;
	pid_t childpid;
	char received_num, transmit;

	pipe(pipe1);
	pipe(pipe2);

	if ((childpid = fork()) == -1) {
		perror("fork");
		exit(0);
	}

	if (childpid == 0) {
		// Close the pipes that are not needed
		close(pipe1[0]);
		close(pipe2[1]);

		child_input_num = 0;
		
		// Get user input until a -1 is entered
		while (child_input_num != -1) {
			printf("Enter a number: \n");
			scanf("%d", &child_input_num);
			
			if (child_input_num > -129 && child_input_num < 128) {
				transmit = child_input_num;
				write(pipe1[1], &transmit, sizeof(transmit));
			} else {
				printf("Number does not fit on 1 byte. Please enter a number between -128 and 127.\n");
			}
		}

		// Read and display the final sum
		read(pipe2[0], &final_sum, sizeof(final_sum));
		printf("The final sum is %d\n", final_sum);

		exit(0);

	} else {
		// Close the pipes that are not needed
		close(pipe1[1]);
		close(pipe2[0]);

		received_num = 0;
		accum = 0;

		// Keep accepting numbers until a -1 is received
		while (received_num != -1) {
			nbytes = read(pipe1[0], &received_num, sizeof(received_num));
			accum += received_num;
		}
		accum++;
		
		// Send the sinal sum 
		write(pipe2[1], &accum, sizeof(accum));
	}
	return 0;
}