#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <math.h>

int main(int argc, char *argv[])
{
    	int i;     int fd;	float *map;

	/* Open a file for reading and writing.*/
	fd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);

	if (fd == -1)
	{
		perror("Error opening file for writing.");
		exit(EXIT_FAILURE);
    	}

	struct stat sb;
	if (fstat(fd, &sb) == -1)
	{
		perror("Could not get file size.");
	}

	map = mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (map == MAP_FAILED)
    	{
        	close(fd);
        	perror("Error mmapping the file");
        	exit(EXIT_FAILURE);
    	}

	long NUMFLTS = sb.st_size;

	int p_cnt = 0;
	int n_cnt = 0;
	
	float pos[NUMFLTS];
	float neg[NUMFLTS];

	float res[NUMFLTS];

	for (i = 0; i < NUMFLTS; i++)
	{
		//All +ve flts
		if (signbit(map[i]) == 0)
		{
			pos[p_cnt] = map[i];
			p_cnt++;
		}
		//All -ve flts
		else
		{
			neg[n_cnt] = map[i];
			n_cnt++;
		}
	}

	//-ve come first in ascending order
	for (i = 0; i < 31; i++)
	{
		int cnt0 = 0;
		int cnt1 = 0;

		float bkt0[n_cnt];
		float bkt1[n_cnt];

		for (int j = 0; j < n_cnt; j++)
		{
			unsigned int x = (unsigned int) neg[i];
			x = x >> i;
			if ((x & 1) == 0)
			{
				bkt0[cnt0] = neg[i];
				cnt0++;
			}
			else if ((x & 1) == 1)
			{
				bkt1[cnt1] = neg[i];
				cnt1++;
			}
		}
			
		//merge 0 and 1... res = 0s + 1s only when at final bit to the final result array
		if (i == 30)
		{
			for (int j = 0; j < cnt0; j++)
			{
				res[j] = bkt0[j];
			}
			for (int k = cnt0; k < (cnt0 + cnt1); k++)
			{
				res[k] = bkt1[(k - cnt0)];
			}
		}

		//merging to form ordered -ve array
		else
		{
			for (int j = 0; j < cnt0; j++)
			{
				neg[j] = bkt0[j];
			}
			for (int k = cnt0; k < (cnt0 + cnt1); k++)
			{
				neg[k] = bkt1[(k - cnt0)];
			}
		}
	}

	//+ve flts after -ve flts because ascending... Same Loop basically
	for (i = 0; i < 31; i++)
	{
		int cnt0 = 0;
		int cnt1 = 0;

		float bkt0[n_cnt];
		float bkt1[n_cnt];

		for (int j = 0; j < p_cnt; j++)
		{
			//flt.f = pos[i];
			//unsigned int x = flt.i;
			unsigned int x = (unsigned int) pos[i];
			x = x >> i;
			if ((x & 1) == 0)
			{
				bkt0[cnt0] = pos[i];
				cnt0++;
			}
			else if ((x & 1) == 1)
			{
				bkt1[cnt1] = pos[i];
				cnt1++;
			}
		}
			
		//merge 0 and 1... res = 0s + 1s only when at final bit to the final result array
		if (i == 30)
		{
			for (int j = 0; j < cnt0; j++)
			{
				res[j + (n_cnt - 1)] = bkt0[j];
			}
			for (int k = cnt0; k < (cnt0 + cnt1); k++)
			{
				res[k + (n_cnt - 1)] = bkt1[(k - cnt0)];
			}
		}

		//merging to form ordered +ve array
		else
		{
			for (int j = 0; j < cnt0; j++)
			{
				pos[j] = bkt0[j];
			}
			for (int k = cnt0; k < (cnt0 + cnt1); k++)
			{
				pos[k] = bkt1[(k - cnt0)];
			}
		}
	}

	for (i = 0; i < NUMFLTS; i++)
	{
		map[i] = res[i];
	}

	close(fd);
	return 0;
}



