#include "ppm.h"


void writePPM(float *p, int w, int h, string file)
{
	ofstream outfile(file);
	outfile << "P3" << endl << w << " " << h << endl << "255" << endl;
	for (int j = 0; j < h; j++)
	{
		for (int i = 0; i < w; i++)
		{
			int offset = 3 * (j * w + i);
			outfile << static_cast<int>(p[offset + 0]) << " "
					<< static_cast<int>(p[offset + 1]) << " "
					<< static_cast<int>(p[offset + 2]) << " ";
			//cout << " bbb " << endl;
		}
		outfile << endl;
	}
}

void readPPM(float *p, int w, int h, string file)
{
	ifstream ifs;

	ifs.open(file, ios::in);
	if (ifs.is_open())
	{
		string line[4];
		getline(ifs, line[0]);
		getline(ifs, line[1]);
		getline(ifs, line[2]);
		getline(ifs, line[3]);
		size_t c = 0;
		size_t b = -1;
		int i = 0;
		b = line[3].find_first_of(' ');
		while (string::npos != b)
		{
			p[i] = atoi(line[3].substr(c, b - c).c_str());
			//cout << p[i] << " ";
			i++;
			c = b + 1;
			b = line[3].find_first_of(' ', c);
		}
	//	cout << "---------------------------\n" << endl;
	//	cout << atoi(line[3].substr(c, b - c).c_str()) << endl;
	}
}