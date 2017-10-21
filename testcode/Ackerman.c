#include <stdio.h>
#include <time.h>
#include <stdlib.h>
clock_t begin, end;
/*
clock()函数捕捉程序从开始运行到调用clock()函数时耗费的时间，时间单位为“时钟打点”(clock tick)
常数CLK_TCK是机器时钟每秒走的时钟打点数
*/

double duration;
// 纪录被测函数运行时间， 单位为秒
int Ack(int m,int n)
{
	if (m == 0)
		return(n + 1);
	else if (n == 0)
		return(Ack(m - 1, 1));
	else
		return(Ack((m - 1), Ack(m, n - 1)));
}
int main()
{
	begin = clock();
	for (int i = 0; i < 1000000; i++)
	{
		Ack(3, 3);
	}
	end = clock();
	duration = ((double)(end - begin))*1000/ CLK_TCK;
	printf("%f\n", duration);
	system("pause");
	return 0;

}
