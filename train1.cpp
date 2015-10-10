//
//  train1.cpp
//  Train
//
//  Created by shouyanguo on 10/8/15.
//  Copyright (c) 2015 郭首彦. All rights reserved.
//

#include "train1.h"
#include <vector>
#include "dtw.h"
#include <math.h>
#include <float.h>
#define SEGMENT (5)
using namespace std;

//1d is num of sample of one digit, 2d is num of mfcc vector,3d is 39
vector<vector<float> > mean(SEGMENT);
vector<vector<float> > vari(SEGMENT);
vector<vector<float> > tran(SEGMENT);
int segStateNum[SEGMENT];

void calMean(vector<vector<vector<float> > > &sample, vector<vector<int> > boundary)
{
    for(int i=0;i<sample.size();i++)
    {
        for(int j=0;j<SEGMENT;j++)
        {
            for(int k=boundary[i][j];k<boundary[i][j+1];k++)
            {
                if(k<boundary[i][j+1])
                {
                    for(int l=0;l<39;l++)
                    {
                        if(i==0 && k==0)
                            mean[j][l]=sample[i][k][l]/segStateNum[j];
                        else
                            mean[j][l]+=sample[i][k][l]/segStateNum[j];
                    }
                }
            }
        }
    }
}

void calVari(vector<vector<vector<float> > > &sample, vector<vector<int> > boundary)
{
    float temp;
    for(int i=0;i<sample.size();i++)
    {
        for(int j=0;j<SEGMENT;j++)
        {
            for(int k=boundary[i][j];k<boundary[i][j+1];k++)
            {
                if(k<boundary[i][j+1])
                {
                    for(int l=0;l<39;l++)
                    {
                        temp=sample[i][k][l]-mean[j][l];
                        if(i==0 && k==0)
                            vari[j][l]=temp*temp/segStateNum[j];
                        else
                            vari[j][l]+=temp*temp/segStateNum[j];
                    }
                }
            }
        }
    }
}

void calTran(vector<vector<int> > boundary)
{
    for(int i=0;i<tran.size();i++)
    {
        tran[i][0]=-log((float)(segStateNum[i]-boundary.size())/(float)segStateNum[i]);
    }
    int sum;
    for(int i=0;i<SEGMENT;i++)
    {
        sum=0;
        for(int j=0;j<boundary.size();j++)
        {
            if(boundary[j][i]==boundary[j][i+1])
                sum++;
        }
        tran[i][1]=-log((float)(boundary.size()-sum)/(float)segStateNum[i]);
        if(sum==0)
            tran[i][2]=FLT_MAX;
        else
            tran[i][2]=-log((float)sum/(float)segStateNum[i]);
    }
}

float calBoundary(vector<vector<int> > boundary,vector<vector<vector<float> > > sample,vector<float> errorRate)
{
    float errorSum=0;
    vector<vector<float> >  templet=makeTemplet(mean);
    for(int i=0;i<sample.size();i++)
    {
        vector<vector<float> > word= makeWord(sample[i]);
        vector<vector<float> > trellis=makeTrellis(templet, sample[i].size());
        float score=DP(templet,word,trellis,vari,boundary[i],tran);
        
        errorSum+=fabs(score-errorRate[i]);
        errorRate[i]=score;
    }
    return errorSum;
}

void calSegStateNum(vector<vector<int> > boundary)
{
    for(int i=0;i<boundary.size();i++)
    {
        for(int j=0;j<boundary[i].size();j++)
        {
            if(i==0)
                segStateNum[j]=boundary[i][j+1]-boundary[i][j];
            else
                segStateNum[j]+=boundary[i][j+1]-boundary[i][j];
        }
    }
}

void train1(vector<vector<vector<float> > > &sample)
{
    //initial boundary
    //1d is num of sample of one digit, 2d is boundary for one digit
    //boundary[0][0] = start point of first segment of first sample
    //boundary[][i]==boundary[][i+1] means there is no segment i
    vector<float> errorRate(sample.size());
    vector<vector<int> > boundary(sample.size());
    
    for (int i=0; i<sample.size(); i++) {
        int b1=(int)sample[i].size()/SEGMENT;
        vector<int> b2(SEGMENT+1);
        for (int j=0; j<SEGMENT; j++) {
            b2[j]=b1*j;
        }
        b2[SEGMENT]=(int)sample[i].size()-1;
        boundary[i]=b2;
        errorRate[i]=0;
    }
    
    //initial mean,vari,tran
    for(int i=0;i<SEGMENT;i++)
    {
        vector<float> temp1(39);
        mean[i]=temp1;
        vector<float> temp2(39);
        vari[i]=temp2;
        vector<float> temp3(3);
        tran[i]=temp3;

    }
    
    float errorSum=1000;
    
    while (errorSum>20) {
        calSegStateNum(boundary);
        calMean(sample,boundary);
        calVari(sample,boundary);
        calTran(boundary);
        //update boundary
        errorSum=calBoundary(boundary,sample,errorRate);
    }
}