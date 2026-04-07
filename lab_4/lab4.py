import numpy as np
import matplotlib.pyplot as plt
from matplotlib.widgets import Slider, Button, CheckButtons
from scipy.signal import butter, filtfilt

t = np.linspace(0, 10, 1000)

np.random.seed(42)
base_noise = np.random.normal(0, 1, len(t))

def harmonic_with_noise(amplitude, frequency, phase, noise_mean, noise_covariance, show_noise):
    y_pure = amplitude * np.sin(2 * np.pi * frequency * t + phase)
    
    noise = base_noise * np.sqrt(noise_covariance) + noise_mean
    
    if show_noise:
        y_noisy = y_pure + noise
    else:
        y_noisy = y_pure 
        
    return y_pure, y_noisy

def apply_filter(y_noisy, frequency):
    nyquist = 50.0  # fs/2 (100 Гц / 2)
    cutoff = min(frequency * 4.0, nyquist - 1.0) # Частота зрізу 
    if cutoff <= 0: 
        cutoff = 0.1
        
    b, a = butter(3, cutoff / nyquist, btype='low')
    y_filtered = filtfilt(b, a, y_noisy)
    return y_filtered

#GUI
fig, ax = plt.subplots(figsize=(10, 8))
plt.subplots_adjust(left=0.1, bottom=0.45) # Звільняємо місце знизу для слайдерів
ax.set_title('Аналіз та фільтрація гармонічного сигналу', fontsize=14)
ax.set_xlabel('Час (t)')
ax.set_ylabel('Амплітуда (y)')

#початкові параметри
init_amp = 1.0
init_freq = 0.5
init_phase = 0.0
init_n_mean = 0.0
init_n_cov = 0.1

#початкові дані
y_pure, y_noisy = harmonic_with_noise(init_amp, init_freq, init_phase, init_n_mean, init_n_cov, True)
y_filtered = apply_filter(y_noisy, init_freq)


line_noisy, = ax.plot(t, y_noisy, color='orange', alpha=0.8, label='Зашумлена гармоніка')
line_pure, = ax.plot(t, y_pure, color='blue', linestyle='--', linewidth=2, label='Чиста гармоніка')
line_filtered, = ax.plot(t, y_filtered, color='purple', linewidth=2.5, label='Відфільтрована')

ax.legend(loc='upper right')
ax.grid(True, linestyle=':', alpha=0.6)

axcolor = 'lightgoldenrodyellow'

#слайдери [left, bottom, width, height]
ax_amp   = plt.axes([0.15, 0.35, 0.65, 0.03], facecolor=axcolor)
ax_freq  = plt.axes([0.15, 0.30, 0.65, 0.03], facecolor=axcolor)
ax_phase = plt.axes([0.15, 0.25, 0.65, 0.03], facecolor=axcolor)
ax_nmean = plt.axes([0.15, 0.20, 0.65, 0.03], facecolor=axcolor)
ax_ncov  = plt.axes([0.15, 0.15, 0.65, 0.03], facecolor=axcolor)

s_amp   = Slider(ax_amp, 'Амплітуда', 0.1, 5.0, valinit=init_amp)
s_freq  = Slider(ax_freq, 'Частота', 0.1, 5.0, valinit=init_freq)
s_phase = Slider(ax_phase, 'Фаза', 0.0, 2 * np.pi, valinit=init_phase)
s_nmean = Slider(ax_nmean, 'Шум (Mean)', -2.0, 2.0, valinit=init_n_mean)
s_ncov  = Slider(ax_ncov, 'Шум (Var)', 0.0, 2.0, valinit=init_n_cov)

# чекбокси
ax_check = plt.axes([0.05, 0.02, 0.3, 0.1])
check = CheckButtons(ax_check, ['Показати шум', 'Показати відфільтровану', 'Показати чисту'], [True, True, True])

# кнопка Reset
ax_reset = plt.axes([0.8, 0.05, 0.1, 0.05])
btn_reset = Button(ax_reset, 'Reset', color=axcolor, hovercolor='0.975')

def update(val):
    #стани чекбоксів
    show_noise_checked = check.get_status()[0]
    show_filt_checked = check.get_status()[1]
    show_pure_checked = check.get_status()[2]
    
    #нові дані
    y_p, y_n = harmonic_with_noise(
        s_amp.val, s_freq.val, s_phase.val, s_nmean.val, s_ncov.val, show_noise_checked
    )
    y_f = apply_filter(y_n, s_freq.val)
    
    # оновдення даних ліній
    line_noisy.set_ydata(y_n)
    line_pure.set_ydata(y_p)
    line_filtered.set_ydata(y_f)
    
    #видимість ліній
    line_noisy.set_visible(show_noise_checked)
    line_filtered.set_visible(show_filt_checked)
    line_pure.set_visible(show_pure_checked)
    
    # Автомасштабування осі Y
    ax.relim()
    ax.autoscale_view()
    fig.canvas.draw_idle()

s_amp.on_changed(update)
s_freq.on_changed(update)
s_phase.on_changed(update)
s_nmean.on_changed(update)
s_ncov.on_changed(update)
check.on_clicked(lambda x: update(None))

# Reset
def reset(event):
    s_amp.reset()
    s_freq.reset()
    s_phase.reset()
    s_nmean.reset()
    s_ncov.reset()
    # Чекбокси залишаємо як є, або їх теж можна скинути вручну
btn_reset.on_clicked(reset)

plt.show()