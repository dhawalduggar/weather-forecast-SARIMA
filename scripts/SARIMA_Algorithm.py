import pandas as pd
from statsmodels.tsa.statespace.sarimax import SARIMAX
from statsmodels.tsa.stattools import adfuller
import warnings

# Load dataset
try:
    data = pd.read_csv('weather_data_test1.csv')  # Replace with your CSV file path
except FileNotFoundError:
    print("Error: File 'weather_data_test1.csv' not found. Please provide the correct file path.")
    exit(1)

# Check for stationarity
def test_stationarity(timeseries):
    dftest = adfuller(timeseries, autolag='AIC')
    return dftest[1]

# Assuming 'Temperature' is the column with time series data
try:
    p_value = test_stationarity(data['Temperature'])
except KeyError:
    print("Error: Column 'Temperature' not found in the dataset. Please check the column name.")
    exit(1)

if p_value > 0.05:
    print("Data is non-stationary. Differencing needed.")
else:
    print("Data is stationary. No differencing needed.")

# Fit SARIMA model (p,d,q)(P,D,Q,s) values to be determined as per your dataset
try:
    model = SARIMAX(data['Temperature'], order=(1, 1, 1), seasonal_order=(1, 1, 1, 12))
    results_SARIMA = model.fit()

    # Predict future values
    predictions = results_SARIMA.get_forecast(steps=7)  # Predict next 7 values (1 week)

    # Extract predicted mean and confidence intervals
    predicted_mean = predictions.predicted_mean
    confidence_intervals = predictions.conf_int()

    # Create a DataFrame with predictions and confidence intervals
    predictions_df = pd.DataFrame({
        'Predicted_Avg_Temperature': predicted_mean,
        'Minimum Temperature': confidence_intervals.iloc[:, 0],
        'Maximum Temperature': confidence_intervals.iloc[:, 1]
    })

    # Save predictions to CSV
    predictions_df.to_csv('sarima_predictions.csv', index=False)
    print("Predictions saved to sarima_predictions.csv")
except Exception as e:
    print(f"Error: {e}")
    exit(1)
