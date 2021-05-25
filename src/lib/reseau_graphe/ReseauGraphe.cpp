/**
 * @file    ReseauGraphe.cpp
 * @brief   Vous trouverez ici toutes les fonctions implementees pour la classe ReseauGraphe.
 * @author  Florian CAMBRESY
 * @author  Gabriel Dos Santos
 * @author  Mickael Le Denmat
 * @date    Mai 2021
 **/

// #include <omp.h>
#include <limits>
#include <cstdlib>
#include <cstdint>
#include <algorithm>

#include "ReseauGraphe.hpp"

std::vector<Machine*> ReseauGraphe::m_Machines;
std::vector<Liaison> ReseauGraphe::m_Liaisons;

/**
 * @brief Constructeur de la classe ReseauGraphe.
 *
 **/
ReseauGraphe::ReseauGraphe() {
    m_Machines.clear();
    m_Liaisons.clear();
}

/**
 * @brief Destructeur de la classe ReseauGraphe.
 *
 **/
ReseauGraphe::~ReseauGraphe() {}

void ReseauGraphe::remettreIdAZero() {
    Machine* m = dynamic_cast<Machine*> (m_Machines[0]);
    m->remettreIdAZero();
    
    for (Machine* m : m_Machines) {
        if (Ordinateur* o = dynamic_cast<Ordinateur*> (m)) {
            o->remettreIdAZero();
        } else if (Routeur* r = dynamic_cast<Routeur*> (m)) {
            r->remettreIdAZero();
        } else if (Commutateur* c = dynamic_cast<Commutateur*> (m)) {
            c->remettreIdAZero();
        }
    }
}

/**
 * @brief Setter pour l'argument nom.
 *
 * @return void.
 **/
void ReseauGraphe::setNom(const std::string nom) {
    m_Nom = nom;
}

/**
 * @brief Getter pour l'argument m_nom.
 *
 * @return std::string.
 **/
const std::string& ReseauGraphe::getNom() const {
    return m_Nom;
}

uint16_t ReseauGraphe::getIdRouteurDepuisIdMachine(const uint16_t& idMachine) {
    if (m_Machines[idMachine - 1]->getIdMachine() == idMachine) {
        Routeur* r = dynamic_cast<Routeur*>(m_Machines[idMachine - 1]);

        if (r != nullptr) {
            return r->getIdRouteur();
        }
    }

    return 0;
}


Routeur* ReseauGraphe::getPtrRouteur(const uint16_t& idRouteur) {
    for (Machine* m: m_Machines) {
        Routeur* r= dynamic_cast<Routeur*>(m);

        if (r != nullptr && r->getIdRouteur() == idRouteur) {
            return r;
        }
    }

    std::cout << "ERREUR : fichier `ReseauGraphe.cpp`\n"
        << "\tmethode `getPtrRouteur` : #"
        << idRouteur << " introuvable sur le reseau.\n";
    return nullptr;
}

Machine* ReseauGraphe::getMachine(const IPv4& ip) {
   for (Machine* m : m_Machines) {
       if (m->getIp() == ip) {
           return m;
       }
   }

    std::cout << "ERREUR : Dans le fichier 'ReseauGraphe.cpp'. "
        << "Dans la fonction 'getMachine(ip)'. "
        << "Adresse IP introuvable sur le reseau.\n";
    exit(EXIT_FAILURE);
}


Machine* ReseauGraphe::getMachine(const uint16_t& id) {
    if (id > m_Machines.size()) {
        std::cout << "ERREUR : Dans le fichier 'ReseauGraphe.cpp'. "
            << "Dans la fonction 'getMachine(id)'. "
            << "Indice en dehors du tableau.\n";
        exit(EXIT_FAILURE);
    }

    return m_Machines[id];
}

IPv4 ReseauGraphe::getSousReseau(const IPv4& ipMachine) {
    for (Machine* iter : m_Machines) {
        if (iter->getIp() == ipMachine) {
            return iter->getSousReseaux()[0];
        }
    }

    std::cout << "ERREUR : Dans le fichier 'ReseauGraphe.cpp'. "
        << "Dans la fonction 'getSousReseau'. "
        << "Aucune ipMachine trouvee.\n";
    exit(EXIT_FAILURE);
}

/**
 * @brief Getter pour l'argument m_Machines.
 *
 * @return std::vector<Machine*>.
 **/
const std::vector<Machine*>& ReseauGraphe::getMachines() const {
    return m_Machines;
}

