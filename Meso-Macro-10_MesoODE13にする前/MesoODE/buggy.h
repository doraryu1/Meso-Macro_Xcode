// Buggy Size


#define Buggy_LENGTH  0.3	// chassis length
#define Buggy_WIDTH   0.3	// chassis width
#define Buggy_HEIGHT  0.2	// chassis height
#define Buggy_RADIUS  0.13	// wheel radius
#define Buggy_THICK   0.05    // wheel thickness
#define Buggy_CASTER  0.065    // caster radius
#define Buggy_STARTZ  0.3	// starting height of chassis
#define Buggy_CMASS   1		// chassis mass
#define Buggy_WMASS   0.2	// wheel mass

#define Backrest_LENGTH 0.1
#define Backrest_WIDTH Buggy_WIDTH
#define Backrest_HEIGHT 0.2
#define Backrest_MASS   0.6

#define Pole_RADIUS 0.05
#define Pole_HEIGHT 0.3
#define Pole_MASS   0.3
#define Handle_RADIUS 0.05
#define Handle_LENGTH 0.2
#define Handle_MASS   0.3


static dSpaceID car_space;


int rtime;
char tn[20]="data/data000.txt";
FILE *hfp;



void write_Buggy_data_Open()
{
    int k,i,j,
        n=0;
    char mn[20]="0123456789";
    
    while ((hfp=fopen(tn,"r"))!=NULL){
        fclose(hfp);
        n++;
        if (n>=100){
            k=(int)(n/100);
            tn[9]=mn[k];
            n=n%100;
        }
        if (n>=10){
            k=(int)(n/10);
            tn[10]=mn[k];
        }
        k=n%10;
        tn[11]=mn[k];
    }
    if ((hfp=fopen(tn,"w+"))==0){
        printf("can't create DATA file");
        exit(1);
    }
    else{
        fprintf(hfp,"ID\tMode\tSensingRange\tdx\tdy\tDanger\tCollision\tSteering\tVelocity\t");
        for (i=0; i<BuggySensorNo; i++)
            fprintf(hfp,"d[%d]\t",i);
        fprintf(hfp,"\n");
    }
}

void write_Buggy_data_Write()
{
    int b,i,j,k;

    for (b=0; b<BuggyNo; b++){
        if ((BuggyInfo[b].Mode==0) || (BuggyInfo[b].Mode==3)){
            fprintf(hfp,"%d\t%d\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t",
                    b, BuggyInfo[b].Mode%2,
                    BuggyInfo[b].Length,
                    BuggyInfo[b].dPos[0],
                    BuggyInfo[b].dPos[1],
                    BuggyInfo[b].Danger,
                    BuggyInfo[b].TarDir,
//                    BuggyInfo[b].Dir,
                    BuggyInfo[b].Steering,
                    BuggyInfo[b].Velocity
                    );
            for (i=0; i<BuggySensorNo; i++)        //  sensing
                fprintf(hfp,"%f\t",BuggyInfo[b].Dist[i]);
            fprintf(hfp,"\n");
        }
    }
}

void write_Buggy_data_Close()
{
    fclose(hfp);
    printf("DATA write end \n ");
}

void ReadBuggy()    //  read Robot information/property - - - - -
{
    FILE *fp;
    int d,i,j,k;
    char    s[500],
            tn[30]="data/dataBuggy.txt";
    
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
            DeviceReType[k]=3;    //  3; Buggy
            BuggyInfo[i].ID=k;  //  global device ID
            
            fscanf(fp,"%d\t",&d);
            BuggyInfo[i].Pos[0]=((double)d)/100.0+ObPos[0][0];
            fscanf(fp,"%d\t",&d);
            BuggyInfo[i].Pos[1]=((double)d)/100.0+ObPos[0][1];
            
            fscanf(fp,"%d\t",&BuggyInfo[i].Type);
            BuggyInfo[i].Valid=1;
            BuggyInfo[i].State=0;
            
            }
        printf("%d: Buggy[%d]: init pos (%f,%f)\n",
               k, BuggyInfo[i].Type, BuggyInfo[i].Pos[0],BuggyInfo[i].Pos[1]);
    }
    printf("Total Buggy No:%d\n", BuggyNo);
    fclose(fp);
}

void MakeBuggySensor(int b)
{
    int i;
    dReal   mdir,
            angle_x = 0,
            angle_y = 0,
            angle_z = 0;
    dMatrix3 R1, R2, R3, R10, R20, R30;
    
    const dReal *R0  = dBodyGetRotation(Buggy[b][0].body);
    const dReal *pos = dBodyGetPosition(Buggy[b][0].body);
        
    for (i=0; i<BuggySensorNo; i++){
        BuggySensor[b][i].geom = dCreateRay( space, BuggyInfo[b].Length);
        dGeomSetBody(BuggySensor[b][i].geom, Buggy[b][0].body);
        BuggyInfo[b].Dist[i]=BuggyInfo[b].Length;
        angle_x=(double)(i-6)*PI*0.09;
        angle_y=PI*BuggyInfo[b].Angle;  //  Sensing Angle
        angle_z=0;  //  mdir;
        dRFromAxisAndAngle(R1, 1, 0, 0, angle_x); // x
        dRFromAxisAndAngle(R2, 0, 1, 0, angle_y); // y
        dRFromAxisAndAngle(R3, 0, 0, 1, angle_z); // z
        dMultiply0(R10, R1,  R0, 3, 3, 3);
        dMultiply0(R20, R2, R10, 3, 3, 3);
        dMultiply0(R30, R3, R20, 3, 3, 3);
        dGeomSetOffsetRotation( BuggySensor[b][i].geom, R30 );
    }
}


static void makeBuggyBody1(int b)       //  Type1: Buggy (WHILL)
{
    int i,j;
    dMass m;
    Buggy[b][0].body = dBodyCreate (world);
    dBodySetPosition (Buggy[b][0].body,
                      BuggyInfo[b].Pos[0], BuggyInfo[b].Pos[1], Buggy_STARTZ); //BuggyInfo[b].Pos[1]
    dMassSetBox (&m, 1, Buggy_LENGTH, Buggy_WIDTH, Buggy_HEIGHT);
    dMassAdjust (&m,Buggy_CMASS);
    dBodySetMass (Buggy[b][0].body,&m);
    Buggy[b][0].geom = dCreateBox (0, Buggy_LENGTH, Buggy_WIDTH, Buggy_HEIGHT);
    dGeomSetBody (Buggy[b][0].geom, Buggy[b][0].body);
    
    // wheel bodies
    for (i=1; i<=4; i++) {
        if(i==1||i==2){
            Buggy[b][i].body = dBodyCreate (world);
            dMassSetSphere (&m,1,Buggy_CASTER);
            dMassAdjust (&m,Buggy_WMASS);
            dBodySetMass (Buggy[b][i].body,&m);
            Buggy[b][i].geom = dCreateSphere (0, Buggy_CASTER);
        }else{
            Buggy[b][i].body = dBodyCreate (world);
            dQuaternion q;
            dQFromAxisAndAngle (q, 1, 0, 0, M_PI*0.5);
            dBodySetQuaternion (Buggy[b][i].body, q);
            dMassSetCylinder (&m,1,3,Buggy_RADIUS, Buggy_THICK);
            dMassAdjust (&m,Buggy_WMASS);
            dBodySetMass (Buggy[b][i].body,&m);
            Buggy[b][i].geom = dCreateCylinder (0, Buggy_RADIUS, Buggy_THICK);
        }
    }
    dBodySetPosition (Buggy[b][1].body, -0.5*Buggy_LENGTH+BuggyInfo[b].Pos[0],
                      Buggy_WIDTH*0.5+BuggyInfo[b].Pos[1], Buggy_STARTZ-Buggy_HEIGHT*0.5-Buggy_CASTER);
    dBodySetPosition (Buggy[b][2].body, -0.5*Buggy_LENGTH+BuggyInfo[b].Pos[0],
                      -Buggy_WIDTH*0.5+BuggyInfo[b].Pos[1], Buggy_STARTZ-Buggy_HEIGHT*0.5-Buggy_CASTER);
    dBodySetPosition (Buggy[b][3].body, 0.5*Buggy_LENGTH+BuggyInfo[b].Pos[0],
                      Buggy_WIDTH*0.5+BuggyInfo[b].Pos[1]+Buggy_THICK*0.5, Buggy_STARTZ-Buggy_HEIGHT*0.5);
    dBodySetPosition (Buggy[b][4].body, 0.5*Buggy_LENGTH+BuggyInfo[b].Pos[0],
                      -Buggy_WIDTH*0.5+BuggyInfo[b].Pos[1]-Buggy_THICK*0.5, Buggy_STARTZ-Buggy_HEIGHT*0.5);
    for (i=1; i<=4; i++)
        dGeomSetBody (Buggy[b][i].geom, Buggy[b][i].body);
    
    
    // front and back wheel hinges
    for (i=1; i<=4; i++) {
        const dReal *a = dBodyGetPosition (Buggy[b][i].body);
        if(i==1||i==2){
            Buggy[b][i].joint = dJointCreateBall(world, 0);
            dJointAttach(Buggy[b][i].joint, Buggy[b][0].body, Buggy[b][i].body);
            const dReal *a = dBodyGetPosition(Buggy[b][i].body);
            dJointSetBallAnchor(Buggy[b][i].joint, a[0], a[1], a[2]);
        }else{
            Buggy[b][i].joint = dJointCreateHinge2 (world,0);
            dJointAttach (Buggy[b][i].joint, Buggy[b][0].body, Buggy[b][i].body);
            dJointSetHinge2Anchor(Buggy[b][i].joint, a[0], a[1], a[2]);
            dJointSetHinge2Axis1 (Buggy[b][i].joint, 0, 0, 1);
            dJointSetHinge2Axis2 (Buggy[b][i].joint, 0, 1, 0);
        }
    }
    
    // set joint suspension
    for (i=1; i<=4; i++) {
        if(i==1||i==2){
            dJointSetBallParam(Buggy[b][1].joint, dParamSuspensionERP, 0.4);
            dJointSetBallParam(Buggy[b][1].joint, dParamSuspensionCFM, 0.05);
        }else{
            dJointSetHinge2Param (Buggy[b][i].joint, dParamSuspensionERP, 0.4);
            dJointSetHinge2Param (Buggy[b][i].joint, dParamSuspensionCFM, 0.05);   //  Soft: 0.8  Hard:0.01
        }
    }
    
    for (i=3; i<=4; i++) {
        // set stops to make sure wheels always stay in alignment
        dJointSetHinge2Param (Buggy[b][i].joint, dParamLoStop, 0);
        dJointSetHinge2Param (Buggy[b][i].joint, dParamHiStop, 0);
    }
    
    //add backrest
    Buggy[b][5].body = dBodyCreate(world);
    dBodySetPosition(Buggy[b][5].body, BuggyInfo[b].Pos[0] - 0.5 * Buggy_LENGTH + 0.5 * Backrest_LENGTH, BuggyInfo[b].Pos[1], Buggy_STARTZ + 0.5 * Buggy_HEIGHT + 0.5 * Backrest_HEIGHT);
    dMassSetBox (&m, 1.0, Backrest_LENGTH, Backrest_WIDTH, Backrest_HEIGHT);
    dMassAdjust(&m, Backrest_MASS);
    dBodySetMass (Buggy[b][5].body, &m);
    Buggy[b][5].geom = dCreateBox(0, Backrest_LENGTH, Backrest_WIDTH, Backrest_HEIGHT);
    dGeomSetBody (Buggy[b][5].geom, Buggy[b][5].body);
    Buggy[b][5].joint = dJointCreateFixed(world, NULL);
    dJointAttach (Buggy[b][5].joint, Buggy[b][0].body, Buggy[b][5].body);
    dJointSetFixed(Buggy[b][5].joint);
    
    // create car space and add it to the top level space
    car_space = dSimpleSpaceCreate (space);
    dSpaceSetCleanup (car_space, 0);
    dSpaceAdd (car_space, Buggy[b][0].geom);
    dSpaceAdd (car_space, Buggy[b][1].geom);
    dSpaceAdd (car_space, Buggy[b][2].geom);
    dSpaceAdd (car_space, Buggy[b][3].geom);
    dSpaceAdd (car_space, Buggy[b][4].geom);
    
}


