#include "Mesh.h"

void GeoMesh::SaveAsObjFile(std::ofstream& outfile)
{
	auto rows = this->altitudes.size();
	auto cols = this->altitudes[0].size();

	// vertices
	for (auto row = 0; row < rows; row++)
	{
		for (auto col = 0; col < cols; col++)
		{
			outfile << "v " << row * this->params.scale << " " << col * this->params.scale << " " << this->altitudes[row][col] << std::endl;
		}
	}

	// faces
	for (auto row = 0; row < rows - 1; row++)
	{
		for (auto col = 0; col < cols - 1; col++)
		{
			auto top_left = (row * cols) + col + 1;
			auto top_right = top_left + 1;
			auto bottom_left = top_left + cols;
			auto bottom_right = bottom_left + 1;

			// first triangle
			outfile << "f " << top_left << " " << bottom_left << " " << top_right << std::endl;
			// second triangle
			outfile << "f " << top_right << " " << bottom_left << " " << bottom_right << std::endl;
		}
	}
}

GeoMesh::GeoMesh(const LandscapeParams& parameters)
{
	this->params = parameters;

	// YYY
	this->altitudes = {
		{ 0, 0, 0 ,0 },
		{ 0, 10, 0, 0 },
		{ 0, 0, 0 ,0 },
		{ 0, 0, 0 ,0 }
	};
}

std::shared_ptr<GeoMesh> GeoMesh::CreateGeoMesh(const LandscapeParams& parameters)
{
	auto gm = GeoMesh(parameters);
	return std::make_shared<GeoMesh>(gm);
}
