import streamlit as st
import pandas as pd
import numpy as np
import plotly.express as px

st.set_page_config(page_title="Data Dashboard", layout="wide")

@st.cache_data 
def load_data():
    try:
        df = pd.read_csv('vhi_data.csv')
    except FileNotFoundError:
        np.random.seed(42)
        regions = ['Вінницька', 'Волинська', 'Дніпропетровська', 'Донецька', 'Житомирська', 'Київська']
        years = np.repeat(np.arange(2000, 2024), 52 * len(regions))
        weeks = np.tile(np.repeat(np.arange(1, 53), len(regions)), 24)
        reg_col = np.tile(regions, 24 * 52)
        
        df = pd.DataFrame({
            'Year': years,
            'Week': weeks,
            'Region': reg_col,
            'VCI': np.random.uniform(10, 90, len(years)),
            'TCI': np.random.uniform(10, 90, len(years)),
            'VHI': np.random.uniform(10, 90, len(years))
        })
    return df

df = load_data()

default_values = {
    'indicator': 'VHI',
    'region': df['Region'].unique()[0],
    'week_range': (1, 52),
    'year_range': (int(df['Year'].min()), int(df['Year'].max())),
    'sort_asc': False,
    'sort_desc': False
}

for key, value in default_values.items():
    if key not in st.session_state:
        st.session_state[key] = value

def reset_filters():
    for key, value in default_values.items():
        st.session_state[key] = value

st.title("Лабораторна робота №5")
col1, col2 = st.columns([1, 3]) 

with col1:
    st.header("Фільтри")
    
    indicator = st.selectbox("Оберіть показник:", ['VCI', 'TCI', 'VHI'], key='indicator')
    region = st.selectbox("Оберіть область:", df['Region'].unique(), key='region')
    
    week_range = st.slider("Інтервал тижнів:", 1, 52, key='week_range')
    year_range = st.slider("Інтервал років:", 
                           int(df['Year'].min()), int(df['Year'].max()), 
                           key='year_range')
    
    st.markdown("---")
    st.write("Сортування даних:")
    sort_asc = st.checkbox("За зростанням", key='sort_asc')
    sort_desc = st.checkbox("За спаданням", key='sort_desc')
    
    st.markdown("---")
    st.button("Reset (Скинути фільтри)", on_click=reset_filters, type="primary")

filtered_df = df[
    (df['Year'] >= year_range[0]) & (df['Year'] <= year_range[1]) &
    (df['Week'] >= week_range[0]) & (df['Week'] <= week_range[1])
]

region_df = filtered_df[filtered_df['Region'] == region].copy()
region_df['Date'] = region_df['Year'].astype(str) + " - Т" + region_df['Week'].astype(str)

if sort_asc and sort_desc:
    st.warning("⚠️ Увімкнено обидва чекбокси сортування! Дані відображаються у хронологічному порядку.")
elif sort_asc:
    region_df = region_df.sort_values(by=indicator, ascending=True)
elif sort_desc:
    region_df = region_df.sort_values(by=indicator, ascending=False)

with col2:
    tab1, tab2, tab3 = st.tabs(["📊 Таблиця даних", "📈 Графік області", "🌍 Порівняння областей"])
    
    with tab1:
        st.subheader(f"Дані {indicator} для області: {region}")
        st.dataframe(region_df[['Year', 'Week', indicator]], use_container_width=True)
        
    with tab2:
        st.subheader(f"Динаміка {indicator} ({year_range[0]}-{year_range[1]})")
        fig1 = px.line(region_df, x='Date', y=indicator, 
                       title=f"{indicator} у {region} області",
                       markers=True, color_discrete_sequence=["#1f9eb4"])
        fig1.update_xaxes(title_text="Рік - Тиждень", tickangle=45)
        st.plotly_chart(fig1, use_container_width=True)
        
    with tab3:
        st.subheader(f"Порівняння середнього {indicator} між областями")
        compare_df = filtered_df.groupby('Region')[indicator].mean().reset_index()
        compare_df = compare_df.sort_values(by=indicator, ascending=False)
        
        compare_df['Color'] = np.where(compare_df['Region'] == region, 'Обрана область', 'Інші області')
        
        fig2 = px.bar(compare_df, x='Region', y=indicator, color='Color',
                      color_discrete_map={'Обрана область': "#ae06ab", 'Інші області': "#1fb4a8"},
                      title=f"Середнє значення {indicator} за вказаний період")
        fig2.update_xaxes(title_text="Область", tickangle=45)
        st.plotly_chart(fig2, use_container_width=True)