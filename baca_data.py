import pyrebase
import pickle
import numpy as np
import time

filename = 'XGBC_model.pkl'

# Konfigurasi Firebase
config = {
    "apiKey": "AIzaSyDjHeDbeIotvRUuF0XZLeiRFjKgfv-6KHo",
    "authDomain": "sensor-b15c7.firebaseapp.com",
    "databaseURL": "https://sensor-b15c7-default-rtdb.firebaseio.com/",
    "storageBucket": "sensor-b15c7.appspot.com",
    "serviceAccount": "/home/noezz/TA#2/kunci.json"
}

# Inisialisasi aplikasi Firebase
firebase = pyrebase.initialize_app(config)

# Akses database
database = firebase.database()

# Load model
with open(filename, 'rb') as file:
    loaded_model = pickle.load(file)

try:
    while True:
        
        max4466 = database.child("/soundLevel%20MAX4466").get().val()
        max9814 = database.child("/soundLevel%20MAX9814").get().val()
        soundSensor = database.child("sound%20Sensor").get().val()

        bising_test = [[soundSensor, max4466, max9814]]
        data_tes = np.array(bising_test)

        y_pred = loaded_model.predict(data_tes)
        print("Prediksi:", y_pred)

        time.sleep(1)
except KeyboardInterrupt:
    print("Selesai.")
