


void ReadPostures()    //  read Posture Information - - - - -
{
    FILE *fp;
    int i,j,k;
    double d;
    
    char tn[50]="data/dataPostures.txt";
    if ((fp=fopen(tn,"r"))==NULL){
        printf("Cannot open %s file\n", tn);
        exit(1);
    }

    for (i=0; i<2; i++) {  //  0: Walking, 1: Stopping
        for (j=0; j<HumanPostureLen[i]; j++)
            for (k=0; k<HumanJointsNo; k++)
                HumanPostureData[i][j][k]=0;
        HumanPostureLen[i]=1;
    }
    
    fscanf(fp,"%d\t",&HumanPostureNo);   //  the number of additional postures/gestures
    
    for (i=10; i<10+HumanPostureNo; i++){   //  0: Walking, 1: Stopping
        if (fscanf(fp,"%d\n", &HumanPostureLen[i])==EOF)
            HumanPostureNo=i-10;
        else{
            fscanf(fp,"%[^\n]\n", HumanPostureName[i]);
            printf(" Posture[%d]: %d, %s\n",
                   i, HumanPostureLen[i], HumanPostureName[i]);
            for (j=0; j<HumanPostureLen[i]; j++){
                for (k=0; k<HumanJointsNo; k++){
                    fscanf(fp,"%lf,", &d);
                    HumanPostureData[i][j][k]=PI*d;
                    printf("P[%d][%d][%d]:%4.3f, ",
                           i,j,k,HumanPostureData[i][j][k]);
                }
                printf("\n");
            }
        }
    }
    printf("Total Posture No:%d\n",HumanPostureNo);
    fclose(fp);
}

static void makeHumanSensorNode(int h, dReal x, dReal y, dReal z)
{
    dReal radius = 0.2, length=0.8;
    dMass m;
    dMassSetZero (&m);
    BodyNode[h].body= dBodyCreate (world);
    dMassSetCylinder(&m, 0.225, 3, radius, length);
    dBodySetMass (BodyNode[h].body,&m);
    dBodySetPosition (BodyNode[h].body, x, y, z);
    BodyNode[h].geom = dCreateCylinder(space, radius, length);
    dGeomSetBody(BodyNode[h].geom, BodyNode[h].body);
    
    int i;
    dReal   angle_x = 0,
            angle_y = 0,
            angle_z = 0;
    dMatrix3 R1, R2, R3, R10, R20, R30;
    dVector3 b, d;   // z: Pos, b: base, d:  direction
    
    const dReal *R0  = dBodyGetRotation(BodyNode[h].body);
    const dReal *pos = dBodyGetPosition(BodyNode[h].body);

    for (i=0; i<HumanSensorNo; i++){
        HumanSensor[h][i].geom = dCreateRay( space, HumanRange);
        dGeomSetBody(HumanSensor[h][i].geom, BodyNode[h].body);
        
        angle_x=(double)(i-4)*angleRange;
        angle_y=PI*HumanInfo[h].Angle;
        angle_z=HumanInfo[h].Dir;
        dRFromAxisAndAngle(R1, 1, 0, 0, angle_x); // x
        dRFromAxisAndAngle(R2, 0, 1, 0, angle_y); // y
        dRFromAxisAndAngle(R3, 0, 0, 1, angle_z); // z

        dMultiply0(R10, R1,  R0, 3, 3, 3);
        dMultiply0(R20, R2, R10, 3, 3, 3);
        dMultiply0(R30, R3, R20, 3, 3, 3);

        dGeomSetOffsetRotation( HumanSensor[h][i].geom, R30 );
        
//        dGeomRayGet(HumanSensor[h][i].geom, b, d);
//        printf("[%d][%d] (%f,%f,%f) - (%f,%f,%f) - (%f,%f,%f)\n", h, i,
//               x, y, z, pos[0], pos[1], pos[2], b[0], b[1], b[2]);
        
    }
}

void HumanTargetSelection(int h)  //    target Selection from the all target list
{
    int k;
    k=(int)(TargetNo*rnd());
    HumanInfo[h].TarPos[0]=TargetInfo[k].TarPos[0];
    HumanInfo[h].TarPos[1]=TargetInfo[k].TarPos[1];
    HumanInfo[h].TarID=TargetInfo[k].ID;
    printf("-Human[%d] TargetID:%d, TarPos(%f,%f)\n",
           h, HumanInfo[h].TarID,
           HumanInfo[h].TarPos[0], HumanInfo[h].TarPos[1]);
}

void HumanTargetSelection2(int h)  //    target Selection from each category according to DeviceReID
{
    int i,j,k,g;
    i=HumanInfo[h].Persona;
    j=HumanInfo[h].Task;
    g=PersonaInfo[i].Model[j];  //  Global Target ID
    k=DeviceReID[g];
    switch (DeviceReType[g]){
        case 0:     //  Furniture
            HumanInfo[h].TarPos[0]=FurnitureInfo[k].TarPos[0];
            HumanInfo[h].TarPos[1]=FurnitureInfo[k].TarPos[1];
            break;
        case 2:     //  Communication robot
            HumanInfo[h].TarPos[0]=RobotInfo[k].TarPos[0];
            HumanInfo[h].TarPos[1]=RobotInfo[k].TarPos[1];
            break;
    }
    HumanInfo[h].TarID=g;
    printf("Human[%d] Persona:%d, TaskID:%d/%d, TargetID:%d/%d, TarPos(%f,%f)\n",
           h, i, j, PersonaInfo[i].TaskNo, k, HumanInfo[h].TarID,
           HumanInfo[h].TarPos[0], HumanInfo[h].TarPos[1]);
}

void HumanTargetFin(int h)  //    target Selection after a human reaches its i-th target
{
    int i,j,k,g;
    double dx,dy,tdir;
    i=HumanInfo[h].Persona;
    j=HumanInfo[h].Task;
    g=PersonaInfo[i].Model[j];  //  Global Target ID
    k=DeviceReID[g];
    switch (DeviceReType[g]){
        case 0:     //  Furniture
            HumanInfo[h].Mode=FurnitureInfo[k].HumanPosture;
            HumanInfo[h].Time=0;
            HumanInfo[h].PostureStage=0;
            
            dx=FurnitureInfo[k].Pos[0]-FurnitureInfo[k].TarPos[0];
            dy=FurnitureInfo[k].Pos[1]-FurnitureInfo[k].TarPos[1];
            tdir=atan2(dy,dx);   //  Target Tracing:  Target Direction
            
            HumanInfo[h].Dir=tdir;
            HumanInfo[h].TarDir=-tdir;
            for (j=0; j<2; j++){
                HumanInfo[h].NextPos[j]=HumanInfo[h].Pos[j];
                HumanInfo[h].Pos[j]=FurnitureInfo[k].TarPos[j];
            }
            break;
        case 2:     //  Communication robot
            HumanInfo[h].Mode=RobotInfo[k].HumanPosture;
            HumanInfo[h].Time=0;
            HumanInfo[h].PostureStage=0;
            
            dx=RobotInfo[k].Pos[0]-RobotInfo[k].TarPos[0];
            dy=RobotInfo[k].Pos[1]-RobotInfo[k].TarPos[1];
            tdir=atan2(dy,dx);   //  Target Tracing:  Target Direction
            
            HumanInfo[h].Dir=tdir;
            HumanInfo[h].TarDir=-tdir;
            for (j=0; j<2; j++){
                HumanInfo[h].NextPos[j]=HumanInfo[h].Pos[j];
                HumanInfo[h].Pos[j]=RobotInfo[k].TarPos[j];
            }
            break;
    }
    
    if (HumanInfo[h].Persona==0){
        HumanTargetSelection(h);
    }
    else{
        i=HumanInfo[h].Persona;
        HumanInfo[h].Task++;
        if (HumanInfo[h].Task>=PersonaInfo[i].TaskNo)
            HumanInfo[h].Task=0;
        HumanTargetSelection2(h);
    }
}

