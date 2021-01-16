#pragma once
#include "MeshDefinition.h"

namespace MeshUtility{
	//���������㣬��ȡ���������
	double CalcTriangleArea(T::Point& p1, T::Point& p2, T::Point& p3);

	//����������棬��ȡ����������
	void GetFaceVertices(T& m, T::FaceHandle& f, T::VertexHandle& v0, T::VertexHandle& v1, T::VertexHandle& v2);

	//�����������㣬�������Ҷ�����ȡ��Ӧ�ĽǶ�ֵ
	void CalcVertexAngle(T::Point& p1, T::Point& p2, T::Point& p3,double&angle1,double&angle2,double&angle3);
}
