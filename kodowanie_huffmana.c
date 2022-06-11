#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MODEL_ARR_LEN 256

/*char* inputFile = "test.txt";
char* outputFile = "wyjscie.txt";
char* outputFileCodeTab = "tree.code";
char* outputFileTreeF = "tree.graf";
char* outputFileModel = "tree.model";*/
char inputFile[50];
char outputFile[50];
char outputFileCodeTab[50];
char outputFileTreeF[50];
char outputFileModel[50];


typedef struct HuffmanNode {
    int symbol;
    int frequency;
    //struct HuffmanTreeNode *left, *right, *parent;
    int left, right, parent;
}HuffmanNode;

//HuffmanNode* huffmanModelArray = NULL;

int CompareHuffmanNodes(const void* item1, const void* item2) {
    HuffmanNode* node1 = (HuffmanNode*)item1;
    HuffmanNode* node2 = (HuffmanNode*)item2;
    //printf("Node 1: %d : %d \n", (*node1).symbol, (*node1).frequency);
    //printf("Node 2: %d : %d \n", node2->symbol, node2->frequency);
    int compareResult = (node1->frequency - node2->frequency);
    if (compareResult == 0) {
        compareResult = (node1->symbol - node2->symbol);
    }
    return -compareResult;
}

void SortHuffmanModel(int modelArrayLength, struct HuffmanNode *huffmanModelArray) {
    qsort(huffmanModelArray, modelArrayLength, sizeof(HuffmanNode), CompareHuffmanNodes);
}

// This functions returns 0 if there is no symbol like this in buffor and if there is some like that it returns it position in array
int IsItInArray(HuffmanNode huffmanModelArray[MODEL_ARR_LEN], int buffer, int iter) {
    int i;
    for (i = 0; i < iter; i++) {
        if (huffmanModelArray[i].symbol == buffer) {
            return i + 1;
        }
    }
    return 0;
}

struct HuffmanNode *GenerateModelFromFile(struct HuffmanNode *huffmanModelArray, int* it, int* ch) {
    FILE* inputFileHandle = fopen(inputFile, "rb");
    int readCount = 0;
    unsigned char buffer[1];
    int readBytesLength = 1;
    int iter = 0;
    int temp = 0, checkSum = 0;
    //huffmanModelArray = (HuffmanNode*)malloc(sizeof(HuffmanNode) * 1);
    while (readCount = fread(buffer, sizeof(unsigned char), readBytesLength, inputFileHandle)) {
        readCount += 1;
        temp = IsItInArray(huffmanModelArray, buffer[0], iter);
        if (temp != 0) {
            huffmanModelArray[temp - 1].frequency += 1;
        }
        else {
            huffmanModelArray = (HuffmanNode*)realloc(huffmanModelArray, sizeof(HuffmanNode) * ((iter)+1));
            huffmanModelArray[iter].symbol = (int)buffer[0];
            huffmanModelArray[iter].parent = -1;
            huffmanModelArray[iter].left = -1;
            huffmanModelArray[iter].right = -1;
            huffmanModelArray[iter].frequency = 1;
            iter++;
        }
        checkSum += 1;
    }
    fclose(inputFileHandle);
    //printf("Przed sortowaniem:\n");
    int i;
    //for (i = 0; i < iter; i++) {
      //  printf("%d:%d\n", huffmanModelArray[i].symbol, huffmanModelArray[i].frequency);
    //}
    *it = iter;
    *ch = checkSum;
    return huffmanModelArray;
}

void WriteModelToFile(int iter, struct HuffmanNode *huffmanModelArray) {
    FILE* outputFileHandle;
    if ((outputFileHandle = fopen(outputFileModel, "w")) == NULL) {
        printf("Otwieranie pliku do zapisu %s nie powiodlo sie\n", outputFile);
    }
    else {
        fprintf(outputFileHandle, "%d\n", iter);
        int i;
        for (i = 0; i < iter; i++) {
            fprintf(outputFileHandle, "%d:%d\n", huffmanModelArray[i].symbol, huffmanModelArray[i].frequency);
        }
    }
    fclose(outputFileHandle);
}

