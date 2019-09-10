/*
 * name: Zhang Yuandi
 * loginID: ics517030910230
 */

#include "cachelab.h"
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>

/*The number of hits, misses and evictions*/
int hits = 0, misses = 0, evictions = 0;
/*Basic parameters.*/
int s, E, b, S, B, t;
/*Cache*/
long long int* cache;
/*Manage file, including pointer and information.*/
char* tfile;
FILE* fp;
char line[20];
char operation;
long long int address, set, tag;

/*Function declarition.*/
void getInput(int argc, char **argv);
void openfile();
void makeCache();
void doCache();
void clean();
void parseLine();

/* 
 * main - This is main function.
 */
int main(int argc, char **argv)
{
    getInput(argc, argv);
    openfile(tfile);
    makeCache();
    doCache();
    printSummary(hits, misses, evictions);
    clean();
    return 0;
}

/*
 * getInput - Get the user's input and set basic parameters.
 */
void getInput(int argc, char **argv)
{
    int result;
    while((result = getopt(argc, argv, "s:E:b:t:")) != -1)
    {
        switch(result)
        {
            case 's':
              s = atoi(optarg);
              S = pow(2, s);
              break;
            case 'E':
              E = atoi(optarg);
              break;
            case 'b':
              b = atoi(optarg);
              B = pow(2, b);
              break;
            case 't':
              tfile = optarg;
              break;
            default:
              printf("Invalid input.");
              exit(0);
        }
    }
}

/*
 * openfile - Open the traces.
 */
void openfile()
{
    fp = fopen(tfile, "r");
    if (fp == NULL)
    {
        printf("Invalid File");
        exit(0);
    }
}

/*
 * makeCache - Make a basic cache.
 */
void makeCache()
{
    cache = (long long int*)calloc(S, 2*E*sizeof(long long int));
}

/*
 * parseLine - Read lines in traces and get the operation, address, set index and tag.
 */
void parseLine()
{
    if(line[0] == 'I')
    {
        operation = 'I';
    }
    else
    {
        operation = line[1];
    }
    char a1[10];
    /*Get address.*/
    for(int i=3; line[i]!=','; i++)
    {
        a1[i-3] = line[i];
    }
    char* ptr;
    address = strtoul(a1, &ptr, 16);
    /*Get set index and tag.*/
    set = (address/B)%(S);
    tag = address/(B*S);

}

/*
 * doCache - Manage the operations.
 */
void doCache()
{
    /* Manage every line. */
    while(fgets(line, 20, fp))
    {
        int miss = 1, evict = 1, index = 0;
        parseLine();
        /*Do nothing with I*/
        if(operation == 'I')
        {
            continue;
        }
        /*Index of set.*/
        long long int this_set = set * 2 * E;
        //long long int this_vset = set * 2 * E + 1;
        long long int next_set = (set + 1) * 2 * E;
        //long long int next_vset = (set + 1) * 2 * E + 1;
        for (int i = this_set; i < next_set; i += 2)
        {
            /*hit*/
            if(cache[i] != 0 && cache[i+1] == tag)
            {
                for(int j = this_set; cache[j] != 0 && j < next_set; j+=2)
                {
                    cache[j] += 1;
                }
                cache[i] = 1;
                hits++;
                miss = 0;
                evict = 0;
                break;
            }
        }
        
        /*miss*/
        if(miss)
        {
            misses++;
            for(int i = this_set; i < next_set; i+=2)
            {
                /*Don't need eviction.*/
                if(cache[i] == 0 && cache[i+1] == 0)
                {
                    cache[i+1] = tag;
                    for(int j=this_set; j<=i+1; j+=2)
                    {
                        cache[j]++;
                    }
                    evict = 0;
                    break;
                }
                if(index < cache[i])
                {
                    index = cache[i];
                }
            }
        }

        /*eviction*/
        if(evict)
        {
            evictions++;
            /*Find eviction block.*/
            for (int i = this_set; i < next_set; i += 2)
            {
                if(cache[i] == index)
                {
                    cache[i] = 0;
                    cache[i+1] = tag;
                    break;
                }
            }
            for (int i = this_set; i < next_set; i+=2)
            {
                cache[i]++;
            }
        }

        /*When M, hits will add once*/
        if(operation == 'M')
        {
            hits++;
        }
    }
}

/*
 * clean - Free cache and close file.
 */
void clean()
{
    free(cache);
    fclose(fp);
}