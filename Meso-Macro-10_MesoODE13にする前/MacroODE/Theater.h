//  ISS Robot Theater Management Program


void ReadTheater()    //  read Theater Information - - - - -
{
    FILE *fp;
    int h,i,j;
    
    char tn[50]="dataU/dataMultiScenario.txt";
    if ((fp=fopen(tn,"r"))==NULL){
        printf("Cannot open %s file\n", tn);
        exit(1);
    }
    
    printf("\n");
    fscanf(fp,"%d\t",&TheaterNo);
    for (i=0; i<TheaterNo; i++){
        if (fscanf(fp,"%[^\n]\n", TheaterInfo[i].TheaterName)==EOF)
            TheaterNo=i;
        else{
            fscanf(fp,"%[^\n]\n", TheaterInfo[i].TheaterFileName);
            fscanf(fp,"%[^\n]\n", TheaterInfo[i].TransitionFileName);
            TheaterInfo[i].Valid=1;
            printf("Theater ID:%d, %s, %s\n",
                   i, TheaterInfo[i].TheaterFileName, TheaterInfo[i].TransitionFileName);
        }
    }
    printf("Total Theater No:%d\n",TheaterNo);
    fclose(fp);
}


void ReadSensorData()    //  read sensor information/property - - - - -
{
    FILE *fp;
    int i,j,k,d;
    char    s[500],
            tn[30]="dataE/dataSensor.txt";
    
    if ((fp=fopen(tn,"r"))==NULL){
        printf("Cannot open %s file\n", tn);
        exit(1);
    }
    
    printf("\n");
    fscanf(fp,"%[^\n]\n", s);

    fscanf(fp,"%d\t",&EnvSensorNo);
    for (i=0; i<EnvSensorNo; i++){
        if (fscanf(fp,"%d\t",&k)==EOF)
            EnvSensorNo=i;
        else{
            DeviceReID[k]=i;
            DeviceReType[k]=0;      //  0: sensor
            EnvSensorReID[k]=i;           //  i: sensor ID in this PC
            if (k==issID)
                issID=i;
            EnvSensorData[i].ID=k;      //  k: sensor ID in a scenario/database
            fscanf(fp,"%d\t",&d);
            EnvSensorData[i].Pos[0]=(double)(d)/100.0+ObPos[0][0];
            fscanf(fp,"%d\t",&d);
            EnvSensorData[i].Pos[1]=(double)(d)/100.0+ObPos[0][1];
            
            fscanf(fp,"%d\t",&EnvSensorData[i].Type);
            fscanf(fp,"%[^\n]\n", EnvSensorData[i].Name);
            EnvSensorData[i].Valid=1;
            EnvSensorData[i].State=0;
            printf("Sensor[%d] ID:%d, %s (%f,%f) Valid:%d, Type:%d\n",
                   i, EnvSensorData[i].ID, EnvSensorData[i].Name,
                   EnvSensorData[i].Pos[0],EnvSensorData[i].Pos[1],
                   EnvSensorData[i].Valid, EnvSensorData[i].Type);
        }
    }
    printf("Total Sensor No:%d\n",EnvSensorNo);
    fclose(fp);
}

