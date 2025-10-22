#include "library/grayscale.hpp"
#include <iostream>

int main() {
	GrScImage<bool> img(100, 100, 0);
	std::cout << "\n\n\n";
	img.draw_circle(50, 50, 25, 1);

	std::cout << img;


}