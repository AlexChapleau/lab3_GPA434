#include "GPA434Lab3DESolver.h"


GPA434Lab3DESolver::GPA434Lab3DESolver(QWidget *parent)
    : QMainWindow(parent)
    , mQDEControllerPanel()
    , mQDEEngineParametersPanel()
    , mQDEBestResultPanel()
    , mQDESolutionTabPanel()
    , mQDEPeakPanel()
    , mQDEOpenBoxPanel()
    , mQDEHistoryChartPanel()
    {
    setWindowTitle("Differential Evolution Solver");
    setWindowIcon(QIcon(":/GPA434Lab3DESolver/dna-icon"));

    setupUi();
    setupConnections();

}

GPA434Lab3DESolver::~GPA434Lab3DESolver()
{
}

void GPA434Lab3DESolver::setupUi()
{
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
    QWidget* leftWidget = new QWidget(centralWidget);
    QVBoxLayout* leftLayout = new QVBoxLayout(leftWidget);
    QSplitter* rightSplitter = new QSplitter(Qt::Vertical, centralWidget);

    mQDEControllerPanel = new QDEControllerPanel(mDEAdapter, leftWidget);
    leftLayout->addWidget(mQDEControllerPanel);

    mQDEEngineParametersPanel = new QDEEngineParametersPanel(mDEAdapter, leftWidget);
    leftLayout->addWidget(mQDEEngineParametersPanel);

    mQDEBestResultPanel = new QDEBestResultPanel(mDEAdapter, leftWidget);
    leftLayout->addWidget(mQDEBestResultPanel);

    mQDESolutionTabPanel = new QDESolutionTabPanel(mDEAdapter, rightSplitter);
    mQDEPeakPanel = new QDEPeakPanel(rightSplitter);
    mQDEOpenBoxPanel = new QDEOpenBoxPanel(rightSplitter);
    mQDEHistoryChartPanel = new QDEHistoryChartPanel(mDEAdapter, rightSplitter);
    mQDESolutionTabPanel->addSolutionPanel(mQDEPeakPanel);
    mQDESolutionTabPanel->addSolutionPanel(mQDEOpenBoxPanel);
    mQDESolutionTabPanel->setCurrentIndex(0);
    rightSplitter->addWidget(mQDEHistoryChartPanel);

    mainLayout->addWidget(leftWidget);
    mainLayout->addWidget(rightSplitter);

}

void GPA434Lab3DESolver::setupConnections()
{
    connect(mQDEEngineParametersPanel,
        &QDEEngineParametersPanel::parameterChanged,
        mQDEControllerPanel, &QDEControllerPanel::resetSimulation);

    connect(mQDEEngineParametersPanel,
        &QDEEngineParametersPanel::parameterChanged,
        [this]() {
            mQDESolutionTabPanel->updateVisualization();
        });

    connect(mQDESolutionTabPanel,
        &QDESolutionTabPanel::solutionChanged,
        mQDEEngineParametersPanel,
        &QDEEngineParametersPanel::setParametersFromSolution);

    connect(mQDESolutionTabPanel,
        &QDESolutionTabPanel::solutionChanged,
        mQDEControllerPanel,
        &QDEControllerPanel::resetSimulation);

    connect(mQDEControllerPanel,
        &QDEControllerPanel::evolutionStarted,
        mQDEEngineParametersPanel,
        &QDEEngineParametersPanel::disable);

    connect(mQDEControllerPanel,
        &QDEControllerPanel::evolutionStopped,
        mQDEEngineParametersPanel,
        &QDEEngineParametersPanel::enable);

    connect(mQDEControllerPanel,
        &QDEControllerPanel::evolutionStarted,
        mQDESolutionTabPanel,
        &QDESolutionTabPanel::disable);

    connect(mQDEControllerPanel,
        &QDEControllerPanel::evolutionStopped,
        mQDESolutionTabPanel,
        &QDESolutionTabPanel::enable);
}
