#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#define MAXLEN 50

void buildDict(char **dict, int **hotVectors, int *wordCount, FILE *file);
int searchDict(char **dict, int *wordCount, char temp[]);
void addDict(char **dict, int *wordCount, char temp[], int tempIndex);
double lossFunc(double *residuals);
void res(double *residuals, int **hotVectors, double *WParams, int *wordCount);
void generateWParams(double *WParams, int *wordCount);
void cloneWParams(double *WParams, double *WParamsTemp, int *wordCount);
void recordWParams(double *WParamsTemp, int *wordCount, FILE *params);
double sum_wxi(int **hotVectors, double *WParams, int *wordCount, int i);
double dfi(int **hotVectors, double *WParams, int *wordCount, int i);
double dri_dwj(int **hotVectors, double *WParams, int *wordCount, int i, int j);
void gd(int **hotVectors, double *WParamsTemp, double *residuals, int *wordCount, int q);
void sgd(int **hotVectors, double *WParamsTemp, double *residuals, int *wordCount, int q);
void adam(int **hotVectors, double *WParamsTemp, double *residuals, int *wordCount, int q);
double egitimBasarisi(int **hotVectors, double *WParams, int *wordCount);
double testBasarisi(int **hotVectors, double *WParams, int *wordCount);

int main(){
    srand(time(NULL));
    FILE *file;
    file = fopen("data.txt","r");
    if(file == NULL){
        printf("File could not be opened\n");
        exit(0);
    }	    
    char **dict;
    int **hotVectors;
    double *WParams, *WParamsTemp, *residuals;
    int *wordCount;
    int wordcnt = 0, i, j;
    wordCount=&wordcnt;
    dict = (char**)calloc(7000, sizeof(char*));
    hotVectors = (int**)calloc(7000, sizeof(int*));
    WParams = (double*)calloc(7000, sizeof(double));
    WParamsTemp = (double*)calloc(7000, sizeof(double));
    residuals = (double*)calloc(160, sizeof(double)); 
    for (i = 0; i < 7000; i++) {
    	dict[i] = (char*)calloc(MAXLEN, sizeof(char));
        hotVectors[i] = (int*)calloc(200, sizeof(int));
    }
    clock_t start_time, end_time;
    buildDict(dict, hotVectors, wordCount, file);
	
    for(i=0;i<5;i++){
    	generateWParams(WParams, wordCount);
    
		cloneWParams(WParams, WParamsTemp, wordCount);
		gd(hotVectors, WParamsTemp, residuals, wordCount, i);
		
		cloneWParams(WParams, WParamsTemp, wordCount);
		sgd(hotVectors, WParamsTemp, residuals, wordCount, i);
		
		cloneWParams(WParams, WParamsTemp, wordCount);
		adam(hotVectors, WParamsTemp, residuals, wordCount, i);
    }
	
    for (i=0;i<7000;i++){
        free(dict[i]);
        free(hotVectors[i]);
    }
    free(dict);
    free(hotVectors);
    free(WParams);
    free(WParamsTemp);
    free(residuals);
    fclose(file);
    return 0;
}

void buildDict(char **dict, int **hotVectors, int *wordCount, FILE *file){
    if(file == NULL){
        printf("File could not be opened\n");
        exit(0);
    }
    char temp[MAXLEN], karakter;
    int tempIndex=0, hotCount=0, index, x; 
    while((karakter = fgetc(file)) != EOF){
        if(karakter == '<') {
            hotCount++;
        }
    	if (karakter != '#' && karakter != '<' && karakter != '>') {
			if(isspace(karakter) || ispunct(karakter) || karakter == '\n'){
            	if (tempIndex > 0 && (karakter == '-')) {
					temp[tempIndex]=karakter;
            		tempIndex++;
            	} else {
	                temp[tempIndex]='\0';
    	            if(searchDict(dict,wordCount,temp)==-1){
        	            addDict(dict,wordCount,temp,tempIndex);
            	    }
                	tempIndex=0;
                    x=searchDict(dict,wordCount,temp);
                    hotVectors[x][hotCount-1]=1;
            	}
        	}else if((karakter != '\"' && karakter != '\'' && karakter != '-')){
				temp[tempIndex]=tolower(karakter);
            	tempIndex++;
        	}
        }
    }    
}

