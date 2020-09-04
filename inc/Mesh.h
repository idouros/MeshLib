#ifndef _MESH
#define _MESH

#include<vector>
#include<fstream>

// TODO make this a class

typedef std::vector<std::vector<double>> GridMesh;
typedef std::vector<std::vector<bool>> FlagMesh;

struct LandscapeParams {
	size_t grid_rows;
	size_t grid_cols;
	double grid_scale;
	double max_altitude;
	double min_altitude;
	double variance;
};

class GeoMesh
{
public:
	void SaveAsObjFile(std::ofstream& outfile);

	// Factory
	static std::shared_ptr<GeoMesh> CreateGeoMesh(const LandscapeParams& parameters);

private:
	GeoMesh(const LandscapeParams& parameters);

	double RandomAltitude(const double& min_altitude, const double& max_altitude);
	void CheckAndSetRandomAltitude(
		const size_t& row,
		const size_t& col,
		const std::vector<double>& v
	);

	void FillRegion(const size_t& start_row, const size_t& start_col, const size_t& end_row, const size_t& end_col);

	GridMesh altitudes;
	FlagMesh setFlags;
	LandscapeParams params;
};

#endif
