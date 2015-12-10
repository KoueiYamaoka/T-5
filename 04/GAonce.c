#include<stdio.h>
#include<stdlib.h>
#include<math.h>


#define RED 0
#define GREEN 1
#define BLUE 2


int main(void){  
  
  // seed 509
  srand(509);
  
<<<<<<< HEAD
  int select = 0; // select size
  int prob = 0; // if 0 then dense, if 1 then sperse
  int SoP = 500; // size of population 
  double mutationRate = 0.1; // mutation rate (%)
  int scaling = 2; // scaling. if 0 then nothing, 1 then linear, 2 then power
  int d = 32; // for power scaling. pow(x, d)
  int minv = 0;
=======
  int select = 2; // select size
  int prob = 1; // if 0 then dense, if 1 then sperse
  int SoP = 500; // size of population 
  double mutationRate = 3; // mutation rate (%)
  int scaling = 2; // scaling. if 0 then nothing, 1 then linear, 2 then power
  int d = 1; // for power scaling. pow(x, d)
  int linear = 0;
  int name = 530;
>>>>>>> d8b0f46b4b0818c561193731d84c98df76fb85be

  int count = 0;
  int loops[10];

  const int size[5] = {30, 60, 90, 120, 150}; // number of nodes
  const int N = size[select]; // which to use? NN[0] to NN[4] or something int value
  const int links[2] = {N * (N - 1) / 4 // number of links for dense
			, 3 * N}; // number of links for sparse
  const int maxloop = 2000; // the maximum number of iterations for ES
  const int seeds[10] = {509, 521, 523, 541, 547, 557, 563, 569, 571, 577}; // seeds

  int graph[N][N];  // AdjacencyMatrix
  int parent[SoP][N+1]; // (parent solution + violation point or fitness) * SoP
  int children[SoP][N+1]; // (children solution + violation point or fitness) * SoP
  int violation = 0; // violation point
  double fitness[SoP]; // fitness
  double roulette[SoP]; // roulette for select parents
  int mask[N]; // mask bit for crossing
  double maxF[10][maxloop]; // max fitness for output
  double aveF[10][maxloop]; // ave fitness for output
  double minF[10][maxloop]; // min fitness for output
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
  for(j=0; j<10; j++){
    for(i=0; i<maxloop; i++){
      maxF[j][i] = 0;
      aveF[j][i] = 0;
    }
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

 loop:srand(seeds[count]);
  printf("seeds=%d: ",seeds[count]);

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
    minF[count][k] = 1;
    sum = 0;
    for(i=0; i<SoP; i++){
      // max
      if(maxF[count][k] < fitness[i]){
	maxF[count][k] = fitness[i];
      }
      // ave
      sum += fitness[i];
      // min
      if(minF[count][k] > fitness[i]){
	minF[count][k] = fitness[i];
      }
    }
    aveF[count][k] = sum / SoP;
    // for debug
    // printf("maxF[%d] = %f, aveF[%d] = %f, minF[%d] = %f\n", k, maxF[k], k, aveF[k], k, minF[k]);
    

    // scaling
    // if scaling = 0 then nothing
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
    if(maxF[count][k] == 1.0){
      printf("completed!!  d = %d: %dloop, max = %f\n", d, k, maxF[count][k]);
      loops[count] = k;
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
  
  printf("Genetic algorithm failed... maxF=%f, \n", maxF[count][maxloop-1]);
  loops[count] = maxloop-1;
  goto next;
  
 OUTPUT:printf(" ");
  
<<<<<<< HEAD
  // for sparse
  sprintf(filename, "max%dD%d.csv", N, seeds[minv]);
=======
    // for sparse
  sprintf(filename, "fitness%dS%d.csv", N, name);
>>>>>>> d8b0f46b4b0818c561193731d84c98df76fb85be
  // open file
  if((fp = fopen(filename, "w")) == NULL){
    printf("file open error.\n");
    exit(EXIT_FAILURE);
  }
  // write
  for(i=0; i<loops[minv]; i++){
    fprintf(fp, "%d, ", i);
  }
  fprintf(fp, "%d\n", loops[minv]);

  for(i=0; i<=loops[minv]; i++){
    fprintf(fp, "%f, ", maxF[minv][i]);
  }
  fprintf(fp, "\n");
  
  for(i=0; i<=loops[minv]; i++){
    fprintf(fp, "%f, ", aveF[minv][i]);
  }
  fprintf(fp, "\n");
  for(i=0; i<=loops[minv]; i++){
    fprintf(fp, "%f, ", minF[minv][i]);
  }
  fprintf(fp, "\n");
  
  // close
  fclose(fp);
  return 0;

  
  // for loop test
 next: count++;
  if(count == 10){
    double sum = 0;
    int ls = 0;
    double ave = 0;
    double sig = 0;
    double ssig = 0;
    double max = 0;
    for(i=0;i<10;i++){
      sum += maxF[i][loops[i]];
      ls += loops[i];
      printf("%f\n", maxF[i][loops[i]]);
      if(max < maxF[i][loops[i]]){
	max = maxF[i][loops[i]];
	minv = i;
      }
    }
    ave = sum / 10;
    ls /= 10;
    for(i=0; i<10; i++){
      sig += pow((maxF[i][loops[i]] - ave), 2);
    }
    sig = sig / 10;
    ssig = sqrt(sig);
    if(sum != 0)
      printf("sum=%f, ave=%f, sig=%f, ssig=%f, ls=%d end\n", sum, ave, sig, ssig, ls);
    goto OUTPUT;
        
    return 0;
  }
  goto loop;

  return 0;
}
