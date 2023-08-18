/*  
*   Developer:   Arda Cifci
*       
*
*   This Program creates new a basic shell.
*/

//Standard Libraries and posix
#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 
#include <time.h> 
#include <unistd.h> 
#include <sys/wait.h> 

// Structure for storing environment variables.
typedef struct {
    char *name;
    char *value;
} EnvVar;
// Structure for storing commands for the logs.
 typedef struct {
    char *name;
    char *time;
    int value;
} Command;

//Initilization for functions.
Command new_command(char *name, int value);
EnvVar new_enviVar(char *name, char *value);
int errorCheck(char *token[], int size, EnvVar environ[], int varSize, int arg, char *argg[]);

// MAIN 
int main(int argc, char *argv[]){ 
    EnvVar environVarible[50]; // Setup 50 environment structs to use.
    Command commandLogs[50]; // Setup 50 command structs to use.

    char userInput[500] = {"\0"}; // User input variable for stdin.
    
    // ANSI escape codes used for chaning text colour.
    const char themeBlue[10] = {"\x1b[34m"};
    const char themeRed[10] = {"\x1b[31m"};
    const char themeGreen[10] = {"\x1b[32m"};
    char themeTracker[10] = {"\0"}; // Current theme colour being tracked.

    const char s[2] = " "; // Space constant used for creating tokens.
    char *tokens[50]; // Tokens created are stored here.
    char *varTokens[5]; // Specific tokens of $<VAR>=<value> stored here.

    int flag = 0; // Used to keep the while loop going till user inputs exit.
    int errorFlag = 0; // Used to store if the command is valid or not.

    // Various variables used for keeping track of sizes for array or loop traversal.
    int sizeCounter = 0;
    int i = 0;
    int h = 0; 
    int k = 0;
    int m = 0;
    int p = 0;

    char scriptInputs[50][200]; // Keeps script mode inputs from txt file. 
       
    // If no txt file: ignore, else store txt file commands into array.
    if(argc == 1){ 
    }else{
        FILE *fp = fopen(argv[1],"r");
        int u = 0;
        while(fgets(scriptInputs[u], 200, fp) != NULL){
            u++;
            m++;
        }
        fclose(fp);
    }

    // Continue to loop until user enters exit.
    while(flag == 0){
        // Reset userInput and tokens variables for next input.
        for(int i = 0; i <= 499; i++){
            userInput[i] = '\0';
        }
        for(int i = 0; i <= 49; i++){
            tokens[i] = '\0';
        }
        i = 0;

        // If in interactive mode, ask user for input. Else take inputs from txt file array.
        if(argc == 1){
            printf("%scshell$ ", themeTracker);
            fgets(userInput, 500, stdin);
        }else{
            if(p <= m){
                strcpy(userInput, scriptInputs[p]);
                p++;
            }
            if(p > m){ // If processed all txt commands, exit program. 
                flag = 1;
                printf("%sBye!\n", themeTracker);

                // FREE STRDUP's which can cause mem leak.
                for(int i = 0; i < h; i++){
                    free(commandLogs[i].name);
                    free(commandLogs[i].time);
                }

                for(int b = 0; b < k; b++){
                    free(environVarible[b].value);
                    free(environVarible[b].name);
                }
            
                exit(0);
            }
        }

        // Create the tokens from user Input & keep track of its size.
        tokens[i] = strtok(userInput, s);
        while(tokens[i] != NULL){
            tokens[++i] = strtok(NULL, s);
            sizeCounter++;
        }
        
        strtok(tokens[0], "\n"); // Remove the \n from the first token.

        // ERROR CHECK : return 0 or -1 depending on if input is valid or not.
        errorFlag = errorCheck(tokens, sizeCounter, environVarible, k, argc, argv);

        // If user inputs a environment varible, handle it here. 
        if(strstr(tokens[0], "$") != NULL && errorFlag == 0){ 
            char comm3[500];
            // Recreate the inputted string and create 2 tokens for struct.
            size_t len2 = 0;
            for(size_t i = 0; i < sizeCounter;i++){
                len2 += (size_t)snprintf(comm3 + len2, (sizeof comm3) - len2,"%s ", tokens[i]);    
            }
            comm3[len2 -1] = '\n';
            int kq = 0;
            varTokens[kq] = strtok(comm3, "=");
            while(varTokens[kq] != NULL){
                varTokens[++kq] = strtok(NULL, "=");
            }
            strtok(varTokens[0], "\n");
            strtok(varTokens[1], "\n");

            // Check if environment variale has been created before.
            int envFlag = 0;
            int z = 0;
            int zTrue = 0;
            for(z = 0; z < k; z++){
                if(strcmp(environVarible[z].name, varTokens[0]) == 0){
                    envFlag = 1;
                    zTrue = z;
                    z = k;
                }
            } // If environment variable has been created before: update it. Else create new.
            if(envFlag == 1){
               environVarible[zTrue].value = strdup(varTokens[1]);
            }else{
                environVarible[k] = new_enviVar(varTokens[0], varTokens[1]);
                ++k;
            }
        }

        // If user inputs print command handle it here.
        if(strcmp(tokens[0], "print") == 0 && errorFlag == 0){
            //Check if user inputted an environment varible and retrieve the value.
            for(int l = 1; l < sizeCounter; ++l){
                if(strchr(tokens[l], '$') != NULL){
                    for(int n = 0; n < k; n++){
                        strtok(tokens[l], "\n");
                        if(strcmp(tokens[l], environVarible[n].name) == 0){ 
                            tokens[l] = environVarible[n].value;
                        }
                    }
                }
            }// Print out tokens user inputted.
            for(int i = 1; i < sizeCounter; i++){
                strtok(tokens[i], "\n");
                printf("%s%s ",themeTracker ,tokens[i]);
            }
            printf("\n");
            sizeCounter = 0;
        }

        // If user inputs theme command handle it here.
        // Change theme color depending on user input (blue, green, red).
        if(strcmp(tokens[0], "theme") == 0 && errorFlag == 0){
            strtok(tokens[1], "\n");
            if(strcmp(tokens[1], "blue") == 0){
                strcpy(themeTracker, themeBlue);
            } else if(strcmp(tokens[1], "red") == 0){
                strcpy(themeTracker, themeRed);
            } else if (strcmp(tokens[1], "green") == 0){
                strcpy(themeTracker, themeGreen);
            }
        }

        // If user inputs log command handle it here.
        // Print out information on commands that are stored.
        if(strcmp(tokens[0], "log") == 0 && errorFlag == 0){
               for(int j = 0; j < h; j++){
                    printf("%s%s",themeTracker, commandLogs[j].time);
                    printf(" %s%s %d\n",themeTracker ,commandLogs[j].name, commandLogs[j].value);
               }
        }

        // If user input is an environment variable: store into logs depending on input.
        // Else (if not an environment variable) store into logs.
        if(strchr(tokens[0], '$') != NULL){
            char temp3[250]; 
            if(errorFlag == -1){ 
                // If invalid
                char tempName[200];
                size_t len =0 ;
                for(size_t i = 0; i < sizeCounter;i++){
                    len += (size_t)snprintf(tempName + len, (sizeof tempName) - len,"%s ", tokens[i]);    
                }
                tempName[len - 1] = '\0';
                strtok(tempName, "\n");
                commandLogs[h] = new_command(tempName, errorFlag);
            }
            else{
                // If not invalid.
                sprintf(temp3, "%s=%s",varTokens[0],varTokens[1]);
                commandLogs[h] = new_command(temp3, errorFlag);
            }
        }else{ // If not an environment variable.
            commandLogs[h] = new_command(tokens[0], errorFlag);
        }
        ++h;

        // If user inputted exit command handle it here.
        if(strcmp(tokens[0], "exit") == 0 && errorFlag == 0){
            flag = 1;
            printf("%sBye!\n", themeTracker);

            // FREE STRDUP's which can cause mem leak.
            for(int i = 0; i < h; i++){
                free(commandLogs[i].name);
                free(commandLogs[i].time);
            }

            for(int b = 0; b < k; b++){
                free(environVarible[b].value);
                free(environVarible[b].name);
            }

            exit(0);
        }

        // If user entered none built-in command handle it here.
        if(strcmp(tokens[0], "exit") != 0 && strcmp(tokens[0], "log") != 0 && 
            strcmp(tokens[0], "print") != 0 && strcmp(tokens[0], "theme") != 0 && 
            strchr(tokens[0], '$') == NULL && errorFlag == 0)
        {
            int fc = fork(); // Create fork
            if(fc<0){
                printf("fork failed\n");
            }else if(fc == 0){
                // If child, execute exec function with inputted command.

                char* argChild[sizeCounter];
                int y = 0;

                for(y = 0; y < sizeCounter; y++){
                    strtok(tokens[y], "\n");
                    argChild[y] = tokens[y];
                }
                argChild[y] = NULL;
                execvp(argChild[0], argChild);
                exit(1);
            }else{
                //If parent, wait for child to finish.
                wait(NULL);
            }
        }
        sizeCounter = 0; //Reset sizeCounter of tokens for next loop.
    }
    return 0;
}

