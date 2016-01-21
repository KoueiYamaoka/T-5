#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<limits.h>

#define MAX_CITY 500

int NNmethod(int *c, float *euc, int num);
int calcEuc(float x1, float x2, float y1, float y2);
void swap(int *x, int *y);
float calcStanDev(int *array, int length, float ave);
void reverse(int *array, int start, int end);

int main(int argc, char *argv[]){

  // variable declaration
  int cityNum; // number of citis
  float x, y; // for tsp file
  int i, j, k, l; // for for loop
  int max, min;
  float ave, standardDeviation; // for output
  int count, tmp, sum, miss;
  
  int maxloop = 1000;

  // usage
  if(argc == 1 || strcmp(argv[1], "-help") == 0){
    printf("usage: $ ./NN2opt tspFileNumber\n");
    printf("tspFileNumber = 0:eil51, 1:pr76, 2:rat99, 3:kroA100, 4:ch130\n");
    return 0;
  }

  //// input tsp file
  char tspName[5][12] = {"eil51.tsp", "pr76.tsp", "rat99.tsp", "kroA100.tsp", "ch130.tsp"};
  FILE *fp;
  char *fname = tspName[atoi(argv[1])];
  char s[100];
  float buffx[MAX_CITY], buffy[MAX_CITY];

  fp = fopen(fname, "r");
  if(fp == NULL){
    printf("File %s is not found.\nusage: $ ./NN2opt -help\n", fname);
    return -1;
  }

  // skip 6 line
  for(i=0; i<6; i++){
    fgets(s, 100, fp);
  }

  // read
  i = 0;
  int ret;
  while((ret = fscanf(fp, "%d%f%f", &cityNum, &x, &y)) != EOF){
    if(ret == 0)
      break;
    buffx[i] = x;
    buffy[i] = y;
    i++;
  }
  fclose(fp);

  // input city locaions
  float X[cityNum], Y[cityNum]; // city locations
  for(i=0; i<cityNum; i++){
    X[i] = buffx[i];
    Y[i] = buffy[i];
  }
  //// end input tsp file

  // variable declaration
  float eucArray[cityNum][cityNum];
  int check[cityNum]; // for check to passed city
  int solutions[cityNum][cityNum];
  int tourLength[cityNum];
  int c1, c2, c3, c4; // selected city
  int l1, l2, l3, l4; // length (c1, c2), (c3, c4), (c1, c3), (c2, c4)
  int total;

  // init
  for(i=0; i<cityNum; i++){
    for(j=0; j<cityNum; j++){
      solutions[i][j] = 0;
    }
    tourLength[i] = 0;
  }

  // calc EUC_2D
  // init diagonal 
  for(i=0; i<cityNum; i++){
    eucArray[i][i] = 0;
  }
  // calc 
  for(i=0; i<cityNum; i++){
    for(j=i+1; j<cityNum; j++){
      tmp = calcEuc(X[i], X[j], Y[i], Y[j]);
      eucArray[i][j] = tmp;
      eucArray[j][i] = tmp;
    }
  }

  // start NN and 2opt
  miss = 0;
  for(i=0; i<cityNum; i++){

    // init
    for(j=0; j<cityNum; j++){
      check[j] = 0;
    }
    
    // make solution by NN
    solutions[i][0] = i; // start city i
    check[i] = 1;
    min = i;
    for(j=1; j<cityNum; j++){
      min = NNmethod(check, eucArray[min], cityNum); // find min EUC_2D
      check[min] = 1; // check city
      solutions[i][j] = min;
    }

    // start 2opt
    for(l=0; l<maxloop; l++){
      count = 0;
      total = 0;
      for(j=0; j<cityNum-2; j++){ // link 1 = (c1, c2)
	c1 = solutions[i][j];
	c2 = solutions[i][j+1];
	for(k=j+2; k<cityNum; k++){ // link 2 = (c3, c4)
	  c3 = solutions[i][k];
	  c4 = solutions[i][(k+1)%cityNum]; // if k = (cityNum - 1) then k+1 means 0 (= cityNum % cityNum)
	  
	  l1 = calcEuc(X[c1], X[c2], Y[c1], Y[c2]); // length c1, c2 
	  l2 = calcEuc(X[c3], X[c4], Y[c3], Y[c4]); // length c3, c4
	  l3 = calcEuc(X[c1], X[c3], Y[c1], Y[c3]); // length c1, c3
	  l4 = calcEuc(X[c2], X[c4], Y[c2], Y[c4]); // length c2, c4
	  
	  if(l1 + l2 > l3 + l4){ // if before length > after length then swap
	    /*	    int l;
	    printf("before(%d, %d)\n",j+1, k);
	    for(l=0; l<cityNum; l++){
	      printf("%d, ", solutions[i][l]);
	    }
	    puts("end");*/
	    reverse(solutions[i], j+1,  k);
	    /*	    printf("after(%d, %d)\n",j+1, k);
	    for(l=0; l<cityNum; l++){
	      printf("%d, ", solutions[i][l]);
	    }
	    puts("end");*/
	    swap(&c2, &c3);
	    count++;
	  }
	}
      }
      total = count;
      printf("%d: total = %d\n", i, total);
      if(count == 0) // if not swaped
	break;
      if(l == maxloop-1){
	printf("failed: start at %d\n", i);
	miss++;
      }
    }
  }
  // end NN and 2opt
  //calc tour length
  max = 0;
  ave = 0;
  min = INT_MAX;
  standardDeviation = 0;
  for(i=0; i<cityNum; i++){
    sum = 0;
    for(j=0; j<cityNum; j++){ // calc link(c1, c2) length
      c1 = solutions[i][j]; 
      c2 = solutions[i][(j+1)%cityNum];
      sum += calcEuc(X[c1], X[c2], Y[c1], Y[c2]);
    }
    tourLength[i] = sum;
    ave += sum;
    if(sum > max){
      max = sum;
    }
    if(sum < min){
      min = sum;
    }
  }
  ave /= cityNum;
  standardDeviation = calcStanDev(tourLength, cityNum, ave);

  printf("max = %d, min = %d, ave = %f, sd = %f, %d miss\n", max, min, ave, standardDeviation, miss);  

  return 0;
}



/* 
 *c; checkArray
 *euc: eucArray[i]
 num: cityNum
 return: index of min EUC_2D
 */
int NNmethod(int *c, float *euc, int num){
  int i, minNum;
  int m = INT_MAX;
  for(i=0; i<num; i++){
    if(*c == 0 && *euc != 0.0 && m > *euc){ // if (not checked and not 0 and min)
      m = *euc;
      minNum = i;
    }
    if(i<num-1){
      ++c;
      ++euc;
    }
  }
  return minNum;
}


/*
  x1, x2, y1, y2: num
  return: EUC_2D
*/
int calcEuc(float x1, float x2, float y1, float y2){
  return (int)(sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2)) + 0.5);
   
}

void swap(int *x, int *y){
  int tmp = *x;
  *x = *y;
  *y = tmp;
}

float calcStanDev(int *array, int length, float ave){
  int i;
  float sum = 0;
  for(i=0; i<length; i++){
    sum += pow(*array - ave, 2);
    if(i<length-1)
      ++array;
  }
  sum /= length; // dispersion

  return sqrt(sum);
}

/* reverse *array from start to end */
void reverse(int *array, int start, int end){
  int i;
  for(i=0; i<(end-start+1)/2; i++){
    swap(&array[start + i], &array[end - i]);
  }
}
