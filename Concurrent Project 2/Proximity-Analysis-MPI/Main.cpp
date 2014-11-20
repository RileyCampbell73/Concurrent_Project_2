#include"Coords.h"
#include <mpi.h>
#include <string>
using namespace std;

string sServiceCode = "";

typedef struct{
	int count;
	double percentage;
}Msg_t;

MPI_Datatype createMsgType()
{
	// Setup the five arguments for a call to MPI_Type_struct()
	int count = 1;	// 2 blocks
	int blocklens[] = { 4, 4 };	//  1 int, 1 double
	MPI_Aint offsets[2];
	offsets[0] = offsetof(Msg_t, count); // offset in bytes to block 1
	offsets[1] = offsetof(Msg_t, percentage); // offset in bytes to block 2

	MPI_Datatype old_types[] = { MPI_INT, MPI_DOUBLE };	// input data types
	MPI_Datatype t; // output data type

	// Call the datatype contructor function
	MPI_Type_struct(count, blocklens, offsets, old_types, &t);

	// Allocate memory for the new type
	MPI_Type_commit(&t);

	return t;
}

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


void processMaster(int numProcs, char* argumentServiceCode)
{
	//DO THE STUFF
	//ONCE ITS PROCESSED ALL ITS RECORDS:
		//RECIEVE ALL COLLEVTIVE INTERFACE MESSAGES
		//DISPLAY RESULTS

	//set the start time
	double startTime = MPI_Wtime();

	vector<Coords> serviceCoords;
	ifstream file("services.dat");
	string str;

	//TODO: check for command line arguments here
	//check to see if its 6 digits

	//loop through file and get all services for code
	while (std::getline(file, str))
	{
		if (str.substr(0, 6) == sServiceCode)
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

	getline(fileagain, str);
	int ilineBytes = (int)fileagain.tellg();

	// Move the file pointer to MY first record
	fileagain.seekg(ilineBytes * 0, ios::beg);

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
		fileagain.seekg(ilineBytes * (numProcs - 1), ios::cur);
	}
	cout << "finished" << endl;
	Msg_t records[5];
	records[0].count = 1;
	records[1].count = 2;
	records[2].count = 3;
	records[3].count = 4;
	records[4].count = 0;
	records[0].percentage = 1.01;
	records[1].percentage = 2.01;
	records[2].percentage = 3.01;
	records[3].percentage = 4.01;


	Msg_t allrecords[10];
	MPI_Datatype recType = createMsgType();
	MPI_Status status;



	MPI_Gather(&records, 5, recType, allrecords, 5, recType, 0, MPI_COMM_WORLD);


// get the service name from the service code
string serviceName = findServiceNameFromFile(argumentServiceCode);

// calculate elapsed time
double elapsedTime = MPI_Wtime() - startTime;

	cout << allrecords[0].percentage << endl;
	cout << allrecords[1].percentage << endl;
	cout << allrecords[2].percentage << endl;
	cout << allrecords[3].percentage << endl;
	cout << allrecords[4].count << endl;
	cout << allrecords[5].count << endl;
	cout << allrecords[6].count << endl;
	cout << allrecords[7].count << endl;
	cout << allrecords[8].count << endl;
	cout << allrecords[9].count << endl;
	cout << allrecords[10].count << endl;
	cout << allrecords[11].count << endl;
	cout << allrecords[12].count << endl;
	cout << allrecords[13].count << endl;
	cout << allrecords[14].count << endl;
	MPI_Type_free(&recType);


	//report the findings
	//cout << setw(64) << right << "Proximites of Residental Addresses to Services in Toronto" << endl;
	//cout << setw(64) << right << "---------------------------------------------------------" << endl << endl;
	//cout << setw(30) << left << "Service: " << setw(16) << serviceName << endl;
	//cout << setw(30) << left << "Service Code: " << setw(16) << sServiceCode << endl;
	//cout << setw(30) << left << "Number of Service Locations: " << setw(16) << serviceCoords.size() << endl;
	//cout << setw(30) << left << "Elapsed Time in Seconds: " << setw(16) << elapsedTime<< endl << endl;
	//cout << setw(64) << right << "Aggregate Results for all 30,000 Addresses..." << endl << endl;
	//cout << setw(20) << left << "Nearest Service (KM)" << "  " << setw(14) << left << "# of Addresses" << "  " << setw(14) << left << "% of Addresses" << endl;
	//cout << setw(20) << left << "--------------------" << "  " << setw(14) << left << "--------------" << "  " << setw(14) << left << "--------------" << endl;
	//cout << setw(20) << right << " 0 - 1" << "  " << setw(14) << right << iUnder1km << "  " << setw(14) << right << setprecision(4) << (iUnder1km / 30000.0) * 100 << endl;
	//cout << setw(20) << right << " 1 - 2" << "  " << setw(14) << right << iBetween1and2 << "  " << setw(14) << right << setprecision(4) << (iBetween1and2 / 30000.0) * 100 << endl;
	//cout << setw(20) << right << " 2 - 5" << "  " << setw(14) << right << iBetween2and5 << "  " << setw(14) << right << setprecision(4) << (iBetween2and5 / 30000.0) * 100 << endl;
	//cout << setw(20) << right << " > 5" << "  " << setw(14) << right << iOver5 << "  " << setw(14) << right << setprecision(4) << (iOver5 / 30000.0) * 100 << endl;

}

