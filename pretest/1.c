#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX 1000

void big_add(int* rst, int* a, int* b)
{
	int i, sum, carry;
	for (carry = i = 0; i < MAX; ++i) {
		rst[i] = a[i] + b[i] + carry;
		carry = rst[i] / 10;
		rst[i] %= 10;
	}
}

void read_from_int(int* big, int x)
{
	int i = 0;
	memset(big, 0, sizeof(int) * MAX); // ±N big ¥ý²M 0
	while (x != 0) {
		big[i++] = x % 10;
		x /= 10;
	}
}

void big_mul(int* rst, int* a, int* b)
{
	int i, j, carry;
	memset(rst, 0, sizeof(int) * MAX); // ¥ý²M0
	for (i = 0; i < MAX; ++i) {
		if (a[i] == 0) continue;
		for (j = 0; i + j < MAX; ++j)
			rst[i + j] += a[i] * b[j];
	}
	// €@Šž©ÊœÕŸã
	for (carry = i = 0; i < MAX; ++i) {
		rst[i] += carry;
		carry = rst[i] / 10;
		rst[i] %= 10;
	}
}

void big_print(int* big)
{
	int i = MAX - 1;
	for (i = MAX - 1; i > 0 && big[i] == 0; --i);
	while (i >= 0) printf("%d", big[i]), --i;
	printf("\n");
}

int main()
{
	int sum[MAX];
	int tmp1[MAX];
	int tmp2[MAX];
	int a[MAX];
	int i = 10;
	memset(sum, 0, sizeof(int) * MAX);
	memset(tmp1, 0, sizeof(int) * MAX);
	memset(tmp2, 0, sizeof(int) * MAX);

	while (i > 0)
	{
		read_from_int(a, i);
		int j = i;
		int k;
		for (k = 0; k < MAX; k++)
		{
			tmp1[k] = a[k];
		}

		while (j > 1)
		{
			big_mul(tmp2, tmp1, a);
			for (k = 0; k < MAX; k++)
			{
				tmp1[k] = tmp2[k];
			}
			j--;
		}
		if (i == 1)
		{
			for (k = 0; k < MAX; k++)
			{
				tmp2[k] = a[k];
			}
		}
		big_print(tmp2);
		big_add(sum, sum, tmp2);
		i--;
	}
	big_print(sum);

	system("pause");
	return 0;
}