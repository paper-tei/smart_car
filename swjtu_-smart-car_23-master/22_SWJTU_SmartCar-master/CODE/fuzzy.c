#include "fuzzy.h"
/*模糊PID可以克服普通PID无法及时调整参数的缺点*/
float Fuzzy_P(float E,float EC)
{
    /*输入量P语言值特征点*/
    float EFF[7]={-50,-30,-10,0,10,30,50};
    /*输入量D语言值特征点*/
    float DFF[7]={-25,-15,-5,0,5,15,25};
    /*输出量U语言值特征点(根据赛道类型选择不同的输出值)*/
    float UFF[7]={ 0.6314, 0.9758, 1.3776, 1.8368, 2.3534, 2.9174, 3.6326 };


    int rule[7][7]={
    //    0  1    2  3    4   5   6
        { 6 , 5 , 4 , 3 , 2 , 1 , 0},//0
        { 5 , 4 , 3 , 2 , 1 , 0 , 1},//1
        { 4 , 3 , 2 , 1 , 0 , 1 , 2},//2
        { 3 , 2 , 1 , 0 , 1 , 2 , 3},//3
        { 2 , 1 , 0 , 1 , 2 , 3 , 4},//4
        { 1 , 0 , 1 , 2 , 3 , 4 , 5},//5
        { 0 , 1 , 2 , 3 , 4 , 5 , 6},//6
    };

    float U=0;  /*偏差,偏差微分以及输出值的精确量*/
    float PF[2]={0},DF[2]={0},UF[4]={0};
    /*偏差,偏差微分以及输出值的隶属度*/
    int Pn=0,Dn=0,Un[4]={0};
    float t1=0,t2=0,t3=0,t4=0,temp1=0,temp2=0;
    /*隶属度的确定*/
    /*根据PD的指定语言值获得有效隶属度*/
    if(E>EFF[0] && E<EFF[6])
    {
        if(E<=EFF[1])
        {
            Pn=-2;
            PF[0]=(EFF[1]-E)/(EFF[1]-EFF[0]);
        }
        else if(E<=EFF[2])
        {
            Pn=-1;
            PF[0]=(EFF[2]-E)/(EFF[2]-EFF[1]);
        }
        else if(E<=EFF[3])
        {
            Pn=0;
            PF[0]=(EFF[3]-E)/(EFF[3]-EFF[2]);
        }
        else if(E<=EFF[4])
        {
            Pn=1;
            PF[0]=(EFF[4]-E)/(EFF[4]-EFF[3]);
        }
        else if(E<=EFF[5])
        {
            Pn=2;
            PF[0]=(EFF[5]-E)/(EFF[5]-EFF[4]);
        }
        else if(E<=EFF[6])
        {
            Pn=3;
            PF[0]=(EFF[6]-E)/(EFF[6]-EFF[5]);
        }
    }

    else if(E<=EFF[0])
    {
        Pn=-2;
        PF[0]=1;
    }
    else if(E>=EFF[6])
    {
        Pn=3;
        PF[0]=0;
    }

    PF[1]=1-PF[0];

    //判断D的隶属度
    if(EC>DFF[0]&&EC<DFF[6])
    {
        if(EC<=DFF[1])
        {
            Dn=-2;
            DF[0]=(DFF[1]-EC)/(DFF[1]-DFF[0]);
        }
        else if(EC<=DFF[2])
        {
            Dn=-1;
            DF[0]=(DFF[2]-EC)/(DFF[2]-DFF[1]);
        }
        else if(EC<=DFF[3])
        {
            Dn=0;
            DF[0]=(DFF[3]-EC)/(DFF[3]-DFF[2]);
        }
        else if(EC<=DFF[4])
        {
            Dn=1;
            DF[0]=(DFF[4]-EC)/(DFF[4]-DFF[3]);
        }
        else if(EC<=DFF[5])
        {
            Dn=2;
            DF[0]=(DFF[5]-EC)/(DFF[5]-DFF[4]);
        }
        else if(EC<=DFF[6])
        {
            Dn=3;
            DF[0]=(DFF[6]-EC)/(DFF[6]-DFF[5]);
        }
    }
    //不在给定的区间内
    else if (EC<=DFF[0])
    {
        Dn=-2;
        DF[0]=1;
    }
    else if(EC>=DFF[6])
    {
        Dn=3;
        DF[0]=0;
    }

    DF[1]=1-DF[0];

    /*使用误差范围优化后的规则表rule[7][7]*/
    /*输出值使用13个隶属函数,中心值由UFF[7]指定*/
    /*一般都是四个规则有效*/
    Un[0]=rule[Pn+2][Dn+2];
    Un[1]=rule[Pn+3][Dn+2];
    Un[2]=rule[Pn+2][Dn+3];
    Un[3]=rule[Pn+3][Dn+3];

    if(PF[0]<=DF[0])    //求小
        UF[0]=PF[0];
    else
        UF[0]=DF[0];
    if(PF[1]<=DF[0])
        UF[1]=PF[1];
    else
        UF[1]=DF[0];
    if(PF[0]<=DF[1])
        UF[2]=PF[0];
    else
        UF[2]=DF[1];
    if(PF[1]<=DF[1])
        UF[3]=PF[1];
    else
        UF[3]=DF[1];
    /*同隶属函数输出语言值求大*/
    if(Un[0]==Un[1])
    {
        if(UF[0]>UF[1])
            UF[1]=0;
        else
            UF[0]=0;
    }
    if(Un[0]==Un[2])
    {
        if(UF[0]>UF[2])
            UF[2]=0;
        else
            UF[0]=0;
    }
    if(Un[0]==Un[3])
    {
        if(UF[0]>UF[3])
            UF[3]=0;
        else
            UF[0]=0;
    }
    if(Un[1]==Un[2])
    {
        if(UF[1]>UF[2])
            UF[2]=0;
        else
            UF[1]=0;
    }
    if(Un[1]==Un[3])
    {
        if(UF[1]>UF[3])
            UF[3]=0;
        else
            UF[1]=0;
    }
    if(Un[2]==Un[3])
    {
        if(UF[2]>UF[3])
            UF[3]=0;
        else
            UF[2]=0;
    }
    t1=UF[0]*UFF[Un[0]];
    t2=UF[1]*UFF[Un[1]];
    t3=UF[2]*UFF[Un[2]];
    t4=UF[3]*UFF[Un[3]];
    temp1=t1+t2+t3+t4;
    temp2=UF[0]+UF[1]+UF[2]+UF[3];//模糊量输出
    U=temp1/temp2;
    return U;
}

