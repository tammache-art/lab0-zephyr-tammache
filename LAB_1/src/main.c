/*
 * ESE5180 Lab 0
 * Section 6: Printing vs. Logging
 */

#ifdef CONFIG_SUM_PRINT
#include "sum_printk.h"
#elif defined(CONFIG_SUM_LOG)
#include "sum_log.h"
#else
#error "A sum implementation must be selected"
#endif

int main(void)
{
	int a = -5;
	int b = 12;

#ifdef CONFIG_SUM_PRINT
	sum_printk(a, b);
#elif defined(CONFIG_SUM_LOG)
	sum_log(a, b);
#endif

	return 0;
}