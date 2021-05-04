#include "console.h"
#include "records.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <signal.h>
#include <errno.h>

#include<sys/wait.h>


#include "citizens.h"
#include "countries.h"


extern int size_in_bytes;   //-s
int bufferSize;             //-b


int sent_message(int fd, char *message);



int main( int argc, char *argv[]){


    //CHECK ARGUMENTS
    if ( argc != 9 ){
        printf("Wrong number of arguments!\n");
        return -1;
    }


    int numMonitors;    //-m
    char *input_dir;    //-i

    

    //check correctness of arguments
    if ( (strcmp(argv[1], "-m") == 0) &&  (strcmp(argv[3], "-b") == 0) 
        &&  (strcmp(argv[5], "-s") == 0)  &&  (strcmp(argv[7], "-i") == 0)){      //-m, -b, -s, -i

        numMonitors = atoi(argv[2]);
        bufferSize = atoi(argv[4]);
        size_in_bytes = atoi(argv[6]);
        input_dir = malloc( sizeof(char) * (strlen(argv[8]) + 1) );
        strcpy(input_dir,argv[8]);
        

    } else if ( (strcmp(argv[1], "-m") == 0) &&  (strcmp(argv[3], "-b") == 0) 
        &&  (strcmp(argv[5], "-i") == 0)  &&  (strcmp(argv[7], "-s") == 0) ){  //-m, -b, -i, -s
        
        numMonitors = atoi(argv[2]);
        bufferSize = atoi(argv[4]);
        size_in_bytes = atoi(argv[8]);
        input_dir = malloc( sizeof(char) * (strlen(argv[6]) + 1) );
        strcpy(input_dir,argv[6]);

    } else if ( (strcmp(argv[1], "-m") == 0) &&  (strcmp(argv[3], "-s") == 0) 
        &&  (strcmp(argv[5], "-b") == 0)  &&  (strcmp(argv[7], "-i") == 0) ){   //-m, -s, -b, -i

        numMonitors = atoi(argv[2]);
        bufferSize = atoi(argv[6]);
        size_in_bytes = atoi(argv[4]);
        input_dir = malloc( sizeof(char) * (strlen(argv[8]) + 1) );
        strcpy(input_dir,argv[8]);

    } else if ((strcmp(argv[1], "-m") == 0) &&  (strcmp(argv[3], "-s") == 0) 
        &&  (strcmp(argv[5], "-i") == 0)  &&  (strcmp(argv[7], "-b") == 0)){    //-m, -s, -i, -b

        numMonitors = atoi(argv[2]);
        bufferSize = atoi(argv[8]);
        size_in_bytes = atoi(argv[4]);
        input_dir = malloc( sizeof(char) * (strlen(argv[6]) + 1) );
        strcpy(input_dir,argv[6]);

    } else if ( (strcmp(argv[1], "-m") == 0) &&  (strcmp(argv[3], "-i") == 0) 
        &&  (strcmp(argv[5], "-b") == 0)  &&  (strcmp(argv[7], "-s") == 0) ){   //-m, -i, -b, -s

        numMonitors = atoi(argv[2]);
        bufferSize = atoi(argv[6]);
        size_in_bytes = atoi(argv[8]);
        input_dir = malloc( sizeof(char) * (strlen(argv[4]) + 1) );
        strcpy(input_dir,argv[4]);


    } else if ((strcmp(argv[1], "-m") == 0) &&  (strcmp(argv[3], "-i") == 0) 
        &&  (strcmp(argv[5], "-s") == 0)  &&  (strcmp(argv[7], "-b") == 0)){    //-m, -i, -s, -b

        numMonitors = atoi(argv[2]);
        bufferSize = atoi(argv[8]);
        size_in_bytes = atoi(argv[6]);
        input_dir = malloc( sizeof(char) * (strlen(argv[4]) + 1) );
        strcpy(input_dir,argv[4]);


    } else if ( (strcmp(argv[1], "-b") == 0) &&  (strcmp(argv[3], "-m") == 0) 
        &&  (strcmp(argv[5], "-s") == 0)  &&  (strcmp(argv[7], "-i") == 0) ){   //-b, -m, -s, -i

        numMonitors = atoi(argv[4]);
        bufferSize = atoi(argv[2]);
        size_in_bytes = atoi(argv[6]);
        input_dir = malloc( sizeof(char) * (strlen(argv[8]) + 1) );
        strcpy(input_dir,argv[8]);


    } else if ( (strcmp(argv[1], "-b") == 0) &&  (strcmp(argv[3], "-m") == 0) 
        &&  (strcmp(argv[5], "-i") == 0)  &&  (strcmp(argv[7], "-s") == 0) ){   //-b, -m, -i, -s

        numMonitors = atoi(argv[4]);
        bufferSize = atoi(argv[2]);
        size_in_bytes = atoi(argv[8]);
        input_dir = malloc( sizeof(char) * (strlen(argv[6]) + 1) );
        strcpy(input_dir,argv[6]);

    } else if ( (strcmp(argv[1], "-b") == 0) &&  (strcmp(argv[3], "-i") == 0) 
        &&  (strcmp(argv[5], "-s") == 0)  &&  (strcmp(argv[7], "-m") == 0) ){   //-b, -i, -s, -m

        numMonitors = atoi(argv[8]);
        bufferSize = atoi(argv[2]);
        size_in_bytes = atoi(argv[6]);
        input_dir = malloc( sizeof(char) * (strlen(argv[4]) + 1) );
        strcpy(input_dir,argv[4]);

    } else if ( (strcmp(argv[1], "-b") == 0) &&  (strcmp(argv[3], "-i") == 0) 
        &&  (strcmp(argv[5], "-m") == 0)  &&  (strcmp(argv[7], "-s") == 0) ){   //-b, -i, -m, -s

        numMonitors = atoi(argv[6]);
        bufferSize = atoi(argv[2]);
        size_in_bytes = atoi(argv[8]);
        input_dir = malloc( sizeof(char) * (strlen(argv[4]) + 1) );
        strcpy(input_dir,argv[4]);

    } else if ( (strcmp(argv[1], "-b") == 0) &&  (strcmp(argv[3], "-s") == 0) 
        &&  (strcmp(argv[5], "-i") == 0)  &&  (strcmp(argv[7], "-m") == 0) ){   //-b, -s, -i, -m

        numMonitors = atoi(argv[8]);
        bufferSize = atoi(argv[2]);
        size_in_bytes = atoi(argv[4]);
        input_dir = malloc( sizeof(char) * (strlen(argv[6]) + 1) );
        strcpy(input_dir,argv[6]);

    } else if ( (strcmp(argv[1], "-b") == 0) &&  (strcmp(argv[3], "-s") == 0) 
        &&  (strcmp(argv[5], "-m") == 0)  &&  (strcmp(argv[7], "-i") == 0) ){   //-b, -s, -m, -i
        
        numMonitors = atoi(argv[6]);
        bufferSize = atoi(argv[2]);
        size_in_bytes = atoi(argv[4]);
        input_dir = malloc( sizeof(char) * (strlen(argv[8]) + 1) );
        strcpy(input_dir,argv[8]);

    } else if ( (strcmp(argv[1], "-s") == 0) &&  (strcmp(argv[3], "-b") == 0) 
        &&  (strcmp(argv[5], "-m") == 0)  &&  (strcmp(argv[7], "-i") == 0) ){   //-s, -b, -m, -i
        
        numMonitors = atoi(argv[6]);
        bufferSize = atoi(argv[4]);
        size_in_bytes = atoi(argv[2]);
        input_dir = malloc( sizeof(char) * (strlen(argv[8]) + 1) );
        strcpy(input_dir,argv[8]);

    } else if ( (strcmp(argv[1], "-s") == 0) &&  (strcmp(argv[3], "-b") == 0) 
        &&  (strcmp(argv[5], "-i") == 0)  &&  (strcmp(argv[7], "-m") == 0) ){   //-s, -b, -i, -m
        
        numMonitors = atoi(argv[8]);
        bufferSize = atoi(argv[4]);
        size_in_bytes = atoi(argv[2]);
        input_dir = malloc( sizeof(char) * (strlen(argv[6]) + 1) );
        strcpy(input_dir,argv[6]);

    } else if ( (strcmp(argv[1], "-s") == 0) &&  (strcmp(argv[3], "-m") == 0) 
        &&  (strcmp(argv[5], "-b") == 0)  &&  (strcmp(argv[7], "-i") == 0) ){   //-s, -m, -b, -i
        
        numMonitors = atoi(argv[4]);
        bufferSize = atoi(argv[6]);
        size_in_bytes = atoi(argv[2]);
        input_dir = malloc( sizeof(char) * (strlen(argv[8]) + 1) );
        strcpy(input_dir,argv[8]);

    } else if ( (strcmp(argv[1], "-s") == 0) &&  (strcmp(argv[3], "-m") == 0) 
        &&  (strcmp(argv[5], "-i") == 0)  &&  (strcmp(argv[7], "-b") == 0) ){   //-s, -m, -i, -b
        
        numMonitors = atoi(argv[4]);
        bufferSize = atoi(argv[8]);
        size_in_bytes = atoi(argv[2]);
        input_dir = malloc( sizeof(char) * (strlen(argv[6]) + 1) );
        strcpy(input_dir,argv[6]);

    } else if ( (strcmp(argv[1], "-s") == 0) &&  (strcmp(argv[3], "-i") == 0) 
        &&  (strcmp(argv[5], "-m") == 0)  &&  (strcmp(argv[7], "-b") == 0) ){   //-s, -i, -m, -b
        
        numMonitors = atoi(argv[6]);
        bufferSize = atoi(argv[8]);
        size_in_bytes = atoi(argv[2]);
        input_dir = malloc( sizeof(char) * (strlen(argv[4]) + 1) );
        strcpy(input_dir,argv[4]);

    } else if ( (strcmp(argv[1], "-s") == 0) &&  (strcmp(argv[3], "-i") == 0) 
        &&  (strcmp(argv[5], "-b") == 0)  &&  (strcmp(argv[7], "-m") == 0) ){   //-s, -i, -b, -m
        
        numMonitors = atoi(argv[8]);
        bufferSize = atoi(argv[6]);
        size_in_bytes = atoi(argv[2]);
        input_dir = malloc( sizeof(char) * (strlen(argv[4]) + 1) );
        strcpy(input_dir,argv[4]);

    } else if ( (strcmp(argv[1], "-i") == 0) &&  (strcmp(argv[3], "-s") == 0) 
        &&  (strcmp(argv[5], "-m") == 0)  &&  (strcmp(argv[7], "-b") == 0) ){   //-i, -s, -m, -b
        
        numMonitors = atoi(argv[6]);
        bufferSize = atoi(argv[2]);
        size_in_bytes = atoi(argv[8]);
        input_dir = malloc( sizeof(char) * (strlen(argv[2]) + 1) );
        strcpy(input_dir,argv[2]);

    } else if ( (strcmp(argv[1], "-i") == 0) &&  (strcmp(argv[3], "-s") == 0) 
        &&  (strcmp(argv[5], "-b") == 0)  &&  (strcmp(argv[7], "-m") == 0) ){   //-i, -s, -b, -m
        
        numMonitors = atoi(argv[8]);
        bufferSize = atoi(argv[6]);
        size_in_bytes = atoi(argv[4]);
        input_dir = malloc( sizeof(char) * (strlen(argv[2]) + 1) );
        strcpy(input_dir,argv[2]);

    } else if ( (strcmp(argv[1], "-i") == 0) &&  (strcmp(argv[3], "-b") == 0) 
        &&  (strcmp(argv[5], "-m") == 0)  &&  (strcmp(argv[7], "-s") == 0) ){   //-i, -b, -m, -s
        
        numMonitors = atoi(argv[6]);
        bufferSize = atoi(argv[4]);
        size_in_bytes = atoi(argv[8]);
        input_dir = malloc( sizeof(char) * (strlen(argv[2]) + 1) );
        strcpy(input_dir,argv[2]);

    } else if ( (strcmp(argv[1], "-i") == 0) &&  (strcmp(argv[3], "-b") == 0) 
        &&  (strcmp(argv[5], "-s") == 0)  &&  (strcmp(argv[7], "-m") == 0) ){   //-i, -b, -s, -m
        
        numMonitors = atoi(argv[8]);
        bufferSize = atoi(argv[4]);
        size_in_bytes = atoi(argv[6]);
        input_dir = malloc( sizeof(char) * (strlen(argv[2]) + 1) );
        strcpy(input_dir,argv[2]);

    } else if ( (strcmp(argv[1], "-i") == 0) &&  (strcmp(argv[3], "-m") == 0) 
        &&  (strcmp(argv[5], "-s") == 0)  &&  (strcmp(argv[7], "-b") == 0) ){   //-i, -m, -s, -b
        
        numMonitors = atoi(argv[4]);
        bufferSize = atoi(argv[8]);
        size_in_bytes = atoi(argv[6]);
        input_dir = malloc( sizeof(char) * (strlen(argv[2]) + 1) );
        strcpy(input_dir,argv[2]);

    } else if ( (strcmp(argv[1], "-i") == 0) &&  (strcmp(argv[3], "-m") == 0) 
        &&  (strcmp(argv[5], "-b") == 0)  &&  (strcmp(argv[7], "-s") == 0) ){   //-i, -m, -b, -s
        
        numMonitors = atoi(argv[4]);
        bufferSize = atoi(argv[6]);
        size_in_bytes = atoi(argv[8]);
        input_dir = malloc( sizeof(char) * (strlen(argv[2]) + 1) );
        strcpy(input_dir,argv[2]);

    } else {

        printf("Wrong Input!\n");
        return -1;
    }


    printf("Size of Bloom: %d\n", size_in_bytes);
    printf("Num of Monitors: %d\n", numMonitors);
    printf("BufferSize: %d\n", bufferSize);
    printf("Directory for Input: %s\n", input_dir);

    

    //List of viruses
    struct List* virus_list = NULL;

    //Hashtable of citizens
    Hashtable citizens;
    citizens = hashtable_create();

    //Hashtable of Countries
    CountryHash countries;
    countries = hashtable_createCoun();


    //READ FILES
    DIR *maindr, *subdr;
    struct dirent *dsub_dir, *dfiles;


    //open directory input_dir
    if ( (maindr = opendir(input_dir)) == NULL ) { perror("Directory cannot open!\n"); return 1; }


    //for every subdirectory
    while ( (dsub_dir = readdir(maindr)) != NULL ){

        printf("%s\n", dsub_dir->d_name);

        if ( (strcmp(dsub_dir->d_name,".") != 0) && (strcmp(dsub_dir->d_name,"..") != 0) ){

            //put country in hashtable
            hashtable_addCoun(countries, dsub_dir->d_name);

            printf("    Opening Directory: %s\n", dsub_dir->d_name);

            char *path;
            path = malloc(sizeof(char) * ( strlen(dsub_dir->d_name) + strlen(input_dir) + 2 ) );
            strcpy(path, input_dir);
            strcat(path, "/");
            strcat(path, dsub_dir->d_name);

            //printf("diretory to open: %s\n", path);

            //open sub-directory
            if ( (subdr = opendir(path)) == NULL ) { perror("Sub-Directory cannot open!"); return 1; }

                
                //for every file in the sub-directory
                while ( (dfiles = readdir(subdr)) != NULL ){

                    if ( (strcmp(dfiles->d_name,".") != 0) && (strcmp(dfiles->d_name,"..") != 0) ){

                        printf("        File: %s\n", dfiles->d_name);

                        char *filepath;
                        filepath = malloc(sizeof(char) * ( strlen(dfiles->d_name) + strlen(path) + 2 ) );         //+1 for the character '/'
                        strcpy(filepath, path);
                        strcat(filepath, "/");
                        strcat(filepath, dfiles->d_name);

                        //printf("file to open: %s\n", filepath);


                        if ( read_file(filepath, citizens, &virus_list, countries, dsub_dir->d_name) == 0 ){
                            printf("Successful reading of file %s!\n", dfiles->d_name);
                        }

                        free(filepath);

                    }


                }



            closedir(subdr);

            free(path);

        }



    }

    


    //Δημιουργία named pipes
    char *gen_pname = "pipes/fifo";
    char *ext_pname;
    char num_pname[5];

    int fd[numMonitors];

    for (int i=0; i<numMonitors; i++){


        //BUILD PIPE NAME
        sprintf(num_pname, "%d", i);        //convert counter to string
        ext_pname = malloc( sizeof(char) * ( strlen(gen_pname) + strlen(num_pname) + 1) );
        strcat(ext_pname, gen_pname);
        strcat(ext_pname, num_pname);       //add counter at the end of string
        printf("Name: %s\n", ext_pname);



        //CREATE NAMED PIPE 
        if ( mkfifo( ext_pname, 0666) == -1){

            if ( errno != EEXIST ){
                perror("ERROR: creating Named Pipe");
                exit(6);
            }
        }


        fd[i] = open(ext_pname, O_WRONLY | O_NONBLOCK);

        //sent_message(fd[i], ext_pname);



//        char bufSize_string[10];
//        sprintf(bufSize_string, "%d", bufferSize);

        //sent_message(fd[i], bufSize_string);



        //FORK CHILD PROCESS
        int pid = fork();

        if ( pid == -1 ){
            perror("ERROR: in fork");
            exit(1);
        }

        if ( pid == 0 ){
            //this is a child process

            char *args[] = {"./monitor", ext_pname, NULL};

            //execute Monitor Process
            if (execv(args[0], args)){
                perror("ERROR: in exec");
                exit(1);
            }
        }


        free(ext_pname);

    }

   
    //το parent process μοιράζει τα subdirectories σε κάθε Monitor μέσω named_pipes
    /*
    int i = 0; //for choosing Monitor

    char example[]= "example";

    while ( i < numMonitors ){    //for all subdirectories


        //get name of subdirectory
        sent_message(fd[i], example);

        i = (i + 1) % numMonitors;

    }

    */







    //PRINT STRUCTURES
    /*printf("\n\nPRINT STRUCTURES:\n");
    print_hashtable(citizens);
    printlist(virus_list);
    print_hashtableCoun(countries);
    */
    /*
    //SIMULATION
    printf("\n\nSIMULATION:\n");
    console(&virus_list, citizens, countries);
    */
   
    //PRINT STRUCTURES
    /*printf("\n\nPRINT STRUCTURES:\n");
    print_hashtable(citizens);
    printlist(virus_list);
    print_hashtableCoun(countries);*/





    for (int i=0; i<numMonitors; i++) close(fd[i]);

    
    //while(wait(NULL) > 0);

    //RELEASE OF MEMORY
    printf("\nRELEASE OF MEMORY.\n");
    hashtable_destroyCoun(countries);
    hashtable_destroy(citizens);
    deletelist(&virus_list);
    free(citizens);
    free(countries);
    citizens = NULL;
    virus_list = NULL;
/*    free(input_dir);*/
    return 0;
}






int sent_message(int fd, char *message){


       
    int total_len = strlen(message)+1;
    int remainder_len = total_len;        //length of string still to be sent
    char remainder[bufferSize];
    
    int i=0;
    while ( remainder_len > 0){

        //printf("Write no %d\n", i);
        memcpy(remainder, message + total_len - remainder_len, remainder_len);
        
        if ( remainder_len < bufferSize ){

            for (int i=remainder_len; i<bufferSize; i++){
                remainder[i] = '\0';
            }
        }

        if ( write(fd, remainder, 12) == -1 ){
            perror("ERROR: in writing in Named Pipe");
            return -1;
        }
        //printf("Write no %d END\n", i);
        remainder_len = remainder_len - bufferSize;
        i++;
    }

    
    return 0;

}