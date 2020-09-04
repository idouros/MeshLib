#ifndef _MESH
#define _MESH

#include<vector>
#include<fstream>

// TODO make this a class

typedef std::vector<std::vector<double>> GridMesh;
typedef std::vector<std::vector<bool>> FlagMesh;

struct LandscapeParams {
	double scale;
};

class GeoMesh
{
public:
	void SaveAsObjFile(std::ofstream& outfile);

	// Factory
	static std::shared_ptr<GeoMesh> CreateGeoMesh(const LandscapeParams& parameters);

private:
	GeoMesh(const LandscapeParams& parameters);
	GridMesh altitudes;
	FlagMesh setFlags;
	LandscapeParams params;
};

#endif
