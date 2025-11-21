import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit
from scipy.signal import find_peaks
from scipy.interpolate import CubicSpline
import pywt
import os

ellipsoids = ['CGCS2000', 'IE1975', 'WGS84', 'Krasovski']

def sin_module(x, a, b, c):
    return a * np.sin((2 * np.pi / b) * x + c) 

def fit_residual_sin(verbose:bool = True): 
    fmt: str = 'meridian_arc_bottom_residual_len_div_1e7_{}.dat'
    dir: str = '../../test_package/build/dat/meridian_arc'
    
    true_val = []
    fit_results = []
    lens = []
    opts = []
    
    for e in ellipsoids:
        fname: str = os.path.join(dir, fmt.format(e))
        dat = pd.read_csv(fname, sep=" ", header=None)
        
        len_div = dat.iloc[:,0]
        residual = dat.iloc[:,1]
        
        init = [1.3, 1]
        opt, cov = curve_fit(sin_module, len_div, residual)
        a, b, c = opt
        
        fit = sin_module(len_div, a, b, c)
        fit_results.append(fit)
        true_val.append(residual)
        lens.append(len_div)
        opts.append(opt)
    
            
    if verbose:
        fig, axes = plt.subplots(2,2)
    
    new_residual = []
    
    for i in range(0,4):
        new_residual.append(true_val[i] - fit_results[i])
    
    if verbose:
        fig, axes = plt.subplots(2,2)
        for i in range(0,4):
            c = int(i % 2)
            r = int((i - c) / 2)
            subf = axes[r, c]
            # subf.plot(lens[i], true_val[i], '--b', label='True')
            # subf.plot(lens[i], fit_results[i], '-r', label='Fit')
            subf.plot(lens[i], new_residual[i], '-b', label='Residual')
            subf.set_title(ellipsoids[i])
            subf.set_xlabel('len / 1e7')
            subf.set_ylabel('residual')
            subf.legend()
            subf.grid(True)
            a,b,c = opts[i]
            print(ellipsoids[i]+f':\n{a} {b} {c}')
        plt.show()
    
    return lens, new_residual

def double_gaussian(x, offset, amp1, center1, sigma1, amp2, center2, sigma2):
    return (offset + 
            amp1 * np.exp(-0.5 * ((x - center1) / sigma1) ** 2) +
            amp2 * np.exp(-0.5 * ((x - center2) / sigma2) ** 2))

def estimate_double_gaussian_initial_params(x_data, y_data):
    offset_init = np.mean([y_data.min(), y_data.max()])
    
    peaks, _ = find_peaks(y_data, height=np.mean(y_data))  
    
    if len(peaks) >= 2:
        peak1_idx = peaks[0]
        peak2_idx = peaks[1] if len(peaks) > 1 else peaks[0] 
        
        center1_init = x_data[peak1_idx]
        center2_init = x_data[peak2_idx]
        
        amp1_init = y_data[peak1_idx] - offset_init
        amp2_init = y_data[peak2_idx] - offset_init
    else:
        center1_init = x_data.min() + (x_data.max() - x_data.min()) * 0.3  
        center2_init = x_data.min() + (x_data.max() - x_data.min()) * 0.7  
        amp1_init = y_data.std()
        amp2_init = y_data.std()
    
    x_range = x_data.max() - x_data.min()
    sigma1_init = x_range / 10
    sigma2_init = x_range / 10
    
    return [offset_init, amp1_init, center1_init, sigma1_init, 
            amp2_init, center2_init, sigma2_init]