void ReadFurData()    //  read Robot information/property - - - - -
{
    FILE *fp;
    int h,i,j,k, d;
    char    s[500],
            tn[30]="dataE/dataFurniture.txt";
    
    if ((fp=fopen(tn,"r"))==NULL){
        printf("Cannot open %s file\n", tn);
        exit(1);
    }
    
    printf("\n");
    fscanf(fp,"%[^\n]\n", s);
    
    fscanf(fp,"%d\t",&EnvFurnitureNo);
    for (i=0; i<EnvFurnitureNo; i++){
        if (fscanf(fp,"%d\t",&k)==EOF)
            EnvFurnitureNo=i;
        else{
            EnvFurData[i].ID=k;
            DeviceReID[k]=i;     //  k: Furnitutr ID in theater
            DeviceReType[k]=1;     //   1: furniture

            fscanf(fp,"%d\t",&d);
            EnvFurData[i].Pos[0]=((double)d)/100.0+ObPos[0][0];
            fscanf(fp,"%d\t",&d);
            EnvFurData[i].Pos[1]=((double)d)/100.0+ObPos[0][1];
            fscanf(fp,"%d\t",&d);
            EnvFurData[i].Pos[2]=(double)d/100.0;
            fscanf(fp,"%d\t",&d);
            EnvFurData[i].Sizes[0]=(double)d/100.0;   //  Box length, Chair length
            fscanf(fp,"%d\t",&d);
            EnvFurData[i].Sizes[1]=(double)d/100.0;   //  Box width,  Chair radius
            fscanf(fp,"%d\t",&d);
            EnvFurData[i].Sizes[2]=(double)d/100.0;   //  Box width,  Chair radius
            fscanf(fp,"%d\t",&d);
            EnvFurData[i].TarPos[0]=((double)d)/100.0+ObPos[0][0];
            fscanf(fp,"%d\t",&d);
            EnvFurData[i].TarPos[1]=((double)d)/100.0+ObPos[0][1];
            
            fscanf(fp,"%d\t",&EnvFurData[i].Type);   //  Shape Type
            fscanf(fp,"%d\t",&EnvFurData[i].Color);   //  Color

            fscanf(fp,"%d\t",&EnvFurData[i].HumanPosture);   //  Human Posture
            
            fscanf(fp,"%[^\n]\n", EnvFurData[i].Name);
            EnvFurData[i].Valid=1;
            printf("Furniture[%d] ID:%d, %s Pos(%f,%f), TarPos(%f,%f), L:%f, W:%f, Type:%d\n",
                   i, EnvFurData[i].ID, EnvFurData[i].Name,
                   EnvFurData[i].Pos[0], EnvFurData[i].Pos[1],
                   EnvFurData[i].TarPos[0], EnvFurData[i].TarPos[1],
                   EnvFurData[i].Sizes[0], EnvFurData[i].Sizes[1], EnvFurData[i].Type);
        }
    }
    printf("Total Furniture No:%d\n",EnvFurnitureNo);
    fclose(fp);
}




//   Robots - - - - - - - - - - - - - - -- - - - - - - - - -

void ReadBuggy()    //  read Robot information/property - - - - -
{
    FILE *fp;
    int d,i,j,k;
    char    s[500],
            tn[30]="dataE/dataBuggy.txt";
    
    if ((fp=fopen(tn,"r"))==NULL){
        printf("Cannot open %s file\n", tn);
        exit(1);
    }
    
    printf("\n");
    fscanf(fp,"%[^\n]\n", s);
    
    fscanf(fp,"%d\t",&BuggyNo);
    for (i=0; i<BuggyNo; i++){
        if (fscanf(fp,"%d\t",&k)==EOF)
            BuggyNo=i;
        else{
            DeviceReID[k]=i;     //  k: Buggy ID in theater
            DeviceReType[k]=3;  //  3: buggy
            BuggyInfo[i].ID=k;
            fscanf(fp,"%d\t",&d);
            BuggyInfo[i].pos[0]=((double)d)/100.0+ObPos[0][0];
            fscanf(fp,"%d\t",&d);
            BuggyInfo[i].pos[1]=((double)d)/100.0+ObPos[0][1];
            
            fscanf(fp,"%d\t",&BuggyInfo[i].Type);
            BuggyInfo[i].Valid=0;
            BuggyInfo[i].State=0;
            
        }
        printf("%d: Buggy[%d]: init pos (%f,%f)\n",
               k, BuggyInfo[i].Type, BuggyInfo[i].pos[0],BuggyInfo[i].pos[1]);
    }
    printf("Total Buggy No:%d\n", BuggyNo);
    fclose(fp);
}

