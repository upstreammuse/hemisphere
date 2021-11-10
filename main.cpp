#include <cmath>
#include <iomanip>
#include <iostream>
#include <memory>

namespace h {
#include "hmath.h"
}

double math_func(double x) {
	return x * x - 3.2;
}

int main() {
	std::cout << "Hello there" << std::endl;
	auto s = std::make_shared<std::string>("This is a string");
	std::cout << s << " " << *s << std::endl;

	double x = M_PI;
	std::cout << std::setprecision(20) << sqrt(x) << std::endl << h::sqrt(x) << std::endl;

	std::cout << "attempting to solve an equation" << std::endl;
	std::cout << h::nsolve(math_func, 0, 10) << std::endl;

	return 0;
}
