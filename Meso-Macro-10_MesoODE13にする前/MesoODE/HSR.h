
// Pay attention to the setting
//void dMassSetCylinder (dMass *, dReal density, int direction, dReal radius, dReal length);
//dGeomID dCreateCylinder ( dSpaceID space, dReal radius, dReal length );
//void dsDrawCylinderD ( const double pos[3], const double R[12], float length, float radius );


#pragma mark -
#pragma mark HSR Robot

#define ObjectNo       5        //  5


MyObject    Obj[ObjectNo],      //  Object in environment
            HSR_Base, HSR_Body,
            HSR_Link0,HSR_Link1,HSR_Link2, HSR_Link3, HSR_Link4,
            BaseLink, Link[10];

dJointFeedback fbJoint[10];

double  TaPos[3]= {0.0, 0.3,0.4},
        TaSize[3]={1.0, 0.2, 0.4};      //  Table Size in Env.

double  PosOriginX = 0,
        PosOriginY = 0,
        PosOriginZ = 0.07;

double posBase[3] = {PosOriginX, PosOriginY, PosOriginZ};
double sizeBase[2] = {0.21, 0.3};

double posBody[3] = { posBase[0], posBase[1]-0.04, posBase[2] + 0.175 + 0.07 };
double sizeBody[3] = {0.30, 0.08, 0.5};

double posLink0[3] = { posBase[0], posBase[1]+0.03, posBase[2]+0.10 };  // slider
double sizeLink0[3] = {0.08, 0.06, 0.14};

double posLink1[3] = { posBase[0], posBase[1]+0.03, posBase[2]+0.24 };
double sizeLink1[3] = {0.04, 0.04, 0.14};

double posLink2[3] = { posBase[0], posBase[1]+0.03, posBase[2]+0.38 };
double sizeLink2[3] = {0.04, 0.04, 0.10};

double posLink3[3] = {  posBase[0], posBase[1]+0.03, posBase[2]+0.48 };
double sizeLink3[3] = {0.04, 0.04, 0.08};

double posLink4[3] = {  posBase[0], posBase[1]+0.03, posBase[2]+0.56 };
double sizeLink4[3] = {0.04, 0.04, 0.005};


dReal   RTar[TIME][10];       //  robot target

dReal   ABody=0,            //  Body Angle
        ZSlideTar=0.3,      //  Target Position of Z in Slider
        ZSlide=0.3,         //  Initial Position of HSR Slider
        targete[10], targete2[10];


int RobotHSR=0,         //  0: no HSR,  1: with HSR
    RobotTargetNo,    //  Number of teaching data
    RobotTargetID;     //  robot state


void control_Robot()
{
    const dReal* pos;
    float  u, err, err1, err2, terr=0, terr2=0,
            kp = 30,  kd = 0.1;
    int i;
    
    for (i=0; i<6; i++){
        if (i==1){
            targete[i+3] = dJointGetSliderPosition (Link[i].joint);
            err = RTar[RobotTargetID][i+3]-targete[i+3];
            terr+=fabs(err);
            u = kp * err;
            dJointSetSliderParam(Link[i].joint, dParamVel, u);
            dJointSetSliderParam(Link[i].joint, dParamFMax, 20);
        }
        else {
            targete[i+3] = dJointGetHingeAngle(Link[i].joint);
            err = RTar[RobotTargetID][i+3]-targete[i+3];
            err1= RTar[RobotTargetID][i+3]-targete[i+3]+PI*2.0;
            if (fabs(err1)<fabs(err))
                err=err1;
            terr+=fabs(err*err);
            err2= targete[i+3]-targete2[i+3];
            terr2+=fabs(err2);
            u = kp * err + kd * err2;
            dJointSetHingeParam(Link[i].joint, dParamVel, u);
            dJointSetHingeParam(Link[i].joint, dParamFMax, 20);
            targete2[i+3]=targete[i+3];
        }
    }
}

