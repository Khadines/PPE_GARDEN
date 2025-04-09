<template>
  <div id="app">
    <div class="layout">
      <Navbar />
      <div class="content">
        <!-- Image en haut à droite -->
        <img src="/assets/historique.png" alt="Logo" class="top-right-image" />
        <!-- Logo au-dessus du titre -->
        <img src="/assets/logo.png" alt="Logo" class="section-logo" />

        <h1 class="title">Section capteur</h1>
        <h2 class="subtitle">Tous vos capteurs réunis, ici</h2>
        <div class="sensor-container">
          <div class="sensor-list">
            <div
              v-for="(sensor, index) in sensors"
              :key="sensor.title"
              class="sensor-card"
              :class="{ active: selectedSensorIndex === index }"
              @click="selectSensor(index)"
            >
              <img :src="sensor.logo" alt="Logo" class="sensor-logo" />
              <span class="sensor-title">{{ sensor.title }}</span>
            </div>
          </div>
          <div class="sensor-details">
            <h3>Qu'est-ce que c'est ?</h3>
            <p>{{ sensors[selectedSensorIndex].description }}</p>

            <div
              v-if="sensors[selectedSensorIndex].labels.length > 0"
              :class="{
                'label-container': true,
                horizontal: sensors[selectedSensorIndex].alignment === 'horizontal',
              }"
            >
              <div
                v-for="(label, idx) in sensors[selectedSensorIndex].labels"
                :key="idx"
                class="label"
              >
                <span class="label-title">{{ label.name }}</span>
                <span class="label-description">{{ label.description }}</span>
              </div>

              <!-- ✅ Conseil en dessous des labels -->
              <div
                v-if="getAdviceForSensor(sensors[selectedSensorIndex])"
                class="sensor-advice"
              >
                Conseil : {{ getAdviceForSensor(sensors[selectedSensorIndex]) }}
              </div>
            </div>
          </div>
        </div>

        <!-- Section potager -->
        <div class="garden-section">
          <h1 class="title" style="margin-top: 5rem;">Votre potager</h1>
          <h2 class="subtitle">Retrouvez l'emplacement de vos capteurs</h2>
          <img src="/assets/schemapotagerbis.png" alt="Diagramme du potager" class="garden-image" />

          <div class="valves-buttons">
            <div v-for="valve in 5" :key="valve" class="valve-control">
              <button @click="openValve(valve)">
                Ouvrir électrovanne {{ valve }}
              </button>
              <div class="valve-status">
                État :
                <span :class="{ open: valves[valve - 1].status === 'open', closed: valves[valve - 1].status === 'closed' }">
                  {{ valves[valve - 1].status === 'open' ? 'Ouverte' : 'Fermée' }}
                </span>
                <span v-if="valves[valve - 1].remainingTime > 0">
                  - Temps restant : {{ valves[valve - 1].remainingTime }}s
                </span>
              </div>
            </div>
          </div>
        </div>

        <!-- Contrôle électrovannes -->
        <div v-if="selectedValve !== null" class="modal-overlay" @click.self="closeModal">
          <div class="modal">
            <h2>Contrôle de l’électrovanne {{ selectedValve }}</h2>
            <label for="duration">Temps d'ouverture (en secondes) :</label>
            <input id="duration" type="number" v-model.number="valveDuration" min="1" placeholder="Ex: 60" />
            <label for="start-time">Heure de début :</label>
            <input id="start-time" type="time" v-model="startTime" />
            <div class="modal-buttons">
              <button @click="startTimedValve">Lancer avec minuterie</button>
              <button @click="closeValve">Fermer maintenant</button>
              <button class="cancel" @click="closeModal">Annuler</button>
            </div>
          </div>
        </div>

        <!-- Section météo -->
        <div class="weather-section">
          <h1 class="title" style="margin-top: 5rem;">Données météo en temps réel</h1>
          <h2 class="subtitle">Station météo connectée WeatherLink</h2>
          <div class="weather-widget">
            <iframe
              src='https://www.weatherlink.com/embeddablePage/show/9a2e3e058fb547f1942db93fc5519c9d/signature'
              width='760'
              height='200'
              frameborder='0'></iframe>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<script>
