#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

extern int errno;

/* Merge cu tot cu tokenize 
int main (){
    FILE * f = NULL;
    char * line = NULL;
    size_t length = 0;
    //int count = 1;
    char vect[2];
    char * user;
    char * password;

    f = fopen("/home/leafy/Retele/WeatherMonitor/login","r");
    if (f == NULL) {
        perror ("Eroare la deschiderea fisierului.\n");
        return errno;
    }

    while (( getline(&line, &length, f)) != EOF ) {
        //printf("Line %d : %s", count, line);
        //count+=1;
        user = strtok(line," ");
        password = strtok(NULL," ");
        //printf("Username : %s ", user);
        //printf("Password: %s ", password);
    }
    
    fclose(f);
    if (line)
        free(line);
}
*/

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

int verifUser(char * username){
    FILE * f = NULL;
    char * line = NULL;
    size_t length = 0;
    char * user;

    f = fopen("/home/leafy/Retele/WeatherMonitor/login","r");
    if (f == NULL) {
        perror ("Eroare la deschiderea fisierului.\n");
        return errno;
    }

    while (( getline(&line, &length, f)) != EOF ) {
        user = strtok(line," ");
        if (strcmp(user,username) == 0) return 1;
    }
    fclose(f);
    return 0;
}

int creareCont(char * username, char * password){
    FILE * f = NULL;
    FILE * f2 = NULL;

    f = fopen("/home/leafy/Retele/WeatherMonitor/login","a");
    if (f == NULL) {
        perror ("Eroare la deschiderea fisierului.\n");
        return errno;
    }
    
    if(verifUser(username) == 1) return 0;

    char instr[100] = "touch /home/leafy/Retele/WeatherMonitor/userfiles/";
    strcat(instr,username);
    system(instr);

    fprintf(f,"%s %s\n",username,password);
    fclose(f);
    return 1;
}

int creareCont2(char * username, char * password, char * town){
    FILE * f = NULL;
    FILE * f2 = NULL;

    if(verifUser(username) == 1) return 0;

    f = fopen("/home/leafy/Retele/WeatherMonitor/login","a");
    if (f == NULL) {
        perror ("Eroare la deschiderea fisierului de login.\n");
        return errno;
    }
    
    f2 = fopen("/home/leafy/Retele/WeatherMonitor/Users","a");
    if (f == NULL) {
        perror ("Eroare la deschiderea fisierului cu utilizatori.\n");
        return errno;
    }

    fprintf(f,"%s %s\n", username, password);
    fprintf(f2,"%s::%s ", username, town);

    fclose(f);
    fclose(f2);
    return 1;
}



int main(void){
    char * username = "root";
    char * password = "toor";
    char * username2 = "vasilez2";
    char * password2 = "tuica";
    char * town = "craiova";
    printf("Verif %d \n",verif(username,password));
    //printf("V_u %d \n",verifUser(username));
    printf("V_u %d \n",verifUser(username2));
    //printf("Creare %d \n",creareCont(username2,password2));
    printf("%d\n",verif("root"," "));
    printf("cont : %d \n",creareCont2(username2,password2,town));
    
}

