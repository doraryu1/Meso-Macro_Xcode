// ISS Robot Theater Management Program


int SimulationMode=12,   //  0: full Server Mode, 1: Virtual Robot Mode,
                        //  5: Virtual Human Mode, 10: full Simulation Mode
                        //  11: full simulation with Utterance TCP server
                        //  12: GNG, 13: Topological Intelligence

    finRT=1,    //  0:started, 1:finished,  finisah a robot theater ?
    fin=0;      //  finish all ?

int GNGShow=1,  //  0: No show, 1: Show GNG
    TraShow=1,  //  0: No show, 1: Show all, 2:Show short-term - - - - - - -
    TextShow=0,  //  0: No show, 1: Show all
    WordShow=1, //  0: No show, 1: Show
    issID=98,           //  initial ISS server ID      (the value may change)
    issMacroID=99;        //  ID Macro-simulation Server


#pragma mark -
#pragma mark ODE

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
#pragma mark Global Parameters

//  Multi-theater - - - - - - - - - - - - - - - - - - - - -

#define THEATER 10
int     TheaterID=-1,       //  Current Theater ID
        TheaterLanguage=6,   //  Theater Language (EN:5, JP:6)
        TheaterNo;          //  total theater No

typedef struct {
    int Valid;      //  validity
    char TheaterName[50];           //  Theater Name
    char TheaterFileName[50];       // Theater File name
    char TransitionFileName[50];    //  Transition File name
    int State;          //  state
} Theater_st;        //  Theater File information

Theater_st   TheaterInfo[THEATER+1];


//   Sensors and Furniture - - - - - - - - - - - - - - -- - - - - - - - - - - - - - -

#define ITEMS  999     //  The number of Device ID

int SensorActivated=999,//  if the sensing is done
    SensorID,           //  SensorID
    EnvSensorReID[ITEMS+1],    //  SensorID in System
    EnvSensorNoAll,     //  Number of all sensors (including voice recognitions)
    EnvSensorNo;        //  Number of Physical Sensors


//   Furniture - - - - - - - - - - - - - - -- - - - - - - - - -
#define ObNo       10    //  Number of walls

dReal   ObPos[ObNo][10]={
            {-6.0, -6.0, 6.0, 6.0, 0, 6.5},     //  x1, y1, x2, y2, z1, z2
        },
        ObPosx[ObNo][3],
        ObSize[ObNo][3];

MyObject    EnvFurniture[ITEMS+1],
            EnvObjects[ObNo+1];


// GNG

#define MN      5    //  Number of GNG
#define NN    500    //  Maximal nunber of Neurons
#define IN     10    //  Input Dim
#define TD  10000    //  Teaching Data
#define HT  10000    //  history

typedef struct {
    int state;      //  0: not used, 1: start (still learning data collection),
                    //  2: start learning (init), 3: learning
    int type;       //  0: GNG, 1: GCS + Age
    int ngn;                    //  number of neurons of GNG
    int maxNeuron;              //  Max Number of Neurons
    int inp;                   //  Input Dim.
    int necSum[NN+1];           //  the number of connected neurons
    int necID[NN+1][NN+1];      //  ID of connected neurons, 0: Number, 1- ID
    int nec[NN+1][NN+1];        // age and connectibity 0: not-connected, 1 or larger: age
    double neu[NN+1][IN+1];       // reference vector
    double nv[NN+1];            // active neuron ( -1: inactive, 1: activeated (>0)
                                // lager > nv_min: weighted sum of error over time)
    
    //  Topological Intelligence
    int adMat[3][NN+1][NN+1];       //  Adjacent Matrix
    int sumMat[3][NN+1][NN+1];       //  Adjacent Matrix
    
    //  Multi-scale Batch Learning GNG
    int necb[NN+1][NN+1];        //  connectibity back
    double update[NN+1][IN+1];   // update for batch learning
    int nes[NN+1];            // selection times
    
    //  Data-set
    int dataMax;                 // Max teaching data Number
    int dataNo;                 //  teaching data Number
    double tdata[TD+1][IN+1];   //  teaching data: (x,y,z), device ID (0-300),
                                //  year (20xx), month (1-12), day(1-31), time(0-86399),
    double dis[NN+1];         //   distance between i-th neuron and input
    int    nn[HT+1];            //  neurons no history
    double er[HT+1];            // average error history
    
} GNG_st;        //  GNG information

int maxNeuronNo=320,       //  Max Neurons
    dataMaxNo=6400,        //  Data Max
    MSL=0,                      //  Multi-scale Level
