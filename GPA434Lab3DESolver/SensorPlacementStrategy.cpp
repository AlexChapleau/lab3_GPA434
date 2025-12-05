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


SensorPlacementStrategy::SensorPlacementStrategy(QVector<Sensor*> sensors,
												 QVector<QPointF> obstacles,
												 double obstaclesRadius,
												 double canvasWidth,
												 double canvasHeight)
	: SolutionStrategy(smTitle, smSummary, smDescription)
	, mSensors{ sensors }
	, mObstacles{ obstacles }
	, mObstaclesRadius{ obstaclesRadius }
	, mCanvasWidth{ canvasWidth }
	, mCanvasHeight{ canvasHeight }
{
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

	QVector<QPainterPath> coveragePaths;
	coveragePaths.reserve(mSensors.size());

	QVector<QPainterPath> bodyPaths;
	bodyPaths.reserve(mSensors.size());
	
	for (Sensor* s : mSensors)
	{
		double x{ solution[i++] };
		double y{ solution[i++] };
		double sensorRange{ s->parameters()[0].value };

		QTransform T;
		T.translate(x, y);

		double sensorAngle{};
		if (Sensor* sweep = dynamic_cast<SweepSensor*>(s))
		{
			sensorAngle = s->parameters()[1].value;
			double angle = solution[i++];
			T.rotate(angle);
		}

		QPainterPath body = T.map(s->bodyPath());

		bodyPaths.push_back(body);
	}

	if (!isInsideCanvas(bodyPaths))
		return 0.0;
	

	return 0.0;
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




//const int gridStep = 10;                     // réglable pour qualité/temps
//double coveredCount = 0;
//double totalPoints = 0;
//
//// --- 3) Scan de grille ---
//for (double x = 0; x < mCanvasWidth; x += gridStep)
//	for (double y = 0; y < mCanvasHeight; y += gridStep)
//	{
//		QPointF p(x, y);
//		totalPoints++;
//
//		bool visible = false;
//		for (const QPainterPath& c : coveragePaths)
//		{
//			if (c.contains(p))
//			{
//				visible = true;
//				break;
//			}
//		}
//
//		if (visible)
//			coveredCount++;
//	}
//
//// --- 4) Score (maximiser couverture → minimiser (1-couv)) ---
//double coverageRatio = coveredCount / totalPoints;
//return 1.0 - coverageRatio;