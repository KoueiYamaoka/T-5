#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<math.h>

#define RED 0
#define GREEN 1
#define BLUE 2

int main(void){

  // seed 509
  srand(509);
  int select = 4;
  int loopCount = 0; // for result
  // for output violation
  /*  int ii=0;
  int v = 3573;
  int v2 = 3750;
  int outputVio[v];
  for(ii=0; ii<v; ii++){
    outputVio[ii]=0;
    }*/
  const int seeds[10] = {509, 521, 523, 541, 547, 557, 563, 569, 571, 577}; // seeds
  int loops = 0;
  int calcs[100];
  int calcCount = 0;

  const int size[6] = {30, 60, 90, 120, 150, 180}; // number of nodes
  const int N = size[select]; // which to use? NN[0] to NN[4] or something int value
  const int maxloop = 2000; // the maximum number of iterations for HC algorithm
  const int maxloopIHC = 500;

  int graph[N][N]; // graph
  int solution[N];  // solution candidates
  int violation = 0; // violation points
  int violationArray[N]; // if violation=1 then 1 else 0
  int Rv = 0;  // RED violation
  int Gv = 0;  // GREEN violation
  int Bv = 0;  // BLUE violation
  int changeVar = 0; // change value

  int i, j, k,l; // for for loop
  int r = 0;  // random number
  int count = 0; // for count
  int tmpVio = INT_MAX; // for debug
  

  //input AdjacencyMatrix for sparse
  FILE *fp;
  char filename[256];
  // read filename
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

  for(i=0; i<100; i++){
    calcs[i] = 0;
  }
  
 loop:
  if(loops%10 == 0)
    srand(seeds[loops/10]);
  // IHC start
  for(l=0; l<maxloopIHC; l++){
  
  ////// HC algorithm
  // for debug
    tmpVio = INT_MAX;

  // make initial value(input RED or GREEN or BLUE)
  for(i=0; i<N; i++){
    solution[i] = 3.0 * (rand()/(RAND_MAX+1.0));
    if(solution[i] != 0 && solution[i] != 1 && solution[i] != 2) // for debug
      printf("error at init solution, num=%d\n",solution[i]);
  }
  
  // hill climbing start
  for(k=0; k<maxloop; k++){
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

    if(violation > tmpVio){
      printf("Rv=%d, Gv=%d, Bv=%d, color=%d\n", Rv, Gv, Bv, solution[changeVar]);
      printf("error\n");
      exit(EXIT_FAILURE);
    }
    tmpVio = violation;
    
    // end determination
    if(violation == 0){
      loopCount++;
      printf("completed!! %d loops, clear at %d\n\n", k, loopCount);
      goto next;
      // return 0;
    }
    
    //// change variable
    changeVar = 0;
    // select variable, ex)[0,1,1,0,0,1]
    count = 0;
    for(i = 0; i < N; i++){
      if(violationArray[i] == 1){ // pick up violation variable, ex)[1,1,1]
	count++;
      }
    }
    changeVar = (double)count * (rand()/(RAND_MAX+1.0)); // select random, ex)if changeVar = 1 then [1, changeVar, 1] 

    if(changeVar >= count) // for debug
      printf("error at random select 'changeVar', count=%d, changeVar=%d\n", count, changeVar);

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
  loopCount++;
  printf("%d:can't solved. violarion = %d\n", loops, violation);
  }
  printf("IHC faild...\n");
  calcCount = 0;
  goto next;  // for debug
  // return 0;

  // for loop test
 next: puts(" ");
  calcs[loops] = calcCount;
  printf("calc = %d", calcCount);
  calcCount = 0;
  printf("%d, ",loops);
  loops++;
  if(loops == 100){
    int sum = 0;
    int tmp = 0;
    printf("calcs = ");
    for(i=0;i<100;i++){
      if(calcs[i] != 0){
	printf("%d:%d, ",i, calcs[i]);
	sum += calcs[i];
	tmp++;
      }
    }
    puts("");
    double sig = 0;
    double ave = sum / tmp;
    tmp = 0;
    for(i=0;i<100;i++){
      if(calcs[i] != 0){
	sig += (calcs[i] - ave) * (calcs[i] - ave);
	tmp++;
      }
    }
    sig = sqrt((sig/tmp));
    if(sum != 0){
      printf("ave = %f, sig = %f, %d clear\n",ave, sig, tmp);
      return 0;
    }
  }
  goto loop;

  return 0;
}
