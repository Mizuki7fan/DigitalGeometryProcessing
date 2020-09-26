#pragma once

#include <QWidget>
#include <QtGui>
#include <QtWidgets>
#include <QStackedWidget>
#include "ChapterWidget.h"

class MeshParamWidget : public QWidget
{
	Q_OBJECT

public:
	MeshParamWidget(QWidget *parent = 0);
	~MeshParamWidget(void);
private:
signals:   
	void Gen_PrintInfoSignal();//General����
	void Ch1_ComputeCurvatureSignal();//��һ��
public slots:
	void SetWidgetVisible(int i);
	void SetControlPanelVisible();
private:
	QStackedWidget* m_pStackedWidget;
	QWidget* pChap1;
	
	QPushButton* pbReturn;
	QPushButton* pbGeneral;
	QPushButton* pbChap1;

	ControlPanel* wControlPanel;
	GeneralWidget* wGeneral;

};