void processSlave(int rank, int numProcs)
{
	//DO THE STUFF
	//ONCE ITS PROCESSED ALL ITS RECORDS:
		//PUT RECORDS INTO OBJECT/2D ARRAY AND USING DERIVED TYPE AND SEND TO MASTER

	vector<Coords> serviceCoords;
	ifstream file("services.dat");
	string str;

	//TODO: check for command line arguments here
	//check to see if its 6 digits

	//loop through file and get all services for code
	while (std::getline(file, str))
	{
		if (str.substr(0, 6) == sServiceCode)
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

	getline(fileagain, str);
	int ilineBytes = (int)fileagain.tellg();

	// Move the file pointer to MY first record
	fileagain.seekg(ilineBytes * rank, ios::beg);
	Msg_t records[5];
	records[0].count = 0;
	records[1].count = 0;
	records[2].count = 0;
	records[3].count = 0;
	records[4].count = rank;
	
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
			//iUnder1km++;
			records[0].count++;
		}
		else if (dShortestDistance <= 2.0){
			iBetween1and2++;
			records[1].count++;
		}
		else if (dShortestDistance <= 5.0){
			iBetween2and5++;
			records[2].count++;
		}
		else if (dShortestDistance > 5.0){
			iOver5++;
			records[3].count++;
		}
		fileagain.seekg(ilineBytes * (numProcs -1), ios::cur);
	}
	cout << "slave finished" << endl;
	MPI_Datatype recType = createMsgType();

	Msg_t* junk[4];
	MPI_Gather(&records, 5, recType, junk, 5, recType, 0, MPI_COMM_WORLD);
	MPI_Type_free(&recType);
	//report the findings
	//cout << setw(64) << right << "Proximites of Residental Addresses to Services in Toronto" << endl;
	//cout << rank<<endl;
	//cout << setw(64) << right << "---------------------------------------------------------" << endl << endl;
	//cout << setw(30) << left << "Service: " << setw(16) << "idk yet" << endl;
	//cout << setw(30) << left << "Service Code: " << setw(16) << sServiceCode << endl;
	//cout << setw(30) << left << "Number of Service Locations: " << setw(16) << serviceCoords.size() << endl;
	//cout << setw(30) << left << "Elapsed Time in Seconds: " << setw(16) << "time here" << endl << endl;
	//cout << setw(64) << right << "Aggregate Results for all 30,000 Addresses..." << endl << endl;
	//cout << setw(20) << left << "Nearest Service (KM)" << "  " << setw(14) << left << "# of Addresses" << "  " << setw(14) << left << "% of Addresses" << endl;
	//cout << setw(20) << left << "--------------------" << "  " << setw(14) << left << "--------------" << "  " << setw(14) << left << "--------------" << endl;
	//cout << setw(20) << right << " 0 - 1" << "  " << setw(14) << right << iUnder1km << "  " << setw(14) << right << setprecision(4) << (iUnder1km / 30000.0) * 100 << endl;
	//cout << setw(20) << right << " 1 - 2" << "  " << setw(14) << right << iBetween1and2 << "  " << setw(14) << right << setprecision(4) << (iBetween1and2 / 30000.0) * 100 << endl;
	//cout << setw(20) << right << " 2 - 5" << "  " << setw(14) << right << iBetween2and5 << "  " << setw(14) << right << setprecision(4) << (iBetween2and5 / 30000.0) * 100 << endl;
	//cout << setw(20) << right << " > 5" << "  " << setw(14) << right << iOver5 << "  " << setw(14) << right << setprecision(4) << (iOver5 / 30000.0) * 100 << endl;



}


int main(int argc, char* argv[])
{

	if (MPI_Init(&argc, &argv) == MPI_SUCCESS)
	{

		int rank, numProcs;
		MPI_Comm_size(MPI_COMM_WORLD, &numProcs);

		MPI_Comm_rank(MPI_COMM_WORLD, &rank);
		sServiceCode = argv[1];
		if (rank == 0)
			processMaster(numProcs, argv[1]);
		else
			processSlave(rank, numProcs);

		MPI_Finalize();
	}

	

}