int searchDict(char **dict, int *wordCount, char temp[]){
    int i=0, compareResult;
    while(i<*wordCount && (compareResult = strcmp(dict[i],temp))!=0){
        i++;
    }
    if(compareResult==0){
        return i;
    }else{
        return -1;
    }
}

void addDict(char **dict, int *wordCount, char temp[], int tempIndex){
    dict[*wordCount]=(char*)calloc((tempIndex+1),sizeof(char));
    strcpy(dict[*wordCount],temp);
    dict[*wordCount][tempIndex]='\0';
    (*wordCount)++;
}

double lossFunc(double *residuals){
	int k;
    double error=0;
    for(k=0;k<160;k++){
        error+=pow(residuals[k], 2);
    }
    return error/160;
}

void res(double *residuals, int **hotVectors, double *WParams, int *wordCount){
    int i;
    double tmp;
    for(i=0;i<80;i++){
        tmp=sum_wxi(hotVectors, WParams, wordCount, i);
        residuals[i]=1-tanh(tmp);
    }
    for(i=80;i<160;i++){
        tmp=sum_wxi(hotVectors, WParams, wordCount, i);
        residuals[i]=-1-tanh(tmp);
    }
}

void generateWParams(double *WParams, int *wordCount){	
    double min=-0.3, max=0.3;
    int i;
    for(i=0;i<*wordCount;i++){
        WParams[i]= min + ((double)rand() / RAND_MAX) * (max - min);
    }
}

void cloneWParams(double *WParams, double *WParamsTemp, int *wordCount){
	int i;
	for(i=0;i<*wordCount;i++){
		WParamsTemp[i]=WParams[i];
	}
}

void recordWParams(double *WParamsTemp, int *wordCount, FILE *params){
    if (params == NULL) {
        printf("Dosya acma hatasi!\n");
    }
    int i;
    for(i=0;i<*wordCount;i++){
    	fprintf(params, "%lf ", WParamsTemp[i]);
	}
	fprintf(params, "\n");
}

double sum_wxi(int **hotVectors, double *WParams, int *wordCount, int i){
    double result=0;
    int j;
    for(j=0;j<*wordCount;j++){
        result+=hotVectors[j][i]*WParams[j];
    }
    return result;
}

double dfi(int **hotVectors, double *WParams, int *wordCount, int i){
	double f_deriv, f_xi_plus_h, f_xi_minus_h, h=0.00000001;
    double temp;
	temp=sum_wxi(hotVectors, WParams, wordCount, i);
	f_xi_plus_h=1-tanh(temp+h);
    f_xi_minus_h=1-tanh(temp-h);
    f_deriv=(f_xi_plus_h-f_xi_minus_h)/(2*h);
	return f_deriv;
}

double dri_dwj(int **hotVectors, double *WParams, int *wordCount, int i, int j){
	double f_deriv, f_xi_plus_h, f_xi_minus_h, wx_plus_h=0, wx_minus_h=0, h=0.00000001;
    double temp, result=0;
    int k;
    for(k=0;k<*wordCount;k++){
    	if(k!=j){
    		wx_plus_h+=hotVectors[k][i]*WParams[k];
    		wx_minus_h+=hotVectors[k][i]*WParams[k];
		}else{
			wx_plus_h+=hotVectors[k][i]*(WParams[k]+h);
			wx_minus_h+=hotVectors[k][i]*(WParams[k]-h);
		}
    }	
	f_xi_plus_h=1-tanh(wx_plus_h);
    f_xi_minus_h=1-tanh(wx_minus_h);
    f_deriv=(f_xi_plus_h-f_xi_minus_h)/(2*h);
	return f_deriv;
}

