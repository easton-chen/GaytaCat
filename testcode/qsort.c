#include<stdio.h>
#include<time.h>
#include<stdlib.h>
clock_t begin, end;
/*
clock()函数捕捉程序从开始运行到调用clock()函数时耗费的时间，时间单位为“时钟打点”(clock tick)
常数CLK_TCK是机器时钟每秒走的时钟打点数
*/

double duration;
//记录被测函数运行时间，单位为秒

int partition(int *array, int l, int r)
{
	int x = array[l];
	int i = l + 1;
	int j = r;
	while (1)
	{
		while (array[j]>x) j--;
		while (array[i] <= x) i++;
		if (i<j)
		{
			int temp = array[i];
			array[i] = array[j];
			array[j] = temp;
		}
		else return j;
	}
}
void qsort(int * array, int l, int r)
{
	if (l<r)
	{
		int q = partition(array, l, r);
		int temp = array[l];
		array[l] = array[q];
		array[q] = temp;
		if (q>l) qsort(array, l, q - 1);
		if (q<r) qsort(array, q + 1, r);
	}
}

int main()
{
	begin = clock();
	for (int i = 0; i<1000000; ++i)
	{
		int array[20] = { 2,3,8,12,24,14,4321,2,543,756,423,324,6754,45,12,54,56,87,33,32 };
		qsort(array, 0, 19);
	}
	end = clock();

	duration = ((double)(end - begin) * 1000) / CLK_TCK;
	printf("%f\n", duration);
	system("pause");
	return 0;
}

