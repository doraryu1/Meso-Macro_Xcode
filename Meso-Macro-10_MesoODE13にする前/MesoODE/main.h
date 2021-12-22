#ifdef dDOUBLE
#define dsDrawLine dsDrawLineD
#define dsDrawBox dsDrawBoxD
#define dsDrawSphere dsDrawSphereD
#define dsDrawCylinder dsDrawCylinderD
#define dsDrawCapsule dsDrawCapsuleD
#endif


#ifdef _MSC_VER
#pragma warning(disable:4244 4305)  // for VC++, no precision loss complaints
#endif

#define PI 3.14159

#pragma mark -
#pragma mark --- Basic Functions ----

double rnd() // uniform random number ( 0 <= rnd() < 1.0)
{
    return((double)(rand()%RAND_MAX)/(double)RAND_MAX);
}

double rndn()            //   normal random number
{
    return (rnd()+rnd()+rnd()+rnd()+rnd()+rnd()+
            rnd()+rnd()+rnd()+rnd()+rnd()+rnd()-6.0);
}

double radf(double q)   // degree to radian
{
    return(q*PI/180.0);
}

double degf(double q)   // radian to degree
{
    return(q/PI*180.0);
}


#pragma mark -
#pragma mark Global Parameters


#define TIME  100000        //  TIME history

int     ServerMode=0,
        SensorShow=1,       //  0: No show, 1: Show - - - - - - -
        Started=-1;

#pragma mark -
#pragma mark UDP, TCP/IP Communication

// UDP  to get the Server IP Address - - - - - -

#define BUFSIZE         8192
#define ISS_UDP_PORT    9000    //  Port ID for UDP/IP ISS Database communication
#define Con_UDP_PORT    9100    //  Port ID for UDP/IP Theater Contents communication
#define ISS_TCP_PORT    9866    //  ISS Database Server
#define Con_TCP_PORT    9865    //  ISS Robot Theater Contents Server (this PC)
#define Macro_TCP_PORT    9864    //  ISS Macro Server (this PC)


char    ISS_ServerIP[30]="192.168.13.100",
        Con_ServerIP[30]="192.168.13.101",
        utter[BUFSIZE],
        buf[BUFSIZE];

int ISS_UDP_receive()    //  receive server (host) IP through UDP/IP ISS Database communication
{
    int i=0,
    sock;
    struct sockaddr_in addr;
    
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(ISS_UDP_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_len = sizeof(addr);
    
    bind(sock, (struct sockaddr *)&addr, sizeof(addr));
    memset(ISS_ServerIP, 0, sizeof(ISS_ServerIP));
    recv(sock, ISS_ServerIP, sizeof(ISS_ServerIP), 0);
    
    printf("ISS Database Server IP %s\n", ISS_ServerIP);
    close(sock);
    return(i);
}

int Con_UDP_receive()    //  receive server (host) IP through UDP/IP robot contents communication
{
    int i=0,
    sock;
    struct sockaddr_in addr;
    
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(Con_UDP_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_len = sizeof(addr);
    
    bind(sock, (struct sockaddr *)&addr, sizeof(addr));
    memset(Con_ServerIP, 0, sizeof(Con_ServerIP));
    recv(sock, Con_ServerIP, sizeof(Con_ServerIP), 0);
    
    printf("ISS Robot Contents Server IP %s\n", Con_ServerIP);
    close(sock);
    return(i);
}

// TCP/IP Communication (for Robot Client) - - - - - - - - - - - - - - -

void MacroTCP(){    // - -  send/receive data to ISS Macro Server
    
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
            printf(" No ISS Macro Server - State\n");
        }
        else{
            write(s, buf, strlen(buf));   //  sending data
            printf("Sent Data (Port:%d)  %s\n",
                   Macro_TCP_PORT, buf);
            memset(buf, 0, sizeof(buf));
            read(s, buf, sizeof(buf));  //  receiving data
            close(s);
            printf("Received Data: %s\n", buf);
            
        }
    }
}

