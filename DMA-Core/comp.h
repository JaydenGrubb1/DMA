#pragma once

namespace DMA {
	struct complex {
		double real;
		double imag;
	};

	complex operator+(const complex& a, const complex& b);
	complex operator-(const complex& a, const complex& b);
	complex operator*(const complex& a, const complex& b);
}

namespace std {
	double abs(const DMA::complex& c);
}