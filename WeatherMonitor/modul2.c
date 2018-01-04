#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int afisare(char * username){
    char path[100];
    FILE *f;
    char * buffer;
    char * token;
    int filesize = 0;
    size_t size;

    strcat(path,"/home/leafy/Retele/WeatherMonitor/userfiles/");
    strcat(path, username);

    f = fopen(path,"r");
    
    fseek(f,0,SEEK_END);
    filesize = ftell(f);

    buffer = malloc(filesize+1);

    fseek(f,0,SEEK_SET);

    size = fread(buffer,1,filesize,f);
    buffer[size] = 0;

    printf("%s\n\n",buffer);
    
    fclose(f);
    
    
    token = strtok(buffer,"&");
    printf("%s",token);
    
    while ((token = strtok(NULL,"&"))!=NULL) 
        printf("%s\n",token);
    
}

int main(){
    char * username = "vasile";
    afisare(username);
}
