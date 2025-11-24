#include "GPA434Lab3DESolver.h"
#include "QDEGeoOptimPanel.h"

GPA434Lab3DESolver::GPA434Lab3DESolver(QWidget *parent)
    : QMainWindow(parent)
    , mQDEControllerPanel()
    , mQDEEngineParametersPanel()
    , mQDEBestResultPanel()
    , mQDESolutionTabPanel()
    , mQDEHistoryChartPanel()
    {
    setWindowTitle("Differential Evolution Solver");
    setWindowIcon(QIcon(":/GPA434Lab3DESolver/dna-icon"));

    setupGUI();
    setupConnections();

}

GPA434Lab3DESolver::~GPA434Lab3DESolver()
{
}

void GPA434Lab3DESolver::setupGUI()
{
    QWidget* centralWidget = new QWidget;
    setCentralWidget(centralWidget);

    QHBoxLayout* mainLayout = new QHBoxLayout;
    centralWidget->setLayout(mainLayout);

    QWidget* leftWidget = new QWidget;
    QVBoxLayout* leftLayout = new QVBoxLayout;
    leftWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    leftWidget->setLayout(leftLayout);

    mQDEControllerPanel = new QDEControllerPanel(mDEAdapter);
    leftLayout->addWidget(mQDEControllerPanel);

    mQDEEngineParametersPanel = new QDEEngineParametersPanel(mDEAdapter);
    leftLayout->addWidget(mQDEEngineParametersPanel);

    mQDEBestResultPanel = new QDEBestResultPanel(mDEAdapter);
    leftLayout->addWidget(mQDEBestResultPanel);

    QSplitter* rightSplitter = new QSplitter;
    rightSplitter->setOrientation(Qt::Vertical);

    mQDESolutionTabPanel = new QDESolutionTabPanel(mDEAdapter);
    mQDESolutionTabPanel->addSolutionPanel(new QDEPeakPanel);
    mQDESolutionTabPanel->addSolutionPanel(new QDEOpenBoxPanel);
    mQDESolutionTabPanel->addSolutionPanel(new QDEGeoOptimPanel);
    mQDESolutionTabPanel->setCurrentIndex(0);
    rightSplitter->addWidget(mQDESolutionTabPanel);

    mQDEHistoryChartPanel = new QDEHistoryChartPanel(mDEAdapter);
    rightSplitter->addWidget(mQDEHistoryChartPanel);
    rightSplitter->setStretchFactor(0, 7); 
    rightSplitter->setStretchFactor(1, 3);

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
        mQDESolutionTabPanel,
        &QDESolutionTabPanel::updateVisualization);

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