void FindSmallestNode(int *lN, int *rN, struct HuffmanNode *huffmanModelArray, int iter) {
    int i, leftNodeMin = 2147483647, rightNodeMin = 2147483647, leftNode, rightNode;
    for (i = 0; i < iter; i++) {
        if (huffmanModelArray[i].frequency <= leftNodeMin && huffmanModelArray[i].parent == -1) //patrze tylko tam gdzie nie ma rodzica
        {
            leftNode = i;
            leftNodeMin = huffmanModelArray[i].frequency;
        }
    }
    for (i = 0; i < iter; i++) {
        if (huffmanModelArray[i].frequency <= rightNodeMin && huffmanModelArray[i].parent == -1 && i != leftNode)
        {
            rightNode = i;
            rightNodeMin = huffmanModelArray[i].frequency;
        }
    }
    //printf("\n",i);
    if (leftNode == rightNode)
    	printf("\n-----------Blad----------\n");
    *lN = leftNode;
    *rN = rightNode;
}

struct HuffmanNode *GenerateHuffmanTreeFromModel(struct HuffmanNode *huffmanModelArray, int iter, int checkSum, int *root) {
    int lastNode = iter-1, leftNode, rightNode, nodeSpecial = 1001;
    while(huffmanModelArray[iter-1].frequency != checkSum)
    {
        FindSmallestNode(&leftNode, &rightNode, huffmanModelArray, iter);
        huffmanModelArray = (struct HuffmanNode*)realloc(huffmanModelArray, sizeof(HuffmanNode) * ((iter)+1));
        huffmanModelArray[iter].symbol = nodeSpecial;
        huffmanModelArray[iter].left = leftNode;
        huffmanModelArray[iter].right = rightNode;
        huffmanModelArray[iter].frequency = huffmanModelArray[leftNode].frequency + huffmanModelArray[rightNode].frequency;
        huffmanModelArray[iter].parent = -1;
        huffmanModelArray[leftNode].parent = iter;
        huffmanModelArray[rightNode].parent = iter;
        //printf("w funkcji %d\n", iter/*huffmanModelArray[iter].frequency*/);
        iter++;
        nodeSpecial++;
    }
    //printf("pytanie %d ", huffmanModelArray[iter-1].right);
    iter = iter-1;
    *root = iter;
	//printf("w funkcji %d\n", huffmanModelArray[iter].frequency);
    return huffmanModelArray;
}

void GenerateCodeTableFromTree(int root, int arr[], int top, struct HuffmanNode *huffmanModelArray, FILE *outFileH) {
    if (huffmanModelArray[root].left != -1) {
        arr[top] = 0;
        GenerateCodeTableFromTree(huffmanModelArray[root].left, arr, top + 1, huffmanModelArray,outFileH);
    }

    if (huffmanModelArray[root].right != -1) {
        arr[top] = 1;
        GenerateCodeTableFromTree(huffmanModelArray[root].right, arr, top + 1, huffmanModelArray,outFileH);
    }

    if (huffmanModelArray[root].left == -1 && huffmanModelArray[root].right == -1) {
        //fprintf(outputFileCodeTable, "%d-", root->symbol);
        //printf("%d-", huffmanModelArray[root].symbol);
        fprintf(outFileH,"%d-", huffmanModelArray[root].symbol);
        int i;
        for (i = 0; i < top; i++) {
        	//printf("%d",arr[i]);
        	fprintf(outFileH, "%d", arr[i]);
		}
        //printf("\n");
        fprintf(outFileH,"\n");
    }
}