void sendinitdata(int m, int l, int n){    // send init data: PeopleNo, BuggyNo, RobotNo

    int        i,j,k;
    char    numberName[12]="01234567890";
   
    memset(buf, 0, sizeof(buf));
    buf[0]=numberName[9];   //  999: init Data
    buf[1]=numberName[9];
    buf[2]=numberName[9];

    if (m>=10){      //  PeopleNo: 00-99
        k=(int)(m/10)%10;
        buf[3]=numberName[k];
    }
    else
        buf[3]=numberName[0];
    k=m%10;
    buf[4]=numberName[k];
    
    if (l>=10){      //  BuggyNo: 00-99
        k=(int)(l/10)%10;
        buf[5]=numberName[k];
    }
    else
        buf[5]=numberName[0];
    k=l%10;
    buf[6]=numberName[k];

    if (n>=10){      //  RobotNo: 00-99
        k=(int)(n/10)%10;
        buf[7]=numberName[k];
    }
    else
        buf[7]=numberName[0];
    k=n%10;
    buf[8]=numberName[k];

    printf("Sent Init Data: People:%d, Buggy:%d, Robots:%d, %s\n",
           m, l, n, buf);
    MacroTCP();             //  sending and receiving trough buf[]

}

void senddata(int m, double x[]){    // send ID:m, positions:x[]

    int        i,j,k,v;
    char    numberName[12]="01234567890";
       
    memset(buf, 0, sizeof(buf));
    if (m>=100){
        k=(int)(m/100)%10;
        buf[0]=numberName[k];   //  ID (000-999)
    }
    else
        buf[0]=numberName[0];   //  ID (000-999)
    if (m>=10){
        k=(int)(m/10)%10;
        buf[1]=numberName[k];   //  ID (000-999)
    }
    else
        buf[1]=numberName[0];   //  ID (000-999)
    k=m%10;
    buf[2]=numberName[k];       //  ID (000-999)
    
    for (i=0; i<3; i++){
        v=(int)((x[i]+50.0)*100.0); //  -50< x[i] < 50
        if (v>=1000){
            k=(int)(v/1000)%10;
            buf[i*4+3]=numberName[k];   //  ID (000-999)
        }
        else
            buf[i*4+3]=numberName[0];   //  ID (000-999)
        if (v>=100){
            k=(int)(v/100)%10;
            buf[i*4+4]=numberName[k];   //  ID (000-999)
        }
        else
            buf[i*4+4]=numberName[0];   //  ID (000-999)
        if (v>=10){
            k=(int)(v/10)%10;
            buf[i*4+5]=numberName[k];   //  ID (000-999)
        }
        else
            buf[i*4+5]=numberName[0];   //  ID (000-999)
        k=v%10;
        buf[i*4+6]=numberName[k];       //  ID (000-999)
    }
    printf("Sent Data: ID:%d, (%4.2f, %4.2f, %4.2f) %s\n",
           m, x[0], x[1], x[2], buf);
    MacroTCP();             //  sending and receiving trough buf[]
}


//  time - - - - - - - - - - - - - - - - - -

int logyear,
    logmon,
    logday,             //  the day from the starting day
    logweek,            // the day of week
    loghour,
    logmin,
    logsec,
    starttime=0;            //  startning time

void currenttime()     //   time
{
    time_t  curtime;    //  current time
    struct tm *t_st;    //  time struct

    time(&curtime);
    if (starttime==0)
        starttime=curtime;
    t_st = localtime(&curtime);
    logyear=t_st->tm_year+1900;
    logmon=t_st->tm_mon+1;
    logday=t_st->tm_mday;
    logweek=t_st->tm_wday;
    
    loghour=t_st->tm_hour;
    logmin=t_st->tm_min;
    logsec=t_st->tm_sec;
}

#pragma mark -
#pragma mark ODE

static dWorldID world;
static dSpaceID space;
static dJointGroupID contactgroup;
static dGeomID ground;
extern dsFunctions fn;

typedef struct {
    dBodyID body;
    dGeomID geom;
    dReal size[3];
    dReal weight;
    dReal position[3];
    dJointID joint;
}
MyObject;           //  Object Information for ODE

double WStep=0.02;

#pragma mark -
#pragma mark Meso-scopic Simulators


// People ---------------------------------

#define PEOPLE          20           // number of people

#define HumanPartsNo    11            // number of parts of a person
#define HumanJointsNo   15            // number of parts of a person
#define HumanPostures   10          //  the number of human postures
#define HumanPostureLength  5       //  the length (stages) of each human postures
#define PeopleModeNo    10            // number of mode of a person
#define HumanSensorNo   9          //  Number of LRF Sensing Directions


MyObject    BodyNode[PEOPLE+1], BodyPart,
            BodyPart1, BodyPart2,
            HumanSensor[PEOPLE+1][HumanSensorNo+1];


