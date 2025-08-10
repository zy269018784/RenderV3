extern "C"
{
#include <sqlite3.h>
}
#include <fstream>
#include <string>

static int ConductorCallback(void* NotUsed, int argc, char** argv, char** azColName)
{
	std::string pbrt;
	std::string MaterialName;
	std::string MaterialNameMapped;
	std::string Color;
	std::string URoughness;
	std::string VRoughness;
	std::string Texture;
	std::fstream* file = (std::fstream*)NotUsed;
	for (int i = 0; i < argc; i++)
	{
		if (0 == strcmp(azColName[i], "Material Name Mapped"))
		{
			MaterialNameMapped = argv[i] ? argv[i] : "";
		}

		if (0 == strcmp(azColName[i], "Material Name"))
		{
			MaterialName = argv[i] ? argv[i] : "";
		}
		if (0 == strcmp(azColName[i], "color"))
		{
			Color = argv[i] ? argv[i] : "";
		}

		if (0 == strcmp(azColName[i], "texture"))
		{
			Texture = argv[i] ? argv[i] : "";
		}

		if (0 == strcmp(azColName[i], "uroughness"))
		{
			URoughness = argv[i] ? argv[i] : "";
		}

		if (0 == strcmp(azColName[i], "vroughness"))
		{
			VRoughness = argv[i] ? argv[i] : "";
		}
	}
	pbrt += "#" + MaterialName + "\n";
	if ("" == Texture)
	{
		pbrt += "Texture \"" + MaterialNameMapped + "\" \"spectrum\" \"const\"\n";
		pbrt += "\t\"float rgb\"\t[ " + Color + " ]\n";
	}
	else
	{
		pbrt += "Texture \"" + MaterialNameMapped + "\" \"spectrum\" \"imagemap\"\n";
		pbrt += "\t\"string filename\"\t[ \"material/textures/" + Texture + "\" ]\n";
	}
	pbrt += "MakeNamedMaterial \"";
	pbrt += MaterialNameMapped + "\"\n";
	pbrt += "\t\"string type\"\t [ \"conductor\" ]\n";
	pbrt += "\t\"string reflectance\"\t[ \"" + MaterialNameMapped + "\" ]\n";
	pbrt += "\t\"float uroughness\"\t[ " + URoughness + " ]\n";
	pbrt += "\t\"float vroughness\"\t[ " + VRoughness + " ]\n";
	pbrt += "\n";
	file->write(pbrt.c_str(), pbrt.size());
	printf("%s\n", pbrt.c_str());
	printf("\n");
	return 0;
}


void QueryConductor(sqlite3* db)
{
	char* sql = "select * from ConductorParameter;";

	std::fstream file;
	file.open("material/Conductor.pbrt", std::ios::out);
	sqlite3_exec(db, sql, ConductorCallback, (void*)&file, nullptr);
	file.close();
}