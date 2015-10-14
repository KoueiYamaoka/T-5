#include<stdio.h>
#include<stdlib.h>

#define TRUE 1

int main(void){

  //seed 509
  srand(509);

  // number if nodes
  const int N[5] = {30, 60, 90, 120, 150};
  int i, j, k; // for for loop

  for(k=0; k<5; k++){
    int SMat[N[k]][N[k]]; // AdjacencyMatrix for sparse
    int DMat[N[k]][N[k]]; // AdjacencyMatrix for dense
    int SLink = 3 * N[k]; // number of links for sparse
    int DLink = N[k] * (N[k] - 1) / 4;  // number of links for dense
    
    // init
    for(i=0; i<N[k]; i++){
      for(j=0; j<N[k]; j++){
	SMat[i][j] = 0;
	DMat[i][j] = 0;
      }
    }

    // make links
    // calc number of upper triangular elements
    int upperTriNum = 0;
    for(i=1; i<N[k]; i++){
      upperTriNum += i;
    }
    // make array for upper tri elem and init
    int SUpperTri[upperTriNum]; // for sparse
    int DUpperTri[upperTriNum]; // for dense
    for(i=0; i<upperTriNum; i++){
      SUpperTri[i] = 0;
      DUpperTri[i] = 0;
    }

    // choice random index for sparse
    int count = 0;
    int r;
    while(TRUE){
      r = upperTriNum * (rand()/(RAND_MAX+1.0));
      if(SUpperTri[r] != 1){
	SUpperTri[r] = 1;
	count++;
      }
      if(count == SLink){
	break;
      }
    }
    // choice random index for dense
    count = 0;
    while(TRUE){
      r =upperTriNum * (rand()/(RAND_MAX+1.0));
      if(DUpperTri[r] != 1){
	DUpperTri[r] = 1;
	count++;
      }
      if(count == DLink){
	break;
      }
    }

    // input SUpperTri to Adjacency Matrix
    count = 0;
    for(i=0; i<N[k]; i++){
      for(j=0; j<N[k]; j++){
	if(i < j){
	  SMat[i][j] = SUpperTri[count];
	  count++;
	}
      }
    }
    // input DUpperTri to Adjacency Matrix
    count = 0;
    for(i=0; i<N[k]; i++){
      for(j=0; j<N[k]; j++){
	if(i < j){
	  DMat[i][j] = DUpperTri[count];
	  count++;
	}
      }
    }

    // output
    FILE *fp;
    char filename[256];
    // for sparse
    sprintf(filename, "Matrix%dforSparse.txt", N[k]);
    // open file
    if((fp = fopen(filename, "w")) == NULL){
      printf("file open error.\n");
      exit(EXIT_FAILURE);
    }
    // write
    for(i=0; i<N[k]; i++){
      for(j=0; j<N[k]; j++){
	fprintf(fp, "%d ", SMat[i][j]);
      }
      fprintf(fp, "\n");
    }
    // close
    fclose(fp);

    // for dense
    sprintf(filename, "Matrix%dfordense.txt", N[k]);
    // open file
    if((fp = fopen(filename, "w")) == NULL){
      printf("file open error.\n");
      exit(EXIT_FAILURE);
    }
    // write
    for(i=0; i<N[k]; i++){
      for(j=0; j<N[k]; j++){
	fprintf(fp, "%d ", DMat[i][j]);
      }
      fprintf(fp, "\n");
    }
    // close
    fclose(fp);
  }
  return 0;
}
