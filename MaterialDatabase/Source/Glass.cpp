extern "C"
{
#include <sqlite3.h>
}
#include <fstream>
#include <string>

static int GlassCallback(void* NotUsed, int argc, char** argv, char** azColName)
{
	std::string pbrt;
	std::string MaterialName;
	std::string MaterialNameMapped;
	std::string Color;
	std::string Reflectivity;
	std::string Transmissivity;
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

		if (0 == strcmp(azColName[i], "reflectivity"))
		{
			Reflectivity = argv[i] ? argv[i] : "";
		}
		if (0 == strcmp(azColName[i], "transmissivity"))
		{
			Transmissivity = argv[i] ? argv[i] : "";
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
	pbrt += "\t\"string type\"\t [ \"glass\" ]\n";
	pbrt += "\t\"string reflectance\"\t[ \"" + MaterialNameMapped + "\" ]\n";
	pbrt += "\t\"float reflectivity\"\t[ " + Reflectivity + " ]\n";
	pbrt += "\t\"float transmissivity\"\t[ " + Transmissivity + " ]\n";
	pbrt += "\n";
	file->write(pbrt.c_str(), pbrt.size());
	printf("%s\n", pbrt.c_str());
	printf("\n");
	return 0;
}


void QueryGlass(sqlite3* db)
{
	char* sql = "select * from GlassParameter;";

	std::fstream file;
	file.open("material/Glass.pbrt", std::ios::out);
	sqlite3_exec(db, sql, GlassCallback, (void*)&file, nullptr);
	file.close();
}