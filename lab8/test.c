#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
/*
 * name: Zhang Yuandi
 * loginID: ics517030910230
 */
int hits = 0, misses = 0, evictions = 0;
int s, E, b;
int S, B, t;
char* tfile;
FILE* fp;
long long int* cache;
char operation;
long long int address;
int size;
char line[20] = " S 50,4";
long long int set, tag;

void getInput(int argc, char **argv);
void openfile();
void makeCache();
void doCache();
void clean();
void parseLine();

int main(int argc, char **argv)
{
    //getInput(argc, argv);//@
    //openfile(tfile);//@
    //makeCache();//@
    //doCache();
    //printSummary(hits, misses, evictions);//@
    //clean();//@
    //char *line = " S 50,4";
    parseLine();
    return 0;
}

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

void openfile()
{
    fp = fopen(tfile, "r");
    if (fp == NULL)
    {
        printf("Invalid File");
        exit(0);
    }
}

void makeCache()
{
    cache = (long long int*)calloc(S, 2*E*sizeof(long long int));
}

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
    char a1[10] = "";
    for(int i=3; line[i]!=','; i++)
    {
        a1[i-3] = line[i];
    }
    char* ptr;
    address = strtoul(a1, &ptr, 16);
    set = (address/32)%(32);
    tag = address/(32*32);
    
}

void doCache()
{
    while(fgets(line, 20, fp))
    {
        int miss = 1, evict = 1, victim = 0;
        parseLine();
        if(operation == 'I')
        {
            continue;
        }
        long long int this_set = set * 2 * E;
        long long int this_vset = set * 2 * E + 1;
        long long int next_set = (set + 1) * 2 * E;
        long long int next_vset = (set + 1) * 2 * E + 1;
        for (int i = this_set; i < next_set; i += 2)
        {
            if(cache[i] == tag && cache[i+1] != 0)
            {
                for(int j = this_vset; cache[j] != 0 && j < next_vset; j+=2)
                {
                    cache[j] += 1;
                }
                cache[i+1] = 1;
            }
            hits++;
            miss = 0;
            evict = 0;
            break;
        }
            
        if(miss)
        {
            misses++;
            for(int i = this_set; i < next_set; i+=2)
            {
                if(cache[i] == 0 && cache[i+1] == 0)
                {
                    cache[i] = tag;
                    for(int j=set*2*E + 1; j<=i+1; j+=2)
                    {
                        cache[j]++;
                    }
                    evict = 0;
                    break;
                }
                if(victim < cache[i+1])
                {
                    victim = cache[i+1];
                }
            }
        }

        if(evict)
        {
            evictions++;
            for (int i = this_vset; i < next_vset; i+=2)
            {
                if(cache[i] == victim)
                {
                    cache[i] = 0;
                    cache[i-1] = tag;
                    break;
                }
            }
            for (int i = this_vset; i < next_vset; i+=2)
            {
                cache[i]++;
            }
        }

        if(operation == 'M')
        {
            hits++;
        }
    }
}

void clean()
{
    free(cache);
    fclose(fp);
}