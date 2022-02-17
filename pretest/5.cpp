#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int isprime(int);
int gcd(int , int);
void add(int*,int*, int);
void mul( int b, int n, int &outA, int &outB);
// #define Max 5
#define Max 6

int main()
{
	//找到所有非法質數
	int* prime_set = (int*)malloc(sizeof(int));
	int length_prime = 0;
	for (int index = 6; index < Max; index++)
		if (isprime(index))
			add(prime_set, &length_prime, index);
	
	//找到2,3,5的倍數
	int* mask = (int*)malloc(sizeof(int));
	int length_mask = 0;
	for (int index = 2; index < Max; index++)
		if (index % 2 == 0 || index % 3 == 0 || index % 5 == 0)
			add(mask, &length_mask, index);

	//濾除質數的倍數
	int* ans = (int*)malloc(sizeof(int));
	int length_ans = 0;
	for (int index = 0; index < length_mask; index++)
	{
		int flag = 1;
		for (int index2 = 0; index2 < length_prime; index2++)
		{
			if (mask[index] % prime_set[index2] == 0)
			{
				flag = 0;
				break;
			}
		}
		if (flag == 1)
			add(ans, &length_ans, mask[index]);
	}

	int outA = 0;
	int outB = 1;
	for (int index = 0; index < length_ans; index++)
	{
		mul(1, ans[index], outA, outB);
	}

	printf("%2d\n", outA);
	printf("--\n");
	printf("%2d\n", outB);

	free(prime_set);
	free(mask);
	getchar();
}

void mul( int b, int n,int &outA,int &outB){
	outA = (outA)*n + b* (outB);
	outB = (outB)*n;
	int x = gcd(outA, outB);
	outA = outA / x;
	outB = outB / x;

}

int gcd(int a, int b)
{
	return b == 0 ? a : gcd(b, a%b);
}

void add(int* result,int* length, int value){
	result = (int*)realloc(result, sizeof(int)*(*length + 1));
	result[*length] = value;
	*length += 1;
}

int isprime(int n){
	int i = 0;
	for (i = 2; i < n/2; i++)
	{
		if (n%i==0)
		{
			return false;
		}
	}
	return true;
}