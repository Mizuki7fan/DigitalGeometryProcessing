#include "ChapterWidget.h"

ControlPanel::ControlPanel(QWidget* parent)
{	
	lName->setText(QStringLiteral("==  �������  =="));
	pbGeneral = new QPushButton(QStringLiteral("ͨ��"));
	pbChap1 = new QPushButton(QStringLiteral("��ɢ΢�ּ���"));

	QGridLayout* pLayout = new QGridLayout();
	pLayout->addWidget(lName, 0, 0);
	pLayout->addWidget(pbGeneral, 1, 0);
	pLayout->addWidget(pbChap1, 2, 0);
	pLayout->setAlignment(Qt::AlignTop);
	pLayout->setMargin(0);
	this->setLayout(pLayout);
	connect(pbGeneral, &QPushButton::clicked, this, [=]() {this->changeWidget(1); });
	connect(pbChap1, &QPushButton::clicked, this, [=]() {this->changeWidget(2); });
}

ControlPanel::~ControlPanel()
{
}

GeneralWidget::GeneralWidget(QWidget* parent)
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

GeneralWidget::~GeneralWidget()
{
}

MyWidget::MyWidget(QWidget* parent)
	: QWidget(parent)
{
	pbReturn = new QPushButton(QStringLiteral("����"));
	connect(pbReturn, &QPushButton::clicked, this, [=]() {this->changeWidget(0);});
	lName = new QLabel("");
	lName->setAlignment(Qt::AlignHCenter);
}

MyWidget::~MyWidget()
{
}

DiscreteDifferentialGeometryWidget::DiscreteDifferentialGeometryWidget(QWidget* _parent)
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

DiscreteDifferentialGeometryWidget::~DiscreteDifferentialGeometryWidget()
{
}
