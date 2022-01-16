import sys
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from sklearn.preprocessing import MinMaxScaler
from keras.models import Sequential, load_model
from keras.layers import Dense, LSTM, Dropout, RepeatVector, TimeDistributed
from datetime import timedelta
from data_import import read_dataset


def create_train_set(ts, training_ratio, lookback):
    training_size = int(training_ratio * ts.shape[0])
    train_ts = ts[:training_size]

    x = train_ts[['value']]
    y = train_ts['value']

    x_train = []
    y_train = []

    for i in range(len(x) - lookback):
        v = x.iloc[i:(i + lookback)].values
        x_train.append(v)
        y_train.append(y.iloc[i + lookback])

    return x_train, y_train


def create_test_set(ts, training_ratio, lookback):
    training_size = int(training_ratio * ts.shape[0])
    test_ts = ts[training_size - lookback:]

    x = test_ts[['value']]
    y = test_ts['value']

    x_test = []
    y_test = []

    for i in range(len(x) - lookback):
        v = x.iloc[i:(i + lookback)].values
        x_test.append(v)
        y_test.append(y.iloc[i + lookback])

    return np.array(x_test)


def create_forecast_set(ts, lookback):
    forecast_ts = ts.copy()

    x = forecast_ts[['value']]
    y = forecast_ts['value']

    x_forecast = []
    for i in range(len(x) - lookback):
        v = x.iloc[i:(i + lookback)].values
        x_forecast.append(v)

    return np.array(x_forecast)


def train(all_ts, training_ratio, lookback, saved_model_name=None):
    scaler = MinMaxScaler(feature_range=(0, 1))
    all_ts_df = pd.DataFrame(all_ts[0])

    for series in all_ts[1:]:
        all_ts_df = all_ts_df.append(series)

    scaler.fit(all_ts_df)

    x_train = []
    y_train = []

    for series in all_ts:
        scaled_ts = series.copy()
        scaled_ts['value'] = scaler.transform(scaled_ts[['value']])
        train_set = create_train_set(scaled_ts, training_ratio, lookback)

        x_train += train_set[0]
        y_train += train_set[1]

    x_train = np.array(x_train)
    y_train = np.array(y_train)

    model = Sequential()
    model.add(LSTM(units=64, input_shape=(x_train.shape[1], x_train.shape[2])))
    model.add(Dropout(rate=0.2))

    model.add(RepeatVector(n=x_train.shape[1]))
    model.add(LSTM(units=64, return_sequences=True))
    model.add(Dropout(rate=0.2))

    model.add(TimeDistributed(Dense(x_train.shape[2])))
    model.compile(optimizer='adam', loss='mae')

    model.fit(x_train, y_train, epochs=4, batch_size=64, validation_split=0.1, shuffle=False)

    if saved_model_name is not None:
        model.save('models/detect/' + saved_model_name)

    return model


def test_forecast(ts, model, lookback, threshold, training_ratio=None):
    scaler = MinMaxScaler()
    test_ts = ts.copy()
    test_ts['value'] = scaler.fit_transform(test_ts[['value']])

    if training_ratio is not None:
        x_test = create_test_set(test_ts, training_ratio, lookback)
        training_size = int(training_ratio * ts.shape[0])
        test_ts = test_ts[training_size - lookback:]
    else:
        x_test = create_forecast_set(test_ts, lookback)

    # threshold = scaler.transform([[threshold]])[0][0]

    x_test_pred = model.predict(x_test)
    test_mae_loss = np.mean(np.abs(x_test_pred - x_test), axis=1)

    test_score_df = pd.DataFrame(index=test_ts[lookback:].index)
    test_score_df['loss'] = test_mae_loss
    test_score_df['threshold'] = threshold
    test_score_df['anomaly'] = test_score_df.loss > test_score_df.threshold
    test_score_df['value'] = test_ts[lookback:].value

    plt.figure(figsize=(16, 8))
    plt.grid()
    plt.title('Error')
    plt.plot(test_score_df.index, test_score_df.loss, label='loss')
    plt.plot(test_score_df.index, test_score_df.threshold, label='threshold')
    plt.xticks(rotation=25)
    plt.legend()

    anomalies = test_score_df[test_score_df.anomaly == True]
    anomalies.reindex()

    corrected_dates = []

    # sync anomalies points with the correct time series points
    for date in anomalies.index:
        day_before = (date - timedelta(days=2)).date()
        corrected_dates.append(day_before)
        anomalies.loc[date, 'value'] = test_ts.loc[str(day_before), 'value']

    anomalies.index = corrected_dates

    plt.figure(figsize=(16, 8))
    plt.grid()
    plt.title('Anomalies')
    plt.plot(test_ts['value'], zorder=1)
    plt.scatter(anomalies.index, anomalies['value'], color='red', linewidths=1, zorder=2)
    plt.show()


if __name__ == '__main__':
    dataset = 'datasets/nasdaq2007_17.csv'
    n = 400
    mae_threshold = 0.15
    save_model = False
    pretrained_model = None  # 'models/detect/model1'

    # read command line arguments
    for i, arg in enumerate(sys.argv):
        if arg == '-model':
            pretrained_model = sys.argv[i + 1]
        if arg == '-n':
            n = int(sys.argv[i + 1])
        if arg == '-d':
            dataset = sys.argv[i + 1]
        if arg == '-save_model':
            save_model = True
        if arg == '-mae':
            mae_threshold = float(sys.argv[i + 1])

    try:
        all_ts, _ = read_dataset(dataset, n)
    except:
        print('Error in reading dataset!')
        exit()

    training_ratio = 0.7
    lookback = 200

    # use pretrained model
    if pretrained_model is not None:
        try:
            model = load_model(pretrained_model)
        except:
            print('Error in loading pretrained model!')
            exit()
    # train and test model
    else:
        try:
            if save_model:
                model_name = input("Saved Model Name: ")
                model = train(all_ts, training_ratio, lookback, model_name)
            else:
                model = train(all_ts, training_ratio, lookback)
        except:
            print('Error while training model!')
    try:
        if pretrained_model is not None:
            for ts in all_ts:
                test_forecast(ts, model, lookback, mae_threshold)
        else:
            for ts in all_ts:
                test_forecast(ts, model, lookback, mae_threshold, training_ratio)
    except:
        print('Error while making forecasts')
        exit()
