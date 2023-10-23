#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct {
	int id;
	int no;
	char isim[50];
	int yas;
	char hastalikBilgisi[100];
} Hasta;


typedef struct {
	long long int tc;
	int id;
} HastaTcId;


typedef enum 			{ HASTA, 			HASTA_TC_ID			} Mode;
const char* PATH[2] = 	{ "hastalar.bin", 	"hastalarTcId.bin"	};

void writeArrayToFile(void* array, int count, Mode mode) {
	FILE* file = fopen(PATH[mode], "wb");
	if (file == NULL) {
		printf("Failed to open the file.\n");
		return;
	}

	fwrite(&count, sizeof(int), 1, file);

	fwrite(array, ( (mode == 0) ? sizeof(Hasta) : sizeof(HastaTcId) ) , count, file);

	fclose(file);
}



Hasta* readHastalarFromFile(int* outCount){
	FILE *file;
	
	int count = 0;
	
	Hasta *readHastalar = NULL;

	file = fopen(PATH[0], "rb");
	if (file == NULL) {
		printf("Failed to open the file.\n");
		return NULL;
	}

	fread(&count, sizeof(int), 1, file);
	

	readHastalar = (Hasta*)malloc(count * sizeof(Hasta));
	if (readHastalar == NULL) {
		printf("Memory allocation failed.\n");
		return NULL;
	}

	fread(readHastalar, sizeof(Hasta), count, file);

	fclose(file);

	(*outCount) = count;
	return readHastalar;
}


HastaTcId* readTcIdFromFile(int* outCount){
	FILE *file;
	
	int count = 0;
	
	HastaTcId *readTcId = NULL;

	file = fopen(PATH[1], "rb");
	if (file == NULL) {
		printf("Failed to open the file.\n");
		return NULL;
	}

	fread(&count, sizeof(int), 1, file);

	readTcId = (HastaTcId*)malloc(count * sizeof(HastaTcId));
	if (readTcId == NULL) {
		printf("Memory allocation failed.\n");
		return NULL;
	}

	fread(readTcId, sizeof(HastaTcId), count, file);

	fclose(file);

	(*outCount) = count;
	return readTcId;
}




int getIdFromTc(long long int tc){
	int count = 0;
	HastaTcId* readHastaTcId = readTcIdFromFile(&count);
	
	for (size_t i = 0; i < count; i++) {
		if (readHastaTcId[i].tc == tc){
			int id = readHastaTcId[i].id;
			free(readHastaTcId);
			return id;
		} 
	}
	return -1;
}

Hasta getHastaFromId(int id){
	int count = 0;
	Hasta* readHastalar = readHastalarFromFile(&count);

	for (size_t i = 0; i < count; i++) {
		if (readHastalar[i].id == id) {
			Hasta hasta = readHastalar[i];
			free(readHastalar);
			return hasta;
		}
	}
}






int getNewId(){
	int count = 0;
	HastaTcId* readHastaTcId = readTcIdFromFile(&count);
	
	int biggiestId = 0;

	for (size_t i = 0; i < count; i++) {
		if (readHastaTcId[i].id > biggiestId){
			biggiestId = readHastaTcId[i].id;
		}
	}

	free(readHastaTcId);
	return (biggiestId+1);
}

void addHastaToFile(Hasta hasta, int* outId){
	int count = 0;
	Hasta* readHastalar = readHastalarFromFile(&count);

	count++;
	readHastalar = (Hasta*)realloc(readHastalar, count * sizeof(Hasta));
	if (readHastalar == NULL) {
		printf("Memory allocation failed.\n");
		return;
	}

	int newID = getNewId();
	readHastalar[count - 1].id = newID;
	readHastalar[count - 1].no = hasta.no;
	strcpy(readHastalar[count - 1].isim, hasta.isim);
	readHastalar[count - 1].yas = hasta.yas;
	strcpy(readHastalar[count - 1].hastalikBilgisi, hasta.hastalikBilgisi);


	writeArrayToFile(readHastalar,count,HASTA);
	free(readHastalar);
	(*outId) = (newID);
}

void addHastaTcIdToFile(HastaTcId tcid){
	int count = 0;
	HastaTcId* readTcId = readTcIdFromFile(&count);


	count++;
	readTcId = (HastaTcId*)realloc(readTcId, count * sizeof(HastaTcId));
	if (readTcId == NULL) {
		printf("Memory allocation failed.\n");
		return;
	}

	readTcId[count - 1].id = tcid.id;
	readTcId[count - 1].tc = tcid.tc;


	writeArrayToFile(readTcId,count,HASTA_TC_ID);
	free(readTcId);
}



void printHasta(Hasta hasta, long long int tc){
	printf("Hasta:	");
	printf("TC: %lld,	", tc);
	printf("Hasta no: %d,	", hasta.no);
	printf("Isim: %s,	", hasta.isim);
	printf("Yas: %d,	", hasta.yas);
	printf("Hastalik Bilgisi: %s", hasta.hastalikBilgisi);
	printf("\n"); 
}