export default {
  name: "App",
  data() {
    return {
      selectedSensorIndex: 0,
      selectedValve: null,
      valveDuration: null,
      startTime: null,
      isConnected: false,
      lastUpdate: null,
      valves: Array.from({ length: 5 }, () => ({ status: 'closed', remainingTime: 0 })),
      sensorValues: { moisture: null, temperature: null, light: null, rain: null },
      sensors: [
        {
          title: "Capteur d'humidité",
          logo: "/assets/eau 1.png",
          description: "Les capteurs d'humidité du sol mesurent la teneur en eau volumétrique du sol.",
          alignment: "horizontal",
          labels: [{ name: " Humidité ", description: "Output: 26" }],
          unit: "%",
          key: "moisture"
        },
        {
          title: "Capteur de température",
          logo: "/assets/climat.png",
          description: "Mesure l'énergie thermique générée par un objet ou un système.",
          alignment: "vertical",
          labels: [{ name: " Température ", description: "Output: 11" }],
          unit: "°C",
          key: "temperature"
        },
        {
          title: "Capteur de lumière",
          logo: "/assets/solar-power 1.png",
          description: "Les capteurs de luminosité sont des détecteurs photosensibles ou détecteurs de lumière qui transforment la lumière absorbée en grandeur mesurable.",
          alignment: "vertical",
          labels: [{ name: " Luminosité ", description: "Output: 189" }],
          unit: "lux",
          key: "light"
        },
        {
          title: "Pluviomètre",
          logo: "/assets/sauver-la-nature 1.png",
          description: "Le pluviomètre est un instrument météorologique destiné à mesurer la quantité de précipitations (surtout la pluie) tombée pendant un intervalle de temps.",
          alignment: "vertical",
          labels: [{ name: " Pluie ", description: "Output: 2" }],
          unit: "mm",
          key: "rain"
        }
      ]
    };
  },
  mounted() {
    this.fetchSensorData();
    this.interval = setInterval(this.fetchSensorData, 5000);
  },
  beforeDestroy() {
    clearInterval(this.interval);
  },
  methods: {
    async fetchSensorData() {
      try {
        const response = await fetch('http://localhost:8000/data');
        if (!response.ok) throw new Error('Network response was not ok');
        const data = await response.json();
        this.sensorValues = data;
        this.isConnected = true;
        this.lastUpdate = new Date().toLocaleTimeString();
        this.updateSensorLabels();
      } catch (error) {
        console.error("Erreur de récupération:", error);
        this.isConnected = false;
      }
    },
    updateSensorLabels() {
      this.sensors.forEach(sensor => {
        if (sensor.key && this.sensorValues[sensor.key] !== undefined) {
          sensor.labels[0].description = `Output: ${this.sensorValues[sensor.key]} ${sensor.unit}`;
        }
      });
    },
    selectSensor(index) {
      this.selectedSensorIndex = index;
    },
    openValve(valveNumber) {
      this.selectedValve = valveNumber;
      this.valveDuration = null;
      this.startTime = null;
    },
    closeModal() {
      this.selectedValve = null;
      this.valveDuration = null;
      this.startTime = null;
    },
    closeValve() {
      const index = this.selectedValve - 1;
      this.valves[index].status = 'closed';
      this.valves[index].remainingTime = 0;
      this.closeModal();
    },
    startTimedValve() {
      const index = this.selectedValve - 1;
      const now = new Date();

      if (this.startTime) {
        const [hours, minutes] = this.startTime.split(":").map(Number);
        const start = new Date();
        start.setHours(hours, minutes, 0, 0);
        const delay = start - now;

        if (delay > 0) {
          setTimeout(() => this.activateValve(index), delay);
          return;
        }
      }

      this.activateValve(index);
      this.closeModal();
    },
    activateValve(index) {
      this.valves[index].status = 'open';
      this.valves[index].remainingTime = this.valveDuration;

      const interval = setInterval(() => {
        if (this.valves[index].remainingTime > 0) {
          this.valves[index].remainingTime--;
        } else {
          this.valves[index].status = 'closed';
          clearInterval(interval);
        }
      }, 1000);
    },
    getAdviceForSensor(sensor) {
      const val = this.sensorValues[sensor.key];
      switch (sensor.key) {
        case 'moisture':
          if (val < 30) return 'Sol trop sec : pensez à arroser ou vos plantes vont se dessécher.';
          if (val > 70) return 'Sol très humide : évitez d’arroser.';
          return 'Humidité correcte.';
        case 'temperature':
          if (val < 10) return 'Température basse, protégez les plantes sensibles.';
          if (val > 30) return 'Température élevée, surveillez l’arrosage.';
          return 'Température idéale pour le potager.';
        case 'light':
          if (val < 200) return 'Lumière faible, évitez les semis aujourd’hui.';
          if (val > 800) return 'Lumière très forte, attention aux brûlures.';
          return 'Luminosité normale.';
        case 'rain':
          if (val > 5) return 'Pluie détectée récemment, inutile d’arroser.';
          return 'Pas de pluie récente, arrosage peut être nécessaire.';
        default:
          return null;
      }
    }
  }
};
</script>

<style>
/* Styles communs */
#app {
  font-family: Arial, sans-serif;
  margin: 0;
  padding: 0;
  display: flex;
  flex-direction: column;
}
.layout {
  display: flex;
}
.content {
  flex: 1;
  padding: 2rem;
  position: relative;
}
.title {
  font-size: 2.5rem;
  font-weight: bold;
  margin-bottom: 0.5rem;
}
.subtitle {
  font-size: 1.25rem;
  color: #555;
  margin-bottom: 1.5rem;
}
.section-logo {
  display: block;
  max-width: 50px;
  height: auto;
  margin-bottom: 1rem;
}
.top-right-image {
  position: absolute;
  top: 0;
  right: 0;
  width: 240px;
  height: auto;
  z-index: 10;
  padding-top: 10px;
  padding-right: 10px;
}