/*
*   Purpose: This function creates a new Command struct that will be used for logs.
*
*   Explanation: It creates a time struct and along side the name and value parameters it
*               stores all three into a Command struct and returns it. 
*
*   Return: This returns a Command struct.
*/
Command new_command(char *name, int value){
    // Get current local time.
    struct tm *info;
    time_t timeT;
    time ( &timeT );
    info = localtime( &timeT );
    char *timer = asctime(info); 

    // Store into struct.
    Command st;
    st.name = strdup(name);
    st.time = strdup(timer);
    st.value = value;

    // Return struct.
    return st;
}

/*
*   Purpose: This function creates a new environment variable struct that will be used
*           throughout the program (print, logs, etc).
*
*   Explanation: Creates a new environment variable struct and stores parameters passed to it.
*
*   Return: This returns a EnvVar struct.
*/
EnvVar new_enviVar(char *name, char *value){
    EnvVar ev;
    ev.name = strdup(name);
    ev.value = strdup(value);

    return ev;
}

/*
*   Purpose: This function checks to see if the user inputs or textfile inputs are valid or not.
*
*   Explanation: This functions goes through servral check to see if the user inputted a valid
*              command.  If yes return 0, if not valid return -1. Also print error messages.
*   
*   Return: This function returns 0 if valid or -1 if invalid.
*/
int errorCheck(char *token[], int size, EnvVar environ[], int varSize, int arg, char *argg[]){
    int valueRet = 0;

    // If in script mode, check if text file is in wrong format.
    if(arg > 1){
        int scriptCount = 0;
        for(int i = 0; i < size; i++){
            if(strcmp(token[i], "exit") == 0 || strcmp(token[i], "log") == 0 ||
            strcmp(token[i], "print") == 0 || strcmp(token[i], "theme") == 0)
            {
                scriptCount++;
                if(scriptCount > 1){ // If in wrong format exit immediately and print error.
                    printf("Unable to read script file: %s\n", argg[1]);
                    exit(0);
                }
            }
        }
    }

    // Check if user inputted nothing or tabs.
    if(strcmp(token[0], "\n") == 0  || strcmp(token[0], "\r") == 0  || strcmp(token[0], "\t") == 0){
        valueRet = -1;
        printf("Missing keyword or command, or permission problem\n");
        return valueRet;
    }

    // Check if user inputted $ symbol before environment variable.
    if(strchr(token[0], '$') == NULL && strchr(token[0], '=') != NULL){
        valueRet = -1;
        printf("Missing keyword or command, or permission problem\n");
        return valueRet;
    }

  
    // Checks for errors if user inputted an environment variable.
    if(strstr(token[0], "$") != NULL){
        int equalCount = 0;
        int spaceCount = 0;

        char *tempo = token[0];
        
        char tempo2[250];
        if(size > 1){
            char comm4[250];
            // Recreate the inputted string and create 2 tokens for struct.
            size_t len3 = 0;
            for(size_t i = 0; i < size;i++){
                len3 += (size_t)snprintf(comm4 + len3, (sizeof comm4) - len3,"%s ", token[i]);    
            }
            comm4[len3 -1] = '\n';

            strcpy(tempo2, comm4);
        }
        
        // Go through each char of input, count the number of spaces or = signs.
        // If user inputted too many, print error.
        for(int i = 0; i < sizeof(tempo);i++){
            if(tempo[i] == '='){
                equalCount++;
            } else if(tempo[i] == ' '){
                spaceCount++;
            }
            if(tempo[i] == '=' && tempo[i+1] == '\0'){
                spaceCount++;
            }

            if(size > 1 && tempo2[i] == ' ' && tempo2[i+1] == '='){
                spaceCount++;
            }
      
            if(spaceCount > 0){
                valueRet = -1;
                i = sizeof(tempo);
                printf("Missing keyword or command, or permission problem\n");
            }else if(equalCount > 1){
                valueRet = -1;
                i = sizeof(tempo);
                printf("Missing keyword or command, or permission problem\n");
            }
        }
    }

    //  Print errors are handled here.
    if(strcmp(token[0], "print") == 0){
        int envFlag = 0;
        int zb = 0;
        int array[20];
        int r = 0;

        if(size<=1){
            printf("Error: no print arugments detected.\n");
            valueRet = -1;
            return valueRet;
        }

        // Check if user wants to print $var.
        for(int i = 0; i < size; i++){
            if(strstr(token[i], "$") != NULL){
                array[r] = i;
                r++;
            }
        }
        // Check if $var exists or not in structs. If no print error.
        for(zb = 0; zb < r; zb++){
            strtok(token[array[zb]], "\n");
            for(int nn = 0; nn < varSize; nn++){
                if(strcmp(environ[nn].name, token[array[zb]]) == 0){
                    nn = varSize;
                    envFlag = 1;
                }
            }
            if(envFlag == 0){
                strtok(token[array[zb]], "\n");
                printf("Error: No Environment Variable %s found.\n", token[array[zb]]);
                valueRet = -1;
            }
            envFlag = 0;
        }
    }

    // Theme errors are handled here.
    if(strcmp(token[0], "theme") == 0){
        // Correct an input error.
        if(size == 3){
            if(strcmp(token[2], "\n") == 0){
                size--;
            }
        }

        strtok(token[1], "\n"); // Remove newline from token.
       
        // Check if too many or too little arguments inputted.
        // Check if user inputted theme thats not blue, red, or green.
        if(size > 2){
            printf("Error: Too many Arguments detected.\n");
            valueRet = -1;
        } else if(size == 1){
            printf("unsupported theme\n");
            valueRet = -1;
        } else if(strcmp(token[1], "blue") != 0 && strcmp(token[1], "red") != 0 && 
            strcmp(token[1], "green") != 0)
        {
            printf("unsupported theme\n");
            valueRet = -1;
        }
    }

    // Log errors are handled here.
    if(strcmp(token[0], "log") == 0){
        // Correct an input error.
        if(size == 2){
            if(strcmp(token[1], "\n") == 0){
                size--;
            }
        }
        // If user inputted too many arugments print error.
        if(size > 1){
            printf("Error:Too many Arguments detected.\n");
            valueRet = -1;
        }
    }

    // Exit errors are handled here.
    if(strcmp(token[0], "exit") == 0){
        // Correct and input error.
        if(size == 2){
            if(strcmp(token[1], "\n") == 0){
                size--;
            }
        }
        // If too many arguments, print error.
        if(size > 1){
            printf("Error:Too many Arguments detected.\n");
            valueRet = -1;
        }
    }

    // Non-built-in command errors are handled here.
    if(strcmp(token[0], "exit") != 0 && strcmp(token[0], "log") != 0 && 
            strcmp(token[0], "print") != 0 && strcmp(token[0], "theme") != 0 && 
            strchr(token[0], '$') == NULL)
    {
        int errorV = 0;
        char comm[500];
        char comm2[500];

        // Re-build the string from its tokens.
        size_t len = 0;
        for(size_t i = 0; i < size;i++){
            len += (size_t)snprintf(comm2 + len, (sizeof comm2) - len,"%s ", token[i]);    
        }
        comm2[len -1] = '\0';
        strtok(comm2, "\n");
        strcat(comm2, " 2> /dev/null\n"); // Send unwanted terminal shell output to dev/null.
        
        // Open the potential command, and capture close function value.
        FILE *fc = popen(comm2, "r");
        fgets(comm, 500, fc);
        errorV = pclose(fc);

        // If errorV value is not 0 then an error occured.
        if(errorV != 0){
            //Erorr happended
            printf("Missing keyword or command, or permission problem\n");
            valueRet = -1;
        }else{
            //No error command worked.
            valueRet = 0;
        }
    }
    return valueRet; // Return error value.
}
