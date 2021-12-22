// Pay attention to the setting
//void dMassSetCylinder (dMass *, dReal density, int direction, dReal radius, dReal length);
//dGeomID dCreateCylinder ( dSpaceID space, dReal radius, dReal length );
//void dsDrawCylinderD ( const double pos[3], const double R[12], float length, float radius );


#pragma mark -
#pragma mark C-Robot Main



void makeRobotC(int n)
{
    int i;
    dMass m;
    
    RobotC[n].body = dBodyCreate(world);
    dMassSetZero(&m);
    dBodySetPosition(RobotC[n].body, RobotInfo[n].Pos[0], RobotInfo[n].Pos[1], RobotInfo[n].Pos[2]);
    dMassSetCylinder(&m, 0.225, 3, RobotInfo[n].Sizes[0], RobotInfo[n].Sizes[2]);
    dBodySetMass(RobotC[n].body, &m);
    RobotC[n].geom = dCreateCylinder(space, RobotInfo[n].Sizes[0], RobotInfo[n].Sizes[2]);
    dGeomSetBody(RobotC[n].geom, RobotC[n].body);
 }



void ReadRobotC()    //  read Robot information/property - - - - -
{
    FILE *fp;
    int d,i,j,k;
    char    s[500],
            tn[30]="data/dataRobot.txt";
    
    if ((fp=fopen(tn,"r"))==NULL){
        printf("Cannot open %s file\n", tn);
        exit(1);
    }
        
    printf("\n");
    fscanf(fp,"%[^\n]\n", s);
    
    fscanf(fp,"%d\t",&RobotNo);
    for (i=0; i<RobotNo; i++){
        if (fscanf(fp,"%d\t",&k)==EOF)
            RobotNo=i;
        else{
            DeviceReID[k]=i;     //  k: Robot ID in theater
            DeviceReType[k]=2;    //  1; Robot
            RobotInfo[i].ID=k;  //  i: Robot ID in this PC

            fscanf(fp,"%d\t",&d);
            RobotInfo[i].Pos[0]=((double)d)/100.0+ObPos[0][0];
            fscanf(fp,"%d\t",&d);
            RobotInfo[i].Pos[1]=((double)d)/100.0+ObPos[0][1];
            fscanf(fp,"%d\t",&d);
            RobotInfo[i].Pos[2]=(double)d/100.0;
            fscanf(fp,"%d\t",&d);
            RobotInfo[i].TarPos[0]=((double)d)/100.0+ObPos[0][0];
            fscanf(fp,"%d\t",&d);
            RobotInfo[i].TarPos[1]=((double)d)/100.0+ObPos[0][1];

            fscanf(fp,"%d\t",&RobotInfo[i].Type);
            fscanf(fp,"%d\t",&RobotInfo[i].Language);
            fscanf(fp,"%d\t",&RobotInfo[i].TheaterID);
            fscanf(fp,"%d\t",&RobotInfo[i].HumanPosture);
            fscanf(fp,"%[^\n]\n", RobotInfo[i].Name);
            RobotInfo[i].Valid=1;
            RobotInfo[i].State=0;
            
            if (RobotInfo[i].Type==1){
                RobotInfo[i].Sizes[0]=0.12; //  Head Radius
                RobotInfo[i].Sizes[1]=0.06;  //  Body Radius
                RobotInfo[i].Sizes[2]=0.4;  //  Body base
            }
            else if (RobotInfo[i].Type==2){
                RobotInfo[i].Sizes[0]=0.16; //  Head Radius
                RobotInfo[i].Sizes[1]=0.08;  //  Body Radius
                RobotInfo[i].Sizes[2]=0.5;  //  Body base
            }
            else if (RobotInfo[i].Type==3){
                RobotInfo[i].Sizes[0]=0.2; //  Head Radius
                RobotInfo[i].Sizes[1]=0.1;  //  Body Radius
                RobotInfo[i].Sizes[2]=0.4;  //  Body base
            }
            makeRobotC(i);
            printf("Robot ID:%d/%d, %s Pos(%f,%f) TarPos(%f,%f) Valid:%d, Type:%d\n",
                   RobotInfo[i].ID, i, RobotInfo[i].Name,
                   RobotInfo[i].Pos[0], RobotInfo[i].Pos[1],
                   RobotInfo[i].TarPos[0], RobotInfo[i].TarPos[1],
                   RobotInfo[i].Valid, RobotInfo[i].Type);
        }
    }
    printf("Total Robot No:%d\n",RobotNo);
    fclose(fp);
}

// TCP/IP Communication (for Robot Client) - - - - - - - - - - - - - - -

