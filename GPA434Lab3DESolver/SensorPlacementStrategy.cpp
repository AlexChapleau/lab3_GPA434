#include "SensorPlacementStrategy.h"

#include <Qtransform>
#include <format>

#include "SweepSensor.h"
#include "CurtainSensor.h"
#include "CircleSensor.h"
#include <OptimizationMaximization.h>
#include <FitnessIdentity.h>


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
												 QVector<CircleObstacle> obstacles,
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
-Espace disponible couvert : {:0.2f} %
-Valeur Fitness : {:0.6f})...", solution.fitness()*100.0, solution.fitness());
}

double SensorPlacementStrategy::process(de::Solution const& solution)
{
	int i{};
	mGrid.reset();
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

		QTransform t;
		t.translate(x, y);

		double angle{};
		if (dynamic_cast<SweepSensor*>(sensor))
		{
			angle = solution[i++];
			t.rotate(angle);
		}

		for (const CircleObstacle& obs : mObstacles) {
			if (sensor->isCollidingObs(obs, t))
				return 0.0;
		}
		coveragePaths.push_back(sensor->buildCoverage(QPointF(x, y), angle, mObstacles, mCanvasWidth, mCanvasHeight));
		bodyPaths.push_back(t.map(sensor->bodyPath()));
	}

	for (int i{}; i < bodyPaths.size(); ++i) {
		for (int j{ i + 1 }; j < bodyPaths.size(); ++j) {
			if (Sensor::isCollidingSensor(bodyPaths[i], bodyPaths[j]))
				return 0.0;
		}
	}

	if (!isInsideCanvas(bodyPaths))
		return 0.0;

	qsizetype coveredCell{ computeCoverageArea(coveragePaths) };
	qsizetype totalCell{ mGrid.cellShape.size() };

	return static_cast<double>(coveredCell) / static_cast<double>(totalCell);
}

int SensorPlacementStrategy::dimensions() const
{
	int totalDim{};
	for (Sensor* sensor : mSensors)
		totalDim += sensor->degreesOfFreedom();
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
			mSolutionDomain[domainIndex++].set(0.0, 360.0); // rotation
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

void SensorPlacementStrategy::buildGrid()
{
	int rows = static_cast<int>(std::floor(mCanvasHeight / smCellSize));
	int cols = static_cast<int>(std::floor(mCanvasWidth / smCellSize));

	mGrid.cellShape.clear();
	mGrid.cellShape.reserve(rows * cols);

	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < cols; ++j)
		{
			QRectF rect(j * smCellSize,i * smCellSize,smCellSize,smCellSize);

			bool insideObstacle = false;

			for (const CircleObstacle& obs : mObstacles)
			{
				const QPointF c = obs.center();
				double r2 = obs.radius2();

				double closestX = std::clamp(c.x(), rect.left(), rect.right());
				double closestY = std::clamp(c.y(), rect.top(), rect.bottom());

				double dx = c.x() - closestX;
				double dy = c.y() - closestY;

				if (dx * dx + dy * dy <= r2)
				{
					insideObstacle = true;
					break;
				}
			}

			if (!insideObstacle)
				mGrid.cellShape.push_back(rect);
		}
	}

	mGrid.covered.resize(mGrid.cellShape.size(), false);
}

qsizetype SensorPlacementStrategy::computeCoverageArea(QVector<QPainterPath> const& coveragePaths)
{
	mGrid.reset();

	if (coveragePaths.isEmpty())
		return 0;

	QRectF pathsBR = coveragePaths[0].boundingRect();
	for (int i = 1; i < coveragePaths.size(); ++i)
		pathsBR = pathsBR.united(coveragePaths[i].boundingRect());

	for (int c = 0; c < mGrid.cellShape.size(); ++c)
	{
		const QRectF& rect = mGrid.cellShape[c];
		QPointF center = rect.center();

		if (!pathsBR.intersects(rect))
			continue;

		for (const QPainterPath& cov : coveragePaths)
		{
			if (!cov.boundingRect().intersects(rect))
				continue;

			if (cov.contains(center))
			{
				mGrid.covered[c] = true;
				break;
			}
		}
	}

	return std::count(mGrid.covered.begin(),mGrid.covered.end(), true);
}


//QPainterPath SensorPlacementStrategy::debugGridMask(QVector<QPainterPath> const& coveragePaths)
//{
//	int number = 0;
//	mGrid.reset();
//	QPainterPath mask;
//
//	if (coveragePaths.isEmpty())
//		return mask;
//
//	const double cellArea = smCellSize * smCellSize;
//
//	QRectF globalBBox = coveragePaths[0].boundingRect();
//	for (int k = 1; k < coveragePaths.size(); ++k)
//		globalBBox = globalBBox.united(coveragePaths[k].boundingRect());
//
//	for (int c = 0; c < mGrid.cellShape.size(); ++c)
//	{
//		const QPainterPath& cellPath = mGrid.cellShape[c];
//		const QRectF rect = cellPath.boundingRect();
//		const QPointF center = rect.center();
//
//		if (!globalBBox.intersects(rect))
//			continue;
//
//		for (const QPainterPath& cov : coveragePaths)
//		{
//			if (!cov.boundingRect().intersects(rect))
//				continue;
//
//			if (cov.contains(center))
//			{
//				mGrid.covered[c] = true;
//				mask.addPath(cellPath);  
//				number++;
//				break;
//			}
//		}
//	}
//	return mask;
//}