void WriteHuffmanTreeToFile(struct HuffmanNode *huffmanModelArray, int size) {
	FILE* outputFileH;
	if ((outputFileH = fopen(outputFileTreeF, "w")) == NULL) {
        printf("Otwieranie pliku do zapisu %s nie powiodlo sie\n", outputFile);
    } 
    int i;
	for (i = 0; i <= size; i++)
	{
		if (huffmanModelArray[i].symbol >= 1000) {
			//printf("#%d", huffmanModelArray[i].symbol - 1000);
			fprintf(outputFileH,"#%d", huffmanModelArray[i].symbol - 1000);
		}
		else {
			//printf("%d", huffmanModelArray[i].symbol);
			fprintf(outputFileH,"%d", huffmanModelArray[i].symbol);
		}
		//printf(":%d", huffmanModelArray[i].frequency);
		fprintf(outputFileH, ":%d", huffmanModelArray[i].frequency);
		
		if (huffmanModelArray[huffmanModelArray[i].left].symbol >= 1000 && huffmanModelArray[i].left != -1) {
			//printf("\tchildLeft:#%d", huffmanModelArray[huffmanModelArray[i].left].symbol - 1000);
			fprintf(outputFileH,"\tchildLeft:#%d", huffmanModelArray[huffmanModelArray[i].left].symbol - 1000);
		}
		else if (huffmanModelArray[huffmanModelArray[i].left].symbol == 0 || huffmanModelArray[i].left == -1) {
			//printf("\tchildLeft:-");
			fprintf(outputFileH, "\tchildLeft:-");
		}
		else {
			//printf("\tchildLeft:%d", huffmanModelArray[huffmanModelArray[i].left].symbol);
			fprintf(outputFileH, "\tchildLeft:%d", huffmanModelArray[huffmanModelArray[i].left].symbol);
		}
		
		if (huffmanModelArray[huffmanModelArray[i].right].symbol >= 1000 && huffmanModelArray[i].right != -1) {
			//printf("\tchildRight:#%d", huffmanModelArray[huffmanModelArray[i].right].symbol - 1000);
			fprintf(outputFileH, "\tchildRight:#%d", huffmanModelArray[huffmanModelArray[i].right].symbol - 1000);
		}
		else if (huffmanModelArray[huffmanModelArray[i].right].symbol == 0 || huffmanModelArray[i].right == -1) {
			//printf("\tchildRight:-");
			fprintf(outputFileH,"\tchildRight:-");
		}
		else {
			//printf("\tchildRight:%d", huffmanModelArray[huffmanModelArray[i].right].symbol);
			fprintf(outputFileH,"\tchildRight:%d", huffmanModelArray[huffmanModelArray[i].right].symbol);
		}

		if (i == size) {
			//printf("\tParent:-");
			fprintf(outputFileH,"\tParent:-");
		}
		else {
			if (huffmanModelArray[huffmanModelArray[i].parent].symbol >= 1000) {
				//printf("\tParent:#%d", huffmanModelArray[huffmanModelArray[i].parent].symbol - 1000);
				fprintf(outputFileH,"\tParent:#%d", huffmanModelArray[huffmanModelArray[i].parent].symbol - 1000);
			}
			else if (huffmanModelArray[huffmanModelArray[i].parent].symbol == -1) {
				//printf("\tParent:-");
				fprintf(outputFileH, "\tParent:-");
			}
			else {
				//printf("\tParent:%d", huffmanModelArray[huffmanModelArray[i].parent].symbol);
				fprintf(outputFileH,"\tParent:%d", huffmanModelArray[huffmanModelArray[i].parent].symbol);
			}
		}
		//printf("\n");
		fprintf(outputFileH, "\n");
	}
	fclose(outputFileH);
}