void init_Human(int i) //  initialize each person without Persona model (random movement)
{
    int j,k;
    double a=0.95;

    HumanInfo[i].Mode=0;        //  behavior mode,  0: Walking, 1: communication/stop,  2-: Postures / Gestures
    HumanInfo[i].Time=0;
    HumanInfo[i].Stage=1;  //  moving direction stage
    HumanInfo[i].PostureStage=0;
    HumanInfo[i].MaxTime=50+(int)(rnd()*10); //  max discrete range
    HumanInfo[i].Angle=0.65;
    HumanInfo[i].Task=-1;           //  random target selection
    HumanInfo[i].Persona=1+i%PersonaNo;     //  Persona ID :0  random target selection
    for (j=0;j<HumanJointsNo;j++)
        HumanInfo[i].Joint[j]=0;
    k=i%4;
    if (k==0){
        HumanInfo[i].Pos[0]=ObPos[0][0]*a+rnd()*0.1;
        HumanInfo[i].Pos[1]=ObPos[0][1]+(ObPos[0][3]-ObPos[0][1])*rnd();
        HumanInfo[i].Dir=rndn()*0.1;            // init moving direction (angle)
    }
    else if (k==1){
        HumanInfo[i].Pos[0]=ObPos[0][2]*a-rnd()*0.1;
        HumanInfo[i].Pos[1]=ObPos[0][1]+(ObPos[0][3]-ObPos[0][1])*rnd();
        HumanInfo[i].Dir=PI+rndn()*0.1;            // init moving direction (angle)
    }
    else if (k==2){
        HumanInfo[i].Pos[1]=ObPos[0][1]*a+rnd()*0.1;
        HumanInfo[i].Pos[0]=ObPos[0][0]+(ObPos[0][2]-ObPos[0][0])*rnd();
        HumanInfo[i].Dir=PI*0.5+rndn()*0.1;            // init moving direction (angle)
    }
    else {
        HumanInfo[i].Pos[1]=ObPos[0][3]*a-rnd()*0.1;
        HumanInfo[i].Pos[0]=ObPos[0][0]+(ObPos[0][2]-ObPos[0][0])*rnd();
        HumanInfo[i].Dir=-PI*0.5+rndn()*0.1;            // init moving direction (angle)
    }

    HumanInfo[i].Pos[2]=0;                    // z
    
    HumanTargetSelection(i);
    
    makeHumanSensorNode(i, HumanInfo[i].Pos[0], HumanInfo[i].Pos[1], HumanInfo[i].Pos[2]);
    
}

void init_Human2(int i) //  initialize each person with Persona ID
{
    int j,k;
    double a=0.95;

    HumanInfo[i].Mode=0;        //  behavior mode,  0: Walking, 1: communication/stop,  2-: Postures / Gestures
    HumanInfo[i].Time=0;
    HumanInfo[i].Stage=1;  //  moving direction stage
    HumanInfo[i].PostureStage=0;
    HumanInfo[i].MaxTime=50+(int)(rnd()*10); //  max discrete range
    HumanInfo[i].Angle=0.65;
    HumanInfo[i].Task=0;            //  current target ID
    HumanInfo[i].Persona=1+i%PersonaNo;     //  Persona ID 1 -
    for (j=0;j<HumanJointsNo;j++)
        HumanInfo[i].Joint[j]=0;
    k=i%4;
    if (k==0){
        HumanInfo[i].Pos[0]=ObPos[0][0]*a+rnd()*0.1;
        HumanInfo[i].Pos[1]=ObPos[0][1]+(ObPos[0][3]-ObPos[0][1])*rnd();
        HumanInfo[i].Dir=rndn()*0.1;            // init moving direction (angle)
    }
    else if (k==1){
        HumanInfo[i].Pos[0]=ObPos[0][2]*a-rnd()*0.1;
        HumanInfo[i].Pos[1]=ObPos[0][1]+(ObPos[0][3]-ObPos[0][1])*rnd();
        HumanInfo[i].Dir=PI+rndn()*0.1;            // init moving direction (angle)
    }
    else if (k==2){
        HumanInfo[i].Pos[1]=ObPos[0][1]*a+rnd()*0.1;
        HumanInfo[i].Pos[0]=ObPos[0][0]+(ObPos[0][2]-ObPos[0][0])*rnd();
        HumanInfo[i].Dir=PI*0.5+rndn()*0.1;            // init moving direction (angle)
    }
    else {
        HumanInfo[i].Pos[1]=ObPos[0][3]*a-rnd()*0.1;
        HumanInfo[i].Pos[0]=ObPos[0][0]+(ObPos[0][2]-ObPos[0][0])*rnd();
        HumanInfo[i].Dir=-PI*0.5+rndn()*0.1;            // init moving direction (angle)
    }

    HumanInfo[i].Pos[2]=0;                    // z

    HumanTargetSelection2(i);

    makeHumanSensorNode(i, HumanInfo[i].Pos[0], HumanInfo[i].Pos[1], HumanInfo[i].Pos[2]);
    
}

void init_Human3(int i) //  initialize each person with Persona ID and initial Target Object
{
    int j,k;
    double a=0.95;

    HumanInfo[i].Mode=0;        //  behavior mode,  0: Walking, 1: communication/stop,  2-: Postures / Gestures
    HumanInfo[i].Time=0;
    HumanInfo[i].Stage=1;  //  moving direction stage
    HumanInfo[i].PostureStage=0;
    HumanInfo[i].MaxTime=50+(int)(rnd()*10); //  max discrete range
    HumanInfo[i].Angle=0.65;
    HumanInfo[i].Task=0;            //  current target ID
    HumanInfo[i].Persona=1+i%PersonaNo;     //  Persona ID 1 -
    for (j=0;j<HumanJointsNo;j++)
        HumanInfo[i].Joint[j]=0;
    
    HumanTargetSelection2(i);
    HumanInfo[i].Pos[0]=HumanInfo[i].TarPos[0]; //  initial position <- first target object
    HumanInfo[i].Pos[1]=HumanInfo[i].TarPos[1];
    HumanInfo[i].Pos[2]=0;                    // z
    HumanInfo[i].Dir=rndn()*0.1;            // init moving direction (angle)

    HumanInfo[i].Task++;
    HumanTargetSelection2(i);
    
    makeHumanSensorNode(i, HumanInfo[i].Pos[0], HumanInfo[i].Pos[1], HumanInfo[i].Pos[2]);
    
}



void init_People_Selection(int i)
{
    if (i<8)
        init_Human3(i); //  Persona Model
    else
        init_Human(i);  //  random target object selection
}

void init_People()      //  initialize people in the beginning
{
    int i,k;

    for (i=0;i<PeopleNo;i++){
        k=80+i;
        HumanInfo[i].ID=k;
        DeviceReID[k]=i;     //  k: Human ID in theater
        DeviceReType[k]=4;    //  4: People
        HumanInfo[i].Language=6;
        HumanInfo[i].TheaterID=-1;
        init_People_Selection(i);
        printf("Human[%d] Persona:%d, Pos(%f,%f)\n",
               i, HumanInfo[i].Persona,
               HumanInfo[i].Pos[0], HumanInfo[i].Pos[1]);
    }
}

