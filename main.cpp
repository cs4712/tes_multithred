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
#include <map>
#include <time.h>
#include <algorithm> // sort

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

int n = 0;   // random range
int t = 0;   // top t frequent numbers
int occurences = 0;
std::vector <int> res;
std::map<int, int> top;

void *genRanNum(void *arg);
void *showNum(void *arg);
int cmp(const std::pair<int, int> &x, const std::pair<int, int> &y);

void* genRanNum(void *arg){
    
    
    int n = *((int *)arg);

    std::ifstream urandom("/dev/urandom");
    while(1){
        pthread_mutex_lock( &mutex1 );
        
        // if every number occurs once, exit
        if (occurences >= n){
            pthread_mutex_unlock( &mutex1 );
            return 0;
        }
        
        unsigned int ran0 = 1;
        urandom.read((char*)&ran0, sizeof(unsigned int));
        int ran = ran0%(n+1);
        top[ran]++;
        
        pthread_mutex_unlock( &mutex1 );
        usleep(100000);
    }
    
    return 0;
}

int cmp(const std::pair<int, int> &x, const std::pair<int, int> &y){
    return x.second > y.second;
}

void* showNum(void *arg){
    while (1){
        pthread_mutex_lock( &mutex1 );
        
        int t = *((int *)arg);
        std::vector<std::pair<int, int> > sortTop;
        
        for (std::map<int, int>::iterator it = top.begin(); it != top.end(); it++)
            sortTop.push_back(std::make_pair(it->first, it->second));
        // sort by value (times of occurences)
        sort(sortTop.begin(), sortTop.end(), cmp);
        
        int sum = 0;
        occurences = 0;
        
        for (std::vector<std::pair<int, int> >::iterator it = sortTop.begin(); it != sortTop.end(); it++){
            sum++;
            if (it->second == 0)
                continue;
            
            std::cout << it->first << ' ';
            occurences++;
            
            if (sum >= t)
                break;
        }
        std::cout << std::endl;
        
        // if every number occurs once, exit
        if (occurences >= n){
            pthread_mutex_unlock( &mutex1 );
            return 0;
        }
    
        pthread_mutex_unlock( &mutex1 );
        sleep(5);
    }
    return 0;
}
int main(int argc, char **argv) {
    
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
    
    // initial map
    for(int i=0;i<=n;i++){
        top[0] = 0;
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
