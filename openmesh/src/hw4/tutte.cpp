#pragma once
#include <iostream>
#include <fstream>
#include "../general/MeshDefinition.h"
#include <Eigen/Dense>
#include <Eigen/Sparse>

void find_neighbour(T& m, int c,int& prev,int&next)
{
	//����һ���㣬Ѱ����ǰ��ı߽��
	int nei[2];
	int i = 0;
	T::VertexHandle vh = m.vertex_handle(c);
	//����Ҫ��ve_range��������vv_range����Ϊ���ܻ����ĳ�������ε������㶼�Ǳ߽������
	for (auto eh : m.ve_range(vh))
	{
		if (m.is_boundary(eh))
		{
			int v1 = m.to_vertex_handle(m.halfedge_handle(eh,0)).idx();
			int v2 = m.to_vertex_handle(m.halfedge_handle(eh, 1)).idx();
			nei[i] = (v1 == c) ? v2 : v1;
			i++;
		}
	}
	prev = nei[0];
	next = nei[1];
}


int main(int argc,char* argv[])
{
	std::cout << "===============\tTutte\t===============\n\n";
	std::cout << "example:\ttutte.exe\talien_open.off\n\n";
	std::cout << "=======================================\n";
	std::string mpath = "D:/MyRepo/DigitalGeometryPrecessing/MyDGP/mesh/";
	T mesh;
	OpenMesh::IO::read_mesh(mesh, ((argc > 1) ? argv[1] : mpath+"alien_open.off"));

	//�Ȱ�����߽�㰴˳��ͶӰ��һ��Բ�ϣ����һ��**�����**�߽��
	//Ϊ���ж��Ƿ��ж���߽磬��Ҫ��ͳ�Ʊߵ�������
	int st = -1;//��һ���߽��
	int n_border_v = 0;
	for (auto vh : mesh.vertices())
	{
		if (mesh.is_boundary(vh))
		{
			n_border_v++;
			st = vh.idx();
		}
	}
	std::vector<int> Bnd_v_inorder;
	Bnd_v_inorder.push_back(st);
	int prev_v = -1;
	int next_v = -1;//��һ�����id
	find_neighbour(mesh, st, prev_v, next_v);
	int center_v = st;
	while (next_v!=st)
	{
		Bnd_v_inorder.push_back(next_v);
		prev_v = center_v;
		center_v = next_v;
		int v1, v2;
		find_neighbour(mesh, center_v, v1, v2);
		next_v = (v1 == prev_v) ? v2 : v1;
	}
	if (Bnd_v_inorder.size() != n_border_v)
	{
		std::cerr << "���񲢷�Բ������" << std::endl;
		return 0;
	}
	//��������
	double r = 5.0;
	int bnd_size = n_border_v;
	Eigen::MatrixXd A, b;
	A.resize(mesh.n_vertices(), mesh.n_vertices());
	A.setZero();
	b.resize(mesh.n_vertices(), 3);
	b.setZero();
	for (double idx = 0; idx < bnd_size; idx++)
	{
		int vid = Bnd_v_inorder[idx];
		A(vid, vid) = 1;
		double theta = 2 * M_PI / bnd_size * idx;
		b(vid, 0) = r * cos(theta);
		b(vid, 1) = r * sin(theta);
	}
	for (T::VertexHandle vh : mesh.vertices())
	{
		if (mesh.is_boundary(vh))
			continue;
		//������vh������
		int nei_count = 0;
		for (T::VertexHandle vnei : mesh.vv_range(vh))
			nei_count++;
		for (T::VertexHandle vnei : mesh.vv_range(vh))
		{
			A(vh.idx(), vnei.idx()) = 1.0;
		}
		A(vh.idx(), vh.idx()) = -nei_count;//��ôд���Ի��һ���Գƾ���
	}
	Eigen::SparseMatrix<double> LL = A.sparseView();
	//Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> solver;Ҫ���������
	Eigen::SparseLU<Eigen::SparseMatrix<double>> solver;
	solver.compute(LL);
	Eigen::MatrixXd res = solver.solve(b);//����������ĵ�
	//Eigen::MatrixXd res = A.inverse() * b;
	for (T::VertexHandle vh : mesh.vertices())
	{
		int idx = vh.idx();
		T::Point p(res(idx, 0), res(idx, 1), res(idx, 2));
		mesh.set_point(vh, p);
	}
	
	OpenMesh::IO::write_mesh(mesh, "output.obj");
	return 0;
}