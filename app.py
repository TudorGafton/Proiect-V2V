from flask import Flask, jsonify, request
from flask_cors import CORS
import requests

app = Flask(__name__)
CORS(app)  # permite CORS pentru orice origine

ESP1_IP = "192.168.1.132"   # IP-ul ESP1
ESP2_IP = "192.168.1.133"   # IP-ul ESP2

@app.route("/")
def home():
    return "Backend funcționează!"

@app.route("/pornire_motor", methods=["POST"])
def pornire_motor():
    try:
        r = requests.get(f"http://{ESP1_IP}/pornire_motor", timeout=2)
        return jsonify({"raspuns": r.text})
    except Exception as e:
        return jsonify({"eroare": str(e)})

@app.route("/oprire_motor", methods=["POST"])
def oprire_motor():
    try:
        r = requests.get(f"http://{ESP1_IP}/oprire_motor", timeout=2)
        return jsonify({"raspuns": r.text})
    except Exception as e:
        return jsonify({"eroare": str(e)})

@app.route("/franare", methods=["POST"])
def franare():
    try:
        r = requests.get(f"http://{ESP1_IP}/franare", timeout=2)
        return jsonify({"raspuns": r.text})
    except Exception as e:
        return jsonify({"eroare": str(e)}) 

@app.route("/status_esp2", methods=["GET"])
def status_esp2():
    try:
        resp = requests.get(f"http://{ESP2_IP}/status", timeout=2)
        return resp.text, 200, {'Content-Type': 'application/json'}
    except Exception as e:
        return jsonify({
            "status": "error",
            "mesaj": f"Eroare la ESP2: {str(e)}"
        }), 500

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5001, debug=True)