void ReadRobot()    //  read Robot information/property - - - - -
{
    FILE *fp;
    int d,i,j,k;
    char    s[500],
            tn[30]="dataE/dataRobot.txt";
    
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
            DeviceReType[k]=2;  //  2: communication robot
            RobotInfo[i].ID=k;  //  i: Robot ID in this PC
            
            fscanf(fp,"%d\t",&d);
            RobotInfo[i].pos[0]=((double)d)/100.0+ObPos[0][0];
            fscanf(fp,"%d\t",&d);
            RobotInfo[i].pos[1]=((double)d)/100.0+ObPos[0][1];
            fscanf(fp,"%d\t",&d);
            RobotInfo[i].pos[2]=(double)d/100.0;
            fscanf(fp,"%d\t",&d);
            RobotInfo[i].tarpos[0]=((double)d)/100.0+ObPos[0][0];
            fscanf(fp,"%d\t",&d);
            RobotInfo[i].tarpos[1]=((double)d)/100.0+ObPos[0][1];

            fscanf(fp,"%d\t",&RobotInfo[i].Type);
            fscanf(fp,"%d\t",&RobotInfo[i].Language);
            fscanf(fp,"%d\t",&RobotInfo[i].Theater);
            fscanf(fp,"%[^\n]\n", RobotInfo[i].Name);
            RobotInfo[i].Valid=0;
            RobotInfo[i].State=0;
            

            if (RobotInfo[i].Type==1){
                RobotInfo[i].sizes[0]=0.08; //  Radius
                RobotInfo[i].sizes[1]=0.1;  //  Head Length
                RobotInfo[i].sizes[2]=0.2;  //  Body Length
            }
            else if (RobotInfo[i].Type==2){
                RobotInfo[i].sizes[0]=0.08; //  Radius
                RobotInfo[i].sizes[1]=0.1;  //  Head Length
                RobotInfo[i].sizes[2]=0.3;  //  Body Length
            }
            else if (RobotInfo[i].Type==3){
                RobotInfo[i].sizes[0]=0.08; //  Radius
                RobotInfo[i].sizes[1]=0.1;  //  Head Length
                RobotInfo[i].sizes[2]=0.4;  //  Body Length
            }
            printf("Robot ID:%d/%d, %s (%f,%f) Language:%d, Theater:%d\n",
                    RobotInfo[i].ID, i, RobotInfo[i].Name,
                    RobotInfo[i].pos[0], RobotInfo[i].pos[1],
                    RobotInfo[i].Language, RobotInfo[i].Theater);
        }
    }
    printf("Total Robot No:%d\n",RobotNo);
    fclose(fp);
}

#pragma mark -
#pragma mark Utterance Data Management



//  Utterance/Voice Recognition - - - - - - - - - - - - - - -




