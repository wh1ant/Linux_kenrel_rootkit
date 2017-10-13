#include <linux/kernel.h>

#include "utils.h"

bool find_str(const char * target, const char * find_str, int find_str_len)
{
	int i=-1;
	int find_count=0;

	do
	{
		i++;
		if(target[i]==find_str[find_count])
		{
			find_count++;
			if(find_count==find_str_len)
				return true;
		}
		else
		{
			find_count=0;
		}
	}while(target[i]!='\0');

	return false;
}
