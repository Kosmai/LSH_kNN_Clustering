import sys
from random import randint
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from sklearn.preprocessing import MinMaxScaler
from keras.layers import Input, Dense, Conv1D, MaxPooling1D, UpSampling1D, BatchNormalization, LSTM, RepeatVector
from keras.models import Model, load_model
from data_import import read_dataset


def create_train_set(all_ts, training_ratio, window_size):
    ts = all_ts[0]

    x_train = []

    scalers = []
    for ts in all_ts:
        for i in range(window_size, int(training_ratio * len(ts['value']))):
            scaler = MinMaxScaler()
            x_train.append(scaler.fit_transform(ts['value'].values[i - window_size:i].reshape(-1, 1)))
            scalers.append(scaler)

    x_train = np.array(x_train)

    x_train = x_train.astype('float32')

    return x_train


def create_test_set(all_ts, training_ratio, window_size):
    ts = all_ts[0]

    x_test = []

    scalers = []
    for ts in all_ts:
        for i in range(int(training_ratio * len(ts['value'])) + window_size, len(ts['value'])):
            scaler = MinMaxScaler()
            x_test.append(scaler.fit_transform(ts['value'].values[i - window_size:i].reshape(-1, 1)))
            scalers.append(scaler)

    x_test = np.array(x_test)

    x_test = x_test.astype('float32')

    return x_test


def create_encoding_set(ts, window):
    pass


def train(all_ts, training_ratio, window_size, saved_model_name=None):
    x_train = create_train_set(all_ts, training_ratio, window_size)

    encoding_dim = 3

    input_window = Input(shape=(window_size, 1))
    x = Conv1D(16, encoding_dim, activation="relu", padding="same")(input_window)
    x = MaxPooling1D(2, padding="same")(x)
    x = Conv1D(1, encoding_dim, activation="relu", padding="same")(x)
    encoded = MaxPooling1D(2, padding="same")(x)

    encoder = Model(input_window, encoded)

    x = Conv1D(1, encoding_dim, activation="relu", padding="same")(encoded)

    x = UpSampling1D(2)(x)
    x = Conv1D(16, 2, activation='relu')(x)
    x = UpSampling1D(2)(x)
    decoded = Conv1D(1, encoding_dim, activation='sigmoid', padding='same')(x)
    autoencoder = Model(input_window, decoded)

    autoencoder.compile(optimizer='adam', loss='mean_squared_error')
    history = autoencoder.fit(x_train, x_train, epochs=10, batch_size=1024, validation_split=0.1, shuffle=True)

    if saved_model_name is not None:
        encoder.save('models/reduce/' + saved_model_name)

    return autoencoder, encoder


def test(all_ts, autoencoder, training_ratio, window_size):
    x_test = create_test_set(all_ts, training_ratio, window_size)

    decoded_stocks = autoencoder.predict(x_test)

    for _ in range(20):
        sample = randint(0, len(x_test))
        plt.figure(figsize=(16, 8))
        plt.title('Autoencoding')
        plt.grid()
        plt.plot(x_test[sample])
        plt.plot(decoded_stocks[sample])
        plt.legend(['Original', 'Decoded'])
        plt.show()


def encode(all_ts, indexes, encoder, window_size, output_file_name, output_query_name=None):
    print('Producing reduced time series, please wait...')

    reduced_df = None

    for count, ts in enumerate(all_ts):
        x_test = []
        scalers = []
        for i in range(window_size, len(ts['value'])):
            scaler = MinMaxScaler()
            x_test.append(scaler.fit_transform(ts['value'].values[i - window_size:i].reshape(-1, 1)))
            scalers.append(scaler)

        x_test = np.array(x_test)

        x_test = x_test.astype('float32')

        decoded_stocks = encoder.predict(x_test)

        decoded = []
        samples = x_test.shape[0]
        for j in range(0, samples, window_size):
            decoded += (scalers[j].inverse_transform(decoded_stocks[j])).tolist()

        decoded = list(map(lambda x: x[0], decoded))

        # fig, axs = plt.subplots(2)
        # axs[0].grid(True)
        # axs[1].grid(True)
        #
        # axs[0].plot(decoded)
        #
        # axs[1].plot(ts[:samples])
        # # axs.grid()
        # plt.show()

        s = pd.Series(decoded)
        s.name = indexes[count]

        if reduced_df is None:
            reduced_df = pd.DataFrame(columns=range(len(decoded)))

        reduced_df = reduced_df.append(s)

    if output_query_name is None:
        reduced_df.to_csv(output_file_name, sep='\t', header=False)
    else:
        reduced_df[:-10].to_csv(output_file_name, sep='\t', header=False)
        reduced_df[-10:].to_csv(output_query_name, sep='\t', header=False)


if __name__ == '__main__':
    pretrained_model = 'models/reduce/reduce_model'
    training_ratio = 0.7
    n = 20
    dataset = 'datasets/nasdaq2007_17.csv'
    output_dataset = 'reduced_stock_ts/input.csv'
    output_query = 'reduced_stock_ts/query.csv'
    save_model = False
    output_size = None

    # read command line arguments
    for i, arg in enumerate(sys.argv):
        if arg == '-od':
            output_dataset = sys.argv[i + 1]
        if arg == '-oq':
            output_query = sys.argv[i + 1]
        if arg == '-model':
            pretrained_model = sys.argv[i + 1]
        if arg == '-n':
            n = int(sys.argv[i + 1])
        if arg == '-out_n':
            output_size = int(sys.argv[i + 1])
        if arg == '-d':
            dataset = sys.argv[i + 1]
        if arg == '-save_model':
            save_model = True

    if pretrained_model:
        try:
            encoder = load_model(pretrained_model)
            all_ts, indexes = read_dataset(dataset, output_size)
            encode(all_ts, indexes, encoder, 10, output_dataset, output_query)
        except:
            print('Error while using pretrained model!')
            exit()
    else:
        try:
            saved_model_name = None
            if save_model:
                saved_model_name = input('Saved Model Name: ')

            all_ts, indexes = read_dataset(dataset, n)
            autoencoder, encoder = train(all_ts, training_ratio, 10, saved_model_name)
            test(all_ts, autoencoder, training_ratio, 10)
        except:
            print('Error while training/testing the model!')
            exit()
