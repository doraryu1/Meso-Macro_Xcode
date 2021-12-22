#pragma mark -
#pragma mark Environment

int viewpoint_change=0,
    Robo_view=0;

void viewpoint_update()
{
    
    float xyz[5],hpr[5];
    
    if (Robo_view == 0) {
        xyz[0] =   0; xyz[1] =   0; xyz[2] = 8.0;
        hpr[0] = -90; hpr[1] = -90; hpr[2] =   0;
    }
    else if (Robo_view == 2) {
        xyz[0] =   0; xyz[1] =   0; xyz[2] = 6.0;
        hpr[0] = -90; hpr[1] = -90; hpr[2] =   0;
    }
    else {
        xyz[0] = 2.5; xyz[1] = 2.5; xyz[2] = 0.5;
        hpr[0] =  225; hpr[1] =   0; hpr[2] = 0;
    };
    dsSetViewpoint(xyz, hpr);
    
    viewpoint_change=0;
}

void viewpoint_update_online()
{
    float xyz[3], hpr[3];
     
    int i;
    
    hpr[0] = 225;
    hpr[1] = -40.0;
    hpr[2] = 0.0;

    xyz[0]=HumanInfo[0].Pos[0]+0.5;
    xyz[1]=HumanInfo[0].Pos[1]+0.5;
    xyz[2]=1.8;
    
    dsSetViewpoint(xyz, hpr);
    
}

