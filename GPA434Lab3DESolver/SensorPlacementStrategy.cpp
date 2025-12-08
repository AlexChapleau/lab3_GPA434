#include "SensorPlacementStrategy.h"

#include <Qtransform>
#include <format>

#include "SweepSensor.h"
#include "CurtainSensor.h"
#include "CircleSensor.h"
#include <OptimizationMaximization.h>
#include <FitnessIdentity.h>
#include "SensorCoverageUtils.h"


const std::string SensorPlacementStrategy::smTitle("Disposition de capteurs");
const std::string SensorPlacementStrategy::smSummary(R".(
<p>Le problème de la disposition de capteurs consiste à trouver les positions de différents
 capteurs pour maximiser leur aire couverte.
</p>
).");

const std::string SensorPlacementStrategy::smDescription(R".(
<p>
On cherche la transformation affine (translation, rotation) de chaque capteur qui permet
de couvrir la plus grande zone possible à l’intérieur d’un canevas rectangulaire contenant des
obstacles circulaire. Les zones couvertes par plusieurs capteurs sont compté une seule fois et les
obstacles.  Les capteurs ont un champ de vision donc les obstacles bloquent la vue (les capteurs
ne voient pas derrière les obstacles).
</p>
).");


const double SensorPlacementStrategy::smCellSize{ 10.0 };

SensorPlacementStrategy::SensorPlacementStrategy(QVector<Sensor*> sensors,
												 QVector<Obstacle> obstacles,
												 double canvasWidth,
												 double canvasHeight)
	: SolutionStrategy(smTitle, smSummary, smDescription)
	, mSensors{ sensors }
	, mObstacles{ obstacles }
	, mCanvasWidth{ canvasWidth }
	, mCanvasHeight{ canvasHeight }
{
	buildGrid();
	configParams();
}

std::string SensorPlacementStrategy::toString(de::Solution const& solution) const
{
	return std::format(R"...(

-Valeur Fitness : {:0.6f})...", solution.fitness());
}

double SensorPlacementStrategy::process(de::Solution const& solution)
{
	size_t i{};

	QVector<QPainterPath> bodyPaths;
	QVector<QPainterPath> coveragePaths;
	
	bodyPaths.reserve(mSensors.size());
	
	for (Sensor* sensor : mSensors)
	{
		if (!sensor) //retirer un warning VisualStudio pour un nullptr possible
			continue;

		double x{ solution[i++] };
		double y{ solution[i++] };
		double sensorRange{ sensor->parameters()[0].value };

		QTransform T;
		T.translate(x, y);

		double sensorAngle{};
		if (dynamic_cast<SweepSensor*>(sensor))
		{
			sensorAngle = sensor->parameters()[1].value;
			double angle = solution[i++];
			T.rotate(angle);
		}

		bodyPaths.push_back(T.map(sensor->bodyPath()));

		QPainterPath covPath = SensorCoverageUtils::buildCoverageForSensor(sensor, QPointF(x, y),
																		   sensorAngle,mObstacles,
																		   mCanvasWidth, mCanvasHeight);
		QPainterPath canvas;
		canvas.addRect(0, 0, mCanvasWidth, mCanvasHeight);
		covPath = covPath.intersected(canvas);
		coveragePaths.push_back(covPath);
	}

	if (!isInsideCanvas(bodyPaths))
		return 0.0;

	if (isColliding(bodyPaths))
		return 0.0;

	double coveredArea{ computeCoverageArea(coveragePaths) };
	double totalArea{ mCanvasHeight * mCanvasWidth };

	return coveredArea/ totalArea;
}

int SensorPlacementStrategy::dimensions() const
{
	int totalDim{};
	for (Sensor* sensor : mSensors) 
		totalDim += (dynamic_cast<SweepSensor*>(sensor)) ? 3 : 2;
	return totalDim;
}