void WriteHuffmanTreeToFileTemp(struct HuffmanNode *huffmanModelArray, int size) {
	FILE* outputFileH;
	if ((outputFileH = fopen("porownanie.txt", "w")) == NULL) {
        printf("Otwieranie pliku do zapisu %s nie powiodlo sie\n", outputFile);
    } 
    int i;
	for (i = 0; i <= size; i++)
	{
		if (huffmanModelArray[i].symbol >= 1000) {
			//printf("#%d", huffmanModelArray[i].symbol - 1000);
			fprintf(outputFileH,"#%d", huffmanModelArray[i].symbol - 1000);
		}
		else {
			//printf("%d", huffmanModelArray[i].symbol);
			fprintf(outputFileH,"%d", huffmanModelArray[i].symbol);
		}
		//printf(":%d", huffmanModelArray[i].frequency);
		fprintf(outputFileH, ":%d", huffmanModelArray[i].frequency);
		
		if (huffmanModelArray[huffmanModelArray[i].left].symbol >= 1000 && huffmanModelArray[i].left != -1) {
			//printf("\tchildLeft:#%d", huffmanModelArray[huffmanModelArray[i].left].symbol - 1000);
			fprintf(outputFileH,"\tchildLeft:#%d", huffmanModelArray[huffmanModelArray[i].left].symbol - 1000);
		}
		else if (huffmanModelArray[huffmanModelArray[i].left].symbol == 0 || huffmanModelArray[i].left == -1) {
			//printf("\tchildLeft:-");
			fprintf(outputFileH, "\tchildLeft:-");
		}
		else {
			//printf("\tchildLeft:%d", huffmanModelArray[huffmanModelArray[i].left].symbol);
			fprintf(outputFileH, "\tchildLeft:%d", huffmanModelArray[huffmanModelArray[i].left].symbol);
		}
		
		if (huffmanModelArray[huffmanModelArray[i].right].symbol >= 1000 && huffmanModelArray[i].right != -1) {
			//printf("\tchildRight:#%d", huffmanModelArray[huffmanModelArray[i].right].symbol - 1000);
			fprintf(outputFileH, "\tchildRight:#%d", huffmanModelArray[huffmanModelArray[i].right].symbol - 1000);
		}
		else if (huffmanModelArray[huffmanModelArray[i].right].symbol == 0 || huffmanModelArray[i].right == -1) {
			//printf("\tchildRight:-");
			fprintf(outputFileH,"\tchildRight:-");
		}
		else {
			//printf("\tchildRight:%d", huffmanModelArray[huffmanModelArray[i].right].symbol);
			fprintf(outputFileH,"\tchildRight:%d", huffmanModelArray[huffmanModelArray[i].right].symbol);
		}

		if (i == size) {
			//printf("\tParent:-");
			fprintf(outputFileH,"\tParent:-");
		}
		else {
			if (huffmanModelArray[huffmanModelArray[i].parent].symbol >= 1000) {
				//printf("\tParent:#%d", huffmanModelArray[huffmanModelArray[i].parent].symbol - 1000);
				fprintf(outputFileH,"\tParent:#%d", huffmanModelArray[huffmanModelArray[i].parent].symbol - 1000);
			}
			else if (huffmanModelArray[huffmanModelArray[i].parent].symbol == -1) {
				//printf("\tParent:-");
				fprintf(outputFileH, "\tParent:-");
			}
			else {
				//printf("\tParent:%d", huffmanModelArray[huffmanModelArray[i].parent].symbol);
				fprintf(outputFileH,"\tParent:%d", huffmanModelArray[huffmanModelArray[i].parent].symbol);
			}
		}
		//printf("\n");
		fprintf(outputFileH, "\n");
	}
	fclose(outputFileH);
}

typedef struct CodeDock {
	int sign;
	int current;
	unsigned int code;
}CodeDock;