void ReadVoice()    //  read Utterance Information - - - - -
{
    FILE *fp;
    int h,i,j,k;
    char    tn[30]="dataW/dataVoiceList.txt";

    
    if ((fp=fopen(tn,"r"))==NULL){                 //  read category data and important phrase - - - -
        printf("Cannot open %s file\n", tn);
        exit(1);
    }
    else
        printf("Open [ %s ] file\n",tn);
    fscanf(fp,"%d\t",&VoiceCategoryNo);
    for (i=0; i<VoiceCategoryNo; i++){
        if (fscanf(fp,"%[^\n]\n", VoiceFileName[i])==EOF)
            VoiceCategoryNo=i;
        else
            printf("[%d] %s\n",i, VoiceFileName[i]);
    }
    fclose(fp);
    
    for (i=0; i<VoiceCategoryNo; i++){
        if ((fp=fopen(VoiceFileName[i],"r"))==NULL){
            printf("Cannot open %s file\n", VoiceFileName[i]);
            exit(1);
        }
        printf("\n");
        fscanf(fp,"%d\t",&VoiceWordNo[i]);
        memset(VoiceCategoryName[i], 0, sizeof(VoiceCategoryName[i]));
        fscanf(fp,"%[^\n]\n", VoiceCategoryName[i]);
        printf("[%d] %s:\n", i,  VoiceCategoryName[i]);
        for (j=0; j<VoiceWordNo[i]; j++){
            memset(VoicePhrase[i][j].Sentence, 0, sizeof(VoicePhrase[i][j].Sentence));
            if (fscanf(fp,"%[^\t]\t", VoicePhrase[i][j].Sentence)==EOF)
                VoiceWordNo[i]=j;
            else {
                memset(VoicePhrase[i][j].SentenceJP, 0, sizeof(VoicePhrase[i][j].SentenceJP));
                fscanf(fp,"%[^\n]\n", VoicePhrase[i][j].SentenceJP);
                memset(VoiceInput.Sentence, 0, sizeof(VoiceInput.Sentence));
                strncpy(VoiceInput.Sentence, VoicePhrase[i][j].Sentence,
                        sizeof(VoicePhrase[i][j].Sentence));
//                printf("[%d][%d] %s, %s\n",
//                        i, j, VoicePhrase[i][j].Sentence, VoicePhrase[i][j].SentenceJP);
            }
        }
    }
    for (i=0; i<VoiceCategoryNo; i++){     //  added as a sensor for voice recognition
        k=EnvSensorNo+i;
        EnvSensorReID[100+i]=k;
        EnvSensorData[k].ID=100+i;
        EnvSensorData[k].Pos[0]=EnvSensorData[1].Pos[0];      //  Global Microphone (ID=1)
        EnvSensorData[k].Pos[1]=EnvSensorData[1].Pos[1];
        EnvSensorData[k].Type=EnvSensorData[1].Type;
        strcpy(EnvSensorData[k].Name, EnvSensorData[1].Name);
        strcat(EnvSensorData[k].Name, VoiceCategoryName[i]);
        EnvSensorData[k].Valid=1;
        printf("Sensor[%d] ID:%d, %s (%f,%f) Valid:%d, Type:%d\n",
                k, EnvSensorData[k].ID, EnvSensorData[k].Name,
                EnvSensorData[k].Pos[0], EnvSensorData[k].Pos[1],
                EnvSensorData[k].Valid, EnvSensorData[k].Type);
    }
    printf("Total Category No:%d\n", VoiceCategoryNo);
    fclose(fp);
    
}



//  Utterance -----------------------------------------------



