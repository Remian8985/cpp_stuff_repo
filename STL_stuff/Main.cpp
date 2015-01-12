/* 
 * File:   Main.cpp
 * Author: ameemtas
 *
 * Created on January 12, 2015, 2:27 PM
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

    
    int x = 23, y =44, z;
    z = add_crap(x,y);
	cout << "The sum is : " << z << endl;
    
    return 0;
}

