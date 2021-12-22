//  ISS Robot Theater Management Program

//  Compile with g++ -I/usr/local/include -L/usr/local/lib -lode -ldrawstuff -lm -framework GLUT -framework OpenGL -o a.out


//  Include Files - - - - - - - - - - - - - - - - - - - -


#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <math.h>
#include <pthread.h>
#include <time.h>

#include <ode/ode.h>
#include <drawstuff/drawstuff.h>

#include "main.h"
#include "Sensor.h"
#include "Theater.h"
#include "GNG.h"            //  GNG
#include "MacroODE.h"        //  Draw by ODE




#pragma mark -
#pragma mark Servers


void* RobotTheater( void* args ) // Robot Theater
{
    int i,j,t;

    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 300000000;     //   Time Interval
    
    while(finRT==0){
        if (PhaseID<=-4){
            PhaseID++;
            printf("Please wait [%d seconds]  PhaseID:%d\n", -3-PhaseID, PhaseID);
        }
        
        if (PhaseID == -2){
            TheaterID=RobotInfo[RobotID].Theater;    //  current facing robot;
            printf("Transition to Theater: ID:%d (%d-th Robot)\n",
                   TheaterID, RobotID);
            init_Theater();
        }
        else if (PhaseID == -1)
            init_transition();      //  init transition
        else if (PhaseID>=0){
            if (SentenceID==-1){    //  Scenario transition
                if (VoicePreviousUtternace.Times>=2){
                    scenario_transition2();     //  scenario transition to confirm ihe transition
                    
                    robot_utterance();      //  robot utterance
                }
                else if (VoicePreviousUtternace.Times>0){
                    robot_utterance2();      //  robot utterance
                }
                scenario_transition();     //  scenario transition
                VoicePreviousUtternace.Times++;
            }
            else
                robot_utterance();      //  robot utterance
        }
        
        if (SimulationMode>=10)     //  Virtual Robot Simulations
            SensorRobot();
        
        if (finRT==1){
            printf("\n\n - - Finished Theater[%d]: %s - -\n\n",
            TheaterID,TheaterInfo[TheaterID].TheaterName);
            TheaterID=-1;
            RobotID=-1;
            HumanID=-1;
            pthread_exit(NULL);
        }
        nanosleep(&ts, NULL);
    }
    return NULL;
}

void initRobotTheater()
{
    finRT=0;
    pthread_t tid1;   // Thread ID
    pthread_create(&tid1, NULL, RobotTheater, (void *)NULL);
}



