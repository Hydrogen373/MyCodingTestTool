#include<iostream>
int stackOverFlowError() {
	return stackOverFlowError();
}


int main() {
	std::ios::sync_with_stdio(0);
	std::cin.tie(0);

	//
	//std::cout << "Input 0~9: ";
	char c;
	std::cin >> c;
	if ('0' <= c && c <= '9') {
		std::cout << "Input is " << c << std::endl;
	}
	else {
		std::cout << "Input is not between 0 and 9" << std::endl;
		throw std::exception("Invalid Input!");
	}
	return 0;
}