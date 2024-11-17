#include "Particle.h"
#include "ResonanceType.h"
#include "ParticleType.h"
#include <iostream>
#include <cmath>
#include <cstdlib>

// Inizializzazione dell'array statico dei tipi di particelle
ParticleType *Particle::fParticleType[Particle::fMaxNumParticleType] = {nullptr};

// Inizializzazione del numero di tipi di particelle registrati
int Particle::fNParticleType = 0;

// Costruttore di default: inizializza le componenti della quantità di moto a 0 e l'indice a -1
Particle::Particle() : fPx(0), fPy(0), fPz(0), fIndex(-1) {}

// Costruttore parametrizzato: imposta il tipo di particella e le componenti iniziali della quantità di moto
// name: nome del tipo di particella
// px, py, pz: componenti della quantità di moto
Particle::Particle(const std::string &name, double px, double py, double pz)
    : fPx(px), fPy(py), fPz(pz)
{
  fIndex = FindParticleType(name);
  if (fIndex == -1)
  {
    std::cout << "Particle type " << name << " not found!" << std::endl;
  }
}

// Metodo statico che cerca un tipo di particella per nome e restituisce il suo indice
// name: nome del tipo di particella
// return: indice del tipo di particella o -1 se non trovato
int Particle::FindParticleType(const std::string &name)
{
  for (int i = 0; i < fNParticleType; ++i)
  {
    if (fParticleType[i]->GetName() == name)
    {
      return i;
    }
  }
  return -1;
}

// Restituisce l'indice del tipo di particella associato
int Particle::GetParticleTypeIndex() const
{
  return fIndex;
}

// Metodo statico che restituisce un tipo di particella dato il suo indice
// index: indice del tipo di particella
// return: puntatore al tipo di particella o nullptr se l'indice non è valido
const ParticleType *Particle::GetParticleType(int index)
{
  if (index >= 0 && index < fNParticleType)
  {
    return fParticleType[index];
  }
  else
  {
    return nullptr;
  }
}

// Aggiunge un nuovo tipo di particella all'array statico, se c'è spazio disponibile
// name: nome del tipo di particella
// mass: massa della particella
// charge: carica elettrica della particella
// width: larghezza della risonanza (opzionale)
void Particle::AddParticleType(const std::string &name, double mass, int charge, double width)
{
  if (fNParticleType >= fMaxNumParticleType)
  {
    std::cout << "Cannot add more particle types, maximum reached!" << std::endl;
    return;
  }

  if (FindParticleType(name) != -1)
  {
    std::cout << "Particle type " << name << " already exists!" << std::endl;
    return;
  }

  if (width == 0)
  {
    fParticleType[fNParticleType] = new ParticleType(name, mass, charge);
  }
  else
  {
    fParticleType[fNParticleType] = new ResonanceType(name, mass, charge, width);
  }
  ++fNParticleType;
}

// Imposta l'indice del tipo di particella usando il nome
// name: nome del tipo di particella
void Particle::SetParticleTypeIndex(const std::string &name)
{
  fIndex = FindParticleType(name);
  if (fIndex == -1)
  {
    std::cout << "Particle type " << name << " not found!" << std::endl;
  }
}

// Imposta l'indice del tipo di particella direttamente
// index: indice del tipo di particella
void Particle::SetParticleTypeIndex(int index)
{
  if (index >= 0 && index < fNParticleType)
  {
    fIndex = index;
  }
  else
  {
    std::cout << "Invalid particle index!" << std::endl;
  }
}

// Stampa le informazioni sul tipo di particella e sulle componenti della quantità di moto
void Particle::Print() const
{
  if (fIndex != -1 && fIndex < fNParticleType)
  {
    fParticleType[fIndex]->Print();
  }
  std::cout << "Px: " << fPx << ", Py: " << fPy << ", Pz: " << fPz << std::endl;
}

// Imposta le componenti della quantità di moto
// px, py, pz: nuove componenti della quantità di moto
void Particle::SetPulse(double px, double py, double pz)
{
  fPx = px;
  fPy = py;
  fPz = pz;
}

// Restituisce la massa della particella in base al suo tipo
// return: massa della particella o 0 se l'indice non è valido
double Particle::GetMass() const
{
  return (fIndex != -1) ? fParticleType[fIndex]->GetMass() : 0;
}

