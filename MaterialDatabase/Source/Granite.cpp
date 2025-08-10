extern "C"
{
#include <sqlite3.h>
}
#include <string>
#include <fstream>

static int GraniteCallback(void* NotUsed, int argc, char** argv, char** azColName)
{
	std::string pbrt;
	std::string MaterialName;
	std::string MaterialNameMapped;
	std::string Color;
	std::string Texture;
	std::string URoughness;
	std::string VRoughness;
	std::string MaterialType;

	std::fstream* file = (std::fstream*)NotUsed;
	for (int i = 0; i < argc; i++)
	{
		if (0 == strcmp(azColName[i], "Material Type"))
		{
			MaterialType = argv[i] ? argv[i] : "";
		}
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
	if (0 == strcmp(MaterialType.c_str(), "Granite"))
	{
		pbrt += "#" + MaterialName + "\n";
		pbrt += "Texture \"" + MaterialNameMapped + "\" \"spectrum\" \"imagemap\"\n";
		pbrt += "\t\"string filename\"\t[ \"material/Granite/textures/" + Texture + "\" ]\n";
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
	}
	return 0;
}

void QueryGranite(sqlite3* db)
{
	char* sql = "select * from ConductorParameter;";


	std::fstream file;
	file.open("material/Granite/Granite.pbrt", std::ios::out);
	sqlite3_exec(db, sql, GraniteCallback, (void*)&file, nullptr);
	file.close();
}