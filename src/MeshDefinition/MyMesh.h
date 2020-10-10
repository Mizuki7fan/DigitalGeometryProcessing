#pragma once
#include <string>
#include <vector>
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <iostream>
//#include "../LinSysSolver/LinSysSolver.hpp"

class MyMesh
{
public:
	MyMesh();
	enum LAR_KIND { BARYCENTRIC, VORONOI, MIXED, LAR_ND};//���塢��¼LAR������
	enum V_NORMAL_KIND { V_NORMAL_ND };
	enum CURVATURE_KIND { MEAN, ABSOLUTEMEAN, GAUSSIAN, CURVATURE_ND};
	enum LAPLACIAN_KIND { UNIFORM, CONTANGENT, LAPLACIAN_ND};
	enum EULER_INTEGRATION_KIND {EXPLICIT,IMPLICIT,EULER_INTEGRATION_ND};//ŷ�����ֵ����ͣ���ʽ����ʽ
private:

public://ֱ��ͨ�������ȡ���еĲ������Լ���ͬ������ʵ���кܴ�ͬ�Ĳ���
	virtual bool Load(std::string) = 0;//��������
	virtual bool Write(std::string) = 0;//��������
	virtual void Clear() = 0;//��������
	virtual void UpdateNormals() = 0;
	virtual void LoadVerticeNormal() = 0;
	virtual void LoadFaceNormal() = 0;
	virtual bool VerticesEmpty() = 0;
	virtual int NVertices() const = 0;
	virtual int NEdges() const = 0;
	virtual int NFaces() const = 0;
	virtual double CalcEdgeLength(int i) = 0;
	virtual Eigen::Vector3d getPoint(int i) const = 0;
	virtual Eigen::Vector3d getVertexNormal(int i) const = 0;
	virtual Eigen::Vector3d getFaceNormal(int i) const = 0;
	virtual void getVerticesNeighbour(int i, std::vector<int>& nei) const=0;//��ȡ���1-����
	virtual void getFaceNeighbour(int i, std::vector<int>& nei)const = 0;
	virtual void getEdgeVertices(int e, int& v1, int& v2) const = 0;
	virtual double getEdgeLength(int e) const = 0;
	virtual void getFaceVertices(int f, int& v1, int& v2, int& v3) const = 0;
	virtual void getFaceAngles(int f, double& angle1, double& angle2, double& angle3) const = 0;
	virtual bool isBoundary(int) const = 0;
	virtual bool isBoundaryVertex(int) const = 0;

	virtual Eigen::Vector3d getVertexCoord(int) = 0;
	virtual double ComputeMeshVolume() = 0;

	virtual void ComputeLaplacian() = 0;//����Laplacian����
	virtual void LoadVertex() = 0;
	virtual void SetVertexNewCoord(int, Eigen::Vector3d) = 0;
	virtual void SetVerticesNewCoord() = 0;
	virtual void SetFacesNewNormalCoord() = 0;

	virtual void BilateralDenoising(double, double);
	virtual void BilateralNormalFiltering(double, double);

	virtual void CalcTutte()=0;
	virtual void MakeBackup() = 0;
	virtual void RestoreBackup() = 0;

protected://��������Ĺ���
	//���������ε�����
	Eigen::Vector3d ComputeTriangleCirumcenter(Eigen::Vector3d& p1, Eigen::Vector3d& p2, Eigen::Vector3d& p3);
	void ComputeLAR();
	double ComputeTriangleArea(Eigen::Vector3d& p1, Eigen::Vector3d& p2, Eigen::Vector3d& p3);
public://��Ҫ�ڸ���cpp��ʵ�ֵ�����

	void MakeNoise();
	void Fairing(int);
	void Smoothing();

	void ComputeCurvature();

	void getVCurvature(std::vector<double>& c);

	
	void SetLaplacianKind(LAPLACIAN_KIND k);
	void SetLARKind(LAR_KIND k);
	void SetCurvatureKind(CURVATURE_KIND k);
	void SetEulerIntegrationKind(EULER_INTEGRATION_KIND k);

	void eigen_output();
protected:

	Eigen::VectorXd LAR; //�ֲ�ƽ����������
	bool LAR_latest = false;
	LAR_KIND LAR_kind = LAR_KIND::LAR_ND;

	Eigen::MatrixXd Vertices;//���񶥵�Eigen::Matrix3d FaceNormal;//�淨��Ψһ
	bool Vertices_latest = false;
	
	Eigen::MatrixXd VertexNormal;//���㷨�������ּ�Ȩ��ʽ�����������ʱ����(��ΪOpenmesh�ṩ��Ĭ�ϵ�)
	bool VertexNormal_latest = false;
 	
	Eigen::MatrixXd FaceNormal;
	bool FaceNormal_latest = false;
	
	Eigen::VectorXd Curvature;//���������
	bool Curvature_latest = false;
	CURVATURE_KIND Curvature_kind = CURVATURE_KIND::CURVATURE_ND;

	Eigen::SparseMatrix<double> Laplacian;
	LAPLACIAN_KIND Laplacian_kind = LAPLACIAN_KIND::LAPLACIAN_ND;
	bool Laplacian_latest = false;

	EULER_INTEGRATION_KIND Euler_integration_kind = EULER_INTEGRATION_KIND::EULER_INTEGRATION_ND;
	bool Euler_integration_latest = false;

	double MeshVolume;
	bool MeshVolume_latest = false;

//	LinSysSolver* solver;
};