//   Furniture - -  Sensors  - - - - - - - - - - -
#define FURNITURE  200  //  Number of Furniture
#define SENSOR  300     //  Number of Sensors
#define ObNo       10    //  Number of walls

MyObject    EnvFurniture[FURNITURE+1],
            EnvObjects[ObNo+1];

dReal   ObPos[ObNo][10]={
                {-6.0, -6.0, 6.0, 6.0, 0, 9.0},     //  x1, y1, x2, y2, z1, z2
            },
        ObPosx[ObNo][10],
        ObSize[ObNo][10];

int FurnitureNo=10,     //  Number of Furniture
    SensorActivated=999,//  if the sensing is done
    issID=90,           //  initial ISS server ID      (the value may change)
    SensorID,           //  SensorID
    EnvSensorNoAll,     //  Number of all sensors (including voice recognitions)
    EnvSensorNo;        //  Number of Physical Sensors

typedef struct {
    int ID;         //  ID for communication
    dReal Pos[3];    //  position
    dReal TarPos[3];    //  target point to come to this object
    dReal Sizes[3]; //  size
    int Valid;      //  validity
    int Type;       //  Furniture Shape type
    int Color;       //  Color
    char Name[100];  //  name
    int HumanPosture;   //  Human posture ID when a person arrives here
    int State;          //  state
} Furniture_st;        //  Furniture information

Furniture_st    EnvSensorInfo[SENSOR+1],
                FurnitureInfo[FURNITURE+1];

dReal   ColorMap[5000][3],
        Colors[10][3]={
            {1.0, 1.0, 1.0},    //  0 White     4095
            {0.9, 0.1, 0.1},    //  1 Red       3857
            {0.1, 0.9, 0.1},    //  2 Green     497
            {0.1, 0.1, 0.9},    //  3 Blue      287
            {0.9, 0.1, 0.9},    //  4 Yellow    3878
            {0.9, 0.9, 0.1},    //  5 Purple    4081
            {0.1, 0.9, 0.9},    //  6 Sky Blue  511
            {0.4, 0.1, 0.1}    //  7 Dark Brown 1809
};

//  Target Points - - - - - - - - - -

#define TARGET     100  //  Number of Targets

typedef struct {
    int ID;         //  ID for communication
    dReal TarPos[3];    //  position
    int Valid;      //  validity
    int Type;       //  Target type     //  0: Furniture,  1: Communication Robot
    char Name[100];  //  name
    int State;          //  state
} Target_st;        //  sensor information
Target_st   TargetInfo[TARGET+1];
int TargetNo;       //  Number of all Targets


//   Communication Robots - - - - - - - - - - - - - - -- - - - - - - - - -
#define ROBOT  50  //  Number of Robots
#define DEVICE 200  //  Number of All Items/Devices

MyObject    RobotC[ROBOT];

int DeviceReID[DEVICE+1],   //  -1: Not Used,  Furniture/Robot/Sensor ID in this PC
    DeviceReType[DEVICE+1], //  Type:  -1: Not used,   0:Furniture, 1:Sensor, 2:Robot,
                            //                      3: Buggy, 4: People
    RobotNo=1;      //  Number of Robots

typedef struct {
    int ID;         //  Robot ID for communication
    dReal Pos[3];    //  position
    dReal TarPos[3];    //  position
    dReal Sizes[3];    //  sizes
    double VX;         //  velocity
    double VY;         //  velocity
    int Valid;      //  validity
    int Type;       //  Robot type
    int Language;   //  Language Mode
    int Gesture;    //  Gesture ID
    int Face;       //  Face ID
    int Utterance;  //  Utterance ID
    char Name[100];  //  name
    int Mode;       //  Mode
    int State;          //  state
    int TheaterID;    //  Theater ID
    int PhaseID;
    int SectionID;
    int ScenarioID;
    int SentenceID;
    int HumanPosture;   //  Human Posture
} Robot_st;        //  Robot information

Robot_st    RobotInfo[ROBOT+1];

//  Buggy- - - - - - - - - -

#define BUGGY        10           //  Number of Buggies
#define BUGGYPARTS   15             //  Number of Body Parts used in Buggy
#define BuggySensorNo   13           //  Number of LRF Sensing Directions

MyObject    Buggy[BUGGY+1][7],   // Buggy
            BuggySensor[BUGGY+1][BuggySensorNo+1];