void gd(int **hotVectors, double *WParamsTemp, double *residuals, int *wordCount, int q){
	printf("GD FOR W%d:\n",q+1);
	FILE *file, *epochLossData;	
	char name[20];
    sprintf(name, "gd%d.txt", q+1);
    file = fopen(name, "a");
    sprintf(name, "gdEpochSureLossData%d.txt", q+1);
    epochLossData = fopen(name, "a");
	int i,j;
	double temp;
    double error=1, eps=0.08, egitimBasari, testBasari;
    int epoch=0, maxiterations=100, tt=1, *t;
    res(residuals, hotVectors, WParamsTemp, wordCount);
    epoch=0;
    recordWParams(WParamsTemp, wordCount, file);
    time_t start_time, current_time;
    start_time = time(NULL);
	while(error>0.0001 && epoch<maxiterations){
		error=lossFunc(residuals);
        printf("epoch: %d error: %lf\n",epoch+1,error);
        epoch++;
		for(i=0;i<160;i++){
			temp=dfi(hotVectors, WParamsTemp, wordCount, i)*residuals[i];
			for(j=0;j<*wordCount;j++){
				WParamsTemp[j]=WParamsTemp[j]-eps*temp*hotVectors[j][i];
			}
			res(residuals, hotVectors, WParamsTemp, wordCount);
		}
		current_time = time(NULL);
		double elapsed_time = difftime(current_time, start_time);
		egitimBasari=egitimBasarisi(hotVectors, WParamsTemp, wordCount);
		testBasari=testBasarisi(hotVectors, WParamsTemp, wordCount);
		error=lossFunc(residuals);
		fprintf(epochLossData, "%d %lf %lf\n", epoch, elapsed_time, error);
		recordWParams(WParamsTemp, wordCount, file);
    }
    testBasarisi(hotVectors, WParamsTemp, wordCount);
	fclose(file);
	fclose(epochLossData);
}

void sgd(int **hotVectors, double *WParamsTemp, double *residuals, int *wordCount, int q){
	printf("SGD FOR W%d:\n",q+1);	
	FILE *file, *epochLossData;	
	char name[20];
    sprintf(name, "sgd%d.txt", q+1);
    file = fopen(name, "a");
    sprintf(name, "sgdEpochSureLossData%d.txt", q+1);
    epochLossData = fopen(name, "a");		
	int i,j,k;
	double temp;
    double error=1, eps=0.08, egitimBasari, testBasari;
    int epoch=0, maxiterations=100, tt=1, *t;
    res(residuals, hotVectors, WParamsTemp, wordCount);
    epoch=0;
    recordWParams(WParamsTemp, wordCount, file);
    time_t start_time, current_time;
    start_time = time(NULL);
	while(error>0.0001 && epoch<maxiterations){
		error=lossFunc(residuals);
        printf("epoch: %d error: %lf\n",epoch+1,error);
        epoch++;
		for(k=0;k<160;k++){
			i = rand() % 160;
			temp=dfi(hotVectors, WParamsTemp, wordCount, i)*residuals[i];
			for(j=0;j<*wordCount;j++){
				WParamsTemp[j]=WParamsTemp[j]-eps*temp*hotVectors[j][i];
			}
			res(residuals, hotVectors, WParamsTemp, wordCount);
		}
		current_time = time(NULL);
		double elapsed_time = difftime(current_time, start_time);
		egitimBasari=egitimBasarisi(hotVectors, WParamsTemp, wordCount);
		testBasari=testBasarisi(hotVectors, WParamsTemp, wordCount);
		error=lossFunc(residuals);
		fprintf(epochLossData, "%d %lf %lf\n", epoch, elapsed_time, error);	
		recordWParams(WParamsTemp, wordCount, file);
    }
    testBasarisi(hotVectors, WParamsTemp, wordCount);
    fclose(file);
}

