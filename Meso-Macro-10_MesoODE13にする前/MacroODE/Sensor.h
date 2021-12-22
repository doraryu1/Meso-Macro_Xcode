// Virtual sensing

#pragma mark -
#pragma mark Human/Robot sensing


void SensorRobot()      //  Virtual Robot Communication
{
    int i,j,k;
    double d[DEVICE+1];
    
    for (i=0; i<RobotNo; i++){
        if (PhaseID<0)
            RobotInfo[i].State=1;
        else if ((TheaterID>=0) && (RobotInfo[i].State==1))
            RobotInfo[i].State=2;
//        else if (RobotInfo[i].State==2)
//            RobotInfo[i].State=RobotInfo[i].Language;
        else if (RobotInfo[i].State==10){
            if (RobotInfo[i].Language==5)
                printf("  - Robot[%d]: [%d][%d][%d][%d]: %s\n",
                        i, PhaseID, SectionID, ScenarioID, SentenceID,
                        ScenarioInfo[PhaseID][SectionID][ScenarioID].Sentence[SentenceID]);
            else
                printf("  - Robot[%d]: [%d][%d][%d][%d]: %s\n",
                        i, PhaseID, SectionID, ScenarioID, SentenceID,
                        ScenarioInfo[PhaseID][SectionID][ScenarioID].SentenceJP[SentenceID]);
            sleep(1);
            RobotInfo[i].State=11;
        }
//        printf("  - Robot[%d]: State: %d\n", i, RobotInfo[i].State);
    }
}

void SensorHumanPosition()      //  Virtual Human Position Sensing
{
    int i,j,k;
    double d[DEVICE+1];

    for (i=0; i<HumanNo; i++){
        if (HumanInfo[i].Interval>0){
            HumanInfo[i].Interval--;
            if ((TextShow==1)&&(WordShow==1)){
                if (HumanInfo[i].Interval%100==0)
                    printf("  Human:%d, Interval:%d\n",
                           i, HumanInfo[i].Interval);
            }
        }
        else if (HumanInfo[i].Valid==1){
            for (j=0; j<RobotNo; j++)
                if (RobotInfo[j].Valid==1){
                    d[j]=( (HumanInfo[i].pos[0]-RobotInfo[j].tarpos[0])
                          *(HumanInfo[i].pos[0]-RobotInfo[j].tarpos[0])
                          +(HumanInfo[i].pos[1]-RobotInfo[j].tarpos[1])
                          *(HumanInfo[i].pos[1]-RobotInfo[j].tarpos[1]));
                }
            k=0;
            while ((RobotInfo[k].Valid==0) && (k<RobotNo))
                k++;
            if (k<RobotNo-1)
                for (j=k+1; j<RobotNo; j++)
                    if (RobotInfo[j].Valid==1){
                        if (d[j]<d[k])
                            k=j;
                    }
            
            if (d[k]<0.8){   //  radius:  if the person is near the k-th robot.
                if ((finRT==1)&&(HumanInfo[i].Interval==0)){
                    initRobotTheater();
                    if ((TextShow==1)&&(WordShow==1))
                        printf(" - - finRT:%d, Human:%d, R:%d, d:%f\n",
                               finRT, i,k,d[k]);
                    HumanInfo[i].State=2;   //  Stop
                    RobotID=k;
                    HumanID=i;
                }
                if (PhaseID==-3){       //  Ready to start
                    RobotID=k;
                    HumanID=i;
                    TheaterID=RobotInfo[k].Theater;
                    if ((TextShow==1)&&(WordShow==1))
                        printf(" - - TheaterID:%d, Human:%d, Robot:%d\n",
                               TheaterID, HumanID, RobotID);
                    init_Theater();    //  start the initial robot theater
                    EnvSensorData[issID].State=RobotInfo[k].Theater;    //  Theater ID
                    
                }
            }
            else
                HumanInfo[i].State=1;   //  the person is just connecetd.
            
        }
    }
}


