#include <iostream>

#include "../include/fredPoint.hpp"
#include "../include/curve.hpp"
#include "../include/frechet.hpp"

int main(void){
	FredPoint p(2);
	FredPoint g(2);
	p.set(0, 1);
	p.set(1, 1);
	g.set(0, 1);
	g.set(1, 2);
	std::cout<<p.size()<<std::endl;
	std::cout<<g.size()<<std::endl;

	Curve c1(2,"bob");
	Curve c2(2,"alice");

	c1.push_back(p);
	c1.push_back(p);
	c1.push_back(p);
	c1.push_back(p);

	c2.push_back(g);
	c2.push_back(g);
	c2.push_back(g);
	c2.push_back(g);

	std::cout<<c1.size()<<std::endl;
	std::cout<<c2.size()<<std::endl;

	std::cout << Frechet::Continuous::distance(c1, c2).value << std::endl;

	return 0;
}