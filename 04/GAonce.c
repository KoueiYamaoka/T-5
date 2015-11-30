#include<stdio.h>
#include<stdlib.h>
#include<math.h>


#define RED 0
#define GREEN 1
#define BLUE 2


int main(void){  
  
  // seed 509
  srand(509);
  
  int select = 2; // select size
  int prob = 0; // if 0 then dense, if 1 then sperse
  int SoP = 2000; // size of population 
  double mutationRate = 0.5; // mutation rate (%)
  int scaling = 2; // scaling. if 0 then nothing, 1 then linear, 2 then power
  int d = 1; // for power scaling. pow(x, d)
  int linear = 105;
  int name = 2005;

  int count = 0;
  int loops[49];

  const int size[5] = {30, 60, 90, 120, 150}; // number of nodes
  const int N = size[select]; // which to use? NN[0] to NN[4] or something int value
  const int links[2] = {N * (N - 1) / 4 // number of links for dense
			, 3 * N}; // number of links for sparse
  const int maxloop = 2000; // the maximum number of iterations for ES
  //  const int seeds[10] = {509, 521, 523, 541, 547, 557, 563, 569, 571, 577}; // seeds

  int graph[N][N];  // AdjacencyMatrix
  int parent[SoP][N+1]; // (parent solution + violation point or fitness) * SoP
  int children[SoP][N+1]; // (children solution + violation point or fitness) * SoP
  int violation = 0; // violation point
  double fitness[SoP]; // fitness
  double roulette[SoP]; // roulette for select parents
  int mask[N]; // mask bit for crossing
  double maxF[maxloop]; // max fitness for output
  double aveF[maxloop]; // ave fitness for output
  double minF[maxloop]; // min fitness for output
  double sum = 0;// for maxF, aveF, minF, roulette
  double sp1 = 0; // selected parent 1
  double sp2 = 0; // selected parent 2
  double mutation = 0; // mutation probability
  
  int i, j, k, l; // for for loop
  int tmp = 0;

  
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
  
  // Input AdjacencyMatrix
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

 loop:srand(509);
  printf("d=%d: ",d);

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

  
  // start Genetic Algorithm
  // make initial value(input RED or GREEN or BLUE to parent)
  for(i=0; i<SoP; i++){
    for(j=0; j<N; j++){
      parent[i][j] = 3 * (rand()/(RAND_MAX+1.0));
    }
  }
  
  // start generation loop
  for(k=0; k<maxloop; k++){

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
    }
    
    // calc fitness, parent[i][N] = fitness, [0,1]
    sum = 0;
    for(i=0; i<SoP; i++){
      fitness[i] = 1.0 - ((double)parent[i][N] / (double)links[prob]);
      sum += fitness[i];
    }

    // input max, ave, min
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
    }
    aveF[k] = sum / SoP;
    // for debug
    // printf("maxF[%d] = %f, aveF[%d] = %f, minF[%d] = %f\n", k, maxF[k], k, aveF[k], k, minF[k]);
    

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

      loops[d-1] = k;
      goto next;
      //      return 0;
    }

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

      // for debug
      if(sp1 > roulette[SoP-1])
	puts("erroe at sp1");
      if(sp2 > roulette[SoP-1])
	puts("error at sp2");
      
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
	  //	  printf("mask[%d] = %d, children[%d][%d] = %d, parent[%d][%d] = %d\n"
	  //		 ,i ,mask[i] , l, i, children[l][i], (int)sp1, i, parent[(int)sp1][i]);
	}
	else{
	  children[l][i] = parent[(int)sp2][i];
	  //	  printf("mask[%d] = %d, children[%d][%d] = %d, parent[%d][%d] = %d\n"
	  //		 , i, mask[i], l, i, children[l][i], (int)sp2, i, parent[(int)sp2][i]);
	}
      }

      // mutation: if mutant then change color (ex) if RED then GREEN or BLUE
      // first: make 1 or 2 (random) and plus to color
      // then: RED(0) -> 1 or 2, GREEN(1) -> 2 or 3, BLUE(2) -> 3 or 4
      // second: % 3 then RED -> 1 or 2, GREEN 2 or 0, BLUE -> 0 or 1
      /*      printf("B: children[%d] = ",l);
      for(i=0; i<N; i++){
	printf("%d, ",children[l][i]);
      }
      puts("");*/
      for(i=0; i<N; i++){
	mutation = 100 * (rand()/(RAND_MAX+1.0));
	if(mutation <= mutationRate){
	  tmp = (2 * (rand()/(RAND_MAX+1.0))) + 1; // tmp = 1 or 2
	  children[l][i] = (int)(children[l][i] + tmp) % 3;
	  // for debug
	  if(children[l][i] != 0 && children[l][i] != 1 && children[l][i] != 2)
	    puts("error at mutation");
	}
      }
      /*      printf("A: children[%d] = ",l);
      for(i=0; i<N; i++){
	printf("%d, ",children[l][i]);
      }
      puts("");*/
      // end make children
    }
    // make next generation
    for(i=0; i<SoP; i++){
      for(j=0; j<N; j++){
	parent[i][j] = children[i][j];
      }
    }
    // end 1 genaration loop
  }
  
  printf("Genetic algorithm failed...\n");
  loops[d-1] = 0;
  goto next;
  
 OUTPUT:printf(" ");
  
    // for sparse
  sprintf(filename, "fitness%dD%d.csv", N, name);
  // open file
  if((fp = fopen(filename, "w")) == NULL){
    printf("file open error.\n");
    exit(EXIT_FAILURE);
  }
  // write
  for(i=1; i<50; i++){
    fprintf(fp, "%d, ", i);
  }
  fprintf(fp, "50\n");

  for(i=0; i<50; i++){
    fprintf(fp, "%d, ", linear);
  }
  fprintf(fp, "\n");
  
  for(i=0; i<50; i++){
    fprintf(fp, "%d, ", loops[i]);
  }
  fprintf(fp, "\n");
  
  // close
  fclose(fp);
  return 0;

  
  // for loop test
 next: d++;
  if(d == 51){
    goto OUTPUT;
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

  return 0;
}
