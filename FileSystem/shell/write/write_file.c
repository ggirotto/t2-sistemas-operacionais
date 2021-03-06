#include <stdio.h>
#include <string.h>
#include "write_file.h"
#include "../../cluster/data_cluster.h"
#include "../utils.h"

/*
    This function write content to a file
    Ex: write "something" folder1/file
    Writes 'something' at file inside folder1
*/
void writeFile(FILE* fatPartition, dir_entry_t* root, uint16_t* fat, uint16_t* address) {
    // Insert user input at directoryPath
    char directoryPath[256];
    fgets(directoryPath, 256, stdin);

    // Create a copy from 'directoryPath' to 'directoryPathContent' to manipulate string content without modify original content
    char directoryPathContent[256];
    snprintf(directoryPathContent, 256, "%s", directoryPath);

    // Get content between "", which the user want to write
    char *delimiter = "\"";
    char *contentToWrite = strtok(directoryPathContent, delimiter);
    contentToWrite = strtok(NULL, delimiter);

    // Remove content between "" from the string.
    char *a = strstr(directoryPath, " ");
    char *b = strrchr(directoryPath, '\"');
    memmove(a-1, b+1, strlen(b)+1);

    char *prev = walkThroughPath(directoryPath, fatPartition, root, address);

    if (prev == NULL) {
        return;
    }

    prev[strlen(prev)-1] = 0;

    dir_entry_t *foundFileToWrite = findEntryNamedAttributed(prev, 1, root);

    if (foundFileToWrite == NULL) {
        printf("Arquivo para escrita não existe\n");
        return;
    } else if (sizeof(contentToWrite) > CLUSTER_SIZE) {
        printf("Conteúdo maior que o do cluster\n");
        return;
    } else {
        fseek(fatPartition, foundFileToWrite->first_block, SEEK_SET);
        fwrite(contentToWrite, strlen(contentToWrite)*sizeof(char), 1, fatPartition);
        foundFileToWrite->size = strlen(contentToWrite);

        updateStructures(fatPartition, root, fat, address);
    }
}
