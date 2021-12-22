#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <pthread.h>
#include <time.h>


#define BUFSIZE         8192

#define Utter_UDP_PORT  9200    //  Port ID for UDP/IP Theater Contents communication
#define Utter_TCP_PORT  9867    //  ISS Utterance Server

char    utter[BUFSIZE],
        buf[BUFSIZE];

void* issUtterance( void* args ) // Utterance Server
{
    struct sockaddr_in server;
    struct sockaddr_in client;
    socklen_t len;

    int i,j,k,g;
        
    int fin=0, s0, s1;
    char    dataOK[100]="Uttered",
            dataYes[100]="Yes",
            numberName[100]="01234567890";

    if ((s0 = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset((char *) &server, 0, sizeof(server));
    server.sin_family      = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port        = htons(Utter_TCP_PORT);     //  ISS Macro Server
    if (bind(s0, (struct sockaddr *) &server, sizeof(server)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    listen(s0, 10);

    printf("\n\n - - Utterance TCP/IP Server (Port:%d) Started  - -\n\n",
           Utter_TCP_PORT);

    fin=0;
    while (fin==0) {
        len = sizeof(client);
        if ((s1 = accept(s0, (struct sockaddr *) &client, &len)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        memset(buf, 0, sizeof(buf));
        read(s1, buf, sizeof(buf));     //  read from Robot
        
        if (buf[0]=='-'){
            printf(" - voice recogntion\n");
            write(s1, dataYes, strlen(dataYes));         //  send data - - - - - - -
        }
        else{
            printf(" - Utterance: %s \n",buf);
            write(s1, dataOK, strlen(dataOK));         //  send data - - - - - - -
        }
        
        close(s1);
        
        if (fin==1)
            pthread_exit(NULL);
    }
    close(s0);
    return NULL;
}





int main()
{
    int     sock;
    struct  sockaddr_in addr;
    struct sockaddr_in *self_addr;
    struct ifreq ifr;
    int result = 1;
    
    
    pthread_t tid;     // Thread ID
//    pthread_create(&tid, NULL, issUtterance, (void *)NULL);    //  Utterance Server
    
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    
    addr.sin_family = AF_INET;
    addr.sin_port = htons(Utter_UDP_PORT);
    inet_pton(AF_INET, "255.255.255.255", &addr.sin_addr.s_addr);   //ブロードキャストの設定
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *)&result, sizeof(result)) != 0) { //ソケットへの情報の登録
        perror("setsockopt");
        return 1;
    }
    
    while (1){
        ifr.ifr_addr.sa_family = AF_INET;           //IPV4のIPアドレス取得のための設定
        strncpy(ifr.ifr_name, "en0", IFNAMSIZ-1);   //引数の”en0”が無線LANを表す（Macの場合）
        ioctl(sock, SIOCGIFADDR, &ifr);             //sockからアドレスの引き出し
        self_addr = ((struct sockaddr_in *)&ifr.ifr_addr);  //アドレス情報の取り出し
        printf("Database Server IP %s, Port:%d \n",
               inet_ntoa(self_addr->sin_addr), Utter_UDP_PORT);   //送信：　IP アドレス
        sendto(sock, inet_ntoa(self_addr->sin_addr),
               strlen(inet_ntoa(self_addr->sin_addr)), 0,
               (struct sockaddr *)&addr, sizeof(addr));
        sleep(1);
    }
    
    if(self_addr != NULL)
        free(self_addr);
    
    close(sock);
    
    
    return 0;
}
