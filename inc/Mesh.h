#ifndef _MESH
#define _MESH

#include <vector>
#include <fstream>
#include <random>

typedef std::vector<std::vector<double>> GridMesh;
typedef std::vector<std::vector<bool>> FlagMesh;

enum class SeaOutput {
	NONE = 0,
	SHOW_LEVEL,
	CHOP
};

struct LandscapeParams {
	size_t grid_rows;
	size_t grid_cols;
	double grid_scale;
	double max_altitude;
	double min_altitude;
	double variance;
	double variance_fade;
	size_t last_random_pass;
	std::string out_file_name;
	SeaOutput sea_output;
};



class GeoMesh
{
public:
	void SaveAsObjFile(std::ofstream& outfile, const SeaOutput sea_output = SeaOutput::NONE);

	// Factory
	static std::shared_ptr<GeoMesh> CreateGeoMesh(const LandscapeParams& parameters);

private:
	GeoMesh(const LandscapeParams& parameters);

	double RandomAltitude(std::uniform_real_distribution<double>& distribution_1, std::normal_distribution<double>& distribution_2);
	void CheckAndSetRandomAltitude(
		const size_t& row,
		const size_t& col,
		std::normal_distribution<double>& distribution_2,
		const std::vector<double>& v = {}
	);

	// "Diamond-square algorithm" - Fournier, Fussell and Carpenter at SIGGRAPH 1982.
	void FillRegion(const size_t& start_row, const size_t& start_col, const size_t& end_row, const size_t& end_col, const double& variance, const size_t& pass);

	GridMesh altitudes;
	FlagMesh setFlags;
	LandscapeParams params;

	std::mt19937_64 generator_1{ std::random_device()() };;
	std::mt19937_64 generator_2{ std::random_device()() };;
};

#endif
