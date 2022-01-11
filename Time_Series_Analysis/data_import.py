import pandas as pd


def get_time_series(data, index, starting_date='2007-01-01', introduce_anomaly=False):
    ts = pd.DataFrame()
    ts['value'] = data.iloc[[index]].transpose().iloc[:, 0]
    ts['date'] = pd.date_range(starting_date, periods=ts.shape[0]).tolist()
    ts = ts.set_index('date')
    if introduce_anomaly:
        ts['value'][3000:3002] = -40
    return ts


def read_dataset(file_name, num_of_ts=None, sep='\t', header=None, index_col=0, introduce_anomaly=False):
    data = pd.read_csv(file_name, sep=sep, header=header, index_col=index_col)
    data.index.name = None

    all_ts = []

    if num_of_ts is None or num_of_ts > data.shape[0]:
        num_of_ts = data.shape[0]

    for i in range(num_of_ts):
        all_ts.append(get_time_series(data, i, introduce_anomaly=introduce_anomaly))

    if num_of_ts is not None and num_of_ts < data.shape[0]:
        all_ts = all_ts[:num_of_ts]

    return all_ts
