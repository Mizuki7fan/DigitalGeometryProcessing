#pragma once
#include "MeshDefinition/MyMesh.h"

class DGPAlgorithm
{
public:
	//������ĵ������
	static void ComputeMeanCurvature( MyMesh& mesh, std::vector<double>& curvature);
	static void ComputeGaussianCurvature( MyMesh& mesh, std::vector<double>& curvature);

private:
//	void ComputeLocalAveragingRegion()

};