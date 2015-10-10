//
//  main.cpp
//  Train
//
//  Created by shouyanguo on 10/8/15.
//  Copyright (c) 2015 郭首彦. All rights reserved.
//

#include <iostream>
#include <vector>
#include <time.h>
#include <fstream>
#include <sstream>
#include "train1.h"
using namespace std;

int main()
{
    string dir="/Users/guo/Desktop/speech/mfcc/";
    string digit[]={"zero","one","two","three","four","five","six","seven","eight","nine"};
    string num[]={"1.txt","2.txt","3.txt","4.txt","5.txt"};
    string fileName[10][5];
    string temp;
    for(int i=0;i<10;i++) //10 digits
    {
        for(int j=0;j<5;j++) //5 samples
        {
            temp=dir+digit[i]+num[j];
            fileName[i][j]=temp;
        }
    }

    //start training for every digit
    for(int i=0;i<10;i++)
    {
        //read in the mfcc data
        vector<vector<vector<float> > > sample(5);
        for(int j=0;j<5;j++)
        {
            //cout<<i<<endl;
            ifstream infile(fileName[i][j]);
            string line;
            vector<vector<float> > temp1;
            while(getline(infile, line))
            {
                vector<float> temp2;
                istringstream is(line);
                //char *word;
                string word;
                while(is>>word)
                {
                    temp2.push_back(atof(word.data()));
                }
                temp1.push_back(temp2);
            }
            infile.close();
            sample[j]=temp1;
        }
        //train
        train1(sample);
    }

    return 0;
}

