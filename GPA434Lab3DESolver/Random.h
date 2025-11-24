#pragma once


#include <random>



//! \class Random
//! 
//! \brief Classe utilitaire générant diverses valeurs aléatoires.
//!
//! \details Cette classe fournit plusieurs fonctions statiques utilitaires 
//! générant différents types de nombres aléatoires. 
//! 
//! Elle ne peut pas être instanciée ou copiée.
class Random
{
public:
    //! \brief Constructeur par défaut supprimé.
    //! \details Empêche la création d'une instance de cette classe.
    Random() = delete;
    //! \brief Constructeur de copie supprimé.
    //! \details Empêche la copie d'une instance de cette classe.
    Random(Random const&) = delete;
    //! \brief Opérateur d'affectation supprimé.
    //! \details Empêche la copie d'une instance de cette classe.
    Random& operator=(Random const&) = delete;
    //! \brief Destructeur supprimé.
    //! \details Empêche la destruction directe d'une instance.
    ~Random() = delete;
    //!
    //!
    //! \brief Évalue si un événement aléatoire se produit.
    //! \param probability Probabilité de l'événement (par défaut, 50%).
    //! \returns Vrai si l'événement se produit, sinon faux.
    static bool event(double probability = 0.5);
    //!
    //!
    //! \brief Génère un entier aléatoire.
    //! \details La valeur entre [0, max_int] où max_int est la valeur 
    //! maximum possible pour le type int.
    //! \returns Un entier aléatoire.
    static int integer();
    //!
    //!
    //! \brief Génère un entier aléatoire jusqu'à un maximum.
    //! \param maximum La limite supérieure pour la valeur générée.
    //! \returns Un entier aléatoire.
    static int integer(int maximum);
    //!
    //!
    //! \brief Génère un entier aléatoire entre un minimum et un maximum.
    //! \param minimum La limite inférieure.
    //! \param maximum La limite supérieure.
    //! \returns Un entier aléatoire entre les limites spécifiées.
    static int integer(int minimum, int maximum);
    //!
    //!
    //! \brief Génère un nombre entier non signé aléatoire.
    //! \details La valeur entre [0, max_size_t] où max_size_t est la valeur 
    //! maximum possible pour le type size_t.
    //! \returns Un nombre entier non signé aléatoire.
    static size_t size();
    //!
    //!
    //! \brief Génère un nombre entier non signé aléatoire jusqu'à un maximum.
    //! \param maximum La limite supérieure pour la valeur générée.
    //! \returns Un nombre entier non signé aléatoire.
    static size_t size(size_t maximum);
    //!
    //!
    //! \brief Génère un nombre entier non signé aléatoire entre un minimum et un maximum.
    //! \param minimum La limite inférieure.
    //! \param maximum La limite supérieure.
    //! \returns Un nombre entier non signé aléatoire entre les limites spécifiées.
    static size_t size(size_t minimum, size_t maximum);
    //!
    //!
    //! \brief Génère un réel aléatoire.
    //! \details Retourne une valeur normalisée entre [0, 1].
    //! \returns Un réel aléatoire.
    static double real();
    //!
    //!
    //! \brief Génère un réel aléatoire jusqu'à un maximum.
    //! \param maximum La limite supérieure pour la valeur générée.
    //! \returns Un réel aléatoire.
    static double real(double maximum);
    //!
    //!
    //! \brief Génère un réel aléatoire entre un minimum et un maximum.
    //! \param minimum La limite inférieure.
    //! \param maximum La limite supérieure.
    //! \returns Un réel aléatoire entre les limites spécifiées.
    static double real(double minimum, double maximum);

private:
    static std::default_random_engine smEngine; //!< \brief Moteur pour la génération aléatoire.
};

