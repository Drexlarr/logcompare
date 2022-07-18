#include <stdio.h> 
#include <string.h>
#include "../lib/rabin_karp.h"

#define d 256
 

int* rabin_karp(char* pat, char* txt, int q){
    int M, N; 
    if(txt[strlen(txt) - 1] == '\n') N = strlen(txt) - 1;
    else N = strlen(txt);
    if(pat[strlen(pat) - 1] == '\n') M = strlen(pat) - 1;
    else M = strlen(pat);
    int i, j;
    int p = 0;
    int t = 0;
    int h = 1;
    int ocd = 0;
    static int posfound[100];
 
    for (i = 0; i < M-1; i++)
        h = (h*d)%q;
 
    for (i = 0; i < M; i++){
        p = (d*p + pat[i])%q;  
        t = (d*t + txt[i])%q;
    }    

    for (i = 0; i <= N - M; i++){ 
        if ( p == t ){
            for (j = 0; j < M; j++){
                if (txt[i+j] != pat[j])
                    break;
            }
 
            if (j == M){
                posfound[ocd] = i;
                ocd++; 
            }
        }
 
        if ( i < N-M ){
            t = (d*(t - txt[i]*h) + txt[i+M])%q;
            
            if (t < 0)
            t = (t + q);
        }
    } 
    posfound[99] = ocd;
    return posfound;
}

 

