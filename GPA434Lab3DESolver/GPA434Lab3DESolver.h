#pragma once

#include <QtWidgets/QMainWindow>
#include <QSplitter>
#include <QVBoxLayout>
#include <QLabel>
#include <QTabWidget>

#include "QDEControllerPanel.h"
#include "QDEEngineParametersPanel.h"
#include "QDEBestResultPanel.h"
#include "QDESolutionTabPanel.h"
#include "QDEPeakPanel.h"
#include "QDEOpenBoxPanel.h"
#include "QDEHistoryChartPanel.h"
#include "QDEAdapter.h"

class GPA434Lab3DESolver : public QMainWindow
{
    Q_OBJECT

public:
    GPA434Lab3DESolver(QWidget *parent = nullptr);
    ~GPA434Lab3DESolver() override;

private:
    void setupUi();
    void setupConnections();

private:
    QDEAdapter mDEAdapter;                
    QDEControllerPanel *mQDEControllerPanel;
    QDEEngineParametersPanel *mQDEEngineParametersPanel;
    QDEBestResultPanel *mQDEBestResultPanel;
    QDESolutionTabPanel *mQDESolutionTabPanel;
    QDEPeakPanel *mQDEPeakPanel;
    QDEOpenBoxPanel *mQDEOpenBoxPanel;
    QDEHistoryChartPanel *mQDEHistoryChartPanel;
};