struct CodeDock *CodeDockCreate(int *toPass) {
	FILE *inFileH;
	CodeDock *cDock;
	unsigned char bufor[1];
	int iter = 0, mode = 1, count = 0, first = 1;
	if((inFileH = fopen(outputFileCodeTab, "r")) == NULL) {
		printf("Nie znaleziono pliku\n");
		return NULL;
	}
	cDock = (CodeDock*)malloc(sizeof(CodeDock));
	cDock[iter].sign = 0;
	while (fread(bufor, sizeof(unsigned char), 1, inFileH)) {
		//printf("%c", bufor[0]);
		if (bufor[0] == '-') {
			first = 0;
			cDock[iter].code = 0x00;
			cDock[iter].current = 0;
			continue;
		}
		if (bufor[0] == '\n') {
			//printf("Code Table: %d 0x%X\n", cDock[iter].sign, cDock[iter].code);
			iter++;
			cDock = (CodeDock*)realloc(cDock, sizeof(CodeDock)*(iter+1));
			cDock[iter].sign = 0;
			first = 1;
			continue;
		}
		if (first) {
			cDock[iter].sign = cDock[iter].sign*10 + bufor[0] - 48;
			//printf("buffor: %c to to: %d ", bufor[0], cDock[iter].sign);
		}
		else {
			cDock[iter].code = cDock[iter].code << 1;
			cDock[iter].code += bufor[0] - 48;	
			cDock[iter].current += 1;	
		}
	}
	*toPass = iter;
	fclose(inFileH);
	return cDock;
}

unsigned int CodeOfSign(int sign, struct CodeDock *code, int *current, int size, FILE *outFileHTemp) {
	int i, c;
	for (i = 0; i < size; i++) {
		if (sign == code[i].sign) {
			*current = code[i].current;
			return code[i].code;
		}
		//printf("code: 0x%X", code[i].code);
	}
	fprintf(outFileHTemp, "%c", code[i].sign);
	printf("Falsz\n");
	return 0x00;
}


void WriteCompressedFile(CodeDock *codeTable, int limes) {
	FILE *input, *output;
	if((input = fopen(inputFile, "rb")) == NULL) {
		printf("Nie znaleziono pliku z danymi!\n");
	}
	if((output = fopen(outputFile, "wb")) == NULL) {
		printf("Nie udalo sie utworzyc pliku!\n");
	}
	unsigned char bufor[1], bit, out = 0x00, mask;
	//unsigned int temp;
	int i,j, shift=8, tableShift;
	while(fread(bufor, sizeof(unsigned char), 1, input)) {
		for (i = 0; i < limes; i++) {
			if (codeTable[i].sign == bufor[0]) {
				tableShift = codeTable[i].current - 1;
				for (j = 0; j < codeTable[i].current; j++) {
					shift--;
					if ( ((codeTable[i].code >> (tableShift - j)) & 0x00000001 ) != 0x00) {
						//printf("1"); 
						mask = 0x01;
					}
					else {
						mask = 0x00;
						//printf("0");
					}
					mask = mask << shift;
					//printf("shift: %d", shift);
					out = out ^ mask;
					//tableShift--;
					if (shift == 0) {
						shift = 8;
						fprintf(output, "%c", out);
						//printf(" %X ",out);
						out = 0x00;
					}					
				}
			}
		}
	}
	if (shift != 0) {
		fprintf(output, "%c", out);
	}
	fclose(input);
	fclose(output);
}

/*void WriteCompressedFile(CodeDock *codeTable, int readCount) {
	FILE *input, *output;
	if((input = fopen(inputFile, "rb")) == NULL) {
		printf("Nie znaleziono pliku z danymi!\n");
		//return 1;
	}
	if((output = fopen(outputFile, "wb")) == NULL) {
		printf("Nie udalo sie utworzyc pliku!\n");
		//return 1;
	}
	unsigned char buffer[1], temp = 0;
	int i, x, temp_size = 0;
	while(fread(buffer, sizeof(unsigned char), 1, input)) {
		for(i=0;i<readCount;i++) {
			if(codeTable[i].sign == buffer[0]) {
				if(temp_size + codeTable[i].current < 8) {
					temp = temp << codeTable[i].current;
					temp += codeTable[i].code;
					//printf("Code: 0x%X\n", codeTable[i].sign);
					temp_size += codeTable[i].current;
				} else if(temp_size + codeTable[i].current < 16) {
					temp = temp << 8 - temp_size;
					temp += codeTable[i].code >> codeTable[i].current + temp_size - 8;
					fprintf(output, "%c", temp);
					temp = codeTable[i].code;
					//printf("Code: 0x%X\n", codeTable[i].sign);
					temp_size = codeTable[i].current + temp_size - 8;
				} else {
					temp = temp << 8 - temp_size;
					printf("Code: 0x%X\n", codeTable[i].sign);
					temp += codeTable[i].code >> codeTable[i].current + temp_size - 8;
					fprintf(output, "%c", temp);
					temp = codeTable[i].code >> codeTable[i].current + temp_size - 16;
					fprintf(output, "%c", temp);
					temp = codeTable[i].code;
					temp_size = codeTable[i].current + temp_size - 16;
				}
			}
		}
	}
	if(temp_size) {
		temp = temp << 8 - temp_size;
		fprintf(output, "%c", temp);
	}
	fclose(input);
	fclose(output);
	//return 0;
}*/