void printAll(){
	int countHasta = 0;
	int countTcId = 0;
	Hasta* readHastalar = readHastalarFromFile(&countHasta);
	HastaTcId* readTcId = readTcIdFromFile(&countTcId);

	if (countHasta != countTcId) {
		printf("\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
		printf("\n!!! Iki dosyada farkli sayida hasta var. !!!");
		printf("\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n");
		return;
	}
	

	for (int i = 0; i < countHasta; i++) {
		printHasta(readHastalar[i],readTcId[i].tc);
	}
	printf("\n");

	free(readTcId);
	free(readHastalar);
}




void createNewHasta(long long int tc, int no, char isim[], int yas, char hastalikBilgisi[]){
	Hasta hasta;
	hasta.no = no;
	strcpy(hasta.isim, isim);
	hasta.yas = yas;
	strcpy(hasta.hastalikBilgisi, hastalikBilgisi);

	int id = -1;
	addHastaToFile(hasta, &id);

	HastaTcId tcid;
	tcid.tc = tc;
	tcid.id = id;

	addHastaTcIdToFile(tcid);
}



void removeElementFromArray(void* array, int index, int* count, Mode mode) {
	if (index < 0 || index >= (*count)) {
		printf("Invalid index.\n");
		return;
	}

	if (mode == HASTA){
		Hasta* hastalar = (Hasta*)array;
		for (int i = index; i < (*count) - 1; i++) {
			hastalar[i] = hastalar[i + 1];
		}
	}
	else if (mode == HASTA_TC_ID) {
		HastaTcId* tcid = (HastaTcId*)array;
		for (int i = index; i < (*count) - 1; i++) {
			tcid[i] = tcid[i + 1];
		}
	}
	(*count)--;
}

void deleteHasta(long long int tc){
	int id = getIdFromTc(tc);
	if (id == -1) {
		printf("Boyle bir hasta bulunmamaktadir.\n");
		return;
	}
	

	int hastaCount = 0;
	Hasta* readHastalar = readHastalarFromFile(&hastaCount);

	for (int i = 0; i < hastaCount; i++) {
		if (readHastalar[i].id == id) {
			removeElementFromArray(readHastalar, i, &hastaCount, HASTA);
		}
	}

	int tcIdCount = 0;
	HastaTcId* readTcId = readTcIdFromFile(&tcIdCount);
	
	for (int i = 0; i < tcIdCount; i++) {
		if (readTcId[i].id == id) {
			removeElementFromArray(readTcId, i, &tcIdCount, HASTA_TC_ID);
		}
	}


	writeArrayToFile(readHastalar,	hastaCount,	HASTA);
	writeArrayToFile(readTcId,		tcIdCount,	HASTA_TC_ID);
}




void _getInputToAdd(){
	long long int tc;
	int no;
	char isim[50];
	int yas;
	char hastalikBilgisi[100];

	printf("Eklemek istediginiz hastanin bilgilerini girin:\n(TC kimlik no) (Hasta no) (Isim) (Yas) (Hastalik bilgisi)\n");
	if (scanf(" %lld %d %s %d %s", &tc, &no, &isim, &yas, &hastalikBilgisi) != 5) {
		printf("\nLutfen gecerli bir deger girin.\n");
		while (getchar() != '\n');
		_getInputToAdd();
		return;
	}
	createNewHasta(tc, no, isim, yas, hastalikBilgisi);
}

void _getInputToEdit(){
	long long int tc;
	int no;
	char isim[50];
	int yas;
	char hastalikBilgisi[100];


	printf("Duzenlemek istediginiz hastanin bilgilerini girin:\n(TC kimlik no) (Hasta no) (Isim) (Yas) (Hastalik bilgisi)\n");
	if (scanf(" %lld %d %s %d %s", &tc, &no, &isim, &yas, &hastalikBilgisi) != 5) {
		printf("\nLutfen gecerli bir deger girin.\n");
		while (getchar() != '\n');
		_getInputToEdit();
		return;
	}

	deleteHasta(tc);
	createNewHasta(tc, no, isim, yas, hastalikBilgisi);
}

void _getInputToDelete(){
	long long int tc;
	printf("Silmek istediginiz hastanin bilgilerini girin:\n(TC kimlik no)\n");
	if (scanf(" %lld", &tc) != 1) {
		printf("\nLutfen gecerli bir deger girin.\n");
		while (getchar() != '\n');
		_getInputToDelete();
		return;
	}

	deleteHasta(tc);
}

void _getInputToShow(){
	long long int tc;
	printf("Gormek istediginiz hastanin bilgilerini girin:\n(TC kimlik no)\n");
	if (scanf(" %lld", &tc) != 1) {
		printf("\nLutfen gecerli bir deger girin.\n");
		while (getchar() != '\n');
		_getInputToShow();
		return;
	}

	
	int id = getIdFromTc(tc);
	if (id == -1) {
		printf("Boyle bir hasta bulunmamaktadir.\n");
		return;
	}
	printHasta(getHastaFromId(id),tc);
}



void getInput(){
	int action; // 0=Add , 1=Edit, 2=Delete, 3=Show, 4=ShowAll, 5=Exit

	printf("\nYapmak istediginiz islemi secin:\n\
	Hasta ekle(0) / Hasta duzenle(1) / Hasta sil(2) / Hasta bilgilerini gor(3) / Tum hastalari listele(4) / cik(5)\n");

	if (scanf(" %d", &action) != 1) {
		printf("\nLutfen gecerli bir deger girin.\n");
		while (getchar() != '\n'); // Clear input buffer
		getInput();
		return;
	}

	switch (action) {
		case 0:
			_getInputToAdd();
			break;

		case 1:
			_getInputToEdit();
			break;

		case 2:
			_getInputToDelete();
			break;

		case 3:
			_getInputToShow();
			break;

		case 4:
			printAll();
			break;

		case 5:
			return;

	}

	getInput();
}




void checkFile(Mode mode){
	FILE* file = fopen(PATH[mode], "r");
	if (file != NULL) {
		fclose(file);
	}
	else{
		FILE* newFile = fopen(PATH[mode], "wb");
		if (newFile != NULL) {
			fclose(newFile);
		}
		else{
			printf("Failed to create file.");
		}
	}
}



int main() {
	checkFile(HASTA);
	checkFile(HASTA_TC_ID);
	getInput();
}