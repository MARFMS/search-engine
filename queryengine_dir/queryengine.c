/*

FILE: queryengine.c
By: Delos Chang

Description: a command-line processing engine that asks users for input
and creates a ranking from the crawler and indexer to display to the user

Inputs: ./queryengine [TARGET INDEXER FILENAME] [RESULTS FILE NAME]
  AND / OR operators for command-line processing
  - a space (" ") represents an 'AND' operator
  - a capital OR represents an 'OR' operator

Outputs: The query engine will output a ranking based on the queries that the 
user enters

Design Spec: 

Implementation Spec Pseudocode: 
  1. Validate user input arguments
  2. Load the indexer's index file into memory
  3. Query the user via the command line
  4. Change the capital letters to lower case letters
  5. Cross-reference the query with the index and retrieve results
  6. Rank results via an algorithm based on word frequency with AND / OR operators
*/

#include <stdio.h>
#include <stdlib.h>
/*#include <string.h>*/

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>

/*#include "indexer.h"*/
/*#include "hash.h"*/
/*#include "file.h"*/
#include "../util/index.h"

INVERTED_INDEX* indexReload = NULL;

// this function prints generic usage information 
void printUsage(){
    printf("Usage: ./queryengine ../indexer_dir/index.dat ../../data \n");
}

void validateArgs(int argc, char* argv[]){
  // struct for checking whether directory exists
  struct stat s;
  struct stat checkDest;

  // test URL with default correct exit status
  int readableResult = 0;
  char* readableTest = NULL; // dynamically allocate to prevent overflow

  // check for correct number of parameters first
  if ( (argc != 3) ){
    fprintf(stderr, "Error: insufficient arguments. 3 required. You provided %d \n", argc);
    printUsage();

    exit(1);
  }

  // Validate that file exists
  if ( stat(argv[1], &s) != 0){
    fprintf(stderr, "Error: The file argument %s was not found.  Please \
    enter a readable and valid file. \n", argv[1]);
    printUsage();

    exit(1);
  }

  // Warn user that the destination file will be overwritten
  if ( stat(argv[2], &checkDest) == 0){
    fprintf(stderr, "Warning: The file %s already exists and will be overwritten \n", argv[2]);
  }

  // Validate that file is readable
  // Allocate memory to prevent overflow
  size_t len1 = strlen("if [[ -r "), len2 = strlen(argv[1]), len3 = strlen(" ]]; then exit 0; else exit 1; fi");
  readableTest = (char*) malloc(len1 + len2 + len3 + 1);

  if (readableTest == NULL){
    fprintf(stderr, "Out of memory! \n ");
    exit(1);
  }

  memcpy(readableTest, "if [[ -r ", len1);
  memcpy(readableTest+len1, argv[1], len2);
  strcat(readableTest, " ]]; then exit 0; else exit 1; fi");

  readableResult = system(readableTest);

  // check the exit status
  if ( readableResult != 0){
    fprintf(stderr, "Error: The file argument %s was not readable. \
    Please enter readable and valid file. \n", argv[1]);
    printUsage();

    free(readableTest);
    exit(1);
  }
  free(readableTest);
}

int main(int argc, char* argv[]){
  char* targetDir;
  char* targetFile;
  int numOfFiles;

  // (1) Validate the parameters
  validateArgs(argc, argv);

  // (2) Initialize the inverted index
  if (initStructures() != 1){
    fprintf(stderr, "Could not initialize data structures. Exiting \n");
    exit(1);
  }

  // NORMAL CREATE-INDEX MODE
    // (2) Grab number of files in target dir to loop through
    targetDir = argv[1]; // set the directory
    targetFile = argv[2]; // set the new file to be written to 
    numOfFiles = dirScan(targetDir); 


    // (4) Loop through files to build index
    buildIndexFromDir(targetDir, numOfFiles, index);
    LOG("Index finished building");

    // (5) Save the index to a file (sorted)
    int saveResult = saveIndexToFile(index, targetFile);
    if (saveResult != 1){
      fprintf(stderr, "Could not save index to file! \n");
      exit(1);
    } else {
      LOG("Writing index to file finished");
    }

    // clean up here
    cleanupIndex(index);

  // DEBUG MODE: reloading the index file
  if ( argc == 5){
    validateDebugArgs(argv[3], argv[4]);
    LOG("Testing index");

    char* loadFile = argv[3];
    char* writeReload = argv[4];

    // will reload the index from the saved file (e.g. from index.dat)
    if (initReloadStructure() != 1){
      fprintf(stderr, "Could not initialize data structures. Exiting");
      exit(1);
    }

    /*int reloadResult = reloadIndexFromFile(loadFile, writeReload);*/
    /*if (reloadResult != 1){*/
      /*fprintf(stderr, "Could not reload the index from the file! \n");*/
      /*exit(1);*/
    /*} else {*/
      /*LOG("Reloading index from file finished");*/
    /*}*/

    // clean up here;
    LOG("Cleaning up");
    /*cleanupIndex(indexReload);*/

    // BATS.sh will test the integrity of the reloaded index.
  }
  return 0;
}