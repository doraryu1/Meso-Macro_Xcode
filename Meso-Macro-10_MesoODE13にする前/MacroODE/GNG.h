//  GNG for Macroscopic Infromation Processing

        
#pragma mark -
#pragma mark Files

void writeTdata(int m)   //  write Teaching data for m-th GNG
{
    FILE *fp;
    int i,j,k,
        n=0;

    char tn[30]="data/tdata00-000.txt";
    char mn[20]="01234567890";
    
    if (m>=10){
        k=(int)(m/10)%10;
        tn[10]=mn[k];
    }
    k=m%10;
    tn[11]=mn[k];

    while ((fp=fopen(tn,"r"))!=NULL){       //  additional writing
        fclose(fp);
        n++;
        if (n>=100){
            k=(int)(n/100)%10;
            tn[13]=mn[k];
            n=n%100;
        }
        if (n>=10){
            k=(int)(n/10);
            tn[14]=mn[k];
        }
        k=n%10;
        tn[15]=mn[k];
    }
    
    if ((fp=fopen(tn,"w+"))==0){
        printf("can't create Teaching Data file\n");
        exit(1);
    }
    
    fprintf(fp,"%d\n",GNGinfo[m].dataNo);
    for(i=0;i<GNGinfo[m].dataNo;i++){
        for (j=0; j<GNGinfo[m].inp; j++)
            fprintf(fp,"%f\t",GNGinfo[m].tdata[i][j]);
        fprintf(fp,"\n");
    }
    fclose(fp);
    printf("Finished Teaching Data writing \n");
}


void readTdata(int m)    //  read teaching data
{
    FILE *fp;
    int fin=0,
        i,j,k,
        n=0;

    char tn[30]="data/tdata00-000.txt";
    char mn[20]="01234567890";
    
    if (m>=10){
        k=(int)(m/10)%10;
        tn[10]=mn[k];
    }
    k=m%10;
    tn[11]=mn[k];

    while ((fp=fopen(tn,"r"))!=NULL){       //  read the latest file
        fclose(fp);
        printf("Teaching Data file [%d][%d], %s\n",
               m,n,tn);
        n++;
        if (n>=100){
            k=(int)(n/100)%10;
            tn[13]=mn[k];
            n=n%100;
        }
        if (n>=10){
            k=(int)(n/10);
            tn[14]=mn[k];
        }
        k=n%10;
        tn[15]=mn[k];
    }
    if (n>0){
        n--;
        if (n>=100){
            k=(int)(n/100)%10;
            tn[13]=mn[k];
            n=n%100;
        }
        if (n>=10){
            k=(int)(n/10);
            tn[14]=mn[k];
        }
        k=n%10;
        tn[15]=mn[k];
    }
    else{
        printf("can't open Teaching Data file\n");
    }

    if ((fp=fopen(tn,"r"))==NULL){
        printf("can't open Teaching Data file\n");
    }
    else {
        fscanf(fp,"%d\n", &GNGinfo[m].dataNo);
        for(i=0;i<GNGinfo[m].dataNo;i++)
            for (j=0; j<GNGinfo[m].inp; j++)
                fscanf(fp,"%lf\t", &GNGinfo[m].tdata[i][j]);
        fclose(fp);
        printf("Finished Teaching Data [%d][%d] reading \n",
               m,GNGinfo[m].dataNo);
        GNGinfo[m].state=2;             //  ready to train GNG/GCS
    }
}

void writeEdata(int m, int t)   //  write Teaching data for m-th GNG
{
    FILE *fp;
    int i,j,k,
        n=0;

    char tn[30]="data/edata00-000.txt";
    char mn[20]="01234567890";
    
    if (m>=10){
        k=(int)(m/10)%10;
        tn[10]=mn[k];
    }
    k=m%10;
    tn[11]=mn[k];

    while ((fp=fopen(tn,"r"))!=NULL){       //  additional writing
        fclose(fp);
        n++;
        if (n>=100){
            k=(int)(n/100)%10;
            tn[13]=mn[k];
            n=n%100;
        }
        if (n>=10){
            k=(int)(n/10);
            tn[14]=mn[k];
        }
        k=n%10;
        tn[15]=mn[k];
    }
    
    if ((fp=fopen(tn,"w+"))==0){
        printf("can't create error Data file\n");
        exit(1);
    }
    
    fprintf(fp,"%d\n",GNGinfo[m].type);
    for(i=0;i<maxMSN;i++)
        fprintf(fp,"%d\t", MSNO[i]);
    fprintf(fp,"\n%f\n%f\t%f\n",
            rgauss, rlearn1, rlearn2);
    for(i=0;i<t;i++)
        fprintf(fp,"%d\t%d\t%lf\n",
                i+1, GNGinfo[m].nn[i], GNGinfo[m].er[i]);
    fclose(fp);
    printf("Finished Error Data writing \n");
}


