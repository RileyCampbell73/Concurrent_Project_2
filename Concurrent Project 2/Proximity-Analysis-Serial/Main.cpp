#include"Coords.h"
using namespace std;

string findServiceNameFromFile (char* argumentServiceCode)
{
	string serviceCode = argumentServiceCode;

	ifstream serviceCodesFile("service-codes.csv");
	string line;

	while(getline(serviceCodesFile,line))
	{
		int stopPoint = line.find(',');

		if( line.substr(0,stopPoint) == serviceCode )
			return line.substr(stopPoint + 1);
	}

	return "NOT FOUND";
}

int main(int argc, char* argv[])
{
	vector<Coords> serviceCoords;
	ifstream file("services.dat");
	string str;

	//TODO: check for command line arguments here
		//check to see if its 6 digits

	//loop through file and get all services for code
	while (std::getline(file, str))
	{
		if (str.substr(0, 6) == argv[1])
			serviceCoords.push_back(Coords(atof(str.substr(7, 11).c_str()), atof(str.substr(19, 11).c_str())));
	}
	//cout << serviceCoords.size() << endl;

	//TODO: Check if # of services is 0


	//loop through residences file and check it with all the coords from the services file to see which is closest
	ifstream fileagain("residences.dat");
	//counters for distances
	int iUnder1km = 0;
	int iBetween1and2 = 0;
	int iBetween2and5 = 0;
	int iOver5 = 0;

	//read file line by line
	while (std::getline(fileagain, str))
	{
		double dResEasting = atof(str.substr(0, 11).c_str());
		double dResNorthing = atof(str.substr(12, 11).c_str());
		double dShortestDistance = 100.0;
		for (unsigned int i = 0; i < serviceCoords.size(); i++){
			//calculate the distance for these residental coorinates
			double dDistance = (sqrt(pow((serviceCoords[i].getEasting() - dResEasting), 2) + pow((serviceCoords[i].getNorthing() - dResNorthing), 2))) / 1000;
			//check shortest distance
			if (dDistance < dShortestDistance){
				dShortestDistance = dDistance;
			}
		}
		
		if (dShortestDistance <= 1.0){
			iUnder1km++;
			
		}
		else if (dShortestDistance <= 2.0){
			iBetween1and2++;
			
		}
		else if (dShortestDistance <= 5.0){
			iBetween2and5++;
			
		}
		else if (dShortestDistance > 5.0){
			iOver5++;
			
		}
	
	}
	
	string serviceName = findServiceNameFromFile(argv[1]);
	

	//report the findings
	cout << setw(64) << right << "Proximites of Residental Addresses to Services in Toronto" << endl;
	cout << setw(64) << right << "---------------------------------------------------------" << endl << endl;
	cout << setw(30) << left << "Service: " << setw(16) << serviceName << endl;
	cout << setw(30) << left << "Service Code: " << setw(16) << argv[1] << endl;
	cout << setw(30) << left << "Number of Service Locations: " << setw(16) << serviceCoords.size() << endl;
	cout << setw(30) << left << "Elapsed Time in Seconds: " << setw(16) <<"time here" << endl << endl;
	cout << setw(64) << right << "Aggregate Results for all 30,000 Addresses..." << endl << endl;
	cout << setw(20) << left << "Nearest Service (KM)" << "  " << setw(14) << left << "# of Addresses" << "  " << setw(14) << left << "% of Addresses" << endl;
	cout << setw(20) << left << "--------------------" << "  " << setw(14) << left << "--------------" << "  " << setw(14) << left << "--------------" << endl;
	cout << setw(20) << right << " 0 - 1" << "  " << setw(14) << right << iUnder1km << "  " << setw(14) << right << setprecision(4) << (iUnder1km / 30000.0) * 100 << endl;
	cout << setw(20) << right << " 1 - 2" << "  " << setw(14) << right << iBetween1and2 << "  " << setw(14) << right << setprecision(4) << (iBetween1and2 / 30000.0) * 100 << endl;
	cout << setw(20) << right << " 2 - 5" << "  " << setw(14) << right << iBetween2and5 << "  " << setw(14) << right << setprecision(4) << (iBetween2and5 / 30000.0) * 100 << endl;
	cout << setw(20) << right << " > 5" << "  " << setw(14) << right << iOver5 << "  " << setw(14) << right << setprecision(4) << (iOver5 / 30000.0) * 100 << endl;




}