static void makeBuggyBody2(int b)       //  Type2: RT1
{
    int i,j;
    dMass m;
    Buggy[b][0].body = dBodyCreate (world);
    dBodySetPosition (Buggy[b][0].body,
                      BuggyInfo[b].Pos[0], BuggyInfo[b].Pos[1], Buggy_STARTZ); //BuggyInfo[b].Pos[1]
    dMassSetBox (&m, 1, Buggy_LENGTH, Buggy_WIDTH, Buggy_HEIGHT);
    dMassAdjust (&m,Buggy_CMASS);
    dBodySetMass (Buggy[b][0].body,&m);
    Buggy[b][0].geom = dCreateBox (0, Buggy_LENGTH, Buggy_WIDTH, Buggy_HEIGHT);
    dGeomSetBody (Buggy[b][0].geom, Buggy[b][0].body);
    
    // wheel bodies
    for (i=1; i<=4; i++) {
        if(i==1||i==2){
            Buggy[b][i].body = dBodyCreate (world);
            dMassSetSphere (&m,1,Buggy_CASTER);
            dMassAdjust (&m,Buggy_WMASS);
            dBodySetMass (Buggy[b][i].body,&m);
            Buggy[b][i].geom = dCreateSphere (0, Buggy_CASTER);
        }else{
            Buggy[b][i].body = dBodyCreate (world);
            dQuaternion q;
            dQFromAxisAndAngle (q, 1, 0, 0, M_PI*0.5);
            dBodySetQuaternion (Buggy[b][i].body, q);
            dMassSetCylinder (&m,1,3,Buggy_RADIUS, Buggy_THICK);
            dMassAdjust (&m,Buggy_WMASS);
            dBodySetMass (Buggy[b][i].body,&m);
            Buggy[b][i].geom = dCreateCylinder (0, Buggy_RADIUS, Buggy_THICK);
        }
    }
    dBodySetPosition (Buggy[b][1].body, -0.5*Buggy_LENGTH+BuggyInfo[b].Pos[0],
                      Buggy_WIDTH*0.5+BuggyInfo[b].Pos[1], Buggy_STARTZ-Buggy_HEIGHT*0.5-Buggy_CASTER);
    dBodySetPosition (Buggy[b][2].body, -0.5*Buggy_LENGTH+BuggyInfo[b].Pos[0],
                      -Buggy_WIDTH*0.5+BuggyInfo[b].Pos[1], Buggy_STARTZ-Buggy_HEIGHT*0.5-Buggy_CASTER);
    dBodySetPosition (Buggy[b][3].body, 0.5*Buggy_LENGTH+BuggyInfo[b].Pos[0],
                      Buggy_WIDTH*0.5+BuggyInfo[b].Pos[1]+Buggy_THICK*0.5, Buggy_STARTZ-Buggy_HEIGHT*0.5);
    dBodySetPosition (Buggy[b][4].body, 0.5*Buggy_LENGTH+BuggyInfo[b].Pos[0],
                      -Buggy_WIDTH*0.5+BuggyInfo[b].Pos[1]-Buggy_THICK*0.5, Buggy_STARTZ-Buggy_HEIGHT*0.5);
    for (i=1; i<=4; i++)
        dGeomSetBody (Buggy[b][i].geom, Buggy[b][i].body);
    
    
    // front and back wheel hinges
    for (i=1; i<=4; i++) {
        const dReal *a = dBodyGetPosition (Buggy[b][i].body);
        if(i==1||i==2){
            Buggy[b][i].joint = dJointCreateBall(world, 0);
            dJointAttach(Buggy[b][i].joint, Buggy[b][0].body, Buggy[b][i].body);
            const dReal *a = dBodyGetPosition(Buggy[b][i].body);
            dJointSetBallAnchor(Buggy[b][i].joint, a[0], a[1], a[2]);
        }else{
            Buggy[b][i].joint = dJointCreateHinge2 (world,0);
            dJointAttach (Buggy[b][i].joint, Buggy[b][0].body, Buggy[b][i].body);
            dJointSetHinge2Anchor(Buggy[b][i].joint, a[0], a[1], a[2]);
            dJointSetHinge2Axis1 (Buggy[b][i].joint, 0, 0, 1);
            dJointSetHinge2Axis2 (Buggy[b][i].joint, 0, 1, 0);
        }
    }
    
    // set joint suspension
    for (i=1; i<=4; i++) {
        if(i==1||i==2){
            dJointSetBallParam(Buggy[b][1].joint, dParamSuspensionERP, 0.4);
            dJointSetBallParam(Buggy[b][1].joint, dParamSuspensionCFM, 0.05);
        }else{
            dJointSetHinge2Param (Buggy[b][i].joint, dParamSuspensionERP, 0.4);
            dJointSetHinge2Param (Buggy[b][i].joint, dParamSuspensionCFM, 0.05);   //  Soft: 0.8  Hard:0.01
        }
    }
    
    for (i=3; i<=4; i++) {
        // set stops to make sure wheels always stay in alignment
        dJointSetHinge2Param (Buggy[b][i].joint, dParamLoStop, 0);
        dJointSetHinge2Param (Buggy[b][i].joint, dParamHiStop, 0);
    }
    
    //add pole
    Buggy[b][5].body = dBodyCreate(world);
    dBodySetPosition(Buggy[b][5].body, BuggyInfo[b].Pos[0] - 0.5 * Buggy_LENGTH + Pole_RADIUS, BuggyInfo[b].Pos[1], Buggy_STARTZ + 0.5 * Buggy_HEIGHT + 0.5 * Pole_HEIGHT);
    dMassSetCylinder(&m, 1.0, 3, Pole_RADIUS, Pole_HEIGHT);
    dMassAdjust(&m, Pole_MASS);
    dBodySetMass (Buggy[b][5].body, &m);
    Buggy[b][5].geom = dCreateCylinder(0, Pole_RADIUS, Pole_HEIGHT);
    dGeomSetBody (Buggy[b][5].geom, Buggy[b][5].body);
    Buggy[b][5].joint = dJointCreateFixed(world, NULL);
    dJointAttach (Buggy[b][5].joint, Buggy[b][0].body, Buggy[b][5].body);
    dJointSetFixed(Buggy[b][5].joint);
    
    //add handle
    Buggy[b][6].body = dBodyCreate(world);
    dBodySetPosition(Buggy[b][6].body, BuggyInfo[b].Pos[0] - 0.5 * Buggy_LENGTH + Handle_RADIUS, BuggyInfo[b].Pos[1], Buggy_STARTZ + 0.5 * Buggy_HEIGHT + Pole_HEIGHT);
    dQuaternion q;
    dQFromAxisAndAngle(q, 1.0, 0.0, 0.0, 0.5 * M_PI);  //x軸周り90°
    dBodySetQuaternion(Buggy[b][6].body, q);
    dMassSetCylinder(&m, 1.0, 3, Handle_RADIUS, Handle_LENGTH);
    dMassAdjust(&m, Handle_MASS);
    dBodySetMass (Buggy[b][6].body, &m);
    Buggy[b][6].geom = dCreateCylinder(0, Handle_RADIUS, Handle_LENGTH);
    dGeomSetBody (Buggy[b][6].geom, Buggy[b][6].body);
    Buggy[b][6].joint = dJointCreateFixed(world, NULL);
    dJointAttach (Buggy[b][6].joint, Buggy[b][0].body, Buggy[b][6].body);
    dJointSetFixed(Buggy[b][6].joint);
    
    // create car space and add it to the top level space
    car_space = dSimpleSpaceCreate (space);
    dSpaceSetCleanup (car_space, 0);
    dSpaceAdd (car_space, Buggy[b][0].geom);
    dSpaceAdd (car_space, Buggy[b][1].geom);
    dSpaceAdd (car_space, Buggy[b][2].geom);
    dSpaceAdd (car_space, Buggy[b][3].geom);
    dSpaceAdd (car_space, Buggy[b][4].geom);
    dSpaceAdd (car_space, Buggy[b][5].geom);
}