struct HuffmanNode *CreateNotConfiguredList(FILE *inFileH, int *ret) {
	unsigned char bufor[1];//, *fullFile = (unsigned char*)malloc(sizeof(unsigned char));
    int mode = 1,  iter = 0, temp = 0, ofset = 0;
	struct HuffmanNode *code = (HuffmanNode*)malloc(sizeof(HuffmanNode)); 
    while (fread(bufor, sizeof(unsigned char), 1, inFileH)) {
    	//printf("%c", bufor[0]);
    	if (mode == 1) {
    		if (bufor[0] == ':') {
    			code[iter].symbol = temp + ofset;
    			//printf("symbol: %d",code[iter].symbol);
    			//printf("symbol: %d",code[iter].symbol);
    			mode = 2; temp = 0; ofset = 0;
    			continue;
			} 
			else if (bufor[0] == '#') {
				ofset = 1000;
			}
			else {
				temp = temp*10 + bufor[0] - 48;
				//printf("symbol: %d",temp);
			}
		}
		else if (mode == 2) {
			if (bufor[0] == '\t') {
				code[iter].frequency = temp;
				//printf(" frequency: %d",code[iter].frequency);
				mode = 3; temp = 0;
				continue; 
			}
			else {
				temp = temp*10 + bufor[0] - 48;
			}
		}
		else if (mode == 3) {
			if (bufor[0] == '\t') {
				if (temp == -3) {
					code[iter].left = -1;
				}
				else {
					code[iter].left = temp + ofset;
				}
				//printf(" left: %d",code[iter].left);
				mode = 4; temp = 0; ofset = 0;
				continue;
			}
			else if ((bufor[0] >= 65 && bufor[0] <= 122) || bufor[0] == ':') {
				continue;
			}
			else if (bufor[0] == '#') {
				ofset = 1000;
			}
			else {
				//printf("tempprzed: %d",temp);
				temp = temp*10 + bufor[0] - 48;
				//printf(" buforL: %d temp: %d", bufor[0], temp);
			}
		}
		else if (mode == 4) {
			if (bufor[0] == '\t') {
				if (temp == -3) {
					code[iter].right = -1;
				}
				else {
					code[iter].right = temp + ofset;
				}
				//printf(" right: %d",code[iter].right);
				mode = 5; temp = 0; ofset = 0;
				continue;
			}
			else if ((bufor[0] >= 65 && bufor[0] <= 122) || bufor[0] == ':') {
				continue;
			}
			else if (bufor[0] == '#') {
				ofset = 1000;
			}
			else {
				temp = temp*10 + bufor[0] - 48;
			}
		}
		else if (mode == 5) {
			if ((int)bufor[0] == 10) {
				if (temp == -3) {
					code[iter].parent = -1;
				}
				else {
					code[iter].parent = temp + ofset;
				}
				//printf(" parent: %d\n",code[iter].parent);
				mode = 1; temp = 0; ofset = 0;
				iter++;
				code = (HuffmanNode*)realloc(code, sizeof(HuffmanNode)*(iter+2));
				continue;
			} 
			else if ((bufor[0] >= 65 && bufor[0] <= 122) || bufor[0] == ':') {
				continue;
			}
			else if (bufor[0] == '#') {
				//printf(" jest hasz ");
				ofset = 1000;
				continue;
			}
			else {
				temp = temp*10 + bufor[0] - 48;
				//printf(" temp: %d", temp);
			}
		}
	}
	*ret = iter;
	return code;
}

