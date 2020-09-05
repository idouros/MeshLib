#include "Mesh.h"

void GeoMesh::SaveAsObjFile(std::ofstream& outfile, const SeaOutput sea_output)
{
	auto rows = this->altitudes.size();
	auto cols = this->altitudes[0].size();

	// vertices
	for (auto row = 0; row < rows; row++)
	{
		for (auto col = 0; col < cols; col++)
		{	
			auto altitude = (sea_output == SeaOutput::CHOP && this->altitudes[row][col] < 0.0) ? 0.0 : this->altitudes[row][col];
			outfile << "v " << row * this->params.grid_scale << " " << col * this->params.grid_scale << " " << altitude << std::endl;
		}
	}

	if (sea_output == SeaOutput::SHOW_LEVEL)
	{
		outfile << "v " << 0.0 << " " << 0.0 << " " << 0.0 << std::endl;
		outfile << "v " << 0.0 << " " << (cols-1) * this->params.grid_scale << " " << 0.0 << std::endl;
		outfile << "v " << (rows - 1) * this->params.grid_scale << " " << 0.0 << " " << 0.0 << std::endl;
		outfile << "v " << (rows - 1) * this->params.grid_scale << " " << (cols - 1) * this->params.grid_scale << " " << 0.0 << std::endl;
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

	// sea level
	if (sea_output == SeaOutput::SHOW_LEVEL)
	{
		auto last_index = rows * cols;
		// first triangle
		outfile << "f " << last_index + 1 << " " << last_index + 2 << " " << last_index + 3 << std::endl;
		// second triangle
		outfile << "f " << last_index + 3 << " " << last_index + 4 << " " << last_index + 2 << std::endl;
	}
}

GeoMesh::GeoMesh(const LandscapeParams& parameters)
{
	this->params = parameters;

	auto rows = this->params.grid_rows;
	auto cols = this->params.grid_cols;

	this->altitudes.resize(rows, std::vector<double>(cols, 0.0));
	this->setFlags.resize(rows, std::vector<bool>(cols, false));

	this->FillRegion(0, 0, rows - 1, cols - 1, parameters.variance, 0);
}

double GeoMesh::RandomAltitude(std::uniform_real_distribution<double>& distribution_1, std::normal_distribution<double>& distribution_2)
{	
	double r = distribution_1(generator_1);
	double v = distribution_2(generator_2);
	return r + v;
}

void GeoMesh::CheckAndSetRandomAltitude(
	const size_t& row,
	const size_t& col,
	std::normal_distribution<double>& distribution_2,
	const std::vector<double>& v
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
		auto sum = 0.0;

		for (int i = 0; i < v.size(); i++)
		{
			sum = sum + v[i];
		}

		auto new_alt = sum / v.size();
		min_altitude = new_alt;
		max_altitude = new_alt;
	}

	std::uniform_real_distribution<double> distribution_1(min_altitude, max_altitude);

	if (!(this->setFlags[row][col]))
	{
		this->altitudes[row][col] = this->RandomAltitude(distribution_1, distribution_2);
		this->setFlags[row][col] = true;
	}
}

void GeoMesh::FillRegion(const size_t& start_row, const size_t& start_col, const size_t& end_row, const size_t& end_col, const double& variance, const size_t& pass)
{
	if ((end_row > start_row + 1) || (end_col > start_col + 1))
	{
		auto mid_row = (start_row + end_row) / 2;
		auto mid_col = (start_col + end_col) / 2;

		std::normal_distribution<double> distribution_2(0.0, variance);

		// Fill corners
		this->CheckAndSetRandomAltitude(start_row, start_col, distribution_2);
		this->CheckAndSetRandomAltitude(start_row, end_col, distribution_2);
		this->CheckAndSetRandomAltitude(end_row, start_col, distribution_2);
		this->CheckAndSetRandomAltitude(end_row, end_col, distribution_2);

		if (pass >= params.last_random_pass)
		{
			// Fill middle (diamond step)
			this->CheckAndSetRandomAltitude(mid_row, mid_col, distribution_2, {
				altitudes[start_row][end_col],
				altitudes[end_row][end_col],
				altitudes[start_row][start_col],
				altitudes[end_row][start_col]
				}
			);

			// Fill sides (square step)
			this->CheckAndSetRandomAltitude(start_row, mid_col, distribution_2, { altitudes[start_row][start_col],	altitudes[start_row][end_col],	altitudes[mid_row][mid_col] });
			this->CheckAndSetRandomAltitude(end_row, mid_col, distribution_2,	{ altitudes[end_row][start_col],	altitudes[end_row][end_col],	altitudes[mid_row][mid_col] });
			this->CheckAndSetRandomAltitude(mid_row, start_col, distribution_2, { altitudes[start_row][start_col],	altitudes[end_row][start_col],	altitudes[mid_row][mid_col] });
			this->CheckAndSetRandomAltitude(mid_row, end_col, distribution_2,	{ altitudes[start_row][end_col],	altitudes[end_row][end_col],	altitudes[mid_row][mid_col] });
		}
		else
		{
			// Fill middle
			this->CheckAndSetRandomAltitude(mid_row, mid_col, distribution_2);

			// Fill sides
			this->CheckAndSetRandomAltitude(start_row, mid_col, distribution_2);
			this->CheckAndSetRandomAltitude(end_row, mid_col, distribution_2);
			this->CheckAndSetRandomAltitude(mid_row, start_col, distribution_2);
			this->CheckAndSetRandomAltitude(mid_row, end_col, distribution_2);
		}

		// Recurse
		auto half_variance = variance * params.variance_fade;
		auto next_pass = pass + 1;
		this->FillRegion(start_row, start_col, mid_row, mid_col, half_variance, next_pass);
		this->FillRegion(mid_row, start_col, end_row, mid_col, half_variance, next_pass);
		this->FillRegion(start_row, mid_col, mid_row, end_col, half_variance, next_pass);
		this->FillRegion(mid_row, mid_col, end_row, end_col, half_variance, next_pass);
	}
}

std::shared_ptr<GeoMesh> GeoMesh::CreateGeoMesh(const LandscapeParams& parameters)
{
	auto gm = GeoMesh(parameters);
	return std::make_shared<GeoMesh>(gm);
}
