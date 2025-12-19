import json
from dotenv import load_dotenv
import os
from flask import Flask, request
from flask_restful import Api, Resource, reqparse, abort


message = []


load_dotenv()
API_KEY = os.environ.get("API_KEY")

app = Flask(__name__)
api = Api(app)

try:
    with open("data/SensorData.json", mode="r") as data:
        sensor_data = json.load(data)
except FileNotFoundError:
    with open("data/SensorData.json", mode="x") as data:
        json.dump({"temperature": 0.0, "soil_moisture": 0.0, "humidity": 0.0}, data)

class Sensor(Resource):
    def get(self):
        return sensor_data, 200

    def put(self):
        if True:  # request.form.get("api_key") == API_KEY:
            message = []

            if request.json.get("temperature"):
                sensor_data["temperature"] = round(float(request.json.get("temperature")), 3)
                with open("SensorData.json", "w") as update:
                    json.dump(sensor_data, update)
                message.append({"temperature": "Successfully updated"})
                
            if request.json.get("soil_moisture"):
                sensor_data["soil_moisture"] = round(float(request.json.get("soil_moisture")), 3)
                with open("SensorData.json", "w") as update:
                    json.dump(sensor_data, update)
                message.append({"soil_moisture": "Successfully updated"})
                
            if request.json.get("humidity"):
                sensor_data["humidity"] = round(float(request.json.get("humidity")), 3)
                with open("SensorData.json", "w") as update:
                    json.dump(sensor_data, update)
                message.append({"humidity": "Successfully updated"})
                
            if len(message) == 0:
                return {"Data": "nothing to see here"}, 202
            else:
                return {"Results": message}, 201

            # return {"Process": "Successfully updated"}, 201
        else:
            return {"Error": "Invalid api key"}, 400


api.add_resource(Sensor, "/sensor/api")

if __name__ == "__main__":
    app.run(host="127.0.0.1", port=8000, debug=True)
