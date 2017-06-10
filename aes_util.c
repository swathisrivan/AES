#include "aes_util.h"

char Sbox[SBOX_ROWS][SBOX_COLS];
char ISbox[SBOX_ROWS][SBOX_COLS];
char P[P_SIZE];
char INVP[P_SIZE];
char Mx = 0x1B;
unsigned int Rcon[] = { 0, 0x01000000, 0x02000000, 0x04000000, 0x08000000, 0x10000000,
                           0x20000000, 0x40000000, 0x80000000, 0x1b000000, 0x36000000};
char std[]    = { 0x01, 0x00, 0x00, 0x00, 0x01};
char std_x1[] = { 0x00, 0x00, 0x00, 0x00, 0x00};
char std_x2[] = { 0x00, 0x00, 0x00, 0x00, 0x01};

/******************************************************************
 Function printKeyValues: Function to print the key values for
			  every round
******************************************************************/
void printKeyValues(int *word_op, int start, int stop, int round)
{
	int i = 0;

	fprintf(stdout, "round[%2d].k_sch    ", round);
        for(i = start; i <= stop; i++)
        {
                fprintf(stdout, "%08x", word_op[i]);
        }
        fprintf(stdout, "\n");
}
/******************************************************************
 Function printIKeyValues: Function to print the key values for
                           every round
******************************************************************/
void printIKeyValues(int *word_op, int start, int stop, int round)
{
        int i = 0;

        fprintf(stdout, "round[%2d].ik_sch   ", round);
        for(i = start; i <= stop; i++)
        {
                fprintf(stdout, "%08x", word_op[i]);
        }
        fprintf(stdout, "\n");
}
/********************************************************************
 Function printStateValues: Function to print the value of the state
		            array after every operation 
********************************************************************/
void printStateValues(char (*state)[Nb])
{
	int i = 0;
	int j = 0;
	
	for(i = 0; i < 4; i++)
	{
		for(j = 0; j < Nb; j++)
		{
			fprintf(stdout, "%02x", (state[j][i] & 0xff));
		}
	}
	fprintf(stdout, "\n");
}
/********************************************************************
 Function computeHex: To compute the equivalent hex value of a given
                      string in key_str and the output is in key_byt
*********************************************************************/
void computeHex(char *key_str, char *key_byt)
{
        int i   = 0;
        int j   = 0;
        int len = strlen(key_str);
        char temp[TEMP_BUFF];

        memset(temp, 0, TEMP_BUFF);

        while(i < len)
        {
                strncpy(temp, &key_str[i], 2);
                key_byt[j] = (char)strtol(temp, NULL, 16);
                j++;
                i += 2;
                memset(temp, 0, TEMP_BUFF);
        }
}
/********************************************************************
 Function validateS: To validate the entry in the input tablefile
                     corresponding to S and to make sure values of 
		     sbox are between 0x00 and 0xff
*********************************************************************/

void validateS(char *buff)
{
	char *token  = NULL;
        char *save   = NULL;
	int i        = 0;
	int j        = 0;
	int k        = 0;
	int m        = 0;
	char calc    = 0;
	char ptr[SBOX_ROWS * SBOX_COLS];
	char temp[BUFF_SIZE];
	int check[SBOX_ROWS * SBOX_COLS];

        memset(ptr, 0, 	(SBOX_ROWS * SBOX_COLS));
	memset(temp, 0, BUFF_SIZE);
	memset(check, 0, (SBOX_ROWS * SBOX_COLS * 4));

        token  = strtok_r(buff, "=", &save);
        if(!save || !strlen(save))
        {
                fprintf(stderr, "\r\nerror: malformed input table file\r\n");
                exit(-1);
        }
        if(strcmp(token, "S"))
        {
                fprintf(stderr, "\r\nerror: malformed input table file\r\n");
                exit(-1);
        }
        strcpy(temp, save); 
        for(i = 0; i < strlen(temp); i++)
	{
		if(!((temp[i] >= '0' && temp[i] <= '9') || (temp[i] >= 'a' && temp[i] <= 'f')))
		{
			fprintf(stderr, "\r\nerror: Sbox can contain only hexadecimal values\r\n");
	                exit(-1);
		}
	}
        computeHex(save, ptr);
	for(i = 0; i < SBOX_ROWS; i++)
	{
		for(j = 0; j < SBOX_COLS; j++)
		{
			m = ptr[k];
			m = (m & 0x000000ff);
			if(check[m] == 1)
			{
				fprintf(stderr, "\r\nerror: Sbox must not contain repeating values\r\n");
	                        exit(-1);
			}
			check[m]   = 1;
			Sbox[i][j] = ptr[k++];
		}
	}

	//computing inverse sbox
	for(i = 0; i < SBOX_ROWS; i++)
	{
		for(j = 0; j < SBOX_COLS; j++)
		{
			calc = (char)((i << 4) & (0x000000f0));
			calc = (char)(calc | (j & 0x0000000f)); 
			ISbox[((Sbox[i][j] >> 4) & 0x0f)][(Sbox[i][j] & 0x0f)] = (calc & 0xff);
		}
	} 
	
}
/********************************************************************
 Function validateP: To validate the entry in the input tablefile
                     corresponding to P
*********************************************************************/
void validateP(char *buff)
{
	
	char *token  = NULL;
        char *save   = NULL;
	int i        = 0;
        char ptr[P_SIZE];
	char temp[BUFF_SIZE];

	memset(ptr, 0, P_SIZE);
	memset(temp, 0, BUFF_SIZE);

        token  = strtok_r(buff, "=", &save);
        if(!save || !strlen(save))
        {
                fprintf(stderr, "\r\nerror: malformed input table file\r\n");
                exit(-1);
        }
        if(strcmp(token, "P"))
        {
                fprintf(stderr, "\r\nerror: malformed input table file\r\n");
                exit(-1);
        }
	if(strlen(save) != (P_SIZE * 2))
	{
		fprintf(stderr, "\r\nerror: malformed input table file - invalid P value\r\n");
                exit(-1);
	}
	strcpy(temp, save);
        for(i = 0; i < strlen(temp); i++)
        {
                if(!((temp[i] >= '0' && temp[i] <= '9') || (temp[i] >= 'a' && temp[i] <= 'f')))
                {
                        fprintf(stderr, "\r\nerror: P can contain only hexadecimal values\r\n");
                        exit(-1);
                }
        }

	computeHex(save, ptr);
	for(i = 0; i < P_SIZE; i++)
        {
		P[i] = ptr[i];
        }

        /*for debugging purposes only
        for(i = 0; i < P_SIZE; i++)
        {
        	printf("%02x\t", P[i] & 0xff);
        }*/

}
/********************************************************************
 Function validateINVP: To validate the entry in the input tablefile
                        corresponding to INVP
*********************************************************************/
void validateINVP(char *buff)
{
	char *token  = NULL;
        char *save   = NULL;
	int i        = 0;
	char ptr[P_SIZE];
	char temp[BUFF_SIZE];

        memset(ptr, 0, P_SIZE);
	memset(temp, 0, BUFF_SIZE);

        token  = strtok_r(buff, "=", &save);
        if(!save || !strlen(save))
        {
                fprintf(stderr, "\r\nerror: malformed input table file\r\n");
                exit(-1);
        }
        if(strcmp(token, "INVP"))
        {
                fprintf(stderr, "\r\nerror: malformed input table file\r\n");
                exit(-1);
        }
	if(strlen(save) != (P_SIZE * 2))
        {
                fprintf(stderr, "\r\nerror: malformed input table file - invalid INVP value\r\n");
                exit(-1);
        }
	strcpy(temp, save);
        for(i = 0; i < strlen(temp); i++)
        {
                if(!((temp[i] >= '0' && temp[i] <= '9') || (temp[i] >= 'a' && temp[i] <= 'f')))
                {
                        fprintf(stderr, "\r\nerror: INVP can contain only hexadecimal values\r\n");
                        exit(-1);
                }
        }

	computeHex(save, ptr);
	for(i = 0; i < P_SIZE; i++)
        {
                INVP[i] = ptr[i];
        }
        /* for debugging purposes only
        for(i = 0; i < P_SIZE; i++)
        {
                printf("%02x\t", INVP[i] & 0xff);
        }*/
}
/***************************************************************
 Function xtime: Function to compute the xtime() of given b 
		 depending on the most significant bit set
		 Returns the xtime value of b
***************************************************************/
char xtime(char b)
{
	if(((b >> 7) & 0x01) != 1)
	{
		return (b << 1);
	}
	else
	{
		return ((b << 1) ^ Mx);
	}
}