void SensorHuman()      //  Human Movement Simulations
{
    int i,j,k;
    double d[DEVICE+1];

    for (i=0; i<HumanNo; i++){
        if (HumanInfo[i].Valid==1){
            if (rnd()<0.9){
                HumanInfo[i].pos[0]+=HumanInfo[i].VX+rndn()*2;
                HumanInfo[i].pos[1]+=HumanInfo[i].VY+rndn()*2;
                if ((HumanInfo[i].pos[0]>495) || (HumanInfo[i].pos[0]<5)
                    || (HumanInfo[i].pos[1]>495) || (HumanInfo[i].pos[1]<5))
                    HumanInfo[i].Valid=0;
                else if (rnd()<0.05){
                    HumanInfo[i].VX=rndn()*3;
                    HumanInfo[i].VY=rndn()*3;
                }
            }
            for (j=0; j<RobotNo; j++)
                if (RobotInfo[j].Valid==1){
                    d[j]=((HumanInfo[i].pos[0]-RobotInfo[j].pos[0])
                          *(HumanInfo[i].pos[0]-RobotInfo[j].pos[0])
                          +(HumanInfo[i].pos[1]-RobotInfo[j].pos[1])
                          *(HumanInfo[i].pos[1]-RobotInfo[j].pos[1]));
                }
            k=0;
            while ((RobotInfo[k].Valid==0) && (k<RobotNo))
                k++;
            if (k<RobotNo)
                for (j=k+1; j<RobotNo; j++)
                    if (RobotInfo[j].Valid==1){
                        if (d[j]<d[k])
                            k=j;
                    }
            if (d[k]<1000){
                HumanInfo[i].State=k;
                printf("Human:%d, R:%d, d:%f\n",i,k,d[k]);
                if (PhaseID==-3){       //  Ready to start
                    RobotID=k;
                    HumanID=i;
                    TheaterID=RobotInfo[i].Theater;
                    init_Theater();    //  start the initial robot theater
                    EnvSensorData[issID].State=RobotInfo[k].Theater;    //  Theater ID
                    printf("TheaterID:%d\n",
                            TheaterID);
                    HumanInfo[i].VX=-HumanInfo[i].VX;
                    HumanInfo[i].VY=-HumanInfo[i].VY;
                }
            }
            else
                HumanInfo[i].State=-1;
        }
        else if (rnd()<0.1){
            HumanInfo[i].Valid=1;
//            init_Human_data(i);
        }
    }
}
                                                 
void SensorWatch()      //  Time
{
    double d;
    currenttime();
    printf("\nCurrent Time:%d:%d:%d\n", loghour, logmin, logsec);
    
    if (rnd()<0.3)
        EnvSensorData[0].State=0;
    else if (loghour<12)
        EnvSensorData[0].State=1;
    else if (loghour<17)
        EnvSensorData[0].State=2;
    else
        EnvSensorData[0].State=3;
}


void SensorVoice(int p, int q)     //  Voice Recognition, p:Category ID, q:Word ID
{
    int i,j,k;
    for (i=0; i<VoiceCategoryNo; i++){
        k=EnvSensorNo+i;
        EnvSensorData[k].State=-1;
        for (j=0; j<VoiceWordNo[i]; j++){
            if (strstr(VoiceInput.Sentence, VoicePhrase[i][j].Sentence)!=NULL){
                VoicePhrase[i][j].State=1;
                EnvSensorData[k].State=j;       //  recognition results i:0-99, ,j:0-999
                printf(" Recognized Word [%d][%d] %s\n",i, j, VoicePhrase[i][j].Sentence);
                j=VoiceWordNo[i];
            }
            else if (strstr(VoiceInput.Sentence, VoicePhrase[i][j].SentenceJP)!=NULL){
                VoicePhrase[i][j].State=1;
                EnvSensorData[k].State=j;       //  recognition results i:0-99, ,j:0-999
                printf(" Recognized Word [%d][%d] %s\n",i, j, VoicePhrase[i][j].SentenceJP);
                j=VoiceWordNo[i];
            }
            else
                VoicePhrase[i][j].State=0;
        }
        if (EnvSensorData[k].State>=0){        //  selected in the i-th category
            printf(" Category:%d, Recognized(%d)\n",i, EnvSensorData[k].State);
            if (q==SensorActivated)             //  if the sensing is done (activated, recognized)
                EnvSensorData[k].State=q;
        }
    }
}

void SensorVoiceInput(int n)   //  Virtual Voice Recognition
{
    WordShow=0;     //  No show of other words.
    memset(VoiceInput.Sentence, 0, sizeof(VoiceInput.Sentence));
    printf("Your Voice Input: ");
    scanf("%[^\n]", VoiceInput.Sentence);
    printf(" -- Input: %s\n",VoiceInput.Sentence);
    scanf("%*c");
    EnvSensorData[n].State=1;
    WordShow=1;     //  Finished.
    HumanInfo[HumanID].State=10;    //  spoken.
}


