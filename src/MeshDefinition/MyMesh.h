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
	virtual void getEdgeVertices(int e, int& v1, int& v2) const = 0;
	virtual double getEdgeLength(int e) const = 0;
	virtual void getFaceVertices(int f, int& v1, int& v2, int& v3) const = 0;
	virtual void getFaceAngles(int f, double& angle1, double& angle2, double& angle3) const = 0;
	virtual bool isBoundary(int) const = 0;
	virtual bool isBoundaryVertex(int) const = 0;

	virtual Eigen::Vector3d getVertexCoord(int) = 0;
	virtual double ComputeMeshVolume() = 0;

	virtual void ComputeLaplacian() = 0;//����Laplacian����
	virtual void ComputeLaplacian(int) = 0;//ָ������ĳ��Laplacian
	virtual void LoadVertex() = 0;
	virtual void SetVertexNewCoord(int, Eigen::Vector3d) = 0;
	virtual void SetVerticesNewCoord() = 0;

protected://��������Ĺ���
	//���������ε�����
	Eigen::Vector3d ComputeTriangleCirumcenter(Eigen::Vector3d& p1, Eigen::Vector3d& p2, Eigen::Vector3d& p3);
	void ComputeLAR();
	double ComputeTriangleArea(Eigen::Vector3d& p1, Eigen::Vector3d& p2, Eigen::Vector3d& p3);
	void VertexModified();
public://��Ҫ�ڸ���cpp��ʵ�ֵ�����

	void MakeNoise();
	void Fairing(int);
	void Smoothing(int);
	void ComputeCurvature();

	void getVCurvature(std::vector<double>& c);

	void SetLaplacianKind(int i);// { Laplacian_kind = i; };
	void SetLARKind(int i);
	void SetCurvatureKind(int i);// { Curvature_kind = i; Curvature_latest = false; };//��Ҫ����

	void eigen_output();
protected:
	Eigen::VectorXd LAR; //�ֲ�ƽ����������
	bool LAR_latest = false;

	Eigen::MatrixXd Vertices;//���񶥵�Eigen::Matrix3d FaceNormal;//�淨��Ψһ
	bool Vertices_latest = false;
	
	Eigen::MatrixXd VertexNormal;//���㷨�򣬶��ּ�Ȩ��ʽ
	bool VertexNormal_latest = false;
	
	Eigen::MatrixXd FaceNormal;
	bool FaceNormal_latest = false;
	
	Eigen::VectorXd Curvature;//���������
	bool Curvature_latest = false;

	Eigen::SparseMatrix<double> Laplacian;
	bool Laplacian_latest = false;

	double MeshVolume;
	bool MeshVolume_latest = false;

	int Laplacian_kind = -1;
	int LAR_kind = -1;
	int Curvature_kind = -1;

//	LinSysSolver* solver;
};