from xgboost import XGBClassifier
from sklearn.model_selection import train_test_split
import pandas as pd
import numpy as np
import seaborn as sns
from sklearn.metrics import accuracy_score, r2_score
import pickle

filename = 'XGBC_model.pkl'
df = pd.read_csv('hasilSB.csv')


tgt = df["Actual"]
ftr = df[['sound Sensor','soundLevel MAX4466','soundLevel MAX9814']]

XA = ftr.values
YA = tgt.values

xtrain, xtest, ytrain, ytest = train_test_split(XA,YA, train_size=0.8, random_state=123) #dari data dibagi 0.8 untuk train dan sisa 0.2 ntuk test

model = XGBClassifier(random_state=123)

model.fit(xtrain, ytrain)

pred = model.predict(xtest)

r2 = r2_score(ytest, pred)
print(r2, "sekian")

with open(filename, 'wb') as file :
    pickle.dump(model, file)