/******************************************************************
 Function computeDotProd: Function to compute the dot product of
		          a(x) and b(x) where b(x) is the target
			  and a(x) is used to calculate xtime()
			  Returns the dot product of a(x) and
                          b(x) in p
******************************************************************/
char computeDotProd(char a, char b)
{
	char varray[V_SIZE];
	char p       = 0x00;
	char A       = a;
        int i        = 0; 

	memset(varray, 0, V_SIZE);
	varray[0]    = A;

	//left shift to check which bits are set in the target -- target is b and xtime is A
	if((b & 0x01) == 1)
	{
		p = A;
	}
	for(i = 1; i < V_SIZE; i++)
	{
		varray[i] = xtime(A);
		A         = varray[i];

		if(((b >> i) & 0x01) == 1)
		{
			p = (A ^ p);
		}
	}
	return p;
}
/************************************************************************
 Function validatePINVPMod(): Function to calculate and validate the 
			      mod-prod of P and INVP and to check if 
			      get the identity elements {00}{00}{00}{01}
*************************************************************************/
void computeModProd(char *p, char *q, char *out)
{
	int i = 0;
	int j = 0;
	int k = 0;
	int n = P_SIZE;
        char aMat[P_SIZE][P_SIZE];
	char bMat[P_SIZE][1];
	char result[P_SIZE][1];

	memset(result, 0, P_SIZE * 1);
	memset(aMat, 0, (P_SIZE * P_SIZE));
	memset(bMat, 0, P_SIZE);

	//populating a matrix with rotation	
	for(i = 0; i < P_SIZE; i++)
	{
		for(j = 0; j < P_SIZE; j++)
		{
			aMat[j][i] = p[P_SIZE - 1 - ((j + n ) % P_SIZE)];
		}
		n--;
	}
	
	//populating b matrix with rotation
	for(i = 0; i < P_SIZE; i++)
	{
		bMat[i][0] = q[P_SIZE - 1 - i];

	}

	//implementing matrix multipication
	for(i = 0; i < P_SIZE; i++)
	{
		for(j = 0; j < 1; j++)
		{
			for(k = 0; k < P_SIZE; k++)
			{
				result[i][j] ^= (computeDotProd(aMat[i][k], bMat[k][j]));
			
			}
		
		}
	
	}
	for(i = 0; i < P_SIZE; i++)
	{
		out[i] = result[i][0];
	}

        /*for debugging only
	for(i = 0; i < 4; i++)
	{
		for(j = 0 ; j < 1; j++)
		{
			printf("%02x ", result[i][j] & 0xff);
		}
	}
	exit(-1);*/

}
/*************************************************************
 Function RotWord: Function to rotate the given word by one
		   byte and return the rotated byte
*************************************************************/
unsigned int RotWord(unsigned int word_in)
{
	unsigned int word_out = 0;

	word_out = ((word_in << 8) & 0xffffff00);
	word_out = (word_out | ((word_in >> 24) & 0x000000ff));
	return word_out;
}
/*************************************************************
 Function SubWord: Function to implement the substitution for
                   the given word using Sbox and return the 
	           newly substituted word
*************************************************************/
unsigned int SubWord(unsigned int word_in)
{
	unsigned int word_out = 0;
	unsigned int temp     = 0;
	int row               = 0;
	int col               = 0;
	int i                 = 0;
	for(i = 0; i < 4; i++)
	{
		row      = (word_in >> (((3 - i) * 8) + 4)) & (0x0000000f);
		col      = (word_in >> ((3 - i) * 8)) & (0x0000000f);
		temp     = Sbox[row][col] & 0x000000ff;
		word_out = (word_out | (temp << ((3 - i) * 8))); 
	}
	return word_out;
}

