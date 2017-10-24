long long array[20] = { 1,3,8,12,24,14,4321,2,543,756,423,324,6754,45,12,54,56,87,33,32 };

long long partition(long long *array, long long l, long long r)
{
	long long x = array[l];
	long long i = l + 1;
	long long j = r;
	while (1)
	{
		while (array[j]>x) j--;
		while (array[i] <= x) i++;
		if (i<j)
		{
			long long temp = array[i];
			array[i] = array[j];
			array[j] = temp;
		}
		else return j;
	}
}
void qsort(long long * array, long long l, long long r)
{
	if (l<r)
	{
		long long q = partition(array, l, r);
		long long temp = array[l];
		array[l] = array[q];
		array[q] = temp;
		if (q>l) qsort(array, l, q - 1);
		if (q<r) qsort(array, q + 1, r);
	}
}

long long main()
{
	qsort(array, 0, 19);
	return 0;
}

