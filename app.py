from flask import Flask, jsonify, request
from flask_cors import CORS
import requests

app = Flask(__name__)
CORS(app)  # permite CORS pentru orice origine

ESP1_IP = "192.168.1.131"   # IP-ul robotului din față (emitator optic)
ESP2_IP = "192.168.1.135"   # IP-ul robotului din spate (receptor + motoare)

@app.route("/")
def home():
    return "✅ Backend funcționează!"

@app.route("/send_franare", methods=["POST"])
def send_franare():
    try:
        # Trimite comanda către ESP1 (emitator LED optic)
        resp = requests.get(f"http://{ESP1_IP}/franare", timeout=3)
        return jsonify({
            "status": "ok",
            "raspuns_esp": resp.text
        })
    except Exception as e:
        return jsonify({
            "status": "error",
            "mesaj": str(e)
        }), 500

ultimul_eveniment_esp1 = "Aștept eveniment optic..."

@app.route("/notify_franare", methods=["POST"])
def notify_franare():
    global ultimul_eveniment_esp1
    data = request.get_json()
    if data and data.get("eveniment") == "franare_urgenta_detectata":
        ultimul_eveniment_esp1 = "🟠 ESP1: Frânare detectată optic!"
        print(ultimul_eveniment_esp1)
        return jsonify({"status": "ok"})
    return jsonify({"status": "error"}), 400

@app.route("/status_esp1", methods=["GET"])
def status_esp1():
    return jsonify({"eveniment": ultimul_eveniment_esp1})

# ==== Proxy pentru /status de la ESP2 ====
@app.route("/status_esp2", methods=["GET"])
def status_esp2():
    try:
        resp = requests.get(f"http://{ESP2_IP}/status", timeout=3)
        return resp.text, 200, {'Content-Type': 'application/json'}
    except Exception as e:
        print(f"[EROARE] Nu s-a putut obține statusul de la ESP2: {e}")
        return jsonify({
            "status": "error",
            "mesaj": f"Eroare la ESP2: {str(e)}"
        }), 500

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5001, debug=True)