/**
 * @brief Getter pour l'argument m_Liaisons.
 *
 * @return std::vector<Liaison>.
 **/
const std::vector<Liaison>& ReseauGraphe::getLiaisons() const {
    return m_Liaisons;
}

const std::vector<uint16_t> ReseauGraphe::getIdsRouteurs() {
    std::vector<uint16_t> idsRouteurs;

    for (Machine* iter: m_Machines) {
        Routeur* r = dynamic_cast<Routeur*>(iter);

        if (r) {
            idsRouteurs.push_back(r->getIdRouteur());
        }
    }

    return idsRouteurs;
}

const std::map<uint32_t, double> ReseauGraphe::getTempsPaquet() const {
    //
    std::map<uint32_t, double> tempsPaquet;

    //
    for (Machine* m : m_Machines) {
        auto tempsTraitementCopie = m->getTempsTraitementPaquets();
        for (auto elt : tempsTraitementCopie) {
            auto trouve = tempsPaquet.find(elt.first);
            if (trouve != tempsPaquet.end()) {
                tempsPaquet[elt.first] += elt.second;
            } else {
                tempsPaquet[elt.first] = elt.second;
            }
        }
    }

    //
    return tempsPaquet;
}

// Methodes
bool ReseauGraphe::estRouteur(const uint16_t& idMachine) {
    return (dynamic_cast<Routeur*>(m_Machines[idMachine - 1])) ? true : false;
}

/**
 * @brief Indique si un graphe est connexe ou non.
 *
 * @return bool.
 **/
bool ReseauGraphe::estConnexe() {
    return true;
}

/**
 * @brief Ajoute une machine dans le tableau de machines.
 *
 * @return void.
 **/
void ReseauGraphe::ajouter(Machine* m) {
    m_Machines.emplace_back(m);
}

/**
 * @brief Ajoute une liaison dans le tableau de liaisons.
 *
 * @return void.
 **/
void ReseauGraphe::ajouter(Liaison l) {
    m_Liaisons.emplace_back(l);
}

uint16_t getIdRouteurPlusProche(std::vector<double>& sommeMetrique,
                                std::vector<uint16_t>& nonVisites,
                                size_t& routeursVus)
{
    double metriqueMinimale = std::numeric_limits<double>::max();
    uint16_t idRouteurPlusProche = 0;

    for (size_t i = 0; i < nonVisites.size(); ++i) {
        if (sommeMetrique[nonVisites[i] - 1] < metriqueMinimale) {
            metriqueMinimale = sommeMetrique[nonVisites[i] - 1];
            idRouteurPlusProche = nonVisites[i];
        }
    }

    for (size_t i = 0; i < nonVisites.size(); ++i) {
        if (nonVisites[i] == idRouteurPlusProche) {
            nonVisites.erase(nonVisites.begin() + i);
        }
    }
    routeursVus++;

    return idRouteurPlusProche;
}

std::vector<Liaison> ReseauGraphe::getCheminsVoisins(const uint16_t& courant) {
    std::vector<Liaison> cheminsVoisins;

    for (Liaison chemin: m_Liaisons) {
        if (estRouteur(chemin.m_NumMachine1) && estRouteur(chemin.m_NumMachine2)) {
            uint16_t r1 = getIdRouteurDepuisIdMachine(chemin.m_NumMachine1);
            uint16_t r2 = getIdRouteurDepuisIdMachine(chemin.m_NumMachine2);

            if (r1 == courant || r2 == courant) {
                cheminsVoisins.push_back(chemin);
            }
        }
    }

    return cheminsVoisins;
}

void ReseauGraphe::getPlusCourtChemin(const uint16_t& depart,
                                      const uint16_t& arrivee,
                                      std::vector<uint16_t>& peres,
                                      std::vector<Liaison*>& plusCourtChemin)
{
    uint16_t courant = arrivee;

    while (courant != depart) {
        uint16_t suivant = peres[courant - 1];

        uint16_t idMachine1 = getPtrRouteur(courant)->getIdMachine();
        uint16_t idMachine2 = getPtrRouteur(suivant)->getIdMachine();

        for (Liaison chemin: m_Liaisons) {
            if ((chemin.m_NumMachine1 == idMachine1 && chemin.m_NumMachine2 == idMachine2) ||
                (chemin.m_NumMachine1 == idMachine2 && chemin.m_NumMachine2 == idMachine1))
            {
                Liaison* c = new Liaison;
                c->m_NumMachine1 = chemin.m_NumMachine1;
                c->m_NumMachine2 = chemin.m_NumMachine2;
                c->m_Debit = chemin.m_Debit;
                plusCourtChemin.push_back(c);
            }
        }

        courant = suivant;
    }

    std::reverse(plusCourtChemin.begin(), plusCourtChemin.end());
}

