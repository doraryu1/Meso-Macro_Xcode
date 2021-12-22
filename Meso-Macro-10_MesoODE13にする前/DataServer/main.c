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

#define UDP_PORT 9000   //  Port ID for UDP/IP ISS Database communication

int main()
{
    int     sock;
    struct  sockaddr_in addr;
    struct sockaddr_in *self_addr;
    struct ifreq ifr;
    int result = 1;
    
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    
    addr.sin_family = AF_INET;
    addr.sin_port = htons(UDP_PORT);
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
               inet_ntoa(self_addr->sin_addr), UDP_PORT);   //送信：　IP アドレス
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
