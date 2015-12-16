#include<stdio.h>
#include<stdlib.h>
#include<limits.h>

#define RED 0
#define GREEN 1
#define BLUE 2

// for qsort
int compare_int(const void *a, const void *b){
  return *(int*)a - *(int*)b;
}

// main
int main(void){
  
  
  // seed 509
  srand(509);
  
  int select = 1;
  int plus = 1; // if 0 then (lambda,mu)-ES, if 1 then (lambda+mu)-ES
  int mu = 10;// number of parents
  int lambda = 50; // number of children
  int count = 0;
  int aveCount[10] = {0,0,0,0,0,0,0,0,0,0};
  const int seeds[10] = {509, 521, 523, 541, 547, 557, 563, 569, 571, 577}; // seeds
  int calcCount = 0;
  int loops = 0;
  int calcs[10] = {0,0,0,0,0,0,0,0,0,0};
  
 loop: srand(seeds[loops]);

  const int size[5] = {30, 60, 90, 120, 150}; // number of nodes
  const int N = size[select]; // which to use? NN[0] to NN[4] or something int value
  const int maxloop = 1000; // the maximum number of iterations for ES

  printf("N=%d, plus=%d: ",N,plus);

  int graph[N][N];  // AdjacencyMatrix
  int parent[mu][N+1]; // (parent solution + violation point) * mu
  int children[lambda][N+1]; // (children solution + violation point) * lambda
  int tmpParent[mu][N+1]; // use for copy something to parent[][]
  int violation = 0; // violation point
  int maxVP[maxloop]; // max violation point in children for output
  int aveVP[maxloop]; // ave violation point in children for output
  int minVP[maxloop]; // min violation point in children for output
  int sum = 0;// for maxVP, aveVP, minVP
  int sortedArrayC[lambda]; // use for sort children
  int sortedArrayCP[mu + lambda]; // use for sort parent and children
  int sp = 0; // selected parent
  int mutantGene = 0; // selected mutant gene

  int i, j, k, l; // for for loop
  int tmp = 0;

  //init
  for(i=0; i<mu; i++){
    for(j=0; j<N; j++){
      parent[i][j] = 0;
      tmpParent[i][j] = 0;
    }
  }
  for(i=0; i<lambda; i++){
    for(j=0; j<N; j++){
      children[i][j] = 0;
    }
    sortedArrayC[i] = 0;
  }
  for(i=0; i<mu + lambda; i++){
    sortedArrayCP[i] = 0;
  }
  for(i=0; i<maxloop; i++){
    maxVP[i] = 0;
    aveVP[i] = 0;
    minVP[i] = 0;
  }
  
 
  
  // input AdjacencyMatrix
  // please use makeAdjacencyMatrix.c to make AdjacencyMatrix
  FILE *fp;
  char filename[256];
  // read filename
  sprintf(filename, "Matrix%dforDense.txt", N);
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

  // start Evolution Strategy
  // make initial value(input RED or GREEN or BLUE to solutions)
  for(i=0; i<mu; i++){
    for(j=0; j<N; j++){
      parent[i][j] = 3.0 * (rand()/(RAND_MAX+1.0));
      if(parent[i][j] != 0 && parent[i][j] != 1 && parent[i][j] != 2) // for debug
	printf("error at init solution, num=%d\n",parent[i][j]);
    }
  }



   // calc violation points
  for(l=0; l<mu; l++){
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

  // start generation loop
  for(k=0; k<maxloop; k++){
    
    //make children
    for(l=0; l<lambda; l++){
      sp = mu * (rand()/(RAND_MAX+1.0)); // select parent
      mutantGene = N * (rand()/(RAND_MAX+1.0)); // select mutant gene
      
      // copy parent[sp] to children[l]
      for(i=0; i<N; i++){
	children[l][i] = parent[sp][i];
      }
      // mutation
      children[l][mutantGene] = 3 * (rand()/(RAND_MAX+1.0)) + 1.0;
      /*
      if(children[l][mutantGene] == RED){ // if RED(0) then GREEN(1) or BRUE(2)
	children[l][mutantGene] = 2 * (rand()/(RAND_MAX+1.0)) + 1.0;
	if(children[l][mutantGene] != 1 && children[l][mutantGene] != 2){ // for debug
	  printf("error at mutation(RED)\n");
	  return 1;
	}
      }
      else if(children[l][mutantGene] == GREEN){ // if GREEN(1) then RED(0) or BLUE(2)
	tmp = 2 * (rand()/(RAND_MAX+1.0));
	if(tmp == 1)
	  tmp = 2; // tmp = RED(0) or BLUE(2)
	children[l][mutantGene] = tmp;
	if(children[l][mutantGene] != 0 && children[l][mutantGene] != 2){ // for debug
	  printf("error at mutation(GREEN)\n");
	  return 1;
	}
      }
      else if(children[l][mutantGene] == BLUE){ // if BLUE(2) then RED(0) or GREEN(1)
	children[l][mutantGene] = 2 * (rand()/(RAND_MAX+1.0));
	if(children[l][mutantGene] != 0 && children[l][mutantGene] != 1){ // for debug
	  printf("error at mutation(BLUE)\n");
	  return 1;
	}
      }
      else{
	printf("error at mutation\ncolor=%d", children[l][mutantGene]);
	return 1;
      }
      */
      // end make children
    }
    
    // calc violation point
    sum = 0;
    for(l=0; l<lambda; l++){
      violation = 0;
      for(i=0; i<N; i++){      
	for(j=0; j<N; j++){
	  if(graph[i][j] == 1 && children[l][i] == children[l][j]){
	    violation++;
	  }	
	}
      }
      children[l][N] = violation;
      calcCount++;
      sum += violation;
      if(plus == 0)
	sortedArrayC[l] = violation;
      else if(plus == 1)
	sortedArrayCP[l] = violation;
      else
	printf("error at input violation to sortedArray.\n");
    }

    // make Array for (mu + lambda)-ES
    if(plus == 1){
      for(i=lambda; i<mu + lambda; i++){
	sortedArrayCP[i] = parent[i - lambda][N];
	sum += parent[i - lambda][N];
      }
    }
    // sort by violation point
    if(plus == 0)
      qsort(sortedArrayC, lambda, sizeof(int), compare_int);
    else if(plus == 1)
      qsort(sortedArrayCP, (mu + lambda), sizeof(int), compare_int);
    else
      printf("error at sort by violation.\n");

    // input gene's index from to sortedArray
    // (mu, lambda)-ES, input index. ex) if sortedArrayC[0] = 5 then it means children[5][N] 
    if(plus == 0){
      for(i=0; i<lambda; i++){
	for(j=0; j<lambda; j++){
	  if(sortedArrayC[i] == children[j][N]){
	    sortedArrayC[i] = j;
	    children[j][N] *= -1; // check to selected gene
	    break;
	  }
	}
      }
    }

    // (mu + lambda)-ES, input index. ex) if sortedArrayCP[0] = 5 then it means children[5][N]
    // ex2) if sortedArrayCP[0] = -5 then it means parent[4][N]
    else if(plus == 1){
      for(i=0; i<mu + lambda; i++){
	for(j=0; j<lambda; j++){
	  if(j<mu){
	    if(sortedArrayCP[i] == children[j][N]){
	      sortedArrayCP[i] = j;
	      children[j][N] *= -1; // check to selected gene
	      break;
	    }else if(sortedArrayCP[i] == parent[j][N]){
	      sortedArrayCP[i] = -(j+1);
	      parent[j][N] *= -1; // check to selected gene
	      break;
	    }
	  }else{
	    if(sortedArrayCP[i] == children[j][N]){
	      sortedArrayCP[i] = j;
	      children[j][N] *= -1; // check to selected gene
	      break;
	    }
	  }
	}
      }
    }
    else
      puts("error at sort and input index.\n");
    // remove check
    if(plus == 0){
      for(i=0; i<lambda; i++){
	children[i][N] *= -1;
      }
    }
    else if(plus == 1){
      for(i=0; i<lambda; i++){
 	if(i<mu){
	  children[i][N] *= -1;
	  parent[i][N] *= -1;	  
	}
	else{
	  children[i][N] *= -1;
	}
      }
    }
    // end sort

    // input max, ave, min
    if(plus == 0){
      maxVP[k] = children[sortedArrayC[lambda - 1]][N];
      aveVP[k] = sum / lambda;
      minVP[k] = children[sortedArrayC[0]][N];
    }
    else if(plus == 1){
      if(sortedArrayCP[lambda + mu - 1] >= 0)
	maxVP[k] = children[sortedArrayCP[lambda + mu -1]][N];
      else
	maxVP[k] = parent[-(sortedArrayCP[lambda + mu - 1] + 1)][N];

      aveVP[k] = sum / (lambda + mu);

      if(sortedArrayCP[0] >= 0)
	minVP[k] = children[sortedArrayCP[0]][N];
      else
	minVP[k] = parent[-(sortedArrayCP[0] + 1)][N];      
    }
    else
      puts("error at input max, ave, min");

    // end determination
    if(minVP[k] == 0){
      printf("completed!!  %dloop\n", k);
      aveCount[count] = k;
      goto next;
      //tmp = k;
      //goto OUTPUT;
      //return 0;
    }

    // make next generation
    if(plus == 0){
      for(i=0; i<mu; i++){
	for(j=0; j<=N; j++){
	  parent[i][j] = children[sortedArrayC[i]][j];
	}
      }
    }
    else if(plus == 1){
      for(i=0; i<mu; i++){
	if(sortedArrayCP[i] >= 0){
	  for(j=0; j<=N; j++){
	    tmpParent[i][j] = children[sortedArrayCP[i]][j];
	  }
	}
	else{
	  for(j=0; j<=N; j++){
	    tmpParent[i][j] = parent[-(sortedArrayCP[i] + 1)][j];
	  }
	}
      }
      for(i=0; i<mu; i++){
	for(j=0; j<=N; j++){
	  parent[i][j] = tmpParent[i][j];
	}
      }
    }
  }
  // end generation loop


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
  printf("ES algorithm failed...\n");
  calcCount=0;
  goto next;
 

  // for loop test
 next: puts(" ");
  calcs[loops] = calcCount;
  calcCount = 0;
  printf("%d, ",loops);
  loops++;
  if(loops == 10){
    int sum = 0;
    int tmp = 0;
    printf("calcs = ");
    for(i=0;i<10;i++){
      if(calcs[i] != 0){
	printf("%d:%d, ",i, calcs[i]);
	sum += calcs[i];
	tmp++;
      }
    }
    puts("");
    if(sum != 0)
      printf("ave = %d, %d clear\n",(sum / tmp), tmp);
    return 0;
  }
  goto loop;

  return 0;
}
