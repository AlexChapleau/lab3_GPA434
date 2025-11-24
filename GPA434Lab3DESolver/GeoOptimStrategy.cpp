#include "GeoOptimStrategy.h"
#include <Qtransform>
#include <format>

#include <OptimizationMaximization.h>
#include <FitnessIdentity.h>

const std::string GeoOptimStrategy::smTitle("Optimisation géométrique");
const std::string GeoOptimStrategy::smSummary(R".(
<p>
Le problème de l'optimisation géométrique consiste à trouver la transformationaffine permettant de
disposer la plus grande forme géométrique sur une surface parsemée d’obstacles.
</p>
).");

const std::string GeoOptimStrategy::smDescription(R".(
<p>
On cherche la transformation affine (translation, rotation, mise à l’échelle uniforme) qui permet
de placer le polygone le plus grand possible à l’intérieur d’un canevas rectangulaire contenant des
obstacles ponctuels. Le polygone ne doit jamais sortir du canevas ni recouvrir un obstacle, mais
peut toucher leurs frontières.
</p>
).");


GeoOptimStrategy::GeoOptimStrategy(QPolygonF polygon, double canvasWidth, double canvasHeight, std::vector<QPointF> obstacles)
	: SolutionStrategy(smTitle, smSummary, smDescription)
	, mCanvasWidth{ canvasWidth }
	, mCanvasHeight{ canvasHeight }
	, mObstacles{ obstacles }
	, mPolygon{ polygon }
{
	mSolutionDomain.resize(4);
	mSolutionDomain[0].set(-canvasWidth/2, canvasWidth/2);
	mSolutionDomain[1].set(-canvasHeight/2, canvasHeight/2);
	mSolutionDomain[2].set(-180.0, 180.0);
	mSolutionDomain[3].set(0.01, 10);

	setOptimizationStrategy(new de::OptimizationMaximization);
	setFitnessStrategy(new de::FitnessIdentity);
}

std::string GeoOptimStrategy::toString(de::Solution const& solution) const
{
	return std::format(R"...(
Transformations du polygon de base:
-Translation (x,y) : ({:0.6f}, {:0.6f})
-Rotation (degres) : {:0.6f}
-Mise à l'echelle  : {:0.6f}
-Aire : {:0.6f})...",solution[0], solution[1], solution[2], solution[3], solution.fitness());
}

double GeoOptimStrategy::process(de::Solution const& solution)
{
	if (!isValidSolution(solution)) {
		return 0.0;
	}
	double tx{ solution[0] };
	double ty{ solution[1] };
	double rotationDeg{ solution[2] };
	double scale{ solution[3] };

	QTransform transform;
	transform.translate(tx, ty);
	transform.rotate(rotationDeg);
	transform.scale(scale, scale);

	QPolygonF resultPolygon = transform.map(mPolygon);

	if (!isInsideCanvas(resultPolygon))
		return 0.0;

	if (isColliding(resultPolygon))
		return 0.0;

	return scale;
}

//utilise la formule shoelace
//https://www.101computing.net/the-shoelace-algorithm/
double GeoOptimStrategy::area(QPolygonF const& poly) const
{
	int n = poly.size();
	double sum = 0.0;
	const QPointF* p1{};
	const QPointF* p2{};

	for (int i = 0; i < n; ++i) {
		p1 = &poly[i];
		p2 = &poly[(i + 1) % n];
		sum += (p1->x() * p2->y()) - (p2->x() * p1->y());
	}

	return std::abs(sum) * 0.5;
}

bool GeoOptimStrategy::isInsideCanvas(QPolygonF const& poly) const
{
	QRectF bounds{ poly.boundingRect() };

	return bounds.left() >= 0.0 &&
		   bounds.top() >= 0.0 &&
		   bounds.right() <= mCanvasWidth &&
		   bounds.bottom() <= mCanvasHeight;
}

bool GeoOptimStrategy::isValidSolution(de::Solution const& solution) const
{
	int n = solution.size();
	for (int i = 0; i < n; ++i) {
		if (!mSolutionDomain[i].validate(solution[i])) {
			return false;
		}
	}
	return true;
}

bool GeoOptimStrategy::isColliding(QPolygonF const& poly) const
{
	for (const QPointF& obs : mObstacles)
	{
		if (poly.containsPoint(obs, Qt::WindingFill))
			return true;  
	}
	return false;
}