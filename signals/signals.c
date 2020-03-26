#include<stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

// Handler function for alarm signal.
void alarm_handler(int signo) {
	if (signo == SIGALRM) {
		printf("Alarm\n");
	}
}

// Handler function for interrupt signal.
void interupt_handler(int signo) {
	if (signo == SIGINT) {
		printf("CTRL+C pressed!\n");
	}
}

// Handler function for stop signal.
void stop_handler(int signo) {
	if (signo == SIGTSTP) {
		printf("CTRL+Z pressed!\n");
		exit(0);
	}
}

int main(void) {
	// Register the alarm handler
	if (signal(SIGALRM, alarm_handler) == SIG_ERR) {
		printf("Failed to register alarm handler");
		exit(1);
	}

	// Register the interrupt handler
	if (signal(SIGINT, interupt_handler) == SIG_ERR) {
		printf("Failed to register interupt handler");
		exit(1);
	}

	// Register the stop handler
	if (signal(SIGTSTP, stop_handler) == SIG_ERR) {
		printf("Failed to register stop handler");
		exit(1);
	}
	
	while(1) {
		alarm(2); // Set an alarm to go off every 2 seconds 
		sleep(2); // Wait 2 seconds for the alarm to go off
	}
} 