// Calcola l'energia totale della particella usando la relazione relativistica
// return: energia totale della particella
double Particle::GetEnergy() const
{
  double mass = GetMass();
  double p2 = fPx * fPx + fPy * fPy + fPz * fPz;
  return std::sqrt(mass * mass + p2);
}

// Calcola la massa invariante con un'altra particella
// other: altra particella
// return: massa invariante
double Particle::InvariantMass(const Particle &other) const
{
  double e1 = GetEnergy();
  double e2 = other.GetEnergy();
  double px_total = fPx + other.GetPulseX();
  double py_total = fPy + other.GetPulseY();
  double pz_total = fPz + other.GetPulseZ();
  double p2_total = px_total * px_total + py_total * py_total + pz_total * pz_total;
  return std::sqrt((e1 + e2) * (e1 + e2) - p2_total);
}

// Simula il decadimento in due particelle figlie conservando la quantità di moto
// dau1, dau2: particelle figlie risultanti dal decadimento
// return: stato del decadimento (0: successo, 1: errore di massa zero, 2: massa insufficiente)
int Particle::Decay2Body(Particle &dau1, Particle &dau2) const
{
  if (GetMass() == 0.0)
  {
    std::cerr << "Decayment cannot be performed if mass is zero\n";
    return 1;
  }

  double massMot = GetMass();
  double massDau1 = dau1.GetMass();
  double massDau2 = dau2.GetMass();

  // Effetto di larghezza per particelle di tipo ResonanceType
  if (fIndex > -1)
  {
    ResonanceType *resonance = dynamic_cast<ResonanceType *>(fParticleType[fIndex]);
    if (resonance)
    {
      float x1, x2, w, y1;
      double invnum = 1. / RAND_MAX;
      do
      {
        x1 = 2.0 * rand() * invnum - 1.0;
        x2 = 2.0 * rand() * invnum - 1.0;
        w = x1 * x1 + x2 * x2;
      } while (w >= 1.0);

      w = sqrt((-2.0 * log(w)) / w);
      y1 = x1 * w;
      massMot += resonance->GetWidth() * y1;
    }
  }

  if (massMot < massDau1 + massDau2)
  {
    std::cerr << "Decayment cannot be performed because mass is too low in this channel\n";
    return 2;
  }

  double pout = sqrt((massMot * massMot - (massDau1 + massDau2) * (massDau1 + massDau2)) *
                     (massMot * massMot - (massDau1 - massDau2) * (massDau1 - massDau2))) /
                (massMot * 2.0);

  double norm = 2 * M_PI / RAND_MAX;
  double phi = rand() * norm;
  double theta = rand() * norm * 0.5 - M_PI / 2.;

  // Assegna la quantità di moto ai prodotti del decadimento in direzioni opposte
  dau1.SetPulse(pout * sin(theta) * cos(phi), pout * sin(theta) * sin(phi), pout * cos(theta));
  dau2.SetPulse(-pout * sin(theta) * cos(phi), -pout * sin(theta) * sin(phi), -pout * cos(theta));

  double energy = sqrt(fPx * fPx + fPy * fPy + fPz * fPz + massMot * massMot);
  double bx = fPx / energy;
  double by = fPy / energy;
  double bz = fPz / energy;

  // Applica un boost relativistico ai decadimenti
  dau1.Boost(bx, by, bz);
  dau2.Boost(bx, by, bz);

  return 0;
}

// Applica un boost relativistico alla quantità di moto della particella
// bx, by, bz: componenti della velocità relativistica
void Particle::Boost(double bx, double by, double bz)
{
  double energy = GetEnergy();
  double b2 = bx * bx + by * by + bz * bz;
  double gamma = 1.0 / sqrt(1.0 - b2);
  double bp = bx * fPx + by * fPy + bz * fPz;
  double gamma2 = (b2 > 0) ? (gamma - 1.0) / b2 : 0.0;

  fPx += gamma2 * bp * bx + gamma * bx * energy;
  fPy += gamma2 * bp * by + gamma * by * energy;
  fPz += gamma2 * bp * bz + gamma * bz * energy;
}