/******************************************************************
 Function SubBytes: Function to implement the subtitution for the
		    given byte using Sbox and return the value of 
		    substitution from the sbox
******************************************************************/
void SubBytes(char (*state)[Nb])
{
	int i    = 0;
	int j    = 0;
	int row  = 0;
	int col  = 0;

	for(i = 0; i < 4; i++)
	{
		for(j = 0; j < Nb; j++)
		{
			row         = ((state[j][i] >> 4) & 0x0f);	
			col         = (state[j][i] & 0x0f);
			state[j][i] = (Sbox[row][col] & 0xff);	
		}
	}
}
/*********************************************************************
 Function InvSubBytes: Function to implement the inverse subtitution 
                       for the given byte using ISbox and return the 
                       value of substitution from the ISbox
******************************************************************/
void InvSubBytes(char (*state)[Nb])
{
        int i    = 0;
        int j    = 0;
        int row  = 0;
        int col  = 0;

        for(i = 0; i < 4; i++)
        {
                for(j = 0; j < Nb; j++)
                {
                        row         = ((state[j][i] >> 4) & 0x0f);
                        col         = (state[j][i] & 0x0f);
                        state[j][i] = (ISbox[row][col] & 0xff);
                }
        }
}

/**************************************************************
 Function validateTable : Function to validate the values of S
		          and check the modprod of P & INVP
**************************************************************/
void validateTable(FILE *fp)
{
	int i       = 0;
        char buff[BUFF_SIZE];
	char out[P_SIZE];

        memset(buff, 0, BUFF_SIZE);
	memset(out , 0, P_SIZE);

        while(fgets(buff, BUFF_SIZE, fp))
        {
		buff[strlen(buff) - 1] = '\0';
                if(i == 0)
                {
                        validateS(buff);
                }
                if(i == 1)
                {
                        validateP(buff);
                }
                if(i == 2)
                {
                        validateINVP(buff);
                }
		i++;
	}
	if(i < 3)
	{
		fprintf(stderr, "error: malformed table file - One of S, P and INVP values missing\r\n");
                exit(-1);
	}	
	if(i > 3)
	{
		fprintf(stderr, "error: malformed table file\r\n");
                exit(-1);
	}
	
	computeModProd(P, INVP, out);
	
	//validation of modprod of P and INVP
	if((out[0] != 0x01) || (out[1] != 0x00) || (out[2] != 0x00) || (out[3] != 0x00))
	{
		fprintf(stderr, "error: malformed table file modprod of P and INVP has to be {00}{00}{00}{01}\r\n");
		exit(-1);
	}
}
/***************************************************************
 Function keyExpand: Function to expand the key using SubWord()
		     RotWord() and Rcon
***************************************************************/
void keyExpand(char *key_in, unsigned int *word_op)
{
	int i             = 0;
	int j             = 0;
	int k             = 0;
	unsigned int Y    = 0;
        unsigned int temp = 0;

	//populating the first 4 words - each word is 4 bytes
        for(i = 0; i < Nk; i++)
        {
                for(j = 0; j < 4; j++)
                {
                        //if bit error possible error spot
                        Y = ((key_in[k] & 0xff) << ((3 - j) * 8));
                        word_op[i] = (word_op[i] | Y);
                        k++;
                }
        }

        //proceeding to remaining set of actions
        for(i = Nk; i < (Nb * (Nr + 1)); i++)
        {
                temp = word_op[i - 1];
                if(i % Nk == 0)
                {
                        temp = ((SubWord(RotWord(temp))) ^ (Rcon[i / Nk]));
                }
                word_op[i] = (word_op[i - Nk] ^ temp);
        }
	
}
/**************************************************************
 Function validateKeyExpand: Function to validate and expand 
			     the input key argument by calling
			     keyExpand()
**************************************************************/
void validateKeyExpand(int argc, char *argv[])
{
	int i             = 0;
	char *token       = NULL;
	char *save        = NULL;
	unsigned int word_op[Nb * (Nr + 1)];
        char key_in[KEY_SIZE];
	FILE *fp          = NULL;

	memset(key_in, 0, KEY_SIZE);
	memset(word_op, 0, sizeof(int) * (Nb * (Nr + 1)));

	if(argc < 4 || argc > 4)
	{
		fprintf(stderr, "\r\nerror: malformed command\r\n Usage: hw5 keyexpand -k=key -t=tablefile\r\n");
                exit(-1);
	}

	token  = strtok_r(argv[2], "=", &save);
        if(!save || !strlen(save))
        {
                fprintf(stderr, "\r\nerror: malformed input key value\r\n");
                exit(-1);
        }
        if(strcmp(token, "-k"))
        {
		if(strcmp(token, "-t"))
        	{
                	fprintf(stderr, "\r\nerror: malformed command\r\n Usage: hw5 keyexpand -k=key -t=tablefile\r\n");
                	exit(-1);
		}
		else
		{
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
        }
	else
	{
		if(strlen(save) != 32)
        	{
                	fprintf(stderr, "\r\nerror: malformed key - key must be 16 bytes in length\r\n");
                	exit(-1);

        	}

		//computing hex values
		computeHex(save, key_in);
	}
	
	//validating and populating Table values
	save  = NULL;
	token = NULL;

        token = strtok_r(argv[3], "=", &save);
        if(!save || !strlen(save))
        {
		fprintf(stderr, "\r\nerror: malformed command\r\n Usage: hw5 keyexpand -k=key -t=tablefile\r\n");
                exit(-1);

        }
        if(strcmp(token, "-t"))
        {
		if(strcmp(token, "-k"))
	        {
			fprintf(stderr, "\r\nerror: malformed command\r\n Usage: hw5 keyexpand -k=key -t=tablefile\r\n");
	                exit(-1);

		}
		else
		{
			if(strlen(save) != 32)
                	{
                        	fprintf(stderr, "\r\nerror: malformed key - key must be 16 bytes in length\r\n");
                        	exit(-1);

                	}

                	//computing hex values
                	computeHex(save, key_in);
		}

        }
	else
	{
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

	keyExpand(key_in, word_op);

	for(i = 0; i < (Nb * (Nr + 1)); i++) 
	{
		fprintf(stdout, "w[%2d]: %08x\n", i, word_op[i]);
	}
}
/*********************************************************************
 Function validateModProd: Function to perform preliminary checks 
			   on the given input dat and pass the two
			   polynomials to the computeModProd function
**********************************************************************/
void validateModProd(int argc, char *argv[])
{
	int i       = 0;
	char *token = NULL;
	char *save  = NULL;
	char poly1[P_SIZE];
	char poly2[P_SIZE];
	char out[P_SIZE];
	
	memset(poly1, 0, P_SIZE);
	memset(poly2, 0, P_SIZE);
	memset(out, 0, P_SIZE);

	if(argc < 4 || argc > 4)
	{
		fprintf(stderr, "\r\nerror: malformed command\r\nusage: hw5 modprod -p1=poly1 -p2=poly2\r\n");
                exit(-1);
	}
	
	token = strtok_r(argv[2], "=", &save);
        if(!save || !strlen(save))
        {
                fprintf(stderr, "\r\nerror: malformed command\r\nusage: hw5 modprod -p1=poly1 -p2=poly2\r\n");
                exit(-1);
        }
        if(strcmp(token, "-p1"))
        {
		if(strcmp(token, "-p2"))
	        {
        	        fprintf(stderr, "\r\nerror: malformed command\r\nusage: hw5 modprod -p1=poly1 -p2=poly2\r\n");
                	exit(-1);
        	}
		else
		{
			computeHex(save, poly2);
		}	
		
        }
	else
	{
		computeHex(save, poly1);		
	}

	token = NULL;
	save  = NULL;
	
	token = strtok_r(argv[3], "=", &save);
        if(!save || !strlen(save))
        {
                fprintf(stderr, "\r\nerror: malformed command\r\nusage: hw5 modprod -p1=poly1 -p2=poly2\r\n");
                exit(-1);
        }
        if(strcmp(token, "-p2"))
        {
		if(strcmp(token, "-p1"))
                {
                        fprintf(stderr, "\r\nerror: malformed command\r\nusage: hw5 modprod -p1=poly1 -p2=poly2\r\n");
                        exit(-1);
                }
                else
                {
                        computeHex(save, poly1);
                }

        }
	else
	{
        	computeHex(save, poly2);
	}

	computeModProd(poly1, poly2, out);
	
	for(i = 0; i < P_SIZE; i++)
	{
		fprintf(stdout, "{%02x}", (poly1[i] & 0xff)); 
	}
	fprintf(stdout, " CIRCLEX ");
	for(i = 0; i < P_SIZE; i++)
        {
                fprintf(stdout, "{%02x}", (poly2[i] & 0xff));
        }
	fprintf(stdout, " = ");
	for(i = (P_SIZE - 1); i >= 0; i--)
        {
                fprintf(stdout, "{%02x}", (out[i] & 0xff));
        }
	fprintf(stdout, "\n");
	
}
/************************************************************************
 Function AddRoundKey: Function to peform the add round key operation
                       where in the input state is xored with four
		       words of subkeys
*************************************************************************/
void AddRoundKey(char (*state)[Nb], unsigned int *word_op, int start, int stop)
{
	int i             = 0;
	int j             = 0;
	int m             = 0;
	int n             = 0;
	char byt          = 0;
	unsigned int temp = 0;

	for(i = start; i <= stop; i++)
	{
		temp = word_op[i];

		for(j = 3; j >= 0; j--)
		{
			byt         = ((temp >> (j * 8)) & (0x000000ff));
			byt         = (byt & 0xff);
			state[m][n] = (state[m][n] ^ byt);
			m++;
			if(m == Nb)
			{
				m = 0;
				n++;
			}
		}
	}

}
/*********************************************************************
 Function ShiftRows: Function to perform shift rows operation on the
		     2D-array. This shift will take row-wise to the 
		     left
*********************************************************************/
void ShiftRows(char (*final_state)[Nb])
{
	int i = 0;
	int j = 0;
	int k = 0;
	char local[4][Nb];
	
	memset(local, 0, (4 * Nb));
	for(i = 0; i < 4; i++)
	{
		for(j = 0; j < Nb; j++)
		{
			local[i][j] = final_state[i][((j + k) % Nb)];
		}
		k++;
	}
	for(i = 0; i < 4; i++)
	{
		for(j = 0; j < Nb; j++)
		{
			final_state[i][j] = local[i][j];
		}
	}
}
/***********************************************************************
 Function InvShiftRows: Function to perform shift rows operation on the
                        2D-array in the reverse. This shift will take 
                        place row-wise to the right
************************************************************************/
void InvShiftRows(char (*final_state)[Nb])
{
	int i = 0;
        int j = 0;
        int k = 0;
        char local[4][Nb];

        memset(local, 0, (4 * Nb));
        for(i = 0; i < 4; i++)
        {
                for(j = 0; j < Nb; j++)
                {
                        local[i][j] = final_state[i][(j - k + Nb) % Nb];
                }
                k++;
        }
        for(i = 0; i < 4; i++)
        {
                for(j = 0; j < Nb; j++)
                {
                        final_state[i][j] = local[i][j];
                }
        }

}
/***************************************************************************
 Function MixColumns: Function to perform mix column transformation with P
		      for every column in a column major fashion
****************************************************************************/
void MixColumns(char (*final_state)[Nb])
{
	int i = 0;
	int j = 0;
	char bMatrix[P_SIZE];
	char result[P_SIZE];

	memset(bMatrix, 0, P_SIZE);
	memset(result, 0, P_SIZE);

	for(i = 0; i < 4; i++)
	{
		for(j = 0; j < 4; j++)
		{
			bMatrix[3 - j] = final_state[j][i];
		}
		computeModProd(P, bMatrix, result);
		for(j = 0; j < 4; j++)
		{
			final_state[j][i] = result[j];
		}
		memset(bMatrix, 0, P_SIZE);
	        memset(result, 0, P_SIZE);
	
	}
}
/*****************************************************************************
 Function InvMixColumns: Function to perform mix column transformation
                         with INVP for every column in a column major fashion
******************************************************************************/
void InvMixColumns(char (*final_state)[Nb])
{
        int i = 0;
        int j = 0;
        char bMatrix[P_SIZE];
        char result[P_SIZE];

        memset(bMatrix, 0, P_SIZE);
        memset(result, 0, P_SIZE);

        for(i = 0; i < 4; i++)
        {
                for(j = 0; j < 4; j++)
                {
                        bMatrix[3 - j] = final_state[j][i];
                }
                computeModProd(INVP, bMatrix, result);
                for(j = 0; j < 4; j++)
                {
                        final_state[j][i] = result[j];
                }
                memset(bMatrix, 0, P_SIZE);
                memset(result, 0, P_SIZE);
        }
}
/*******************************************************************
 Function AESEncrypt: Function to perform AES encryption on the
		      given inp_str and output in out_str by 
		      using the subkeys in word_op
********************************************************************/
void AESEncrypt(char *inp_str, char (*out_str)[Nb], unsigned int *word_op)
{
	int start = 0;
	int stop  = 3;
	int round = 0;
	int i     = 0;
	int j     = 0;
	int k     = 0;
	char state[4][Nb];

	memset(state, 0, (4 * Nb));

	fprintf(stdout, "round[%2d].input    ", round);
	//converting inp_str to a 2-D column major array
	for(i = 0; i < 4; i++)
	{
		for(j = 0; j < Nb; j++)
		{
			state[j][i] = inp_str[k];
			fprintf(stdout, "%02x", (state[j][i] & 0xff));
			k++;
		}
	}
	fprintf(stdout, "\n");

	//performing xor with roundKey
	printKeyValues((int *)word_op, start, stop, round);

	AddRoundKey(state, word_op, start, stop);

	//performing remaining 9 rounds of AES
	for(round = 1; round < Nr; round++)
	{
		fprintf(stdout, "round[%2d].start    ", round);
        	printStateValues(state);

		//performing Sub byte transformation
		SubBytes(state);
		fprintf(stdout, "round[%2d].s_box    ", round);
	        printStateValues(state);

		//performing shift rows operation
		ShiftRows(state);
		fprintf(stdout, "round[%2d].s_row    ", round);
	        printStateValues(state);

		//performing mixed column operation
		MixColumns(state);
		fprintf(stdout, "round[%2d].m_col    ", round);
                printStateValues(state);


		//performing AddRoundkey
		start = round * Nb;
		stop  = (((round + 1) * Nb) - 1);

		printKeyValues((int *)word_op, start, stop, round);
		AddRoundKey(state, word_op, start, stop);
		
	}

	//performing last round of unique operation
	fprintf(stdout, "round[%2d].start    ", round);
        printStateValues(state);	

	SubBytes(state);
	fprintf(stdout, "round[%2d].s_box    ", round);
        printStateValues(state);

	ShiftRows(state);
	fprintf(stdout, "round[%2d].s_row    ", round);
        printStateValues(state);

	start = (Nr * Nb);
	stop  = (((Nr + 1) * Nb) - 1);

	printKeyValues((int *)word_op, start, stop, round);
	AddRoundKey(state, word_op, start, stop);
	fprintf(stdout, "round[%2d].output   ", round);
        printStateValues(state);

	//copying state into the output array
	for(i = 0; i < 4; i++)
        {
                for(j = 0; j < Nb; j++)
                {
			out_str[i][j] = state[i][j];
                }
        }

}
/****************************************************************
 Function validateAESEncrypt: Function to validate the input
                              arguments for encrypt and perform
                              AES encryption
****************************************************************/
void validateAESEncrypt(int argc, char *argv[])
{
	char *token = NULL;
        char *save  = NULL;
	int n       = 0;
	int first   = 1;
        FILE *fp    = NULL;
	FILE *fp_i  = NULL;
        unsigned int word_op[Nb * (Nr + 1)];
        char key_in[KEY_SIZE];
	char inp_str[BLK_SIZE];
	char out_str[4][Nb];

        memset(key_in, 0, KEY_SIZE);
	memset(inp_str, 0, BLK_SIZE);
	memset(out_str, 0, (4 * Nb));
        memset(word_op, 0, sizeof(int) * (Nb * (Nr + 1)));

	if(argc < 4 || argc > 5)
	{
		fprintf(stderr, "\r\nerror: malformed command\r\nusage: hw5 encrypt -k=key -t=tablefile [file]\r\n");
                exit(-1);
	}
	//parsing, validating and populating the key value 
	token       = strtok_r(argv[2], "=", &save);
        if(!save || !strlen(save))
        {
        	fprintf(stderr, "\r\nerror: malformed command\r\nusage: hw5 encrypt -k=key -t=tablefile [file]\r\n");
                exit(-1);
        }
        if(strcmp(token, "-k"))
	{
		if(strcmp(token, "-t"))
		{
			fprintf(stderr, "\r\nerror: malformed command\r\nusage: hw5 encrypt -k=key -t=tablefile [file]\r\n");
			exit(-1);
		}
		else
		{
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
        }
	else
	{
		if(strlen(save) != 32)
		{
			fprintf(stderr, "\r\nerror: malformed key - key must be 16 bytes in length\r\n");
                	exit(-1);

		}
		computeHex(save, key_in);
	}

	//parsing, validating and populating the table values
	token = NULL;
	save  = NULL;
	token       = strtok_r(argv[3], "=", &save);
        if(!save || !strlen(save))
        {
                fprintf(stderr, "\r\nerror: malformed command\r\nusage: hw5 encrypt -k=key -t=tablefile [file]\r\n");
                exit(-1);
        }
        if(strcmp(token, "-t"))
        {
		if(strcmp(token, "-k"))
	        {
        	        fprintf(stderr, "\r\nerror: malformed command\r\nusage: hw5 encrypt -k=key -t=tablefile [file]\r\n");
                	exit(-1);
		}
		else
		{
			if(strlen(save) != 32)
	                {
        	                fprintf(stderr, "\r\nerror: malformed key - key must be 16 bytes in length\r\n");
                	        exit(-1);

                	}
                	computeHex(save, key_in);
		}
        }
	else
	{
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
	
	//parsing and validating the input file
	if(argc > 4)
        {
                fp_i = fopen(argv[4], "r");
        }
        else
        {
                fp_i = stdin;
        }
        if(!fp_i)
        {
                fprintf(stderr, "\r\nerror: %s %s\n", argv[4], strerror(errno));
                exit(-1);
        }
        if(errno)
        {
                fprintf(stderr, "\r\nerror: %s %s\n", argv[4], strerror(errno));
                exit(-1);
        }

	//computing the subkeys
	keyExpand(key_in, word_op);

	//performing actual encryption on every set of 16 bytes from the input file	
	while(1)
        {
		
                n = 0;
                n = fread(inp_str, 1, BLK_SIZE, fp_i);

                if(n == 0)
                {
                        break;
                }
		if(!first)
		{
			fprintf(stderr, "error: input for encryption can only be 16 bytes long\r\n");
        	        exit(-1);
	        }

		AESEncrypt(inp_str, out_str, word_op);	
                memset(inp_str, 0, BLK_SIZE);
		memset(out_str, 0, BLK_SIZE);
		first = 0;
        }

}
/**************************************************************************
 Function AESDecrypt: Function to perform AES decryption on the
                      given inp_str and output in out_str by 
                      using the subkeys in word_op
***************************************************************************/
void AESDecrypt(char *inp_str, char (*out_str)[Nb], unsigned int *word_op)
{
	int start = Nr * Nb;
        int stop  = (((Nr + 1) * Nb) - 1);
        int round = 0;
        int i     = 0;
        int j     = 0;
        int k     = 0;
        char state[4][Nb];

        memset(state, 0, (4 * Nb));

	fprintf(stdout, "round[%2d].iinput   ", round);
        //converting inp_str to a 2-D column major array
        for(i = 0; i < 4; i++)
        {
                for(j = 0; j < Nb; j++)
                {
                        state[j][i] = inp_str[k];
			fprintf(stdout, "%02x", (state[j][i] & 0xff));
                        k++;
                }
        }
	fprintf(stdout, "\n");

        //performing xor with roundKey
	printIKeyValues((int *)word_op, start, stop, round);
        AddRoundKey(state, word_op, start, stop);

	//performing 10 rounds of AES Decryption
	for(round = (Nr - 1); round > 0; round--) 
	{
		fprintf(stdout, "round[%2d].istart   ", (Nr - round));
                printStateValues(state);

		//performing inverse shift rows to the right
		InvShiftRows(state);
		fprintf(stdout, "round[%2d].is_row   ", (Nr - round));
                printStateValues(state);

		//performing inverse substitution using ISbox
		InvSubBytes(state);
		fprintf(stdout, "round[%2d].is_box   ", (Nr - round));
                printStateValues(state);

		//performing AddRoundKey
		start = (round * Nb);
		stop  = (((round + 1) * Nb) - 1);

		printIKeyValues((int *)word_op, start, stop, (Nr - round));
		AddRoundKey(state, word_op, start, stop);
		fprintf(stdout, "round[%2d].ik_add   ", (Nr - round));
                printStateValues(state);

		//performing inverse mix columns using INVP
		InvMixColumns(state);

	}

	//performing unique operation for the last round of AES
	fprintf(stdout, "round[%2d].istart   ", (Nr - round));
        printStateValues(state);

	InvShiftRows(state);
	fprintf(stdout, "round[%2d].is_row   ", (Nr - round));
        printStateValues(state);

	InvSubBytes(state);
	fprintf(stdout, "round[%2d].is_box   ", (Nr - round));
        printStateValues(state);

	start = 0;
	stop  = (Nb - 1);
	printIKeyValues((int *)word_op, start, stop, (Nr - round));
	AddRoundKey(state, word_op, start, stop);

	fprintf(stdout, "round[%2d].ioutput  ", (Nr - round));
        printStateValues(state);

	//copying state into the output array
        for(i = 0; i < 4; i++)
        {
                for(j = 0; j < Nb; j++)
                {
                        out_str[i][j] = state[i][j];
                }
        }
	
}
/****************************************************************
 Function validateAESDecrypt: Function to validate the input
                              arguments for decrypt and perform
                              AES decryption
****************************************************************/
void validateAESDecrypt(int argc, char *argv[])
{
	int first   = 1;
        char *token = NULL;
        char *save  = NULL;
        int n       = 0;
        FILE *fp    = NULL;
        FILE *fp_i  = NULL;
        unsigned int word_op[Nb * (Nr + 1)];
        char key_in[KEY_SIZE];
        char inp_str[BLK_SIZE];
        char out_str[4][Nb];

        memset(key_in, 0, KEY_SIZE);
        memset(inp_str, 0, BLK_SIZE);
        memset(out_str, 0, (4 * Nb));
        memset(word_op, 0, sizeof(int) * (Nb * (Nr + 1)));

	if(argc < 4 || argc > 5)
	{
		fprintf(stderr, "\r\nerror: malformed command\r\nusage: hw5 decrypt -k=key -t=tablefile [file]\r\n");
                exit(-1);

	}
        //parsing, validating and populating the key value 
        token       = strtok_r(argv[2], "=", &save);
        if(!save || !strlen(save))
        {
                fprintf(stderr, "\r\nerror: malformed command\r\nusage: hw5 decrypt -k=key -t=tablefile [file]\r\n");
                exit(-1);
        }
	if(strcmp(token, "-k"))
        {
		if(strcmp(token, "-t"))
	        {
        	        fprintf(stderr, "\r\nerror: malformed command\r\nusage: hw5 decrypt -k=key -t=tablefile [file]\r\n");
                	exit(-1);
		}
		else
		{
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
        }
	else
	{
		if(strlen(save) != 32)
        	{
                	fprintf(stderr, "\r\nerror: malformed key - key must be 16 bytes in length\r\n");
                	exit(-1);

        	}
        	computeHex(save, key_in);
	}

        //parsing, validating and populating the table values
        token = NULL;
        save  = NULL;
        token       = strtok_r(argv[3], "=", &save);
        if(!save || !strlen(save))
        {
                fprintf(stderr, "\r\nerror: malformed command\r\nusage: hw5 decrypt -k=key -t=tablefile [file]\r\n");
                exit(-1);
        }
        if(strcmp(token, "-t"))
        {
		if(strcmp(token, "-k"))
		{
                	fprintf(stderr, "\r\nerror: malformed command\r\nusage: hw5 decrypt -k=key -t=tablefile [file]\r\n");
	                exit(-1);
		}
		else
		{
			if(strlen(save) != 32)
	                {
        	                fprintf(stderr, "\r\nerror: malformed key - key must be 16 bytes in length\r\n");
                	        exit(-1);

	                }
        	        computeHex(save, key_in);

		}
        }
	else
	{
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
	
	if(argc > 4)
        {
                fp_i = fopen(argv[4], "r");
        }
        else
        {
                fp_i = stdin;
        }
        if(!fp_i)
        {
                fprintf(stderr, "\r\nerror: %s %s\n", argv[4], strerror(errno));
                exit(-1);
        }
        if(errno)
        {
                fprintf(stderr, "\r\nerror: %s %s\n", argv[4], strerror(errno));
                exit(-1);
        }

        //computing the subkeys
        keyExpand(key_in, word_op);

        //performing actual encryption on every set of 16 bytes from the input file     
        while(1)
        {
                n = 0;
                n = fread(inp_str, 1, BLK_SIZE, fp_i);

                if(n == 0)
                {
                        break;
                }
		if(!first)
                {
                        fprintf(stderr, "error: input for encryption can only be 16 bytes long\r\n");
                        exit(-1);
                }
                AESDecrypt(inp_str, out_str, word_op);
                memset(inp_str, 0, BLK_SIZE);
                memset(out_str, 0, BLK_SIZE);
		first = 0;
        }

}
/**************************************************************
 Function computeInvByt: Function to compute the inverse byte
                          of the given byt 
**************************************************************/
char computeInvByt(char byt)
{
	int  j   = 0;
	char i   = 0x01;
	char res = 0x00; 
	for (j = 1; j <= 255; j++) 
	{
		res  = computeDotProd(byt, i);
 		if (res == 0x01)
		{
			
			//i is the multiplicative inverse of byt
			return  i;
 		}
		i++;
	}
	return 0x00;
}
/***********************************************************************
 Function computePolyDivide: Function to compute polynomial division
************************************************************************/
void computePolyDivide( char (*rem)[P_SIZE + 1], int start, int *at_start,
       			        int *bt_start,char *q, char *r, int count)
{
	int i       = 0;
	int j       = 0;
	int a_start = (*at_start);
	int b_start = (*bt_start); 
	char temp   = 0x00;
	char prod[P_SIZE + 1];
	char r_t[P_SIZE + 1];
	char a_t[P_SIZE + 1];
	char b_t[P_SIZE + 1];

	memset(a_t, 0, (P_SIZE + 1)); 	
	memset(b_t, 0, (P_SIZE + 1));
	memset(prod, 0, (P_SIZE + 1));
	memset(r_t, 0, (P_SIZE + 1));

	for(i = 0; i < (P_SIZE + 1); i++)
	{
		a_t[i] = rem[start][i];
		b_t[i] = rem[start + 1][i];
	}

	while(1)
	{
		if(a_start > b_start)
		{
			break;
		}
		
		if(a_t[a_start] == 0x01)
		{
			temp = computeInvByt(b_t[b_start]);
			if((a_start == 4) && (b_start == 4) && count == 3)
			{
				r[4] = 0x01;
				q[4] = temp;
				return;
				
			}
			if((a_start == 4) && (b_start == 4) && count != 3)
                        {
                                temp = computeDotProd(temp, (a_t[a_start] ^ 0x01));
                        }
		}
		else
		{
			
			temp = computeInvByt(b_t[b_start]);
			if((a_start == 4) && (b_start == 4))
	                {
				temp = computeDotProd(temp, (a_t[a_start] ^ 0x01));
        	        }
			else
			{
				temp = computeDotProd(temp, a_t[a_start]);
			}
		}

		q[(4 - (b_start - a_start))] = temp;	

		j = a_start;
		for(i = b_start; i < (P_SIZE + 1); i++)
		{
			prod[j] = computeDotProd(b_t[i], temp);
			j++;	
		}
		for(i = a_start; i < (P_SIZE + 1); i++)
		{
			r_t[i] = (a_t[i] ^ prod[i]);
		}
		memset(prod, 0, (P_SIZE + 1));
		memset(a_t, 0, (P_SIZE + 1));
		for(i = 0; i < (P_SIZE + 1); i++)
		{
			a_t[i] = r_t[i];
		}
		if(a_start == 4)
		{
			break;
		}
		for(i = a_start; i < (P_SIZE + 1); i++)
	        {
        	        if(r_t[i] != 0x00)
                	{
                        	break;
                	}
               		else
                	{
                        	a_start++;
                	}
        	}
		memset(r_t, 0, (P_SIZE + 1));
	}
	for(i = 0; i < (P_SIZE + 1); i++)
	{
		r[i] = a_t[i];
	}
}
/*********************************************************************
 Function computeInvPoly: Function to compute the inverse polynomial
			  of the given polynomial using table method
*********************************************************************/
void computeInvPoly(char *p)
{
	int i       = 0;
	int j       = 0;
	int count   = 2;
	int x_index = 2;
	int q_index = 2;
	int r_index = 2;
	int start   = 0;
	int a_start = 0;
	int b_start = 0;
	char quo[IP_SIZE][P_SIZE + 1];
	char rem[IP_SIZE][P_SIZE + 1];
	char aux[IP_SIZE][P_SIZE + 1];
	char x_inter[P_SIZE];
	char q_inter[P_SIZE];
	char o_inter[P_SIZE];
	char out_rev[P_SIZE + 1];
	char out_x[P_SIZE + 1];
	char q[P_SIZE + 1];
	char r[P_SIZE + 1];
	
	memset(quo, 0, (IP_SIZE * (P_SIZE + 1)));
	memset(rem, 0, (IP_SIZE * (P_SIZE + 1)));
	memset(aux, 0, (IP_SIZE * (P_SIZE + 1)));	
	memset(x_inter, 0, P_SIZE);
	memset(q_inter, 0, P_SIZE);
	memset(o_inter, 0, P_SIZE);
	memset(out_rev, 0, (P_SIZE + 1));
	memset(out_x, 0, (P_SIZE + 1));
	memset(q, 0, (P_SIZE + 1));
	memset(r, 0, (P_SIZE + 1));

	//populating the first and the second remainder values
	for(i = 0; i < (P_SIZE + 1); i++)
	{
		rem[0][i] = std[i];
	}
	for(i = 0; i < P_SIZE; i++)
        {
                rem[1][i + 1] = p[i];
        }

	//populating the first and the second aux values
	for(i = 0; i < (P_SIZE + 1); i++)
        {
                aux[0][i] = std_x1[i];
		aux[1][i] = std_x2[i];
        }
	for(i = b_start; i < (P_SIZE + 1); i++)
	{
		if(rem[1][i] != 0x00)
		{
			break;
		}
		else
		{
			b_start++;
		}
	}
	while(1)
	{
		
		count++;

		computePolyDivide(rem, start, &a_start, &b_start, q, r, count);
	
		for(i = 0; i < (P_SIZE + 1); i++)
		{
			rem[r_index][i] = r[i];
			quo[q_index][i] = q[i];
		}
		for(i = 0; i < P_SIZE; i++)
		{
			x_inter[i] = aux[x_index - 1][i + 1];
			q_inter[i] = q[i + 1];
		}

	        computeModProd(x_inter, q_inter, o_inter);
	
		for(i = 0; i < P_SIZE; i++)
		{
			out_x[i + 1] = o_inter[3 - i];
		}
		for(i = 0; i < (P_SIZE + 1); i++)
		{
			out_rev[i]      = out_x[i];
			aux[x_index][i] = (out_rev[i] ^ aux[x_index - 2][i]);
		}
	
		start++;
		r_index++;
		q_index++;
		x_index++;
		for(i = a_start; i < (P_SIZE + 1); i++)
	        {
        	        if(rem[r_index - 2][i] != 0x00)
                	{
                        	break;
                	}
                	else
                	{
                        	a_start++;
                	}
        	}
		for(i = b_start; i < (P_SIZE + 1); i++)
	        {
        	        if(rem[r_index - 1][i] != 0x00)
                	{
                        	break;
                	}
                	else
                	{
        	                b_start++;
                	}
        	}

		if(r[1] == 0x00 && r[2] == 0x00 && r[3] == 0x00 && r[4] == 0x01)
		{
			for(i = 0; i < count; i++)
        		{
                		fprintf(stdout, "i=%d, ", i + 1);
                		fprintf(stdout, "rem[i]=");
                		for(j = 0; j < P_SIZE; j++)
                		{
                        		fprintf(stdout, "{%02x}", (rem[i][j + 1] & 0xff));
                		}
                		fprintf(stdout, ", ");
                		fprintf(stdout, "quo[i]=");
                		for(j = 0; j < P_SIZE; j++)
                		{
                        		fprintf(stdout, "{%02x}", (quo[i][j + 1] & 0xff));
                		}
                		fprintf(stdout, ", ");
                		fprintf(stdout, "aux[i]=");
                		for(j = 0; j < P_SIZE; j++)
                		{
                        		fprintf(stdout, "{%02x}", (aux[i][j + 1] & 0xff));
                		}
                		fprintf(stdout, "\n");

        		}
        		fprintf(stdout, "Multiplicative inverse of ");
        		for(i = 0; i < P_SIZE; i++)
        		{
                		fprintf(stdout, "{%02x}", (p[i] & 0xff));
       			}
        		fprintf(stdout, " is ");
        		for(i = 0; i < P_SIZE; i++)
        		{
                		fprintf(stdout, "{%02x}", (aux[x_index - 1][i + 1] & 0xff));
        		}
        		fprintf(stdout, "\n");	
			break;
		}
		if(r[1] == 0x00 && r[2] == 0x00 && r[3] == 0x00 && r[4] == 0x00)
                {
			for(i = 0; i < count; i++)
                        {
                                fprintf(stdout, "i=%d, ", i + 1);
                                fprintf(stdout, "rem[i]=");
                                for(j = 0; j < P_SIZE; j++)
                                {
                                        fprintf(stdout, "{%02x}", (rem[i][j + 1] & 0xff));
                                }
                                fprintf(stdout, ", ");
                                fprintf(stdout, "quo[i]=");
                                for(j = 0; j < P_SIZE; j++)
                                {
                                        fprintf(stdout, "{%02x}", (quo[i][j + 1] & 0xff));
                                }
                                fprintf(stdout, ", ");
                                fprintf(stdout, "aux[i]=");
                                for(j = 0; j < P_SIZE; j++)
                                {
                                        fprintf(stdout, "{%02x}", (aux[i][j + 1] & 0xff));
                                }
                                fprintf(stdout, "\n");

                        }

			for(i = 0; i < P_SIZE; i++)
                        {
                                fprintf(stdout, "{%02x}", (p[i] & 0xff));
                        }

			fprintf(stdout, " does not have a multiplicative inverse\n");
                        break;
                }
		memset(q, 0, (P_SIZE + 1));
        	memset(r, 0, (P_SIZE + 1));
		memset(out_x, 0, (P_SIZE + 1));
		memset(out_rev, 0, (P_SIZE + 1));
		memset(x_inter, 0, P_SIZE);
		memset(q_inter, 0, P_SIZE);
		memset(o_inter, 0, P_SIZE);
		
	}

}
/*******************************************************************
 Function validateInversePoly: Function to perform validations on 
			       inverse poly and compute its inverse 
		               by calling computeInvPoly()
********************************************************************/
void validateInversePoly(int argc, char *argv[])
{
	int i       = 0;
	char *token = NULL;
	char *save  = NULL;
	char *temp  = NULL;
	char poly[P_SIZE];

	memset(poly, 0, P_SIZE);

	if(argc < 3)
	{
		fprintf(stderr, "\r\nerror: malformed command\r\nusage: hw5 inverse -p=poly\r\n");
                exit(-1);
	}
	token       = strtok_r(argv[2], "=", &save);
        if(!save || !strlen(save))
        {
        	fprintf(stderr, "\r\nerror: malformed command\r\nusage: hw5 inverse -p=poly\r\n");
                exit(-1);
        }
        if(strcmp(token, "-p"))
        {
                fprintf(stderr, "\r\nerror: malformed command\r\nusage: hw5 inverse -p=poly\r\n");
                exit(-1);
        }
	if(strlen(save) != (P_SIZE * 2))
	{
		fprintf(stderr, "\r\nerror: malformed command polynomial: -p has to be 4 bytes in length\r\n");
                exit(-1);
	}
	temp = save;
	for(i = 0; i < strlen(temp); i++)
	{
		if(!((temp[i] >= '0' && temp[i] <= '9') || (temp[i] >= 'a' && temp[i] <= 'f')))
        	{
        		fprintf(stderr, "\r\nerror: -p polynomial can contain only hexadecimal values\r\n");
        		exit(-1);
        	}
	}

	computeHex(save, poly);
	computeInvPoly(poly);
	
}