//
//static void makeBuggyBody3(int b)       //  Type1: Buggy (HSR)
//{
//    int i,j;
//    dMass m;
//    Buggy[b][0].body = dBodyCreate (world);
//    dBodySetPosition (Buggy[b][0].body,
//                      BuggyInfo[b].Pos[0], BuggyInfo[b].Pos[1], Buggy_STARTZ); //BuggyInfo[b].Pos[1]
//    dMassSetBox (&m, 1, Buggy_LENGTH, Buggy_WIDTH, Buggy_HEIGHT);
//    dMassAdjust (&m,Buggy_CMASS);
//    dBodySetMass (Buggy[b][0].body,&m);
//    Buggy[b][0].geom = dCreateBox (0, Buggy_LENGTH, Buggy_WIDTH, Buggy_HEIGHT);
//    dGeomSetBody (Buggy[b][0].geom, Buggy[b][0].body);
//    
//    // wheel bodies
//    for (i=1; i<=4; i++) {
//        if(i==1||i==2){
//            Buggy[b][i].body = dBodyCreate (world);
//            dMassSetSphere (&m,1,Buggy_CASTER);
//            dMassAdjust (&m,Buggy_WMASS);
//            dBodySetMass (Buggy[b][i].body,&m);
//            Buggy[b][i].geom = dCreateSphere (0, Buggy_CASTER);
//        }else{
//            Buggy[b][i].body = dBodyCreate (world);
//            dQuaternion q;
//            dQFromAxisAndAngle (q, 1, 0, 0, M_PI*0.5);
//            dBodySetQuaternion (Buggy[b][i].body, q);
//            dMassSetCylinder (&m,1,3,Buggy_RADIUS, Buggy_THICK);
//            dMassAdjust (&m,Buggy_WMASS);
//            dBodySetMass (Buggy[b][i].body,&m);
//            Buggy[b][i].geom = dCreateCylinder (0, Buggy_RADIUS, Buggy_THICK);
//        }
//    }
//    dBodySetPosition (Buggy[b][1].body, -0.5*Buggy_LENGTH+BuggyInfo[b].Pos[0],
//                      Buggy_WIDTH*0.5+BuggyInfo[b].Pos[1], Buggy_STARTZ-Buggy_HEIGHT*0.5-Buggy_CASTER);
//    dBodySetPosition (Buggy[b][2].body, -0.5*Buggy_LENGTH+BuggyInfo[b].Pos[0],
//                      -Buggy_WIDTH*0.5+BuggyInfo[b].Pos[1], Buggy_STARTZ-Buggy_HEIGHT*0.5-Buggy_CASTER);
//    dBodySetPosition (Buggy[b][3].body, 0.5*Buggy_LENGTH+BuggyInfo[b].Pos[0],
//                      Buggy_WIDTH*0.5+BuggyInfo[b].Pos[1]+Buggy_THICK*0.5, Buggy_STARTZ-Buggy_HEIGHT*0.5);
//    dBodySetPosition (Buggy[b][4].body, 0.5*Buggy_LENGTH+BuggyInfo[b].Pos[0],
//                      -Buggy_WIDTH*0.5+BuggyInfo[b].Pos[1]-Buggy_THICK*0.5, Buggy_STARTZ-Buggy_HEIGHT*0.5);
//    for (i=1; i<=4; i++)
//        dGeomSetBody (Buggy[b][i].geom, Buggy[b][i].body);
//    
//    
//    // front and back wheel hinges
//    for (i=1; i<=4; i++) {
//        const dReal *a = dBodyGetPosition (Buggy[b][i].body);
//        if(i==1||i==2){
//            Buggy[b][i].joint = dJointCreateBall(world, 0);
//            dJointAttach(Buggy[b][i].joint, Buggy[b][0].body, Buggy[b][i].body);
//            const dReal *a = dBodyGetPosition(Buggy[b][i].body);
//            dJointSetBallAnchor(Buggy[b][i].joint, a[0], a[1], a[2]);
//        }else{
//            Buggy[b][i].joint = dJointCreateHinge2 (world,0);
//            dJointAttach (Buggy[b][i].joint, Buggy[b][0].body, Buggy[b][i].body);
//            dJointSetHinge2Anchor(Buggy[b][i].joint, a[0], a[1], a[2]);
//            dJointSetHinge2Axis1 (Buggy[b][i].joint, 0, 0, 1);
//            dJointSetHinge2Axis2 (Buggy[b][i].joint, 0, 1, 0);
//        }
//    }
//    
//    // set joint suspension
//    for (i=1; i<=4; i++) {
//        if(i==1||i==2){
//            dJointSetBallParam(Buggy[b][1].joint, dParamSuspensionERP, 0.4);
//            dJointSetBallParam(Buggy[b][1].joint, dParamSuspensionCFM, 0.05);
//        }else{
//            dJointSetHinge2Param (Buggy[b][i].joint, dParamSuspensionERP, 0.4);
//            dJointSetHinge2Param (Buggy[b][i].joint, dParamSuspensionCFM, 0.05);   //  Soft: 0.8  Hard:0.01
//        }
//    }
//    
//    for (i=3; i<=4; i++) {
//        // set stops to make sure wheels always stay in alignment
//        dJointSetHinge2Param (Buggy[b][i].joint, dParamLoStop, 0);
//        dJointSetHinge2Param (Buggy[b][i].joint, dParamHiStop, 0);
//    }
//    
//    // add backrest
//    Buggy[b][5].body = dBodyCreate(world);
//    dBodySetPosition(Buggy[b][5].body,
//                     BuggyInfo[b].Pos[0] - 0.5 * Buggy_LENGTH + 0.5 * Backrest_LENGTH,
//                     BuggyInfo[b].Pos[1],
//                     Buggy_STARTZ + 0.5 * Buggy_HEIGHT + 0.5 * Backrest_HEIGHT);
//    dMassSetBox (&m, 1.0, Backrest_LENGTH, Backrest_WIDTH, Backrest_HEIGHT);
//    dMassAdjust(&m, Backrest_MASS);
//    dBodySetMass (Buggy[b][5].body, &m);
//    Buggy[b][5].geom = dCreateBox(0, Backrest_LENGTH, Backrest_WIDTH, Backrest_HEIGHT);
//    dGeomSetBody (Buggy[b][5].geom, Buggy[b][5].body);
//    Buggy[b][5].joint = dJointCreateFixed(world, NULL);
//    dJointAttach (Buggy[b][5].joint, Buggy[b][0].body, Buggy[b][5].body);
//    dJointSetFixed(Buggy[b][5].joint);
//
//    // add robot arm
//    Buggy[b][6].body = dBodyCreate(world);
//    dMassSetZero(&m);
//    dBodySetPosition(Buggy[b][6].body,
//                     BuggyInfo[b].Pos[0] - 0.5 * Buggy_LENGTH + 0.3 * Backrest_LENGTH,
//                     BuggyInfo[b].Pos[1],
//                     Buggy_STARTZ + 0.5 * Buggy_HEIGHT + 0.2 * Backrest_HEIGHT);
//    dMassSetBox (&m, 1.0, Backrest_LENGTH, Backrest_WIDTH, Backrest_HEIGHT);
//    dMassAdjust(&m, Backrest_MASS);
//    dBodySetMass (Buggy[b][6].body, &m);
//
//    Buggy[b][5].geom = dCreateBox(0, 0.3*Backrest_LENGTH,
//                                  Backrest_WIDTH, Backrest_HEIGHT);
//    dGeomSetBody (Buggy[b][5].geom, Buggy[b][5].body);
//    Buggy[b][5].joint = dJointCreateFixed(world, NULL);
//    dJointAttach (Buggy[b][5].joint, Buggy[b][0].body, Buggy[b][5].body);
//    dJointSetFixed(Buggy[b][5].joint);
//
//    
//    
//    HSR_Link1.geom = dCreateBox(space, sizeLink1[0], sizeLink1[1], sizeLink1[2]);
//    dGeomSetBody(HSR_Link1.geom, HSR_Link1.body);
//        
//        HSR_Link2.body = dBodyCreate(world);
//        dMassSetZero(&m);
//        dBodySetPosition(HSR_Link2.body, posLink2[0], posLink2[1], posLink2[2]);
//        dMassSetBox(&m, 0.05, sizeLink2[0], sizeLink2[1], sizeLink2[2]);
//        dBodySetMass(HSR_Link2.body, &m);
//        HSR_Link2.geom = dCreateBox(space, sizeLink2[0], sizeLink2[1], sizeLink2[2]);
//        dGeomSetBody(HSR_Link2.geom, HSR_Link2.body);
//    
//        HSR_Link3.body = dBodyCreate(world);
//        dMassSetZero(&m);
//        dBodySetPosition(HSR_Link3.body, posLink3[0], posLink3[1], posLink3[2]);
//        dMassSetBox(&m, 0.05, sizeLink3[0], sizeLink3[1], sizeLink3[2]);
//        dBodySetMass(HSR_Link3.body, &m);
//        HSR_Link3.geom = dCreateBox(space, sizeLink3[0], sizeLink3[1], sizeLink3[2]);
//        dGeomSetBody(HSR_Link3.geom, HSR_Link3.body);
//    
//    
//    
//    Buggy[b][5].joint = dJointCreateSlider (world, 0);                  //  slider
//    dJointAttach (Link[1].joint, HSR_Link0.body, HSR_Body.body);
//        dJointSetSliderAxis(Link[1].joint, 0, 0, 1);
//        dJointSetSliderParam (Link[1].joint, dParamLoStop, -0.5);
//        dJointSetSliderParam (Link[1].joint, dParamHiStop, 1.5);
//        
//        Link[2].joint = dJointCreateHinge(world, 0);
//        dJointAttach(Link[2].joint, HSR_Link1.body, HSR_Link0.body);
//        dJointSetHingeAxis(Link[2].joint, -1, 0, 0);
//        dJointSetHingeAnchor(Link[2].joint,  posBase[0], posBase[1]+0.03, posBase[2]+0.17);
//        dJointSetFeedback(Link[2].joint, &fbJoint[1]);
//    
//        Link[3].joint = dJointCreateHinge(world, 0);
//        dJointAttach(Link[3].joint, HSR_Link2.body, HSR_Link1.body);
//        dJointSetHingeAxis(Link[3].joint, -1, 0, 0);
//        dJointSetHingeAnchor(Link[3].joint,  posBase[0], posBase[1]+0.03, posBase[2]+0.31);
//        dJointSetFeedback(Link[3].joint, &fbJoint[2]);
//
//    
//    
//    // create car space and add it to the top level space
//    car_space = dSimpleSpaceCreate (space);
//    dSpaceSetCleanup (car_space, 0);
//    dSpaceAdd (car_space, Buggy[b][0].geom);
//    dSpaceAdd (car_space, Buggy[b][1].geom);
//    dSpaceAdd (car_space, Buggy[b][2].geom);
//    dSpaceAdd (car_space, Buggy[b][3].geom);
//    dSpaceAdd (car_space, Buggy[b][4].geom);
//    
//}


