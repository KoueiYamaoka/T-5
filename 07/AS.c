#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<limits.h>

#define MAX_CITY 500

int NNmethod(int *c, int *euc, int num);
int calcEuc(double x1, double x2, double y1, double y2);
void swap(int *x, int *y);
double calcStanDev(int *array, int length, double ave);
void reverse(int *array, int start, int end);

int main(int argc, char *argv[]){

  srand(509);
  const int seeds[10] = {509, 521, 523, 541, 547, 557, 563, 569, 571, 577}; // seeds
  int sd = 0;
  int b = 0; // beta select

  // variable declaration
  int cityNum; // number of citis
  double x, y; // for tsp file
  int i, j, ant, city; // for for loop
  int max, min, minNum;
  double ave, sum, standardDeviation; // for output
  int count, loopcount, tmp;
  
  const int maxloop = 1000;

  // variable declaration for Ant System
  int alpha = 1;
  int beta[4] = {2, 3, 4, 5};
  int m; // m = cityNum
  double rho = 0.5;

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
  double buffx[MAX_CITY], buffy[MAX_CITY];

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
  while((ret = fscanf(fp, "%d%lf%lf", &cityNum, &x, &y)) != EOF){
    if(ret == 0)
      break;
    buffx[i] = x;
    buffy[i] = y;
    i++;
  }
  fclose(fp);
  

  // input city locaions
  double X[cityNum], Y[cityNum]; // city locations
  for(i=0; i<cityNum; i++){
    X[i] = buffx[i];
    Y[i] = buffy[i];
  }
  //// end input tsp file
  int bc = 0;
  int sdc = 0;
  int bss[4][10];
  int loops[4][10];
  
  for(i=0;i<4;i++){
    for(j=0;j<10;j++){
      bss[i][j] = 0;
      loops[i][j] = 0;
    }
  }

  int tl[5000];
  for(i=0; i<5000; i++){
    tl[i] = 0;
  }

  
 loop:srand(seeds[sd]);
  
  // variable declaration
  m = cityNum;
  int eucArray[cityNum][cityNum];
  int check[m][cityNum]; // for check to passed city
  int solution[cityNum];
  int solutions[m][cityNum];
  double tau[cityNum][cityNum]; // pheromone

  double p[m][cityNum]; // selection probability
  int tourLength[cityNum];
  int c1, c2; // selected city
  int bs = INT_MAX; // tour length @best solution
  double ftmp; // double tmp

  // init
  for(i=0; i<cityNum; i++){
    for(j=0; j<cityNum; j++){
      tau[i][j] = 0;
    }
    tourLength[i] = 0;
  }
  for(i=0; i<m; i++){
    for(j=0; j<cityNum; j++){
      solutions[i][j] = 0;
      p[i][j] = 0;
      check[i][j] = 0;
    }
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

  // init pheromone
  // make solution by NN
  solutions[0][0] = 0; // start city 0
  check[0][0] = 1;
  min = 0;
  for(j=1; j<cityNum; j++){
    min = NNmethod(check[0], eucArray[min], cityNum); // find min EUC_2D
    check[0][min] = 1; // check city
    solutions[0][j] = min;
  }
  
  // clac tour length 
  sum = 0;
  for(i=0; i<cityNum; i++){ // calc link(c1, c2) length
    c1 = solutions[0][i]; 
    c2 = solutions[0][(i+1)%cityNum];
    sum += eucArray[c1][c2];
  }
  bs = sum;
  // set initial pheromone
  ftmp = m / sum;
  for(i=0; i<cityNum; i++){
    for(j=i+1; j<cityNum; j++){
      tau[i][j] = ftmp;
      tau[j][i] = ftmp;
    }
  }
  // end init pheromone
  
  // start iteration
  count = 0;
  loopcount = 0;
  while(1){
    loopcount++;
    for(i=0; i<m; i++){
      for(j=0; j<cityNum; j++){
	check[i][j] = 0;
      }
      tourLength[i] = 0;
    }
    // loop for ant
    for(ant=0; ant<m; ant++){
      c1 = cityNum * (rand()/(RAND_MAX+1.0)); // select first city
      check[ant][c1] = 1;
      solutions[ant][0] = c1;
      
      // loop for make tour
      for(city=1; city<cityNum; city++){
	// init
	sum = 0;
	for(i=0; i<m; i++){
	  for(j=0; j<cityNum; j++){
	    p[i][j] = 0;
	  }
	}

	// calc selection probability ( p[][] )
	for(i=0; i<cityNum; i++){ // calc denominator
	  if(check[ant][i] == 0){
	    sum += pow(tau[c1][i], alpha) * pow((1.0 / eucArray[c1][i]), beta[b]);
	  }
	}
	for(i=0; i<cityNum; i++){
	  if(check[ant][i] == 0){
	    ftmp = pow(tau[c1][i], alpha) * pow((1.0 / eucArray[c1][i]), beta[b]) / sum;
	    p[c1][i] = ftmp;
	    p[i][c1] = ftmp;
	  }
	}
	// make selection probability array
	for(i=1; i<cityNum; i++){
	  p[c1][i] += p[c1][i-1];
	}

	// select next city
	ftmp = p[c1][cityNum-1] * (rand()/(RAND_MAX+1.0));
	i = 0;
	while(1){
	  if(p[c1][i] >= ftmp){
	    c2 = i; // now c2 = the city next to c1
	    check[ant][c2] = 1; // check
	    solutions[ant][city] = c2; // input solution
	    tourLength[ant] += eucArray[c1][c2]; // add length(c1, c2);
	    c1 = c2;
	    break;
	  }
	  i++;
	}
      }
      // end loop for make tour
      tourLength[ant] += eucArray[solutions[ant][0]][solutions[ant][cityNum-1]];
    }
    // end loop for ant
    
    // update pheromone
    // calc each tau according to ants
    // evaporation
    for(i=0; i<m; i++){
      for(j=0; j<cityNum; j++){
	tau[i][j] *= (1.0 - rho);
      }
    }
    // add pheromone
    for(ant=0; ant<m; ant++){
      ftmp = 1.0 / tourLength[ant];
      for(i=0; i<cityNum; i++){
	c1 = solutions[ant][i];
	c2 = solutions[ant][(i+1)%cityNum];
	tau[c1][c2] += ftmp;
	tau[c2][c1] += ftmp;
      }
    }
    // end update pheromone

    // search min
    min = INT_MAX;
    for(i=0; i<cityNum; i++){
      if(min > tourLength[i]){
	min = tourLength[i];
	minNum = i;
      }
    }
    printf("\n%d\n", bs);

    // end decision
    if(bs < min){ // continue 
      count++;
    }
    else{ // update best solution, reset count and continue
      bs = min;
      count = 0;
      for(i=0; i<cityNum; i++){
	solution[i] = solutions[minNum][i];
      }
    }
    tl[loopcount] = bs;
    if(count == maxloop){// end
      break;
    }
  }
  goto output;
  bss[bc][sdc] = bs;
  loops[bc][sdc] = loopcount;
  sdc++;
  if(sdc == 10){
    sdc = 0;
    bc++;
  }
  if(bc == 4)
    goto end;
  goto loop;

 end:puts("");
  int sum2;
  int op[2][4];
  for(i=0;i<4;i++){
    sum = 0;
    sum2 = 0;
    for(j=0;j<10;j++){
      sum += bss[i][j];
      sum2 += loops[i][j];
    }
    op[0][i] = sum / 10;
    op[1][i] = sum2 / 10;
  }
  
  printf("bs:");
  for(i=0;i<4;i++){
    printf("%d ", op[0][i]);
  }
  printf("\nlps:");
  for(i=0;i<4;i++){
    printf("%d ", op[1][i]);
  }
  puts("");

 output:puts("");
  // for sparse
  char filename[100];
  sprintf(filename, "test.csv");
    // open file
    if((fp = fopen(filename, "w")) == NULL){
      printf("file open error.\n");
      exit(EXIT_FAILURE);
    }
    // write

    for(i=0; i<5000; i++){
      fprintf(fp, "%d, ", i);
    }
    fprintf(fp, "5000\n");

    for(i=0; i<5000; i++){
      fprintf(fp, "%d, ", tl[i]);
    }
    fprintf(fp, "\n");


    // close
    fclose(fp);
    


  
  return 0;
}



/* 
 *c; checkArray
 *euc: eucArray[i]
 num: cityNum
 return: index of min EUC_2D
 */
int NNmethod(int *c, int *euc, int num){
  int i, minNum;
  int m = INT_MAX;
  for(i=0; i<num; i++){
    if(*c == 0 && *euc != 0 && m > *euc){ // if (not checked and not 0 and min)
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
int calcEuc(double x1, double x2, double y1, double y2){
  return (int)(sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2)) + 0.5);
   
}

/*void swap(int *x, int *y){
  int tmp = *x;
  *x = *y;
  *y = tmp;
  }*/

double calcStanDev(int *array, int length, double ave){
  int i;
  double sum = 0;
  for(i=0; i<length; i++){
    sum += pow(*array - ave, 2);
    if(i<length-1)
      ++array;
  }
  sum /= length; // dispersion

  return sqrt(sum);
}

/* reverse *array from start to end */
/*void reverse(int *array, int start, int end){
  int i;
  for(i=0; i<(end-start+1)/2; i++){
    swap(&array[start + i], &array[end - i]);
  }
  }*/
