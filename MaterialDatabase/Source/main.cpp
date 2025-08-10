#include <iostream>
#include <string>
#include <fstream>
extern "C"
{
#include <sqlite3.h>
}
void QueryGlass(sqlite3* db);
void QueryGranite(sqlite3* db); 
void QueryAluminiumPlate(sqlite3* db);
void QueryWallTiles(sqlite3* db);
void QueryCeramic(sqlite3* db);
void QueryDielectric(sqlite3* db);
void QueryConductor(sqlite3* db);

int main(int argc, char **argv)
{
	sqlite3* db = nullptr;

	sqlite3_open("Materials.db", &db);

	QueryGlass(db);
	QueryConductor(db);
	QueryDielectric(db);
	//printf("==================================================================\n");
	//QueryWallTiles(db);
	//printf("==================================================================\n");
	//QueryCeramic(db);
	//printf("==================================================================\n");
	//QueryGranite(db);
	//printf("==================================================================\n");
	//QueryAluminiumPlate(db);
	sqlite3_close(db);

	std::string str;
	str += "Include \"material/Glass.pbrt\"";
	str += "\n";
	str += "Include \"material/Dielectric.pbrt\"";
	str += "\n";
	str += "Include \"material/Conductor.pbrt\"";
	str += "\n"; 

	std::fstream file;
	file.open("material/material.pbrt", std::ios::out);
	file.write(str.c_str(), str.length());
	file.close();
	return 0;
}