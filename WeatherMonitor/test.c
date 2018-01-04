#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define PORT 2031

extern int errno;

int verif(char * username, char * password){
    FILE * f = NULL;
    char * line = NULL;
    size_t length = 0;
    int count = 1;
    char vect[2];
    char * user;
    char * pass;

    f = fopen("/home/leafy/Retele/WeatherMonitor/login","r");
    if (f == NULL) {
        perror ("Eroare la deschiderea fisierului.\n");
        return errno;
    }

    while (( getline(&line, &length, f)) != EOF ) {
        user = strtok(line," ");
        pass = strtok(NULL," ");
        if (strcmp(user,username) == 0 && strcmp(pass, password) == 10) return 1;
        /* VERIFICARI
         * printf("Line %d : %s", count, line);
         * count+=1;
         * printf("Username : %s ", user);
         * printf("Password: %s ", pass);
         * printf("l_u, %zu , l_p , %zu",strlen(user),strlen(pass));
         * printf("Verif : %d %d \n",strcmp(user,username),strcmp(pass, password));
         */
    }
    fclose(f);
    return 0;
}


int main(){
    int i;
    char c[3][10] = { "iasi","craiova","dolj"};
    for(i=0;i<3;i++){
        printf("%s",strcmp(c+i,"iasi"));
    
    }
}
