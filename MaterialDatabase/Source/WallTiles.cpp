extern "C"
{
#include <sqlite3.h>
}
#include <string>
#include <fstream>

static int WallTilesCallback(void* NotUsed, int argc, char** argv, char** azColName)
{
	std::string pbrt;
	std::string MaterialName;
	std::string MaterialNameMapped;
	std::string Color;
	std::string Texture;
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
	}
	if (0 == strcmp(MaterialType.c_str(), "Wall Tiles"))
	{
		pbrt += "#" + MaterialName + "\n";
		pbrt += "Texture \"" + MaterialNameMapped + "\" \"spectrum\" \"imagemap\"\n";
		pbrt += "\t\"string filename\"\t[ \"material/WallTiles/textures/" + Texture + "\" ]\n";
		pbrt += "MakeNamedMaterial \"";
		pbrt += MaterialNameMapped + "\"\n";
		pbrt += "\t\"string type\"\t [ \"diffuse\" ]\n";
		pbrt += "\t\"string reflectance\"\t[ \"" + MaterialNameMapped + "\" ]\n";
		pbrt += "\n";
		file->write(pbrt.c_str(), pbrt.size());
		printf("%s\n", pbrt.c_str());
		printf("\n");
	}

	return 0;
}


void QueryWallTiles(sqlite3* db)
{
	char* sql = "select * from DiffuseParameter;";

	std::fstream file;
	file.open("material/WallTiles/WallTiles.pbrt", std::ios::out);
	sqlite3_exec(db, sql, WallTilesCallback, (void*)&file, nullptr);
	file.close();
}