void writeNdata(int b)   //  write node data for b-th buggy
{
    FILE *fp;
    int h,i,j,k,g,t;
    double d[100], v;
    
    char tn1[30]="data/Pdata00.txt";
    char tn2[30]="data/Cdata00.txt";
    char mn[20]="01234567890";
    
    if (b>=10){
        k=(int)(b/10)%10;
        tn1[10]=mn[k];
        tn2[10]=mn[k];
    }
    k=b%10;
    tn1[11]=mn[k];
    tn2[11]=mn[k];

    
    if (BuggyInfo[b].t>100)
        t=100;
    else
        t=BuggyInfo[b].t;

    if ((fp=fopen(tn1,"w+"))==0){
        printf("can't create error Data file\n");
        exit(1);
    }
    for (i=0; i<t; i++){
        g=(int)BuggyInfo[b].Hdata[i][1]-2;
        for (j=0; j<g; j++)
            d[j]=BuggyInfo[b].Hdata[i][j+2];
        for (j=0; j<g-1; j++){   //  Bubble sort of data
            k=j;
            for (h=j+1; h<g; h++){
                if (d[h]>d[k])
                    k=h;
            }
            v=d[j];
            d[j]=d[k];
            d[k]=v;
        }
        fprintf(fp,"%d\t", (int)BuggyInfo[b].Hdata[i][0]);
        fprintf(fp,"%d\t", g);
        for(j=0; j<g; j++)
            fprintf(fp,"%lf\t", d[j]);
        fprintf(fp,"\n");
    }
    fclose(fp);
    
    
    if ((fp=fopen(tn2,"w+"))==0){
        printf("can't create error Data file\n");
        exit(1);
    }
    for (i=0; i<t; i++){
        g=(int)BuggyInfo[b].Hdata[i][1]-2;
        for (j=0; j<g; j++)
            d[j]=BuggyInfo[b].HdataC[i][j+2];
        for (j=0; j<g-1; j++){   //  Bubble sort of data
            k=j;
            for (h=j+1; h<g; h++){
                if (d[h]>d[k])
                    k=h;
            }
            v=d[j];
            d[j]=d[k];
            d[k]=v;
        }

        fprintf(fp,"%d\t", (int)BuggyInfo[b].Hdata[i][0]);
        fprintf(fp,"%d\t", g);
        for(j=0; j<g; j++)
            fprintf(fp,"%lf\t", d[j]);
        fprintf(fp,"\n");
    }
    
    printf("\n--- Buggy Node Data writing ---- \n\n");
}


#pragma mark -
#pragma mark Topological Intelligence

void init_adMat(int m) // init 0-th adjacent Matrix
{
    int d,h,k,i,j,n;
    n=GNGinfo[m].ngn;   //  current No of Neurons
    
    for (i=0; i<n; i++)
        for (j=0; j<n; j++){
            if (GNGinfo[m].nec[i][j]>=1)
                GNGinfo[m].adMat[0][i][j]=1;
            else
                GNGinfo[m].adMat[0][i][j]=0;
        }

    printf("\nEdge Connectivity(age)\n");
    for (i=0; i<n; i++){
        for (j=0; j<n; j++)
                printf("%d",GNGinfo[m].nec[i][j]);
        printf("\n");
    }

    printf("\nAdjacent Matrix\n");
    for (i=0; i<n; i++){
        for (j=0; j<n; j++)
            printf("%d",GNGinfo[m].adMat[0][i][j]);
            printf("\n");
    }
}

void multi_adMat(int m, int L1, int L2, int L3) // L1 * L2 >> L3
{
    int d,h,k,i,j,n;
    n=GNGinfo[m].ngn;   //  current No of Neurons
    
    for (h=0; h<n; h++)
        for (k=0; k<n; k++){
            d=0;
            for (i=0; i<n; i++)
                d+=GNGinfo[m].adMat[L1][i][k]*GNGinfo[m].adMat[L2][h][i];
            GNGinfo[m].adMat[L3][h][k]=d;
        }
    
    printf("\nAdjacent Matrix (%d * %d) -> %d\n",
           L1,L2,L3);
    for (i=0; i<n; i++){
        for (j=0; j<n; j++)
            if (GNGinfo[m].adMat[L3][i][j]==GNGinfo[m].adMat[L3][j][i])
                printf("%2d",GNGinfo[m].adMat[L3][i][j]);
        printf("\n");
    }
}

