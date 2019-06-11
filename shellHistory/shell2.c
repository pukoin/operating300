#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

#define COMMAND_LENGTH 1024
#define NUM_TOKENS (COMMAND_LENGTH / 2 + 1)
#define MAX_BUFFER_LENGTH 1024
#define HISTORY_DEPTH 10

char history[HISTORY_DEPTH][COMMAND_LENGTH];
char buffer[MAX_BUFFER_LENGTH];
int Hist_count=0, Hist_index=0;

void clear(char *S)
{
	int i;
	for(i=0;i<MAX_BUFFER_LENGTH;i++)
		S[i] = '\0';
	return;
}

int tokenize_command(char *buff, char *tokens[])
{
	int token_count = 0;
	_Bool in_token = false;
	int num_chars = strnlen(buff, COMMAND_LENGTH);
	for (int i = 0; i < num_chars; i++) {
		switch (buff[i]) {
		// Handle token delimiters (ends):
		case ' ':
		case '\t':
		case '\n':
			buff[i] = '\0';
			in_token = false;
			break;

		// Handle other characters (may be start)
		default:
			if (!in_token) {
				tokens[token_count] = &buff[i];
				token_count++;
				in_token = true;
			}
		}
	}
	tokens[token_count] = NULL;
	return token_count;
}

/**
 * Read a command from the keyboard into the buffer 'buff' and tokenize it
 * such that 'tokens[i]' points into 'buff' to the i'th token in the command.
 * buff: Buffer allocated by the calling code. Must be at least
 *       COMMAND_LENGTH bytes long.
 * tokens[]: Array of character pointers which point into 'buff'. Must be at
 *       least NUM_TOKENS long. Will strip out up to one final '&' token.
 *       tokens will be NULL terminated (a NULL pointer indicates end of tokens).
 * in_background: pointer to a boolean variable. Set to true if user entered
 *       an & as their last token; otherwise set to false.
 */
void read_command(char *buff, char *tokens[], _Bool *in_background)
{
	*in_background = false;

	// Read input
	int length = read(STDIN_FILENO, buff, COMMAND_LENGTH-1);

	if ((length < 0) && (errno !=EINTR) ) 
	{
		perror("Unable to read command from keyboard. Terminating.\n");
		exit(-1);
	}

	// Null terminate and strip \n.
	buff[length] = '\0';
	if (buff[strlen(buff) - 1] == '\n') {
		buff[strlen(buff) - 1] = '\0';
	}

	// Tokenize (saving original command string)
	int token_count = tokenize_command(buff, tokens);
	if (token_count == 0) {
		return;
	}

	// Extract if running in background:
	if (token_count > 0 && strcmp(tokens[token_count - 1], "&") == 0) {
		*in_background = true;
		tokens[token_count - 1] = 0;
	}
}

int Judge(char* S) //Judge inner type command
{
	if(strcmp(S,"exit") == 0 || strcmp(S,"pwd") == 0 || strcmp(S,"cd") == 0 || strcmp(S,"type") == 0)
		return 1;
	else
		return 0;
}

void handle_SIGINT() //handle for control_C
{
	int temp,i;
	char tempbuffer[1024];

	write(STDOUT_FILENO,"\n",1);
		
	if(Hist_count <= 10)
	{
		for(i=0;i<Hist_count;i++)
		{
			sprintf(tempbuffer,"%-6d",i+1);
			strcat(tempbuffer,history[i]);
			strcat(tempbuffer,"\n");
			write(STDOUT_FILENO,tempbuffer,strlen(tempbuffer));
		}
	}
	else
	{
		temp = Hist_index;
		for(i=0;i<10;i++)
		{
			sprintf(tempbuffer,"%-6d",Hist_count-9+i);
			strcat(tempbuffer,history[temp]);
			strcat(tempbuffer,"\n");
			temp ++;

			if(temp == 10)
				temp = 0;
			write(STDOUT_FILENO,tempbuffer,strlen(tempbuffer));
		}
	}

	return;
}
/**
 * Main and Execute Commands
 */