void RobotConTCP(int r){    // - -  send/receive data
    
    struct sockaddr_in server;
    unsigned long dst_ip;
    int port,s;
    
    char codeno[BUFSIZE];      //  code No
    
    if (ServerMode==0){
        dst_ip = inet_addr(ISS_ServerIP);
        port   = Macro_TCP_PORT;
    }
    else if (ServerMode==1){
        dst_ip = inet_addr(Con_ServerIP);
        port   = Con_TCP_PORT;
    }
    
    if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Fail to Robot Theater Server\n");
    }
    else{
        memset((char *) &server, 0, sizeof(server));
        server.sin_family      = AF_INET;
        server.sin_addr.s_addr = dst_ip;
        server.sin_port        = htons(port);
        if (connect(s, (struct sockaddr *) &server, sizeof server) < 0) {
//            printf(" Robot [%d] No Theater Server - State:%d\n",
//                   r, RobotInfo[r].State);
            RobotInfo[r].State=0;
        }
        else{
            write(s, buf, strlen(buf));   //  sending data
            memset(buf, 0, sizeof(buf));
            read(s, buf, sizeof(buf));  //  receiving data
            close(s);
//            printf("Received Data: %s\n", buf);
            
            //  Extraction of Robot Information
            if (RobotInfo[r].State==0){
                RobotInfo[r].State=2;          // connected:   1: moving, 2: stopped
//                printf("Robot [%d] Connected - State:%d\n",
//                       r, RobotInfo[r].State);
            }
            else if (RobotInfo[r].State==3){
                memset(codeno, 0, sizeof(codeno));
                codeno[0]=buf[0];   //  Robot Theater ID
                codeno[1]=buf[1];
                codeno[2]=buf[2];
                RobotInfo[r].TheaterID = atoi(codeno);
                if (RobotInfo[r].TheaterID==999)
                    RobotInfo[r].TheaterID=-1;
                else{
                    RobotInfo[r].State=RobotInfo[r].Language;   //  Robot Language
//                    ReadSentence(RobotInfo[r].TheaterID);
                }
//                printf("Robot [%d] Connected - State:%d\n",
//                       r, RobotInfo[r].State);
            }
            else if (RobotInfo[r].State==RobotInfo[r].Language){
                memset(codeno, 0, sizeof(codeno));
                codeno[0]=buf[0];   //  Robot Theater ID
                codeno[1]=buf[1];
                RobotInfo[r].PhaseID = atoi(codeno);
                if (RobotInfo[r].PhaseID==99){
                    RobotInfo[r].PhaseID=-1;
                    RobotInfo[r].State=2;
                }
                else if (RobotInfo[r].PhaseID<98){
                    memset(codeno, 0, sizeof(codeno));
                    codeno[0]=buf[2];   //  Robot Utterance ID
                    codeno[1]=buf[3];
                    RobotInfo[r].SectionID = atoi(codeno);
                    memset(codeno, 0, sizeof(codeno));
                    codeno[0]=buf[4];   //  Robot Utterance ID
                    codeno[1]=buf[5];
                    RobotInfo[r].ScenarioID = atoi(codeno);
                    memset(codeno, 0, sizeof(codeno));
                    codeno[0]=buf[6];   //  Robot Utterance ID
                    codeno[1]=buf[7];
                    RobotInfo[r].SentenceID = atoi(codeno);
                    RobotInfo[r].State=10;
                    printf("  -- Robot: Theater [%d][%d][%d][%d] in [%d]\n",
                               RobotInfo[r].PhaseID, RobotInfo[r].SectionID,
                               RobotInfo[r].ScenarioID, RobotInfo[r].SentenceID,
                               RobotInfo[r].Language);
                }
            }
        }
    }
}


