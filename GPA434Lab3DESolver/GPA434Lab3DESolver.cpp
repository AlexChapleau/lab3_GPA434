#include "GPA434Lab3DESolver.h"


GPA434Lab3DESolver::GPA434Lab3DESolver(QWidget *parent)
    : QMainWindow(parent)
    , mCentralWidget{}
    , mMainLayout{}
    , mLeftPanelWidget{}
    , mLeftPanelLayout{}
    , mRightSplitter{}
    {
    setWindowTitle("Differential Evolution Solver");
    setWindowIcon(QIcon(":/GPA434Lab3DESolver/dna-icon"));

    
    mCentralWidget = new QWidget(this);
    setCentralWidget(mCentralWidget);

    mMainLayout = new QHBoxLayout(mCentralWidget);
    mLeftPanelWidget = new QWidget(mCentralWidget);
    mLeftPanelLayout = new QVBoxLayout(mLeftPanelWidget);

}

GPA434Lab3DESolver::~GPA434Lab3DESolver()
{
}