float Fuzzy_D(float E,float EC)
{
    /*输入量P语言值特征点*/
    float EFF[7]={-50,-30,-10,0,10,30,50};
    /*输入量D语言值特征点*/
    float DFF[7]={-25,-15,-5,0,5,15,25};
    /*输出量U语言值特征点(根据赛道类型选择不同的输出值)*/
    float UFF[7]={0.35,0.55,0.89,1.32,1.67,2.39,3.14};


    int rule[7][7]={
    //    0  1    2  3    4   5   6
        { 6 , 1 , 2 , 3 , 4 , 5 , 6},//0
        { 1 , 2 , 3 , 4 , 5 , 6 , 5},//1
        { 2 , 3 , 4 , 5 , 6 , 5 , 4},//2
        { 3 , 4 , 5 , 6 , 5 , 4 , 3},//3
        { 4 , 5 , 6 , 5 , 4 , 3 , 2},//4
        { 5 , 6 , 5 , 4 , 3 , 2 , 1},//5
        { 6 , 5 , 4 , 3 , 2 , 1 , 0},//6
    };

    float U=0;  /*偏差,偏差微分以及输出值的精确量*/
    float PF[2]={0},DF[2]={0},UF[4]={0};
    /*偏差,偏差微分以及输出值的隶属度*/
    int Pn=0,Dn=0,Un[4]={0};
    float t1=0,t2=0,t3=0,t4=0,temp1=0,temp2=0;
    /*隶属度的确定*/
    /*根据PD的指定语言值获得有效隶属度*/
    if(E>EFF[0] && E<EFF[6])
    {
        if(E<=EFF[1])
        {
            Pn=-2;
            PF[0]=(EFF[1]-E)/(EFF[1]-EFF[0]);
        }
        else if(E<=EFF[2])
        {
            Pn=-1;
            PF[0]=(EFF[2]-E)/(EFF[2]-EFF[1]);
        }
        else if(E<=EFF[3])
        {
            Pn=0;
            PF[0]=(EFF[3]-E)/(EFF[3]-EFF[2]);
        }
        else if(E<=EFF[4])
        {
            Pn=1;
            PF[0]=(EFF[4]-E)/(EFF[4]-EFF[3]);
        }
        else if(E<=EFF[5])
        {
            Pn=2;
            PF[0]=(EFF[5]-E)/(EFF[5]-EFF[4]);
        }
        else if(E<=EFF[6])
        {
            Pn=3;
            PF[0]=(EFF[6]-E)/(EFF[6]-EFF[5]);
        }
    }

    else if(E<=EFF[0])
    {
        Pn=-2;
        PF[0]=1;
    }
    else if(E>=EFF[6])
    {
        Pn=3;
        PF[0]=0;
    }

    PF[1]=1-PF[0];


    //判断D的隶属度
    if(EC>DFF[0]&&EC<DFF[6])
    {
        if(EC<=DFF[1])
        {
            Dn=-2;
            DF[0]=(DFF[1]-EC)/(DFF[1]-DFF[0]);
        }
        else if(EC<=DFF[2])
        {
            Dn=-1;
            DF[0]=(DFF[2]-EC)/(DFF[2]-DFF[1]);
        }
        else if(EC<=DFF[3])
        {
            Dn=0;
            DF[0]=(DFF[3]-EC)/(DFF[3]-DFF[2]);
        }
        else if(EC<=DFF[4])
        {
            Dn=1;
            DF[0]=(DFF[4]-EC)/(DFF[4]-DFF[3]);
        }
        else if(EC<=DFF[5])
        {
            Dn=2;
            DF[0]=(DFF[5]-EC)/(DFF[5]-DFF[4]);
        }
        else if(EC<=DFF[6])
        {
            Dn=3;
            DF[0]=(DFF[6]-EC)/(DFF[6]-DFF[5]);
        }
    }
    //不在给定的区间内
    else if (EC<=DFF[0])
    {
        Dn=-2;
        DF[0]=1;
    }
    else if(EC>=DFF[6])
    {
        Dn=3;
        DF[0]=0;
    }

    DF[1]=1-DF[0];

    /*使用误差范围优化后的规则表rule[7][7]*/
    /*输出值使用13个隶属函数,中心值由UFF[7]指定*/
    /*一般都是四个规则有效*/
    Un[0]=rule[Pn+2][Dn+2];
    Un[1]=rule[Pn+3][Dn+2];
    Un[2]=rule[Pn+2][Dn+3];
    Un[3]=rule[Pn+3][Dn+3];

    if(PF[0]<=DF[0])    //求小
        UF[0]=PF[0];
    else
        UF[0]=DF[0];
    if(PF[1]<=DF[0])
        UF[1]=PF[1];
    else
        UF[1]=DF[0];
    if(PF[0]<=DF[1])
        UF[2]=PF[0];
    else
        UF[2]=DF[1];
    if(PF[1]<=DF[1])
        UF[3]=PF[1];
    else
        UF[3]=DF[1];
    /*同隶属函数输出语言值求大*/
    if(Un[0]==Un[1])
    {
        if(UF[0]>UF[1])
            UF[1]=0;
        else
            UF[0]=0;
    }
    if(Un[0]==Un[2])
    {
        if(UF[0]>UF[2])
            UF[2]=0;
        else
            UF[0]=0;
    }
    if(Un[0]==Un[3])
    {
        if(UF[0]>UF[3])
            UF[3]=0;
        else
            UF[0]=0;
    }
    if(Un[1]==Un[2])
    {
        if(UF[1]>UF[2])
            UF[2]=0;
        else
            UF[1]=0;
    }
    if(Un[1]==Un[3])
    {
        if(UF[1]>UF[3])
            UF[3]=0;
        else
            UF[1]=0;
    }
    if(Un[2]==Un[3])
    {
        if(UF[2]>UF[3])
            UF[3]=0;
        else
            UF[2]=0;
    }
    t1=UF[0]*UFF[Un[0]];
    t2=UF[1]*UFF[Un[1]];
    t3=UF[2]*UFF[Un[2]];
    t4=UF[3]*UFF[Un[3]];
    temp1=t1+t2+t3+t4;
    temp2=UF[0]+UF[1]+UF[2]+UF[3];//模糊量输出
    U=temp1/temp2;
    return U;
}