void* issMacro( void* args ) // Macro-scopic Simulations with GNG
{
    struct sockaddr_in server;
    struct sockaddr_in client;
    socklen_t len;

    int m,          //  GNG ID
        DeviceType,
        DeviceID,
        DeviceState,
        i,j,k,g;
    double x[5];
    
    m=gngID;        //  GNG ID
    
    int s0, s1;
    char    codeno[10],
            dataOK[12]="999",
            numberName[12]="01234567890";

    if ((s0 = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset((char *) &server, 0, sizeof(server));
    server.sin_family      = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port        = htons(Macro_TCP_PORT);     //  ISS Macro Server
    if (bind(s0, (struct sockaddr *) &server, sizeof(server)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    listen(s0, 10);

    printf("\n\n - - ISS Macro ODE Server (Port:%d) Started  - -\n\n",
           Macro_TCP_PORT);

    fin=0;
    if (GNGinfo[m].state==0)
        GNGinfo[m].state=1;     //  GNG data collection starts

    while (fin==0) {
        len = sizeof(client);
        if ((s1 = accept(s0, (struct sockaddr *) &client, &len)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        memset(buf, 0, sizeof(buf));
        read(s1, buf, sizeof(buf));     //  read from Robot
        write(s1, dataOK, strlen(dataOK));         //  send data - - - - - - -
        close(s1);
        
//        printf(" Received:%s\n", buf);
    
        memset(codeno, 0, sizeof(codeno));
        for (j=0; j<3; j++)
            codeno[j]=buf[j];   //  Robot ID
        DeviceID = atoi(codeno);   // DeviceID : Global ID

        k=DeviceReID[DeviceID];   //  k: local ID in each category / this PC
        
        if (DeviceID==999){     //  init People and Robots
            memset(codeno, 0, sizeof(codeno));
            for (j=0; j<2; j++)
                codeno[j]=buf[j+3];   //
            HumanNo = atoi(codeno);

            memset(codeno, 0, sizeof(codeno));
            for (j=0; j<2; j++)
                codeno[j]=buf[j+5];   //
            BuggyNo = atoi(codeno);
            
            memset(codeno, 0, sizeof(codeno));
            for (j=0; j<2; j++)
                codeno[j]=buf[j+7];   //
            RobotNo = atoi(codeno);
            printf("People No:%d, Buggy No:%d, Robot No:%d\n",
                   HumanNo, BuggyNo, RobotNo);
        }
        else {
            memset(codeno, 0, sizeof(codeno));
            for (j=0; j<2; j++)
                codeno[j]=buf[j+3];   //  Robot ID
            DeviceState = atoi(codeno);   //  State
            
            if (DeviceID<50){
                DeviceType=1;   //  Sensors (0-49)
            }
            else if (DeviceID<60){
                DeviceType=2;   //  Robots (50-59)
                RobotInfo[k].Valid=1; //  robot
            }
            else if (DeviceID<80){
                DeviceType=3;   //  Buggy  (60-79)
                BuggyInfo[k].Valid=1; //  buggy
            }
            else if (DeviceID<95){
                DeviceType=4;   //  Humans (80-95)
                HumanInfo[k].Valid=1; //  people
            }
            
            for (i=0; i<3; i++){
                memset(codeno, 0, sizeof(codeno));
                for (j=0; j<4; j++)
                    codeno[j]=buf[i*4+j+5];   //  position (xxx.x)
                x[i] = (double)(atoi(codeno))*0.01-50.0;
                if (DeviceType==2)
                    RobotInfo[k].pos[i]=x[i];
                else if (DeviceType==3)
                    BuggyInfo[k].pos[i]=x[i];
                else if (DeviceType==4)
                    HumanInfo[k].pos[i]=x[i];
//            printf(" %4.1f, ",x[i]);
            }
            if (WordShow==1){
                if (DeviceType==2)
                    printf(" Robot[%d/%d] (%4.2f, %4.2f, %4.2f)  State:%d, Received:%s\n",
                       k, DeviceID, x[0], x[1], x[2], DeviceState, buf);
                else if (DeviceType==3)
                    printf(" Buggy[%d/%d] (%4.2f, %4.2f, %4.2f)  State:%d, Received:%s\n",
                       k, DeviceID, x[0], x[1], x[2], DeviceState, buf);
                else if (DeviceType==4)
                    printf(" People[%d/%d] (%4.2f, %4.2f, %4.2f) State:%d, Received:%s\n",
                       k, DeviceID, x[0], x[1], x[2], DeviceState, buf);
            }
            if ((SimulationMode>=12)&&(DeviceType>=2)&&(DeviceState==1)){       //  GNG with moving agents
                if (GNGinfo[m].dataNo<GNGinfo[m].dataMax)
                    g=GNGinfo[m].dataNo;
                else
                    g=(int)(GNGinfo[m].dataMax*rnd());  // random teaching data replacement
                if (GNGinfo[m].state>=1){
                    for (j=0; j<GNGinfo[m].inp; j++)
                        GNGinfo[m].tdata[g][j]=x[j];
                    GNGinfo[m].tdata[g][3]=DeviceID;
                    currenttime();
                    GNGinfo[m].tdata[g][4]=logyear;
                    GNGinfo[m].tdata[g][5]=logmon;
                    GNGinfo[m].tdata[g][6]=logday;
                    GNGinfo[m].tdata[g][7]=loghour*3600+logmin*60+logsec;
                }
                if (GNGinfo[m].dataNo<GNGinfo[m].dataMax){
                    GNGinfo[m].dataNo++;
                    if (GNGinfo[m].dataNo>=GNGinfo[m].dataMax){  //  updte of data number till max
                        if (GNGinfo[m].state==1)
                            GNGinfo[m].state=2;
                        GNGinfo[m].dataNo=GNGinfo[m].dataMax;
                        writeTdata(m);      //  write teaching data
                    }
                }
                if (WordShow==1)
                    printf("GNG[%d] state:%d data:%d, %d/%d\n",
                       m, GNGinfo[m].state, g, GNGinfo[m].dataNo, GNGinfo[m].dataMax);
                
                if ((SimulationMode==13)&&(DeviceType==3)){ //  Buggy Top-In
                    Topological_main(gngID, k);
                }
            }
        }
        if (fin==1)
            pthread_exit(NULL);
    }
    close(s0);
    return NULL;
}


void* issTCP( void* args ) // human interaction mode selection
{
    struct sockaddr_in server;
    struct sockaddr_in client;
    socklen_t len;

    int DeviceType,
        DeviceID,
        DeviceState,
        LocalID,
        i,j,k,m;
    int s0, s1;
    double x[5];
    
    char    codeno[10],
            NoTheater[10]="999",    //  Not started theater ("99") or theater finished ("99")
            StopHuman[10]="996",     //  Stop Human
            InitSim[10]="97",       //  initialize simulation
            NoUtterance[10]="98",
            numberName[12]="01234567890";

    if ((s0 = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset((char *) &server, 0, sizeof(server));
    server.sin_family      = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port        = htons(Con_TCP_PORT);
    if (bind(s0, (struct sockaddr *) &server, sizeof(server)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    listen(s0, 10);

    printf("\n\n - - Robot Theater Server (Macro) Started  - -\n\n");

    while (fin==0) {
        len = sizeof(client);
        if ((s1 = accept(s0, (struct sockaddr *) &client, &len)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        memset(buf, 0, sizeof(buf));
        read(s1, buf, sizeof(buf));     //  read from Robot

        memset(codeno, 0, sizeof(codeno));
        for (j=0; j<3; j++)
                   codeno[j]=buf[j];   //  Robot ID
        DeviceID = atoi(codeno);   // DeviceID : Global ID
        LocalID=DeviceReID[DeviceID];   //  local ID in each category / this PC
       
        DeviceType=0;
        if (DeviceID==999){     //  init People and Robots
            memset(codeno, 0, sizeof(codeno));
            for (j=0; j<2; j++)
                codeno[j]=buf[j+3];   //
            HumanNo = atoi(codeno);

            memset(codeno, 0, sizeof(codeno));
            for (j=0; j<2; j++)
                codeno[j]=buf[j+5];   //
            BuggyNo = atoi(codeno);

            memset(codeno, 0, sizeof(codeno));
            for (j=0; j<2; j++)
                codeno[j]=buf[j+7];   //
            RobotNo = atoi(codeno);
            printf("People No:%d, Buggy No:%d, Robot No:%d\n",
                   HumanNo, BuggyNo, RobotNo);
        }
        else {           
            memset(codeno, 0, sizeof(codeno));
            for (j=0; j<2; j++)
                codeno[j]=buf[j+3];
            DeviceState = atoi(codeno);   // Device State
                    
            if (DeviceID<50){
                DeviceType=1;   //  Sensors (0-49)
            }
            else if (DeviceID<60){
                DeviceType=2;   //  Robots (50-59)
                RobotInfo[LocalID].Valid=1; //  robot
            }
            else if (DeviceID<80){
                DeviceType=3;   //  Buggy  (60-79)
                BuggyInfo[LocalID].Valid=1; //  buggy
            }
            else if (DeviceID<95){
                DeviceType=4;   //  Humans (80-95)
                HumanInfo[LocalID].Valid=1; //  people
            }
                    
            for (i=0; i<3; i++){
                memset(codeno, 0, sizeof(codeno));
                for (j=0; j<4; j++)
                    codeno[j]=buf[i*4+j+5];   //  position (xxx.x)
                x[i] = (double)(atoi(codeno))*0.01-50.0;
                if (DeviceType==2)
                    RobotInfo[LocalID].pos[i]=x[i];
                else if (DeviceType==3)
                    BuggyInfo[LocalID].pos[i]=x[i];
                else if (DeviceType==4)
                    HumanInfo[LocalID].pos[i]=x[i];
            }
                
//            if (DeviceType==2)
//                printf(" Robot[%d/%d] (%4.2f, %4.2f, %4.2f)  State:%d, Received:%s\n",
//                        k, DeviceID, x[0], x[1], x[2], k, buf);
//            else if (DeviceType==3)
//                printf(" Buggy[%d/%d] (%4.2f, %4.2f, %4.2f)  State:%d, Received:%s\n",
//                        k, DeviceID, x[0], x[1], x[2], k, buf);
//            else if (DeviceType==4)
//                printf(" People[%d/%d] (%4.2f, %4.2f, %4.2f) State:%d, Received:%s\n",
//                        k, DeviceID, x[0], x[1], x[2], k, buf);
        }
    
//        printf(" Received:%s,  Robot[%d/%d/%d] State:%d/%d, Theater:%d\n",
//        buf, k, i,DeviceID,  DeviceState, RobotInfo[DeviceID].State, TheaterID);

        switch (DeviceType){
            case 0:           //  others --------------------------------------------------

                break;

            case 1:           //  Sensors --------------------------------------------------

                break;

            case 2:           //  Communication Robots --------------------------------------------------
                if ((TextShow==1)&&(WordShow==1))
                    printf("Valid[%d/%d]:%d, Device State:%d, RobotState:%d\n",
                           LocalID, DeviceID, RobotInfo[LocalID].Valid,
                           DeviceState, RobotInfo[LocalID].State);
                
                if (RobotInfo[LocalID].Valid==1){
                    if ((DeviceState==0) || (PhaseID<0)){
                        RobotInfo[LocalID].State=1;             //  connected
                        write(s1, NoTheater, strlen(NoTheater));      //  send data - - - - - - -
                    }
                else if (DeviceState==1){             //  ready to start theater
                        if (TheaterID>=0){
                            RobotInfo[LocalID].State=2;
                            memset(buf, 0, sizeof(buf));    //  send data
                            m=TheaterID;
                            if (m>=100){
                                k=(int)(m/100)%10;
                                buf[0]=numberName[k];   //  Theater ID (000-999)
                            }
                            else
                                buf[0]=numberName[0]; // Theater ID
                            if (m>=10){
                                k=(int)(m/10)%10;
                                buf[1]=numberName[k];   //  Theater ID (000-999)
                            }
                            else
                                buf[1]=numberName[0]; // Theater ID
                            k=m%10;
                            buf[2]=numberName[k];       //  Theater ID (000-999)
                            write(s1, buf, strlen(buf));         //  send data - - - - - - -
                        }
                        else {
                            RobotInfo[LocalID].State=1;             //  not started
                            write(s1, NoTheater, strlen(NoTheater));       //  send data - - - - - - -
                        }
                    }
                    else if ((DeviceState==5)||(DeviceState==6)){        //  ready to speak
                        if (RobotInfo[LocalID].State==10){
                            memset(buf, 0, sizeof(buf));    //  send data
                            m=PhaseID;
                            if (m>=10){
                                k=(int)(m/10)%10;
                                buf[0]=numberName[k];   //  Phase ID (00-99)
                            }
                            else
                                buf[0]=numberName[0]; // Phase ID
                            k=m%10;
                            buf[1]=numberName[k];       //  Phase ID (00-99)
                            m=SectionID;
                            if (m>=10){
                                k=(int)(m/10)%10;
                                buf[2]=numberName[k];   //  Section ID (00-99)
                            }
                            else
                                buf[2]=numberName[0]; // Section ID
                            k=m%10;
                            buf[3]=numberName[k];       //  Section ID (00-99)
                            m=ScenarioID;
                            if (m>=10){
                                k=(int)(m/10)%10;
                                buf[4]=numberName[k];   //  Scenario ID (00-99)
                            }
                            else
                                buf[4]=numberName[0]; // Scenario ID
                            k=m%10;
                            buf[5]=numberName[k];       //  Scenario ID (00-99)
                            m=SentenceID;
                            if (m>=10){
                                k=(int)(m/10)%10;
                                buf[6]=numberName[k];   //  Sentence ID (00-99)
                            }
                            else
                                buf[6]=numberName[0]; // Sentence ID
                            k=m%10;
                            buf[7]=numberName[k];       //  Sentence ID (00-99)
                            write(s1, buf, strlen(buf));            //  send data - - - - - - -
                        }
                        else {
                            write(s1, NoUtterance, strlen(NoUtterance));      //  send data - - - - - - -
                        }
                    }
                    else if (DeviceState==11){            //  finished speaking -> ready to speak
                        RobotInfo[LocalID].State=11;
                        write(s1, NoUtterance, strlen(NoUtterance));         //  send data - - - - - - -
                    }
                }
                break;
                
            case 3:           //  Buggy --------------------------------------------------
            
                break;
                
            case 4:           //  Humans --------------------------------------------------
                if ((TextShow==1)&&(WordShow==1))
                    printf("Valid[%d/%d]:%d, Device State:%d, HumanState:%d\n",
                        LocalID, DeviceID, HumanInfo[LocalID].Valid,
                        DeviceState, HumanInfo[LocalID].State);
                if (HumanInfo[LocalID].Valid==1){
                    if (HumanInfo[LocalID].Interval>0)
                        write(s1, NoTheater, strlen(NoTheater));         //  send data -
                    else
                    if (DeviceState==0){
                        HumanInfo[LocalID].State=1;             //  connected
                        write(s1, NoTheater, strlen(NoTheater));         //  send data - - - - - - -
                    }
                    else if (DeviceState==1){                    //  ready to start theater
                        if (HumanInfo[LocalID].State==2)             //  not started
                            write(s1, StopHuman, strlen(StopHuman));          //  send data
                        else
                            write(s1, NoTheater, strlen(NoTheater));          //  send data - - - - - - -
                    }
                    else if (LocalID != HumanID){
                        write(s1, NoTheater, strlen(NoTheater));  //  send data - - - - - - -
                       if ((TextShow==1)&&(WordShow==1))
                            printf(" Human:%d (not target), HumanID:%d\n",
                                   LocalID, HumanID);
                    }
                    else if (DeviceState==2){              //  ready to start theater
                        HumanInfo[LocalID].State=2;
                        if ((TextShow==1)&&(WordShow==1)){
                            printf("D2: Valid[%d/%d]:%d, Device State:%d, HumanState:%d\n",
                                 LocalID, DeviceID, HumanInfo[LocalID].Valid,
                                 DeviceState, HumanInfo[LocalID].State);
                            printf("D2: TheaterID:%d, PhaseID:%d\n",
                                   TheaterID, PhaseID);
                        }
                        if (TheaterID>=0){
                            memset(buf, 0, sizeof(buf));    //  send data
                            m=TheaterID;
                            if (m>=100){
                                k=(int)(m/100)%10;
                                buf[0]=numberName[k];   //  Theater ID (000-999)
                            }
                            else
                                buf[0]=numberName[0]; // Theater ID
                            if (m>=10){
                                k=(int)(m/10)%10;
                                buf[1]=numberName[k];   //  Theater ID (000-999)
                            }
                            else
                                buf[1]=numberName[0]; // Theater ID
                            k=m%10;
                            buf[2]=numberName[k];       //  Theater ID (000-999)
                            write(s1, buf, strlen(buf));         //  send data - - - - - - -
                            if ((TextShow==1)&&(WordShow==1))
                                printf("D2: sent TheaterID:%d, data:%s\n",
                                   TheaterID,buf);
                        }
                        else {
                            HumanInfo[LocalID].State=2;             //  not started
                            write(s1, StopHuman, strlen(StopHuman));     //  send data - - - - - - -
                        }
                    }
                    else if ((DeviceState==5)||(DeviceState==6)){     //  ready to speak
                        HumanInfo[LocalID].Language=DeviceState;
                        if ((TextShow==1)&&(WordShow==1)){
                            printf("D%d: Valid[%d/%d]:%d, HumanState:%d\n",
                                   DeviceState, LocalID, DeviceID,
                                   HumanInfo[LocalID].Valid, HumanInfo[LocalID].State);
                            printf("D%d: TheaterID:%d, PhaseID:%d\n",
                                 DeviceState, TheaterID, PhaseID);
                        }
                        if (PhaseID<-3){           //  Robot Theater Finished
                            write(s1, NoTheater, strlen(NoTheater));  //  send data - - -
                        }
                        else if (HumanInfo[LocalID].State==10){
                            memset(buf, 0, sizeof(buf));    //  send data
                            m=PhaseID;
                            if (m>=10){
                                k=(int)(m/10)%10;
                                buf[0]=numberName[k];   //  Phase ID (00-99)
                            }
                            else
                                buf[0]=numberName[0]; // Phase ID
                            k=m%10;
                            buf[1]=numberName[k];       //  Phase ID (00-99)
                            m=SectionID;
                            if (m>=10){
                                k=(int)(m/10)%10;
                                buf[2]=numberName[k];   //  Section ID (00-99)
                            }
                            else
                                buf[2]=numberName[0]; // Section ID
                            k=m%10;
                            buf[3]=numberName[k];       //  Section ID (00-99)
                            m=ScenarioID;
                            if (m>=10){
                                k=(int)(m/10)%10;
                                buf[4]=numberName[k];   //  Scenario ID (00-99)
                            }
                            else
                                buf[4]=numberName[0]; // Scenario ID
                            k=m%10;
                            buf[5]=numberName[k];       //  Scenario ID (00-99)
                            m=SentenceID;
                            if (m>=10){
                                k=(int)(m/10)%10;
                                buf[6]=numberName[k];   //  Sentence ID (00-99)
                            }
                            else
                                buf[6]=numberName[0]; // Sentence ID
                            k=m%10;
                            buf[7]=numberName[k];       //  Sentence ID (00-99)
                            write(s1, buf, strlen(buf));     //  send data - - - - - - -
                        }
                        else {
                            write(s1, NoUtterance, strlen(NoUtterance)); //  send data - -
                        }
                    }
                    else if (DeviceState==10){          //  spoken, ready to speak
                        HumanInfo[LocalID].State=10;
                        if ((TextShow==1)&&(WordShow==1)){
                            printf("D%d: Valid[%d/%d]:%d, HumanState:%d\n",
                                   DeviceState, LocalID, DeviceID,
                                   HumanInfo[LocalID].Valid, HumanInfo[LocalID].State);
                            printf("D%d: TheaterID:%d, PhaseID:%d\n",
                                 DeviceState, TheaterID, PhaseID);
                        }
                        write(s1, NoUtterance, strlen(NoUtterance));         //  send data - - -
                    }
                    else if (DeviceState==11){          //  spoken
                        HumanInfo[LocalID].State=HumanInfo[LocalID].Language;
                        if ((TextShow==1)&&(WordShow==1)){
                            printf("D%d: Valid[%d/%d]:%d, HumanState:%d\n",
                                   DeviceState, LocalID, DeviceID,
                                   HumanInfo[LocalID].Valid, HumanInfo[LocalID].State);
                            printf("D%d: TheaterID:%d, PhaseID:%d\n",
                                 DeviceState, TheaterID, PhaseID);
                        }
                        write(s1, NoUtterance, strlen(NoUtterance));         //  send data - - -
                    }
                }
                break;
        }
        close(s1);
        if (fin==1)
            pthread_exit(NULL);
    }
    close(s0);
    return NULL;
}





#pragma mark -
#pragma mark Init and Main

void init()
{
    double d;
    int i,t=0;
    
    srand((unsigned int)time(NULL));
 
    for (i=0; i<DEVICE; i++){
            DeviceReID[i]=-1;
            DeviceReType[i]=-1;
        }

    ReadTheater();      //  read Theater Information
    ReadSensorData();
    ReadFurData();
    ReadBuggy();
    ReadRobot();        //  read Robot Information
    ReadVoice();
        
    InitHuman();   //  initialize
    InitBuggy();   //  initialize
    initDevices();   //  ODE Draw
    GNG_initAll();  //  GNG
    
    for (i=0; i<DEVICE; i++)
        if (DeviceReType[i]>=0)
            printf("D[%d] Type:%d, Local ID:%d\n",
                   i, DeviceReType[i], DeviceReID[i]);
    RobotID=-1;
    HumanID=-1;
    
    currenttime();
    printf("\nDate: %d/%d/%d (%d)  Time:%d:%d:%d\n\n",
           logyear, logmon, logday, logweek, loghour, logmin, logsec);
}

int main(int argc, char **argv)
{
    int i,j;
        
    init();     //  initialization


    
    if (SimulationMode<10){     //
        ISS_UDP_receive();  //  receive ISS Server IP
        Con_UDP_receive();  //  receive Robot Contents Server IP
        pthread_t tid2;   // Thread ID
        pthread_create(&tid2, NULL, issTCP, (void *)NULL);
    }
    else  if (SimulationMode==10){     //
        Con_UDP_receive();  //  receive ISS Server IP
//        Utter_UDP_receive();
        pthread_t tid2;   // Thread ID
        pthread_create(&tid2, NULL, issTCP, (void *)NULL);
    }
    else  if (SimulationMode==11){     //
        Con_UDP_receive();  //  receive ISS Server IP
        Utter_UDP_receive();
        pthread_t tid2;   // Thread ID
        pthread_create(&tid2, NULL, issTCP, (void *)NULL);
    }
    else if (SimulationMode>=12){     //  11: without GNG,  12: with GNG
        ISS_UDP_receive();  //  receive ISS Server IP
        pthread_t tid3;     // Thread ID
        pthread_create(&tid3, NULL, issMacro, (void *)NULL);    //  Macro-scopic simulations
    }
    else
        InitRobot();   //  init virtual robot simulation
    
    dsFunctions fn;                 //  ODE Setting
    fn.version = DS_VERSION;
    fn.start = &start;
    fn.step = &simLoop;
    fn.command = &command;
    fn.stop = 0;
    
#if defined(__APPLE__) || defined(__MACOSX__)
    fn.path_to_textures = "textures";                     // Mac
#else
    fn.path_to_textures = "../../drawstuff/textures";   // Windows
#endif
    
    // create world
    dInitODE2(0);
    world = dWorldCreate();
    space = dHashSpaceCreate (0);
    contactgroup = dJointGroupCreate (0);
    ground = dCreatePlane (space,0,0,1.0,0);
    dWorldSetGravity (world,0,0,-9.8);      //  to be updated
    dWorldSetCFM(world, 1e-3);
    dWorldSetERP(world, 0.8);
    dsSimulationLoop (argc,argv,500,500,&fn);
    
    printf(" --- Fin Theater server ---\n");

    dJointGroupDestroy (contactgroup);
    dSpaceDestroy (space);
    dWorldDestroy (world);
    dCloseODE();
    return 0;
}