void ReadSentence(int t)    //  read sentences of t-th theater - - - - - - - -
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
        if ((TextShow==1)&&(WordShow==1))
            printf("total Section No[%d]:%d\n",
                   i, SectionNo[i]);
        for (j=0; j<SectionNo[i]; j++){
            fscanf(fp,"%d\n",&ScenarioNo[i][j]);    // Number of Scenarios in SectionID
            if ((TextShow==1)&&(WordShow==1))
                printf("total Scenario No[%d][%d]:%d\n",
                       i,j, ScenarioNo[i][j]);
            for (k=0; k<ScenarioNo[i][j]; k++){
                fscanf(fp,"%d\t",&ScenarioInfo[i][j][k].No);   // Number of Senetences in ScenarioID
                fscanf(fp,"%[^\n]\n",ScenarioInfo[i][j][k].Name);    //  Name (title) of Scenario
                if ((TextShow==1)&&(WordShow==1))
                    printf("total Sentence No[%d][%d][%d]:%d, %s\n",
                           i,j,k, ScenarioInfo[i][j][k].No, ScenarioInfo[i][j][k].Name);
                for (h=0; h<ScenarioInfo[i][j][k].No; h++){
                    fscanf(fp,"%d\t",&ScenarioInfo[i][j][k].RobotID[h]);   // RobotID
                    fscanf(fp,"%[^\t]\t",ScenarioInfo[i][j][k].Sentence[h]);    //  Utterance
                    strcat(ScenarioInfo[i][j][k].Sentence[h],s);
                    fscanf(fp,"%[^\n]\n",ScenarioInfo[i][j][k].SentenceJP[h]);    //  Utterance
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

// state transition rule - - - - - - - - - - - - - - - - - - - -

void ReadRule(int t)    //  read State Transition Rule of t-th theater - - - - - - - -
{
    FILE *fp;
    int h,i,j,k,m,p,q;
    
    if ((fp=fopen(TheaterInfo[t].TransitionFileName,"r"))==NULL){
        printf("Cannot open %s file\n", TheaterInfo[t].TransitionFileName);
        exit(1);
    }
    
    printf("\n");
    fscanf(fp,"%d\t",&InitTransitionNo);       // the number of Init Transition Rules
    fscanf(fp,"%[^\n]\n",RuleName[0][0][0]);        // Name or title of this rule
    if ((TextShow==1)&&(WordShow==1))
        printf("total Init Transition Rules:%d,  %s\n",
               InitTransitionNo, RuleName[0][0][0]);
    for (h=0; h<InitTransitionNo; h++){
        fscanf(fp,"%d\n",&InitTransition[h].No);   // the number of Conditions
        if ((TextShow==1)&&(WordShow==1))
            printf("total Conditions [%d]:%d\n",
                   h, InitTransition[h].No);
        for (m=0; m<InitTransition[h].No; m++){
            fscanf(fp,"%d\t",&InitTransition[h].Sensor[m]);   // [if] the input(sensor) ID
            fscanf(fp,"%d\n",&InitTransition[h].State[m]);    // [if] the condition (state value)
            p=InitTransition[h].Sensor[m];
            q=EnvSensorReID[p];
            if ((TextShow==1)&&(WordShow==1))
                printf("if (%d[%d] = %d) \n",
                       InitTransition[h].Sensor[m], q, InitTransition[h].State[m]);
        }
        fscanf(fp,"%d\t",&InitTransition[h].Phase);          // [then] the phase to transit
        fscanf(fp,"%d\t",&InitTransition[h].Section);        // [then] the section to transit
        fscanf(fp,"%d\t",&InitTransition[h].Scenario);       // [then] the scenario to transit
        fscanf(fp,"%[^\n]\n",InitTransition[h].Rule);        // Text: if-then rule
        if ((TextShow==1)&&(WordShow==1))
            printf("transit to [%d][%d][%d]:  %s\n",
                   InitTransition[h].Phase,    InitTransition[h].Section,
                   InitTransition[h].Scenario, InitTransition[h].Rule);
    }

    if ((TextShow==1)&&(WordShow==1))
        printf("\nTransition Rules\n");
    for (i=0; i<PHASE; i++)
        for (j=0; j<SectionNo[i]; j++)
            for (k=0; k<ScenarioNo[i][j]; k++){
                fscanf(fp,"%d\t",&TransitionNo[i][j][k]);       // the number of Rules
                fscanf(fp,"%[^\n]\n",RuleName[i][j][k]);        // Name or title of this rule
                if ((TextShow==1)&&(WordShow==1))
                    printf("\ntotal Transition Rules [%d][%d][%d]:%d, %s\n",
                           i,j,k, TransitionNo[i][j][k], RuleName[i][j][k]);
                for (h=0; h<abs(TransitionNo[i][j][k]); h++){
                    fscanf(fp,"%d\n",&Transition[i][j][k][h].No);   // the number of Conditions
                    if ((TextShow==1)&&(WordShow==1))
                        printf("total Conditions [%d][%d][%d][%d]:%d\n",
                               i,j,k,h, Transition[i][j][k][h].No);
                    for (m=0; m<Transition[i][j][k][h].No; m++){
                      fscanf(fp,"%d\t",&Transition[i][j][k][h].Sensor[m]);   // [if] the input(sensor) ID
                      fscanf(fp,"%d\t",&Transition[i][j][k][h].State[m]);    // [if] the condition (state value)
                      if ((TextShow==1)&&(WordShow==1))
                          printf("if (%d = %d) \n",
                                 Transition[i][j][k][h].Sensor[m],
                                 Transition[i][j][k][h].State[m]);
                    }
                    fscanf(fp,"%d\t",&Transition[i][j][k][h].Phase);          // [then] the phase to transit
                    fscanf(fp,"%d\t",&Transition[i][j][k][h].Section);        // [then] the scenario to transit
                    fscanf(fp,"%d\t",&Transition[i][j][k][h].Scenario);       // [then] the sentence to transit
                    fscanf(fp,"%[^\n]\n",Transition[i][j][k][h].Rule);        // Text: if-then rule
                    if ((TextShow==1)&&(WordShow==1))
                        printf("transit to [%d][%d][%d]  %s\n",
                           Transition[i][j][k][h].Phase,Transition[i][j][k][h].Section,
                           Transition[i][j][k][h].Scenario,Transition[i][j][k][h].Rule);
                    }
                }
    fclose(fp);
}






#pragma mark -
#pragma mark Theater Management

void init_transition()
{
    int g,h,i,j,k,m,n;
    int SensorState;
    
    SensorWatch();
    
    if ((TextShow==1)&&(WordShow==1))
        printf("total Init Transition Rules :%d\n", InitTransitionNo);
    
    k=-1;
    h=0;
    while (h<InitTransitionNo){
        g=1;
        for (m=0; m<InitTransition[h].No; m++){
            j=InitTransition[h].Sensor[m];  //  Sensor ID in the scenario/database
            i=EnvSensorReID[j];               //  SensorID is the sensor ID in this PC
            SensorState=InitTransition[h].State[m]; //  Sensor State
            if ((TextShow==1)&&(WordShow==1))
                printf("if (%s:%d = %d) Current state:%d ",
                       EnvSensorData[i].Name, i, SensorState, EnvSensorData[i].State);
            if (EnvSensorData[i].State == SensorState)
                g*=1;
            else
                g=0;
        }
        if ((TextShow==1)&&(WordShow==1))
            printf(" then transit to [%d][%d][%d]\n %s\n",
        InitTransition[h].Phase,InitTransition[h].Section,
        InitTransition[h].Scenario,InitTransition[h].Rule);

        if (g==1){
            k=h;
            h=InitTransitionNo;
        }
        else
            h++;
    }
    if (k>=0){
        PhaseID=InitTransition[k].Phase;            // [then] the phase to transit
        SectionID=InitTransition[k].Section;        // [then] the section to transit
        ScenarioID=InitTransition[k].Scenario;      // [then] the scenario to transit
        if ((TextShow==1)&&(WordShow==1))
            printf("Selected Rule[%d]: %s,  Transition to Scenario (%d,%d,%d) \n",
                   k, InitTransition[k].Rule, PhaseID, SectionID, ScenarioID);
        SentenceID=0;
    }
}



void init_Theater()    //  n: Theater ID
{
    int t=TheaterID;
    printf("\n\n - - Selected Theater[%d]: %s - -\n\n",
             t,TheaterInfo[t].TheaterName);

    ReadSentence(t);     //  read Sentence Information
    ReadRule(t);         //  read Transition Rule
    
    PhaseID=-1;     //  Current Phase
    SectionID=-1;   //  Current Section
    ScenarioID=-1;  //  Selected Scenario
    SentenceID=-1;  //  Current Sentence
    
}




void scenario_transition()      //  scenario transtion
{
    int g,h,i,k,m, p,q,r,t;
    int SensorState;
    
    if ((TextShow==1)&&(WordShow==1)){
        printf("Current Scene (%d,%d,%d)\n",PhaseID, SectionID, ScenarioID);
        printf("total Transition Rules [%d][%d][%d]:%d\n",
               PhaseID, SectionID, ScenarioID,
               TransitionNo[PhaseID][SectionID][ScenarioID]);
    }
    
    if  (TransitionNo[PhaseID][SectionID][ScenarioID]==1)       //  Simple Transition
        k=0;
    else if (TransitionNo[PhaseID][SectionID][ScenarioID]<0)    //  Random Transition
        k=(int)(-TransitionNo[PhaseID][SectionID][ScenarioID]*rnd());
    else {                                                      //  Conditional Scenario Transition
        k=-1;
        h=0;
        while (h<TransitionNo[PhaseID][SectionID][ScenarioID]){
            g=1;
            for (m=0; m<Transition[PhaseID][SectionID][ScenarioID][h].No; m++){
                p=Transition[PhaseID][SectionID][ScenarioID][h].Sensor[m];    // p: Sensor ID in a scenario/Database
                SensorID=EnvSensorReID[p];    //  SensorID is the sensor ID in this PC
                SensorState=Transition[PhaseID][SectionID][ScenarioID][h].State[m]; //  Sensor State

                
                
//  -   -   -   -   -   -   -   -  Refer to Sensor Data (MySQL) : Start -   -   -   -   -   -   -
                
                if (p==0)                   //  Check the time
                    SensorWatch();
                else if ((p==1) && (EnvSensorData[SensorID].State==0)
                         && (SimulationMode==11))                        //  Virtual Voice Input
                    VoiceInputTCP(RobotID, p);
                else if ((p==1) && (EnvSensorData[SensorID].State==0)
                         && (SimulationMode>=5))                        //  Virtual Voice Input
                    SensorVoiceInput(p);
                else if ((p>=100) && (p<200) && (SimulationMode>=5)){   //  Virtual Voice Input
                    q=SensorState;
                    SensorVoice(p,q);
                }
//  -   -   -   -   -   -   -   -  Refer to Sensor Data (MySQL) : End -   -   -   -   -   -   -

                
                if ((TextShow==1)&&(WordShow==1))
                    printf("[%d] if (%s:%d = %d) Current state:%d ",
                           m, EnvSensorData[SensorID].Name, p, SensorState, EnvSensorData[SensorID].State);
                if (EnvSensorData[SensorID].State != SensorState)
                    g=0;
            }
            if ((TextShow==1)&&(WordShow==1))
                printf(" then transit to [%d][%d][%d]\n %s\n",
                       Transition[PhaseID][SectionID][ScenarioID][h].Phase,
                       Transition[PhaseID][SectionID][ScenarioID][h].Section,
                       Transition[PhaseID][SectionID][ScenarioID][h].Scenario,
                       Transition[PhaseID][SectionID][ScenarioID][h].Rule);
            if (g==1){
                k=h;
                h=TransitionNo[PhaseID][SectionID][ScenarioID];
            }
            else
                h++;
        }
    }
    EnvSensorData[1].State=0;   //  Voice Input is reset

    //  Decision Making on Scene Transition
    if (k>=0) {
        if ((TextShow==1)&&(WordShow==1))
            printf(" Selected Rule[%d]: PhaseID:%d, %s, ",
                    k, PhaseID, Transition[PhaseID][SectionID][ScenarioID][k].Rule);
        p=Transition[PhaseID][SectionID][ScenarioID][k].Phase;            // [then] the phase to transit
        q=Transition[PhaseID][SectionID][ScenarioID][k].Section;        // [then] the section to transit
        r=Transition[PhaseID][SectionID][ScenarioID][k].Scenario;      // [then] the scenario to transit
        PhaseID=p;
        SectionID=q;
        ScenarioID=r;
        if (PhaseID==-10){
            HumanInfo[HumanID].Interval=HumanInterval;
            HumanInfo[HumanID].State=1;
            if ((TextShow==1)&&(WordShow==1))
                printf("\n\nTheater (Human:%d, Robot:%d) Finished (%d,%d,%d) \n\n",
                       HumanID, RobotID, PhaseID, SectionID, ScenarioID);
            finRT=1;
        }
        else if (PhaseID==-1){
            TheaterID=(int)(TheaterNo*rnd());   //  (tentative) random theater selection
            TheaterID=RobotInfo[i].Theater;
            init_Theater();
        }
        else{
            SentenceID=0;
            if ((TextShow==1)&&(WordShow==1))
                printf("Transition to Scenario (%d,%d,%d) \n",
                       PhaseID, SectionID, ScenarioID);
        }
    }
}

void scenario_transition2()        //  scenario transition to confirm ihe transition
{
    SectionID++;
    ScenarioID=(int)(ScenarioNo[PhaseID][SectionID]*rnd());  //  Random Transition to the next section
    SentenceID=0;
    if ((TextShow==1)&&(WordShow==1))
        printf("\nTransition to the next Scenario (%d,%d,%d) \n",
                PhaseID, SectionID, ScenarioID);
}



void robot_utterance()     //  n: Robot ID
{
    int i,j,k,n;
    n=ScenarioInfo[PhaseID][SectionID][ScenarioID].RobotID[SentenceID];     //  Robot ID
    k=DeviceReID[n];     // ID: n: scenario(global), k: this pc(local)
    if (RobotInfo[k].State==0){
        if ((TextShow==1)&&(WordShow==1))
            printf(" Robot [%d/%d]: Not connected\n", k, n);
    }
    else if (RobotInfo[k].State==1){
            if ((TextShow==1)&&(WordShow==1))
                printf(" Robot [%d/%d]: connected, but not ready to talk\n", k, n);
    }
    else if (RobotInfo[k].State==2){
        RobotInfo[k].State=10;
        if (TheaterLanguage==5)
            printf("  -- Robot-U: [%d][%d][%d][%d]: %s\n",
                   PhaseID,SectionID,ScenarioID,SentenceID,
                   ScenarioInfo[PhaseID][SectionID][ScenarioID].Sentence[SentenceID]);
        else
            printf("  -- Robot-U: [%d][%d][%d][%d]: %s\n",
                    PhaseID,SectionID,ScenarioID,SentenceID,
                    ScenarioInfo[PhaseID][SectionID][ScenarioID].SentenceJP[SentenceID]);
        
        memset(VoicePreviousUtternace.Sentence, 0, sizeof(VoicePreviousUtternace.Sentence));
        strncpy(VoicePreviousUtternace.Sentence,
                ScenarioInfo[PhaseID][SectionID][ScenarioID].Sentence[SentenceID],
                sizeof(ScenarioInfo[PhaseID][SectionID][ScenarioID].Sentence[SentenceID]));
        VoicePreviousUtternace.Times=0;
        if (SimulationMode==11){
            memset(utter, 0, sizeof(utter));
            strncpy(utter,
                    ScenarioInfo[PhaseID][SectionID][ScenarioID].Sentence[SentenceID],
                    sizeof(ScenarioInfo[PhaseID][SectionID][ScenarioID].Sentence[SentenceID]));
            UtteranceTCP(RobotID, 1);   //  RobotID, 1: utterance
        }
    }
    else if (RobotInfo[k].State==10) {   //  talking
        if ((TextShow==1)&&(WordShow==1))
            printf(" Robot [%d/%d]: connected, and talking now\n", k, n);
    }
    else if (RobotInfo[k].State==11){
        RobotInfo[k].State=2;
        SentenceID++;
        if (SentenceID>=ScenarioInfo[PhaseID][SectionID][ScenarioID].No)
            SentenceID=-1;
    }
}

void robot_utterance2()     //  repeated
{
    int i,j,k;
    printf("  -- Robot[repeated, %d times]: %s\n",
           VoicePreviousUtternace.Times, VoicePreviousUtternace.Sentence);
}