void ReadPersona()    //  read Persona Information - - - - -
{
    FILE *fp;
    int h,i,j,k;
    
    char    s[500],
            tn[50]="data/dataPersona.txt";
    
    if ((fp=fopen(tn,"r"))==NULL){
        printf("Cannot open %s file\n", tn);
        exit(1);
    }
    
    printf("\n");
    fscanf(fp,"%[^\n]\n", s);
    
    fscanf(fp,"%d\t",&PeopleNo);
    fscanf(fp,"%d\t",&PersonaNo);
    for (i=1; i<PersonaNo; i++){        //  start with 1, (0: random target selection)
        if (fscanf(fp,"%d\t",&k)==EOF)
            PersonaNo=i;
        else{
            PersonaInfo[i].TaskNo=k;      //  k: sensor ID in a scenario/database
            printf("Persona[%d] %d, Target: ",i,k);
            for (j=0; j<k; j++){
                fscanf(fp,"%d\t",&PersonaInfo[i].Model[j]);
                printf(" [%d]:%d, ",
                       j, PersonaInfo[i].Model[j]);
            }
            fscanf(fp,"%[^\n]\n", PersonaInfo[i].Name);
            printf("Name:%s\n", PersonaInfo[i].Name);
        }
    }
    printf("Total Persona No:%d\n",PersonaNo);
    fclose(fp);
    
}


void ReadTheater()    //  read Theater Information - - - - -
{
    FILE *fp;
    int i,j,k,d;
    
    char tn[30]="data/dataPersona.txt";
    
    if ((fp=fopen(tn,"r"))==NULL){
        printf("Cannot open %s file\n", tn);
        exit(1);
    }
    
    printf("\n");
    fscanf(fp,"%d\t",&PersonaNo);
    for (i=0; i<EnvSensorNo; i++){
        if (fscanf(fp,"%d\t",&k)==EOF)
            EnvSensorNo=i;
        else{
            DeviceReID[k]=i;           //  i: sensor ID in this PC
            if (k==issID)
                issID=i;
            EnvSensorInfo[i].ID=k;      //  k: sensor ID in a scenario/database
            fscanf(fp,"%d\t",&d);
            EnvSensorInfo[i].Pos[0]=(double)(d)/100.0+ObPos[0][0];
            fscanf(fp,"%d\t",&d);
            EnvSensorInfo[i].Pos[1]=(double)(d)/100.0+ObPos[0][1];
            fscanf(fp,"%d\t",&EnvSensorInfo[i].Type);
            fscanf(fp,"%[^\n]\n", EnvSensorInfo[i].Name);
            EnvSensorInfo[i].Valid=1;
            printf("Sensor[%d] ID:%d, %s (%f,%f) Valid:%d, Type:%d\n",
                   i, EnvSensorInfo[i].ID, EnvSensorInfo[i].Name,
                   EnvSensorInfo[i].Pos[0],EnvSensorInfo[i].Pos[1],
                   EnvSensorInfo[i].Valid, EnvSensorInfo[i].Type);
        }
    }
    printf("Total Sensor No:%d\n",EnvSensorNo);
    fclose(fp);
}



void ReadSentence(int t)    //  read sentences - - - - - - - -
{
    FILE *fp;
    int h,i,j,k,m,n;
    char s[3]={'\0'};
    if ((fp=fopen(TheaterInfo[t].TheaterFileName,"r"))==NULL){
        printf("Cannot open %s file\n", TheaterInfo[t].TheaterFileName);
        exit(1);
    }
    
    n=0;    //  Utterance ID
    printf("\n");
    for (i=0; i<PHASE; i++){
        fscanf(fp,"%d\n",&SectionNo[i]);    // Number of Sections in PhaseID
        printf("total Section No[%d]:%d\n",
               i, SectionNo[i]);
        for (j=0; j<SectionNo[i]; j++){
            fscanf(fp,"%d\n",&ScenarioNo[i][j]);    // Number of Scenarios in SectionID
            printf("total Scenario No[%d][%d]:%d\n",
                   i,j, ScenarioNo[i][j]);
            for (k=0; k<ScenarioNo[i][j]; k++){
                fscanf(fp,"%d\t",&ScenarioInfo[i][j][k].No);   // Number of Senetences in ScenarioID
                fscanf(fp,"%[^\n]\n",ScenarioInfo[i][j][k].Name);    //  Name (title) of Scenario
                printf("total Sentence No[%d][%d][%d]:%d, %s\n",
                       i,j,k, ScenarioInfo[i][j][k].No, ScenarioInfo[i][j][k].Name);
                for (h=0; h<ScenarioInfo[i][j][k].No; h++){
                    fscanf(fp,"%d\t",&ScenarioInfo[i][j][k].ID[h]);   // RobotID
                    fscanf(fp,"%[^\t]\t",ScenarioInfo[i][j][k].Sentence[h]);    //  Utterance in EN
                    strcat(ScenarioInfo[i][j][k].Sentence[h],s);
                    fscanf(fp,"%[^\n]\n",ScenarioInfo[i][j][k].SentenceJP[h]);  //  Utterance in JP
                    strcat(ScenarioInfo[i][j][k].SentenceJP[h],s);
                    ScenarioInfo[i][j][k].UtteranceID[h]=n;    // Utterance ID
                    n++;
//                    printf("[%d][%d][%d][%d]: Robot[%d] %s,  %s\n",
//                           i,j,k,h,ScenarioInfo[i][j][k].RobotID[h],
//                           ScenarioInfo[i][j][k].Sentence[h], ScenarioInfo[i][j][k].SentenceJP[h]);
                }
            }
        }
    }
    fclose(fp);
    
//    WriteRuleTemp(TheaterInfo[t].TheaterFileName);  //  generate sample rule transition template file
    
}