void UtteranceTCP(int r, int u)   // - -  send/receive utterance data   h: Human ID
{
    struct sockaddr_in server;
    int d,
        port,s;

    char    Recog[10]="-";    //  -: Recognition Mode
    
    unsigned long dst_ip;
    
    dst_ip = inet_addr(Utter_ServerIP);
    port   = Utter_TCP_PORT;

    if (u==0){  //  Recognition Mode
        memset(utter, 0, sizeof(utter));
        strncpy(utter, Recog, sizeof(Recog));
    }
        
    if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Fail to Utterance Server\n");
    }
    else{
        memset((char *) &server, 0, sizeof(server));
        server.sin_family      = AF_INET;
        server.sin_addr.s_addr = dst_ip;
        server.sin_port        = htons(port);
        if (connect(s, (struct sockaddr *) &server, sizeof server) < 0) {
            printf("Robot [%d] Fail to Utterance Server\n", r);
        }
        else{
            write(s, utter, strlen(utter));   //  sending data
            memset(utter, 0, sizeof(utter));
            read(s, utter, sizeof(utter));  //  receiving data
            close(s);
        }
        if (u==0)  //  Recognition Mode
            printf(" - [Utterance Server] Robot:%d, Recognized: %s\n", r, utter);
        else
            printf(" - [Utterance Server] Robot:%d, Received: %s\n", r, utter);
    }
}


void VoiceInputTCP(int r, int n)   //  Virtual Voice Recognition
{
    WordShow=0;     //  No show of other words.
    memset(VoiceInput.Sentence, 0, sizeof(VoiceInput.Sentence));
    
    UtteranceTCP(r, 0);
    strncpy(VoiceInput.Sentence, utter, sizeof(utter));
    
    printf("Your Voice Input: ");
    printf(" -- Input: %s\n",VoiceInput.Sentence);
    EnvSensorData[n].State=1;
    WordShow=1;     //  Finished.
    HumanInfo[HumanID].State=10;    //  spoken.
}

#pragma mark -
#pragma mark Init Human/Buggy/Robot data



void init_Human_data(int i)
{
    int k;

    k=(int)(rnd()*4.0);
    if (k==0){
        HumanInfo[i].pos[0]=ObPos[0][0];
        HumanInfo[i].pos[1]=ObPos[0][1]+rnd()*(ObPos[0][3]-ObPos[0][1]);
    }
    else if (k==1){
        HumanInfo[i].pos[0]=ObPos[0][2];
        HumanInfo[i].pos[1]=ObPos[0][1]+rnd()*(ObPos[0][3]-ObPos[0][1]);
    }
    else if (k==2){
        HumanInfo[i].pos[0]=ObPos[0][0]+rnd()*(ObPos[0][2]-ObPos[0][0]);
        HumanInfo[i].pos[1]=ObPos[0][1];
    }
    else {
        HumanInfo[i].pos[0]=ObPos[0][0]+rnd()*(ObPos[0][2]-ObPos[0][0]);
        HumanInfo[i].pos[1]=ObPos[0][3];
    }
    HumanInfo[i].VX=rndn()*3;
    HumanInfo[i].VY=rndn()*3;
    HumanInfo[i].State=-1;
    HumanInfo[i].pos[2]=0.3;
    HumanInfo[i].sizes[0]=0.06;    //  sizes
    HumanInfo[i].sizes[1]=0.4;    //  sizes
    HumanInfo[i].sizes[2]=0.1;    //  sizes
}

void InitHuman()      //  Init Human Movement
{
    int i,j,k;

    for (i=0; i<HumanNo; i++){
        k=80+i;
        HumanInfo[i].ID=k;
        DeviceReID[k]=i;     //  k: Human ID in theater
        DeviceReType[k]=4;    //  4: People
        
        HumanInfo[i].Interval=0;
        HumanInfo[i].t=0;
        HumanInfo[i].Valid=0;
        HumanInfo[i].State=0;
    }
}

void InitBuggy()      //  Init Buggy
{
    int i,j,k;

    for (i=0; i<BuggyNo; i++){
        k=70+i;
        BuggyInfo[i].ID=k;
        DeviceReID[k]=i;     //  k: Buggy ID in theater
        DeviceReType[k]=3;    //  3: Buggy
        
        BuggyInfo[i].t=0;
        BuggyInfo[i].Valid=0;
        BuggyInfo[i].State=0;
    }
}

void InitRobot()      //  Init Virtual Robot Simulation
{
    int i,k;

    for (i=0; i<RobotNo; i++){
        k=60+i;
        RobotInfo[i].ID=k;
        DeviceReID[k]=i;     //  k: Robot ID in theater
        DeviceReType[k]=2;    //  2: Communication Robots
        
        RobotInfo[i].t=0;
        RobotInfo[i].Valid=0;
        RobotInfo[i].State=0;
    }
}

