from fastapi import FastAPI, Request
import json

app = FastAPI()

@app.post("/data")
async def receive_data(request: Request):
    data = await request.json()
    print("📥 Données reçues :", json.dumps(data, indent=4))  # Log en console
    return {"message": "Données reçues avec succès"}

#lancer FastAPI : uvicorn main:app --host 0.0.0.0 --port 8000
#relancer : uvicorn main:app --host 0.0.0.0 --port 8000 --reload
#navgateur : http://127.0.0.1:8000
#documentation : http://127.0.0.1:8000/docs