void sum_adMat(int m, int L3) // L1 to L3
{
    int d,h,k,i,j,n;
    n=GNGinfo[m].ngn;   //  current No of Neurons

    for (i=0; i<n; i++)
        for (j=0; j<n; j++){
            GNGinfo[m].sumMat[0][i][j]=GNGinfo[m].adMat[0][i][j];
            for (h=1; h<L3; h++)
                GNGinfo[m].sumMat[h][i][j]=GNGinfo[m].sumMat[h-1][i][j]+GNGinfo[m].adMat[h][i][j];
        }
    
    for (h=1; h<L3; h++){
        printf("\nSum of Adjacent Matrix till %d\n", L3);
        for (i=0; i<n; i++){
            for (j=0; j<n; j++)
                printf("%2d",GNGinfo[m].sumMat[h][i][j]);
            printf("\n");
        }
    }
}


void potential_adMat(int m, int b, int L3) // Calculate potential movement direction
{
    int i,j;
    double  d,
            u[NN+1];    //  inputs
       
    for (j=0; j<GNGinfo[m].ngn; j++){
        if ((j==BuggyInfo[b].NNode1)||(j==BuggyInfo[b].NNode2))          //  Nearest 1-2
            u[j]=1.0;                       //  target direction
        else
            u[j]=-BuggyInfo[b].HNodes[j];   //  repulsive force from the previous trajectory
    }

    for (i=0; i<GNGinfo[m].ngn; i++){
        d=0;
        for (j=0; j<GNGinfo[m].ngn; j++)
            d+=GNGinfo[m].sumMat[L3][i][j]*u[j];
        if (d>0)
            BuggyInfo[b].Nodes[i]=d;
        else
            BuggyInfo[b].Nodes[i]=0;
    }
    for (i=0; i<GNGinfo[m].ngn; i++)
        BuggyInfo[b].HNodes[i]*=0.9;    //  temporal discount
}


void collision_adMat(int m, int b, int L3) // Predict collision in potential movement direction
{
    int i,j,
        n=0;    //  number of other agents
    double  d,
            v[NN+1],    //  outputs
            u[NN+1];    //  inputs
    
    for (i=0; i<GNGinfo[m].ngn; i++)
        u[i]=0;
        
    for (i=0; i<HumanNo; i++){
        if (HumanInfo[i].Valid==1){
            for (j=0; j<GNGinfo[m].inp; j++)
                nni[j]=HumanInfo[i].pos[j];
            GNG_GID(m);  //  Global Nearest Node Selection
            u[nnbest1]=1;  //  Nearest
            u[nnbest2]=1;  //  2nd Nearest
        }
    }
          
    for (i=0; i<BuggyNo; i++){
        if ((i!=b)&&(BuggyInfo[i].Valid==1)){       //  other buggies
            for (j=0; j<GNGinfo[m].inp; j++)
                nni[j]=BuggyInfo[i].pos[j];
            GNG_GID(m);  //  Global Nearest Node Selection
            u[nnbest1]=1;  //  Nearest
            u[nnbest2]=1;  //  2nd Nearest
        }
    }
  
    for (i=0; i<GNGinfo[m].ngn; i++){
        d=0;
        for (j=0; j<GNGinfo[m].ngn; j++)
            d+=GNGinfo[m].sumMat[L3][i][j]*u[j];
        v[i]=d;     //  calculate the max degree of danger
    }
    
    for (i=0; i<GNGinfo[m].ngn; i++)
        BuggyInfo[b].NodesC[i]=BuggyInfo[b].Nodes[i]-v[i];

}
    
    
void Topological_main(int m, int b) //  m: GNG-Map ID, b: Buggy-ID
{
    int h,j,k,t,
        N1,N2;
            
    for (j=0; j<GNGinfo[m].inp; j++)
        nni[j]=BuggyInfo[b].pos[j];

    GNG_GID(m);  //  Global Nearest Node Selection
    N1=nnbest1;  //  Nearest
    N2=nnbest2;  //  Nearest
    BuggyInfo[b].NNode1=N1;  //  Nearest
    BuggyInfo[b].NNode2=N2;  //  Nearest
    BuggyInfo[b].HNodes[N1]=1.0;
    BuggyInfo[b].HNodes[N2]=1.0;

    potential_adMat(m,b,1);
    collision_adMat(m,b,1);
    
    printf("b[%d], N1:%d, N2:%d\n",
           b,BuggyInfo[b].NNode1, BuggyInfo[b].NNode1);
    
    h=2;
    t=BuggyInfo[b].t%100;
    for (j=0; j<GNGinfo[m].ngn; j++){   //  L3=2
        if ((GNGinfo[m].sumMat[2][N1][j]>0)||(GNGinfo[m].sumMat[2][N2][j]>0)){
            BuggyInfo[b].Hdata[t][h]=BuggyInfo[b].Nodes[j];
            BuggyInfo[b].HdataC[t][h]=BuggyInfo[b].NodesC[j];
            h++;
        }
    }
    BuggyInfo[b].Hdata[t][0]=BuggyInfo[b].t;
    BuggyInfo[b].HdataC[t][0]=BuggyInfo[b].t;
    BuggyInfo[b].Hdata[t][1]=h;
    BuggyInfo[b].HdataC[t][1]=h;
    BuggyInfo[b].t++;
}