int getNodeIndex(struct HuffmanNode *code, int size,int symbol) {
	int i;
	for (i = 0; i < size; i++) {
		if (symbol == code[i].symbol) {
			return i;
		}
	}
	return 0;
}

struct HuffmanNode *ConfigureListToTree(struct HuffmanNode *code, int size) {
	int i, index;
	for (i = 0; i < size; i++) {
		if (code[i].left > 0) {
			index = getNodeIndex(code, size, code[i].left);
			code[i].left = index;
		}
		if (code[i].right > 0) {
			index = getNodeIndex(code, size, code[i].right);
			code[i].right = index;
		}
		if (code[i].parent > 0) {
			index = getNodeIndex(code, size, code[i].parent);
			code[i].parent = index;
		}
	}
	return code;
}

struct HuffmanNode *ReadHuffmanTreeFromFile(int *length) {
	FILE *inFileH;
	if ((inFileH = fopen(outputFileTreeF, "r")) == NULL) {
        printf("Wystapil blad - niektore pliki sa niedostepne\n", outputFileTreeF);
    }
    struct HuffmanNode *code;
    int size, i;
    code = CreateNotConfiguredList(inFileH, &size);
	*length = size;
	fclose(inFileH);
	return ConfigureListToTree(code, size);
}

unsigned char decodeSign(int iter, struct HuffmanNode *code) {
	return code[iter].symbol;
}

void WriteDecompressedFile(HuffmanNode *code, int limes, int root) {
	FILE *inFileH;
	if ((inFileH = fopen(inputFile, "rb")) == NULL) {
	    printf("Wystapil blad - niektore pliki sa niedostepne\n");
	}
	FILE *outFileH;
	if ((outFileH = fopen(outputFile, "wb")) == NULL) {
	    printf("Wystapil blad - niektore pliki sa niedostepne\n", outputFileCodeTab);
	}
	unsigned char bufor[1], mask = 0x00, temp = 0x00;
	int iter = 0, node = root;
	while (iter < limes) {
		if (mask == 0x00) {
			fread(bufor, sizeof(unsigned char), 1, inFileH);
			mask = 0x80;
		}
		temp = bufor[0] & mask;
		if (temp) {
			node = code[node].right;
		}
		else {
			node = code[node].left;
		}
		mask = mask >> 1;
		if (code[node].left == -1 && code[node].right == -1) {
			fprintf(outFileH, "%c", code[node].symbol);
			node = root;
			iter++;
		}
	}
	//printf("%d ",iter);
	fclose(inFileH);
	fclose(outFileH);
}


