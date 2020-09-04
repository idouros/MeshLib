#include "Mesh.h"
#include <time.h>

void GeoMesh::SaveAsObjFile(std::ofstream& outfile)
{
	auto rows = this->altitudes.size();
	auto cols = this->altitudes[0].size();

	// vertices
	for (auto row = 0; row < rows; row++)
	{
		for (auto col = 0; col < cols; col++)
		{
			outfile << "v " << row * this->params.grid_scale << " " << col * this->params.grid_scale << " " << this->altitudes[row][col] << std::endl;
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

	auto rows = this->params.grid_rows;
	auto cols = this->params.grid_cols;

	this->altitudes.resize(rows, std::vector<double>(cols, 0.0));
	this->setFlags.resize(rows, std::vector<bool>(cols, false));

	this->FillRegion(0, 0, rows - 1, cols - 1);
}

double GeoMesh::RandomAltitude(const double& min_altitude, const double& max_altitude)
{
	double r = rand() % 10000;
	return min_altitude + (max_altitude - min_altitude) * (r / 10000.0);
}


void GeoMesh::CheckAndSetRandomAltitude(
	const size_t& row,
	const size_t& col,
	const std::vector<double>& v = {}
)
{
	double min_altitude = std::numeric_limits<double>::max();
	double max_altitude = std::numeric_limits<double>::min();

	if (v.empty())
	{
		min_altitude = this->params.min_altitude;
		max_altitude = this->params.max_altitude;
	}
	else
	{
		for (int i = 0; i < v.size(); i++)
		{
			if (v[i] > max_altitude)
			{
				max_altitude = v[i];
			}
			if (v[i] < min_altitude)
			{
				min_altitude = v[i];
			}
		}
	}

	if (!(this->setFlags[row][col]))
	{
		this->altitudes[row][col] = this->RandomAltitude(min_altitude, max_altitude);
		this->setFlags[row][col] = true;
	}
}

void GeoMesh::FillRegion(const size_t& start_row, const size_t& start_col, const size_t& end_row, const size_t& end_col)
{
	if ((end_row > start_row + 1) || (end_col > start_col + 1))
	{
		auto mid_row = (start_row + end_row) / 2;
		auto mid_col = (start_col + end_col) / 2;

		// Fill corners
		this->CheckAndSetRandomAltitude(start_row, start_col);
		this->CheckAndSetRandomAltitude(start_row, end_col);
		this->CheckAndSetRandomAltitude(end_row, start_col);
		this->CheckAndSetRandomAltitude(end_row, end_col);
		
		// Fill sides
		this->CheckAndSetRandomAltitude(start_row, mid_col, { altitudes[start_row][start_col],	altitudes[start_row][end_col] });
		this->CheckAndSetRandomAltitude(end_row, mid_col, { altitudes[end_row][start_col],		altitudes[end_row][end_col] });
		this->CheckAndSetRandomAltitude(mid_row, start_col, { altitudes[start_row][start_col],	altitudes[end_row][start_col] });
		this->CheckAndSetRandomAltitude(mid_row, end_col, { altitudes[start_row][end_col],		altitudes[end_row][end_col] });

		// Fill middle
		this->CheckAndSetRandomAltitude(mid_row, mid_col, { altitudes[start_row][end_col], altitudes[end_row][end_col], altitudes[start_row][start_col], altitudes[end_row][start_col] });

		// Recurse
		this->FillRegion(start_row, start_col, mid_row, mid_col);
		this->FillRegion(mid_row, start_col, end_row, mid_col);
		this->FillRegion(start_row, mid_col, mid_row, end_col);
		this->FillRegion(mid_row, mid_col, end_row, end_col);
	}
}





std::shared_ptr<GeoMesh> GeoMesh::CreateGeoMesh(const LandscapeParams& parameters)
{
	auto gm = GeoMesh(parameters);
	srand(time(NULL)); // TODO - better randomization
	return std::make_shared<GeoMesh>(gm);
}