void control_RobotData() {   //  teaching data generation
    
    const dReal* pos;
    float   u, err, err1, err2, terr=0, terr2=0,
            kp = 30,  kd = 0.1;
    int i;
    
    for (i=0; i<6; i++){
        if (i==1){
            targete[i+3] = dJointGetSliderPosition (Link[i].joint);
            err = RTar[RobotTargetID][i+3]-targete[i+3];
            terr+=fabs(err);
//                err2= targete[i]-targete2[i];
//                terr2+=err2*err2;
            u = kp * err;
            dJointSetSliderParam(Link[i].joint, dParamVel, u);
            dJointSetSliderParam(Link[i].joint, dParamFMax, 20);
//                targete2[i]=targete[i];
        }
        else {
            targete[i+3] = dJointGetHingeAngle(Link[i].joint);
//            err = RobotTarget[RobotTargetID][i]*M_PI-targete[i];
            err = RTar[RobotTargetID][i+3]-targete[i+3];
            err1= RTar[RobotTargetID][i+3]-targete[i+3]+PI*2.0;
            if (fabs(err1)<fabs(err))
                err=err1;
            terr+=fabs(err*err);
            err2= targete[i+3]-targete2[i+3];
            terr2+=fabs(err2);
            u = kp * err + kd * err2;
            dJointSetHingeParam(Link[i].joint, dParamVel, u);
            dJointSetHingeParam(Link[i].joint, dParamFMax, 20);
            targete2[i+3]=targete[i+3];
        }
    }
}

void RobotTarget()  //  target data generation
{
    int h,i,j,k,x;
    
    RobotTargetNo=0;
    for (h=0; h<20; h++)
        for (i=0; i<5; i++)
            for (j=0; j<5; j++)
                for (k=0; k<5; k++){
                    x=RobotTargetNo;
//                    x=h*125+i*25+j*5+k;
                    RTar[x][3]=(double)(h-10)*M_PI/20.0;
                    RTar[x][4]=ZSlideTar;
                    RTar[x][5]=(double)i*M_PI/10.0;
                    RTar[x][6]=(double)j*M_PI/10.0;
                    RTar[x][7]=(double)k*M_PI/10.0;
                    RTar[x][8]=0;
                    
                    RobotTargetNo++;
//                    printf("Teaching data Generation:%d [%d][%d][%d]\n",x,i,j,k);
                }
    
    printf("Teaching data Generation: %d, finished\n",RobotTargetNo);
}


