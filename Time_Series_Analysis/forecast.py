import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from sklearn.preprocessing import MinMaxScaler
from sklearn.metrics import mean_absolute_error
from keras.models import Sequential
from keras.layers import Dense, LSTM, Dropout
from data_import import read_dataset

pretrained_model = False
batch_training = False

all_ts = read_dataset('datasets/nasdaq2007_17.csv', 5)


def create_train_set(ts, training_ratio, lookback):
    training_size = int(training_ratio * ts.shape[0])
    train_ts = ts[:training_size]

    x_train = []
    y_train = []

    for i in range(lookback, len(train_ts)):
        x_train.append(train_ts[i - lookback:i, 0])
        y_train.append(train_ts[i, 0])

    return x_train, y_train


def create_test_set(ts, training_ratio, lookback):
    training_size = int(training_ratio * ts.shape[0])
    test_ts = ts[training_size - lookback:, :]

    x_test = []

    for i in range(lookback, len(test_ts)):
        x_test.append(test_ts[i - lookback:i, 0])

    x_test = np.array(x_test)
    x_test = np.reshape(x_test, (x_test.shape[0], x_test.shape[1], 1))

    return x_test


def create_forecast_set(ts, lookback):
    x_forecast = []

    for i in range(lookback, len(ts)):
        x_forecast.append(ts[i - lookback:i, 0])

    x_forecast = np.array(x_forecast)
    x_test = np.reshape(x_forecast, (x_forecast.shape[0], x_forecast.shape[1], 1))

    return x_test


def train(all_ts, training_ratio, lookback, saved_model_name=None):
    # create scaler
    all_ts_df = pd.DataFrame(all_ts[0])

    for series in all_ts[1:]:
        all_ts_df = all_ts_df.append(series)

    scaler = MinMaxScaler(feature_range=(0, 1))
    scaler.fit(all_ts_df)

    # create x_train and y_train
    x_train = []
    y_train = []

    for series in all_ts:
        scaled_ts = scaler.transform(series)
        train_set = create_train_set(scaled_ts, training_ratio, lookback)

        x_train += train_set[0]
        y_train += train_set[1]

    x_train, y_train = np.array(x_train), np.array(y_train)
    x_train = np.reshape(x_train, (x_train.shape[0], x_train.shape[1], 1))

    model = Sequential()

    model.add(LSTM(50, return_sequences=True, input_shape=(x_train.shape[1], 1)))
    model.add(Dropout(0.2))

    model.add(LSTM(50, return_sequences=True))
    model.add(Dropout(0.2))

    model.add(LSTM(50, return_sequences=True))
    model.add(Dropout(0.2))

    model.add(LSTM(50, return_sequences=False))
    model.add(Dropout(0.2))

    model.add(Dense(1))

    model.compile(optimizer='Adam', loss='mean_squared_error')

    model.fit(x_train, y_train, batch_size=50, epochs=2)

    return model


def test(ts, training_ratio, model, lookback):
    scaler = MinMaxScaler()
    scaled_ts = scaler.fit_transform(ts)
    x = create_test_set(scaled_ts, training_ratio, lookback)
    predictions = model.predict(x)
    predictions = scaler.inverse_transform(predictions)
    return predictions


def forecast(ts, model, lookback):
    scaler = MinMaxScaler()
    scaled_ts = scaler.fit_transform(ts)
    x = create_forecast_set(scaled_ts, lookback)
    predictions = model.predict(x)
    predictions = scaler.inverse_transform(predictions)
    return predictions


training_ratio = 0.7
lookback = 10

if pretrained_model:
    pass
else:
    if batch_training:
        model = train(all_ts, training_ratio, lookback)

        for ts in all_ts:
            training_size = int(0.7 * ts.shape[0])
            forecasts = test(ts, training_ratio, model, lookback)

            forecasted_part = ts[training_size:].copy()
            forecasted_part['forecasts'] = forecasts

            plt.figure(figsize=(16, 8))
            plt.title('Forecasting')
            plt.xlabel('Date')
            plt.ylabel('Value')
            plt.plot(forecasted_part['value'])
            plt.plot(forecasted_part['forecasts'])
            mae = mean_absolute_error(forecasted_part['value'], forecasted_part['forecasts'])
            plt.legend(['Real Values', 'Predicted Values\n' + 'Mean Absolute Error: ' + str(round(mae, 2))])
            plt.show()

    else:
        for ts in all_ts:
            model = train([ts], training_ratio, lookback)
            training_size = int(training_ratio * ts.shape[0])
            forecasts = test(ts, training_ratio, model, lookback)

            forecasted_part = ts[training_size:].copy()
            forecasted_part['forecasts'] = forecasts

            plt.figure(figsize=(16, 8))
            plt.title('Forecasting')
            plt.xlabel('Date')
            plt.ylabel('Value')
            plt.plot(forecasted_part['value'])
            plt.plot(forecasted_part['forecasts'])
            mae = mean_absolute_error(forecasted_part['value'], forecasted_part['forecasts'])
            plt.legend(['Real Values', 'Predicted Values\n' + 'Mean Absolute Error: ' + str(round(mae, 2))])
            plt.show()
