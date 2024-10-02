#ifndef _MESH
#define _MESH

#include <vector>
#include <map>
#include <fstream>
#include <random>

typedef std::vector<std::vector<double>> GridMesh;
typedef std::vector<std::vector<bool>> FlagMesh;

enum class SeaOutput {
	NONE = 0,
	SHOW_LEVEL,
	CHOP
};

// TODO - Is there a way to do this automatically? A macro maybe?
std::map<std::string, SeaOutput> seaOutputMap = {
	{"NONE", SeaOutput::NONE},
	{"SHOW_LEVEL",	SeaOutput::SHOW_LEVEL},
	{"CHOP", SeaOutput::CHOP}
};

struct LandscapeParams {
	size_t grid_rows = 500;
	size_t grid_cols = 500;
	double grid_scale = 100.0;
	double max_altitude = 5000.0;
	double min_altitude = -1000.0;
	double variance = 6000.0;		// Typically max_altitude minus min_altitude
	double variance_fade = 0.53;	// Typically just over 0.5, to conceal the grid ridges
	size_t last_random_pass = 1;
	SeaOutput sea_output = SeaOutput::SHOW_LEVEL;
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