#pragma mark -
#pragma mark GNG

void GNG_GID(int m) //  Selection of the Globally nearest neuron ID
{
    int h,k,i,j;
    double d;
    
    for (i=0; i<GNGinfo[m].ngn; i++){     //  calculate distance between i-th neuron and input
        d=0;
        if (GNGinfo[m].nv[i]>0){
            for (j=0; j<GNGinfo[m].inp; j++)
                d+=(GNGinfo[m].neu[i][j]-nni[j])*(GNGinfo[m].neu[i][j]-nni[j]);
            GNGinfo[m].dis[i]=sqrt(d);
        }
        else
            GNGinfo[m].dis[i]=1000000;
    }
    
   if (GNGinfo[m].dis[0]<GNGinfo[m].dis[1]){
        k=0;        //  nearest
        h=1;        //  2nd nearest
    }
    else{
        k=1;
        h=0;
    }
    for (i=2; i<GNGinfo[m].ngn; i++){
        if (GNGinfo[m].dis[i]<GNGinfo[m].dis[k]){
            h=k;
            k=i;
        }
        else if (GNGinfo[m].dis[i]<GNGinfo[m].dis[h])
            h=i;
    }
    nnbest1=k;   // the nearest
    nnbest2=h;   // the 2nd nearest
}

void GNG_Error(int m, int t)    //  t: iteration
{
    int i,j,k;
    double e=0;
    
    for (i=0; i<GNGinfo[m].dataNo; i++){
        for (j=0; j<GNGinfo[m].inp; j++)
            nni[j]=GNGinfo[m].tdata[i][j];
        GNG_GID(m);  //  Global Nearest Node Selection
        k=nnbest1;  //  Nearest
        e+=GNGinfo[m].dis[k];     //
    }
    GNGinfo[m].nn[t]=GNGinfo[m].ngn;
    GNGinfo[m].er[t]=e/(double)GNGinfo[m].dataNo;
}

void GNG_add(int m){ // add a Neuron
    int i,j,k,h,g;
    
    k=0;    //  select the worst neuron
    for (i=1; i<GNGinfo[m].ngn; i++)
        if (GNGinfo[m].nv[i]>GNGinfo[m].nv[k])
            k=i;
    if (GNGinfo[m].nv[k]>1.0){        //  to be updated
        h=0;    //  select the worst neuron connecting with k-th neuron
        while (GNGinfo[m].nec[k][h]==0)
            h++;

        for (j=h+1; j<GNGinfo[m].ngn; j++){
            if ((GNGinfo[m].nec[k][j]>=1) && (GNGinfo[m].nv[j]>GNGinfo[m].nv[h]))
                h=j;
        }
        
        g=0;
        while (GNGinfo[m].nv[g]>0)
            g++;
        if (g==GNGinfo[m].ngn){     //  if current no of nodes
            if (g<GNGinfo[m].maxNeuron)
                GNGinfo[m].ngn++;
            else
                return;
        }
        GNGinfo[m].nv[k]*=0.5;     //  reduce the error of parent neurons
        GNGinfo[m].nv[h]*=0.5;

        printf("Add Neuron[%d], k:%f h:%f from [%d],[%d]\n",
               g,GNGinfo[m].nv[k], GNGinfo[m].nv[h], k,h);
        for (j=0; j<GNGinfo[m].inp; j++)
            GNGinfo[m].neu[g][j]=(GNGinfo[m].neu[h][j]+GNGinfo[m].neu[k][j])*0.5;  // generate new node
        GNGinfo[m].nv[g]=(GNGinfo[m].nv[k]+GNGinfo[m].nv[h])*0.1;
        GNGinfo[m].nec[g][k]=1;        // connect with the new node
        GNGinfo[m].nec[k][g]=1;
        GNGinfo[m].nec[g][h]=1;
        GNGinfo[m].nec[h][g]=1;
        GNGinfo[m].nec[k][h]=0;        //  remove the original connection between k and h
        GNGinfo[m].nec[h][k]=0;

//        if (GNGinfo[m].type==1){        //  GCS
//            for (i=0; i<GNGinfo[m].ngn; i++){
//                if ((i!=h)&&(i!=k)&&(GNGinfo[m].nec[h][i]>=1)&&(GNGinfo[m].nec[k][i]>=1)){
//                    GNGinfo[m].nec[g][i]=1;        // connect with the nearest node
//                    GNGinfo[m].nec[i][g]=1;
//                }
//            }
//        }
    }
}

