long long res = 0;

long long Ack(long long m,long long n)
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
	res = Ack(3, 3);
	return 0;
}
