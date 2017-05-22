/*
 * guide-agent.c - implements the KRAG game logic for the Guide Agent module
 * to handle input messages and update the interface based on input, and
 * send messages to server based on game status
 *
 *
 * GREP, CS50, May 2017
 */







int main(int argc, char **argv) 
{
	// invalid argument count error
	if (argc != 6) {
		fprintf(stderr, "usage: guideagent guideId=... team=... player=... 
			host=... port=...\n");
		exit(1);
	}
}