#include "../include/dlib/global_optimization/find_max_global.h"
#include <iostream>
#include <spdlog/spdlog.h>
namespace Extrapolator
{

int test()
{
	auto complex_holder_table = [](double x0, double x1)
	{
		// This function is a version of the well known Holder table test
		// function, which is a function containing a bunch of local optima.
		// Here we make it even more difficult by adding more local optima
		// and also a bunch of discontinuities. 

		// add discontinuities
		double sign = 1;
		for (double j = -4; j < 9; j += 0.5)
		{
			if (j < x0 && x0 < j + 0.5)
				x0 += sign * 0.25;
			sign *= -1;
		}
		// Holder table function tilted towards 10,10 and with additional
		// high frequency terms to add more local optima.
		return -(std::abs(sin(x0)*cos(x1)*exp(std::abs(1 - std::sqrt(x0*x0 + x1 * x1) / 3.14))) - (x0 + x1) / 10 - sin(x0 * 10)*cos(x1 * 10));
	};
	dlib::max_function_calls var;
	auto a = dlib::find_min_global(complex_holder_table,
		{ -10,-10 }, // lower bounds
		{ 10,10 }, // upper bounds
		std::chrono::milliseconds(500) // run this long);
	);
  spdlog::info("Vallues {}",a.y);
	return 1;
}
}
