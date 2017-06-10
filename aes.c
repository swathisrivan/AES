#include <sys/types.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include "aes_util.h"

/*************************************************************
 Function Main : Beginning of execution to classify which
		 command was issued and call the appropriate
                 validation functions
**************************************************************/

int main(int argc, char *argv[])
{
	memset(Sbox, 0, (SBOX_ROWS * SBOX_COLS));
	memset(ISbox, 0, (SBOX_ROWS * SBOX_COLS));
        memset(P, 0, P_SIZE);
        memset(INVP, 0, P_SIZE);

	if((argc < MIN_ARGS) || (argc > MAX_ARGS))
	{
		fprintf(stderr, "error: malformed command\r\nusage: hw5 tablecheck -t=tablefile\r\n");
                fprintf(stderr, "       hw5 modprod -p1=poly1 -p2=poly2\r\n       hw5 keyexpand -k=key -t=tablefile\r\n");
                fprintf(stderr, "       hw5 encrypt -k=key -t=tablefile [file]\r\n       hw5 decrypt -k=key -t=tablefile [file]\r\n");
                fprintf(stderr, "       hw5 inverse -p=poly\r\n");
                exit(-1);
	}
	if(!strcmp(argv[1], "tablecheck"))
        {
		char *token = NULL;
                char *save  = NULL;
                FILE *fp    = NULL;
	
		if(argc < 3)
		{
			fprintf(stderr, "\r\nerror: malformed command\r\nusage: hw5 tablecheck -t=tablefile\r\n");
                        exit(-1);
		}
                token       = strtok_r(argv[2], "=", &save);
                if(!save || !strlen(save))
                {
                        fprintf(stderr, "\r\nerror: malformed command\r\nusage: hw5 tablecheck -t=tablefile\r\n");
                        exit(-1);
                }
                if(strcmp(token, "-t"))
                {
                        fprintf(stderr, "\r\nerror: malformed command\r\nusage: hw5 tablecheck -t=tablefile\r\n");
                        exit(-1);
                }
                fp = fopen(save, "r");
                if(!fp)
                {
                        fprintf(stderr, "\r\nerror: %s %s\n", save, strerror(errno));
                        exit(-1);
                }
                if(errno)
                {
                        fprintf(stderr, "\r\nerror: %s %s\n", save, strerror(errno));
                        exit(-1);
                }
                validateTable(fp);
	}
	else if(!strcmp(argv[1], "modprod"))
	{
		validateModProd(argc, argv);
	}
	else if(!strcmp(argv[1], "keyexpand"))
	{
		validateKeyExpand(argc, argv);
	}
	else if(!strcmp(argv[1], "encrypt"))
	{
		validateAESEncrypt(argc, argv);
	}
	else if(!strcmp(argv[1], "decrypt"))
        {
                validateAESDecrypt(argc, argv);
        }
	else if(!strcmp(argv[1], "inverse"))
	{
		validateInversePoly(argc, argv);
	}
	else
	{
		fprintf(stderr, "Error: malformed command\r\nusage: hw5 tablecheck -t=tablefile\r\n");
		fprintf(stderr, "       hw5 modprod -p1=poly1 -p2=poly2\r\n       hw5 keyexpand -k=key -t=tablefile\r\n");
		fprintf(stderr, "       hw5 encrypt -k=key -t=tablefile [file]\r\n       hw5 decrypt -k=key -t=tablefile [file]\r\n");
		fprintf(stderr, "       hw5 inverse -p=poly\r\n");
		exit(-1);
	}
	return 0;
}
