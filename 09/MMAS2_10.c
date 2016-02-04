#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<limits.h>

#define MAX_CITY 500

int NNmethod(int *c, int *euc, int num);
int calcEuc(double x1, double x2, double y1, double y2);
double calcStanDev(double *array, int length, double ave);
double calcTauMin(double taumax, int n, double p);
void swap(int *x, int *y);
void reverse(int *array, int start, int end);

int main(int argc, char *argv[]){

  srand(509);
  const int seeds[10] = {509, 521, 523, 541, 547, 557, 563, 569, 571, 577}; // seeds
  int rho = 4;
  int pbest = 2;
  int sdc = 0;
  
  // variable declaration
  int cityNum; // number of citis
  double x, y; // for tsp file
  int i, j, ant, city; // for for loop
  int min, minNum;
  double sum; // for output
  int count, change, loopcount, tmp;
  
  const int maxloop = 200;

  // variable declaration for Ant System
  int alpha = 1;
  int beta = 2;
  int m = 25; // m = cityNum
  double rhos[6] = {0.01, 0.02, 0.1, 0.2, 0.5, 0.8};
  double pbests[6] = {0.001, 0.005, 0.01, 0.05, 0.1, 0.5};

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

  double besttl[10];
  double loops[10];
  double avetl;
  double aveloops;
  double sdtl;
  double sdloops;
  
  int tl[10000];
  for(i=0; i<10000; i++){
    tl[i] = 0;
  }
  
 loop:srand(seeds[sdc]);
  
  // variable declaration
  int eucArray[cityNum][cityNum];
  int check[m][cityNum]; // for check to passed city
  int bsf[cityNum]; // best-so-fat
  int solutions[m][cityNum];
  double tau[cityNum][cityNum]; // pheromone
  double taumax, taumin;
  double p[cityNum][cityNum]; // selection probability
  int tourLength[m];
  int c1, c2; // selected city
  int c3, c4, l1, l2, l3, l4; // for 2opt
  int bs, ibs; // tour length @best solution, @iteration
  double ftmp; // double tmp

  // init
  for(i=0; i<cityNum; i++){
    for(j=0; j<cityNum; j++){
      tau[i][j] = 0;
      p[i][j] = 0;
    }
    bsf[i] = 0;
  }
  for(i=0; i<m; i++){
    for(j=0; j<cityNum; j++){
      solutions[i][j] = 0;
      check[i][j] = 0;
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

  // init pheromone
  // make solution by NN
  solutions[0][0] = 0; // start city 0
  check[0][0] = 1;
  min = 0;
  for(i=1; i<cityNum; i++){
    min = NNmethod(check[0], eucArray[min], cityNum); // find min EUC_2D
    check[0][min] = 1; // check city
    bsf[i] = min;
  }
  
  // clac tour length 
  sum = 0;
  for(i=0; i<cityNum; i++){ // calc link(c1, c2) length
    c1 = bsf[i]; 
    c2 = bsf[(i+1)%cityNum];
    sum += eucArray[c1][c2];
  }
  bs = sum;
  // set initial pheromone
  taumax = 1.0 / (bs * rhos[rho]);
  taumin = calcTauMin(taumax, cityNum, pbests[pbest]);
  for(i=0; i<cityNum; i++){
    for(j=i+1; j<cityNum; j++){
      tau[i][j] = taumax;
      tau[j][i] = taumax;
    }
  }
  // end init pheromone
  
  // start iteration
  count = 0;
  loopcount = 0;
  while(1){
    loopcount++;
    for(i=0; i<m; i++){ // init
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
	for(i=0; i<cityNum; i++){
	  for(j=0; j<cityNum; j++){
	    p[i][j] = 0;
	  }
	}

	// calc selection probability ( p[][] )
	for(i=0; i<cityNum; i++){ // calc denominator
	  if(check[ant][i] == 0){
	    sum += pow(tau[c1][i], alpha) * pow((1.0 / eucArray[c1][i]), beta);
	  }
	}
	for(i=0; i<cityNum; i++){
	  if(check[ant][i] == 0 && tau[c1][i] != 0.0){
	    ftmp = pow(tau[c1][i], alpha) * pow((1.0 / eucArray[c1][i]), beta) / sum;
	    if(isnan(ftmp)){
	      ftmp = 1.0 / (bs*rho);
	    }
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
	    c1 = c2;
	    break;
	  }
	  i++;
	}
      }
      // end loop for make tour
      
      // start 2opt
      while(1){
	change = 0;
	for(i=0; i<cityNum-2; i++){ // link 1 = (c1, c2)
	  c1 = solutions[ant][i];
	  c2 = solutions[ant][i+1];
	  for(j=i+2; j<cityNum; j++){ // link 2 = (c3, c4)
	    c3 = solutions[ant][j];
	    c4 = solutions[ant][(j+1)%cityNum]; // if k = (cityNum - 1) then k+1 means 0 (= cityNum % cityNum)
	    
	    l1 = calcEuc(X[c1], X[c2], Y[c1], Y[c2]); // length c1, c2
	    l2 = calcEuc(X[c3], X[c4], Y[c3], Y[c4]); // length c3, c4
	    l3 = calcEuc(X[c1], X[c3], Y[c1], Y[c3]); // length c1, c3
	    l4 = calcEuc(X[c2], X[c4], Y[c2], Y[c4]); // length c2, c4
	    
	    if(l1 + l2 > l3 + l4){ // if before length > after length then swap
	      reverse(solutions[ant], i+1,  j);
	      swap(&c2, &c3);
	      change++;
	    }
	  }
	}
	if(change == 0) // if not swaped
	  break;
      }
      // end 2opt
      // calc tour length
      sum = 0;
      for(i=0; i<cityNum; i++){ // calc link(c1, c2) length
	c1 = solutions[ant][i]; 
	c2 = solutions[ant][(i+1)%cityNum];
	sum += eucArray[c1][c2];
      }
      tourLength[ant] = sum;
      // goto next ant
    }
    // end loop for ant
    
    // search iteration beat
    ibs = INT_MAX;
    for(i=0; i<m; i++){
      if(ibs > tourLength[i]){
	ibs = tourLength[i];
	minNum = i;
      }
    }
    
    // end decision
    if(bs <= ibs){ // continue 
      count++;
    }
    else{ // reset count and update best solution, taumax, and taumin, 
      //      printf("update: %d -> %d @%dloops\n", bs, ibs, loopcount);
      bs = ibs;
      count = 0;
      for(i=0; i<cityNum; i++){ 
	bsf[i] = solutions[minNum][i]; // update best-so-far
      }
    }
    tl[loopcount-1] = bs;
    if(count == maxloop){// end
      break;
    }
    
    // update pheromone
    taumax = 1.0 / (rhos[rho] * ibs);
    taumin = calcTauMin(taumax, cityNum, pbests[pbest]);
    // evaporation
    for(i=0; i<cityNum; i++){
      for(j=0; j<cityNum; j++){
	tau[i][j] *= (1.0 - rhos[rho]);
      }
    }
    // add pheromone
    ftmp = 1.0 / ibs;
    for(i=0; i<cityNum; i++){
      c1 = solutions[minNum][i];
      c2 = solutions[minNum][(i+1)%cityNum];
      tau[c1][c2] += ftmp;
      tau[c2][c1] += ftmp;
    }
    // MAX-MIN
    for(i=0; i<cityNum; i++){
      for(j=i+1; j<cityNum; j++){
	if(tau[i][j] > taumax){
	  tau[i][j] = taumax;
	  tau[j][i] = taumax;
	}
	if(tau[i][j] < taumin){
	  tau[i][j] = taumin;
	  tau[j][i] = taumin;
	}
      }
    }
    //    printf("%d\n",loopcount);
    // end update pheromone
  }

  besttl[sdc] = bs;
  loops[sdc] = loopcount;
  printf("seed: %d, bs: %d, %dloops\n", seeds[sdc], bs, loopcount);

  sdc++;
  if(sdc == 10){
    goto end;
  }

  goto loop;

  return 0;


 end:puts("");
  int k;
  // calc ave
  for(k=0; k<10; k++){
    avetl += besttl[k] * 0.1;
    aveloops += loops[k] * 0.1;
  }
  // calc sd
  sdtl = calcStanDev(besttl, 10, avetl);
  sdloops = calcStanDev(loops, 10, aveloops);

  printf("avetl:%f, avelp:%f, sdtl:%f, sdlp:%f\n",avetl, aveloops, sdtl, sdloops);
  
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

double calcStanDev(double *array, int length, double ave){
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

double calcTauMin(double taumax, int n, double p){
  double tmp = pow(p, (1.0 / n));
  return ((1.0 - tmp) * taumax) / ((n/2.0 - 1.0) * tmp);
}

void swap(int *x, int *y){
  int tmp = *x;
  *x = *y;
  *y = tmp;
  
}

/* reverse *array from start to end */
void reverse(int *array, int start, int end){
  int i;
  for(i=0; i<(end-start+1)/2; i++){
    swap(&array[start + i], &array[end - i]);
      
  }
  
}