void sendRobotCdata(int r){    // r:robot ID, send state (m) data to Robot Contents Server

    struct sockaddr_in server;
    unsigned long dst_ip;
    int        port;
    int        i,k,m,v;

    char    numberName[15]="01234567890";
       
    memset(buf, 0, sizeof(buf));
    
    i=RobotInfo[r].ID;
    
    if (i>=100){
        k=(int)(i/100)%10;
        buf[0]=numberName[k];   //  ID (000-999)
    }
    else
        buf[0]=numberName[0];   //  ID (000-999)
    if (i>=10){
        k=(int)(i/10)%10;
        buf[1]=numberName[k];   //  ID (000-999)
    }
    else
        buf[1]=numberName[0];   //  ID (000-999)
    k=i%10;
    buf[2]=numberName[k];       //  ID (000-999)

    m=RobotInfo[r].State;        //  Send State
    if (m>=10) {
        k=(int)(m/10)%10;
        buf[3]=numberName[k]; // State ID
    }
    else
        buf[3]=numberName[0]; // State ID
    k=m%10;
    buf[4]=numberName[k]; // State ID
    
    for (i=0; i<3; i++){
        v=(int)((RobotInfo[r].Pos[i]+50.0)*100.0);
        if (v>=1000){
            k=(int)(v/1000)%10;
            buf[i*4+5]=numberName[k];   //  ID (0000-9999)
        }
        else
            buf[i*4+5]=numberName[0];   //  ID (0000-9999)
        if (v>=100){
            k=(int)(v/100)%10;
            buf[i*4+6]=numberName[k];   //  ID (000-999)
        }
        else
            buf[i*4+6]=numberName[0];   //  ID (000-999)
        if (v>=10){
            k=(int)(v/10)%10;
            buf[i*4+7]=numberName[k];   //  ID (00-99)
        }
        else
            buf[i*4+7]=numberName[0];   //  ID (00-99)
        k=v%10;
        buf[i*4+8]=numberName[k];       //  ID (0-9)
    }
    
//    printf("RobotC[%d] Sent Data: %s\n",
//           r, buf);
    
    RobotConTCP(r);             //  sending and receiving trough buf[]
}



void communication_RobotC() // Robot Client
{
    int i,j,t;
    
    for (i=0; i<RobotNo; i++){
        if (RobotInfo[i].Valid==1){
            if (RobotInfo[i].State==10)
                RobotInfo[i].State=11;
            else if (RobotInfo[i].State==11)
                RobotInfo[i].State=RobotInfo[i].Language;
//            senddata(RobotInfo[i].State);   //  Access to Robot Theater Server
        }
    }
}


void control_RobotC()  //  Communication Robots
{
    const dReal* pos;
    float  u, err, err1, err2, terr=0, terr2=0,
            kp = 30,  kd = 0.1;
    int i;
    
//    for (i=0; i<6; i++){
//            if (i==1){
//                targete[i+3] = dJointGetSliderPosition (Link[i].joint);
//                err = RTar[RobotTargetID][i+3]-targete[i+3];
//                terr+=fabs(err);
//                u = kp * err;
//                dJointSetSliderParam(Link[i].joint, dParamVel, u);
//                dJointSetSliderParam(Link[i].joint, dParamFMax, 20);
//            }
//            else {
//                targete[i+3] = dJointGetHingeAngle(Link[i].joint);
//                err = RTar[RobotTargetID][i+3]-targete[i+3];
//                err1= RTar[RobotTargetID][i+3]-targete[i+3]+PI*2.0;
//                if (fabs(err1)<fabs(err))
//                    err=err1;
//                terr+=fabs(err*err);
//                err2= targete[i+3]-targete2[i+3];
//                terr2+=fabs(err2);
//                u = kp * err + kd * err2;
//                dJointSetHingeParam(Link[i].joint, dParamVel, u);
//                dJointSetHingeParam(Link[i].joint, dParamFMax, 20);
//                targete2[i+3]=targete[i+3];
//            }
//        }
        
}



void drawRobotC()   //  Communication Robots
{
  
    int i,j,k;
    const dReal* pos, * R;
    dReal   sizef[3], posf[3],
    Rf[12]={ 1,0,0,0, 0,1,0,0, 0,0,1,0};
    double* radius = (double*)malloc(sizeof(double) * 2);
    double* length = (double*)malloc(sizeof(double) * 2);
    dVector3 sides;

    for (i=0; i<RobotNo; i++){
        if (RobotInfo[i].Valid==1){
            pos = dBodyGetPosition(RobotC[i].body);
            R = dBodyGetRotation(RobotC[i].body);
            sizef[0]=0.01;                      //  radius
            sizef[1]=RobotInfo[i].Sizes[1];     //  length
            posf[0]=pos[0];
            posf[1]=pos[1];
            posf[2]=pos[2]*2.0
                    +RobotInfo[i].Sizes[1]+RobotInfo[i].Sizes[0]*1.5;
            dsSetColorAlpha(1.0, 0.4, 1.0, 0.98);
            dsDrawSphere(posf, Rf, (float)RobotInfo[i].Sizes[1]);
            posf[2]=pos[2]*2.0+RobotInfo[i].Sizes[1]*0.5;
            dsSetColorAlpha(0.4, 0.2, 0.4, 0.98);
            dsDrawSphere(posf, Rf, (float)RobotInfo[i].Sizes[0]);
            
            dsSetColorAlpha(0.0, 0.4, 0.4, 0.98);
            dsDrawCylinder(pos, R,
                    RobotInfo[i].Sizes[2], RobotInfo[i].Sizes[0]); //  Body
        }
    }
};

