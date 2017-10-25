#include <stdio.h>
//#include <time.h>
#include<stdlib.h>
//clock_t begin, end;
/*
clock()函数捕捉程序从开始运行到调用clock()函数时耗费的时间，时间单位为“时钟打点”(clock tick)
常数CLK_TCK是机器时钟每秒走的时钟打点数
*/

//double duration;
// 纪录被测函数运行时间， 单位为秒
long long a[5][5] = { { 1,2,3,4,5 },
	{ 6,7,8,9,10 },
	{ 11,12,13,14,15 },
	{ 16,17,18,19,20 },
	{ 21,22,23,24,25 } };
long long b[5][5] = { { 1,2,3,4,5 },
	{ 6,7,8,9,10 },
	{ 11,12,13,14,15 },
	{ 16,17,18,19,20 },
	{ 21,22,23,24,25 } };
long long c[5][5] = { { 0,0,0,0,0 },
	{ 0,0,0,0,0 },
	{ 0,0,0,0,0 },
	{ 0,0,0,0,0 },
	{ 0,0,0,0,0 } };
void MatrixMulti()
{
	int i, j, k;
	long temp;
	
	for (i = 0; i < 5; i++)
	{
		for (j = 0; j < 5; j++)
		{
			temp = 0;
			for (k = 0; k < 5; k++)
			{
				temp += a[i][k] * b[k][j];
			}
			c[i][j] = temp;
		}
	}
}
int main()
{
	//begin = clock();
	for (int i = 0; i < 1; i++)
	{
		MatrixMulti();
	}
	//end = clock();
	//duration = ((double)(end - begin)) * 1000 / CLK_TCK;
	//printf("%f\n", duration);
	//system("pause");
	return 0;

}
