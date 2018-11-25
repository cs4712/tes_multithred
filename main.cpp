//
//  main.cpp
//  multithread
//
//  Created by Shi Cheng on 2018/11/24.
//  Copyright © 2018年 Shi Cheng. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <pthread.h>
#include <unistd.h>
#include <string>
#include <time.h>

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

std::vector <int> res;

void *genRanNum(void *arg);
void *showNum(void *arg);

void* genRanNum(void *arg){
    
    
    int n = *((int *)arg);
//    for(int i=0;i<n;i++)
//        std::cout << "genRanNum: " << i << std::endl;
    std::ifstream urandom("/dev/urandom");
    while(1){
        pthread_mutex_lock( &mutex1 );
        unsigned int ran0 = 1;
        urandom.read((char*)&ran0, sizeof(unsigned int));
        int ran = ran0%n;
        res.push_back(ran);
        std::cout << "genRanNum: " << ran << std::endl;
        pthread_mutex_unlock( &mutex1 );
    }
    
    return 0;
}

void* showNum(void *arg){
    pthread_mutex_lock( &mutex1 );
    
    int t = *((int *)arg);
    for(int j=0;j<t;j++)
        std::cout << "showNum: " << j << std::endl;
    
    pthread_mutex_unlock( &mutex1 );
    return 0;
}
int main(int argc, char **argv) {
    
    int n = 0;   // random range
    int t = 0;   // top t frequent numbers
    
    int a;
    std::string nvalue, tvalue;
    while ((a = getopt(argc, argv, "n:t:")) != -1)
        switch (a)
    {
        case 'n':
            nvalue = optarg;
            n = atoi(nvalue.c_str());
            if (n < 0){
                std::cerr << "Error: invalid n, n should be larger than -1" << std::endl;
                return 1;
            }
            break;
        case 't':
            tvalue = optarg;
            t = atoi(tvalue.c_str());
            if (t < 1){
                std::cerr << "Error: invalid t, t should be larger than 0" << std::endl;
                return 1;
            }
            break;
        case '?':
            if (optopt == 'n' || optopt == 't'){
                std::cerr << "Error: option " << (char)optopt << " requires an argument." << std::endl;
            }
            else
                std::cerr << "Error: unknown option: " << (char)optopt << std::endl;
            return 1;
        default:
            return 0;
    }
    
    std::ifstream urandom("/dev/urandom");
    
    if (urandom.fail())
    {
        std::cerr << "Error: cannot open /dev/urandom\n";
        return 1;
    }
    
    pthread_t thread1, thread2;
    int  iret1, iret2;

    
    iret1 = pthread_create( &thread1, NULL, genRanNum, (void *)&n);
    if(iret1)
    {
        fprintf(stderr,"Error - pthread_create() return code: %d\n",iret1);
        return -1;
    }
    
    iret2 = pthread_create( &thread2, NULL, showNum, (void *)&t);
    if(iret2)
    {
        fprintf(stderr,"Error - pthread_create() return code: %d\n",iret2);
        return -1;
    }
    pthread_join( thread1, NULL);
    pthread_join( thread2, NULL);
    
    // close random stream
    urandom.close();
    
    return 0;
}