//    MSNO[8]={20,30,40},   //  Max number of nodes in each phase
//      MSNO[8]={80,160,240,300,320},   //  Max number of nodes in each phase
    MSNO[8]={80,120,140,160},   //  Max number of nodes in each phase
//    MSNO[8]={120,180,220,240},   //  Max number of nodes in each phase
//    MSNO[8]={80,160,200,220,240},   //  Max number of nodes in each phase
//    MSNO[8]={40,80,120,140,160},   //  Max number of nodes in each phase
//     MSNO[8]={60,100,130,150,160},   //  Max number of nodes in each phase
    MSID[TD],                   //  shuffled data ID
    MSD,                        //  current data set ID
    maxMSN=4,
//    MSN[8]={4,2,1};           //   Multi-scale Data Sets
    MSN[8]={8,4,2,1};           //   Multi-scale Data Sets
//MSN[8]={16,8,4,2,1};           //   Multi-scale Data Sets


GNG_st  GNGinfo[MN+1];      //  GNG info

double  nni[IN+1],          //  Inputs to GNG
        nv_min=0.1,         //  minimal value for neuron value
        rgauss=1.0,
        rlearn1=0.05,       //  learning rate for the nearest node
        rlearn2=0.01,       //  learning rate for nodes connecting with the nearest node
        rdis=0.995;        //  temporal discount rate for error of nodes
        
int     gng=1,              //  the number of GNG
        gngID=0,            //  the current GNG ID
        cutage=50,          //  cut age for GNG
        nnbest1,            //  nearest neuron
        nnbest2;            //  second nearest neuron
  
int gngl=0,     //  for node addition frequency
    ltimes=0,   //  for data history
    htimes=0;   //  for data history

double  nniweight[5]={ 1.0, 1.0, 1.0, 1.0, 1.0};   //  importance to inputs
        

// Furniture

int EnvFurnitureNo=10;     //  Number of Furniture

typedef struct {
    int ID;         //  ID for communication
    dReal Pos[3];    //  position
    dReal TarPos[3];    //  position
    dReal Sizes[3]; //  size
    int Valid;      //  validity
    int Type;       //  Furniture Shape type
    int Color;       //  Color
    char Name[100];  //  name
    int HumanPosture;   //  Human posture ID when a person arrives here
    int State;          //  state
} EnvObject_st;        //  Furniture information

EnvObject_st    EnvFurData[ITEMS+1],
                EnvSensorData[ITEMS+1];

#define DEVICE  100      //  max Number of DEVICES in Global
#define LocalDEVICE 20   //  max Number of DEVICES in Local

int DeviceReID[DEVICE+1],   //  Device ID in this PC
    DeviceReType[DEVICE+1],   //  Device Type in this PC
    BuggyNo=0,      //  Number of Buggies , from file
    RobotNo=0,      //  Number of Robots , from file
    HumanNo=15;     //  max Number of People , predefined

typedef struct {
    int ID;         //  Robot ID for communication
    dReal pos[3];    //  position
    dReal tarpos[3];    //  position
    dReal sizes[3];    //  sizes
    double VX;         //  velocity
    double VY;         //  velocity
    int Valid;      //  validity
    int Type;       //  Robot type
    char Name[100];  //  name
    
    int NNode1;       //  the nearest Node in GNG
    int NNode2;       //  the nearest Node in GNG
    double Nodes[NN+1];  //   Node Potential State in GNG
    double NodesC[NN+1];  //   Node Collision State in GNG
    double HNodes[NN+1];  //   Histroy of selected Nodes in GNG
    int t;          //  time in Simulation Mode 13
    double Hdata[100][100];   //  Potential history data
    double HdataC[100][100];   //  Collision history data

    int Interval;   //  Interval for the next robot theater
    int Language;   //  Language Mode
    int Theater;    //  Theater ID
    int Gesture;    //  Gesture ID
    int Face;       //  Face ID
    int Utterance;  //  Utterance ID
    int Mode;       //  Mode
    int State;          //  state
} Agent_st;        //  Robot information

Agent_st    BuggyInfo[LocalDEVICE],
            RobotInfo[LocalDEVICE],
            HumanInfo[LocalDEVICE];

int HumanInterval=1000;     //  Interval to restart the next theater.

//  Utterance/Voice Recognition - - - - - - - - - - - - - - -

#define VoiceCategory   50      //  Category of Recognized Words
#define VoiceWord      200     //  The number of Words in a category    (Important: to be updated)
#define W_NO          3000      //  the number of words
#define S_NO          1000      //  the number of sentences
#define W_LENGTH        50      //  word length
#define S_LENGTH       300      //  sentence length
#define P_LENGTH        30      //  Phrase length (the number of words)