void makeRobotH(){
    int i;
    dMass m;
    dMatrix3 R;
    
    HSR_Base.body = dBodyCreate(world);
    dMassSetZero(&m);
    dBodySetPosition(HSR_Base.body, posBase[0], posBase[1], posBase[2]);
    dMassSetCylinder(&m, 0.225, 3, sizeBase[0], sizeBase[1]);
    dBodySetMass(HSR_Base.body, &m);
    HSR_Base.geom = dCreateCylinder(space, sizeBase[0], sizeBase[1]);
    dGeomSetBody(HSR_Base.geom, HSR_Base.body);
    
    HSR_Body.body = dBodyCreate(world);
    dMassSetZero(&m);
    dBodySetPosition(HSR_Body.body, posBody[0], posBody[1], posBody[2]);
    dMassSetBox(&m, 0.225, sizeBody[0], sizeBody[1], sizeBody[2]);
    dBodySetMass(HSR_Body.body, &m);
    HSR_Body.geom = dCreateBox(space, sizeBody[0], sizeBody[1], sizeBody[2]);
    dGeomSetBody(HSR_Body.geom, HSR_Body.body);
    
    HSR_Link0.body = dBodyCreate(world);
    dMassSetZero(&m);
    dBodySetPosition(HSR_Link0.body, posLink0[0], posLink0[1], posLink0[2]);
    dMassSetBox(&m, 0.05, sizeLink0[0], sizeLink0[1], sizeLink0[2]);
    dBodySetMass(HSR_Link0.body, &m);
    HSR_Link0.geom = dCreateBox(space, sizeLink0[0], sizeLink0[1], sizeLink0[2]);
    dGeomSetBody(HSR_Link0.geom, HSR_Link0.body);
    
    HSR_Link1.body = dBodyCreate(world);
    dMassSetZero(&m);
    dBodySetPosition(HSR_Link1.body, posLink1[0], posLink1[1], posLink1[2]);
    dMassSetBox(&m, 0.05, sizeLink1[0], sizeLink1[1], sizeLink1[2]);
    dBodySetMass(HSR_Link1.body, &m);
    HSR_Link1.geom = dCreateBox(space, sizeLink1[0], sizeLink1[1], sizeLink1[2]);
    dGeomSetBody(HSR_Link1.geom, HSR_Link1.body);
    
    HSR_Link2.body = dBodyCreate(world);
    dMassSetZero(&m);
    dBodySetPosition(HSR_Link2.body, posLink2[0], posLink2[1], posLink2[2]);
    dMassSetBox(&m, 0.05, sizeLink2[0], sizeLink2[1], sizeLink2[2]);
    dBodySetMass(HSR_Link2.body, &m);
    HSR_Link2.geom = dCreateBox(space, sizeLink2[0], sizeLink2[1], sizeLink2[2]);
    dGeomSetBody(HSR_Link2.geom, HSR_Link2.body);

    HSR_Link3.body = dBodyCreate(world);
    dMassSetZero(&m);
    dBodySetPosition(HSR_Link3.body, posLink3[0], posLink3[1], posLink3[2]);
    dMassSetBox(&m, 0.05, sizeLink3[0], sizeLink3[1], sizeLink3[2]);
    dBodySetMass(HSR_Link3.body, &m);
    HSR_Link3.geom = dCreateBox(space, sizeLink3[0], sizeLink3[1], sizeLink3[2]);
    dGeomSetBody(HSR_Link3.geom, HSR_Link3.body);

    HSR_Link4.body = dBodyCreate(world);
    dMassSetZero(&m);
    dBodySetPosition(HSR_Link4.body, posLink4[0], posLink4[1], posLink4[2]);
    dMassSetBox(&m, 0.05, sizeLink4[0], sizeLink4[1], sizeLink4[2]);
    dBodySetMass(HSR_Link4.body, &m);
    HSR_Link4.geom = dCreateBox(space, sizeLink4[0], sizeLink4[1], sizeLink4[2]);
    dGeomSetBody(HSR_Link4.geom, HSR_Link4.body);
    
    BaseLink.joint = dJointCreateFixed(world,0);
    dJointAttach(BaseLink.joint, 0, HSR_Base.body);
    dJointSetFixed(BaseLink.joint);
   
    Link[0].joint = dJointCreateHinge(world, 0);
    dJointAttach(Link[0].joint, HSR_Body.body, HSR_Base.body);
    dJointSetHingeAxis(Link[0].joint, 0, 0, 1);
    dJointSetHingeAnchor(Link[0].joint,  posBase[0], posBase[1], posBase[2]);
    dJointSetFeedback(Link[0].joint, &fbJoint[0]);
    
    Link[1].joint = dJointCreateSlider (world, 0);                  //  slider
    dJointAttach (Link[1].joint, HSR_Link0.body, HSR_Body.body);
    dJointSetSliderAxis(Link[1].joint, 0, 0, 1);
    dJointSetSliderParam (Link[1].joint, dParamLoStop, -0.5);
    dJointSetSliderParam (Link[1].joint, dParamHiStop, 1.5);
    
    Link[2].joint = dJointCreateHinge(world, 0);
    dJointAttach(Link[2].joint, HSR_Link1.body, HSR_Link0.body);
    dJointSetHingeAxis(Link[2].joint, -1, 0, 0);
    dJointSetHingeAnchor(Link[2].joint,  posBase[0], posBase[1]+0.03, posBase[2]+0.17);
    dJointSetFeedback(Link[2].joint, &fbJoint[1]);

    Link[3].joint = dJointCreateHinge(world, 0);
    dJointAttach(Link[3].joint, HSR_Link2.body, HSR_Link1.body);
    dJointSetHingeAxis(Link[3].joint, -1, 0, 0);
    dJointSetHingeAnchor(Link[3].joint,  posBase[0], posBase[1]+0.03, posBase[2]+0.31);
    dJointSetFeedback(Link[3].joint, &fbJoint[2]);

    Link[4].joint = dJointCreateHinge(world, 0);
    dJointAttach(Link[4].joint, HSR_Link3.body, HSR_Link2.body);
    dJointSetHingeAxis(Link[4].joint, -1, 0, 0);
    dJointSetHingeAnchor(Link[4].joint,  posBase[0], posBase[1]+0.03, posBase[2]+0.41);
    dJointSetFeedback(Link[4].joint, &fbJoint[3]);

    Link[5].joint = dJointCreateHinge(world, 0);
    dJointAttach(Link[5].joint, HSR_Link4.body, HSR_Link3.body);
    dJointSetHingeAxis(Link[5].joint, -1, 0, 0);
    dJointSetHingeAnchor(Link[5].joint,  posBase[0], posBase[1]+0.03, posBase[2]+0.49);
    dJointSetFeedback(Link[5].joint, &fbJoint[3]);
 
   // init - - - -
    
    RobotTarget();  //  target data generation
    RobotTargetID=(int)(RobotTargetNo*0.5);
    for (i=3;i<9;i++){
        
        targete[i]=0;
        targete2[i]=0;
    }
    
}