void SensorPlacementStrategy::configParams()
{
	mSolutionDomain.resize(dimensions());
	int domainIndex{};

	for (Sensor* sensor : mSensors) {

		mSolutionDomain[domainIndex++].set(0, mCanvasWidth); // position X
		mSolutionDomain[domainIndex++].set(0, mCanvasHeight); // position Y

		if (dynamic_cast<SweepSensor*>(sensor)) {
			mSolutionDomain[domainIndex++].set(-180.0, 180.0); // rotation
		}
	}
	setOptimizationStrategy(new de::OptimizationMaximization);
	setFitnessStrategy(new de::FitnessIdentity);
}

bool SensorPlacementStrategy::isInsideCanvas(QVector<QPainterPath> const& bodyPaths) const
{
	for (const QPainterPath& body : bodyPaths) {
		QRectF bounds{ body.boundingRect() };
		if (bounds.left() < 0.0 || bounds.top() < 0.0 ||
			bounds.right() > mCanvasWidth || bounds.bottom() > mCanvasHeight) {
			return false;
		}
	}
	return true;
}

bool SensorPlacementStrategy::isColliding(QVector<QPainterPath> const& bodyPaths) const
{
	for (const Obstacle& obs : mObstacles) {

		const QPainterPath& shape{ obs.shape };

		for (const QPainterPath& body : bodyPaths) {
			if (!body.intersected(shape).isEmpty())
				return true;
		}
	}

	for (int i{}; i < bodyPaths.size(); ++i) {
		for (int j{ i + 1 }; j < bodyPaths.size(); ++j) {
			if (!bodyPaths[i].intersected(bodyPaths[j]).isEmpty())
				return true;
		}
	}
	return false;
}

void SensorPlacementStrategy::buildGrid()
{
	int rows = std::ceil(mCanvasHeight / smCellSize);
	int cols = std::ceil(mCanvasWidth / smCellSize);

	mGrid.cellShape.reserve(rows * cols);

	for (int i{}; i < rows; i++) {
		for (int j{}; j < cols; j++) {

			QRectF rect(j * smCellSize, i * smCellSize, smCellSize, smCellSize);

			QPainterPath cellPath;
			cellPath.addRect(rect);

			bool isFullyInsideObstacle{ false };
			for (const Obstacle& obs : mObstacles) {

				const QPainterPath& shape{ obs.shape };

				if (shape.boundingRect().intersects(rect)) { //beaucoup plus rapide que contains
					if (shape.contains(cellPath))
					{
						isFullyInsideObstacle = true;
						break;
					}
				}
			}

			if (!isFullyInsideObstacle) 
				mGrid.cellShape.push_back(cellPath);
		}
	}

	mGrid.covered.resize(mGrid.cellShape.size(), false);
}

double SensorPlacementStrategy::computeCoverageArea(QVector<QPainterPath> const& coveragePaths)
{
	mGrid.reset();

	for (int c = 0; c < mGrid.cellShape.size(); ++c)
	{
		const QPainterPath& cell = mGrid.cellShape[c];

		if (mGrid.covered[c]) continue;

		for (const QPainterPath& cov : coveragePaths)
		{
			if (!cov.boundingRect().intersects(cell.boundingRect()))
				continue;
			if (cov.contains(cell.boundingRect().center())) {
				mGrid.covered[c] = true;
				break;
			}
			//if (!cov.intersected(cell).isEmpty()) 
			//{
			//	mGrid.covered[c] = true;
			//	break;
			//}
		}
	}

	int coveredCount = std::count(mGrid.covered.begin(), mGrid.covered.end(), true);
	double areaCovered = coveredCount * (smCellSize * smCellSize);

	return areaCovered; 
}


QPainterPath SensorPlacementStrategy::debugGridMask(QVector<QPainterPath> const& coveragePaths)
{
	QPainterPath debug;

	mGrid.reset();

	for (int c = 0; c < mGrid.cellShape.size(); ++c)
	{
		const QPainterPath& cell = mGrid.cellShape[c];

		for (const QPainterPath& cov : coveragePaths)
		{
			if (cov.boundingRect().intersects(cell.boundingRect()) &&
				!cov.intersected(cell).isEmpty())
			{
				mGrid.covered[c] = true;
				debug.addPath(cell);          // <-- visualisation directe
				break;
			}
		}
	}

	return debug;  // un path contenant chaque cellule colorée
}