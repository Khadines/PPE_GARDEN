from fastapi import FastAPI, Request
from fastapi.middleware.cors import CORSMiddleware
import json
from datetime import datetime
from typing import Dict, Any
from pydantic import BaseModel
#from fastapi.responses import JSONResponse
#import threading
#import requests

app = FastAPI()

#lancer FastAPI : uvicorn main:app --host 0.0.0.0 --port 8000
#navgateur : http://127.0.0.1:8000
#documentation : http://127.0.0.1:8000/docs
#npm run dev

# Configuration CORS
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],  # À restreindre en production
    allow_methods=["*"],
    allow_headers=["*"],
)

################################# SENSORS ####################################

# Stockage des données avec historique
sensor_data = {
    "latest": {
        "moisture": None,
        "temperature": None,
        "light": None,
        "rain": None,
        "timestamp": None
    },
    "history": []
}

@app.post("/data")
async def receive_data(request: Request):
    try:
        data = await request.json()
        
        # Accepte les deux formats :
        sensor_values = data if "sensors" not in data else data["sensors"]
        
        # Enregistrement
        sensor_data["latest"] = {
            "moisture": float(sensor_values.get("moisture", 0)),
            "temperature": float(sensor_values.get("temperature", 0)),
            "light": float(sensor_values.get("light", 0)),
            "rain": float(sensor_values.get("rain", 0)),
            "timestamp": datetime.utcnow().isoformat()
        }
        
        # Historique (optionnel)
        sensor_data["history"].append(sensor_data["latest"])
        
        return {"status": "success", "received_data": sensor_data["latest"]}
    
  
    except Exception as e:
        print(f"Erreur: {str(e)}")
        return {"error": str(e)}, 500

@app.get("/data")
def get_data(historical: bool = False, limit: int = 10):
    """
    Retourne les données des capteurs
    Paramètres:
    - historical: si True, retourne l'historique
    - limit: nombre maximal de points d'historique à retourner
    """
    if historical:
        return {
            "latest": sensor_data["latest"],
            "history": sensor_data["history"][-limit:]
        }
    return sensor_data["latest"]

@app.get("/")
def read_root():
    return {
        "message": "API de surveillance du potager",
        "endpoints": {
            "POST /data": "Envoyer des donnees de capteurs",
            "GET /data": "Recuperer les dernieres donnees",
            "GET /data?historical=true": "Recuperer l'historique"
        }
    }

################################# VALVES ####################################



# État initial des valves
valve_state = {
    "valve1": False,
    "valve2": False,
    "valve3": False,
    "valve4": False,
    "valve5": False
}


@app.post("/valves")
async def set_valves(request: Request):
    try:
        data = await request.json()
        print("Requête reçue du frontend:", data)
        #console.log("Payload reçu :", req.body);
        # Mise à jour de l'état de chaque vanne
        for i in range(1, 6):  # 5 vannes (valve1 à valve5)
            valve_key = f"valve{i}"
            valve_state[valve_key] = data.get(valve_key, False)
        
        return {"status": "valves updated", "valve_state": valve_state}
    except Exception as e:
        return {"error": str(e)}, 500


@app.get("/valves")
async def get_valve_states():
    print("Requête GET /valves - état actuel :", valve_state)
    return valve_state





