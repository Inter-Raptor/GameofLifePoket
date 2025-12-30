# 🧬 GameOfLifePoket

Un petit appareil “pocket” basé sur **ESP32** pour observer et manipuler le **Game of Life** (automate cellulaire) directement sur écran, avec des réglages physiques simples.

🎬 **Vidéo de présentation (YouTube)** : https://youtu.be/aNUI-kbQdfw  
📦 **Code / projet GitHub** : https://github.com/Inter-Raptor/GameofLifePoket

---

## ✨ Fonctionnalités

- Affichage du **Game of Life** sur écran
- **Mode Auto** (simulation en continu)
- **Mode Manuel** (contrôle / actions selon le firmware)
- **Play / Pause**
- Réglages rapides via **potentiomètres** (dimensions/paramètres)
- Interface 100% “hardware” (boutons + potars)

---

## 🕹️ Interface & commandes

### Face avant
- **Écran**
- **Bouton Toggle** (à droite de l’écran)
- Sous l’écran :
  - **Bouton Manu/Auto** (en bas à gauche)
  - **Bouton Play/Pause** (en bas à droite)

### Côté gauche
- **Potentiomètre “Hauteur”** (réglage de la hauteur)

### Dessus
- **Potentiomètre “Largeur”** (réglage de la largeur)
- **Interrupteur ON/OFF** (en haut à droite)

### Côté droit
- **Port USB-C** (recharge / alimentation)

---

## 🚀 Démarrage rapide

1. **Allume** l’appareil (interrupteur ON/OFF).
2. Ajuste la **largeur** (potar du dessus) et la **hauteur** (potar du flanc gauche).
3. Choisis le mode avec **Manu/Auto** :
   - **Manu** : contrôle plus “posé” (selon la version du firmware)
   - **Auto** : la simulation évolue en continu
4. Lance / stoppe la simulation avec **Play/Pause**.
5. Utilise **Toggle** pour activer la fonction associée (bascule d’option / action rapide selon la version).

> 💡 Astuce : fais tes réglages largeur/hauteur **avant** de lancer en Auto pour éviter de perturber une scène en cours.

---

## 🧠 Notes sur les modes (à adapter si besoin)

- **Auto** : la simulation avance en continu tant que ce mode est actif.
- **Manu** : permet une interaction plus “contrôlée” (ex : step, injection, bascule d’un paramètre… selon le code).
- **Play/Pause** : met en pause ou relance l’évolution.
- **Toggle** : bouton multi-usage pour changer rapidement d’option.

---

## 🛠️ Dépannage (rapide)

- **Écran noir / rien ne s’affiche** :
  - vérifie l’alimentation (USB-C / batterie)
  - vérifie l’interrupteur **ON/OFF**
- **Réglages incohérents** :
  - mets les potars au milieu et reteste
  - passe en **Pause** puis ajuste largeur/hauteur

---

## 📌 Idées futures (Roadmap)

- Menu d’options à l’écran (vitesse, densité, thèmes…)
- Sauvegarde / chargement de patterns
- Indicateur batterie / charge
- Boîtier final + PCB custom

---

## 📄 Licence

À définir (MIT / GPL / autre).  
Si tu veux une base simple et permissive : **MIT**.

---

## 🙌 Crédits

- Concept : **Conway’s Game of Life**
- Projet : **Inter-Raptor**
