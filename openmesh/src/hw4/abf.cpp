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
	std::cout << "===============\tABF\t===============\n\n";
	std::cout << "example:\tabf.exe\talien_open.off\n\n";
	std::cout << "=======================================\n";
	std::string mpath;
	T mesh;
	std::stringstream ss;
	if (argc != 2)
	{
		std::cout << "������Ŀ������ȷ\n";
		return -1;
	}
	else
	{
		ss << argv[1];
		ss >> mpath;
	}
	OpenMesh::IO::read_mesh(mesh, argv[1]);

	//=====================1.����Ԥ����=======================
	int nv = mesh.n_vertices();
	int nf = mesh.n_faces();
	//������id���±����vid

	std::vector<std::vector<int>> fid_2_vid(mesh.n_faces(), std::vector<int>(3));
	//������Ͷ��������Ϣ
	std::vector<std::map<int, double>> real_angle_info;
	std::vector<std::map<int, double>> beta_info;
	real_angle_info.resize(mesh.n_faces());
	for (auto fh : mesh.faces())
	{
		T::VertexHandle fv[3];
		MeshUtility::GetFaceVertices(mesh, fh, fv[0], fv[1], fv[2]);
		double angle[3];
		MeshUtility::CalcVertexAngle(mesh.point(fv[0]), mesh.point(fv[1]), mesh.point(fv[2]), angle[0], angle[1], angle[2]);
		real_angle_info[fh.idx()][fv[0].idx()] = angle[0];
		fid_2_vid[fh.idx()][0] = fv[0].idx();
		real_angle_info[fh.idx()][fv[1].idx()] = angle[1];
		fid_2_vid[fh.idx()][1] = fv[1].idx();
		real_angle_info[fh.idx()][fv[2].idx()] = angle[2];
		fid_2_vid[fh.idx()][2] = fv[2].idx();
	}
	//��¼��Ȩƽ����ĽǶȵ���Ϣ
	beta_info.resize(mesh.n_faces());
	for (auto vh : mesh.vertices())
	{
		int vid = vh.idx();
		if (mesh.is_boundary(vh))//����Ǳ߽��
		{
			for (auto vfi : mesh.vf_range(vh))
			{
				beta_info[vfi.idx()][vh.idx()] = real_angle_info[vfi.idx()][vh.idx()];
			}
		}
		else
		{
			double total_angle_value = 0;//��һ����ĽǶ�ֵ֮��
			for (auto vfi : mesh.vf_range(vh))
			{
				total_angle_value += real_angle_info[vfi.idx()][vid];
			}
			for (auto vfi : mesh.vf_range(vh))
			{
				beta_info[vfi.idx()][vh.idx()] = real_angle_info[vfi.idx()][vh.idx()] * total_angle_value / (2 * M_PI);
			}
		}
	}

	int n_bnd_v = 0;
	for (T::VertexHandle vh : mesh.vertices())
		if (!mesh.is_boundary(vh))
			n_bnd_v++;


	int dim = 3 * nf + nf + n_bnd_v + n_bnd_v;
	Eigen::SparseMatrix<double> A(dim, dim);
	Eigen::VectorXd b(dim);
	std::vector<Eigen::Triplet<double>> tripletsL;
	A.setZero();
	b.setZero();

	//============================2.��д��������Ӧ��ϵ�������ֵ
	//��д�������Ӧ����ֵ��ƽ���󵼺���һ��2������Ȩ��Ҫ����2
	for (int fid = 0; fid < nf; fid++)
	{
		tripletsL.push_back(Eigen::Triplet<double>(3 * fid, 3 * fid, 2 / pow(beta_info[fid][fid_2_vid[fid][0]], 2)));
		b[3 * fid] = 0;
		tripletsL.push_back(Eigen::Triplet<double>(3 * fid + 1, 3 * fid + 1, 2 / pow(beta_info[fid][fid_2_vid[fid][1]], 2)));
		b[3 * fid + 1] = 0;
		tripletsL.push_back(Eigen::Triplet<double>(3 * fid + 2, 3 * fid + 2, 2 / pow(beta_info[fid][fid_2_vid[fid][2]], 2)));
		b[3 * fid + 2] = 0;
	}
	//============================3.��д�ڽǺ͵���180�ȵ�Լ��
	for (int fid = 0; fid < nf; fid++)
	{
		tripletsL.push_back(Eigen::Triplet<double>(3 * nf + fid, 3 * fid, 1));
		tripletsL.push_back(Eigen::Triplet<double>(3 * nf + fid, 3 * fid + 1, 1));
		tripletsL.push_back(Eigen::Triplet<double>(3 * nf + fid, 3 * fid + 2, 1));//�з����ֵΪ1

		tripletsL.push_back(Eigen::Triplet<double>(3 * fid, 3 * nf + fid, -1));
		tripletsL.push_back(Eigen::Triplet<double>(3 * fid + 1, 3 * nf + fid, -1));
		tripletsL.push_back(Eigen::Triplet<double>(3 * fid + 2, 3 * nf + fid, -1));//�з����ֵΪ-1

		b[3 * nf + fid] = M_PI - beta_info[fid][fid_2_vid[fid][0]] - beta_info[fid][fid_2_vid[fid][1]] - beta_info[fid][fid_2_vid[fid][2]];
	}

	//=============================4.��д�ڲ���1-����ǶȺ͵���2pi��Լ��
	//����ÿ���ڲ��㣬��Ӧ������һ����ĽǶȵ���2pi��Լ����ͬ�ϣ������ֵӦ����2pi��ȥһ�����ĽǶ�֮��
	int n_bnd_idx = 0;
	for (auto vh : mesh.vertices())
	{
		if (mesh.is_boundary(vh))
			continue;
		double total_degree = 0;
		for (auto vfh : mesh.vf_range(vh))
		{
			int fid = vfh.idx();
			total_degree += beta_info[fid][vh.idx()];
			//Ѱ���������������еı��
			int vid = 0;
			if (fid_2_vid[fid][0] == vh.idx()) vid = 0;
			else if (fid_2_vid[fid][1] == vh.idx()) vid = 1;
			else if (fid_2_vid[fid][2] == vh.idx()) vid = 2;
			//���λ������ֵ
			tripletsL.push_back(Eigen::Triplet<double>(4 * nf + n_bnd_idx, 3 * fid + vid, 1));
			tripletsL.push_back(Eigen::Triplet<double>(3 * fid + vid, 4 * nf + n_bnd_idx, -1));
		}
		b[4 * nf + n_bnd_idx] = 2 * M_PI - total_degree;
		n_bnd_idx++;
	}
	//=============================5.��д�ع�Լ��
	//������ع�Լ��������һ�����Ⱦ�Ĳ�����
	for (auto vh : mesh.vertices())
	{
		if (mesh.is_boundary(vh))
			continue;
		//ͳ��������������
		int vf_count = 0;
		for (auto vfi : mesh.vf_range(vh))
			vf_count++;

		//ѡȡһ����ʼ��
		T::FaceHandle f_it;
		for (auto vfi : mesh.vf_range(vh))
		{
			f_it = vfi;
			break;
		}
		int fv_positive_idx = 0, fv_negative_idx = 0;
		double b_value = 0;
		//���ݶ���vh�Լ���f_it�����ó�ʼ������
		if (fid_2_vid[f_it.idx()][0] == vh.idx())
		{
			fv_positive_idx = 1;
			fv_negative_idx = 2;
		}
		else if (fid_2_vid[f_it.idx()][1] == vh.idx())
		{
			fv_positive_idx = 0;
			fv_negative_idx = 2;
		}
		else if (fid_2_vid[f_it.idx()][2] == vh.idx())
		{
			fv_positive_idx = 0;
			fv_negative_idx = 1;
		}
		//��������͸��㣬�ֱ�������ֵ
		double gamma1 = beta_info[f_it.idx()][fid_2_vid[f_it.idx()][fv_positive_idx]];
		tripletsL.push_back(Eigen::Triplet<double>(4 * nf + n_bnd_idx, 3 * f_it.idx() + fv_positive_idx, atan(gamma1)));
		tripletsL.push_back(Eigen::Triplet<double>(3 * f_it.idx() + fv_positive_idx, 4 * nf + n_bnd_idx, -atan(gamma1)));

		double gamma2 = beta_info[f_it.idx()][fid_2_vid[f_it.idx()][fv_negative_idx]];
		tripletsL.push_back(Eigen::Triplet<double>(4 * nf + n_bnd_idx, 3 * f_it.idx() + fv_negative_idx, atan(gamma2)));
		tripletsL.push_back(Eigen::Triplet<double>(3 * f_it.idx() + fv_negative_idx, 4 * nf + n_bnd_idx, -atan(gamma2)));
		b_value = b_value - log(sin(gamma1)) + log(sin(gamma2));
		while (vf_count > 1)
		{
			//��֪vh.idx(),fv_iter,fv_positive_idx,�����������Ϣ
			int v2idx = fid_2_vid[f_it.idx()][fv_positive_idx];//�����ڵ�
			T::HalfedgeHandle hvhv2;//Ѱ�����ڰ��
			for (auto heh : mesh.voh_range(vh))
			{
				int oppov = mesh.to_vertex_handle(heh).idx();
				if (oppov == v2idx)
				{
					hvhv2 = heh;
					break;
				}
			}
			//Ѱ�ҵ����ڵ���id
			f_it = mesh.opposite_face_handle(hvhv2);
			//��֪��id�Ͷ���id������������������Ϊ��
			if (fid_2_vid[f_it.idx()][0] == v2idx)
				fv_negative_idx = 0;
			else 	if (fid_2_vid[f_it.idx()][1] == v2idx)
				fv_negative_idx = 1;
			else 	if (fid_2_vid[f_it.idx()][2] == v2idx)
				fv_negative_idx = 2;

			if (fid_2_vid[f_it.idx()][0] == vh.idx())
				fv_positive_idx = 3 - fv_negative_idx;
			else 	if (fid_2_vid[f_it.idx()][1] == vh.idx())
				fv_positive_idx = 3 - 1 - fv_negative_idx;
			else 	if (fid_2_vid[f_it.idx()][2] == vh.idx())
				fv_positive_idx = 3 - 2 - fv_negative_idx;
			//��֪f_it,positive_idx��negetive_idx
			double gamma1 = beta_info[f_it.idx()][fid_2_vid[f_it.idx()][fv_positive_idx]];
			tripletsL.push_back(Eigen::Triplet<double>(4 * nf + n_bnd_idx, 3 * f_it.idx() + fv_positive_idx, atan(gamma1)));
			tripletsL.push_back(Eigen::Triplet<double>(3 * f_it.idx() + fv_positive_idx, 4 * nf + n_bnd_idx, -atan(gamma1)));

			double gamma2 = beta_info[f_it.idx()][fid_2_vid[f_it.idx()][fv_negative_idx]];
			tripletsL.push_back(Eigen::Triplet<double>(4 * nf + n_bnd_idx, 3 * f_it.idx() + fv_negative_idx, atan(gamma2)));
			tripletsL.push_back(Eigen::Triplet<double>(3 * f_it.idx() + fv_negative_idx, 4 * nf + n_bnd_idx, -atan(gamma2)));

			b_value = b_value - log(sin(gamma1)) + log(sin(gamma2));
			vf_count--;
		}
		b[4 * nf + n_bnd_idx] = b_value;
		n_bnd_idx++;
	}

	std::cout << "��ʼʹ��Eigen�ⷽ�̻�ýǶȱ仯\n";
	A.setFromTriplets(tripletsL.begin(), tripletsL.end());//���ú�A
	Eigen::SparseLU<Eigen::SparseMatrix<double>> solver;
	solver.compute(A);
	Eigen::VectorXd delta_angle = solver.solve(b);//��ȡÿ���ǶȲ����ֵ


	//=============================6.��ȡ���������������Ƭ�ĽǶ�ֵ
	std::vector<std::map<int, double>> alpha_info;
	alpha_info.resize(mesh.n_faces());
	for (int i = 0; i < beta_info.size(); i++)
	{
		alpha_info[i][fid_2_vid[i][0]] = beta_info[i][fid_2_vid[i][0]] + delta_angle[3 * i + 0];
		alpha_info[i][fid_2_vid[i][1]] = beta_info[i][fid_2_vid[i][1]] + delta_angle[3 * i + 1];
		alpha_info[i][fid_2_vid[i][2]] = beta_info[i][fid_2_vid[i][2]] + delta_angle[3 * i + 2];
	}

	//==============================7.ʹ����С���˷���ԭ����λ��
	dim = 2 * nv + 4;//ϵ������Ϊ2���Ķ��������������̶���
	Eigen::SparseMatrix<double> A2(dim, dim);
	Eigen::VectorXd b2(dim);
	std::vector<Eigen::Triplet<double>> tripletsL2;
	A2.setZero();
	b2.setZero();
	Eigen::Matrix2d M;//����M��һ����ά����
	//����ÿ������˵�����ܽ���һ����ʽ
	for (auto fh : mesh.faces())
	{
		//��ȷ��M����ֵ
		int Pj = fid_2_vid[fh.idx()][0], Pk = fid_2_vid[fh.idx()][1], Pl = fid_2_vid[fh.idx()][2];
		double rate = sin(alpha_info[fh.idx()][Pk]) / sin(alpha_info[fh.idx()][Pl]);
		//����M
		M(0, 0) = cos(alpha_info[fh.idx()][Pj]);		M(0, 1) = -sin(alpha_info[fh.idx()][Pj]);
		M(1, 0) = -M(0, 1);									M(1, 1) = M(0, 0);
		M = M * rate;

		//֮������Ƶ�����6��������ÿ��ϵ��������䣬���˵���ļ���
		tripletsL2.push_back(Eigen::Triplet<double>(Pj, Pj, 2 * (1 - M(0, 0)) * (1 - M(0, 0)) + 2 * M(1, 0) * M(1, 0)));
		tripletsL2.push_back(Eigen::Triplet<double>(Pj, Pk, 2 * (1 - M(0, 0)) * M(0, 0) - 2 * M(1, 0) * M(1, 0)));
		tripletsL2.push_back(Eigen::Triplet<double>(Pj, Pl, -2 * (1 - M(0, 0))));
		tripletsL2.push_back(Eigen::Triplet<double>(Pj, Pj + nv, -2 * M(0, 1) * (1 - M(0, 0)) - 2 * M(1, 0) * (1 - M(1, 1))));
		tripletsL2.push_back(Eigen::Triplet<double>(Pj, Pk + nv, 2 * M(0, 1) * (1 - M(0, 0)) - 2 * M(1, 0) * M(1, 1)));
		tripletsL2.push_back(Eigen::Triplet<double>(Pj, Pl + nv, 2 * M(1, 0)));

		//========================================
		tripletsL2.push_back(Eigen::Triplet<double>(Pk, Pj, 2 * M(0, 0) * (1 - M(0, 0)) - 2 * M(1, 0) * M(1, 0)));
		tripletsL2.push_back(Eigen::Triplet<double>(Pk, Pk, 2 * M(0, 0) * M(0, 0) + 2 * M(1, 0) * M(1, 0)));
		tripletsL2.push_back(Eigen::Triplet<double>(Pk, Pl, -2 * M(0, 0)));
		tripletsL2.push_back(Eigen::Triplet<double>(Pk, Pj + nv, -2 * M(0, 0) * M(0, 1) + 2 * M(1, 0) * (1 - M(1, 1))));
		tripletsL2.push_back(Eigen::Triplet<double>(Pk, Pk + nv, 2 * M(0, 0) * M(0, 1) + 2 * M(1, 0) * M(1, 1)));
		tripletsL2.push_back(Eigen::Triplet<double>(Pk, Pl + nv, -2 * M(1, 0)));

		//============================
		tripletsL2.push_back(Eigen::Triplet<double>(Pl, Pj, -2 * (1 - M(0, 0))));
		tripletsL2.push_back(Eigen::Triplet<double>(Pl, Pk, -2 * M(0, 0)));
		tripletsL2.push_back(Eigen::Triplet<double>(Pl, Pl, 2));
		tripletsL2.push_back(Eigen::Triplet<double>(Pl, Pj + nv, 2 * M(0, 1)));
		tripletsL2.push_back(Eigen::Triplet<double>(Pl, Pk + nv, -2 * M(0, 1)));
		//tripletsL2.push_back(Eigen::Triplet<double>(Pl, Pl + nv,0));

		//=========================================
		tripletsL2.push_back(Eigen::Triplet<double>(Pj + nv, Pj, -2 * M(0, 1) * (1 - M(0, 0)) - 2 * (1 - M(1, 1)) * M(1, 0)));
		tripletsL2.push_back(Eigen::Triplet<double>(Pj + nv, Pk, -2 * M(0, 1) * M(0, 0) + 2 * (1 - M(1, 1)) * M(1, 0)));
		tripletsL2.push_back(Eigen::Triplet<double>(Pj + nv, Pl, 2 * M(0, 1)));
		tripletsL2.push_back(Eigen::Triplet<double>(Pj + nv, Pj + nv, 2 * M(0, 1) * M(0, 1) + 2 * (1 - M(1, 1)) * (1 - M(1, 1))));
		tripletsL2.push_back(Eigen::Triplet<double>(Pj + nv, Pk + nv, -2 * M(0, 1) * M(0, 1) + 2 * (1 - M(1, 1)) * M(1, 1)));
		tripletsL2.push_back(Eigen::Triplet<double>(Pj + nv, Pl + nv, -2 * (1 - M(1, 1))));

		//=====================================
		tripletsL2.push_back(Eigen::Triplet<double>(Pk + nv, Pj, 2 * M(0, 1) * (1 - M(0, 0)) - 2 * M(1, 0) * M(1, 1)));
		tripletsL2.push_back(Eigen::Triplet<double>(Pk + nv, Pk, 2 * M(0, 1) * M(0, 0) + 2 * M(1, 0) * M(1, 1)));
		tripletsL2.push_back(Eigen::Triplet<double>(Pk + nv, Pl, -2 * M(0, 1)));
		tripletsL2.push_back(Eigen::Triplet<double>(Pk + nv, Pj + nv, -2 * M(0, 1) * M(0, 1) + 2 * M(1, 1) * (1 - M(1, 1))));
		tripletsL2.push_back(Eigen::Triplet<double>(Pk + nv, Pk + nv, 2 * M(0, 1) * M(0, 1) + 2 * M(1, 1) * M(1, 1)));
		tripletsL2.push_back(Eigen::Triplet<double>(Pk + nv, Pl + nv, -2 * M(1, 1)));

		//==============================================
		tripletsL2.push_back(Eigen::Triplet<double>(Pl + nv, Pj, 2 * M(1, 0)));
		tripletsL2.push_back(Eigen::Triplet<double>(Pl + nv, Pk, -2 * M(1, 0)));
		//tripletsL2.push_back(Eigen::Triplet<double>(Pl + nv, Pl,0));
		tripletsL2.push_back(Eigen::Triplet<double>(Pl + nv, Pj + nv, -2 * (1 - M(1, 1))));
		tripletsL2.push_back(Eigen::Triplet<double>(Pl + nv, Pk + nv, -2 * M(1, 1)));
		tripletsL2.push_back(Eigen::Triplet<double>(Pl + nv, Pl + nv, 2));
	}
	//��ӵ�ʽԼ��
	int v1, v2;
	double length;
	T::EdgeHandle he = mesh.edge_handle(1);
	v1 = mesh.to_vertex_handle(mesh.halfedge_handle(he, 0)).idx();
	v2 = mesh.from_vertex_handle(mesh.halfedge_handle(he, 0)).idx();
	length = mesh.calc_edge_length(he);

	std::cout << v1 << "   " << v2 << std::endl;

	tripletsL2.push_back(Eigen::Triplet<double>(2*nv, v1, 1));
	tripletsL2.push_back(Eigen::Triplet<double>(v1, 2*nv, -1));

	tripletsL2.push_back(Eigen::Triplet<double>(2 * nv+1, v1+nv, 1));
	tripletsL2.push_back(Eigen::Triplet<double>(v1+nv, 2 * nv+1, -1));

	tripletsL2.push_back(Eigen::Triplet<double>(2 * nv + 2, v2, 1));
	tripletsL2.push_back(Eigen::Triplet<double>(v2, 2 * nv + 2, -1));

	tripletsL2.push_back(Eigen::Triplet<double>(2 * nv + 3, v2+nv, 1));
	tripletsL2.push_back(Eigen::Triplet<double>(v2+nv, 2 * nv + 3, -1));

	b2[2 * nv + 2] = length;
	A2.setFromTriplets(tripletsL2.begin(), tripletsL2.end());//���ú�A

	Eigen::SparseLU<Eigen::SparseMatrix<double>> solver2;
	solver2.compute(A2);
	Eigen::VectorXd res = solver2.solve(b2);//��ȡÿ���ǶȲ����ֵ

	for (T::VertexHandle vh : mesh.vertices())
	{
		int idx = vh.idx();
		T::Point p(res(idx), res(idx + nv), 0);
		mesh.set_point(vh, p);
	}
	OpenMesh::IO::write_mesh(mesh, "output.off");
	std::cout << "�������" << std::endl;
}