void GNG_check(int m) // remove the node if it is not connected with others
{
    int i,j,k,h;
    
    k=0;
    while (k==0){
        for (i=0; i<GNGinfo[m].ngn;i++){
            if (GNGinfo[m].nv[i]>0){
                k=0;
                for (j=0; j<GNGinfo[m].ngn;j++)
                    if (GNGinfo[m].nec[i][j]>0)
                    k++;
                if (k==0){
                    GNGinfo[m].nv[i]=-1;
                    for (j=0; j<GNGinfo[m].inp; j++)
                        GNGinfo[m].neu[i][j]=0;
                    for (j=0; j<GNGinfo[m].ngn; j++)
                        GNGinfo[m].nec[j][i]=0;
                    i=GNGinfo[m].ngn;
                }
            }
        }
    }
}
   
void GNG_initAll()  // initialization for GNG
{
    int h,i,j,m;
    
    gngID=0;            //  the current GNG ID
    
    for (m=0; m<gng; m++){
        GNGinfo[m].ngn=0;
        GNGinfo[m].type=2;          //  0: GNG, 1:GCS, 2: MS-BL-GNG-Disntace, 3: MS-BL-GNG
        GNGinfo[m].state=0;
        GNGinfo[m].maxNeuron=maxNeuronNo;   //  Max Number of Neurons
        GNGinfo[m].inp=3;           //  Input Dim.
        GNGinfo[m].dataNo=0;        //  Data
        GNGinfo[m].dataMax=dataMaxNo;    //  Max Data
        readTdata(m);               //  read init teaching data
    }
}

void GNG_init(int m)  // initialization for GNG after teaching data generation
{
    int h,i,j,k;
    

    for (i=0; i<GNGinfo[m].maxNeuron; i++){
        GNGinfo[m].nv[i]=-1;
        for (j=0; j<GNGinfo[m].inp; j++)
            GNGinfo[m].neu[i][j]=0;
        for (j=0; j<GNGinfo[m].maxNeuron; j++)
            GNGinfo[m].nec[i][j]=0;
    }
    
    i=0;
    j=0;
    k=0;
    while ((i==j) && (i==k)){
        i=(int)(GNGinfo[m].dataNo*rnd());
        j=(int)(GNGinfo[m].dataNo*rnd());
        k=(int)(GNGinfo[m].dataNo*rnd());
    }
    
    for (h=0; h<GNGinfo[m].inp; h++)
        GNGinfo[m].neu[0][h]=GNGinfo[m].tdata[i][h];
    printf("Init: GNG-Node[%d][0] (%4.2f, %4.2f, %4.2f)   \n",
            m,GNGinfo[m].neu[0][0],GNGinfo[m].neu[0][1],GNGinfo[m].neu[0][2]);
    for (h=0; h<GNGinfo[m].inp; h++)
        GNGinfo[m].neu[1][h]=GNGinfo[m].tdata[j][h];
    printf("Init: GNG-Node[%d][1] (%4.2f, %4.2f, %4.2f)   \n",
            m,GNGinfo[m].neu[1][0],GNGinfo[m].neu[1][1],GNGinfo[m].neu[1][2]);
    for (h=0; h<GNGinfo[m].inp; h++)
        GNGinfo[m].neu[2][h]=GNGinfo[m].tdata[k][h];
    printf("Init: GNG-Node[%d][2] (%4.2f, %4.2f, %4.2f)   \n",
            m,GNGinfo[m].neu[2][0],GNGinfo[m].neu[2][1],GNGinfo[m].neu[2][2]);

    GNGinfo[m].ngn=3;
    
    GNGinfo[m].nv[0]=1;
    GNGinfo[m].nv[1]=1;
    GNGinfo[m].nv[2]=1;

    GNGinfo[m].nec[0][1]=1;    //  connectivity
    GNGinfo[m].nec[1][0]=1;
    GNGinfo[m].nec[0][2]=1;    //  connectivity
    GNGinfo[m].nec[2][0]=1;
    GNGinfo[m].nec[2][1]=1;    //  connectivity
    GNGinfo[m].nec[1][2]=1;
    
    printf("\nGNG[%d] Initilized (%d,%d,%d)\n\n",m,i,j,k);
    
}

