#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "qrcodegen.c"

typedef struct {
	unsigned char B;
	unsigned char G;
	unsigned char R;
} pixel;

static int makeQR(const char *text,uint8_t qrcode[]);
void makeBMP(uint8_t qrcode[],int mida,char file[]);
void takeTextFile(char *fileName, char []);

void main(int argc,char *argv[]){
	uint8_t qrcode[qrcodegen_BUFFER_LEN_MAX];
	char str[5000];
	char fileIn[5000];
	char fileOut[5000];
	char strAux[100]="";
	int i;
	
	if(argc==4){
		strcpy(fileOut,argv[3]);
		for(i=strlen(fileOut)-1;fileOut[i]!='.'&&i!=0;i--){
			strAux[strlen(strAux)-1]=fileOut[i];
		}
		strAux[strlen(strAux)-1]='\0';
		if(i==0||(i!=0&&strcmp(strAux,"bmp"))){
			if(i==0){
				strcat(fileOut,".bmp");
			}
			strcpy(fileIn,argv[1]);
			int mida=atoi(argv[2]);
		
			takeTextFile(fileIn,str);
			makeQR(str,qrcode);
			makeBMP(qrcode,mida,fileOut);
			printf("QR generated succesfully");
		}
	/*printf("introdueix el text: ");
	scanf("%s",str);*/
	
	/*printf("introdueix la mida: ");
	fflush(stdin);
	scanf("%i",&mida);
	printf("introdueix el nom del fitxer a generar: ");
	scanf("%s",file);
	printf("%s\n",str);*/
	/*makeQR(&str,qrcode);
	*/
	}
	else{
		printf("%s    textFile    wishedQRsize    outputBMPfileName",argv[0]);
	}
	
}

void takeTextFile(char *fileName,char text[]){
	FILE *file;
	char car;
	int i=0;
	file=fopen(fileName,"r");
	if(file!=NULL){
		while((car=fgetc(file))!=EOF){
			text[i]=car;
			i++;
		}
		text[i]='\0';
		fclose(file);
	}
}

static int makeQR(const char *text,uint8_t qrcode[]) {
	enum qrcodegen_Ecc errCorLvl = qrcodegen_Ecc_LOW;
	uint8_t tempBuffer[qrcodegen_BUFFER_LEN_MAX];
	bool ok = qrcodegen_encodeText(text, tempBuffer, qrcode, errCorLvl,qrcodegen_VERSION_MIN, qrcodegen_VERSION_MAX, qrcodegen_Mask_AUTO, true);
	
	if (ok){
		return 1;
	}
	return 0;
}

//supply an array of pixels[height][width] <- notice that height comes first
int writeBMP(char filename[], unsigned int width, unsigned int height, pixel pixels[height][width]) {
	FILE *fd; 
	static unsigned char header[54] = {66,77,0,0,0,0,0,0,0,0,54,0,0,0,40,0,0,0,0,0,0,0,0,0,0,0,1,0,24}; //rest is zeroes
	unsigned int pixelBytesPerRow = width*sizeof(pixel);
	unsigned int paddingBytesPerRow = (4-(pixelBytesPerRow%4))%4;
	unsigned int* sizeOfFileEntry = (unsigned int*) &header[2];
	*sizeOfFileEntry = 54 + (pixelBytesPerRow+paddingBytesPerRow)*height;  
	unsigned int* widthEntry = (unsigned int*) &header[18];    
	*widthEntry = width;
	unsigned int* heightEntry = (unsigned int*) &header[22];    
	*heightEntry = height;
	int i;
	fd= fopen(filename, "wb");
	fwrite(header,sizeof(header[0]),54,fd);
	static unsigned char zeroes[3] = {0,0,0}; //for padding    
	int row;
	for (row = 0; row < height; row++) {
		fwrite(pixels[row],pixelBytesPerRow,1,fd);
		fwrite(zeroes,paddingBytesPerRow,1,fd);
	}
	fclose(fd);
	return 1;
}

void makeBMP(uint8_t qrcode[],int mida,char file[]){
	pixel pixelBlack={0,0,0};
	pixel pixelWhite={255,255,255};
	
	int size = qrcodegen_getSize(qrcode);
	int x,y,i=0,j;
	int midaPixel=1;
	//calculem la mesura desitjada
	while(size*midaPixel<mida){
		midaPixel++;
	}

	pixel pixels[size*midaPixel][size*midaPixel];
	//pixel pixels[][4000];
		
	for (y = 0; y < size; y++) {
		for (x = 0; x < size; x++) {
			for(j=0;j!=midaPixel;j++){
				for(i=0;i!=midaPixel;i++){
					if(qrcodegen_getModule(qrcode, x, y)){
						pixels[j+(y*midaPixel)][i+(x*midaPixel)]=pixelBlack;
					}
					else {
						pixels[j+(y*midaPixel)][i+(x*midaPixel)]=pixelWhite;
					}
				}
			}
		}
	}
	writeBMP(file,size*midaPixel,size*midaPixel,pixels);
}
