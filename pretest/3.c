#include <stdio.h>
#include <stdlib.h>
int* path;
int pathTop;
int** ans;
int ansTop;
int* length;

void backtracking(int target, int index, int* candidates, int candidatesSize, int sum){
    if(sum>target){
        return;
    }
    if(sum==target){
        int* tempPath = (int*)malloc(sizeof(int)*pathTop);
        for(int j=0; j<pathTop; j++){
            tempPath[j] = path[j];
        }
        ans[ansTop] = tempPath;
        length[ansTop++] = pathTop;
    }
    for(int i=index; i<candidatesSize; i++){
        sum+=candidates[i];
        path[pathTop++] = candidates[i];
        backtracking(target, i, candidates, candidatesSize, sum);
        sum-=candidates[i];
        pathTop--;
    }
}
int main(){
    int candidates[] = {1,2,3,4,5,6};
    int candidatesSize=sizeof(candidates)/sizeof(candidates[0]);
    int target = 15;
    path = (int*)malloc(sizeof(int) * 50);
    ans = (int**)malloc(sizeof(int*) * 200);
    length = (int*)malloc(sizeof(int) * 200);
    ansTop = pathTop = 0;
    backtracking(target, 0, candidates, candidatesSize, 0);
    
    for(int i=0; i<ansTop; i++){
        if(length[i]==5){
            // printf("Combine: %d ", i);
            for(int j=0; j<length[i]; j++){
                printf("%d ", ans[i][j]);
            }
            printf("\n");
        }
    }
}