void drawGNG(int m)
{
    int b,h,i,j,k;
    double d;
    dReal   R[12]={1.0,0,0,0, 0,1.0,0,0, 0,0,1.0,0},
            r, Spos[3], pos1[3];  //  Shpere position

    dsSetColorAlpha(0.0, 1.0, 0.0, 0.6);

    for (i=0; i<GNGinfo[m].ngn-1; i++)
        if (GNGinfo[m].nv[i]>0){
            Spos[0]=GNGinfo[m].neu[i][0];
            Spos[1]=GNGinfo[m].neu[i][1];
            Spos[2]=GNGinfo[m].neu[i][2]+0.1;
            for (j=i+1; j<GNGinfo[m].ngn; j++){
                if (GNGinfo[m].nec[i][j]>=1){
                    pos1[0]=GNGinfo[m].neu[j][0];
                    pos1[1]=GNGinfo[m].neu[j][1];
                    pos1[2]=GNGinfo[m].neu[j][2]+0.1;
                    dsDrawLine(Spos,pos1);
                }
            }
        }
        
    for (i=0; i<GNGinfo[m].ngn; i++)
        if (GNGinfo[m].nv[i]>0){
            Spos[0]=GNGinfo[m].neu[i][0];
            Spos[1]=GNGinfo[m].neu[i][1];
            Spos[2]=GNGinfo[m].neu[i][2]+0.1;
            if (SimulationMode==13){
                d=BuggyInfo[0].NodesC[i];
                if (BuggyNo>1){
                    for (b=1; b<BuggyNo; b++)
                        if (BuggyInfo[b].NodesC[i]>d)
                            d=BuggyInfo[b].NodesC[i];
                }
                h=0;
                if (d>0)
                    h=1;
                else if (d<0)
                    h=-1;
                for (b=0; b<BuggyNo; b++){
                    if ((BuggyInfo[b].NNode1==i)||(BuggyInfo[b].NNode2==i))
                        h=2;
                }
                if (h==2){
                    dsSetColorAlpha(1.0, 0.0, 0.0, 0.95);
                    r=0.07;
                }
                else if (h==1){
                    dsSetColorAlpha(0.8, 0.0, 0.0, 0.95);
                    r=0.05;
                }
                else if (h==0){
                    dsSetColorAlpha(0.0, 0.0, 1.0, 0.8);
                    r=0.03;
                }
                else {
                    dsSetColorAlpha(1.0, 1.0, 0.0, 0.95);
                    r=0.05;
                }
            }
            else {
                dsSetColorAlpha(0.0, 0.0, 1.0, 0.8);
                r=0.03;
            }
            dsDrawSphere(Spos, R, r);
        }
}