def fit_residual_double_gaussian(xs, residuals, verbose:bool=False):    
    new_residual = []
    lens = []
    fits = []
    true_val = []
    opts = []
    
    for i in range(0,4):
        x = xs[i]
        r = residuals[i]
        
        lens.append(x)
        true_val.append(r)
        
        init = estimate_double_gaussian_initial_params(x, r)
        opt, cov = curve_fit(double_gaussian, x, r, p0=init)
        opts.append(opt)
        off, a1, c1, s1, a2, c2, s2 = opt
        fits.append(double_gaussian(x, off, a1, c1, s1, a2, c2, s2))
        new_residual.append(true_val[i]-fits[i])
    
    if verbose:
        fig, axes = plt.subplots(2,2)
        for i in range(0,4):
            c = int(i % 2)
            r = int((i - c) / 2)
            subf = axes[r, c]
            subf.plot(lens[i], true_val[i], '--b', label='True')
            subf.plot(lens[i], fits[i], '-r', label='Fit')
            subf.plot(lens[i], new_residual[i], '-b', label='Residual')
            subf.set_title(ellipsoids[i])
            subf.set_xlabel('len / 1e7')
            subf.set_ylabel('residual')
            subf.legend()
            subf.grid(True)
            off, a1, c1, s1, a2, c2, s2 = opts[i]
            print(ellipsoids[i]+f':offset = {off}, amp1 = {a1}, center1 = {c1}, sigma1 = {s1}, amp2 = {a2}, center2 = {c2}, sigma2 = {s2}')
        plt.show()
    
    return lens, new_residual

