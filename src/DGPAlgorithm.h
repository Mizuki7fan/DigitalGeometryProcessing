#pragma once
#include "MeshDefinition/MyMesh.h"

class DGPAlgorithm
{
public:
	//算网格的点的曲率
	static void ComputeMeanCurvature( MyMesh& mesh, std::vector<double>& curvature);
	static void ComputeGaussianCurvature( MyMesh& mesh, std::vector<double>& curvature);

private:
//	void ComputeLocalAveragingRegion()

};