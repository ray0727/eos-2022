#include <stdio.h>
int main(){
    unsigned long ans=0;
    for(int i=1; i<=10000; i++){
        ans = ans+i*i;
    }
    printf("ans = %lu\n", ans);
    printf("ans = %lu\n", ans*2);
}