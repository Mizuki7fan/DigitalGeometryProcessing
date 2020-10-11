#pragma once
#include "MeshDefinition/MyMesh.h"

class DGPAlgorithm
{
public:
	//������ĵ�����ʣ�����Ϊ�ֲ�ƽ����������͡����ʵ�����
	static void ComputeCurvature(int LAR_Kind, int CurvatureKind, MyMesh& mesh, std::vector<double>& curvature);
	
	//������������
	static void MakeNoise(MyMesh& mesh);
	//����Fairing����
	static void DoFairing(MyMesh& mesh, int power,int lap_kind);
	//����Smoothing����
	static void DoSmoothing(MyMesh& mesh, int laplacekind,int integrationkind);
	static void DoBilateralDenoising(MyMesh& mesh, double stdevs,double stdevr);
	static void DoBilateralNormalFiltering(MyMesh& mesh, double stdevs, double stdevr);
	static void CalcTutte(MyMesh& mesh);
	static void CalcLSCM(MyMesh& mesh);
private:

};