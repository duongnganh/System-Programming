//NAME: DUONG NGUYEN ANH
//pID: p20150003
#include "cachelab.h"
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <stdio.h>
int opt, s, E, b;
FILE * t;

//get_opt for s, E, b, t
void get_opt(int argc, char **argv){
	while(-1 != (opt = getopt(argc, argv, "s:E:b:t:"))){
		switch(opt){
			case 's':
				s = atoi(optarg);
				break;
			case 'E':
				E = atoi(optarg);
				break;
			case 'b':
				b = atoi(optarg);
				break;
			case 't':
				t = fopen(optarg,"r");
				break;
			default:
				printf("wrong argument\n");
				exit(1);
				break;
		}
	}
}
//Structure for a line
typedef struct{
	int valid;
	int tag;
} cache_line;

int main(int argc, char **argv)
{
	//Read from imput to get s, E, b
	get_opt(argc, argv);
	int S = 1 << s;

	//Allocate enough space for sets and initialize the values
	cache_line **temp = malloc(S*sizeof(int *));
	cache_line* **Q = malloc(S*sizeof(int *));
	
	for (int i = 0; i < S; i++){
		temp[i] = malloc(E*sizeof(cache_line));
		Q[i] = malloc(E*sizeof(cache_line));
		for (int j = 0; j < E; j++){
			temp[i][j].valid = 0;
			temp[i][j].tag = 0;
			Q[i][j] = &temp[i][j];
		}
	}

	int miss = 0, hit = 0, eviction = 0;

	unsigned si, tag, filter = ~(~0 << s);
	int if_hit;
	cache_line* temp_cache;

	char identifier;
	unsigned address;
	int size;	
	//Readfile
	while(fscanf(t," %c %x,%d", &identifier, &address, &size)>0){
		printf("%c %x,%d", identifier, address, size);
		if (identifier == 'L' || identifier == 'S' || identifier == 'M'){
			if_hit = 0;
			tag = (address >> (s+b));
			si = (address >> b) & filter;

			for (int i = 0; i < E; i++){
				temp_cache = Q[si][i];
				if (temp_cache->valid == 1 && temp_cache->tag == tag){
					if_hit = 1;
					printf(" hit");
					hit++;
					for (int k = i; k < E-1; k++)
						Q[si][k] = Q[si][k+1];
					Q[si][E-1] = temp_cache;
					break;
				}
			}

			if (if_hit == 0){
				printf(" miss");
				miss++;

				temp_cache = Q[si][0];
				if (temp_cache->valid == 1){
					eviction++;
					printf(" eviction");
				}
				temp_cache->tag = tag;
				temp_cache->valid = 1;
				for (int k = 0; k < E-1; k++)
					Q[si][k] = Q[si][k+1];
			
				Q[si][E-1] = temp_cache;
			}

			//Store in M always hits
			if (identifier == 'M'){
				printf(" hit");
				hit++;
			}
			printf("\n");
		}
	}
	fclose(t);

    printSummary(hit, miss, eviction);
    for (int i = 0; i < S; i++){
	    free(Q[i]);
	    free(temp[i]);
    }
    free(Q);
    free(temp);
    return 0;
}
