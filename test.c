#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>

/*
 * A very simple shell that supports only commands 'exit', 'help', and 'today'.
 */

#define MAX_BUF 160
#define MAX_TOKS 100

int main(int argc, char **argv) {

	int ch;
	char *pos;
	char s[MAX_BUF+2];   // 2 extra for the newline and ending '\0'
	static const char prompt[] = "(egsh) ";
	char *toks[MAX_TOKS];
	char *tok;
	char *toks2[MAX_TOKS];
	
	// 
	// YOUR CODE HERE   (add declarations as needed)
	//
       	int i;
	int k = 0;
	int day;
	int month;
	int year;
	FILE* strm;
	char* file;
	char* status;
	FILE* strm2;
	int flag = 0;
	int tracker1 = 0;
	int tracker2 = 0;
	int l = 0;
	int done = 1;
	int r = 0;		
	char cwd[MAX_BUF];
  	struct dirent *dp;
  	DIR *dirp;  
      	
	//check if any arguments were added to run command
	if (argc > 1) {
	  file = *(argv + 1);
	  strm = fopen(file, "r");
	  /* printf("%d\n", dup(fileno(strm))); */
       }

  while (1) {
    if (argc > 1) {
      char *status = fgets(s, MAX_BUF+2, strm);
      if (status == NULL) {
	printf("\n");
	break;
      }
    } else {
      // prompt for input if input from terminal
      if (isatty(fileno(stdin))) {
        if (getcwd(cwd, MAX_BUF) != NULL) {
          printf("%s%s: ", prompt, cwd);
        } else {
          printf("error: unable to get current working directory\n");
          printf(prompt);
        }
      }

		// read input
		status = fgets(s, MAX_BUF+2, stdin);
	  
		// exit if ^d entered
		if (status == NULL) {
			printf("\n");
			break;
		}
	    }
		// input is too long if last character is not newline 
		if ((pos = strchr(s, '\n')) == NULL) {
			printf("error: input too long\n");
			// clear the input buffer
			while ((ch = getchar()) != '\n' && ch != EOF) ;
 			continue;
		}
	  
		// remove trailing newline
		*pos = '\0';

                //
		// your code here
	        //
		//

				
		time_t t;
		time(&t);
		struct tm *today = localtime(&t);
		day = today -> tm_mday;
		month = today -> tm_mon + 1;
		year = today -> tm_year + 1900; // is this Y2K??? haha
		
		// fill the toks array with text separated by spaces		
		i = 0;
		char *rest = s;
		while ((tok = strtok_r(rest, " ", &rest)) != NULL && i < MAX_TOKS) {		  
		    toks[i] = tok;
		    i++;
		}
		if (i == MAX_TOKS) {
		  printf("error: too many tokens\n");
		  continue;
		}
		toks[i] = NULL;

		// if no tokens do nothing
		if (toks[0] == NULL) {
		  continue;
		}
		
		if (strcmp(toks[0], "help") == 0 && toks[1] == NULL) {
		  printf("Enter Linux commands, or 'exit' to quit\n");
		  continue;
		}
		
		if (strcmp(toks[0], "today") == 0 && toks[1] == NULL) {
		  printf("%02d/%02d/%04d\n", month, day, year);
		  continue;
		}
		
		if (strcmp(toks[0], "exit") == 0 && toks[1] == NULL) {
		  break;
		}	       	
		if (strcmp(toks[0], "cd") == 0) {
		  errno = 0;
		  if (toks[1] == NULL) {
		    const char* home = getenv("HOME");
		    chdir(home);
		  }
		  else if (strcmp(toks[1], "$HOME") == 0) {
		    const char* home = getenv("HOME");
		    chdir(home);
		  }
		  else if (strcmp(toks[1], "~") == 0) {
		    const char* home = getenv("HOME");
		    chdir(home);		      
		  }
		  else {
		    chdir(toks[1]);
		  }
		    
		  if (errno != 0){
		    printf("(egsh): %s: %s: %s\n", toks[0], toks[1], strerror(errno));
		  }
		}

		else { // create new thread
		  int rc = fork();
		  if (rc < 0) { // fork failed
		    fprintf(stderr, "fork failed\n");
		    exit(1);		      
		  }
		  else if (rc == 0) { // new child process
		    l = 0;
		    k = 0;
		    while (toks[k] != NULL) {
		      if (strcmp(toks[k], ">") ==  0) {
		 	strm2 = fopen(toks[k+1], "w");
			dup2(fileno(strm2), 1);
			k++;
			if (toks[k+2] != NULL) {
			  k++;
			}
			else break;
		      }
		      else if (strcmp(toks[k], "<") ==  0) {
			strm2 = fopen(toks[k+1], "r");
			dup2(fileno(strm2), 0);
			k++;
			if (toks[k+2] != NULL) {
			  k++;
			}
			else break;			
		      }
		      
		      else {
			toks2[l] = toks[k];
			l++;
		        k++;
		       }
		    }
		    toks2[l] = NULL;		    
		    execvp(toks2[0], toks2); // execute linux commands
		    printf("(egsh): %s: %s\n", toks2[0], strerror(errno));    
		    exit(1);			
		  }
		  else {
		    int rc_wait = wait(NULL);
		  }		    
		}		
	}
	exit(EXIT_SUCCESS);	
}
