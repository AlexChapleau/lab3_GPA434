#include "GeoOptimStrategy.h"

const std::string GeoOptimStrategy::smTitle("Optimisation géométrique");
const std::string GeoOptimStrategy::smSummary("<p>Le problème de l'optimisation géométrique consiste à trouver la transformation affine permettant de disposer la plus grande forme géométrique sur une surface parsemée d’obstacles.</p>");
const std::string GeoOptimStrategy::smDescription(R".(<p>Ce problème se résoud dans un espace à une dimension, représantant la taille de la coupe 'cut' au quatre coins de la surface rectangulaire.</p>

<p>Le domaine de cette dimension correspond aux tailles possible de la coupe. Elle doit être comprise entre 0 et la moitié de la plus petite dimension de la surface rectangulaire. Théoriquement, les bornes devraient être excluses c = ]0, min(width, height) / 2[.</p>

<p>La fonction objective est le volume de la boîte issue de la coupe.<br/>
f(cut) = (width - 2 * cut) * (height - 2 * cut) * cut</p>).");