void HumanConTCP(int h){    // - -  send/receive data   h: Human ID

    struct sockaddr_in server;
    unsigned long dst_ip;
    int d,
        port,s;

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
        if (ServerMode==1)
            printf("Fail to Robot Theater Server\n");
        else
            printf("Fail to Macro GNG Server\n");
    }
    else{
        memset((char *) &server, 0, sizeof(server));
        server.sin_family      = AF_INET;
        server.sin_addr.s_addr = dst_ip;
        server.sin_port        = htons(port);
        if (connect(s, (struct sockaddr *) &server, sizeof server) < 0) {
//            printf(" Human [%d] Mode:%d No Theater Server - State: Not connected (0)\n",
//                   h, HumanInfo[h].State);
            if (ServerMode==1)
                printf("Human [%d] State:%d, Fail to Robot Theater Server\n",
                        h, HumanInfo[h].State);
            else
                printf("Human [%d] Fail to Macro GNG Server\n",h);
            HumanInfo[h].State=0;
        }
        else{
            write(s, buf, strlen(buf));   //  sending data
            memset(buf, 0, sizeof(buf));
            read(s, buf, sizeof(buf));  //  receiving data
            close(s);
//            printf("Received Data: %s\n", buf);

            //  Extraction of Robot Information
            if (HumanInfo[h].State==0){
                HumanInfo[h].State=1;              // connected:   1: moving, 2: stopped,
                printf("Human [%d] Connected now - State:%d\n",
                       h, HumanInfo[h].State);
            }
            else if (HumanInfo[h].State==1){
                memset(codeno, 0, sizeof(codeno));
                codeno[0]=buf[0];   //  State Changed ?
                codeno[1]=buf[1];
                codeno[2]=buf[2];
                d = atoi(codeno);
                printf("S1: Human [%d] received:%d, %s\n",
                       h, d, buf);
                if (d==996){
                    HumanInfo[h].State=2;       // connected:   1: moving, 2: stopped
                    HumanInfo[h].Mode=1;        // 1: stoped
                    printf("S1: Human [%d] Stopped at the robot - State:%d, Mode:%d, TheaterID:%d\n",
                       h, HumanInfo[h].State, HumanInfo[h].Mode, HumanInfo[h].TheaterID);
                }
            }
            else if (HumanInfo[h].State==2){
                memset(codeno, 0, sizeof(codeno));
                codeno[0]=buf[0];   //  Robot Theater ID
                codeno[1]=buf[1];
                codeno[2]=buf[2];
                d = atoi(codeno);
                if (d==999){
                    printf("S2: Human [%d] No theater\n",h);
                    HumanInfo[h].TheaterID=-1;
                }
                else if (d==996){
                    HumanInfo[h].Mode=1;              // connected:   1:
                    printf("S2: Human [%d] Stopped at the robot - State:%d\n",
                            h, HumanInfo[h].State);
                }
                else{
                    HumanInfo[h].TheaterID=d;
                    HumanInfo[h].State=HumanInfo[h].Language;   //  Human Language
                    printf("S2: Human [%d] TheaterID:%d, Language :%d\n",
                    h, HumanInfo[h].TheaterID, HumanInfo[h].Language);
            //                    ReadSentence(TheaterID);
                }
                printf("Human [%d] Connected - State:%d, Data:%d, %s\n",
                        h, HumanInfo[h].State, d, buf);
            }
            else if (HumanInfo[h].State==3){    //  to be updated
                memset(codeno, 0, sizeof(codeno));
                codeno[0]=buf[0];   //  Robot Theater ID
                codeno[1]=buf[1];
                codeno[2]=buf[2];
                d = atoi(codeno);
                if (d==999){
                    HumanInfo[h].TheaterID=-1;
                    HumanInfo[h].State=1;       // connected:   1: moving, 2: stopped
                }
                else{
                    HumanInfo[h].State=HumanInfo[h].Language;   //  Human Language
//                    ReadSentence(TheaterID);
                }
                printf("S3: Human [%d] Connected - State:%d\n",
                       h, HumanInfo[h].State);
            }
            else if (HumanInfo[h].State==HumanInfo[h].Language){
                memset(codeno, 0, sizeof(codeno));
                codeno[0]=buf[0];   //  Robot Theater ID
                codeno[1]=buf[1];
                d= atoi(codeno);
                printf("S%d: Human [%d] Data:%d, Mode:%d\n",
                       HumanInfo[h].Language, h, d,
                       HumanInfo[h].Mode);
                
                if (d==99){
                    printf("S%d: Human [%d] No theater\n",
                           HumanInfo[h].Language, h);
                    HumanInfo[h].TheaterID=-1;
                    HumanInfo[h].State=1;       // connected:   1: moving, 2: stopped
                    HumanInfo[h].Mode=0;        // 0: Walk 1: stoped
                    printf("Theater Finished. Human [%d] Walk again - State:%d, Mode:%d\n",
                       h, HumanInfo[h].State, HumanInfo[h].Mode);
                }
                else if (d<98){
                    HumanInfo[h].PhaseID=d;
                    memset(codeno, 0, sizeof(codeno));
                    codeno[0]=buf[2];   //  Robot Utterance ID
                    codeno[1]=buf[3];
                    HumanInfo[h].SectionID = atoi(codeno);
                    memset(codeno, 0, sizeof(codeno));
                    codeno[0]=buf[4];   //  Robot Utterance ID
                    codeno[1]=buf[5];
                    HumanInfo[h].ScenarioID = atoi(codeno);
                    memset(codeno, 0, sizeof(codeno));
                    codeno[0]=buf[6];   //  Robot Utterance ID
                    codeno[1]=buf[7];
                    HumanInfo[h].SentenceID = atoi(codeno);
                    HumanInfo[h].State=10;
                    printf("S%d  Human [%d]  Reply to the Robot: [%d][%d][%d][%d] in [%d]\n",
                           HumanInfo[h].State, h, HumanInfo[h].PhaseID,
                           HumanInfo[h].SectionID, HumanInfo[h].ScenarioID,
                           HumanInfo[h].SentenceID, HumanInfo[h].Language);
                }
            }
            else if (HumanInfo[h].State==10){      // spoken, ready to speak
                HumanInfo[h].State=11;
                printf("S%d Human [%d]  Replied to the Robot\n",
                HumanInfo[h].State, h);
            }
            else if (HumanInfo[h].State==11){      // spoken, ready to speak
                HumanInfo[h].State=HumanInfo[h].Language;
                printf("S%d Human [%d]  Ready to speak\n",
                HumanInfo[h].State, h);}
            
//            if (HumanInfo[h].TheaterID==-1)
//                HumanInfo[h].State=1;       // connected:   1: moving, 2: stopped
        }
    }
}