int VoiceCategoryNo,             //  the number of categories
    VoiceAllWordNo,                 //  the number of utterances
    VoiceUtteranceNo,            //  the number of utterances
    VoiceWordNo[VoiceCategory+1];    //  the number of words in each category
char    VoiceFileName[VoiceCategory+1][W_LENGTH+1],
        VoiceCategoryName[VoiceCategory+1][W_LENGTH+1],               //  Category Names
        VoiceWordName[VoiceCategory+1][VoiceWord+1][W_LENGTH+1];   //   Word Names

int UtteranceID,            // Current UtteranceID
    PreviousUtteranceID,    // Previous UtteranceID
    VoiceCategoryID,     // Category ID as a result of Voice Recognition
    VoiceWordID;         // Word ID as a result of VOice Recognition
char    VoiceInputWord[W_LENGTH+1];        //  Input Words for Word-check

typedef struct {
    int Times;              // selection time
    char Sentence[W_LENGTH+1];     // Sentence
    char SentenceJP[W_LENGTH+1];     // Sentence in JP
//    int To[S_NO+1];         // Next utterance (utterance transition)
//    int From[S_NO+1];       // previous utterance
} Word_st;        // Word Information
Word_st    VoiceAllWord[W_NO+1];

typedef struct {
    int WordNo;                 //  the number of Words in a sentence
    int Structure[P_LENGTH+1];  //  Phase Structure (Sequence of Words)
    int Times;                  //  selection time
    char Sentence[S_LENGTH+1];  //  Sentence
    char SentenceJP[W_LENGTH+1];     // Sentence in JP
    int State;                  //  state
    int StateJP;                //  state
//    int To[S_NO+1];           //  Next utterance (utterance transition)
//    int From[S_NO+1];         //  previous utterance
} Utterance_st;        // Utterance Information
Utterance_st    VoiceInput,                     //  Input Sentence for Word-check,
                VoicePreviousUtternace,         //  Previous utterance from the robot
                VoiceGeneralUtternace[10][5],   //  General Utterances for natural communication
                VoicePhrase[VoiceCategory+1][VoiceWord+1],  //  for voice recognition and understanding
                VoiceUtternace[S_NO+1];         //  All sentence

//  Utterance -----------------------------------------------

#define PHASE         4  //   the number of phases: introduction, development, turn and conclusion
#define SECTION      10  //   the number of sections: story transition
#define SCENARIO     10  //   the number of scenarios: scenarios dependent on user's interaction and communication
#define SENTENCE     20  //   the number of sentences
#define LENGTH      200  //  sentence length

int PhaseID=-10,        //  -10 ~ -4: Waiting interval
                        //  -3: Ready to start (Theater Selection)
                        //  -2: Individual Theater: Standby
                        //  -1: Initialize the theater
                        //  -0: Phase 0 starts
    PhaseInitID=-10,    //  Current Phase
    SectionID=-1,   //  Current Section
    ScenarioID=-1,  //  Selected Scenario
    SentenceID=-1,     //  Selected Sentence
    HumanID,        //  selected Human
    RobotID,        //  Selected Robot
    SectionNo[PHASE+1],                             // Number of Sections in PhaseID
    ScenarioNo[PHASE+1][SECTION+1],                 // Number of Scenarios in SectionID
    SectionTransition[PHASE+1][SECTION+1],          // Number of transition times to SectionID
    PhaseTime[PHASE+1],                             // Staying time in PhaseID
    SectionTime[PHASE+1][SECTION+1];                // Staying time in SectionID
    
typedef struct {
    int No;                                 //  Number of Sentences in ScenarioID
    int Transition[SCENARIO+1];             //  Number of transition times to ScenarioID
    int Time;                               //  Staying time in ScenarioID
    int RobotID[SENTENCE+1];                //  Robot ID for Utterance
    int UtteranceID[SENTENCE+1];            //  Utterance ID for Robots
    int UtteranceTimes[SENTENCE+1];         //  Selection Times of Sentences
    char Name[LENGTH+1];                    //  Name (title) of scenario
    char Sentence[SENTENCE+1][LENGTH+1];    //  Sentences
    char SentenceJP[SENTENCE+1][LENGTH+1];  //  Sentences
//    char Utterance[SENTENCE+1][LENGTH+1];   //  Utterance
    int State;                              //  state
} Scenario_st;                              //  Scenario information

Scenario_st     ScenarioInfo[PHASE+1][SECTION+1][SCENARIO+1];

// state transition rule - - - - - - - - - - - - - - - - - - - -
#define RULE       10   //  the number of Transition Rules

