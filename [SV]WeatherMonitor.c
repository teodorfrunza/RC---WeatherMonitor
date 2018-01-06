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

int createAccount(char * username, char * password){
    FILE * f = NULL;
    FILE * f2 = NULL;

    if(verifUser(username) == 1 || username == NULL || password == NULL) return 0;
    else{
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

        char instr[100] = "touch /home/leafy/Retele/WeatherMonitor/userfiles/";
        strcat(instr,username);
        system(instr);
            

        fprintf(f,"%s %s\n", username, password);
        //fprintf(f2,"%s:: \n", username);

        fclose(f);
        fclose(f2);
        return 1;
    }
}

int addTown(char * user, char * town){

    FILE * f = NULL;
    FILE * f2 = NULL;
    char * line = NULL;
    char * line2 = NULL;
    size_t length = 0;
    size_t length2 = 0;
    char path[100];
    int flag1 = 0, flag2 = 0;

    bzero(path,100);
    strcat(path,"/home/leafy/Retele/WeatherMonitor/userfiles/");
    strcat(path,user);

    f = fopen(path,"r+");
    if (f == NULL) {
        perror ("Eroare la deschiderea fisierului.\n");
        return errno;
    }

    f2 = fopen("/home/leafy/Retele/WeatherMonitor/cities/index","r");
    if (f2 == NULL) {
        perror ("Eroare la deschiderea fisierului.\n");
        return errno;
    }

    while (( getline(&line, &length, f)) != EOF ) {
        line[strcspn(line, "\r\n")] = 0;
        if(strcmp(town,line)==0) flag1 = 1;
    }

    while (( getline(&line2, &length2, f2)) != EOF ) {
        line2[strcspn(line2, "\r\n")] = 0;
        if(strcmp(town,line2)==0) flag2 = 1;
    }

    if(flag2 == 0) return 0;

    if(flag1 == 0) fprintf(f,"%s\n",town);

    fclose(f);
    fclose(f2);
    return 1;
}