void sendHumandata(int h)    // h: human ID,  send Mode and State data to Robot Contents Server
{
    int        port;
    int        i,j,k,m,v;

    char    numberName[20]="01234567890";

    memset(buf, 0, sizeof(buf));

    i=HumanInfo[h].ID;  //  i: global Human ID, h: Human ID in this PC
    
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

    m=HumanInfo[h].State;        //  Send State
    if (m>=10) {
        k=(int)(m/10)%10;
        buf[3]=numberName[k]; // State ID
    }
    else
        buf[3]=numberName[0]; // State ID
    k=m%10;
    buf[4]=numberName[k]; // State ID
    
    for (i=0; i<3; i++){
        v=(int)((HumanInfo[h].Pos[i]+50.0)*100.0);
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

//    printf("Sent Data: %s\n", buf);
    HumanConTCP(h);             //  sending and receiving trough buf[]
}



#pragma mark -
#pragma mark Fuzzy Control and Human Movement

double humanTargetTracing(int h)
{
    int i,j,k;
    double  dx, dy,
            dang=0, tdir,
            TarTraceKP=0.05;
    
    dx=HumanInfo[h].TarPos[0]-HumanInfo[h].Pos[0];
    dy=HumanInfo[h].TarPos[1]-HumanInfo[h].Pos[1];
    
    if (dx*dx+dy*dy<0.4){   //  reach the target, and select its next target point
        HumanTargetFin(h);
    }
    else {
        tdir=atan2(dy,dx);   //  Target Tracing:  Target Direction
        if (tdir>PI)
            tdir-=PI*2;
        else if (tdir<-PI)
            tdir+=PI*2;
        
        dang=tdir-HumanInfo[h].Dir;        //  Target Tracing:   steering direction
        if (dang>PI)
            dang-=PI*2;
        else if (dang<-PI)
            dang+=PI*2;
        dang*=TarTraceKP;        //  Target Tracing:   steering direction
    }
    return(dang);
}

void humanPostureControl(int h)     //  h: human ID
{
    int i,j,k,m,n;
    double  alpha=0.03,  //  gain for stopping behavior
            thres1=0.2,  //  threshold for stopping
            thres2=0.08,  //  threshold for stopping
            e=0;   //  estimation errors
    
    m=HumanInfo[h].Mode;
    n=HumanInfo[h].PostureStage;
    
    for (i=0; i< HumanJointsNo; i++){
        HumanInfo[h].Joint[i]-=alpha*(HumanInfo[h].Joint[i]-HumanPostureData[m][n][i])+rndn()*0.002;    // with random noise
        e+=fabs(HumanInfo[h].Joint[i]-HumanPostureData[m][n][i]);
    }
//    printf("Human[%d]: (%d,%d), e:%3.2f, angle[12]:%f3.2f \n",h,m,n,e, HumanInfo[h].Joint[12]);
    if (n<HumanPostureLen[m]-1){
//        printf("Human[%d]: (%d,%d) error: %f\n",h,m,n,e);
        if (e<thres1)               //  n-th stage is finished
            HumanInfo[h].PostureStage++;
    }
    else if (e<thres2){               //  n-th stage is finished
        HumanInfo[h].Mode=3;
        HumanInfo[h].PostureStage=0;
//        for (j=0; j<2; j++)
//            HumanInfo[h].Pos[j]=HumanInfo[h].NextPos[j];
    }
}


void fuzzyInf(int h)       // Fuzzy inference, h: human ID
{
    int     i,j;
    double  d, dang,
            x[MEMFS+1],                 //  Inputs
            Firing[RULES+1],
            maxfit=0,                   //  max fitness
            TotalFiring=0.0000001;      // total output, total firing (weight)
    
    for (j=0; j<memf; j++)
        x[j]=HumanDist[h][j];   //  input to fuzzy if-then rules
    
    for (i=0;i<ruls;i++){
        Firing[i]=1.0;
        for (j=0; j<memf; j++){
            if (FuzzyIf[i][j]==0){
                d=exp(-x[j]*x[j]);
                Firing[i]*=d;
                if (maxfit<d)
                    maxfit=d;
            }
            else
                Firing[i]*=exp(-(x[j]-HumanRange)*(x[j]-HumanRange));
        }
        TotalFiring+=Firing[i];
    }
    FuzzyOut=0;
    for (i=0;i<ruls;i++)
        FuzzyOut+=Firing[i]*FuzzyThen[i];
    FuzzyOut/=TotalFiring;
    HumanInfo[h].Dir+=FuzzyOut;     //  Collision avoidance
    
    dang=humanTargetTracing(h);     //  target tracing
    if (maxfit<0.3)
        HumanInfo[h].Dir+=dang;
    
}

void humanMove()    //  human movement
{
    int p,h,i,j,
        m,  //  HumanInfo[h].Mode;
        s,  //  HumanInfo[h].State;
        n;
    double  alpha=0.3,  //  gain for stopping behavior
            thres=0.05,  //  threshold for stopping
            x[10],
            pep[HumanJointsNo],   //    joint angle
            e;   //  estimation errors
    
    for (h=0; h<PeopleNo; h++){       //  PeopleNo
        if  ((HumanInfo[h].Dist<HumanRange)&&(HumanInfo[h].Angle<0.75))
            HumanInfo[h].Angle+=0.002;
        else if (HumanInfo[h].Angle>0.65)
            HumanInfo[h].Angle-=0.002;
        m=HumanInfo[h].Mode;
        s=HumanInfo[h].State;
        if ((m==-1)&&(rnd()<0.01)){
            HumanInfo[h].Mode=0;
            HumanInfo[h].Time=0;
            HumanInfo[h].PostureStage=0;
        }
//        else if ((m==-1)&&(rnd()<0.005)){
//            HumanInfo[h].Mode=1+(int)(rnd()*(HumanPostureNo-1));
//            HumanInfo[h].Time=0;
//            HumanInfo[h].PostureStage=0;
//        }
        
        else if ((m==0)&&(HumanInfo[h].Dist<0.6)&&(rnd()<0.1)){
            HumanInfo[h].Mode=1;            //  move to a stopping posture from walking in dangerous state
            HumanInfo[h].Time=0;
            HumanInfo[h].PostureStage=0;
        }
//        else if ((m==1)&&(HumanInfo[h].TheaterID==-1)){            //  Human-Robot Communication
//            HumanInfo[h].Mode=0;            //  move to walking from the stopping posture
//            HumanInfo[h].Time=0;
//            HumanInfo[h].PostureStage=0;
//        }
        
        else if ((m==1)&&(s>5)){            //  Human-Robot Communication
            HumanInfo[h].Mode=10+(int)(rnd()*HumanPostureNo);
            HumanInfo[h].Time=0;
            HumanInfo[h].PostureStage=0;
            for (j=0; j<2; j++)
                HumanInfo[h].NextPos[j]=HumanInfo[h].Pos[j];
        }
        
        else if ((m==1)&&(HumanInfo[h].TheaterID==-1)&&(s<=1)&&(rnd()<0.1)){
            HumanInfo[h].Mode=0;            //  move to walking from the stopping posture
            HumanInfo[h].Time=0;
            HumanInfo[h].PostureStage=0;
        }

        for (i=0; i< HumanJointsNo; i++)
            pep[i]=HumanInfo[h].Joint[i];
        if ((m==0)||(m==2)){  //0: walk
            HumanInfo[h].Time+=HumanInfo[h].Stage;      //  forward / reverse
            if (HumanInfo[h].Time > HumanInfo[h].MaxTime) {
                HumanInfo[h].Time=HumanInfo[h].MaxTime;
                HumanInfo[h].Stage=-HumanInfo[h].Stage;
                x[0]=-HumanBodyData[0][5]*sin(pep[0]+pep[1])-HumanBodyData[1][5]*sin(pep[1])
                     +HumanBodyData[3][5]*sin(pep[2]+pep[3])+HumanBodyData[2][5]*sin(pep[2]);
                HumanInfo[h].Pos[0]+=(x[0])*cos(HumanInfo[h].Dir);   //  x
                HumanInfo[h].Pos[1]+=(x[0])*sin(HumanInfo[h].Dir);   //  y
                if ((HumanInfo[h].Pos[0]<ObPos[0][0]) || (HumanInfo[h].Pos[0]>ObPos[0][2]) ||
                    (HumanInfo[h].Pos[1]<ObPos[0][1]) || (HumanInfo[h].Pos[1]>ObPos[0][3]))
                    init_People_Selection(h);                   //  Outside from the area
            }
            else if (HumanInfo[h].Time < -HumanInfo[h].MaxTime) {
                HumanInfo[h].Time=-HumanInfo[h].MaxTime;
                HumanInfo[h].Stage=-HumanInfo[h].Stage;
                x[0]=HumanBodyData[0][5]*sin(pep[0]+pep[1])+HumanBodyData[1][5]*sin(pep[1])
                    -HumanBodyData[3][5]*sin(pep[2]+pep[3])-HumanBodyData[2][5]*sin(pep[2]);
                HumanInfo[h].Pos[0]+=(x[0])*cos(HumanInfo[h].Dir);   //  x
                HumanInfo[h].Pos[1]+=(x[0])*sin(HumanInfo[h].Dir);   //  y
            }
            for (n=0;n<HumanJointsNo;n++){
                HumanInfo[h].Joint[n]=HumanMoveData[m][n]*HumanInfo[h].Time*alpha;   //  updaete of joint angles
            }
            if (m==0)
                fuzzyInf(h);
            else if (m==2){
                
            }
            
        }
        else if (m==3){  //  Moving towards the next target point (turning)
            e=HumanInfo[h].TarDir-HumanInfo[h].Dir;
            HumanInfo[h].Dir+=e*0.1;
            if (e<0.1){
                HumanInfo[h].Mode=1;
                HumanInfo[h].Time=0;
                HumanInfo[h].PostureStage=0;
            }
        }
        else if (m>=10){  //  Postures / Gestures
            humanPostureControl(h);
        }
    }
    
}

static void makeBodyParts(int h, double d)
{
    int i;
    dReal radius=0.02, length=0.1;
    dMass m;
    dMassSetZero (&m);
    
    BodyPart.body= dBodyCreate (world);
    dMassSetCylinder(&m,0.225,3,radius,length);
    dBodySetMass (BodyPart.body,&m);
    
    dReal angle_x = 0, angle_y = 0, angle_z = 0;
    const dReal *R0;
    dMatrix3 R1, R2, R3, R10, R20, R30;

    R0 = dBodyGetRotation(BodyPart.body);

    angle_y=d;
    angle_z=HumanInfo[h].Dir;
    dRFromAxisAndAngle(R1, 1, 0, 0, angle_x); // x
    dRFromAxisAndAngle(R2, 0, 1, 0, angle_y); // y
    dRFromAxisAndAngle(R3, 0, 0, 1, angle_z); // z

    dMultiply0(R10, R2,  R0, 3, 3, 3);
    dMultiply0(R20, R1, R10, 3, 3, 3);
    dMultiply0(R30, R3, R20, 3, 3, 3);

    dBodySetRotation(BodyPart.body, R30);
    R0 = dBodyGetRotation(BodyPart.body);
    dBodySetPosition(BodyPart.body, 0,3,1);
}


static void makeBodyParts2(int h, double d, double e)   //  2 DOF
{
    int i;
    dReal   radius=0.02, length=0.1;
    dMass m;
    dMassSetZero (&m);
    
    BodyPart.body= dBodyCreate (world);
    dMassSetCylinder(&m,0.225,3,radius,length);
    dBodySetMass (BodyPart.body,&m);
    
    dReal angle_x = 0.0, angle_y = 0.0, angle_z = 0.0;
    const dReal *R0;
    dMatrix3 R1, R2, R3, R10, R20, R30;

    R0 = dBodyGetRotation(BodyPart.body);

    angle_x=e;
    angle_y=PI+d;
    angle_z=HumanInfo[h].Dir;
    dRFromAxisAndAngle(R1, 1, 0, 0, angle_x); // x
    dRFromAxisAndAngle(R2, 0, 1, 0, angle_y); // y
    dRFromAxisAndAngle(R3, 0, 0, 1, angle_z); // z

    dMultiply0(R10, R2,  R0, 3, 3, 3);
    dMultiply0(R20, R1, R10, 3, 3, 3);
    dMultiply0(R30, R3, R20, 3, 3, 3);

    dBodySetRotation(BodyPart.body, R30);
    R0 = dBodyGetRotation(BodyPart.body);
    dBodySetPosition(BodyPart.body, 0,3,1);

}

void HumanSensing(int h)       //  h: human ID,  Sensing
{
    int c,i,j,n,p,
        sensed;         //  0: no sense, 1: sensed
    dReal   Calpha=0.9;
    const dReal* pos, * R;
    dReal   x[3],w,
            posh[HumanPartsNo+1][3],
            posd[HumanPartsNo+1][3],
            posf[HumanPartsNo+1][3],Rf[12];
    dVector3 sides, z, b, d;   // z: Pos, b: base, d:  direction
    dContactGeom SenCol;
    
    for (i=0; i<HumanSensorNo; i++){        //  sensing
        HumanDist[h][i]=HumanRange;
        sensed=0;
        for (j=0; j<PeopleNo; j++){
            if (j!=h){
                c = dCollide(HumanSensor[h][i].geom, BodyNode[j].geom, 1, &SenCol, sizeof( dContactGeom ) );
                if (c > 0){
                    if (HumanDist[h][i] > SenCol.depth){
                        HumanDist[h][i] = SenCol.depth;
                        sensed=1;
                    }
            
                }
            }
        }
        if (RobotHSR==1){
            c = dCollide(HumanSensor[h][i].geom, HSR_Base.geom, 1, &SenCol, sizeof( dContactGeom ) );   // with HSR
            if (c > 0){
                if (HumanDist[h][i] > SenCol.depth){
                    HumanDist[h][i] = SenCol.depth;
                    sensed=1;
                }
            }
        }
        for (j=0; j<BuggyNo; j++){
            c = dCollide(HumanSensor[h][i].geom, Buggy[j][0].geom, 1, &SenCol, sizeof( dContactGeom ) );   // with Buggy
            if (c > 0){
                if (HumanDist[h][i] > SenCol.depth){
                    HumanDist[h][i] = SenCol.depth;
                    sensed=1;
                }
            }
        }
        for (j=0; j<FurnitureNo; j++){  //  furnuture 2:box, 3:chair  FurnitureNo
            if (FurnitureInfo[j].Type>=2){
                c = dCollide(HumanSensor[h][i].geom, EnvFurniture[j].geom, 1, &SenCol, sizeof( dContactGeom ) );   // HSR
                if (c > 0){
                    if (HumanDist[h][i] > SenCol.depth){
                        HumanDist[h][i] = SenCol.depth;
                        sensed=1;
                    }
                }
            }
        }
        
        for (j=0; j<RobotNo; j++){  //  Communication Robots
            c = dCollide(HumanSensor[h][i].geom, RobotC[j].geom, 1, &SenCol, sizeof( dContactGeom ) );
            if (c > 0){
                if (HumanDist[h][i] > SenCol.depth){
                    HumanDist[h][i] = SenCol.depth;
                    sensed=1;
                }
            }
        }
        
        if (SensorShow==1){
            if (HumanDist[h][i] < HumanRange)
                dsSetColorAlpha(1.0, 0.0, 0.0, 0.6);
            else
                dsSetColorAlpha(1.0, 0.0, 0.0, 0.05);
            dGeomRayGet(HumanSensor[h][i].geom, b, d);
            z[0] = b[0] + (d[0] * HumanDist[h][i]);
            z[1] = b[1] + (d[1] * HumanDist[h][i]);
            z[2] = b[2] + (d[2] * HumanDist[h][i]);
            z[3] = b[3] + (d[3] * HumanDist[h][i]);
            dsDrawLine(b, z);
        }
    }
    

    
//    for (j=0; j<FurnitureNo; j++){  //  furnuture 2:box, 3:chair  FurnitureNo
//        if (FurnitureInfo[j].Type>=2){
//              HumanFurniture[h][j]=((HumanInfo[h].Pos[0]-FurnitureInfo[j].pos[0])      // distance to furniture
//                                   *(HumanInfo[h].Pos[0]-FurnitureInfo[j].pos[0])
//                                   +(HumanInfo[h].Pos[1]-FurnitureInfo[j].pos[1])
//                                   *(HumanInfo[h].Pos[1]-FurnitureInfo[j].pos[1]));
//        }
//        if (j>=5){
//            if ((HumanFurniture[h][5]<0.5)||(HumanFurniture[h][6]<0.5)){      //  sitting on a chair
//                 HumanInfo[h].Mode=3;
//                 HumanInfo[h].Time=0;
//                 HumanInfo[h].PostureStage=0;
//            }
//        }
//    }
//    printf("Chair[5]: %4.3f, [6]: %4.3f\n",
//           HumanFurniture[h][5],HumanFurniture[h][6]);
}



void drawPeople(int h)       //  h: human ID,  Sensing
{
    int c,i,j,n,p,
        sensed;         //  0: no sense, 1: sensed
    dReal   Calpha=0.9;
    const dReal* pos, * R;
    dReal   post[3],
            Rt[12]={1,0,0,0, 0,1,0,0, 0,0,1,0};
    dReal   x[3],w,
            pep[HumanJointsNo],
            ang[HumanJointsNo],
            posh[HumanPartsNo+1][3],
            posd[HumanPartsNo+1][3],
            posf[HumanPartsNo+1][3],Rf[12];
    dVector3 sides, z, b, d;   // z: Pos, b: base, d:  direction
    
    dBodyDestroy(BodyNode[h].body);
    dGeomDestroy(BodyNode[h].geom);
    for (i=0; i<HumanSensorNo; i++)
        dGeomDestroy(HumanSensor[h][i].geom);
        
    for (int i=0; i< HumanJointsNo; i++)
        pep[i]=HumanInfo[h].Joint[i];
    
    if (HumanInfo[h].Stage==-1){
        ang[0]=pep[0]+pep[1];
        posh[0][0]=HumanBodyData[0][5]*sin(ang[0]);    //  right lower leg
        posh[0][2]=HumanBodyData[0][5]*cos(ang[0]);
        posd[0][0]=HumanBodyData[0][5]*sin(ang[0])*0.5;
        posd[0][2]=posh[0][2]*0.5;

        ang[1]=pep[1];
        posh[1][0]=posh[0][0]+HumanBodyData[1][5]*sin(ang[1]);    //  right upper leg
        posh[1][2]=posh[0][2]+HumanBodyData[1][5]*cos(ang[1]);
        posd[1][0]=posh[0][0]+HumanBodyData[1][5]*sin(ang[1])*0.5;
        posd[1][2]=posh[0][2]+HumanBodyData[1][5]*cos(ang[1])*0.5;

        ang[2]=pep[2];
        posh[2][0]=posh[1][0]-HumanBodyData[2][5]*sin(ang[2]);    //  left upper leg
        posh[2][2]=posh[1][2]-HumanBodyData[2][5]*cos(ang[2]);
        posd[2][0]=posh[1][0]-HumanBodyData[2][5]*sin(ang[2])*0.5;
        posd[2][2]=posh[1][2]-HumanBodyData[2][5]*cos(ang[2])*0.5;
        
        ang[3]=pep[2]+pep[3];
        posh[3][0]=posh[2][0]-HumanBodyData[3][5]*sin(ang[3]); //  left lower leg
        posh[3][2]=posh[2][2]-HumanBodyData[3][5]*cos(ang[3]);
        posd[3][0]=posh[2][0]-HumanBodyData[3][5]*sin(ang[3])*0.5;
        posd[3][2]=posh[2][2]-HumanBodyData[3][5]*cos(ang[3])*0.5;

        
        ang[4]=pep[4];
        posh[4][0]=posh[1][0]+HumanBodyData[4][5]*sin(ang[4]);     //  main lower body
        posh[4][2]=posh[1][2]+HumanBodyData[4][5]*cos(ang[4]);
        posd[4][0]=posh[1][0]+HumanBodyData[4][5]*sin(ang[4])*0.5;
        posd[4][2]=posh[1][2]+HumanBodyData[4][5]*cos(ang[4])*0.5;
    }
    else {
        ang[3]=pep[2]+pep[3];
        posh[3][0]=HumanBodyData[3][5]*sin(ang[3]); //  left lower leg
        posh[3][2]=HumanBodyData[3][5]*cos(ang[3]);
        posd[3][0]=HumanBodyData[3][5]*sin(ang[3])*0.5;
        posd[3][2]=HumanBodyData[3][5]*cos(ang[3])*0.5;
        
        ang[2]=pep[2];
        posh[2][0]=posh[3][0]+HumanBodyData[2][5]*sin(ang[2]);    //  left upper leg
        posh[2][2]=posh[3][2]+HumanBodyData[2][5]*cos(ang[2]);
        posd[2][0]=posh[3][0]+HumanBodyData[2][5]*sin(ang[2])*0.5;
        posd[2][2]=posh[3][2]+HumanBodyData[2][5]*cos(ang[2])*0.5;
        
        ang[1]=pep[1];
        posh[1][0]=posh[2][0]-HumanBodyData[1][5]*sin(ang[1]);    //  right upper leg
        posh[1][2]=posh[2][2]-HumanBodyData[1][5]*cos(ang[1]);
        posd[1][0]=posh[2][0]-HumanBodyData[1][5]*sin(ang[1])*0.5;
        posd[1][2]=posh[2][2]-HumanBodyData[1][5]*cos(ang[1])*0.5;

        ang[0]=pep[0]+pep[1];
        posh[0][0]=posh[1][0]-HumanBodyData[0][5]*sin(ang[0]);    //  right lower leg
        posh[0][2]=posh[1][2]-HumanBodyData[0][5]*cos(ang[0]);
        posd[0][0]=posh[1][0]-HumanBodyData[0][5]*sin(ang[0])*0.5;
        posd[0][2]=posh[1][2]-HumanBodyData[0][5]*cos(ang[0])*0.5;

        ang[4]=pep[4];
        posh[4][0]=posh[2][0]+HumanBodyData[4][5]*sin(ang[4]);     //  main lower body
        posh[4][2]=posh[2][2]+HumanBodyData[4][5]*cos(ang[4]);
        posd[4][0]=posh[2][0]+HumanBodyData[4][5]*sin(ang[4])*0.5;
        posd[4][2]=posh[2][2]+HumanBodyData[4][5]*cos(ang[4])*0.5;
    }
        
    ang[5]=pep[4]+pep[5];
    posh[5][0]=posh[4][0]+HumanBodyData[5][5]*sin(ang[5]);     //  main upper body
    posh[5][2]=posh[4][2]+HumanBodyData[5][5]*cos(ang[5]);
    posd[5][0]=posh[4][0]+HumanBodyData[5][5]*sin(ang[5])*0.5;
    posd[5][2]=posh[4][2]+HumanBodyData[5][5]*cos(ang[5])*0.5;

    ang[6]=pep[4]+pep[5]+pep[6];
    posh[6][0]=posh[5][0]+HumanBodyData[6][5]*sin(ang[6]);     //  head
    posh[6][2]=posh[5][2]+HumanBodyData[6][5]*cos(ang[6]);
    posd[6][0]=posh[5][0]+HumanBodyData[6][5]*sin(ang[6])*0.5;
    posd[6][2]=posh[5][2]+HumanBodyData[6][5]*cos(ang[6])*0.5;

    for (n=0; n<7; n++){      //  rotation by z
        w=(HumanBodyData[n][2]+HumanBodyData[n][3])*0.5;
        posf[n][0] = HumanInfo[h].Pos[0]+posd[n][0]*cos(HumanInfo[h].Dir)-w*sin(HumanInfo[h].Dir);
        posf[n][1] = HumanInfo[h].Pos[1]+posd[n][0]*sin(HumanInfo[h].Dir)+w*cos(HumanInfo[h].Dir);
        posf[n][2] = HumanInfo[h].Pos[2]+posd[n][2];
    }

    ang[7]=pep[7];
    ang[11]=pep[11];
    w=HumanBodyData[7][5]*1.2;
    x[0]=w              *sin(PI+ang[7]);     //  right upper arm
    x[1]=w*sin(-ang[11])*cos(PI+ang[7]);
    x[2]=w*cos(-ang[11])*cos(PI+ang[7]);
    w=(HumanBodyData[7][2]+HumanBodyData[7][3])*0.5;
    posd[7][0]=posd[5][0]+x[0]*0.5;
    posd[7][1]=w         +x[1]*0.5;
    posd[7][2]=posd[5][2]+x[2]*0.5;
    posf[7][0]=HumanInfo[h].Pos[0]+posd[7][0]*cos(HumanInfo[h].Dir)
                                       -posd[7][1]*sin(HumanInfo[h].Dir);
    posf[7][1]=HumanInfo[h].Pos[1]+posd[7][0]*sin(HumanInfo[h].Dir)
                                       +posd[7][1]*cos(HumanInfo[h].Dir);
    posf[7][2]=HumanInfo[h].Pos[2]+posd[7][2];
    posd[7][0]=posd[5][0]+x[0];
    posd[7][1]=w         +x[1];
    posd[7][2]=posd[5][2]+x[2];
    posh[7][0]=HumanInfo[h].Pos[0]+posd[7][0]*cos(HumanInfo[h].Dir)
                                       -posd[7][1]*sin(HumanInfo[h].Dir);
    posh[7][1]=HumanInfo[h].Pos[1]+posd[7][0]*sin(HumanInfo[h].Dir)
                                       +posd[7][1]*cos(HumanInfo[h].Dir);
    posh[7][2]=HumanInfo[h].Pos[2]+posd[7][2];

    ang[8]=pep[7]+pep[8];
    ang[12]=pep[11]+pep[12];
    w=HumanBodyData[8][5]*1.2;
    x[0]=w              *sin(PI+ang[8]);     //  right upper arm
    x[1]=w*sin(-ang[12])*cos(PI+ang[8]);
    x[2]=w*cos(-ang[12])*cos(PI+ang[8]);
    x[0]*=0.5;
    x[1]*=0.5;
    x[2]*=0.5;
    posf[8][0]=posh[7][0]+x[0]*cos(HumanInfo[h].Dir)-x[1]*sin(HumanInfo[h].Dir);
    posf[8][1]=posh[7][1]+x[0]*sin(HumanInfo[h].Dir)+x[1]*cos(HumanInfo[h].Dir);
    posf[8][2]=posh[7][2]+x[2];
    
    ang[9]=pep[9];
    ang[13]=pep[13];
    w=HumanBodyData[9][5]*1.2;
    x[0]=w              *sin(PI+ang[9]);     //  right upper arm
    x[1]=w*sin(-ang[13])*cos(PI+ang[9]);
    x[2]=w*cos(-ang[13])*cos(PI+ang[9]);
    w=(HumanBodyData[9][2]+HumanBodyData[9][3])*0.5;
    posd[9][0]=posd[5][0]+x[0]*0.5;
    posd[9][1]=w         +x[1]*0.5;
    posd[9][2]=posd[5][2]+x[2]*0.5;
    posf[9][0]=HumanInfo[h].Pos[0]+posd[9][0]*cos(HumanInfo[h].Dir)
                                       -posd[9][1]*sin(HumanInfo[h].Dir);
    posf[9][1]=HumanInfo[h].Pos[1]+posd[9][0]*sin(HumanInfo[h].Dir)
                                       +posd[9][1]*cos(HumanInfo[h].Dir);
    posf[9][2]=HumanInfo[h].Pos[2]+posd[9][2];
    posd[9][0]=posd[5][0]+x[0];
    posd[9][1]=w         +x[1];
    posd[9][2]=posd[5][2]+x[2];
    posh[9][0]=HumanInfo[h].Pos[0]+posd[9][0]*cos(HumanInfo[h].Dir)
                                       -posd[9][1]*sin(HumanInfo[h].Dir);
    posh[9][1]=HumanInfo[h].Pos[1]+posd[9][0]*sin(HumanInfo[h].Dir)
                                       +posd[9][1]*cos(HumanInfo[h].Dir);
    posh[9][2]=HumanInfo[h].Pos[2]+posd[9][2];
    
    ang[10]=pep[9]+pep[10];
    ang[14]=pep[13]+pep[14];
    w=HumanBodyData[10][5]*1.2;
    x[0]=w              *sin(PI+ang[10]);     //  right upper arm
    x[1]=w*sin(-ang[14])*cos(PI+ang[10]);
    x[2]=w*cos(-ang[14])*cos(PI+ang[10]);
    x[0]*=0.5;
    x[1]*=0.5;
    x[2]*=0.5;
    posf[10][0]=posh[9][0]+x[0]*cos(HumanInfo[h].Dir)-x[1]*sin(HumanInfo[h].Dir);
    posf[10][1]=posh[9][1]+x[0]*sin(HumanInfo[h].Dir)+x[1]*cos(HumanInfo[h].Dir);
    posf[10][2]=posh[9][2]+x[2];
    
    dsSetColorAlpha(0.8,0.8,0.5,Calpha);
    for (n=0; n<HumanPartsNo; n++){               //  draw human model  HumanPartsNo
        if (n==4)
            makeHumanSensorNode(h,posf[n][0],posf[n][1],posf[n][2]);    //  dummy body
        
        if (n<7)
            makeBodyParts(h, ang[n]);
        else
            makeBodyParts2(h, ang[n], ang[n+4]);
        R = dBodyGetRotation(BodyPart.body);
        for (j=0; j<12; j++)
            Rf[j]=R[j];
        
        if (n==6){
            sides[1] = HumanBodyData[n][3]*0.8;
            dsDrawSphere(posf[n], Rf, (float)sides[1]);
        }
        else if ((n==4)||(n==5)){
            sides[0] = HumanBodyData[n][1]*2;
            sides[1] = HumanBodyData[n][3]*2;
            sides[2] = HumanBodyData[n][5];
            dsDrawBox(posf[n], Rf, sides);
        }
        else{
            sides[0] = HumanBodyData[n][1]*0.8;
            sides[2] = HumanBodyData[n][5]*0.8;
            dsDrawCapsule(posf[n], Rf, sides[2],sides[0]);
        }
        dBodyDestroy(BodyPart.body);
    }
    
    if (SensorShow==1){
        if (HumanInfo[h].TheaterID==-1)
            dsSetColorAlpha(0.8, 0.0, 0.0, 0.5);
        else
            dsSetColorAlpha(0.0, 0.8, 0.0, 0.5);
        pos = dBodyGetPosition(BodyNode[h].body);
        R = dBodyGetRotation(BodyNode[h].body);
        dsDrawCylinder(pos, R, 0.8, 0.2);
        
//        for (i=0; i<HumanSensorNo; i++){
//            if (HumanDist[h][i] < HumanRange)
//                dsSetColorAlpha(1.0, 0.0, 0.0, 0.6);
//            else
//                dsSetColorAlpha(1.0, 0.0, 0.0, 0.05);
//            dGeomRayGet(HumanSensor[h][i].geom, b, d);
//            z[0] = b[0] + (d[0] * HumanDist[h][i]);
//            z[1] = b[1] + (d[1] * HumanDist[h][i]);
//            z[2] = b[2] + (d[2] * HumanDist[h][i]);
//            z[3] = b[3] + (d[3] * HumanDist[h][i]);
//            dsDrawLine(b, z);
//        }
    }
    
    if (HumanInfo[h].Persona==0)
        dsSetColor (0.5, 0.5, 0);
    else
        dsSetColor (1.0, 0.0, 0);
    post[0]=HumanInfo[h].TarPos[0];
    post[1]=HumanInfo[h].TarPos[1];
    post[2]=0.25;
    dsDrawCylinder(post, Rt, 0.5, 0.05);        //  Target

    HumanSensing(h);    //  sensing
    
    HumanInfo[h].Dist=HumanRange;
    for (i=0; i<HumanSensorNo; i++)
        if (HumanDist[h][i]<HumanInfo[h].Dist)
            HumanInfo[h].Dist=HumanDist[h][i];
}

