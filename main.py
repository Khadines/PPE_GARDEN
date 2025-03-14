from fastapi import FastAPI

app = FastAPI()

@app.get("/")
def read_root():
    return {"message": "Hello, FastAPI!"}

#lancer FastAPI : uvicorn main:app --host 0.0.0.0 --port 8000
#navgateur : http://127.0.0.1:8000
#documentation : http://127.0.0.1:8000/docs