int main (){
    
    struct sockaddr_in server;	// structura folosita de server
    struct sockaddr_in from;	
    char msg[100];		//mesajul primit de la client 
    char msgrasp[100]=" ";        //mesaj de raspuns pentru client
    char bigmsgrasp[200]=" ";
    int sd;			//descriptorul de socket 


    //CREAREA MENIURILOR

    //MENIU DUPA LOGIN

    bzero(bigmsgrasp,200);
    strcat(bigmsgrasp,"[server]Logged in successfully!\n");
    strcat(bigmsgrasp,"[server]***MENU***\n");
    strcat(bigmsgrasp,"[server]1.Show weather\n");
    strcat(bigmsgrasp,"[server]2.Add town\n");
    strcat(bigmsgrasp,"[server]3.Remove town\n");
    strcat(bigmsgrasp,"[server]q.Exit\n");
    strcat(bigmsgrasp,"[server]Please choose one of the above: ");

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

            char * user;
            static char savedUser[100];
            char * pass;

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
                                
                bzero (msg, 100);
                /* citirea mesajului */
                if (read (client, msg, 100) <= 0){
	                perror ("[server]Eroare la read() de la client.\n");
	                close (client);	/* inchidem conexiunea cu clientul */
	                continue;		/* continuam sa ascultam */
	            }

                printf ("[server]Mesajul a fost receptionat...%s\n", msg);
                       
                user = strtok(msg," ");
                pass = strtok(NULL," \n");
                
                printf("[server] user : %d\n",verif(user,pass));
                printf("[server] pass : %s\n",pass);

                if(verif(user,pass) == 0){
                    bzero(msgrasp,100);
                    strcat(msgrasp,"[server]Wrong user or password!!! Closing connection.\n");

                    if (write (client, msgrasp, 100) <= 0){
	                    perror ("[server]Eroare la write() catre client.\n");
	                    continue;		/* continuam sa ascultam */
                    }
                    else printf ("[server]Mesajul a fost trasmis cu succes. %s\n",msgrasp);

                    close (client);
                    exit(0);
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

                   
                bzero (msg, 100);
                /* citirea mesajului */
                if (read (client, msg, 100) <= 0){
	                perror ("[server]Eroare la read() de la client.\n");
	                close (client);	/* inchidem conexiunea cu clientul */
	                continue;		/* continuam sa ascultam */
	            }

                printf ("[server]Mesajul a fost receptionat...%s\n", msg);
                                
                user = strtok(msg," ");
                
                pass = strtok(NULL," \n");

                while(createAccount(user,pass)==0){
                    bzero(msgrasp,100); 
                    strcat(msgrasp,"sendR");
                    if (write (client, msgrasp, 100) <= 0){
	                    perror ("[server]Eroare la write() catre client.\n");
	                    continue;		/* continuam sa ascultam */
                    }
                    else printf ("[server]Mesajul a fost trasmis cu succes. %s\n",msgrasp);
                    
                    bzero (msg, 100);
                    /* citirea mesajului */
                    if (read (client, msg, 100) <= 0){
	                    perror ("[server]Eroare la read() de la client.\n");
	                    close (client);	/* inchidem conexiunea cu clientul */
	                    continue;		/* continuam sa ascultam */
	                }

                    printf ("[server]Mesajul a fost receptionat...%s\n",msg);

                    user = strtok(msg," ");
                    pass = strtok(NULL," \n"); 
                    
                }

                bzero(msgrasp,100);
                strcat(msgrasp,"[server]The account creation was successfull! Logging in... ");

                if (write (client, msgrasp, 100) <= 0){
	                perror ("[server]Eroare la write() catre client.\n");
	                continue;		/* continuam sa ascultam */
                }
                else printf ("[server]Mesajul a fost trasmis cu succes. %s\n",msgrasp);
                             
                
            }
            strcpy(savedUser,user);

            //TRIMITEM MENIUL DE DUPA LOG IN
            if (write (client, bigmsgrasp, 200) <= 0){
                perror ("[server]Eroare la write() catre client.\n");
                continue;		/* continuam sa ascultam */
            }
            else printf ("[server]Mesajul a fost trasmis cu succes. %s\n",bigmsgrasp);

            //DE AICI SE FACE ADAUGAREA

            bzero(msg,100);
            if (read (client, msg, 100) <= 0){
	            perror ("[server]Eroare la read() de la client.\n");
	            close (client);	/* inchidem conexiunea cu clientul */
	            continue;		/* continuam sa ascultam */
	        }
            
            printf ("[server]Mesajul a fost receptionat...%s\n", msg);
            
            //pana aici merge 20:13

            if (msg[0] == '2') {

                int j=0;
                int count = 0;
                char * line = NULL;
                size_t length = 0;
                
               //GENERARE VECTOR CU ORASE PENTRU MENIUL 1
                FILE *f;
                f = fopen("/home/leafy/Retele/WeatherMonitor/cities/index","r");

                while(getline(&line,&length, f)!=EOF){
                    count++;
                }
                //printf("%d",count);

                char vect[count][10];
                
                for(int i=0;i<count;i++){
                    bzero(vect[i],10);
                }
                
                rewind(f);
                while(getline(&line, &length, f)!=EOF){
                    line = strtok(line,"\n");
                    strcpy(vect[j],line);
                    j++;
                }

                /*for(int i=0;i<count;i++){
                    printf("%s\n",vect[i]);
                }*/
                fclose(f);

                //GENERARE MENIU PROPRIU-ZIS - pana aici merge

                char addMenu[100]= " ";
                char nr[20];
                
                bzero(addMenu,100);
                strcat(addMenu,"Please select one of the following:\n");
                for(int i=0;i<count;i++){
                    bzero(nr,2);
                    snprintf(nr,sizeof(nr),"%d",i); //conversie pentru strcat a int spre char
                    strcat(addMenu,nr);
                    strcat(addMenu,".");
                    strcat(addMenu,vect[i]);
                    strcat(addMenu,"\n");
                }
                strcat(addMenu,"&");
                bzero(nr,20);
                snprintf(nr,sizeof(nr),"%d",count);
                strcat(addMenu,nr);
                
                //printf("%s",addMenu);
                
                if (write (client, addMenu, 100) <= 0){
	                perror ("[server]Eroare la write() catre client.\n");
	                continue;		/* continuam sa ascultam */
                }
                else printf ("[server]Mesajul a fost trasmis cu succes. %s\n",addMenu);

                bzero(msg,100);
                if (read (client, msg, 100) <= 0){
	                perror ("[server]Eroare la read() de la client.\n");
	                close (client);	/* inchidem conexiunea cu clientul */
	                continue;		/* continuam sa ascultam */
	            }

                printf ("[server]Mesajul a fost receptionat...%s\n", msg);
                
                //printf("user: %s",savedUser);
                //printf("%d",addTown(savedUser,vect[atoi(msg)]));
                addTown(savedUser,vect[atoi(msg)]);

            }
            else printf("NASPA");

            /*bzero(msg,100);
                  


            /*if (msg[0] == '2'){
                

                
                

                if (write (client, addMenu, 100) <= 0){
	                perror ("[server]Eroare la write() catre client.\n");
	                continue;		/* continuam sa ascultam 
                }
                else printf ("[server]Mesajul a fost trasmis cu succes. %s\n",addMenu);
            //}else printf("MORT");*/

            
            



            
	
          

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