void BuggyConTCP(int b){    // - -  send/receive data
    
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
            if (ServerMode==1)
                printf("Buggy [%d] Fail to Robot Theater Server\n",b);
            else
                printf("Buggy [%d] Fail to Macro GNG Server\n",b);
//            printf(" Buggy [%d] No Theater Server - State:%d\n",
//                   b, BuggyInfo[b].State);
            BuggyInfo[b].State=0;
        }
        else{
            write(s, buf, strlen(buf));   //  sending data
            memset(buf, 0, sizeof(buf));
            read(s, buf, sizeof(buf));  //  receiving data
            close(s);
//            printf("Received Data: %s\n", buf);
            
            //  Extraction of Robot Information
            if (BuggyInfo[b].State==0){
                BuggyInfo[b].State=1;           // connected:   1: moving, 2: stopped
                printf("B [%d] Connected now - State:%d\n",
                       b, BuggyInfo[b].State);
            }
            else if (BuggyInfo[b].State==1){
                memset(codeno, 0, sizeof(codeno));
                codeno[0]=buf[0];   //  State Changed ?
                codeno[1]=buf[1];
                codeno[2]=buf[2];
                d = atoi(codeno);
                printf("S1: B [%d] received:%d, %s\n",
                       b, d, buf);
//                if (d==996){
//                    HumanInfo[h].State=2;       // connected:   1: moving, 2: stopped
//                    HumanInfo[h].Mode=1;        // 1: stoped
//                    printf("S1: B [%d] Stopped at the robot - State:%d, Mode:%d\n",
//                           h, BuggyInfo[b].State, BuggyInfo[b].Mode);
//                }
            }
            else if (BuggyInfo[b].State==3){
                memset(codeno, 0, sizeof(codeno));
                codeno[0]=buf[0];   //  Robot Theater ID
                codeno[1]=buf[1];
                codeno[2]=buf[2];
                BuggyInfo[b].TheaterID = atoi(codeno);
                if (BuggyInfo[b].TheaterID==999)
                    BuggyInfo[b].TheaterID=-1;
                else{
                    BuggyInfo[b].State=BuggyInfo[b].Language;   //  Buggy Language
//                    ReadSentence(BuggyInfo[b].TheaterID);
                }
//                printf("Buggy [%d] Connected - State:%d\n",
//                       b, BuggyInfo[b].State);
            }
            else if (BuggyInfo[b].State==BuggyInfo[b].Language){
                memset(codeno, 0, sizeof(codeno));
                codeno[0]=buf[0];   //  Robot Theater ID
                codeno[1]=buf[1];
                BuggyInfo[b].PhaseID = atoi(codeno);
                if (BuggyInfo[b].PhaseID==99){
                    BuggyInfo[b].PhaseID=-1;
                    BuggyInfo[b].State=2;
                }
                else if (BuggyInfo[b].PhaseID<98){
                    memset(codeno, 0, sizeof(codeno));
                    codeno[0]=buf[2];   //  Robot Utterance ID
                    codeno[1]=buf[3];
                    BuggyInfo[b].SectionID = atoi(codeno);
                    memset(codeno, 0, sizeof(codeno));
                    codeno[0]=buf[4];   //  Robot Utterance ID
                    codeno[1]=buf[5];
                    BuggyInfo[b].ScenarioID = atoi(codeno);
                    memset(codeno, 0, sizeof(codeno));
                    codeno[0]=buf[6];   //  Robot Utterance ID
                    codeno[1]=buf[7];
                    BuggyInfo[b].SentenceID = atoi(codeno);
                    BuggyInfo[b].State=10;
                    printf("  -- Buggy: Theater [%d][%d][%d][%d] in [%d]\n",
                               BuggyInfo[b].PhaseID, BuggyInfo[b].SectionID,
                               BuggyInfo[b].ScenarioID, BuggyInfo[b].SentenceID,
                               BuggyInfo[b].Language);
                }
            }
        }
    }
}


