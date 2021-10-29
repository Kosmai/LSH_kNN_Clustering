#include <iostream>


int main(){
	std::list<Point*> points;
	readDataSet("datasets/input_small_id", ' ', points);
	return 0;
}