typedef struct {
    int No;             // the number of Rules
    int Sensor[10];     // [if] the input(sensor) ID
    int State[10];      // [if] the condition (state value)
    int Phase;          // [then] the Phase to transit
    int Section;        // [then] the Scenario to transit
    int Scenario;       // [then] the Scenario to transit
    char Rule[LENGTH+11];   // Text: if-then rule
} Transition_st;        // Transition Information

Transition_st   InitTransition[RULE+1],
                Transition[PHASE+1][SECTION+1][SCENARIO+1][RULE+1];
int InitTransitionNo,
    TransitionNo[PHASE+1][SECTION+1][SCENARIO+1];   //  the number of transition rules
char RuleName[PHASE+1][SECTION+1][SCENARIO+1][LENGTH+1];    //  Name or title or rules



#pragma mark -
#pragma mark Basic Function

#define PI  3.141592

double rnd() /* uniform random number  */
{
    return((double)(rand()%RAND_MAX)/(double)RAND_MAX);
}


double rndn()            /*   normal random number */
{
    return (rnd()+rnd()+rnd()+rnd()+rnd()+rnd()+
            rnd()+rnd()+rnd()+rnd()+rnd()+rnd()-6.0);
}

double sigm(double x)   //  sigmoidal function
{
    return(1.0/(1.0+exp(-x)));
}



#pragma mark -
#pragma mark UDP- TCP/IP

#define BUFSIZE         8192
#define ISS_UDP_PORT    9000    //  Port ID for UDP/IP ISS Database communication
#define Con_UDP_PORT    9100    //  Port ID for UDP/IP Theater Contents communication
#define Utter_UDP_PORT  9200    //  Port ID for UDP/IP Theater Contents communication
#define ISS_TCP_PORT    9866    //  ISS Database Server
#define Con_TCP_PORT    9865    //  ISS Robot Theater Contents Server (this PC)
#define Macro_TCP_PORT    9864    //  ISS Macro Server (this PC)
#define Utter_TCP_PORT    9867    //  ISS Utterance Server

char    ISS_ServerIP[30]="192.168.13.100",
        Con_ServerIP[30]="192.168.13.101",
        Utter_ServerIP[30]="192.168.13.102",
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


int Utter_UDP_receive()    //  receive server (host) IP through UDP/IP Utterance
{
    int i=0,
    sock;
    struct sockaddr_in addr;
    
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(Utter_UDP_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_len = sizeof(addr);
    
    bind(sock, (struct sockaddr *)&addr, sizeof(addr));
    memset(Utter_ServerIP, 0, sizeof(Utter_ServerIP));
    recv(sock, Utter_ServerIP, sizeof(Utter_ServerIP), 0);
    
    printf("Utterance Server IP %s\n", Utter_ServerIP);
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



#pragma mark -
#pragma mark Time

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


// Functions - - - - - - - - - - - - - - - - - - - - - -

void ReadTheater();
void ReadSensor();
void ReadFurniture();
void ReadRobot();

void WriteVoiceWord();
void WriteVoiceUtterance(); // write utterances
void VoiceWordCheck();   //  Voice Word Checking and adding
void VoiceInputExtraction();      //  extract words from a sentence
void VoicePhraseStructure(int p, int q);      //  extract words from a phrase
void VoiceSentenceStructure(int p);      //  extract words from a phrase
void VoiceSentenceCheck();      //  extract words from a sentence
void ReadVoice();    //  read Utterance Information - - - - -

void UtteranceExtraction(int i, int j, int k, int h);   //  Utterance extraction from scenario data
void ReadSentence(int t);   //  read sentences - - - - - - - -
void ReadRule(int t);    //  read State Transition Rule - - - - - - - -

void drawNumber(int n, int x, int y);
void drawScreen();

void init_transition();
void init_Human_data(int i);
void Init_Human();     //  Init Human Movement
void init_Theater();    //  Init Theater 

void SensorHuman();      //  Human Movement Simulations
void SensorWatch();      //  Time
void SensorVoice(int p, int q);     //  Voice Recognition
void SensorVoiceInput(int n);   //  Virtual Voice Recognition

void scenario_transition();
void scenario_transition2();        //  scenario transition to confirm ihe transition

void robot_utterance();     //  n: Robot ID
void robot_utterance2();     //  repeated

void* RobotTheater( void* args ); // Robot Theater
void initRobotTheater();

void Topological_main(int m, int b); //  m: GNG-ID, b: Buggy-ID
void GNG_GID(int m); //  Selection of the Globally nearest neuron ID
void drawGNG(int m);

void UtteranceTCP(int r, int u);