void drawHSR()
{
    int i,j,k;
    const dReal* pos, * R;
    dReal   sizef[3], posf[3],
    Rf[12]={ 1,0,0,0, 0,1,0,0, 0,0,1,0};
    double* radius = (double*)malloc(sizeof(double) * 2);
    double* length = (double*)malloc(sizeof(double) * 2);
    dVector3 sides;
    
    dsSetColor(0.2,0.2,0.2);
    dGeomCylinderGetParams(HSR_Base.geom, radius, length);
    pos = dBodyGetPosition(HSR_Base.body);
    R = dBodyGetRotation(HSR_Base.body);
    dsDrawCylinder(pos, R, length[0], radius[0]);

    dsSetColor(0.7, 0.7,0.7);                   //  body
    dGeomBoxGetLengths(HSR_Body.geom, sides);
    pos = dBodyGetPosition(HSR_Body.body);
    R = dBodyGetRotation(HSR_Body.body);
    dsDrawBox(pos, R, sides);
    
    dsSetColor(0.7, 0.5,0.5);
    dGeomBoxGetLengths(HSR_Link0.geom, sides);
    pos = dBodyGetPosition(HSR_Link0.body);
    R = dBodyGetRotation(HSR_Link0.body);
    dsDrawBox(pos, R, sides);

    dsSetColor(0.0, 0.5,0.5);
    dGeomBoxGetLengths(HSR_Link1.geom, sides);
    pos = dBodyGetPosition(HSR_Link1.body);
    R = dBodyGetRotation(HSR_Link1.body);
    dsDrawBox(pos, R, sides);

    dsSetColor(1.0, 0.5,0.5);
    dGeomBoxGetLengths(HSR_Link2.geom, sides);
    pos = dBodyGetPosition(HSR_Link2.body);
    R = dBodyGetRotation(HSR_Link2.body);
    dsDrawBox(pos, R, sides);

    dsSetColor(0.8, 0.7,0.5);
    dGeomBoxGetLengths(HSR_Link3.geom, sides);
    pos = dBodyGetPosition(HSR_Link3.body);
    R = dBodyGetRotation(HSR_Link3.body);
    dsDrawBox(pos, R, sides);

    dsSetColor(1.0, 0.0, 0.0);
    dGeomBoxGetLengths(HSR_Link4.geom, sides);
    pos = dBodyGetPosition(HSR_Link4.body);
    R = dBodyGetRotation(HSR_Link4.body);
    dsDrawBox(pos, R, sides);
    
    free(radius);
    free(length);

};