int main(int argc, char* argv[])
{
	char input_buffer[COMMAND_LENGTH];
	char *tokens[NUM_TOKENS];
	_Bool in_background;
	pid_t pid;
	int i, flag = 0, token_count;

	struct sigaction handler;
	handler.sa_handler = handle_SIGINT;
	handler.sa_flags = 0;
	sigemptyset(&handler.sa_mask);
	sigaction(SIGINT, &handler, NULL);


	while (true) 
	{
		// Get command

		if(flag == 0)  //if the command are read from trminal
		{
			clear(buffer);
			clear(input_buffer);

			getcwd(buffer,MAX_BUFFER_LENGTH);
			write(STDOUT_FILENO,buffer,strlen(buffer));
			write(STDOUT_FILENO, "$ ", strlen("$ "));
			in_background = false;
			read_command(input_buffer, tokens, &in_background);
			
			if(tokens[0] != NULL && tokens[0][0] != '!')
			{
				Hist_count++;
				strcpy(history[Hist_index],input_buffer);
				Hist_index++;
			}
			
			if(Hist_index == 10)
				Hist_index = 0;
		}
		else		//run history command, input_buffer is already history command
		{
			token_count = tokenize_command(input_buffer, tokens);

			if (token_count > 0 && strcmp(tokens[token_count - 1], "&") == 0) 
			{
				in_background = true;
				tokens[token_count - 1] = 0;
			}
			flag = 0;
				
			if(tokens[0] != NULL && tokens[0][0] != '!')
			{
				Hist_count++;
				strcpy(history[Hist_index],input_buffer);
				Hist_index++;
			}
			
			if(Hist_index == 10)
				Hist_index = 0;
		}

		if(tokens[0] == NULL)    //simply press enter
			continue;
		else if(Judge(tokens[0]))  //inner command
		{
			if(strcmp(tokens[0],"exit") == 0)
			{
				exit(-1);
			}
			else if(strcmp(tokens[0],"pwd") == 0)
			{
				clear(buffer);
				getcwd(buffer,MAX_BUFFER_LENGTH);
				write(STDOUT_FILENO,buffer,strlen(buffer));
				write(STDOUT_FILENO,"\n",1);
			}
			else if(strcmp(tokens[0],"cd") == 0)
			{
				if(chdir(tokens[1]) == -1)
				{
					clear(buffer);
					strcpy(buffer,"Change failed");
					write(STDOUT_FILENO,buffer,strlen(buffer));
					write(STDOUT_FILENO,"\n",1);
				}
			}
			else if(strcmp(tokens[0],"type") == 0)
			{
				if(Judge(tokens[1]))
				{
					clear(buffer);
					strcpy(buffer,tokens[1]);
					strcat(buffer," is a shell300 builtin\n");
					write(STDOUT_FILENO,buffer,strlen(buffer));
				}
				else
				{
					clear(buffer);
					strcpy(buffer,tokens[1]);
					strcat(buffer," is external to shell300\n");
					write(STDOUT_FILENO,buffer,strlen(buffer));
				}
			}
		}
		else if(strcmp(tokens[0],"history") == 0)  //show history
		{
			int temp;
			if(Hist_count <= 10)		//when count is less than 10
			{
				for(i=0;i<Hist_count;i++)
				{
					clear(buffer);
					sprintf(buffer,"%-6d",i+1); //index of command is i+1
					strcat(buffer,history[i]);
					strcat(buffer,"\n");
					write(STDOUT_FILENO,buffer,strlen(buffer));
				}
			}
			else			//othwewise
			{
				temp = Hist_index;
				for(i=0;i<10;i++)
				{
					clear(buffer);
					sprintf(buffer,"%-6d",Hist_count-9+i);  //compute index
					strcat(buffer,history[temp]);
					strcat(buffer,"\n");

					temp ++;
					if(temp == 10)
						temp = 0;
					write(STDOUT_FILENO,buffer,strlen(buffer));
				}
			}	
		}
		else if(tokens[0][0] == '!')   //run history command
		{
			int temp, num = strlen(tokens[0]), index;
			
			if(tokens[0][1] == '!')  // for !! run last command
			{
				temp  =Hist_index - 1;
				if(temp < 0)
					temp += 10;
				strcpy(input_buffer,history[temp]);	
				flag = 1;
			}
			else		//!x run the xth command
			{		
				for(i=0;i<num;i++)
					tokens[0][i] = tokens[0][i+1];	//get the number x
				tokens[0][i] = '\0';
				index = atoi(tokens[0]);

				if(index == 0 || index > Hist_count)
				{
					write(STDOUT_FILENO,"Error input for history",23);
					continue;
				}
				else
				{
					if(Hist_count > 10) // the array are used as a loop queue. Data continue fill in
					{		    // from head to tail
						temp = Hist_count - index;
						temp = Hist_index -1 - temp; //compute actual index in array
						if(temp < 0)
							temp += 10;
						strcpy(input_buffer,history[temp]);	
					}
					else
						strcpy(input_buffer,history[index-1]);
					flag = 1;	
				}	
			}		
		}
		else
		{
			pid = fork();	//fork a new process
			if(pid < 0)
			{
				clear(buffer);
				strcpy(buffer,"Fork failed");
				write(STDOUT_FILENO,buffer,strlen(buffer));
				exit(-1);
			}
			else if(pid == 0) //child process
			{
				execvp(tokens[0], tokens);
				break;	//after child process finish, break the loop and terminate
			}
			else
			{
				if(in_background == false)	//wait for child not in background
					waitpid(pid,NULL,0); 
			}
		}
		
		while (waitpid(-1, NULL, WNOHANG) > 0); //wait
	
	}
	return 0;
}