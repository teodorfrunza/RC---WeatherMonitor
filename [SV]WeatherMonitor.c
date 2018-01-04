#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "port.h"

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

int createAccount(char * username, char * password, char * town){
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

int main (){
    
    struct sockaddr_in server;	// structura folosita de server
    struct sockaddr_in from;	
    char msg[100];		//mesajul primit de la client 
    char msgrasp[100]=" ";        //mesaj de raspuns pentru client
    int sd;			//descriptorul de socket 

    /* crearea unui socket */
    if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1){
        perror ("[server]Eroare la socket().\n");
        return errno;
    }
    
    /* pregatirea structurilor de date */
    bzero (&server, sizeof (server));
    bzero (&from, sizeof (from));

    /* umplem structura folosita de server */
    /* stabilirea familiei de socket-uri */
    server.sin_family = AF_INET;	
    /* acceptam orice adresa */
    server.sin_addr.s_addr = htonl (INADDR_ANY);
    /* utilizam un port utilizator */
    server.sin_port = htons (PORT);

    /* atasam socketul */
    if (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1){
        perror ("[server]Eroare la bind().\n");
        return errno;
    }

    /* punem serverul sa asculte daca vin clienti sa se conecteze */
    if (listen (sd, 5) == -1){
        perror ("[server]Eroare la listen().\n");
        return errno;
    }

    /* servim in mod concurent clientii... */
    while (1){

        int client;
        int length = sizeof (from);
        
        printf ("[server]Asteptam la portul %d...\n",PORT);
        fflush (stdout);

        /*acceptam un client (stare blocanta pina la realizarea conexiunii) */
        client = accept (sd, (struct sockaddr *) &from, &length);
        
        /* eroare la acceptarea conexiunii de la un client */
        if (client < 0){
            perror ("[server]Eroare la accept().\n");
            continue;
        }

        /* FORK */
        if(fork()==0){

            close(sd);                    
            /* s-a realizat conexiunea, se astepta mesajul */
            bzero (msg, 100);
            printf ("[server]Asteptam mesajul...\n");
            fflush (stdout);
          
            /* citirea mesajului */
            if (read (client, msg, 100) <= 0){
	            perror ("[server]Eroare la read() de la client.\n");
	            close (client);	/* inchidem conexiunea cu clientul */
	            continue;		/* continuam sa ascultam */
	        }

            printf ("[server]Mesajul a fost receptionat...%s\n", msg);        

            if (msg[0] == '1'){

                /*pregatim mesajul de raspuns */
                bzero(msgrasp,100);
                strcat(msgrasp,"[server]Please insert your username and password separated by a space (ex:asd 1234): ");

                if (write (client, msgrasp, 100) <= 0){
	                perror ("[server]Eroare la write() catre client.\n");
	                continue;		/* continuam sa ascultam */
                }
                else printf ("[server]Mesajul a fost trasmis cu succes.\n");
            
                bzero(msgrasp,100);    
                bzero (msg, 100);
                /* citirea mesajului */
                if (read (client, msg, 100) <= 0){
	                perror ("[server]Eroare la read() de la client.\n");
	                close (client);	/* inchidem conexiunea cu clientul */
	                continue;		/* continuam sa ascultam */
	            }

                printf ("[server]Mesajul a fost receptionat...%s\n", msg);

                char * user;
                char * pass;
                
                user = strtok(msg," ");
                pass = strtok(NULL," \n");
                
                printf("[server] user : %d\n",verif(user,pass));
                printf("[server] pass : %s\n",pass);
                if(verif(user,pass) == 1){
                    bzero(msgrasp,100);
                    strcat(msgrasp,"[server]Logged in successfully!\n");
                    strcat(msgrasp,"[server]***MENU***\n");
                    strcat(msgrasp,"[server]1.Add town\n");
                    strcat(msgrasp,"[server]2.Remove town\n");
                    strcat(msgrasp,"[server]3.Exit\n");
                }
                else{
                    bzero(msgrasp,100);
                    strcat(msgrasp,"[server]Wrong user or password!!! Closing connection.\n");
                }


            }
            else if(msg[0] == '2'){

                /*pregatim mesajul de raspuns */
                bzero(msgrasp,100);
                strcat(msgrasp,"[server]Please insert an username and a password that does not contain uppercase or space: ");

                if (write (client, msgrasp, 100) <= 0){
	                perror ("[server]Eroare la write() catre client.\n");
	                continue;		/* continuam sa ascultam */
                }
                else printf ("[server]Mesajul a fost trasmis cu succes. %s\n",msgrasp);

                bzero(msgrasp,100);    
                bzero (msg, 100);
                /* citirea mesajului */
                if (read (client, msg, 100) <= 0){
	                perror ("[server]Eroare la read() de la client.\n");
	                close (client);	/* inchidem conexiunea cu clientul */
	                continue;		/* continuam sa ascultam */
	            }

                printf ("[server]Mesajul a fost receptionat...%s\n", msg);

                char * user;
                char * pass;
                
                user = strtok(msg," ");
                pass = strtok(NULL," \n");

                bzero(msgrasp,100);
                strcat(msgrasp,"[server]Please insert the default town with lowercase: ");

                if (write (client, msgrasp, 100) <= 0){
	                perror ("[server]Eroare la write() catre client.\n");
	                continue;		/* continuam sa ascultam */
                }
                else printf ("[server]Mesajul a fost trasmis cu succes.\n");


                bzero(msgrasp,100);    
                bzero (msg, 100);
                /* citirea mesajului */
                if (read (client, msg, 100) <= 0){
	                perror ("[server]Eroare la read() de la client.\n");
	                close (client);	/* inchidem conexiunea cu clientul */
	                continue;		/* continuam sa ascultam */
	            }

                printf ("[server]Mesajul a fost receptionat...%s\n", msg);


                char * town = msg;

                if(createAccount(user,pass,town) == 1){
                    printf("OK");
                }
                else printf("NO");




            }




            if (write (client, msgrasp, 100) <= 0){
	                perror ("[server]Eroare la write() catre client.\n");
	                continue;		/* continuam sa ascultam */
	        }
            else printf ("[server]Mesajul a fost trasmis cu succes.\n");



            
	
          

            /*
            bzero(msgrasp,100);
            strcat(msgrasp,"Hello ");
            strcat(msgrasp,msg);
          
            printf("[server]Trimitem mesajul inapoi...%s\n",msgrasp);
          
          
            
            if (write (client, msgrasp, 100) <= 0){
	            perror ("[server]Eroare la write() catre client.\n");
	            continue;		
	        }
            else printf ("[server]Mesajul a fost trasmis cu succes.\n");*/

            /* am terminat cu acest client, inchidem conexiunea */
            close (client);

            exit(0); //ADAUGARE PT CONCURENT
        }
    close(client);
    }
}
