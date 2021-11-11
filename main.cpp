#include <cassert>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <memory>

namespace h {
#include "bitset.h"
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


	h::BITSET* b = h::new_bitset(13);
	h::set_bit(b, 12);
	h::set_bit(b, 3);
	h::set_bit(b, 6);
	h::set_bit(b, 7);
	h::set_bit(b, 0);
	assert(h::find_clear_bit(b, 12) == (size_t)-1);
	assert(h::find_clear_bit(b, 6) == 8);
	assert(h::find_clear_bit(b, 1) == 1);
	print_bitset(stdout, b);
	free(b);

	return 0;
}
