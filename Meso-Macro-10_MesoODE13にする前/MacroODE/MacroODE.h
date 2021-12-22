// Draw by ODE



#pragma mark -
#pragma mark Environment

int viewpoint_change=0,
    Robo_view=1;

void viewpoint_update()
{
    
    float xyz[5],hpr[5];
    
    if (Robo_view == 1) {
        xyz[0] =   0; xyz[1] =   0; xyz[2] = 8.0;
        hpr[0] = -90; hpr[1] = -90; hpr[2] =   0;
    }
    else if (Robo_view == 2) {
        xyz[0] =   0; xyz[1] =   0; xyz[2] = 6.0;
        hpr[0] = -90; hpr[1] = -90; hpr[2] =   0;
    }
    else if (Robo_view == 3) {
           xyz[0] = -2.5; xyz[1] = -2.5; xyz[2] = 0.5;
           hpr[0] = 45; hpr[1] = 0; hpr[2] =   0;
    }
    else {
        xyz[0] = 2.5; xyz[1] = 2.5; xyz[2] = 0.5;
        hpr[0] =  225; hpr[1] =   0; hpr[2] = 0;
    };
    dsSetViewpoint(xyz, hpr);
    
    viewpoint_change=0;
}

void initDevices()   //  people, robot size
{
    int i;
    
    for (i=0; i<LocalDEVICE; i++){
        HumanInfo[i].sizes[0]=0.06;    //  sizes
        HumanInfo[i].sizes[1]=0.4;    //  sizes
        HumanInfo[i].sizes[2]=0.1;    //  sizes

        BuggyInfo[i].sizes[0]=0.08; //  Radius
        BuggyInfo[i].sizes[1]=0.08;  //  Head Length
        BuggyInfo[i].sizes[2]=0.3;  //  Body Length

        RobotInfo[i].sizes[0]=0.08;  //  Head radius
        RobotInfo[i].sizes[1]=0.12;  //  Body radius
    }
    
}


void drawEnv()     // display all
{
    int i,j,k,m,t;
    double v;
    dReal   Rf[12]={1.0,0,0,0, 0,1.0,0,0, 0,0,1.0,0},
            posf[3], sizef[3];

    for (i=0; i<HumanNo; i++){
        if (HumanInfo[i].Valid==1){
            dsSetColorAlpha(0.8, 0.4, 0.2, 0.98);
            posf[0]=HumanInfo[i].pos[0];
            posf[1]=HumanInfo[i].pos[1];
            posf[2]=HumanInfo[i].pos[2]+HumanInfo[i].sizes[1]*0.85;
            dsDrawSphere(posf, Rf, (float)HumanInfo[i].sizes[0]);
            dsSetColorAlpha(0.2, 0.8, 0.2, 0.98);
            dsDrawCapsule(HumanInfo[i].pos, Rf,
                          HumanInfo[i].sizes[1], HumanInfo[i].sizes[2]);
        }
    }

    for (i=0; i<BuggyNo; i++){
        if (BuggyInfo[i].Valid==1){
            sizef[0]=0.01;                      //  radius
            sizef[1]=BuggyInfo[i].sizes[1];     //  length
            posf[0]=BuggyInfo[i].pos[0];
            posf[1]=BuggyInfo[i].pos[1];
            posf[2]=BuggyInfo[i].pos[2]
                   +BuggyInfo[i].sizes[2]*0.5+BuggyInfo[i].sizes[1]*1.5;
            dsSetColorAlpha(0.6, 0.4, 0.4, 0.98);
            dsDrawCylinder(posf, Rf, sizef[1], sizef[0]);
            posf[2]=BuggyInfo[i].pos[2]
                   +BuggyInfo[i].sizes[2]*0.5+BuggyInfo[i].sizes[1]*0.8;
            dsSetColorAlpha(0.4, 0.4, 0.4, 0.98);
            dsDrawCylinder(posf, Rf,
                           BuggyInfo[i].sizes[1], BuggyInfo[i].sizes[0]);
            dsSetColorAlpha(0.4, 0.2, 0.4, 0.98);
            dsDrawCylinder(BuggyInfo[i].pos, Rf,
                           BuggyInfo[i].sizes[2], BuggyInfo[i].sizes[0]);
        }
    }

    for (i=0; i<RobotNo; i++){
        if (RobotInfo[i].Valid==1){
            sizef[0]=0.01;                      //  radius
            sizef[1]=RobotInfo[i].sizes[1];     //  length
            posf[0]=RobotInfo[i].pos[0];
            posf[1]=RobotInfo[i].pos[1];
            posf[2]=RobotInfo[i].pos[2]
                   +RobotInfo[i].sizes[2]*0.5+RobotInfo[i].sizes[1]*1.5;
            dsSetColorAlpha(1.0, 0.4, 1.0, 0.98);
            dsDrawSphere(posf, Rf, (float)RobotInfo[i].sizes[0]);
            posf[2]=RobotInfo[i].pos[2]
                   +RobotInfo[i].sizes[2]*0.5;
            dsSetColorAlpha(0.4, 0.2, 0.4, 0.98);
            dsDrawSphere(posf, Rf, (float)RobotInfo[i].sizes[1]);
        }
    }
    
    dsSetColorAlpha(1.0, 1.0, 1.0, 1.0);
    m=gngID;
    if ((TraShow>0)&&(GNGinfo[m].state>0)){
        currenttime();
        t=loghour*3600+logmin*60+logsec;
        for (i=0; i<GNGinfo[m].dataNo; i++){     //  data distribution
            v=(double)(t-GNGinfo[m].tdata[i][7]);
            if ((TraShow==1)||((v<15)&&(GNGinfo[m].tdata[i][7]<=t))){
                v=1.0-v*0.04;
                if(TraShow==2)
                    dsSetColorAlpha(1.0, 1.0, 1.0, v);
                posf[2]=0.04;
                posf[0]=GNGinfo[m].tdata[i][0];
                posf[1]=GNGinfo[m].tdata[i][1];
                dsDrawSphere(posf, Rf, 0.03);
            }
        }
    }
    
};

static void start()
{
    dAllocateODEDataForThread(dAllocateMaskAll);
    viewpoint_update();
}

static void command (int cmd)       // called when a key pressed
{
    int i;
    switch (cmd) {
        case 'q':
            if (SimulationMode==13)
                writeNdata(0);
            exit(1);
            break;
            
        case '1':
            TraShow++;
            if (TraShow>=3)
                TraShow=0;
            break;

        case '2':
            GNGShow++;
            if (GNGShow>=2)
                GNGShow=0;
            break;

        case 'w':
        case 'W':    //  write teaching data
            writeTdata(gngID);
            break;

        case 'e':
        case 'E':    //  write Node data
            writeNdata(0);
            break;

        case 32: //viewpoint
            viewpoint_change=1;
            Robo_view++;
            if (Robo_view>3) Robo_view = 0;
        break;
            
    }
}

static void simLoop(int pause)      //  modes controlled by Stated
{
    int h,i,j;

    //  viewpoint update - - - - -
    if (viewpoint_change==1)
        viewpoint_update();
        
    
    if ((SimulationMode==10)||(SimulationMode==11))
        SensorHumanPosition();      //  Whether or no a person reaches a robot
    else if (SimulationMode==12)
        GNG_main(gngID);
    else if (SimulationMode==13){
        if (GNGShow==1)
            drawGNG(gngID);
    }
    drawEnv();
}