int     BuggyNo=1;          //  Number of Buggies
float   BuggyRangeMin=0.8,     //  Min Sensing Range (Distance)
        BuggyRangeMax=1.5;     //  Max Sensing Range (Distance)

typedef struct {
    int ID;         //  ID
    int Type;       //  0: Buggy (WHILL), 1: RT1
    int Valid;      //  validity
    double Dist[BuggySensorNo+1];   //  range sensing
    double Danger;                  //  the degree of danger
    int Mode;       //  behavior mode;  0:forward, 1:transition to 2, 3: backward, 2:transition to 0
    int Counter;    //  Counter
    double Angle;   //  Sensing Angle
    double Length;  //  Sensing length
    double Pos[3];  //  position
    double dPos[3];  //  direction (dx,dy) to the target
    double Velocity;
    double Steering;
    double speedLR[2];
    double TarPos[3];  //  target position
//    double Dir;     //  target moving direction
    double TarDir;  //  target moving angle
    int T;  //  time
    int Task;       //  Task ID
    int TarID;      //  Target ID
    int Persona;    //  Persona ID
    int HumanPosture;   //  Human posture ID when a person arrives here
    int Language;   //  Language Mode
    int State;          //  state
    int TheaterID;    //  Theater ID
    int PhaseID;
    int SectionID;
    int ScenarioID;
    int SentenceID;

} Buggy_st;        //  People information

Buggy_st    BuggyInfo[BUGGY+1],     //  Bugggy
            HSRInfo[BUGGY+1];       //  HSR

//  People  - - - - - - - - - 

typedef struct {
    int Valid;      //  validity
    int ID;         //  Human ID for communication
    int Mode;       //  behavior mode; // 0:walking, 1:siting, 2;shaking hand, 3;bye-bye, 4;ground sitting
    double Pos[3];  //  Pos
    double NextPos[3];  // Next Position (the position of foot in the swing phase
    double TarPos[3];  //  target Pos
    double Joint[HumanJointsNo+1];  //  joint angle
    double Dir;     //  moving direction (angle)
    double Angle;   //  Sensing Angle
    double Dist;    //  Distance
    double TarDir;  //  target moving angle
    int PostureStage;   //  current stage in a posture
    int Stage;  //  moving direction stage  1: forward, -1: reverse
    int Time;   //  motion time
    int MaxTime;    //  max motion time
    int State;      //  state for communication with server
    int Task;       //  Task ID
    int TarID;      //  Target ID
    int Persona;    //  Persona ID
    int Language;   //  Language Mode (Japanese or English)
    int TheaterID;    //  Theater ID
    int PhaseID;
    int SectionID;
    int ScenarioID;
    int SentenceID;
} Human_st;        //  People information

Human_st   HumanInfo[PEOPLE+1];

typedef struct {
    int TaskNo;      //  the number of Tasks (target objects)
    int Model[10];       //  sequence of Target Objects
    char Name[100];  //  name
} Persona_st;        //  sensor information
Persona_st   PersonaInfo[10];

int PeopleNo=8,            // number of people
    PersonaNo;              //  Number of Persona Models

//  -- - - Robot Theater - - - - - - -

#define THEATER 10


int     TheaterNo;          //  the number of theaters

typedef struct {
    int Valid;      //  validity
    char TheaterName[50];           //  Theater Name
    char TheaterFileName[50];       // Theater File name
    char TransitionFileName[50];    //  Transition File name
    int State;          //  state
} Theater_st;        //  Theater File information

Theater_st   TheaterInfo[THEATER+1];


//  Utterance -----------------------------------------------

#define PHASE         4  //   the number of phases: introduction, development, turn and conclusion
#define SECTION      10  //   the number of sections: story transition
#define SCENARIO     10  //   the number of scenarios: scenarios dependent on user's interaction and communication
#define SENTENCE     20  //   the number of sentences
#define LENGTH      200  //  sentence length

int SectionNo[PHASE+1],                             // Number of Sections in PhaseID
    ScenarioNo[PHASE+1][SECTION+1],                 // Number of Scenarios in SectionID
    SectionTransition[PHASE+1][SECTION+1],          // Number of transition times to SectionID
    PhaseTime[PHASE+1],                             // Staying time in PhaseID
    SectionTime[PHASE+1][SECTION+1];                // Staying time in SectionID
    
   
