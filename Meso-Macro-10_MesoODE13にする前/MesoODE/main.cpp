// compile with g++ -I/usr/local/include -L/usr/local/lib -lode -ldrawstuff -lm -framework GLUT -framework OpenGL -o a.out



/*************************************************************************
 *                                                                       *
 * Open Dynamics Engine, Copyright (C) 2001,2002 Russell L. Smith.       *
 * All rights reserved.  Email: russ@q12.org   Web: www.q12.org          *
 *                                                                       *
 * This library is free software; you can redistribute it and/or         *
 * modify it under the terms of EITHER:                                  *
 *   (1) The GNU Lesser General Public License as published by the Free  *
 *       Software Foundation; either version 2.1 of the License, or (at  *
 *       your option) any later version. The text of the GNU Lesser      *
 *       General Public License is included with this library in the     *
 *       file LICENSE.TXT.                                               *
 *   (2) The BSD-style license that is included with this library in     *
 *       the file LICENSE-BSD.TXT.                                       *
 *                                                                       *
 * This library is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the files    *
 * LICENSE.TXT and LICENSE-BSD.TXT for more details.                     *
 *                                                                       *
 *************************************************************************/


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
// #include "textures/texturepath.h"

#include "main.h"
#include "env.h"
#include "HSR.h"
#include "robot.h"
#include "buggy.h"
#include "human.h"


#pragma mark -
#pragma mark --- Main Functions ----


dJointID collision_joint;
static void nearCallback(void *data, dGeomID o1, dGeomID o2)
{
    int b,h,i,j,k=0;
    static const int N = 10;
    dContact contact[N];

    dReal l;    //  measurement length
    
    int g1 = (o1 == ground);
    int g2 = (o2 == ground);
    
    
    dBodyID b1 = dGeomGetBody(o1);
    dBodyID b2 = dGeomGetBody(o2);
    if (b1 && b2 && dAreConnected(b1, b2)) return;
    
    for (b=0; b<BuggyNo; b++)
        for (i=0; i<BuggySensorNo; i++)    //  LRF
            if (o1==BuggySensor[b][i].geom || o2==BuggySensor[b][i].geom){
                return;
            }
    
    for (h=0; h<PeopleNo; h++)
        for (i=0; i<HumanSensorNo; i++)   //  LRF
            if (o1==HumanSensor[h][i].geom || o2==HumanSensor[h][i].geom){
                return;
            }
    
    int n = dCollide(o1, o2, N, &contact[0].geom, sizeof(dContact));
    
    if (n > 0) {
        for (int i = 0; i < n; i++) {
            contact[i].surface.mode = dContactSlip1 | dContactSlip2 | dContactSoftERP | dContactSoftCFM | dContactApprox1;
            contact[i].surface.mu = dInfinity;
            contact[i].surface.slip1 = 0.0001;
            contact[i].surface.slip2 = 0.0001;
            contact[i].surface.soft_erp = 0.4;
            contact[i].surface.soft_cfm = 1e-4;
            dJointID c = dJointCreateContact(world, contactgroup, &contact[i]);
            dJointAttach(c, dGeomGetBody(contact[i].geom.g1),dGeomGetBody(contact[i].geom.g2));
        }
    }
}



// start simulation - set viewpoint

static float xyz[3] = {0.8317f,-0.9817f,0.8000f};   //  initial viewpoint
static float hpr[3] = {121.0000f,-27.5000f,0.0000f};
float xyz2[3] = {0.8317f,-0.9817f,0.8000f};         //  viewpoint


static void start()
{
  dAllocateODEDataForThread(dAllocateMaskAll);

  dsSetViewpoint (xyz,hpr);
  printf ("Press:\t'a' to increase speed.\n"
	  "\t'z' to decrease speed.\n"
	  "\t',' to steer left.\n"
	  "\t'.' to steer right.\n"
	  "\t' ' to reset speed and steering.\n"
	  "\t'1' to save the current state to 'state.dif'.\n");
}


// called when a key pressed

static void command (int cmd)
{
    int b,h,i;
    switch (cmd) {
        case '1':          //  Show Sensor data ?
            if (SensorShow==0)
                SensorShow=1;
            else
                SensorShow=0;
            break;

        case 'q':          //  write data & quit
//            write_Buggy_data_Close();
            exit(1);
        
        case 'a':          //  change the direction
            for (b=0; b<BuggyNo; b++){
                if (BuggyInfo[b].Mode==0)
                    BuggyInfo[b].Mode=1;
                else if (BuggyInfo[b].Mode==3)
                    BuggyInfo[b].Mode=2;
            }
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
    int b,h,i,j;
    static int t=0;
        
    dWorldStep(world, WStep);
    dJointGroupEmpty(contactgroup);

//      viewpoint update - - - - -
    if (Robo_view==1)
        viewpoint_update_online();
    else
        if (viewpoint_change==1)
        viewpoint_update();
    
//  Environment main - - - - - - - - - - - - - - - - - - - -
 
    dSpaceCollide(space, 0, &nearCallback);
    
    humanMove();
    for (h=0; h<PeopleNo; h++)
        drawPeople(h);
    drawEnv();
    
    if (RobotHSR==1){
        control_Robot();
        drawHSR();
    }
    drawRobotC();
    
//    if (t%30==0){
//        for (i=0; i<PeopleNo; i++)
//            sendHumandata(i);
//    }
//    if (t%120==0){
//        for (i=0; i<RobotNo; i++)
//            sendRobotCdata(i);
//        for (i=0; i<BuggyNo; i++)
//            sendBuggydata(i);
//        t=0;
//    }
//    t++;
    
    for (b=0; b<BuggyNo; b++){
        control_Buggy(b);
        if (BuggyInfo[b].Type==1)
            drawBuggy1(b);
        else if (BuggyInfo[b].Type==2)
            drawBuggy2(b);
        else if (BuggyInfo[b].Type==3)
            drawBuggy3(b);
        else if (BuggyInfo[b].Type==4)
            drawBuggy4(b);
    }
}

void init()
{
    int b,k,i;
    
    for (i=0; i<DEVICE; i++){
        DeviceReID[i]=-1;
        DeviceReType[i]=-1;
    }
    
    initColorMap();
    ReadSensor();
    ReadFurniture();
    
    ReadPostures();     //  read human posture data
    ReadPersona();      //  read Persona Model
    makeEnv();
    
    ReadRobotC();   //  Communication Robots
    if (RobotHSR==1)
        makeRobotH();

    ReadBuggy();
    makeBuggy();
    
    InitAllTargets();       //  Target Points
     
    init_People();
    
    for (i=0; i<DEVICE; i++)
         if (DeviceReType[i]>=0)
             printf("D[%d] Type:%d, Local ID:%d\n",
                    i, DeviceReType[i], DeviceReID[i]);
    
//    for (b=0; b<BuggyNo; b++)
//        BuggyTargetSelection(b);
    
//    sendinitdata(PeopleNo, BuggyNo, RobotNo);
}



int main (int argc, char **argv)
{
    int i,j;
    WStep = 0.01;

//    if (argc==2){    //  a.out c   (2 words: [a.out] + [c])
//        ServerMode=1;
//        Con_UDP_receive();
//    }
//    else
//        ISS_UDP_receive();  //  a.out
    
    dsFunctions fn;
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
    
    init();
    
    // run simulation
    dsSimulationLoop (argc,argv,500,500,&fn);
    
    dJointGroupDestroy (contactgroup);
    dSpaceDestroy (space);
    dWorldDestroy (world);
    dCloseODE();
    return 0;
}
