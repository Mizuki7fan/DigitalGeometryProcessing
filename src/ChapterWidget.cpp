#include "ChapterWidget.h"

ControlPanel::ControlPanel(QWidget* parent)
	: QWidget(parent)
{
	pbGeneral = new QPushButton(QStringLiteral("ͨ��"));
	pbChap1 = new QPushButton(QStringLiteral("��ɢ΢�ּ���"));

	QGridLayout* pLayout = new QGridLayout();
	pLayout->addWidget(pbGeneral, 0, 0);
	pLayout->addWidget(pbChap1, 1, 0);
	pLayout->setAlignment(Qt::AlignTop);
	this->setLayout(pLayout);
	connect(pbGeneral, &QPushButton::clicked, this, [=]() {this->changeWidget(0); });
	connect(pbChap1, &QPushButton::clicked, this, [=]() {this->changeWidget(1); });
}

ControlPanel::~ControlPanel()
{
}

GeneralWidget::GeneralWidget(QWidget* parent)
	: QWidget(parent)
{
	pbReturn = new QPushButton(QStringLiteral("����ѡ�����"));
	pbPrintInfo = new QPushButton(QStringLiteral("��ӡ������Ϣ"));
	QGridLayout* layout = new QGridLayout();
	layout->addWidget(pbReturn, 0, 0, 1, 1);
	layout->addWidget(pbPrintInfo, 1, 0, 1, 1);
	layout->setAlignment(Qt::AlignTop);
	this->setLayout(layout);

	connect(pbReturn, &QPushButton::clicked, this, [=]() {this->ReturnToControlPanel(); });
}

GeneralWidget::~GeneralWidget()
{
}