void sendBuggydata(int b){    // b:robot ID to Robot Contents Server

    struct sockaddr_in server;
    unsigned long dst_ip;
    int        port;
    int        i,k,m,v;

    char    numberName[15]="01234567890";
       
    memset(buf, 0, sizeof(buf));
    
    i=BuggyInfo[b].ID;
    
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

    m=BuggyInfo[b].State;        //  Send State
    if (m>=10) {
        k=(int)(m/10)%10;
        buf[3]=numberName[k]; // State ID
    }
    else
        buf[3]=numberName[0]; // State ID
    k=m%10;
    buf[4]=numberName[k]; // State ID
    
    for (i=0; i<3; i++){
        v=(int)((BuggyInfo[b].Pos[i]+50.0)*100.0);
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
    
//    printf("Buggy[%d] Sent Data: %s\n",
//           b, buf);
    
    BuggyConTCP(b);             //  sending and receiving trough buf[]
}



#pragma mark -
#pragma mark Fuzzy Control for Buggy

#define BRULES   15       // Buggy: Fuzzy Rules
#define BMEMFS   15       // Buggy: Membership Functions
#define BCONTL    5       // Buggy: Membership Functions

int bmemf=13,        // Buggy: Number of Membership Functions  ( menf = HumanSensorNo)
    bruls=12;        // Buggy: Number of Rules
    
double  bFuzzyIf[BRULES+1][BMEMFS+1]={    // Buggy:   Fuzzy Rules if parts (premise)
            //0,1,2,3,4,5,6,7,8,9,a,b,c
            { 1,1,1,1,1,1,1,1,1,1,1,0,0 },    //  0   Collision Avoidance using
            { 1,1,1,1,1,1,1,1,1,1,0,0,1 },    //  1  Ultrasonic Sensor
            { 1,1,1,1,1,1,1,1,1,0,0,1,1 },    //  2
            { 1,1,1,1,1,1,1,1,0,0,1,1,1 },    //  3
            { 1,1,1,1,1,1,1,0,0,1,1,1,1 },    //  4
            { 1,1,1,1,1,1,0,0,1,1,1,1,1 },     //  5
            { 1,1,1,1,1,0,0,1,1,1,1,1,1 },    //  6
            { 1,1,1,1,0,0,1,1,1,1,1,1,1 },     //  7
            { 1,1,1,0,0,1,1,1,1,1,1,1,1 },     //  8
            { 1,1,0,0,1,1,1,1,1,1,1,1,1 },     //  9
            { 1,0,0,1,1,1,1,1,1,1,1,1,1 },     //  10
            { 0,0,1,1,1,1,1,1,1,1,1,1,1 },     //  11
            },
        bFuzzyThen[BRULES+1]={    // Fuzzy Rules then parts (conclusions)
            0.12, 0.14, 0.17, 0.22, 0.28, 0.35, -0.35, -0.28, -0.22, -0.17, -0.14, -0.12
        };            // Outputs


void BuggyFuzzyInf(int b)       // Fuzzy inference, b:Buggy ID
{
    int     i,j;
    double  x[BMEMFS+1],       //  Inputs
            l=BuggyInfo[b].Length,
            Firing[BRULES+1],
            FuzzyOut,
            TotalFiring=0;  // total output, total firing (weight)

    for (j=0; j<bmemf; j++)
        x[j]=BuggyInfo[b].Dist[j];   //  input to fuzzy if-then rules
    
    for (i=0;i<bruls;i++){
        Firing[i]=1.0;
        for (j=0; j<bmemf; j++){
            if (bFuzzyIf[i][j]==0)
                Firing[i]*=exp(-x[j]*x[j]);
            else
                Firing[i]*=exp(-(x[j]-l)*(x[j]-l));
        }
        TotalFiring+=Firing[i];
    }
    FuzzyOut=0;
    for (i=0;i<bruls;i++)
        FuzzyOut+=Firing[i]*bFuzzyThen[i];
    FuzzyOut/=TotalFiring;
    BuggyInfo[b].TarDir=FuzzyOut;     //  Collision avoidance
}


void BuggySensing(int b)     //  control, b: Buggy ID
{
    int c, i,j,k;
    dContactGeom SenCol;
    
    BuggyInfo[b].Danger=BuggyInfo[b].Length;
    for (i=0; i<BuggySensorNo; i++){        //  sensing
        BuggyInfo[b].Dist[i]=BuggyInfo[b].Length;
        for (j=0; j<PeopleNo; j++){
            c = dCollide(BuggySensor[b][i].geom, BodyNode[j].geom, 1, &SenCol, sizeof( dContactGeom ) );
            if (c > 0){
                if (BuggyInfo[b].Dist[i] > SenCol.depth){
                    BuggyInfo[b].Dist[i] = SenCol.depth;
                }
            }
        }
        if (RobotHSR==1){
            c = dCollide(BuggySensor[b][i].geom, HSR_Base.geom, 1, &SenCol, sizeof( dContactGeom ) );
            if (c > 0){
                if (BuggyInfo[b].Dist[i] > SenCol.depth){
                    BuggyInfo[b].Dist[i] = SenCol.depth;
                }
            }
        }
        for (j=0; j<BuggyNo; j++){
            if (j!=b){
                c = dCollide(BuggySensor[b][i].geom, Buggy[j][0].geom, 1, &SenCol, sizeof( dContactGeom ) );   // with Buggy
                if (c > 0){
                    if (BuggyInfo[b].Dist[i] > SenCol.depth){
                        BuggyInfo[b].Dist[i] = SenCol.depth;
                    }
                }
            }
        }
        for (j=0; j<FurnitureNo; j++){  //  furnuture 2:box, 3:chair  FurnitureNo
            if (FurnitureInfo[j].Type>=2){
                c = dCollide(BuggySensor[b][i].geom, EnvFurniture[j].geom, 1, &SenCol, sizeof( dContactGeom ) );
                if (c > 0){
                    if (BuggyInfo[b].Dist[i] > SenCol.depth){
                        BuggyInfo[b].Dist[i] = SenCol.depth;
                    }
                }
            }
        }
        for (j=0; j<RobotNo; j++){  //  Communication Robots
            c = dCollide(BuggySensor[b][i].geom, RobotC[j].geom, 1, &SenCol, sizeof( dContactGeom ) );
            if (c > 0){
                if (BuggyInfo[b].Dist[i] > SenCol.depth){
                    BuggyInfo[b].Dist[i] = SenCol.depth;
                }
            }
//            printf("D[%d][%d]:%f ",i,j,SenCol.depth);
        }
//        printf("\n");
        if (BuggyInfo[b].Dist[i]<BuggyInfo[b].Danger)
            BuggyInfo[b].Danger=BuggyInfo[b].Dist[i];
    }
    BuggyInfo[b].Danger/=BuggyInfo[b].Length;    //  rate of min-distance to BuggyRange
}

void BuggyTargetSelection(int b)  //    target Selection
{
    int k;
    k=(int)(TargetNo*rnd());
    BuggyInfo[b].TarPos[0]=TargetInfo[k].TarPos[0];
    BuggyInfo[b].TarPos[1]=TargetInfo[k].TarPos[1];
    printf("Buggy [%d] Tareget [%d] (%f,%f)\n",
               b,k,BuggyInfo[b].TarPos[0],BuggyInfo[b].TarPos[1]);
}


void BuggyTargetSelection2(int b)  //    target Selection with Persona Model
{
    int i,j,k,g;
    i=BuggyInfo[b].Persona;
    j=BuggyInfo[b].Task;
    g=PersonaInfo[i].Model[j];  //  Global Target ID
    k=DeviceReID[g];
    switch (DeviceReType[g]){
        case 0:     //  Furniture
            BuggyInfo[b].TarPos[0]=FurnitureInfo[k].TarPos[0];
            BuggyInfo[b].TarPos[1]=FurnitureInfo[k].TarPos[1];
            break;
        case 2:     //  Communication robot
            BuggyInfo[b].TarPos[0]=RobotInfo[k].TarPos[0];
            BuggyInfo[b].TarPos[1]=RobotInfo[k].TarPos[1];
            break;
    }
    BuggyInfo[b].TarID=g;
    printf("Buggy[%d] Persona:%d, TaskID:%d/%d, TargetID:%d/%d, TarPos(%f,%f)\n",
           b, i, j, PersonaInfo[i].TaskNo, k, BuggyInfo[b].TarID,
           BuggyInfo[b].TarPos[0], BuggyInfo[b].TarPos[1]);
}

void BuggyTargetFin(int b)  //    target Selection after a buggy reaches its i-th target
{
    int i;
    
    if (BuggyInfo[b].Persona==0){
        BuggyTargetSelection(b);
    }
    else{
        i=BuggyInfo[b].Persona;
        BuggyInfo[b].Task++;
        if (BuggyInfo[b].Task>=PersonaInfo[i].TaskNo)
            BuggyInfo[b].Task=0;
        BuggyTargetSelection2(b);
    }
}

void control_Buggy(int b)     //  control, b: Buggy ID  - - - - - - -
{
    int c, i,j,k;
    dContactGeom SenCol;
    dReal s, v;
    float   dang,    //  target angle
            tdir,   //  target moving direction
            mdir,    //  current moving direction
            dx,dy;  //  distance

    double  TarTraceKP=0.1;     //  KP for target tracing
    
    
    const dReal *pos= dBodyGetPosition(Buggy[b][0].body);     // current position
    const dReal *rot= dBodyGetRotation(Buggy[b][0].body);     // current posture, unit vector
//    const dReal ang= dJointGetHinge2Angle1(Buggy[b][1].joint);

    for (j=0; j<3; j++)
         BuggyInfo[b].Pos[j] = pos[j];
    
    if (BuggyInfo[b].Mode==1){
        for (i=0; i<BuggySensorNo; i++)
            dGeomDestroy(BuggySensor[b][i].geom);
        if (BuggyInfo[b].Angle>-0.51)
            BuggyInfo[b].Angle-=0.01;
        else
            BuggyInfo[b].Mode=3;
        MakeBuggySensor(b);
    }
    else if (BuggyInfo[b].Mode==2){
        for (i=0; i<BuggySensorNo; i++)
            dGeomDestroy(BuggySensor[b][i].geom);
        if (BuggyInfo[b].Angle<0.51)
            BuggyInfo[b].Angle+=0.01;
        else
            BuggyInfo[b].Mode=0;
        MakeBuggySensor(b);
    }
    else{
        BuggyInfo[b].Counter++;
        BuggyInfo[b].T++;
        if ((rot[4]==0)&&(rot[0]==0))
            mdir=0;
        else if (BuggyInfo[b].Mode==0)
            mdir=atan2(rot[4],rot[0]);      //  current Moving Direction
        else
            mdir=atan2(rot[4],rot[0])+PI;      //  current Moving Direction
        if (mdir>PI)
            mdir-=PI*2;
        else if (mdir<-PI)
            mdir+=PI*2;
    
        dx=BuggyInfo[b].TarPos[0]-pos[0];
        dy=BuggyInfo[b].TarPos[1]-pos[1];
        BuggyInfo[b].dPos[0]=dx;
        BuggyInfo[b].dPos[1]=dy;
        if (dx*dx+dy*dy<0.4){   //  reach the target, and select its next target point
            BuggyTargetFin(b);
        }
        if (BuggyInfo[b].Mode==0)
            tdir=atan2(dy,dx);   //  Target Tracing:  Target Direction
        else
            tdir=atan2(dy,dx)+PI;    //  Target Tracing:  Target Direction
        if (tdir>PI)
            tdir-=PI*2;
        else if (tdir<-PI)
            tdir+=PI*2;
    
        dang=tdir-mdir;        //  Target Tracing:   steering direction
        if (dang>PI)
            dang-=PI*2;
        else if (dang<-PI)
            dang+=PI*2;
        dang*=TarTraceKP;        //  Target Tracing:   steering direction
        
        //  Dangerous Situation
        if ((BuggyInfo[b].Dist[2]<BuggyInfo[b].Length*0.35)||
            (BuggyInfo[b].Dist[3]<BuggyInfo[b].Length*0.45)||
            (BuggyInfo[b].Dist[4]<BuggyInfo[b].Length*0.5)||
            (BuggyInfo[b].Dist[5]<BuggyInfo[b].Length*0.55)||
            (BuggyInfo[b].Dist[6]<BuggyInfo[b].Length*0.65)||
            (BuggyInfo[b].Dist[7]<BuggyInfo[b].Length*0.55)||
            (BuggyInfo[b].Dist[8]<BuggyInfo[b].Length*0.5)||
            (BuggyInfo[b].Dist[9]<BuggyInfo[b].Length*0.45)||
            (BuggyInfo[b].Dist[10]<BuggyInfo[b].Length*0.35)){
            if (BuggyInfo[b].Mode==0){
                BuggyInfo[b].Mode=1;
                BuggyInfo[b].Counter=0; //  Backward Counter
            }
            else if (BuggyInfo[b].Mode==3)
                BuggyInfo[b].Mode=2;
        }
        else if ((BuggyInfo[b].Mode==3)&&(BuggyInfo[b].Danger>0.8)&&(BuggyInfo[b].Counter>400))
            BuggyInfo[b].Mode=2;
        else{
            if (BuggyInfo[b].Danger<0.8){                   //  Sensory Network
                if (BuggyInfo[b].Length>BuggyRangeMin){
                    BuggyInfo[b].Length-=0.002;
                    for (i=0; i<BuggySensorNo; i++)
                        dGeomDestroy(BuggySensor[b][i].geom);
                    MakeBuggySensor(b);
                }
            }
            else if (BuggyInfo[b].Length<BuggyRangeMax){    //  Sensory Network
                BuggyInfo[b].Length+=0.002;
                for (i=0; i<BuggySensorNo; i++)
                    dGeomDestroy(BuggySensor[b][i].geom);
                MakeBuggySensor(b);
            }
            BuggySensing(b);
            BuggyFuzzyInf(b);                               //  Fuuzy Control
            
            //  MOBC
            s = BuggyInfo[b].TarDir + dang*BuggyInfo[b].Danger*BuggyInfo[b].Danger;
            s *= 20.0;
        }
        //printf ("Taget:%f, Moving:%f, dang:%f, Danger:%f, Col:%f, steer:%f\n",tdir/M_PI*180.0, mdir/M_PI*180.0, dang/M_PI*180.0*10.0, BuggyInfo[b].Danger, BuggyInfo[b].TarDir,s);
    
        v = 1.5*(BuggyInfo[b].Danger+0.005); //  velocity
        if (BuggyInfo[b].Mode==0)
            v=-v;
    }
    BuggyInfo[b].Velocity=v;
    BuggyInfo[b].Steering=s;
    
    BuggyInfo[b].speedLR[0] = BuggyInfo[b].Velocity + 0.5 * BuggyInfo[b].Steering;
    BuggyInfo[b].speedLR[1] = BuggyInfo[b].Velocity - 0.5 * BuggyInfo[b].Steering;
    
    // motor
    dJointSetHinge2Param (Buggy[b][3].joint, dParamVel2, BuggyInfo[b].speedLR[0]);
    dJointSetHinge2Param (Buggy[b][3].joint, dParamFMax2, 20.0);
    dJointSetHinge2Param (Buggy[b][4].joint, dParamVel2, BuggyInfo[b].speedLR[1]);
    dJointSetHinge2Param (Buggy[b][4].joint, dParamFMax2, 20.0);
    // steering
    /*dJointSetHinge2Param (Buggy[b][1].joint,dParamVel, s);
    dJointSetHinge2Param (Buggy[b][1].joint,dParamFMax, 10.0);
    dJointSetHinge2Param (Buggy[b][1].joint,dParamLoStop,-0.3);
    dJointSetHinge2Param (Buggy[b][1].joint,dParamHiStop,0.3);
    dJointSetHinge2Param (Buggy[b][1].joint,dParamFudgeFactor,0.1);*/
}


static void drawBuggy1(int b)    //  b; Buggy ID (WHILL)
{
    int i,j;
    const dReal* pos, * R;
    dReal posf[3], Rf[12];
    dReal sidesf[3]={Buggy_LENGTH, Buggy_WIDTH, Buggy_HEIGHT};
    
    dsSetColor (0,0.4+b*0.2,0.4+b*0.2);
    pos =dBodyGetPosition(Buggy[b][0].body);
    R = dBodyGetRotation(Buggy[b][0].body);
    for (j=0; j<3; j++)
        posf[j]=pos[j];
    for (j=0; j<12; j++)
        Rf[j]=R[j];
    dsDrawBox(posf, Rf, sidesf);
    
    posf[0]=BuggyInfo[b].TarPos[0];
    posf[1]=BuggyInfo[b].TarPos[1];
    posf[2]=0.25;
    dsDrawCylinder(posf, Rf, 0.5, 0.05);        //  Target

    dsSetColor (1, 1, 1);
    for (i=1; i<=4; i++){
        const dReal* pos =dBodyGetPosition(Buggy[b][i].body);
        const dReal* R = dBodyGetRotation(Buggy[b][i].body);
        for (j=0; j<3; j++)
            posf[j]=pos[j];
        for (j=0; j<12; j++)
            Rf[j]=R[j];
        if(i == 1||i == 2){
            dsDrawSphere(posf, Rf, Buggy_CASTER);
        }else{
            dsDrawCylinder(posf, Rf, Buggy_THICK, Buggy_RADIUS);
        }
    }
    

    dVector3 z, r, d;   // z: position, r: base, d:  direction
    if (SensorShow==1){
        for (i=0; i<BuggySensorNo; i++){        //  sensing
            if (BuggyInfo[b].Dist[i] < BuggyInfo[b].Length)
                dsSetColorAlpha(1.0, 0.0, 0.0, 0.9);
            else
                dsSetColorAlpha(1.0, 1.0, 0.0, 0.3);
            dGeomRayGet(BuggySensor[b][i].geom, r, d);
            z[0] = r[0] + (d[0] * BuggyInfo[b].Dist[i]);
            z[1] = r[1] + (d[1] * BuggyInfo[b].Dist[i]);
            z[2] = r[2] + (d[2] * BuggyInfo[b].Dist[i]);
            z[3] = r[3] + (d[3] * BuggyInfo[b].Dist[i]);
            dsDrawLine(r, z);
        }
    }
    
    //add backrest
    dReal size[3] = {Backrest_LENGTH, Backrest_WIDTH, Backrest_HEIGHT};
    dsSetColor (0,0.4+b*0.2,0.4+b*0.2);
    dsDrawBox(dGeomGetPosition(Buggy[b][5].geom), dGeomGetRotation(Buggy[b][5].geom), size);
    
}


static void drawBuggy2(int b)    //  b; Buggy ID (RT1)
{
    int i,j;
    const dReal* pos, * R;
    dReal posf[3], Rf[12];
    dReal sidesf[3]={Buggy_LENGTH, Buggy_WIDTH, Buggy_HEIGHT};
    
    dsSetColor (0, 1.0,0.2+b*0.2);
//    dsSetTexture (DS_WOOD);
    pos =dBodyGetPosition(Buggy[b][0].body);
    R = dBodyGetRotation(Buggy[b][0].body);
    for (j=0; j<3; j++)
        posf[j]=pos[j];
    for (j=0; j<12; j++)
        Rf[j]=R[j];
    dsDrawBox(posf, Rf, sidesf);
    
    posf[0]=BuggyInfo[b].TarPos[0];
    posf[1]=BuggyInfo[b].TarPos[1];
    posf[2]=0.25;
    dsDrawCylinder(posf, Rf, 0.5, 0.02);        //  Target

    dsSetColor (1, 1, 1);
    for (i=1; i<=4; i++){
        const dReal* pos =dBodyGetPosition(Buggy[b][i].body);
        const dReal* R = dBodyGetRotation(Buggy[b][i].body);
        for (j=0; j<3; j++)
            posf[j]=pos[j];
        for (j=0; j<12; j++)
            Rf[j]=R[j];
        if(i == 1||i == 2){
            dsDrawSphere(posf, Rf, Buggy_CASTER);
        }else{
            dsDrawCylinder(posf, Rf, Buggy_THICK, Buggy_RADIUS);
        }
    }
    
    dVector3 z, r, d;   // z: position, r: base, d:  direction
    if (SensorShow==1){
        for (i=0; i<BuggySensorNo; i++){        //  sensing
            if (BuggyInfo[b].Dist[i] < BuggyInfo[b].Length)
                dsSetColorAlpha(1.0, 0.0, 0.0, 0.9);
            else
                dsSetColorAlpha(1.0, 1.0, 0.0, 0.3);
            dGeomRayGet(BuggySensor[b][i].geom, r, d);
            z[0] = r[0] + (d[0] * BuggyInfo[b].Dist[i]);
            z[1] = r[1] + (d[1] * BuggyInfo[b].Dist[i]);
            z[2] = r[2] + (d[2] * BuggyInfo[b].Dist[i]);
            z[3] = r[3] + (d[3] * BuggyInfo[b].Dist[i]);
            dsDrawLine(r, z);
        }
    }
    
    //add pole
    dsSetColor (0, 1.0,0.2+b*0.2);
    dsDrawCylinder(dGeomGetPosition(Buggy[b][5].geom), dGeomGetRotation(Buggy[b][5].geom), Pole_HEIGHT, Pole_RADIUS);
    
    //add handle
    dsSetColor (0, 1.0,0.2+b*0.2);
    dsDrawCylinder(dGeomGetPosition(Buggy[b][6].geom), dGeomGetRotation(Buggy[b][6].geom), Handle_LENGTH, Handle_RADIUS);
    
}


double  HumanBodyData2[HumanPartsNo+5][10]={        // positions of people parts,  front-rear, left-right, length(hieght)
    //  x1,    x2,    y1,     y2,    z1, z2
        {-0.045, 0.045, -0.1,   -0.02,   0, 0.2},   //  0: right lower leg
        {-0.055, 0.055, -0.105, -0.0105, 0, 0.2},   //  1: right upper leg
        {-0.055, 0.055,  0.0105, 0.105,  0, 0.2},   //  2: left upper leg
        {-0.045, 0.045,  0.1,    0.02,   0, 0.2},   //  3: left lower leg
        {-0.06,  0.06,  -0.1,    0.1,    0, 0.2},   //  4: main lower body
        {-0.07,  0.07,  -0.11,   0.11,   0, 0.12},  //  5: main upper body
        {-0.06,  0.06,  -0.06,   0.06,   0, 0.12},  //  6: head
        {-0.04,  0.04,  -0.18,  -0.11,   0, 0.1},   //  7: right upper arm
        {-0.04,  0.04,  -0.18,  -0.11,   0, 0.15},  //  8: right lower arm
        {-0.04,  0.04,   0.11,   0.18,   0, 0.1},   //  9: left upper arm
        {-0.04,  0.04,   0.11,   0.18,   0, 0.15}   //  10 left lower arm
    },
    HumanPosute[3][HumanJointsNo]={
        {   0,    0,     0,   0, 0, 0, 0, -0.1,  -0.45, -0.1,  -0.45}, //  Walking
        { 0.5,-0.75, -0.75, 0.5, 0, 0, 0, 0, -0.35, 0, -0.35}  //  Sitting
    },
    HumanLoco[3][HumanJointsNo+1]={        // angular velocity for posture of people parts
        { 0, -0.2, 0.2, 0, 0, 0, 0, 0, 0, 0, 0},    // 0   walking
        { 0,     0,    0, 0, 0, 0, 0, 0, 0, 0, 0}    // 1   sitting
    };


MyObject BodyPartB;

static void makeBodyPartsB(double d, double mdir)
{
    int i;
    dReal radius=0.02, length=0.1;
    dMass m;
    dMassSetZero (&m);
    
    BodyPartB.body= dBodyCreate (world);
    dMassSetCylinder(&m,0.225,3,radius,length);
    dBodySetMass (BodyPartB.body,&m);
    
    dReal angle_x = 0, angle_y = 0, angle_z = 0;
    const dReal *R0;
    dMatrix3 R1, R2, R3, R10, R20, R30;

    R0 = dBodyGetRotation(BodyPartB.body);

    angle_y=d;
    angle_z=mdir;
    dRFromAxisAndAngle(R1, 1, 0, 0, angle_x); // x
    dRFromAxisAndAngle(R2, 0, 1, 0, angle_y); // y
    dRFromAxisAndAngle(R3, 0, 0, 1, angle_z); // z

    dMultiply0(R10, R2,  R0, 3, 3, 3);
    dMultiply0(R20, R1, R10, 3, 3, 3);
    dMultiply0(R30, R3, R20, 3, 3, 3);

    dBodySetRotation(BodyPartB.body, R30);
    R0 = dBodyGetRotation(BodyPartB.body);
    dBodySetPosition (BodyPartB.body, 0,3,1);
}


void drawHuman(int b, int m)       //  b: buggy ID, m: human mode
{
    int c,i,j,n,p,
        sensed;         //  0: no sense, 1: sensed
    dReal t, mdir, Calpha=0.9;
    const dReal* Pos, * R;
    dReal   x[3],w,l,
            pep[HumanJointsNo],
            ang[HumanJointsNo],
            posh[HumanPartsNo+1][3],
            posd[HumanPartsNo+1][3],
            posf[HumanPartsNo+1][3],Rf[12];
    dVector3 sides;   // z: position, b: base, d:  direction
    
    Pos =dBodyGetPosition(Buggy[b][0].body);
    R = dBodyGetRotation(Buggy[b][0].body);

    if ((R[4]==0)&&(R[0]==0))
        mdir=0;
    else
        mdir=atan2(R[4],R[0]);      //  current Moving Direction
    
    t=sin(PI*0.01*BuggyInfo[b].T);
        
    for (int i=0; i< HumanPartsNo; i++)
        pep[i]=HumanPosute[m][i]*PI+t*HumanLoco[m][i];
    
    ang[0]=pep[0]+pep[1];
    posh[0][0]=HumanBodyData2[0][5]*sin(ang[0]);    //  right lower leg
    posh[0][2]=HumanBodyData2[0][5]*cos(ang[0]);
    posd[0][0]=HumanBodyData2[0][5]*sin(ang[0])*0.5;
    posd[0][2]=posh[0][2]*0.5;

    ang[1]=pep[1];
    posh[1][0]=posh[0][0]+HumanBodyData2[1][5]*sin(ang[1]);    //  right upper leg
    posh[1][2]=posh[0][2]+HumanBodyData2[1][5]*cos(ang[1]);
    posd[1][0]=posh[0][0]+HumanBodyData2[1][5]*sin(ang[1])*0.5;
    posd[1][2]=posh[0][2]+HumanBodyData2[1][5]*cos(ang[1])*0.5;

    ang[2]=pep[2];
    posh[2][0]=posh[1][0]-HumanBodyData2[2][5]*sin(ang[2]);    //  left upper leg
    posh[2][2]=posh[1][2]-HumanBodyData2[2][5]*cos(ang[2]);
    posd[2][0]=posh[1][0]-HumanBodyData2[2][5]*sin(ang[2])*0.5;
    posd[2][2]=posh[1][2]-HumanBodyData2[2][5]*cos(ang[2])*0.5;
    
    ang[3]=pep[2]+pep[3];
    posh[3][0]=posh[2][0]-HumanBodyData2[3][5]*sin(ang[3]); //  left lower leg
    posh[3][2]=posh[2][2]-HumanBodyData2[3][5]*cos(ang[3]);
    posd[3][0]=posh[2][0]-HumanBodyData2[3][5]*sin(ang[3])*0.5;
    posd[3][2]=posh[2][2]-HumanBodyData2[3][5]*cos(ang[3])*0.5;

    ang[4]=pep[4];
    posh[4][0]=posh[1][0]+HumanBodyData2[4][5]*sin(ang[4]);     //  main lower body
    posh[4][2]=posh[1][2]+HumanBodyData2[4][5]*cos(ang[4]);
    posd[4][0]=posh[1][0]+HumanBodyData2[4][5]*sin(ang[4])*0.5;
    posd[4][2]=posh[1][2]+HumanBodyData2[4][5]*cos(ang[4])*0.5;

    ang[5]=pep[4]+pep[5];
    posh[5][0]=posh[4][0]+HumanBodyData2[5][5]*sin(ang[5]);     //  main upper body
    posh[5][2]=posh[4][2]+HumanBodyData2[5][5]*cos(ang[5]);
    posd[5][0]=posh[4][0]+HumanBodyData2[5][5]*sin(ang[5])*0.5;
    posd[5][2]=posh[4][2]+HumanBodyData2[5][5]*cos(ang[5])*0.5;

    ang[6]=pep[4]+pep[5]+pep[6];
    posh[6][0]=posh[5][0]+HumanBodyData2[6][5]*sin(ang[6]);     //  head
    posh[6][2]=posh[5][2]+HumanBodyData2[6][5]*cos(ang[6]);
    posd[6][0]=posh[5][0]+HumanBodyData2[6][5]*sin(ang[6])*0.5;
    posd[6][2]=posh[5][2]+HumanBodyData2[6][5]*cos(ang[6])*0.5;

    ang[7]=pep[7];
    posh[7][0]=posh[5][0]+HumanBodyData2[7][5]*sin(PI+ang[7]);     //  right upper arm
    posh[7][2]=posh[5][2]+HumanBodyData2[7][5]*cos(PI+ang[7]);
    posd[7][0]=posh[5][0]+HumanBodyData2[7][5]*sin(PI+ang[7])*0.5;
    posd[7][2]=posh[5][2]+HumanBodyData2[7][5]*cos(PI+ang[7])*0.5;

    ang[8]=pep[7]+pep[8];
    posh[8][0]=posh[7][0]+HumanBodyData2[8][5]*sin(PI+ang[8]);     //  right lower arm
    posh[8][2]=posh[7][2]+HumanBodyData2[8][5]*cos(PI+ang[8]);
    posd[8][0]=posh[7][0]+HumanBodyData2[8][5]*sin(PI+ang[8])*0.5;
    posd[8][2]=posh[7][2]+HumanBodyData2[8][5]*cos(PI+ang[8])*0.5;

    ang[9]=pep[9];
    posh[9][0]=posh[5][0]+HumanBodyData2[9][5]*sin(PI+ang[9]);     //  left upper arm
    posh[9][2]=posh[5][2]+HumanBodyData2[9][5]*cos(PI+ang[9]);
    posd[9][0]=posh[5][0]+HumanBodyData2[9][5]*sin(PI+ang[9])*0.5;
    posd[9][2]=posh[5][2]+HumanBodyData2[9][5]*cos(PI+ang[9])*0.5;

    ang[10]=pep[9]+pep[10];
    posh[10][0]=posh[9][0]+HumanBodyData2[10][5]*sin(PI+ang[10]);     //  left lower  arm
    posh[10][2]=posh[9][2]+HumanBodyData2[10][5]*cos(PI+ang[10]);
    posd[10][0]=posh[9][0]+HumanBodyData2[10][5]*sin(PI+ang[10])*0.5;
    posd[10][2]=posh[9][2]+HumanBodyData2[10][5]*cos(PI+ang[10])*0.5;

    
    if (m==1)
        l=posh[1][0];
    else
        l=posh[1][0]+Buggy_LENGTH;
    for (n=0; n<HumanPartsNo; n++){      //  rotation by z(mdir)
        w=(HumanBodyData2[n][2]+HumanBodyData2[n][3])*0.5;
        posf[n][0] = Pos[0]+(posd[n][0]-l)*cos(mdir)-w*sin(mdir);
        posf[n][1] = Pos[1]+(posd[n][0]-l)*sin(mdir)+w*cos(mdir);
        if (m==1)
            posf[n][2] = Pos[2]+posd[n][2];
        else
            posf[n][2] = posd[n][2];
    }

    
    dsSetColorAlpha(0.8,0.8,0.5,Calpha);
    for (n=0; n<HumanPartsNo; n++){               //  draw human model  HumanPartsNo
        if (n<7)
            makeBodyPartsB(ang[n], mdir);
        else
            makeBodyPartsB(PI+ang[n], mdir);
        R = dBodyGetRotation(BodyPartB.body);
        for (j=0; j<12; j++)
            Rf[j]=R[j];
        if (n==6){
            sides[1] = HumanBodyData2[n][3]*0.8;
            dsDrawSphere(posf[n], Rf, (float)sides[1]);
        }
        else if ((n==4)||(n==5)){
            sides[0] = HumanBodyData2[n][1]*2;
            sides[1] = HumanBodyData2[n][3]*2;
            sides[2] = HumanBodyData2[n][5];
            dsDrawBox(posf[n], Rf, sides);
        }
        else{
            sides[0] = HumanBodyData2[n][1]*0.8;
            sides[2] = HumanBodyData2[n][5]*0.8;
            dsDrawCapsule(posf[n], Rf, sides[2],sides[0]);
        }
        dBodyDestroy(BodyPartB.body);
    }
}



static void drawBuggy3(int b)    //  b; Buggy ID (WHILL)
{
    int i,j;
    const dReal* pos, * R;
    dReal posf[3], Rf[12];
    dReal sidesf[3]={Buggy_LENGTH, Buggy_WIDTH, Buggy_HEIGHT};
    
    dsSetColor (0,0.4+b*0.2,0.4+b*0.2);
//    dsSetTexture (DS_WOOD);
    pos =dBodyGetPosition(Buggy[b][0].body);
    R = dBodyGetRotation(Buggy[b][0].body);
    for (j=0; j<3; j++)
        posf[j]=pos[j];
    for (j=0; j<12; j++)
        Rf[j]=R[j];
    dsDrawBox(posf, Rf, sidesf);
    
    posf[0]=BuggyInfo[b].TarPos[0];
    posf[1]=BuggyInfo[b].TarPos[1];
    posf[2]=0.25;
    dsDrawCylinder(posf, Rf, 0.5, 0.02);        //  Target

    dsSetColor (1, 1, 1);
    for (i=1; i<=4; i++){
        const dReal* pos =dBodyGetPosition(Buggy[b][i].body);
        const dReal* R = dBodyGetRotation(Buggy[b][i].body);
        for (j=0; j<3; j++)
            posf[j]=pos[j];
        for (j=0; j<12; j++)
            Rf[j]=R[j];
        if(i == 1||i == 2){
            dsDrawSphere(posf, Rf, Buggy_CASTER);
        }else{
            dsDrawCylinder(posf, Rf, Buggy_THICK, Buggy_RADIUS);
        }
    }
    

    dVector3 z, r, d;   // z: position, r: base, d:  direction
    if (SensorShow==1){
        for (i=0; i<BuggySensorNo; i++){        //  sensing
            if (BuggyInfo[b].Dist[i] < BuggyInfo[b].Length)
                dsSetColorAlpha(1.0, 0.0, 0.0, 0.9);
            else
                dsSetColorAlpha(1.0, 1.0, 0.0, 0.3);
            dGeomRayGet(BuggySensor[b][i].geom, r, d);
            z[0] = r[0] + (d[0] * BuggyInfo[b].Dist[i]);
            z[1] = r[1] + (d[1] * BuggyInfo[b].Dist[i]);
            z[2] = r[2] + (d[2] * BuggyInfo[b].Dist[i]);
            z[3] = r[3] + (d[3] * BuggyInfo[b].Dist[i]);
            dsDrawLine(r, z);
        }
    }
    
    //add backrest
    dReal size[3] = {Backrest_LENGTH, Backrest_WIDTH, Backrest_HEIGHT};
    dsSetColor (0,0.4+b*0.2,0.4+b*0.2);
    dsDrawBox(dGeomGetPosition(Buggy[b][5].geom), dGeomGetRotation(Buggy[b][5].geom), size);
    
    drawHuman(b,1);
    
}


static void drawBuggy4(int b)    //  b; Buggy ID (RT1)
{
    int i,j;
    const dReal* pos, * R;
    dReal posf[3], Rf[12];
    dReal sidesf[3]={Buggy_LENGTH, Buggy_WIDTH, Buggy_HEIGHT};
    
    dsSetColor (0, 1.0,0.2+b*0.2);
//    dsSetTexture (DS_WOOD);
    pos =dBodyGetPosition(Buggy[b][0].body);
    R = dBodyGetRotation(Buggy[b][0].body);
    for (j=0; j<3; j++)
        posf[j]=pos[j];
    for (j=0; j<12; j++)
        Rf[j]=R[j];
    dsDrawBox(posf, Rf, sidesf);
    
    posf[0]=BuggyInfo[b].TarPos[0];
    posf[1]=BuggyInfo[b].TarPos[1];
    posf[2]=0.25;
    dsDrawCylinder(posf, Rf, 0.5, 0.02);        //  Target

    dsSetColor (1, 1, 1);
    for (i=1; i<=4; i++){
        const dReal* pos =dBodyGetPosition(Buggy[b][i].body);
        const dReal* R = dBodyGetRotation(Buggy[b][i].body);
        for (j=0; j<3; j++)
            posf[j]=pos[j];
        for (j=0; j<12; j++)
            Rf[j]=R[j];
        if(i == 1||i == 2){
            dsDrawSphere(posf, Rf, Buggy_CASTER);
        }else{
            dsDrawCylinder(posf, Rf, Buggy_THICK, Buggy_RADIUS);
        }
    }
    
    dVector3 z, r, d;   // z: position, r: base, d:  direction
    if (SensorShow==1){
        for (i=0; i<BuggySensorNo; i++){        //  sensing
            if (BuggyInfo[b].Dist[i] < BuggyInfo[b].Length)
                dsSetColorAlpha(1.0, 0.0, 0.0, 0.9);
            else
                dsSetColorAlpha(1.0, 1.0, 0.0, 0.3);
            dGeomRayGet(BuggySensor[b][i].geom, r, d);
            z[0] = r[0] + (d[0] * BuggyInfo[b].Dist[i]);
            z[1] = r[1] + (d[1] * BuggyInfo[b].Dist[i]);
            z[2] = r[2] + (d[2] * BuggyInfo[b].Dist[i]);
            z[3] = r[3] + (d[3] * BuggyInfo[b].Dist[i]);
            dsDrawLine(r, z);
        }
    }
    
    //add pole
    dsSetColor (0, 1.0,0.2+b*0.2);
    dsDrawCylinder(dGeomGetPosition(Buggy[b][5].geom), dGeomGetRotation(Buggy[b][5].geom), Pole_HEIGHT, Pole_RADIUS);
    
    //add handle
    dsSetColor (0, 1.0,0.2+b*0.2);
    dsDrawCylinder(dGeomGetPosition(Buggy[b][6].geom), dGeomGetRotation(Buggy[b][6].geom), Handle_LENGTH, Handle_RADIUS);
    
    drawHuman(b,0);
    
}


static void makeBuggy()
{
    int b, k;
    for (b=0; b<BuggyNo; b++){
        BuggyInfo[b].T=0;
        BuggyInfo[b].Mode=0;
        BuggyInfo[b].Angle=0.51;
        BuggyInfo[b].Length=BuggyRangeMax;
        
        BuggyInfo[b].Persona=1+b%PersonaNo;
        BuggyInfo[b].Task=0;
        BuggyTargetSelection2(b);
        
        if (BuggyInfo[b].Type==1)           //  (WHILL)
            makeBuggyBody1(b);
        else if (BuggyInfo[b].Type==2)      //  (RT1)
            makeBuggyBody2(b);
        else if (BuggyInfo[b].Type==3)      //  (WHILL)  with sitting Human
            makeBuggyBody1(b);
        else if (BuggyInfo[b].Type==4)      //  (RT1)   with walking human
            makeBuggyBody2(b);
        MakeBuggySensor(b);
//        BuggyInfo[b].Mode=1;
    }
//    write_Buggy_data_Open();
}
