<script setup lang="ts">
import { ref, onUnmounted } from 'vue'

interface SensorData {
  moisture?: number;
  temperature?: number;
  light?: number;
  rainGauge?: number;
}

const sensorData = ref<SensorData | null>(null)
const error = ref<string | null>(null)

// Récupérer les données toutes les 5 secondes
const fetchData = async () => {
  try {
    const response = await fetch('http://localhost:8000/data')
    if (!response.ok) throw new Error(`HTTP error! status: ${response.status}`)
    sensorData.value = await response.json()
  } catch (err) {
    error.value = err instanceof Error ? err.message : 'Unknown error'
    console.error("Erreur de récupération:", err)
  }
}

// Appel initial
fetchData()

// Mise à jour périodique
const interval = setInterval(fetchData, 5000)

// Nettoyer l'intervalle quand le composant est démonté
onUnmounted(() => clearInterval(interval))
</script>

<template>
  <div>
    <NuxtRouteAnnouncer />
    <NuxtPage />

    <h1>Dashboard Capteurs</h1>

    <!-- Afficher les données des capteurs -->
    <div v-if="sensorData">
      <div class="sensor-card">
        <h3>Humidité du sol</h3>
        <p>{{ sensorData.moisture }} (valeur brute)</p>
      </div>

      <div class="sensor-card">
        <h3>Température</h3>
        <p>{{ sensorData.temperature }}°C</p>
      </div>

      <div class="sensor-card">
        <h3>Température</h3>
        <p>{{ sensorData.light }}°C</p>
      </div>

      <div class="sensor-card">
        <h3>Température</h3>
        <p>{{ sensorData.rainGauge }}°C</p>
      </div>
    </div>

    <div v-else-if="error" class="error">
      Erreur: {{ error }}
    </div>

    <div v-else>
      <p>Chargement des données...</p>
    </div>
  </div>
</template>

<style>
.sensor-card {
  border: 1px solid #ddd;
  border-radius: 8px;
  padding: 20px;
  margin: 10px 0;
  max-width: 300px;
}

.error {
  color: red;
}
</style>