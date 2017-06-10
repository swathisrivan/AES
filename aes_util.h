#include <sys/types.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

#define MIN_ARGS 3
#define MAX_ARGS 5
#define SBOX_ROWS 16
#define SBOX_COLS 16
#define P_SIZE 4   
#define IP_SIZE 6
#define BUFF_SIZE 1024
#define TEMP_BUFF 25
#define V_SIZE 8
#define KEY_SIZE 16
#define BLK_SIZE 16
#define Nb 4
#define Nk 4
#define Nr 10

extern char Sbox[SBOX_ROWS][SBOX_COLS];
extern char ISbox[SBOX_ROWS][SBOX_COLS];
extern char P[P_SIZE];
extern char INVP[P_SIZE];
extern char Mx;
extern unsigned int Rcon[11] ;
extern char std[5];
extern char std_x1[5];
extern char std_x2[5];

extern void printKeyValues(int *word_op, int start, int stop, int round);
extern void printIKeyValues(int *word_op, int start, int stop, int round);
extern void printStateValues(char (*state)[Nb]);
extern void computeHex(char *key_str, char *key_byt);
extern void validateTable(FILE *fp);
extern void validateS(char *buff);
extern void validateP(char *buff);
extern void validateINVP(char *buff);
extern char xtime(char b);
extern char computeDotProd(char a, char b);
extern void computeModProd(char *p, char *q, char *out);
extern unsigned int RotWord(unsigned int word_in);
extern unsigned int SubWord(unsigned int word_in);
extern void SubBytes(char (*state)[Nb]);
extern void InvSubBytes(char (*state)[Nb]);
extern void keyExpand(char *key_in, unsigned int *word_op);
extern void validateKeyExpand(int argc, char *argv[]);
extern void validateModProd(int argc, char *argv[]); 
extern void AddRoundKey(char (*state)[Nb], unsigned int *word_op, int start, int stop);
extern void ShiftRows(char (*final_state)[Nb]);
extern void InvShiftRows(char (*final_state)[Nb]);
extern void MixColumns(char (*final_state)[Nb]);
extern void InvMixColumns(char (*final_state)[Nb]); 
extern void AESEncrypt(char *inp_str, char (*out_str)[Nb], unsigned int *word_op);
extern void validateAESEncrypt(int argc, char *argv[]);
extern void AESDecrypt(char *inp_str, char (*out_str)[Nb], unsigned int *word_op);
extern void validateAESDecrypt(int argc, char *argv[]);
extern char computeInvByt(char byt);
extern void computePolyDivide( char (*rem)[P_SIZE + 1], int start, int *at_start, int *bt_start,char *q, char *r, int count);
extern void computeInvPoly(char *p);
extern void validateInversePoly(int argc, char *argv[]);