typedef struct {
    int No;                                 //  Number of Sentences in ScenarioID
    int Transition[SCENARIO+1];             //  Number of transition times to ScenarioID
    int Time;                               //  Staying time in ScenarioID
    int ID[SENTENCE+1];                     //  Robot ID for Utterance
    int UtteranceID[SENTENCE+1];            //  Utterance ID for Robots
    int UtteranceTimes[SENTENCE+1];         //  Selection Times of Sentences
    char Name[LENGTH+1];                    //  Name (title) of scenario
    char Sentence[SENTENCE+1][LENGTH+1];    //  Sentences + data
    char SentenceJP[SENTENCE+1][LENGTH+1];  //  Sentences
//    char Utterance[SENTENCE+1][LENGTH+1];   //  Utterance
    int State;                              //  state
} Scenario_st;                              //  Scenario information

Scenario_st     ScenarioInfo[PHASE+1][SECTION+1][SCENARIO+1];


// - -  Human Physical Model - - - - -

double  HumanBodyData[HumanPartsNo+5][15]={        // positions of people parts,  width, left-right, length
    {-0.045, 0.045, -0.1, -0.02,  0, 0.25},      //  0: right lower leg
    {-0.055, 0.055, -0.105, -0.0105, 0, 0.25},   //  1: right upper leg
    {-0.055, 0.055, 0.0105, 0.105, 0, 0.25},        //  2: left upper leg
    {-0.045, 0.045, 0.1, 0.02,  0, 0.25},           //  3: left lower leg
    {-0.03, 0.03,-0.1, 0.1, 0, 0.25},               //  4: main lower body
    {-0.04, 0.04, -0.11, 0.11, 0, 0.15},           //  5: main upper body
    {-0.06, 0.06, -0.06, 0.06, 0, 0.1},           //  6: head
    { -0.04, 0.04, -0.18, -0.11, 0, 0.1},          //  7: right upper arm
    {-0.04, 0.04, -0.18, -0.11, 0, 0.15},          //  8: right lower arm
    {-0.04, 0.04, 0.11, 0.18, 0, 0.1},             //  9: left upper arm
    { -0.04, 0.04,0.11, 0.18, 0, 0.15}             //  10 left lower arm
},
HumanMoveData[PeopleModeNo+5][HumanPartsNo+5]={        // angular velocity for posture of people parts
    {0.015,-0.04,0.04,-0.015,0,0,0,0.04,0.03,-0.04,-0.03},    // 0   walk
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}        //  0  Stoping / staninging
};


double  HumanFurniture[PEOPLE+1][FURNITURE+1],       // distance to furniture
        HumanDist[PEOPLE+1][HumanSensorNo+1],         //  Sensing Distance
        HumanRange=1.5;     //  Sensing Range (Distance)
dReal   angleRange=PI*0.1;  //  Resolution of LRF

#define RULES   10       // Fuzzy Rules
#define MEMFS   10       // Membership Functions
#define CONTL    5       // Membership Functions

int memf=9,        // Number of Membership Functions  ( menf = HumanSensorNo)
    ruls=8;        // Number of Rules
    
double  FuzzyIf[RULES+1][MEMFS+1]={    //   Fuzzy Rules if parts (premise)
            //0,1,2,3,4,5,6
            { 1,1,1,1,1,1,1,0,0 },    //  0   Collision Avoidance using
            { 1,1,1,1,1,1,0,0,1 },    //  1  Ultrasonic Sensor
            { 1,1,1,1,1,0,0,1,1 },    //  2
            { 1,1,1,1,0,0,1,1,1 },    //  3
            { 1,1,1,0,0,1,1,1,1 },    //  4
            { 1,1,0,0,1,1,1,1,1 },     //  5
            { 1,0,0,1,1,1,1,1,1 },    //  6
            { 0,0,1,1,1,1,1,1,1 }     //  7
        },
        FuzzyThen[RULES+1]={    // Fuzzy Rules then parts (conclusions)
            0.05, 0.1, 0.15, 0.2, -0.2, -0.15, -0.1, -0.05
        },
        FuzzyOut;            // Outputs


int HumanPostureNo=2,   //  the initial number of human target postures (0:Walking, 1:Stopping)
    HumanPostureLen[HumanPostures+1];   //  the length of human target postures

dReal  HumanPostureData[HumanPostures+1][HumanPostureLength+1][HumanJointsNo+1];   //  human target posture data
char    HumanPostureName[HumanPostures+1][50];   //  human posture name


