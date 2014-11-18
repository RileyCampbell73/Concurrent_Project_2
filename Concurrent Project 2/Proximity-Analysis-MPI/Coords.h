#include <fstream>
#include <string>
#include <vector>
#include<iostream>
#include <iomanip>
using namespace std;

class Coords{
private:
	double dEasting;
	double dNorthing;
public:
	Coords(double e, double n){
		dEasting = e;
		dNorthing = n;
	}
	double getEasting(){
		return dEasting;
	}
	void setEasting(double e){
		dEasting = e;
	}
	double getNorthing(){
		return dNorthing;
	}
	void setNorthing(double n){
		dNorthing = n;
	}
};