void GNG_learning(int m) //  mini-batch short-term Learning
{
    int it=500,     //  local learninng iterations to add a node to the network
        n,s,
        g,h,k,i,j,t;
    
    rlearn1=0.05;
    rlearn2=0.01;
    rgauss=1.0;
    
    for (t=1; t<=it; t++){
        gngl++;
        s=(int)(GNGinfo[m].dataNo*rnd());  //  random sampling for learning
        for (j=0; j<GNGinfo[m].inp; j++)
            nni[j]=GNGinfo[m].tdata[s][j];

        GNG_GID(m);  //  Global Nearest Node Selection
        k=nnbest1;  //  Nearest
        h=nnbest2;  //  Second Nearest

//        printf("best:%d, 2nd-best:%d\n",nnbest1,nnbest2);
        GNGinfo[m].nv[k]+=GNGinfo[m].dis[k];     // vital value of a node
        for (j=0;j<GNGinfo[m].inp; j++)      //   update of reference vectors
            GNGinfo[m].neu[k][j]+=(nni[j]-GNGinfo[m].neu[k][j])*rlearn1;
        
        GNGinfo[m].nec[k][h]=1;  //  reset to 1 (age)
        GNGinfo[m].nec[h][k]=1;
    
        for (i=0; i<GNGinfo[m].ngn; i++){
            GNGinfo[m].nv[i]*=rdis;        //  temporal discount to evaluate the state
            if (GNGinfo[m].nec[k][i]>0){   //  if the nearest neuron is connected with k-th neuron
                for (j=0; j<GNGinfo[m].inp; j++){
                    GNGinfo[m].neu[i][j]+=(nni[j]-GNGinfo[m].neu[i][j])*rlearn2;
                    
//                                        *exp(-GNGinfo[m].dis[i]/1.0)*rlearn2;       // Distance-based Learning
                }
                GNGinfo[m].nv[i]+=GNGinfo[m].dis[i];     //  *0.5;             //  error update
                GNGinfo[m].nec[k][i]++;    //  ageing
                GNGinfo[m].nec[i][k]=GNGinfo[m].nec[k][i];
            }
        }
        if ((GNGinfo[m].ngn<GNGinfo[m].maxNeuron)&&(gngl%500==0)){   //  add neuron per every it iteration
            GNG_add(m);     // generate and add ngn-th neuron
        }
        if (GNGinfo[m].type==0){                //  GNG: cut the connection
            for (i=0;i<GNGinfo[m].ngn-1;i++)
                for (j=i+1;j<GNGinfo[m].ngn;j++){
                    if (GNGinfo[m].nec[i][j]>cutage){  // cut conneectiong if no-selection for long term
                        GNGinfo[m].nec[i][j]=0;
                        GNGinfo[m].nec[j][i]=0;
                        GNG_check(m);
                    }
                }
        }

        n=0;    //  cutting process
        while (n==0){
            n=1;
            for (i=0;i<GNGinfo[m].ngn;i++){
                if (GNGinfo[m].nv[i]>-0.1){
                    if (GNGinfo[m].nv[i]<0.004){
                        GNGinfo[m].nv[i]=-1;
                        for (j=0; j<GNGinfo[m].ngn; j++){
                            GNGinfo[m].nec[i][j]=0;
                            GNGinfo[m].nec[j][i]=0;
                        }
//                        printf(" Remove rarely selected Neuron[%d]\n", i);
                        n=0;
                    }
                    else {
                        g=0;
                        for (j=0; j<GNGinfo[m].ngn; j++)
                            if (GNGinfo[m].nec[i][j]>0)
                                g++;
                        if (g==0){    // remove if there is no connection with other neurons
                            GNGinfo[m].nv[i]=-1;
//                            printf(" Remove No-connected: Neuron[%d]\n", i);
                            n=0;
                        }
                    }
                }
            }
        }
//        ltimes++;
//        if (ltimes%100==0){
//            GNG_Error(m, htimes);
//            htimes++;
//            if (htimes==2000){
//                writeEdata(m,htimes);
//                exit(1);
//            }
//        }

    }
}

#pragma mark -
#pragma mark MS-BL-GNG

void GNG_MS_Init(int m)
{
    int i,j,k;
    for (i=0; i<GNGinfo[m].dataNo; i++)
        MSID[i]=i;               //  shuffled data ID
        
    for (i=0; i<GNGinfo[m].dataNo; i++){
        j=(int)(rnd()*GNGinfo[m].dataNo);
        k=MSID[j];
        MSID[j]=i;
        MSID[i]=k;
    }
    MSL=0;
    MSD=0;
    
    GNGinfo[m].maxNeuron=MSNO[maxMSN-1];   //  Max Number of Neurons: 160 in this example
}

