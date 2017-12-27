#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>

/* codul de eroare returnat de anumite apeluri */
extern int errno;

/* portul de conectare la server*/
int port;

int main (int argc, char *argv[]){

    int sd;			            // descriptorul de socket
    struct sockaddr_in server;	// structura folosita pentru conectare 
    char msg[100];		        // mesajul trimis

    /* exista toate argumentele in linia de comanda? */
    if (argc != 3){
        printf ("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
        return -1;
    }

    /* stabilim portul */
    port = atoi (argv[2]);

    /* cream socketul */
    if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1){
        perror ("Eroare la socket().\n");
        return errno;
    }

    /* umplem structura folosita pentru realizarea conexiunii cu serverul */
    /* familia socket-ului */
    server.sin_family = AF_INET;
    /* adresa IP a serverului */
    server.sin_addr.s_addr = inet_addr(argv[1]);
    /* portul de conectare */
    server.sin_port = htons (port);

    /* ne conectam la server */
    if (connect (sd, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1){
        perror ("[client]Eroare la connect().\n");
        return errno;
    }

    /* citirea mesajului -> LOGIN*/
    bzero (msg, 100);
    printf ("[client]Welcome to WeatherMonitor!\n");
    printf("[client]***MENU***\n");
    printf("[client]1.Log in\n");
    printf("[client]2.Register\n");
    printf("[client]Please select one option of the above: ");
    fflush (stdout);
    read (0, msg, 100);

    while(msg[0] != '1' && msg[0] != '2'){
        printf("[client]Please select one option of the above: ");
        fflush (stdout);
        bzero (msg, 100);
        read (0, msg, 100);
    }
  
    /* trimiterea mesajului la server */
    if (write (sd, msg, 100) <= 0){
        perror ("[client]Eroare la write() spre server.\n");
        return errno;
    }

    if (read (sd, msg, 100) < 0){
        perror ("[client]Eroare la read() de la server.\n");
        return errno;
    }
    /* afisam mesajul primit */

    if(strcmp(msg,"[server]Please insert your username and password separated by a space (ex:asd 1234): ") == 0){
        printf ("%s", msg);
        fflush(stdout);

        int count = 0;
        bzero (msg, 100);
        read(0, msg, 100);

        for(int i=0;i<strlen(msg);i++)
            if(msg[i] == ' ') count++;
        
        while (count == 0 || count >=2){
            count = 0;
            printf("[client]You have entered a wrong number of arguments! Please retry: ");
            fflush(stdout);

            bzero (msg, 100);
            read(0, msg, 100);
            for(int i=0;i<strlen(msg);i++)
                if(msg[i] == ' ') count++;
        }
        
        if (write (sd, msg, 100) <= 0){
            perror ("[client]Eroare la write() spre server.\n");
            return errno;
        }

    }

    system("clear");
    bzero (msg, 100);
    if (read (sd, msg, 100) < 0){
        perror ("[client]Eroare la read() de la server.\n");
        return errno;
    }
    printf("%s",msg);
    fflush(stdout);

    /* inchidem conexiunea, am terminat */
    close (sd);

}