#include "library/grayscale.hpp"
#include <iostream>

int main() {
	GrScImage<bool> img(1000, 1000, 0);
	std::cout << "\n\n\n";
	img.draw_circle(500, 500, 200, 1);

	std::cout << img;


}