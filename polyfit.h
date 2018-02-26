#pragma once

//taken from https://github.com/natedomin/polyfit

#ifdef __cplusplus
extern "C" {
#endif

	int polyfit(const double* const dependentValues,
		const double* const independentValues,
		unsigned int        countOfElements,
		unsigned int        order,
		double*             coefficients);


#ifdef __cplusplus
}
#endif