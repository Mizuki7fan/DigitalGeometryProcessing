#include "DGPAlgorithm.h"
#include <fstream>

void DGPAlgorithm::ComputeCurvature(int localaverageregionKind, int curvatureKind, MyMesh& mesh, std::vector<double>& curvature)
{
	mesh.ComputeLAR(localaverageregionKind);
	switch (curvatureKind)
	{
	case 0:
		ComputeMeanCurvature(mesh, curvature);
		break;
	case 1:
		ComputeMeanCurvature(mesh, curvature);
		for (int i = 0; i < curvature.size(); i++)
			if (curvature[i] < 0) curvature[i] = -curvature[i];
		break;
	case 2:
		ComputeGaussianCurvature(mesh, curvature);
		break;
	default:
		break;
	}
}
void DGPAlgorithm::ComputeMeanCurvature( MyMesh& mesh, std::vector<double>& curvature)
{//��ƽ������
	//����ÿ���㣬����ƽ���������Ϣ
	mesh.UpdateMeanCurvature();
	mesh.getVCurvature(curvature);

}

void DGPAlgorithm::ComputeGaussianCurvature( MyMesh& mesh, std::vector<double>& curvature)
{
	mesh.UpdateGaussianCurvature();
	mesh.getVCurvature(curvature);
}

void DGPAlgorithm::MakeNoise(MyMesh& mesh)
{
	mesh.UpdateNormals();
	mesh.MakeNoise();
	//����ƽ���߳���Ȼ�����ķ�����������ŷ������ƽ��
}

void DGPAlgorithm::DoFairing(MyMesh& mesh, int power)
{
	mesh.ComputeLAR(0);
	mesh.LoadVertex();
	mesh.Fairing(power);

}

void DGPAlgorithm::DoSmoothing(MyMesh& mesh, int laplacekind,int integrationkind)
{//����Զ��
	if (laplacekind == 0)//uniformLaplacian
		mesh.ComputeLaplacian(0);
	double pre_vol = mesh.ComputeMeshVolume();
	for (int i = 0; i < 10; i++)
	{
		mesh.ComputeLAR(0);
		mesh.LoadVertex();
		mesh.Smoothing(laplacekind, integrationkind);
	}
	//�����
	/*
	for (int i = 0; i < mesh.NVertices(); i++)
	{
		Vertices(i, 0) = Vertices(i, 0) / rate;
		Vertices(i, 1) = Vertices(i, 1) / rate;
		Vertices(i, 2) = Vertices(i, 2) / rate;
	}
	ComputeMeshVolume();
	SetVertexNewCoord();
	*/
}
