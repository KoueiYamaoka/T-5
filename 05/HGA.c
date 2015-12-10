#include<stdio.h>
#include<stdlib.h>
#include<math.h>


#define RED 0
#define GREEN 1
#define BLUE 2

// for qsort
int compare_int(const void *a, const void *b){
  return *(int*)a - *(int*)b;
}

int main(void){  
  
  // seed 509
  srand(509);

  
  int select = 4; // select size
  int prob = 1; // if 0 then dense, if 1 then sperse
  int SoP = 500; // size of population 
  double mutationRate = 3; // mutation rate (%)
  int scaling = 2; // scaling. if 0 then nothing, 1 then linear, 2 then power
  int d = 35; // for power scaling. pow(x, d)
  int Ne = 20; // use top Ne gene to HC 

  const int size[5] = {30, 60, 90, 120, 150}; // number of nodes
  const int N = size[select]; // which to use? NN[0] to NN[4] or something int value
  const int links[2] = {N * (N - 1) / 4 // number of links for dense
			, 3 * N}; // number of links for sparse
  const int maxloop = 500; // the maximum number of iterations for GA
  const int maxloopH = 500; // the maximum number of iterations for HC
  const int seeds[10] = {509, 521, 523, 541, 547, 557, 563, 569, 571, 577}; // seeds

  int graph[N][N];  // AdjacencyMatrix
  int parent[SoP][N+1]; // (parent solution + violation point or fitness) * SoP
  int children[SoP][N+1]; // (children solution + violation point or fitness) * SoP
  int topChildren[Ne][N];
  int violation = 0; // violation point
  int calcCount = 0; // The number of calculations
  double fitness[SoP]; // fitness
  double roulette[SoP]; // roulette for select parents
  int mask[N]; // mask bit for crossing
  double maxF[maxloop]; // max fitness for output
  double aveF[maxloop]; // ave fitness for output
  double minF[maxloop]; // min fitness for output
  double sum = 0; // for maxF, aveF, minF, roulette
  int topNe[Ne]; // input top Ne gene's index
  double sp1 = 0; // selected parent 1
  double sp2 = 0; // selected parent 2
  double mutation = 0; // mutation probability

  int solution[N];  // solution candidates
  int violationArray[N]; // if violation=1 then 1 else 0
  int Rv = 0;  // RED violation
  int Gv = 0;  // GREEN violation
  int Bv = 0;  // BLUE violation
  int changeVar = 0; // change value
  int r = 0; // random number
  
  int i, j, k, l, m; // for for loop
  int tmp;
  double tmpd;
  int count;

  //init
  for(i=0; i<SoP; i++){
    for(j=0; j<N; j++){
      parent[i][j] = 0;
      children[i][j] = 0;
    }
    fitness[i] = 0;
    roulette[i] = 0;
  }
  for(i=0; i<N; i++){
    mask[i] = 0;
  }
  for(i=0; i<maxloop; i++){
    maxF[i] = 0;
    aveF[i] = 0;
  }
  
  // input AdjacencyMatrix
  // please use makeAdjacencyMatrix.c to make AdjacencyMatrix
  FILE *fp;
  char filename[256];
  // read filename
  if(prob == 0)
    sprintf(filename, "Matrix%dforDense.txt", N);
  else
    sprintf(filename, "Matrix%dforSparse.txt", N);
  // open file
  if((fp = fopen(filename, "r")) == NULL){
    printf("File open error.\n");
    exit(EXIT_FAILURE);
  }
  // input AdjecencyMatrix to graph
  char Buff[512];
  for(i = 0; i < N; i++){
    // reading 1 line
    fgets(Buff, sizeof(Buff), fp);
    for(j = 0; j < N*2; j++){
      // reading 1 char and char to int
      if(Buff[j] == 48)
	  graph[i][j/2] = 0;
	else if(Buff[j] == 49)
	  graph[i][j/2] = 1;
      // skip spase
      j++;
    }
  }
  // close
  fclose(fp);

  
  // start hybrid Genetic Algorithm
  // make initial value(input RED or GREEN or BLUE to parent)
  for(i=0; i<SoP; i++){
    for(j=0; j<N; j++){
      parent[i][j] = 3 * (rand()/(RAND_MAX+1.0));
    }
  }

  // init violation, fitness, max, ave, min
  // calc violation points, parent[i][N] = violation point
  for(l=0; l<SoP; l++){
    violation = 0;
    for(i=0; i<N; i++){      
      for(j=0; j<N; j++){
	if(graph[i][j] == 1 && parent[l][i] == parent[l][j]){
	  violation++;
	}
      }
    }
    parent[l][N] = violation;
    calcCount++;
  }
  
  // calc fitness, parent[i][N] = fitness, [0,1]
  sum = 0;
  for(i=0; i<SoP; i++){
    fitness[i] = 1.0 - ((double)parent[i][N] / (double)links[prob]);
    sum += fitness[i];
  }
  
  // input max, ave, min
  minF[0] = 1;
  sum = 0;
  for(i=0; i<SoP; i++){
    // max
    if(maxF[0] < fitness[i]){
      maxF[0] = fitness[i];
    }
    // ave
    sum += fitness[i];
    // min
    if(minF[0] > fitness[i]){
      minF[0] = fitness[i];
    }
  }
  aveF[0] = sum / SoP;


  // scaling
  // if scaling = 0 then nothing
  if(scaling == 1){ // then linear scaling
    sum = 0;
    for(i=0; i<SoP; i++){
      fitness[i] = (fitness[i] - minF[0]) / (maxF[0] - minF[0]);
      sum += fitness[i];
    }
  }
  if(scaling == 2){ // then power scaling
    sum = 0;
    for(i=0; i<SoP; i++){
      fitness[i] = pow(fitness[i], d);
      sum += fitness[i];
    }
  }

  // make roulette
  for(i=0; i<SoP; i++){
    if(i==0){
      roulette[i] = fitness[i] / sum;
    }else{
      roulette[i] = roulette[i-1] + fitness[i] / sum;
    }
  }
  
  // end determination
  if(maxF[0] == 1.0){
    printf("completed!! in init value\n");
    return 0;
  }
  
  //// start generation loop
  for(k=1; k<maxloop; k++){
    
    //// Genetic operation
    // make mask bit for crossing
    for(i=0; i<N; i++){
      mask[i] = 2 * (rand()/(RAND_MAX+1.0));
      if(mask[i] != 0 && mask[i] != 1){
	printf("error at make mask, mask bit = %d",mask[i]);
	return 1;
      }
    }
    
    // make children
    // select parents
    for(l=0; l<SoP; l++){
      sp1 = roulette[SoP-1] * (rand()/(RAND_MAX+1.0)); // select parent 1
      sp2 = roulette[SoP-1] * (rand()/(RAND_MAX+1.0)); // select parent 2

      // find selected parents index in parent[][]
      for(i=0; i<SoP; i++){
	if(sp1 <= roulette[i]){
	  sp1 = i;
	  break;
	}
      }
      for(i=0; i<SoP; i++){
	if(sp2 <= roulette[i]){
	  sp2 = i;
	  break;
	}
      }

      // make children by crossing and mutation.
      // crossing: if mask[j] = 0 then use sp1, if mask[j] = 1 then use sp2
      for(i=0; i<N; i++){
	if(mask[i] == 0){
	  children[l][i] = parent[(int)sp1][i];
	}
	else{
	  children[l][i] = parent[(int)sp2][i];
	}
      }

      // mutation: if mutant then change color (ex) if RED then GREEN or BLUE
      // first: make 1 or 2 (random) and plus to color
      // then: RED(0) -> 1 or 2, GREEN(1) -> 2 or 3, BLUE(2) -> 3 or 4
      // second: % 3 then RED -> 1 or 2, GREEN 2 or 0, BLUE -> 0 or 1
      for(i=0; i<N; i++){
	mutation = 100 * (rand()/(RAND_MAX+1.0));
	if(mutation <= mutationRate){
	  tmp = (2 * (rand()/(RAND_MAX+1.0))) + 1; // tmp = 1 or 2
	  children[l][i] = (int)(children[l][i] + tmp) % 3;
	}
      }
      // end make children
    }

    // make next generation
    for(i=0; i<SoP; i++){
      for(j=0; j<N; j++){
	parent[i][j] = children[i][j];
      }
    }

    //// calc violation
    // calc violation points, parent[i][N] = violation point
    for(l=0; l<SoP; l++){
      violation = 0;
      for(i=0; i<N; i++){      
	for(j=0; j<N; j++){
	  if(graph[i][j] == 1 && parent[l][i] == parent[l][j]){
	    violation++;
	  }
	}
      }
      parent[l][N] = violation;
      calcCount++;
    }
    
    // calc fitness, parent[i][N] = fitness, [0,1]
    sum = 0;
    for(i=0; i<SoP; i++){
      fitness[i] = 1.0 - ((double)parent[i][N] / (double)links[prob]);
      sum += fitness[i];
    }

    // input max, ave, min, and search top Ne gene
    for(i=0; i<Ne; i++){
      topNe[i] = 0;
    }
    minF[k] = 1;
    sum = 0;
    for(i=0; i<SoP; i++){
      // max
      if(maxF[k] < fitness[i]){
	maxF[k] = fitness[i];
      }
      // ave
      sum += fitness[i];
      // min
      if(minF[k] > fitness[i]){
	minF[k] = fitness[i];
      }
      // make topNe
      if(i < Ne){
	topNe[i] = i;
      }else{
	// search min in fitness[topNe[j]]
	tmpd = fitness[topNe[0]];
	tmp = 0;
	for(j=1; j<Ne; j++){
	  if(fitness[topNe[j]] < tmpd){
	    tmpd = fitness[topNe[j]];
	    tmp = j;
	  }
	}
	if(fitness[i] > tmpd){
	  topNe[j] = i;
	}
      }
    }
    aveF[k] = sum / SoP;

    // scaling
    // if scaling = 0 then nothing
    if(scaling == 1){ // then linear scaling
      sum = 0;
      for(i=0; i<SoP; i++){
	fitness[i] = (fitness[i] - minF[k]) / (maxF[k] - minF[k]);
	sum += fitness[i];
      }
    }
    if(scaling == 2){ // then power scaling
      sum = 0;
      for(i=0; i<SoP; i++){
	fitness[i] = pow(fitness[i], d);
	sum += fitness[i];
      }
    } 
    
    // make roulette
    for(i=0; i<SoP; i++){
      if(i==0){
	roulette[i] = fitness[i] / sum;
      }else{
	roulette[i] = roulette[i-1] + fitness[i] / sum;
      }
    }

    // end determination
    if(maxF[k] == 1.0){
      printf("completed!!  d = %d: %dloop\n", d, k);
      return 0;
    }

    //// HC start
    for(l=0; l<Ne; l++){
      for(i=0; i<N; i++){
	solution[i] = children[topNe[l]][i];
      }
      // hill climbing start
      for(m=0; m<maxloopH; m++){
	// init  violation and violationArray
	violation = 0;
	for(i=0; i<N; i++){
	  violationArray[i] = 0;
	}
	
	// calc Violation points and updaate violationArray
	for(i = 0; i < N; i++){
	  for(j = i + 1; j < N; j++){
	    if(graph[i][j] == 1 && solution[i] == solution[j]){
	      violation++;
	      violationArray[i] = 1;
	      violationArray[j] = 1;
	    }	
	  }
	}
	calcCount++;
	
	// end determination
	if(violation == 0){
	  printf("completed in HC!! %d loops, topNe = %d, clear at %d GA\n", m, l, k);
	  return 0;
	}
    
	// change variable
	changeVar = 0;
	// select variable, ex)[0,1,1,0,0,1]
	count = 0;
	for(i = 0; i < N; i++){
	  if(violationArray[i] == 1){ // pick up violation variable, ex)[1,1,1]
	    count++;
	  }
	}
	changeVar = (double)count * (rand()/(RAND_MAX+1.0)); // select random, ex)if changeVar = 1 then [1, changeVar, 1] 
	
	count = -1;   // becouse of origin 0. After first count++ , count = 0;
	for(i = 0; i < N; i++){
	  if(violationArray[i] == 1){
	    count++;
	  }
	  if(count == changeVar){
	    changeVar = i;  // input changeVar to violationArray, ex)[0,1,changeVar,0,0,1]
	    break;
	  }
	}
	
	// if color is change to XXX then violation is ??? point
	Rv = 0; // XXX = RED, ??? = Rv
	Gv = 0; // XXX = GREEN, ??? = Gv
	Bv = 0; // XXX = BLUE, ??? = Bv
	for(i = 0; i < N; i++){
	  if(graph[changeVar][i] == 1){ // if there is link (column)
	    if(RED == solution[i]){ // if RED
	      Rv++;
	    }
	    else if(GREEN == solution[i]){ // if GREEN
	      Gv++;
	    }
	    else if(BLUE == solution[i]){ // if BLUE
	      Bv++;
	    }
	  }
	  if(graph[i][changeVar] == 1){ // if there is link (row)
	    if(RED == solution[i]){ // if RED
	      Rv++;
	    }
	    else if(GREEN == solution[i]){ // if GREEN
	      Gv++;
	    }
	    else if(BLUE == solution[i]){ // if BLUE
	      Bv++;
	    }
	  }
	}
	calcCount++;
	
	//change value (use min violation coler)(if same then random)
	if(Rv < Bv){
	  if(Rv < Gv)
	    solution[changeVar] = RED;
	  else if(Gv < Rv)
	    solution[changeVar] = GREEN;
	  else if(Gv == Rv){ // RED = 0 or GREEN = 1
	    solution[changeVar] = 2.0 * rand()/(RAND_MAX+1.0); // (double)[0,2) -> (int)0 or 1
	    if(solution[changeVar] != 0 && solution[changeVar] != 1) // for debug
	      printf("error at Rv=Gv<Bv, color=%d\n",solution[changeVar]);
	  }
	}
	else if(Bv < Rv){
	  if(Bv < Gv)
	    solution[changeVar] = BLUE;
	  else if(Gv < Bv)
	    solution[changeVar] = GREEN;
	  else if(Gv == Bv){ // GRENN = 1 or BLUE = 2
	    solution[changeVar] = (2.0 * rand()/(RAND_MAX+1.0)) + 1.0; // (double)[0,2) + 1.0 -> (int)1 or 2
	    if(solution[changeVar] != 1 && solution[changeVar] != 2) // for debug
	      printf("error at Bv=Gv<Rv, color=%d\n",solution[changeVar]);
	  }
	}
	else if(Rv == Bv){
	  if(Gv < Rv)
	    solution[changeVar] = GREEN;
	  else if(Rv < Gv){ // RED = 0 or BLUE = 2
	    r = 2.0 * rand()/(RAND_MAX+1.0); // (double)[0,2) -> (int)0 or 1
	    if(r == 1){ // 0 or 1 -> if r=1 then r=2 -> 0 or 2
	      r = 2;  // 0 or 2
	    }
	    solution[changeVar] = r;
	    if(solution[changeVar] != 0 && solution[changeVar] != 2) // for debug
	      printf("error at Rv=Bv<Gv, color=%d\n",solution[changeVar]);
	  }
	  else if(Rv == Gv){ // RED = 0 or GREEN = 1 or BLUE = 2
	    solution[changeVar] = 3.0 * (rand()/(RAND_MAX+1.0)); // (double)[0,3) -> (int)0 or 1 or 2
	    if(solution[changeVar] != 0 && solution[changeVar] != 1 && solution[changeVar] != 2) // for debug
	      printf("error at Rv=Gv=Bv, color=%d\n",solution[changeVar]);
	  }
	}
	// end 1 step
      }
      
    }

    // end 1 genaration loop
  }

  /*   OUTPUT:printf("");

    // for sparse
    sprintf(filename, "violation%dSP.csv", N);
    // open file
    if((fp = fopen(filename, "w")) == NULL){
      printf("file open error.\n");
      exit(EXIT_FAILURE);
    }
    // write
    int loops;
    if(tmp < 100)
      loops = 100;
    else
      loops = (tmp % 100) * 100;

    for(i=0; i<loops; i++){
      fprintf(fp, "%d, ", i);
    }
    fprintf(fp, "%d\n", loops);

    for(i=0; i<=tmp; i++){
      fprintf(fp, "%d, ", maxVP[i]);
    }
    fprintf(fp, "\n");

    for(i=0; i<=tmp; i++){
      fprintf(fp, "%d, ", aveVP[i]);
    }
    fprintf(fp, "\n");

    for(i=0; i<=tmp; i++){
      fprintf(fp, "%d, ", minVP[i]);
    }
    fprintf(fp, "\n");

    // close
    fclose(fp);
  */
  printf("Genetic algorithm failed...\n");
  //  goto next;
 

  // for loop test
  /* next: count++;
  if(count == 10){
    int sum = 0;
    int tmp = 0;
    for(i=0;i<10;i++){
      if(loops[i] != 0){
	sum += loops[i];
	tmp++;
      }
    }
    if(sum != 0)
      printf("ave = %d, %d clear\n",(sum / tmp), tmp);
    return 0;
  }
  goto loop;
  */
  return 0;
}
