#include "DGPAlgorithm.h"
/*
void DGPAlgorithm::ComputeCurvature(const MyMesh& mesh, std::vector<double>& curvature)
{
	int N = mesh.NVertices();
	curvature.resize(N);
	for (int i = 0; i < mesh.NVertices(); i++)
	{
		curvature[i] = 1-i / double(N);
	}
}
*/
void DGPAlgorithm::ComputeMeanCurvature( MyMesh& mesh, std::vector<double>& curvature)
{//��ƽ������
	//����ÿ���㣬����ƽ���������Ϣ
	mesh.UpdateMeanCurvature();
	mesh.getVCurvature(curvature);

}

void DGPAlgorithm::ComputeGaussianCurvature( MyMesh& mesh, std::vector<double>& curvature)
{
	//double area = mesh.CalcFaceArea();
	mesh.UpdateGaussianCurvature();
	mesh.getVCurvature(curvature);
}
