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
	enum LAR_KIND { BARYCENTRIC, VORONOI, MIXED, LAR_ND};//定义、记录LAR的类型
	enum V_NORMAL_KIND { V_NORMAL_ND };
	enum CURVATURE_KIND { MEAN, ABSOLUTEMEAN, GAUSSIAN, CURVATURE_ND};
	enum LAPLACIAN_KIND { UNIFORM, CONTANGENT, LAPLACIAN_ND};
	enum EULER_INTEGRATION_KIND {EXPLICIT,IMPLICIT,EULER_INTEGRATION_ND};//欧拉积分的类型，显式和隐式
private:

public://直接通过网格读取进行的操作，以及不同网格类实现有很大不同的操作
	virtual bool Load(std::string) = 0;//读入网格
	virtual bool Write(std::string) = 0;//导出网格
	virtual void Clear() = 0;//清理网格
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
	virtual void getVerticesNeighbour(int i, std::vector<int>& nei) const=0;//获取点的1-邻域
	virtual void getFaceNeighbour(int i, std::vector<int>& nei)const = 0;
	virtual void getEdgeVertices(int e, int& v1, int& v2) const = 0;
	virtual double getEdgeLength(int e) const = 0;
	virtual void getFaceVertices(int f, int& v1, int& v2, int& v3) const = 0;
	virtual void getFaceAngles(int f, double& angle1, double& angle2, double& angle3) const = 0;
	virtual bool isBoundary(int) const = 0;
	virtual bool isBoundaryVertex(int) const = 0;

	virtual Eigen::Vector3d getVertexCoord(int) = 0;
	virtual double ComputeMeshVolume() = 0;

	virtual void ComputeLaplacian() = 0;//构建Laplacian矩阵
	virtual void LoadVertex() = 0;
	virtual void SetVertexNewCoord(int, Eigen::Vector3d) = 0;
	virtual void SetVerticesNewCoord() = 0;
	virtual void SetFacesNewNormalCoord() = 0;

	virtual void BilateralDenoising(double, double);
	virtual void BilateralNormalFiltering(double, double);

protected://辅助子类的功能
	//计算三角形的外心
	Eigen::Vector3d ComputeTriangleCirumcenter(Eigen::Vector3d& p1, Eigen::Vector3d& p2, Eigen::Vector3d& p3);
	void ComputeLAR();
	double ComputeTriangleArea(Eigen::Vector3d& p1, Eigen::Vector3d& p2, Eigen::Vector3d& p3);
public://主要在父类cpp中实现的内容

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

	Eigen::VectorXd LAR; //局部平均区域的面积
	bool LAR_latest = false;
	LAR_KIND LAR_kind = LAR_KIND::LAR_ND;

	Eigen::MatrixXd Vertices;//网格顶点Eigen::Matrix3d FaceNormal;//面法向，唯一
	bool Vertices_latest = false;
	
	Eigen::MatrixXd VertexNormal;//顶点法向，有三种加权方式，不过这个暂时不管(因为Openmesh提供了默认的)
	bool VertexNormal_latest = false;
 	
	Eigen::MatrixXd FaceNormal;
	bool FaceNormal_latest = false;
	
	Eigen::VectorXd Curvature;//顶点的曲率
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