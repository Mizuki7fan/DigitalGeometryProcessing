#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include "../general/MeshDefinition.h"
#include "../general/MeshUtilities.h"
#include <Eigen/Dense>
#include <Eigen/Sparse>

int main(int argc, char* argv[])
{
	std::cout << "===============\tLSCM\t===============\n\n";
	std::cout << "example:\tlscm.exe\talien_open.off\t1\t100\n\n";
	std::cout << "=======================================\n";
	std::string mpath;
	int vid1, vid2;
	T mesh;
	std::stringstream ss;
	if (argc != 4)
	{
		std::cout << "������Ŀ������ȷ\n";
		return -1;
	}
	else
	{
		ss << argv[1] << " " << argv[2] << " " << argv[3];
		ss >> mpath >> vid1 >> vid2;
	}
	OpenMesh::IO::read_mesh(mesh, ((argc > 1) ? argv[1] : mpath + "alien_open.off"));
	int nv = mesh.n_vertices();
	if (vid1 > nv - 1 || vid2 > nv - 1 || vid1 == vid2)
	{
		std::cout << "����������\n";
		return -1;
	}

	Eigen::SparseMatrix<double> A(2 * nv + 4, 2 * nv + 4);
	//������̴��������E��������ÿ��ֵ��ƫ��������2nv��������ÿ����������ƫ������������2nv*2nv�ġ�
	//��Ҫ�̶�2���㣬����Ҫ��4����ʽ��
	std::vector<Eigen::Triplet<double>> tripletsL;
	A.setZero();

	for (T::FaceHandle fh : mesh.faces())
	{
		int idx = fh.idx();
		T::VertexHandle fv[3];
		MeshUtility::GetFaceVertices(mesh, fh, fv[0], fv[1], fv[2]);
		double x0, y0, x1, y1, x2, y2;
		T::Point p0 = mesh.point(fv[0]), p1 = mesh.point(fv[1]), p2 = mesh.point(fv[2]);
		double Area = MeshUtility::CalcTriangleArea(p0, p1, p2);
		//ÿ����Ƭ����ƽ������
		x0 = 0; y0 = 0;
		OpenMesh::Vec3d e01 = p1 - p0;
		x1 = e01.length(), y1 = 0;
		OpenMesh::Vec3d e02 = p2 - p0;
		y2 = 2 * Area / e01.length();
		x2 = sqrt(e02.length() * e02.length() - y2 * y2);
		//S��һ��2x3�ľ��󣬱�ʾƫ������ֵ�������ʾ�������󵼺���u��v�����ϣ��������������λ�á�
		Eigen::MatrixXd S(2, 3);
		S(0, 0) = y1 - y2; S(0, 1) = y2 - y0; S(0, 2) = y0 - y1;
		S(1, 0) = x2 - x1; S(1, 1) = x0 - x2; S(1, 2) = x1 - x0;
		//������Ҫ������������2�Ƕ�ƽ���󵼲�����
		S = S / (2 * Area);
		//֮�����뷽��ϵ�����ɡ�

		// ������ǰһ��
		// /partial{E}_{/partial{u_i}}
		tripletsL.push_back(Eigen::Triplet<double>(fv[0].idx(), fv[0].idx(), 2 * S(0, 0) * S(0, 0)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[0].idx(), fv[1].idx(), 2 * S(0, 0) * S(0, 1)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[0].idx(), fv[2].idx(), 2 * S(0, 0) * S(0, 2)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[0].idx(), fv[0].idx() + nv, -2 * S(0, 0) * S(1, 0)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[0].idx(), fv[1].idx() + nv, -2 * S(0, 0) * S(1, 1)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[0].idx(), fv[2].idx() + nv, -2 * S(0, 0) * S(1, 2)));
		// /partial{E}_{/partial{u_j}}
		tripletsL.push_back(Eigen::Triplet<double>(fv[1].idx(), fv[0].idx(), 2 * S(0, 1) * S(0, 0)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[1].idx(), fv[1].idx(), 2 * S(0, 1) * S(0, 1)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[1].idx(), fv[2].idx(), 2 * S(0, 1) * S(0, 2)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[1].idx(), fv[0].idx() + nv, -2 * S(0, 1) * S(1, 0)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[1].idx(), fv[1].idx() + nv, -2 * S(0, 1) * S(1, 1)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[1].idx(), fv[2].idx() + nv, -2 * S(0, 1) * S(1, 2)));
		// /partial{E}_{/partial{u_k}}
		tripletsL.push_back(Eigen::Triplet<double>(fv[2].idx(), fv[0].idx(), 2 * S(0, 2) * S(0, 0)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[2].idx(), fv[1].idx(), 2 * S(0, 2) * S(0, 1)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[2].idx(), fv[2].idx(), 2 * S(0, 2) * S(0, 2)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[2].idx(), fv[0].idx() + nv, -2 * S(0, 2) * S(1, 0)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[2].idx(), fv[1].idx() + nv, -2 * S(0, 2) * S(1, 1)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[2].idx(), fv[2].idx() + nv, -2 * S(0, 2) * S(1, 2)));

		// /partial{E}_{/partial{v_i}}
		tripletsL.push_back(Eigen::Triplet<double>(fv[0].idx() + nv, fv[0].idx(), -2 * S(1, 0) * S(0, 0)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[0].idx() + nv, fv[1].idx(), -2 * S(1, 0) * S(0, 1)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[0].idx() + nv, fv[2].idx(), -2 * S(1, 0) * S(0, 2)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[0].idx() + nv, fv[0].idx() + nv, 2 * S(1, 0) * S(1, 0)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[0].idx() + nv, fv[1].idx() + nv, 2 * S(1, 0) * S(1, 1)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[0].idx() + nv, fv[2].idx() + nv, 2 * S(1, 0) * S(1, 2)));
		// /partial{E}_{/partial{v_j}}
		tripletsL.push_back(Eigen::Triplet<double>(fv[1].idx() + nv, fv[0].idx(), -2 * S(1, 1) * S(0, 0)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[1].idx() + nv, fv[1].idx(), -2 * S(1, 1) * S(0, 1)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[1].idx() + nv, fv[2].idx(), -2 * S(1, 1) * S(0, 2)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[1].idx() + nv, fv[0].idx() + nv, 2 * S(1, 1) * S(1, 0)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[1].idx() + nv, fv[1].idx() + nv, 2 * S(1, 1) * S(1, 1)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[1].idx() + nv, fv[2].idx() + nv, 2 * S(1, 1) * S(1, 2)));
		// /partial{E}_{/partial{u_k}}
		tripletsL.push_back(Eigen::Triplet<double>(fv[2].idx() + nv, fv[0].idx(), -2 * S(1, 2) * S(0, 0)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[2].idx() + nv, fv[1].idx(), -2 * S(1, 2) * S(0, 1)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[2].idx() + nv, fv[2].idx(), -2 * S(1, 2) * S(0, 2)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[2].idx() + nv, fv[0].idx() + nv, 2 * S(1, 2) * S(1, 0)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[2].idx() + nv, fv[1].idx() + nv, 2 * S(1, 2) * S(1, 1)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[2].idx() + nv, fv[2].idx() + nv, 2 * S(1, 2) * S(1, 2)));

		//�����ĺ�һ��
		// /partial{E}_{/partial{u_i}}
		tripletsL.push_back(Eigen::Triplet<double>(fv[0].idx(), fv[0].idx(), 2 * S(1, 0) * S(1, 0)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[0].idx(), fv[1].idx(), 2 * S(1, 0) * S(1, 1)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[0].idx(), fv[2].idx(), 2 * S(1, 0) * S(1, 2)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[0].idx(), fv[0].idx() + nv, 2 * S(1, 0) * S(0, 0)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[0].idx(), fv[1].idx() + nv, 2 * S(1, 0) * S(0, 1)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[0].idx(), fv[2].idx() + nv, 2 * S(1, 0) * S(0, 2)));
		// /partial{E}_{/partial{u_j}}
		tripletsL.push_back(Eigen::Triplet<double>(fv[1].idx(), fv[0].idx(), 2 * S(1, 1) * S(1, 0)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[1].idx(), fv[1].idx(), 2 * S(1, 1) * S(1, 1)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[1].idx(), fv[2].idx(), 2 * S(1, 1) * S(1, 2)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[1].idx(), fv[0].idx() + nv, 2 * S(1, 1) * S(0, 0)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[1].idx(), fv[1].idx() + nv, 2 * S(1, 1) * S(0, 1)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[1].idx(), fv[2].idx() + nv, 2 * S(1, 1) * S(0, 2)));
		// /partial{E}_{/partial{u_k}}
		tripletsL.push_back(Eigen::Triplet<double>(fv[2].idx(), fv[0].idx(), 2 * S(1, 2) * S(1, 0)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[2].idx(), fv[1].idx(), 2 * S(1, 2) * S(1, 1)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[2].idx(), fv[2].idx(), 2 * S(1, 2) * S(1, 2)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[2].idx(), fv[0].idx() + nv, 2 * S(1, 2) * S(0, 0)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[2].idx(), fv[1].idx() + nv, 2 * S(1, 2) * S(0, 1)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[2].idx(), fv[2].idx() + nv, 2 * S(1, 2) * S(0, 2)));

		// /partial{E}_{/partial{v_i}}
		tripletsL.push_back(Eigen::Triplet<double>(fv[0].idx() + nv, fv[0].idx(), 2 * S(0, 0) * S(1, 0)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[0].idx() + nv, fv[1].idx(), 2 * S(0, 0) * S(1, 1)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[0].idx() + nv, fv[2].idx(), 2 * S(0, 0) * S(1, 2)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[0].idx() + nv, fv[0].idx() + nv, 2 * S(0, 0) * S(0, 0)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[0].idx() + nv, fv[1].idx() + nv, 2 * S(0, 0) * S(0, 1)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[0].idx() + nv, fv[2].idx() + nv, 2 * S(0, 0) * S(0, 2)));
		// /partial{E}_{/partial{v_j}}
		tripletsL.push_back(Eigen::Triplet<double>(fv[1].idx() + nv, fv[0].idx(), 2 * S(0, 1) * S(1, 0)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[1].idx() + nv, fv[1].idx(), 2 * S(0, 1) * S(1, 1)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[1].idx() + nv, fv[2].idx(), 2 * S(0, 1) * S(1, 2)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[1].idx() + nv, fv[0].idx() + nv, 2 * S(0, 1) * S(0, 0)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[1].idx() + nv, fv[1].idx() + nv, 2 * S(0, 1) * S(0, 1)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[1].idx() + nv, fv[2].idx() + nv, 2 * S(0, 1) * S(0, 2)));
		// /partial{E}_{/partial{u_k}}
		tripletsL.push_back(Eigen::Triplet<double>(fv[2].idx() + nv, fv[0].idx(), 2 * S(0, 2) * S(1, 0)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[2].idx() + nv, fv[1].idx(), 2 * S(0, 2) * S(1, 1)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[2].idx() + nv, fv[2].idx(), 2 * S(0, 2) * S(1, 2)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[2].idx() + nv, fv[0].idx() + nv, 2 * S(0, 2) * S(0, 0)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[2].idx() + nv, fv[1].idx() + nv, 2 * S(0, 2) * S(0, 1)));
		tripletsL.push_back(Eigen::Triplet<double>(fv[2].idx() + nv, fv[2].idx() + nv, 2 * S(0, 2) * S(0, 2)));
	}
	//���ù̶���
	T::VertexHandle vh1 = mesh.vertex_handle(vid1);
	T::VertexHandle vh2 = mesh.vertex_handle(vid2);
	double dis = (mesh.point(vh1) - mesh.point(vh2)).length();//������������
	tripletsL.push_back(Eigen::Triplet<double>(2 * nv, vid1, 1));
	tripletsL.push_back(Eigen::Triplet<double>(vid1, 2 * nv, 1));
	tripletsL.push_back(Eigen::Triplet<double>(2 * nv + 1, vid1 + nv, 1));
	tripletsL.push_back(Eigen::Triplet<double>(vid1 + nv, 2 * nv + 1, 1));
	tripletsL.push_back(Eigen::Triplet<double>(2 * nv + 2, vid2, 1));
	tripletsL.push_back(Eigen::Triplet<double>(vid2, 2 * nv + 2, 1));
	tripletsL.push_back(Eigen::Triplet<double>(2 * nv + 3, vid2 + nv, 1));
	tripletsL.push_back(Eigen::Triplet<double>(vid2 + nv, 2 * nv + 3, 1));

	A.setFromTriplets(tripletsL.begin(), tripletsL.end());//���ú�A
	Eigen::VectorXd b(2 * nv + 4);
	b.setZero();
	b(2 * nv + 2) = dis;
	Eigen::SparseLU<Eigen::SparseMatrix<double>> solver;
	solver.compute(A);
	Eigen::VectorXd res = solver.solve(b);//����������ĵ�
	for (T::VertexHandle vh : mesh.vertices())
	{
		int idx = vh.idx();
		T::Point p(res(idx), res(idx + nv), 0);
		mesh.set_point(vh, p);
	}
	OpenMesh::IO::write_mesh(mesh, "output.off");
	return 0;
}