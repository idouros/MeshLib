#ifndef _MESH
#define _MESH

#include<vector>
#include<fstream>

using namespace std;

// TODO make this a class
typedef std::vector<std::vector<double>> GridMesh;

void SaveAsObjFile(const GridMesh& m, const double& scale, std::ofstream& outfile)
{
	auto rows = m.size();
	auto cols = m[0].size();

	// vertices
	for (auto row = 0; row < rows; row++)
	{
		for (auto col = 0; col < cols; col++)
		{
			outfile << "v " << row * scale << " " << col * scale << " " << m[row][col] << std::endl;
		}
	}

	// faces
	for (auto row = 0; row < rows-1; row++)
	{ 
		for (auto col = 0; col < cols-1; col++)
		{
			auto top_left = (row * cols) + col + 1;
			auto top_right = top_left + 1;
			auto bottom_left = top_left + cols;
			auto bottom_right = bottom_left + 1;

			// first triangle
			outfile << "f " << top_left  << " " << top_right  << " " << bottom_left << std::endl;
			// second triangle
			outfile << "f " << top_right  << " " << bottom_right  << " " << bottom_left << std::endl;
		}
	}
}

#endif
