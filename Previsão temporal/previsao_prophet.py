

import pandas as pd
from prophet import Prophet


dftemp = pd.read_csv('temperatura.csv')
dfumd = pd.read_csv('umidade.csv')
dfrad = pd.read_csv('radiacao.csv')

nhoras_prever = 31
nhoras_csv = 24
# Previsao Temperatura

m=Prophet(changepoint_prior_scale=0.001).fit(dftemp)
future = m.make_future_dataframe(periods=nhoras_prever, freq='H')
forecast_temp = m.predict(future)

#fig1 = m.plot(forecast)
#fig2 = m.plot_components(forecast)

dados_previsao = forecast_temp.tail(nhoras_csv)[['ds', 'yhat']]
dados_previsao.rename(columns = {'yhat': 'TEM_INS'}, inplace=True)
dados_previsao['TEM_INS'] = dados_previsao['TEM_INS'].round(decimals = 1);

# Previsao Umidade

m2=Prophet(changepoint_prior_scale=0.001).fit(dfumd)
future = m2.make_future_dataframe(periods=nhoras_prever, freq='H')
forecast_umd = m2.predict(future)

dados_previsao['UMD_INS'] = forecast_umd.tail(nhoras_csv)['yhat'].round(decimals = 1)

# Previsao Radiação

m3=Prophet(changepoint_prior_scale=0.001).fit(dfrad)
future = m3.make_future_dataframe(periods=nhoras_prever, freq='H')
forecast_rad = m3.predict(future)

dados_previsao['RAD_GLO'] = forecast_rad.tail(nhoras_csv)['yhat'].round(decimals = 1)


print(dados_previsao)

dados_previsao[['DT_MEDICAO', 'HR_MEDICAO']]=dados_previsao['ds'].astype(str).str.split(" ", 1, expand=True)
print(dados_previsao['HR_MEDICAO'])
dados_previsao['HR_MEDICAO']=dados_previsao['HR_MEDICAO'].str[slice(2)]+"00"

dados_feito = dados_previsao[['DT_MEDICAO', 'HR_MEDICAO', 'TEM_INS', 'UMD_INS', 'RAD_GLO']]
dados_feito.loc[dados_feito['RAD_GLO'] < 0, 'RAD_GLO'] = 0
dados_feito.loc[dados_feito['UMD_INS'] > 100, 'UMD_INS'] = 100
dados_feito.reset_index(drop=True, inplace=True)
print(dados_feito)

dados_feito.to_csv('previsao_feito.csv')