def fit_residual_fft_decomposition(xs, residuals, verbose:bool=False):
    """
    使用FFT将残差分解为频域成分
    """
    new_residual = []
    lens = []
    fits = []
    true_val = []
    fft_results = []
    
    for i in range(0,4):
        x = xs[i]
        r = residuals[i]
        
        lens.append(x)
        true_val.append(r)
        
        # 执行FFT
        fft_coeffs = np.fft.fft(r)
        freqs = np.fft.fftfreq(len(r), d=(x[1]-x[0]) if len(x) > 1 else 1)  # 频率
        
        # 保留主要频率成分（去除高频噪声）
        # 计算能量谱
        power_spectrum = np.abs(fft_coeffs)**2
        
        # 找到主要频率成分（例如，保留能量最大的前N个）
        n_main_components = min(len(fft_coeffs)//4, 50)  # 保留前25%或最多50个成分
        
        # 获取能量最大的频率索引
        main_indices = np.argsort(power_spectrum)[-n_main_components:]
        
        # 构造主要成分的IFFT
        fft_filtered = np.zeros_like(fft_coeffs)
        fft_filtered[main_indices] = fft_coeffs[main_indices]
        
        # 逆FFT得到拟合结果
        fitted_values = np.real(np.fft.ifft(fft_filtered))
        
        # 存储FFT结果
        fft_results.append({
            'coeffs': fft_coeffs,
            'freqs': freqs,
            'power_spectrum': power_spectrum,
            'filtered_coeffs': fft_filtered
        })
        
        fits.append(fitted_values)
        new_residual.append(r - fitted_values)
    
    if verbose:
        fig, axes = plt.subplots(2, 2)
        
        for i in range(0,4):
            c = int(i % 2)
            r = int((i - c) / 2)
            subf = axes[r, c]
            # subf.plot(lens[i], true_val[i], '--b', label='True')
            # subf.plot(lens[i], fits[i], '-r', label='FFT Gaussian High-pass')
            subf.plot(lens[i], new_residual[i], '-g', label='Residual (Low-freq)')
            subf.set_title(f'{ellipsoids[i]}')
            subf.set_xlabel('len / 1e7')
            subf.set_ylabel('residual')
            subf.legend()
            subf.grid(True)
        
        plt.tight_layout()
        plt.show()
    
    return lens, new_residual

def fit_residual_fft_gaussian_highpass(xs, residuals, sigma_ratio=0.05, verbose:bool=False):
    """
    使用FFT + 高斯高通滤波器
    """
    new_residual = []
    lens = []
    fits = []
    true_val = []
    
    for i in range(0,4):
        x = xs[i]
        r = residuals[i]
        
        lens.append(x)
        true_val.append(r)
        
        # 执行FFT
        fft_coeffs = np.fft.fft(r)
        n = len(fft_coeffs)
        
        # 创建高斯高通滤波器
        freq_indices = np.arange(n)
        freq_indices = np.where(freq_indices > n//2, freq_indices - n, freq_indices)
        
        # 高斯滤波器（高通：1 - 低通滤波器）
        sigma = sigma_ratio * n
        gaussian_lowpass = np.exp(-0.5 * (freq_indices / sigma) ** 2)
        gaussian_highpass = 1 - gaussian_lowpass
        
        # 应用滤波器
        fft_filtered = fft_coeffs * gaussian_highpass
        
        # 逆FFT
        fitted_values = np.real(np.fft.ifft(fft_filtered))
        
        fits.append(fitted_values)
        new_residual.append(r - fitted_values)
    
    if verbose:
        fig, axes = plt.subplots(2, 2, figsize=(15, 10))
        
        for i in range(0,4):
            c = int(i % 2)
            r = int((i - c) / 2)
            subf = axes[r, c]
            subf.plot(lens[i], true_val[i], '--b', label='True')
            subf.plot(lens[i], fits[i], '-r', label='FFT Gaussian High-pass')
            subf.plot(lens[i], new_residual[i], '-g', label='Residual (Low-freq)')
            subf.set_title(f'{ellipsoids[i]}')
            subf.set_xlabel('len / 1e7')
            subf.set_ylabel('residual')
            subf.legend()
            subf.grid(True)
        plt.show()
    
    return lens, new_residual

def fit_residual_fft_gaussian_lowpass(xs, residuals, sigma_ratio=0.05, verbose:bool=False):
    """
    使用FFT + 高斯低通滤波器进行平滑拟合
    sigma_ratio: 高斯滤波器的标准差相对于总频率范围的比例
    """
    new_residual = []
    lens = []
    fits = []
    true_val = []
    
    for i in range(0,4):
        x = xs[i]
        r = residuals[i]
        
        lens.append(x)
        true_val.append(r)
        
        # 执行FFT
        fft_coeffs = np.fft.fft(r)
        n = len(fft_coeffs)
        
        # 创建高斯低通滤波器
        freq_indices = np.arange(n)
        # 将频率中心化
        freq_indices = np.where(freq_indices > n//2, freq_indices - n, freq_indices)
        
        # 高斯滤波器
        sigma = sigma_ratio * n
        gaussian_filter = np.exp(-0.5 * (freq_indices / sigma) ** 2)
        
        # 应用滤波器
        fft_filtered = fft_coeffs * gaussian_filter
        
        # 逆FFT
        fitted_values = np.real(np.fft.ifft(fft_filtered))
        
        fits.append(fitted_values)
        new_residual.append(r - fitted_values)
    
    if verbose:
        fig, axes = plt.subplots(2, 2)
        
        for i in range(0,4):
            c = int(i % 2)
            r = int((i - c) / 2)
            subf = axes[r, c]
            subf.plot(lens[i], true_val[i], '--b', label='True')
            subf.plot(lens[i], fits[i], '-r', label='FFT Gaussian Filter Fit')
            subf.plot(lens[i], new_residual[i], '-g', label='Residual')
            subf.set_title(f'{ellipsoids[i]}')
            subf.set_xlabel('len / 1e7')
            subf.set_ylabel('residual')
            subf.legend()
            subf.grid(True)
        plt.show()
    
    return lens, new_residual

def fit_residual_wavelet(xs, residuals, wavelet='db4', level=3, verbose:bool=False):
    """
    使用小波变换进行多尺度分析
    """
    
    new_residual = []
    lens = []
    fits = []
    true_val = []
    
    for i in range(0,4):
        x = xs[i]
        r = residuals[i]
        
        lens.append(x)
        true_val.append(r)
        
        # 小波分解
        coeffs = pywt.wavedec(r, wavelet, level=level)
        
        # 保留主要成分，滤除噪声
        # 只保留前几个最大系数
        threshold = np.std(coeffs[-1]) * 0.5  # 阈值
        coeffs_thresh = [coeffs[0]]  # 保留低频部分
        for detail_coeff in coeffs[1:]:
            # 软阈值处理
            coeffs_thresh.append(pywt.threshold(detail_coeff, threshold, mode='soft'))
        
        # 重构
        fitted_values = pywt.waverec(coeffs_thresh, wavelet)
        # 确保长度匹配
        fitted_values = fitted_values[:len(r)]
        
        cs = CubicSpline(x,fitted_values)
        fitted_values_sp = cs(x)
        
        fits.append(fitted_values_sp)
        new_residual.append(r - fitted_values_sp)
        
        coefficients = {
            'x_knots': x.tolist(),  # 节点
            'y_knots': r.tolist(),  # 节点处的值
            'spline_coeffs': {  # 每个区间的系数 [d, c, b, a]
                'd': cs.c[0, :].tolist(),  # 三次项系数
                'c': cs.c[1, :].tolist(),  # 二次项系数  
                'b': cs.c[2, :].tolist(),  # 一次项系数
                'a': cs.c[3, :].tolist()   # 常数项系数
            },
            'x_range': [float(x.min()), float(x.max())]
        }
        
        def print_coefficients(coeff):            
            print("{")
            
            print(f'.x_knots {coefficients['x_knots']},\n'.replace('[', '{').replace(']', '}'))
            print(f'.y_knots {coefficients['y_knots']},\n'.replace('[', '{').replace(']', '}'))

            print(f'.a {coefficients['spline_coeffs']['a']},\n'.replace('[', '{').replace(']', '}'))
            print(f'.b {coefficients['spline_coeffs']['b']},\n'.replace('[', '{').replace(']', '}'))
            print(f'.c {coefficients['spline_coeffs']['c']},\n'.replace('[', '{').replace(']', '}'))
            print(f'.d {coefficients['spline_coeffs']['d']},\n'.replace('[', '{').replace(']', '}'))

            print(f'.x_max = {coefficients['x_range'][0]},\n.x_min = {coefficients['x_range'][1]},\n')
            
            print(f'.n_intervals = {len(coefficients['spline_coeffs']['a'])}')
            
            print("}")
        
        # print(ellipsoids[i])
        # print_coefficients(coefficients)
        print(np.max(fitted_values_sp))
        
    if verbose:
        fig, axes = plt.subplots(2,2, figsize=(15, 10))
        for i in range(0,4):
            c = int(i % 2)
            r = int((i - c) / 2)
            subf = axes[r, c]
            subf.plot(lens[i], true_val[i], '--b', label='True')
            subf.plot(lens[i], fits[i], '-r', label='Wavelet Fit (Spline)')
            subf.plot(lens[i], new_residual[i], '-g', label='Residual')
            subf.set_title(ellipsoids[i])
            subf.set_xlabel('len / 1e7')
            subf.set_ylabel('residual')
            subf.legend()
            subf.grid(True)
        plt.show()
    
    return lens, new_residual

def main() -> None:
    lens_sin, residual_sin = fit_residual_sin(False)
    lens_doub, residual_doub = fit_residual_double_gaussian(lens_sin, residual_sin, False)
    
    # fit_residual_fft_decomposition(lens_doub, residual_doub, True)
    
    # lens_fft_hp, residual_fft_hp = fit_residual_fft_gaussian_highpass(lens_doub, residual_doub, verbose=False)
    # lens_fft_lp, residual_fft_lp = fit_residual_fft_gaussian_lowpass(lens_fft_hp, residual_fft_hp, verbose=True)
        
    fit_residual_wavelet(lens_doub, residual_doub, verbose=True)
    
    
if __name__ == "__main__":
    main()