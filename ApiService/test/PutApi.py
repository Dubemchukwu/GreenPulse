import requests

def post_data():
    request = requests.put('http://dubemguy.pythonanywhere.com/sensor/api', json={'temperature': 25.48, 'humidity': 60.94, 'soil_moisture': 542})
    print(request.status_code)
    print(request.json())

post_data()