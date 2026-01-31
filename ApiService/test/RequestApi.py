import requests
import pytest

@pytest.fixture
def api_client():
    return requests.Session()

def test_sensor_api(api_client):
    response = api_client.get("http://127.0.0.1:8000/sensor/api")
    assert response.status_code == 200
    assert response.json() == {"temperature": 0.0, "soil_moisture": 0.0, "humidity": 0.0}
    
def test_sensor_data(api_client):
    data = {"temperature": 25.0, "soil_moisture": 0.5, "humidity": 60.0}
    response = api_client.put("http://127.0.0.1:8000/sensor/api", json=data)
    assert response.status_code == 201
    print(response.json())

if __name__ == "__main__":
    pytest.main(["-v", __file__])
    # test_sensor_api(api_client)