void MS_GNG_learning(int m) //  Multi-scale Batch-Learning
{
    int it,     //  local learninng iterations to add a node to the network
        n,s,
        g,h,k,i,j,t;
    
    rlearn1=1.0;
    rlearn2=0.05;
    rgauss=1.0;

    for(i=0; i<GNGinfo[m].ngn; i++){        //  init for multi-scale mini-batch
        GNGinfo[m].nes[i]=0;      //  selection times
        for (j=0; j<GNGinfo[m].inp; j++)
            GNGinfo[m].update[i][j]=0;      //  sum of Delta for update
        for(j=0; j<GNGinfo[m].ngn; j++)
            GNGinfo[m].necb[i][j]=0;        //  connectibity back
    }
    
    for (t=0; t<GNGinfo[m].dataNo; t+=MSN[MSL]){
        s=MSID[t+MSD];  //  Data ID for learning:  MSD: Data Set ID, MSID: Data ID
        for (j=0; j<GNGinfo[m].inp; j++)
            nni[j]=GNGinfo[m].tdata[s][j];

        GNG_GID(m);  //  Global Nearest Node Selection
        k=nnbest1;  //  Nearest
        h=nnbest2;  //  Second Nearest

//        printf("best:%d, 2nd-best:%d\n",nnbest1,nnbest2);
        GNGinfo[m].nes[k]++;
        GNGinfo[m].nv[k]+=GNGinfo[m].dis[k];     // vital value of a node
        for (j=0;j<GNGinfo[m].inp; j++)      //   update of reference vectors
            GNGinfo[m].update[k][j]+=(nni[j]-GNGinfo[m].neu[k][j])*rlearn1;
        
        GNGinfo[m].necb[k][h]++;
        GNGinfo[m].necb[h][k]=GNGinfo[m].necb[k][h];

        for (i=0; i<GNGinfo[m].ngn; i++){
            if (GNGinfo[m].nec[k][i]>0){   //  if the nearest neuron is connected with k-th neuron
                GNGinfo[m].nes[i]++;
                for (j=0; j<GNGinfo[m].inp; j++){
                    if (GNGinfo[m].type==3)
                        GNGinfo[m].update[i][j]+=(nni[j]-GNGinfo[m].neu[i][j])*rlearn2;       //
                    else
                        GNGinfo[m].update[i][j]+=(nni[j]-GNGinfo[m].neu[i][j])
                                            *exp(-GNGinfo[m].dis[i]/rgauss)*rlearn2;       // Distance-based Learning
                }
                GNGinfo[m].nv[i]+=GNGinfo[m].dis[i]*exp(-GNGinfo[m].dis[i]/1.0)*rlearn2;;             //  error update
            }
        }
//        ltimes++;
//        if (ltimes%100==0){
//            GNG_Error(m, htimes);
//            htimes++;
//            if (htimes==2000){
//                writeEdata(m,htimes);
//                exit(1);
//            }
//        }
            
    }
      
    for(i=0; i<GNGinfo[m].ngn; i++){        //   multi-scale mini-batch update
        if (GNGinfo[m].nes[i]>0)    //  selected once at least
            for (j=0; j<GNGinfo[m].inp; j++)
                GNGinfo[m].neu[i][j]+=GNGinfo[m].update[i][j]/(double)GNGinfo[m].nes[i];      //  sum of Delta for update
        else {
            GNGinfo[m].nv[i]=-1;
            for (j=0; j<GNGinfo[m].inp; j++)
                GNGinfo[m].neu[i][j]=0;
            for (j=0; j<GNGinfo[m].maxNeuron; j++){
                GNGinfo[m].nec[i][j]=0;
                GNGinfo[m].nec[j][i]=0;
            }
        }
        for(j=0; j<GNGinfo[m].ngn; j++)
            GNGinfo[m].nec[i][j]=GNGinfo[m].necb[i][j];        //  connectibity update
    }

    if (GNGinfo[m].ngn<GNGinfo[m].maxNeuron)   //  add neuron per every it iteration
        GNG_add(m);
    
}


#pragma mark -
#pragma mark GNG Main

int gngt=0;

void GNG_main(int m) //  mini-batch short-term Learning
{
    int i,j;
    
    if (GNGinfo[m].state==2){
//        for (i=0; i<GNGinfo[m].dataNo; i++)
//            printf("D[%d] (%4.2f, %4.2f, %4.2f)   ",
//                   i,GNGinfo[m].tdata[i][0],GNGinfo[m].tdata[i][1],GNGinfo[m].tdata[i][2]);
        
        if ((GNGinfo[m].type==0)||(GNGinfo[m].type==1))
            GNGinfo[m].state=3;     //  learning started (standard GNG)
        else if (GNGinfo[m].type>=2)
            GNGinfo[m].state=4;     //  learning started (standard GNG)
        
        GNG_init(m);
        if (GNGinfo[m].type>=2)
            GNG_MS_Init(m);     //  init multi-scale BL-GNG
    }
    else if (GNGinfo[m].state==3){      //  standard GNG
//        printf("GNG[%d]: Node:%d\n", m, GNGinfo[m].ngn);
        if (gngt%2==0)
            GNG_learning(m);
        if (GNGShow==1)
            drawGNG(m);
        gngt++;     //  training times
    }
    else if (GNGinfo[m].state==4){      //  Multi-scale Batch Learning GNG
        MS_GNG_learning(m);
        if (GNGShow==1)
            drawGNG(m);
        MSD++;
        if ((MSL<maxMSN)&&(GNGinfo[m].ngn>MSNO[MSL])){
            MSL++;
            MSD=0;
            printf("\nMS Level:%d \n\n",MSL);
        }
        else if (MSD>=MSN[MSL])
            MSD=0;
    
        if (GNGinfo[m].ngn==maxNeuronNo){
            init_adMat(m);
            multi_adMat(m, 0, 0, 1);
            multi_adMat(m, 0, 1, 2);
            sum_adMat(m, 3);
            SimulationMode=13;      //  move to topological intelligence
        }
    }
}