std::vector<Liaison*> ReseauGraphe::routageDynamique(const uint16_t& depart,
                                                     const uint16_t& arrivee)
{
    std::vector<uint16_t> nonVisites = getIdsRouteurs();
    std::vector<double> sommeMetrique(nonVisites.size(), std::numeric_limits<double>::max());
    std::vector<uint16_t> peres(nonVisites.size(), UINT16_MAX);
    std::vector<Liaison*> plusCourtChemin;
    size_t nbRouteur = nonVisites.size(), routeursVus = 0;
    double debitReference = 100.0;
    sommeMetrique[depart - 1] = 0.0;

    while (routeursVus < nbRouteur) {
        uint16_t courant = getIdRouteurPlusProche(sommeMetrique, nonVisites, routeursVus);
        std::vector<Liaison> cheminsVoisins = getCheminsVoisins(courant);

        for (Liaison suivante: cheminsVoisins) {
            uint16_t r1 = getIdRouteurDepuisIdMachine(suivante.m_NumMachine1);
            uint16_t r2 = getIdRouteurDepuisIdMachine(suivante.m_NumMachine2);
            double debit = (double)(suivante.m_Debit);

            double debitAncienR2 = sommeMetrique[r2 - 1];
            double debitAncienR1 = sommeMetrique[r1 - 1];
            double debitNouveau = sommeMetrique[courant - 1] + (debitReference / debit);

            if (r1 == courant) {
                if (debitAncienR2 > debitNouveau) {
                    sommeMetrique[r2 - 1] = debitNouveau;
                    peres[r2 - 1] = courant;
                }
            } else if (r2 == courant) {
                if (debitAncienR1 > debitNouveau) {
                    sommeMetrique[r1 - 1] = debitNouveau;
                    peres[r1 - 1] = courant;
                }
            }
        }
    }

    getPlusCourtChemin(depart, arrivee, peres, plusCourtChemin);

    return plusCourtChemin;
}

void ReseauGraphe::lancerOSPF() {
    for (size_t i = 0; i < m_Machines.size(); ++i) {
        Routeur* routeur = dynamic_cast<Routeur*>(m_Machines[i]);

        if (routeur) {
            std::vector<Machine*> voisins = routeur->getVoisins();

            // #pragma omp parallel for
            for (size_t j = 0; j < voisins.size(); ++j) {
                Routeur* routeurVoisin = dynamic_cast<Routeur*>(voisins[j]);

                if (routeurVoisin && j < 1) {
                    PaquetOSPF* hello = new PaquetHello(routeurVoisin->getIdRouteur());
                    hello->setEntete(Hello, routeur->getIdRouteur());
                    routeur->envoyerOSPF(routeurVoisin, hello);
                }
            }            
        }
    }

    for (size_t i = 0; i < m_Machines.size(); ++i) {
        Routeur* r1 = dynamic_cast<Routeur*>(m_Machines[i]);

        if (r1) {
            for (size_t j = 0; j < m_Machines.size(); ++j) {
                Routeur* r2 = dynamic_cast<Routeur*>(m_Machines[j]);

                if (r2) {
                    if (r1 != r2) {
                        std::map<Routeur*, std::vector<Liaison*>>& table = r1->getTableRoutage();
                        table[r2] = ReseauGraphe::routageDynamique(
                            r1->getIdRouteur(),
                            r2->getIdRouteur()
                        );
                    }
                }
            }
        }
    }
}

// Overloading
std::ostream& operator<<(std::ostream& flux, const ReseauGraphe& reseau) {
    flux << "Nom du réseau : " << reseau.getNom() << "\n";

    // Affichage de la liste des machines.
    for (Machine* m : reseau.getMachines()) {
        if (Routeur* r = dynamic_cast<Routeur*> (m)) {
            flux << *r << "\n";
        } else if (Commutateur* c = dynamic_cast<Commutateur*> (m)) {
            flux << *c << "\n";
        } else if (Ordinateur* o = dynamic_cast<Ordinateur*> (m)) {
            flux << *o << "\n";
        } else {
            flux << *m << "\n";
        }
    }

    return flux;
}
