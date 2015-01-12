/* 
 * File:   main.cpp
 * Author: ameemtas
 *
 * Created on January 12, 2015, 3:43 PM
 */

#include <cstdlib>
#include <iostream>

using namespace std;

/*
 * 
 */
int add_crap(int a, int b){
	return a + b;
}


int main(int argc, char** argv) {

	int x = 22, y = 33, z;
	z = add_crap(x,y);
	cout << "the sum is : " << z << endl;
	return 0;
}

