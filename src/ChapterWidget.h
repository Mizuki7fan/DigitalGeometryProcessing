#pragma once
#include <QtWidgets>
#include <QWidget>
#include <QtGui>

class MyWidget :public QWidget
{
	Q_OBJECT;
public:
	MyWidget();

protected:
	QLabel* lName;
	QPushButton* pbReturn;
	QPushButton* pbRedo;//����
	std::vector<QFrame*> Seperator;//�ָ���
	int SeperatorCount = 10;//���÷ָ��ߵ���Ŀ
protected:
signals:
	void changeWidget(int i);
	void Redo();
	void openDebug();
};

class ControlPanel :public MyWidget
{
	Q_OBJECT;
public:
	ControlPanel();

private:
	QPushButton* pbDebug;
	QPushButton* pbGeneral;
	QPushButton* pbChap1;
	QPushButton* pbChap2;
	QPushButton* pbChap3;
};

class GeneralWidget :public MyWidget
{
	Q_OBJECT;
public:
	GeneralWidget();

private:
signals:
	void PrintInfo();
	void CalcVolumeSignal();//�������

private:
	QPushButton* pbPrintInfo;
	QPushButton* pbCalcVolume;
};

class DiscreteDifferentialGeometryWidget :public MyWidget
{
	Q_OBJECT;
public:
	DiscreteDifferentialGeometryWidget();

private:
signals:
	void ComputeCurvatureSignal(int,int);
private:
	QComboBox* qbLocalAverageRegion;
	QComboBox* qbCurvatureKind;
	QPushButton* pbComputeCurvature;

};

class Smoothing :public MyWidget
{
	Q_OBJECT;
public:
	Smoothing();

private:
signals:
	void MakeNoiseSignal();
	void DoFairingSignal(int,int);//Laplace�Գ˵Ĵ�����laplace������
	void DoSmoothingSignal(int,int);
	void DoBilateralDenoisingSignal(double,double);
	void DoBilateralNormalFilteringSignal(double, double);

private:
	int SeperatorCount = 5;//����зָ���������
	QPushButton* pbMakeNoise;
	QPushButton* pbFairing;//��Fairing
	QPushButton* pbLaplacianSmoothing;//����Laplacianƽ��
	QPushButton* pbBilateralMeshDenoising;//���������˫���˲�
	QPushButton* pbBiateralNormalFiltering;//����ķ�������˲�

	QPushButton* pbManifoldHarmonics;//Ƶ�׵ĽǶ������д���

	QLineEdit* leFairingPower;//Fairing����
	QComboBox* qbLaplacianKind;//Laplacian������
	QComboBox* qbIntegrationKind;//ŷ�����ֵ�����
	QComboBox* qbLaplacianSmoothObject;//�Զ���λ�û������ʽ���Laplacian��ƽ��
};

class Parameterization :public MyWidget
{
	Q_OBJECT;
public:
	Parameterization();

private:
signals:
	void CalcTutteSignal();
	void RedoSignal();

private:
	int SeperatorCount = 1;
	QPushButton* pbCalcTutte;

};