int main() {
	int option = 1;
    while (1)
    {
    	printf("Tik Laboratorium nr 3\n1. Kompresjia\n2. Dekompresja\n3.Wyjscie\n-->");
    	scanf("%d", &option);
    	if (option == 1) {
    		printf("Plik wejsciowy: ");
    		scanf("%s", inputFile);
    		printf("Plik wyjsciowy: ");
    		scanf("%s", outputFile);
    		strcpy(outputFileCodeTab, outputFile);
    		strcat(outputFileCodeTab, ".code");
    		strcpy(outputFileTreeF, outputFile);
    		strcat(outputFileTreeF, ".graf");
    		strcpy(outputFileModel, outputFile);
    		strcat(outputFileModel, ".model");
    		
    		int iter, checkSum, root;
		    struct HuffmanNode* huffmanModelArray = (HuffmanNode*)malloc(sizeof(HuffmanNode) * 1);
		    huffmanModelArray = GenerateModelFromFile(huffmanModelArray, &iter, &checkSum);
		    //printf("%d", iter);
		    SortHuffmanModel(iter, huffmanModelArray);
		    //printf("Po sortowaniu: checkSum: %d\n", checkSum);
		    int i;
		    //for (i = 0; i < iter; i++) {
		      //  printf("%d:%d\n", huffmanModelArray[i].symbol, huffmanModelArray[i].frequency);
		    //}
		    WriteModelToFile(iter, huffmanModelArray);
		    printf("\n\nModel zrodla zostal zapisany do pliku.\n");
		    huffmanModelArray =  GenerateHuffmanTreeFromModel(huffmanModelArray, iter, checkSum, &root);
		    int* arr;
		    arr = (int*)malloc(sizeof(int) * iter);
		    int top = 0;
		    //for (i = 0; i <= root; i++) {
		      //  printf("%d parent: %d, symbol: %d,lewy: %d, prawy: %d\n",i, huffmanModelArray[i].frequency, huffmanModelArray[i].symbol, huffmanModelArray[i].left, huffmanModelArray[i].right);
		    ///}
		    WriteHuffmanTreeToFile(huffmanModelArray, root);
		    printf("Drzewo kodowania + ");
		    FILE *fileForCode;
		    if ((fileForCode = fopen(outputFileCodeTab, "w")) == NULL) {
		        printf("Otwieranie pliku do zapisu %s nie powiodlo sie\n", outputFileCodeTab);
		    }
		    GenerateCodeTableFromTree(root, arr, top, huffmanModelArray, fileForCode);
		    printf("tablica kodowa zostaly zapisane do pliku.\n");
		    //printf("ok root: %d ", root);
		    //WriteDecompressedFile(huffmanModelArray, huffmanModelArray[root].frequency, root);
		    free(huffmanModelArray);
			fclose(fileForCode);
			int sizeCodeTable;
			struct CodeDock *code = CodeDockCreate(&sizeCodeTable);
			int j, calc = 0;
			/*for(i = 0; i < sizeCodeTable; i++) {
				printf("%c-0x%X %d", code[i].sign, code[i].code, code[i].current);
				putc('\n', stdout);
			}*/
			//printf("ok2");
			WriteCompressedFile(code, sizeCodeTable);
			printf("Kompresja zakonczona\n\n\n");
			//printf("ok4");
			free(code);
		}
		else if (option == 2){
			printf("Plik wejsciowy: ");
    		scanf("%s", inputFile);
    		printf("Plik wyjsciowy: ");
    		scanf("%s", outputFile);
    		strcpy(outputFileTreeF, "");
    		strcpy(outputFileTreeF, inputFile);
    		strcat(outputFileTreeF, ".graf");
			struct HuffmanNode* huffmanModelArray = (HuffmanNode*)malloc(sizeof(HuffmanNode) * 1);
			int size;
			huffmanModelArray = ReadHuffmanTreeFromFile(&size);
			int i;
			/*for (i = 0; i < size; i++) {
		        printf("%d:%d lewy: %d, prawy: %d parent: %d\n", huffmanModelArray[i].symbol, huffmanModelArray[i].frequency, huffmanModelArray[i].symbol, huffmanModelArray[i].left, huffmanModelArray[i].right, huffmanModelArray[i].parent);
			}*/
		    size -= 1;
		    WriteHuffmanTreeToFileTemp(huffmanModelArray, size);
		    //struct HuffmanNode *code, FILE *inFileH, FILE *outFileH, int limes, int root
		    WriteDecompressedFile(huffmanModelArray, huffmanModelArray[size].frequency, size);
		    //WriteDecompressedFile(huffmanModelArray, huffmanModelArray[size].frequency, size);
		    //printf("%d", huffmanModelArray[size].parent);
		    printf("\n\nDekompresja zakonczona\n\n\n");
			free(huffmanModelArray);
		}
		else {
			return 0;
		}
	}
    return 0;
}
