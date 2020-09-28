#include "ChapterWidget.h"

ControlPanel::ControlPanel()
{	
	pbDebug = new QPushButton(QStringLiteral("�򿪲�������"));
	lName->setText(QStringLiteral("==  �������  =="));
	pbGeneral = new QPushButton(QStringLiteral("ͨ��"));
	pbChap1 = new QPushButton(QStringLiteral("��ɢ΢�ּ���"));
	pbChap2 = new QPushButton(QStringLiteral("Smoothing"));

	QGridLayout* pLayout = new QGridLayout();
	pLayout->addWidget(pbDebug);
	pLayout->addWidget(lName);
	pLayout->addWidget(pbGeneral);
	pLayout->addWidget(pbChap1);
	pLayout->addWidget(pbChap2);

	pLayout->setAlignment(Qt::AlignTop);
	pLayout->setMargin(0);
	this->setLayout(pLayout);
	connect(pbDebug, &QPushButton::clicked, this, [=]() {this->openDebug(); });
	connect(pbGeneral, &QPushButton::clicked, this, [=]() {this->changeWidget(1); });
	connect(pbChap1, &QPushButton::clicked, this, [=]() {this->changeWidget(2); });
	connect(pbChap2, &QPushButton::clicked, this, [=]() {this->changeWidget(3); });

}

GeneralWidget::GeneralWidget()
{
	lName->setText(QStringLiteral("==  ͨ�ù���  =="));
	pbPrintInfo = new QPushButton(QStringLiteral("��ӡ������Ϣ"));
	QGridLayout* layout = new QGridLayout();
	layout->addWidget(lName, 0, 0);
	layout->addWidget(pbReturn, 1, 0);
	layout->addWidget(pbPrintInfo, 2, 0);
	layout->setAlignment(Qt::AlignTop);
	layout->setMargin(0);
	this->setLayout(layout);
	connect(pbPrintInfo, &QPushButton::clicked, this, [=]() {this->PrintInfo(); });

}

MyWidget::MyWidget()
{
	pbReturn = new QPushButton(QStringLiteral("����"));
	connect(pbReturn, &QPushButton::clicked, this, [=]() {
		printf("123");
		this->changeWidget(0);}
	);
	lName = new QLabel("");
	lName->setAlignment(Qt::AlignHCenter);
}


DiscreteDifferentialGeometryWidget::DiscreteDifferentialGeometryWidget()
{
	lName->setText(QStringLiteral("==  ��ɢ΢�ּ���  =="));
	QLabel* lLocalAverageRegionKind=new QLabel(QStringLiteral("�ֲ�ƽ����������:"));
	QStringList LocalAverageRegionsKind;
	LocalAverageRegionsKind << "Barycentric" << "Voronoi" << "Mixed";
	QLabel* lCurvatureKind=new QLabel(QStringLiteral("�������:"));
	QStringList CurvatureKind;
	CurvatureKind << "Mean" << "AbsoluteMean" << "Gaussian";
	qbLocalAverageRegion = new QComboBox();
	qbLocalAverageRegion->addItems(LocalAverageRegionsKind);
	qbCurvatureKind = new QComboBox();
	qbCurvatureKind->addItems(CurvatureKind);
	pbComputeCurvature = new QPushButton(QStringLiteral("��������"));

	QVBoxLayout* pLayout = new QVBoxLayout();
	pLayout->addWidget(lName);
	pLayout->addWidget(pbReturn);
	pLayout->addWidget(lLocalAverageRegionKind);
	pLayout->addWidget(qbLocalAverageRegion);
	pLayout->addWidget(lCurvatureKind);
	pLayout->addWidget(qbCurvatureKind);
	pLayout->addWidget(pbComputeCurvature);

	pLayout->setAlignment(Qt::AlignTop);
	pLayout->setMargin(0);
	this->setLayout(pLayout);

	connect(pbComputeCurvature, &QPushButton::clicked, this, [=]() {
		int localaverageregion = qbLocalAverageRegion->currentIndex();
		int curvaturekind = qbCurvatureKind->currentIndex();
		this->ComputeCurvatureSignal(localaverageregion,curvaturekind); 
		});
}

Smoothing::Smoothing()
{
	lName->setText(QStringLiteral("==  SMOOTHING  =="));
	pbMakeNoise = new QPushButton(QStringLiteral("��������"));
	QLabel* lFairingPower=new QLabel(QStringLiteral("Fairing���ݴ�:"));
	leFairingPower = new QLineEdit();
	leFairingPower->setText("1");
	pbFairing = new QPushButton(QStringLiteral("��Fairing"));
	QLabel* lLaplacianKind = new QLabel(QStringLiteral("Laplacian����:"));
	QStringList LaplacianKind;
	LaplacianKind << "Uniform" << "Cotangent";
	qbLaplacianKind = new QComboBox();
	qbLaplacianKind->addItems(LaplacianKind);
	QFrame* line = new QFrame(), *line2 = new QFrame();
	line->setFrameShape(QFrame::HLine);
	line->setFrameShadow(QFrame::Sunken);
	line2->setFrameShape(QFrame::HLine);
	line2->setFrameShadow(QFrame::Sunken);
//	lSeparator->

	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(lName);
	layout->addWidget(pbReturn);
	layout->addWidget(pbMakeNoise);
	layout->addWidget(line);
	layout->addWidget(lFairingPower);
	layout->addWidget(leFairingPower);
	layout->addWidget(pbFairing);
	layout->addWidget(line2);
	layout->addWidget(lLaplacianKind);

	layout->setAlignment(Qt::AlignTop);
	layout->setMargin(0);
	this->setLayout(layout);

	connect(pbMakeNoise, SIGNAL(clicked()), this, SIGNAL(MakeNoiseSignal()));
	connect(pbFairing, &QPushButton::clicked, this, [=]() {
		int fairingpower = leFairingPower->text().toInt();
		this->DoFairingSignal(fairingpower);
		});

}
