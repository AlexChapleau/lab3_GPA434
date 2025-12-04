#include "SensorPlacementStrategy.h"

#include <format>

#include "SweepSensor.h"
#include "CurtainSensor.h"
#include "CircleSensor.h"


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
	, mCatalog{ buildCatalog() }
	, mObstaclesRadius{ obstaclesRadius }
	, mCanvasWidth{ canvasWidth }
	, mCanvasHeight{ canvasHeight }
{

	mSolutionDomain.resize(dimensions());
	int domainIndex{};

	for (Sensor * sensor : mSensors) {

		mSolutionDomain[domainIndex].set(0, canvasWidth); // position X
		mSolutionDomain[domainIndex + 1].set(0, canvasHeight); // position Y

		if (dynamic_cast<SweepSensor*>(sensor)) {
			mSolutionDomain[domainIndex + 2].set(-180.0, 180.0); // rotation
			domainIndex += 3;
		}
		else
			domainIndex += 2;
		
	}
}

std::string SensorPlacementStrategy::toString(de::Solution const& solution) const
{
	return std::string();
}

double SensorPlacementStrategy::process(de::Solution const& solution)
{
	return 0.0;
}

QVector<SensorPlacementStrategy::SensorEntry> SensorPlacementStrategy::buildCatalog() const
{
	QVector<SensorEntry> catalog;
	catalog.reserve(mSensors.size());

	for (Sensor* sensor : mSensors) {
		SensorEntry entry;
		entry.sensor = sensor;
		entry.dimCount = (dynamic_cast<SweepSensor*>(sensor)) ? 3 : 2;
		catalog.push_back(entry);
	}
	return catalog;
}

int SensorPlacementStrategy::dimensions() const
{
	int totalDim{};
	for (SensorEntry entry : mCatalog)
		totalDim += entry.dimCount;
	return totalDim;
}