void adam(int **hotVectors, double *WParamsTemp, double *residuals, int *wordCount, int q){
	printf("ADAM FOR W%d:\n",q+1);	
	FILE *file, *epochLossData;	
	char name[20];
    sprintf(name, "adam%d.txt", q+1);
    file = fopen(name, "a");
    sprintf(name, "adamEpochSureLossData%d.txt", q+1);
    epochLossData = fopen(name, "a");		
	double b1=0.9, b2=0.999, z=0.00000001;
	double m_hat, v_hat, gradient=0;
	double error=1, eps=0.08, egitimBasari, testBasari;
    int epoch=0, maxiterations=800, tt=1, *t;
    t=&tt;
	int i, j;
    double *m, *v;
	m = (double*)calloc(6000, sizeof(double));
    v = (double*)calloc(6000, sizeof(double));  

    res(residuals, hotVectors, WParamsTemp, wordCount);
    epoch=0;
    recordWParams(WParamsTemp, wordCount, file);
    time_t start_time, current_time;
    start_time = time(NULL);    
	while(error>0.0001 && epoch<maxiterations){
		error=lossFunc(residuals);
        printf("epoch: %d error: %lf\n",epoch+1,error);
        epoch++;
		j = rand() % 160;
		for(i=0;i<(*wordCount);i++){
			gradient=160*dri_dwj(hotVectors, WParamsTemp, wordCount, j, i)*residuals[j];
			m[i]=b1*m[i] + (1-b2)*gradient;
			v[i]=b2*v[i] + (1-b2)*gradient*gradient;
			m_hat = m[i] / (1 - pow(b1, *t));
		    v_hat = v[i] / (1 - pow(b2, *t));
		    WParamsTemp[i] -= eps * hotVectors[i][j] * m_hat / (sqrt(v_hat) + z);
		}
		(*t)++;
		res(residuals, hotVectors, WParamsTemp, wordCount);
		current_time = time(NULL);
		double elapsed_time = difftime(current_time, start_time);
		egitimBasari=egitimBasarisi(hotVectors, WParamsTemp, wordCount);
		testBasari=testBasarisi(hotVectors, WParamsTemp, wordCount);
		error=lossFunc(residuals);
		fprintf(epochLossData, "%d %lf %lf\n", epoch, elapsed_time, error);			
		recordWParams(WParamsTemp, wordCount, file);    
    }
    testBasarisi(hotVectors, WParamsTemp, wordCount);			
    free(m);
    free(v);
	fclose(file);	
}

double egitimBasarisi(int **hotVectors, double *WParams, int *wordCount){
	double tmp, temp;
	int k, successful=0;
	for(k=0;k<80;k++){
		tmp=sum_wxi(hotVectors, WParams, wordCount, k);
		if(tanh(tmp)>0.5){
			successful++;
		}
	}
	for(k=80;k<160;k++){
		tmp=sum_wxi(hotVectors, WParams, wordCount, k);
		if(tanh(tmp)<-0.5){
			successful++;
		}
	}
	temp=successful/160;
	return temp;
}

double testBasarisi(int **hotVectors, double *WParams, int *wordCount){
	double tmp, temp;
	int k, successful=0, successfulA=0, successfulB=0;
	for(k=160;k<180;k++){
		tmp=sum_wxi(hotVectors, WParams, wordCount, k);
		if(tanh(tmp)>0.5){
			successful++;
			successfulA++;
		}
	}
	for(k=180;k<200;k++){
		tmp=sum_wxi(hotVectors, WParams, wordCount, k);
		if(tanh(tmp)<-0.5){
			successful++;
			successfulB++;
		}
	}
	//printf("A test basari : %d\n",successfulA);
	//printf("B test basari : %d\n",successfulB);
	temp=successful/40;
	return temp;
}
