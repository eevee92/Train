//
//  dtw.cpp
//  speech_test
//
//  Created by shouyanguo on 10/6/15.
//  Copyright (c) 2015 郭首彦. All rights reserved.
//

#include "dtw.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <time.h>
#include <math.h>
#include <float.h>
using namespace std;

vector<vector<float> >  makeTemplet(vector<vector<float> > &mean)
{
    vector<vector<float> > templets(mean.size());
    for(int i=0;i<mean.size();i++)
    {
        templets[i]=mean[i];
        //cout<<"size1="<<temp1.size()<<endl;
    }
    return templets;
}

vector<vector<float> > makeWord(vector<vector<float> > mfcc)
{
    vector<vector<float> > words;
    //add prefix
    vector<float> temp3(39);
    for(int j=0;j<39;j++)
    {
        temp3[j]=0;
    }
    words.push_back(temp3);
    vector<float> word(39);
    for(int i=0;i<mfcc.size();i++)
    {
        for(int j=0;j<39;j++)
        {
            word[j]=mfcc[i][j];
        }
        words.push_back(word);
    }
    return words;
}

void printMatrix(vector<vector<float> > score)
{
    for(int i=(int)score[0].size()-1;i>-1;i--)
    {
        for(int j=0;j<score.size();j++)
        {
            cout<<score[j][i]<<" ";
        }
        cout<<endl;
    }
}

void traceBack(vector<vector<float> > &score, vector<int> &boundary,vector<vector<float> > &tran)
{
    int x=(int)score.size()-1; //word
    int y=(int)score.back().size()-1; //templet
    while(x!=0 && y!=0)
    {
        if(score[x-1][y]+tran[x-1][0]<=score[x][y]) //left
        {
            x=x-1;
        }
        else if(score[x-1][y-1]+tran[x-1][1]<=score[x][y]) //diagonal
        {
            boundary[y]=x;
            x=x-1;
            y=y-1;
        }
        else //sdiagonal
        {
            boundary[y]=x;
            boundary[y-1]=x;
            x=x-1;
            y=y-2;
        }
        
    }
    boundary[0]=0;
}

vector<vector<float> > makeTrellis(vector<vector<float> >  templet, long wordLength)
{
    vector<vector<float> > score(wordLength);
    for(int i=0;i<wordLength;i++)
    {
        score[i].resize(templet.size());
    }
    
    for(int i=0;i<wordLength;i++)
    {
        score[i][0]=0;
        for(int j=1;j<templet.size();j++)
        {
            score[i][j]=FLT_MAX;
        }
    }
    return score;
}

float distance(vector<float> templet, vector<float> word, vector<float> vari)
{
    float sum1=0,sum2=0;
    float temp;
    for (int i=0;i<templet.size(); i++) {
        sum1=sum1+log(2*3.1416*vari[i]);
        temp=word[i]-templet[i];
        sum2=sum2+temp*temp/vari[i];
    }
    return 0.5*sum1+0.5*sum2;
}

float getMin(vector<vector<float> > &trellis,int i,int j,vector<vector<float> > tran)
{
    float left=FLT_MAX,sdiagonal=FLT_MAX,diagonal=FLT_MAX;
    left=trellis[i-1][j]+tran[j][0];
    if(j-1>=0)
        diagonal=trellis[i-1][j-1]+tran[j-1][1];
    if(j-2>=0)
        sdiagonal=trellis[i-1][j-2]+tran[j-2][2];
    
    float min;
    if(left>diagonal)
    {
        min=diagonal;
        
    }
    min=(sdiagonal>min)?min:sdiagonal;
    return min;
}

float DP(vector<vector<float> >  &templet, vector<vector<float> > &words, vector<vector<float> > &trellis, vector<vector<float> > &vari, vector<int> &boundary,vector<vector<float> > tran)
{
    for(int i=1;i<words.size();i++) // i for length of word
    {
        for(int j=0;j<templet.size();j++) //j for length of dict
        {
            float min=getMin(trellis,i,j,tran);
            trellis[j][i]=min+distance(templet[j], words[i], vari[j]);
        }
        
    }
    traceBack(trellis,boundary,tran);
    return trellis[words.size()-1][templet.size()-1];
}