void ReadSensor()    //  read sensor information/property - - - - -
{
    FILE *fp;
    int i,j,k,d;
    
    char    s[500],
            tn[30]="data/dataSensor.txt";
    
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
            DeviceReID[k]=i;           //  i: sensor ID in this PC
            DeviceReType[k]=1;    //  1; Sensor
            
            if (k==issID)       //  to be used later .. .
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

void ReadFurniture()    //  read Robot information/property - - - - -
{
    FILE *fp;
    int h,i,j,d,k;
    char    s[500],
            tn[30]="data/dataFurniture.txt";
    
    if ((fp=fopen(tn,"r"))==NULL){
        printf("Cannot open %s file\n", tn);
        exit(1);
    }
    
    printf("\n");
    fscanf(fp,"%[^\n]\n", s);
    
    fscanf(fp,"%d\t",&FurnitureNo);
    for (i=0; i<FurnitureNo; i++){
        if (fscanf(fp,"%d\t",&k)==EOF)
            FurnitureNo=i;
        else{
            DeviceReID[k]=i;     //  k: Furnitutr ID in theater
            DeviceReType[k]=0;    //  0: furniture
            FurnitureInfo[i].ID=k;
            
            fscanf(fp,"%d\t",&d);
            FurnitureInfo[i].Pos[0]=((double)d)/100.0+ObPos[0][0];
            fscanf(fp,"%d\t",&d);
            FurnitureInfo[i].Pos[1]=((double)d)/100.0+ObPos[0][1];
            fscanf(fp,"%d\t",&d);
            FurnitureInfo[i].Pos[2]=(double)d/100.0;
            fscanf(fp,"%d\t",&d);
            FurnitureInfo[i].Sizes[0]=(double)d/100.0;   //  Box length, Chair length
            fscanf(fp,"%d\t",&d);
            FurnitureInfo[i].Sizes[1]=(double)d/100.0;   //  Box width,  Chair radius
            fscanf(fp,"%d\t",&d);
            FurnitureInfo[i].Sizes[2]=(double)d/100.0;   //  Box width,  Chair radius
            fscanf(fp,"%d\t",&d);
            FurnitureInfo[i].TarPos[0]=((double)d)/100.0+ObPos[0][0];
            fscanf(fp,"%d\t",&d);
            FurnitureInfo[i].TarPos[1]=((double)d)/100.0+ObPos[0][1];
            
            fscanf(fp,"%d\t",&FurnitureInfo[i].Type);   //  Shape Type
            fscanf(fp,"%d\t",&FurnitureInfo[i].Color);   //  Color
            
            fscanf(fp,"%d\t",&FurnitureInfo[i].HumanPosture);   //  Human Posture
            
            fscanf(fp,"%[^\n]\n", FurnitureInfo[i].Name);
            FurnitureInfo[i].Valid=1;
            printf("Furniture[%d] ID:%d, %s Pos(%f,%f), TarPos(%f,%f), L:%f, W:%f, Type:%d\n",
                   i, FurnitureInfo[i].ID, FurnitureInfo[i].Name,
                   FurnitureInfo[i].Pos[0], FurnitureInfo[i].Pos[1],
                   FurnitureInfo[i].TarPos[0], FurnitureInfo[i].TarPos[1],
                   FurnitureInfo[i].Sizes[0], FurnitureInfo[i].Sizes[1], FurnitureInfo[i].Type);
        }
    }
    printf("Total Furniture No:%d\n",FurnitureNo);
    fclose(fp);
}

void initColorMap()
{
    int i,j,k,
        m=0;
    
    for (i=0; i<16; i++)
        for (j=0; j<16; j++)
            for (k=0; k<16; k++){
                ColorMap[m][0]=(double)i/16.0;
                ColorMap[m][1]=(double)j/16.0;
                ColorMap[m][2]=(double)k/16.0;
                m++;
            }
    
}


void InitAllTargets()
{
    int i,j,g,
        k=0;
    
    for (i=0; i<DEVICE; i++){
        if (DeviceReType[i]==0){
            g=DeviceReID[i];
            TargetInfo[k].ID=FurnitureInfo[g].ID;       //  Global ID
            TargetInfo[k].TarPos[0]=FurnitureInfo[g].TarPos[0];
            TargetInfo[k].TarPos[1]=FurnitureInfo[g].TarPos[1];
            strcpy(TargetInfo[k].Name,FurnitureInfo[g].Name);
            TargetInfo[g].Type=0;
            TargetInfo[k].Valid=1;
            TargetInfo[k].State=1;
            printf("Tar[%d]: ID:%d (%f,%f) %s\n",
                   k, i, TargetInfo[k].TarPos[0],TargetInfo[k].TarPos[1],
                   TargetInfo[k].Name);
            k++;
        }
        else if (DeviceReType[i]==2){
            g=DeviceReID[i];
            TargetInfo[k].ID=RobotInfo[g].ID;
            TargetInfo[k].TarPos[0]=RobotInfo[g].TarPos[0];
            TargetInfo[k].TarPos[1]=RobotInfo[g].TarPos[1];
            strcpy(TargetInfo[k].Name,RobotInfo[g].Name);
            TargetInfo[k].Type=1;
            TargetInfo[k].Valid=1;
            TargetInfo[k].State=1;
            printf("Tar[%d]: ID:%d (%f,%f) %s\n",
                   k, i, TargetInfo[k].TarPos[0],TargetInfo[k].TarPos[1],
                   TargetInfo[k].Name);
            k++;
        }
    }
    TargetNo=k;

    printf("Targets: %d\n",TargetNo);
}



static void makeEnv()
{
    int i;
    dReal w=0.1;
    dMass m;
    
    for (i=0; i<5; i++)
        EnvObjects[i].body= dBodyCreate (world);
    
    ObPosx[0][0]=(ObPos[0][2]+ObPos[0][0])*0.5;
    ObPosx[0][1]=ObPos[0][1];
    ObPosx[0][2]=(ObPos[0][4]+ObPos[0][5])*0.5;
    ObSize[0][0]=ObPos[0][2]-ObPos[0][0];
    ObSize[0][1]=w;
    ObSize[0][2]=ObPos[0][5];
    dBodySetPosition (EnvObjects[0].body, ObPosx[0][0], ObPosx[0][1], ObPosx[0][2]);
    dMassSetZero (&m);
    dMassSetBox(&m, 0.225, ObSize[0][0], ObSize[0][1], ObSize[0][2]);
    dBodySetMass (EnvObjects[0].body,&m);
//    EnvObjects[0].geom = dCreateBox(space, ObSize[0][0], ObSize[0][1], ObSize[0][2]);
//    dGeomSetBody(EnvObjects[0].geom, EnvObjects[0].body);
    
//
    ObPosx[1][0]=(ObPos[0][2]+ObPos[0][0])*0.5;
    ObPosx[1][1]=ObPos[0][3];
    ObPosx[1][2]=(ObPos[0][4]+ObPos[0][5])*0.5;
    ObSize[1][0]=ObPos[0][2]-ObPos[0][0];
    ObSize[1][1]=w;
    ObSize[1][2]=ObPos[0][5];
    dBodySetPosition (EnvObjects[1].body, ObPosx[1][0], ObPosx[1][1], ObPosx[1][2]);
    dMassSetZero (&m);
    dMassSetBox(&m, 0.225, ObSize[1][0], ObSize[1][1], ObSize[1][2]);
    dBodySetMass (EnvObjects[1].body,&m);
//    EnvObjects[1].geom = dCreateBox(space, ObSize[1][0], ObSize[1][1], ObSize[1][2]);
//    dGeomSetBody(EnvObjects[1].geom, EnvObjects[1].body);

    ObPosx[2][0]=ObPos[0][0];
    ObPosx[2][1]=(ObPos[0][3]+ObPos[0][1])*0.5;
    ObPosx[2][2]=(ObPos[0][4]+ObPos[0][5])*0.5;
    ObSize[2][0]=w;
    ObSize[2][1]=ObPos[0][3]-ObPos[0][1];
    ObSize[2][2]=ObPos[0][5];
    dBodySetPosition (EnvObjects[2].body, ObPosx[2][0], ObPosx[2][1], ObPosx[2][2]);
    dMassSetZero (&m);
    dMassSetBox(&m, 0.225, ObSize[2][0], ObSize[2][1], ObSize[2][2]);
    dBodySetMass (EnvObjects[2].body,&m);

    ObPosx[3][0]=ObPos[0][2];
    ObPosx[3][1]=(ObPos[0][3]+ObPos[0][1])*0.5;
    ObPosx[3][2]=(ObPos[0][4]+ObPos[0][5])*0.5;
    ObSize[3][0]=w;
    ObSize[3][1]=ObPos[0][3]-ObPos[0][1];
    ObSize[3][2]=ObPos[0][5];
    dBodySetPosition (EnvObjects[3].body, ObPosx[3][0], ObPosx[3][1], ObPosx[3][2]);
    dMassSetZero (&m);
    dMassSetBox(&m, 0.225, ObSize[3][0], ObSize[3][1], ObSize[3][2]);
    dBodySetMass (EnvObjects[3].body,&m);
//    EnvObjects[3].geom = dCreateBox(space, ObSize[3][0], ObSize[3][1], ObSize[3][2]);
//    dGeomSetBody(EnvObjects[3].geom, EnvObjects[3].body);
    
    ObPosx[4][0]=(ObPos[0][2]+ObPos[0][0])*0.5;
    ObPosx[4][1]=(ObPos[0][3]+ObPos[0][1])*0.5;
    ObPosx[4][2]=ObPos[0][5];
    ObSize[4][0]=ObPos[0][2]-ObPos[0][0];
    ObSize[4][1]=ObPos[0][3]-ObPos[0][1];
    ObSize[4][2]=w;
    dBodySetPosition (EnvObjects[4].body, ObPosx[4][0], ObPosx[4][1], ObPosx[4][2]);
    dMassSetZero (&m);
    dMassSetBox(&m, 0.225, ObSize[4][0], ObSize[4][1], ObSize[4][2]);
    dBodySetMass (EnvObjects[3].body,&m);
    //    EnvObjects[3].geom = dCreateBox(space, ObSize[3][0], ObSize[3][1], ObSize[3][2]);
    //    dGeomSetBody(EnvObjects[3].geom, EnvObjects[3].body);

    printf("Env Model Started\n");
    
    for (i=0; i<FurnitureNo; i++){
        if  (FurnitureInfo[i].Type==2){  //  furnuture :box
            EnvFurniture[i].body= dBodyCreate (world);
            dBodySetPosition (EnvFurniture[i].body, FurnitureInfo[i].Pos[0],
                                                    FurnitureInfo[i].Pos[1],
                                                    FurnitureInfo[i].Pos[2]);
            dMassSetZero (&m);
            dMassSetBox(&m, 0.225,  FurnitureInfo[i].Sizes[0],
                                    FurnitureInfo[i].Sizes[1],
                                    FurnitureInfo[i].Sizes[2]);
            dBodySetMass (EnvFurniture[i].body,&m);
            EnvFurniture[i].geom = dCreateBox(space,    FurnitureInfo[i].Sizes[0],
                                                        FurnitureInfo[i].Sizes[1],
                                                        FurnitureInfo[i].Sizes[2]);
            dGeomSetBody(EnvFurniture[i].geom, EnvFurniture[i].body);
        }
        else if  (FurnitureInfo[i].Type==3){  //  furnuture :chair
            EnvFurniture[i].body= dBodyCreate (world);
            dBodySetPosition (EnvFurniture[i].body, FurnitureInfo[i].Pos[0],
                                                    FurnitureInfo[i].Pos[1],
                                                    FurnitureInfo[i].Pos[2]);
            dMassSetZero (&m);
            dMassSetCylinder(&m, 0.225,  3, FurnitureInfo[i].Sizes[1],
                                            FurnitureInfo[i].Sizes[2]);
            dBodySetMass (EnvFurniture[i].body,&m);
            EnvFurniture[i].geom = dCreateCylinder(space,   FurnitureInfo[i].Sizes[1],
                                                            FurnitureInfo[i].Sizes[2]);
            dGeomSetBody(EnvFurniture[i].geom, EnvFurniture[i].body);
        }
        else if  (FurnitureInfo[i].Type==4){  //  furnuture :Bed
            EnvFurniture[i].body= dBodyCreate (world);
            dBodySetPosition (EnvFurniture[i].body, FurnitureInfo[i].Pos[0],
                                                    FurnitureInfo[i].Pos[1],
                                                    FurnitureInfo[i].Pos[2]);
            dMassSetZero (&m);
            dMassSetBox(&m, 0.225,  FurnitureInfo[i].Sizes[0],
                                    FurnitureInfo[i].Sizes[1],
                                    FurnitureInfo[i].Sizes[2]);
            dBodySetMass (EnvFurniture[i].body,&m);
            EnvFurniture[i].geom = dCreateBox(space,    FurnitureInfo[i].Sizes[0],
                                                        FurnitureInfo[i].Sizes[1],
                                                        FurnitureInfo[i].Sizes[2]);
            dGeomSetBody(EnvFurniture[i].geom, EnvFurniture[i].body);
        }
        
        EnvFurniture[i].joint = dJointCreateFixed(world, 0);
        dJointAttach(EnvFurniture[i].joint, EnvFurniture[i].body, 0);
        dJointSetFixed(EnvFurniture[i].joint);
    }
    printf("Env Model Finished\n");
}

void drawEnv()     // Teaching data
{
    int c,i,j,k;
    const dReal* pos, * R;
    dReal   Rf[12]={1,0,0,0, 0,1,0,0, 0,0,1,0},
            posf[3], sizef[3];
    
    R = dBodyGetRotation(EnvObjects[0].body);

    for (i=0; i<5; i++){    //  wall
        dsSetColorAlpha(0.95, 0.95, 0.95, 0.98);
        dsDrawBox(ObPosx[i], R, ObSize[i]);
    }
    
    for (i=0; i<FurnitureNo; i++){  //  furnuture
        c=FurnitureInfo[i].Color;
        dsSetColorAlpha(ColorMap[c][0], ColorMap[c][1], ColorMap[c][2], 0.98);
        if (FurnitureInfo[i].Type==1){
            dsDrawSphere(FurnitureInfo[i].Pos, R, FurnitureInfo[i].Sizes[0]);
        }
        else if (FurnitureInfo[i].Type==2){
            pos = dBodyGetPosition(EnvFurniture[i].body);
            R = dBodyGetRotation(EnvFurniture[i].body);
            dsDrawBox(pos, R, FurnitureInfo[i].Sizes);
        }
        else if (FurnitureInfo[i].Type==3){
            pos = dBodyGetPosition(EnvFurniture[i].body);
            R = dBodyGetRotation(EnvFurniture[i].body);
            dsDrawCylinder(pos, R, FurnitureInfo[i].Sizes[2], FurnitureInfo[i].Sizes[1]);
        }
        else if (FurnitureInfo[i].Type==4){
            pos = dBodyGetPosition(EnvFurniture[i].body);
            R = dBodyGetRotation(EnvFurniture[i].body);
            dsDrawBox(pos, R, FurnitureInfo[i].Sizes);
        }
    }
};