/* Sensors */
.sensor-container {
  display: flex;
  gap: 2rem;
}
.sensor-list {
  display: flex;
  flex-direction: column;
  gap: 15px;
}
.sensor-card {
  display: flex;
  align-items: center;
  background-color: #F9F8FD;
  border: 2px solid #D9D9D9;
  border-radius: 15px;
  padding: 1rem;
  width: 240px;
  cursor: pointer;
  transition: transform 0.2s;
}
.sensor-card.active {
  background-color: #eceaf5;
  border-color: #b6b3c7;
}
.sensor-card:hover {
  transform: scale(1.02);
}
.sensor-logo {
  width: 40px;
  height: 40px;
  margin-right: 1rem;
}
.sensor-title {
  font-size: 1.2rem;
  font-weight: bold;
  color: #333;
}
.sensor-details {
  flex: 1;
  max-width: 700px;
  background-color: #F9F8FD;
  border: 1px solid #D9D9D9;
  border-radius: 15px;
  padding: 1rem;
  display: flex;
  flex-direction: column;
}
.label-container {
  margin-top: 1rem;
  display: flex;
  flex-direction: column;
  gap: 10px;
}
.label-container.horizontal {
  flex-direction: row;
  gap: 15px;
  flex-wrap: wrap;
}
.label {
  display: flex;
  flex-direction: column;
  align-items: center;
  text-align: center;
  background-color: #fff;
  border: 1px solid #d9d9d9;
  padding: 0.5rem;
  border-radius: 8px;
  min-width: 80px;
  max-width: 90px;
}
.label-title {
  font-weight: bold;
  color: #333;
}
.label-description {
  font-size: 0.9rem;
  color: #555;
  margin-top: 0.3rem;
}

.sensor-advice {
  display: table; /* ✅ force le shrink-wrap */
  margin-top: 1rem;
  font-size: 0.9rem;
  color: #6c63ff;
  background-color: #f4f2ff;
  padding: 0.75rem 1rem;
  border-radius: 8px;
  border: 1px solid #dcd2ff;
}

.garden-section {
  margin-top: 5rem;
  text-align: left;
}
.garden-image {
  max-width: 100%;
  height: auto;
  margin-top: 1rem;
  display: block;
}
.valves-buttons {
  margin-top: 2rem;
  display: flex;
  gap: 1rem;
  flex-wrap: wrap;
}
.valve-control {
  display: flex;
  flex-direction: column;
  align-items: flex-start;
  gap: 0.5rem;
}
.valve-status {
  font-size: 0.9rem;
  color: #333;
}
.valve-status .open {
  color: green;
  font-weight: bold;
}
.valve-status .closed {
  color: red;
  font-weight: bold;
}
.modal-overlay {
  position: fixed;
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
  background: rgba(0, 0, 0, 0.4);
  display: flex;
  align-items: center;
  justify-content: center;
  z-index: 999;
}
.modal {
  background: white;
  padding: 2rem;
  border-radius: 12px;
  max-width: 400px;
  width: 100%;
  box-shadow: 0 2px 12px rgba(0, 0, 0, 0.3);
}
.modal h2 {
  margin-bottom: 1rem;
}
.modal input {
  width: 100%;
  padding: 0.5rem;
  margin: 0.5rem 0 1rem;
  border-radius: 8px;
  border: 1px solid #ccc;
}
.modal-buttons {
  display: flex;
  justify-content: space-between;
  gap: 10px;
}
.modal-buttons button {
  flex: 1;
  padding: 0.5rem;
  border: none;
  border-radius: 8px;
  font-weight: bold;
  cursor: pointer;
}
.modal-buttons button:first-child {
  background-color: #4caf50;
  color: white;
}
.modal-buttons button:nth-child(2) {
  background-color: #f44336;
  color: white;
}
.modal-buttons button.cancel {
  background-color: #bbb;
}
.weather-section {
  margin-top: 3rem;
}
.weather-widget {
  margin-top: 1rem;
  display: flex;
  justify-content: center;
}
.valves-buttons button {
  background-color: #6c63ff;
  color: white;
  border: none;
  padding: 0.75rem 1.5rem;
  border-radius: 10px;
  cursor: pointer;
  font-weight: bold;
  font-size: 1rem;
  transition: all 0.3s ease;
  box-shadow: 0 4px 8px rgba(108, 99, 255, 0.3);
}
.valves-buttons button:hover {
  background-color: #5848d7;
  transform: translateY(-2px);
}
.valves-buttons button:active {
  background-color: #4c3ec4;
  transform: scale(0.97);
}
.top-right-image {
  position: absolute;
  top: 0;
  right: 0;
  width: 240px;
  height: auto;
  z-index: 10;
  padding-top: 10px;
  padding-right: 10px;
}

.section-logo {
  display: block;
  max-width: 50px; /* taille du logo */
  height: auto;
  margin-bottom: 1rem;
}
</style>
