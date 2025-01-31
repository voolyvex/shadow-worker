import wave
import math
import struct
import os

def create_directory_if_not_exists(path):
    if not os.path.exists(path):
        os.makedirs(path)

def generate_sine_wave(frequency, duration, amplitude=0.5, sample_rate=44100):
    n_samples = int(sample_rate * duration)
    samples = []
    
    for i in range(n_samples):
        t = float(i) / sample_rate
        sample = amplitude * math.sin(2.0 * math.pi * frequency * t)
        samples.append(sample)
    
    return samples

def apply_envelope(samples, attack=0.1, decay=0.2, sustain=0.7, release=0.2):
    n_samples = len(samples)
    envelope = []
    
    attack_samples = int(n_samples * attack)
    decay_samples = int(n_samples * decay)
    sustain_samples = int(n_samples * sustain)
    release_samples = n_samples - attack_samples - decay_samples - sustain_samples
    
    # Attack
    for i in range(attack_samples):
        envelope.append(i / attack_samples)
    
    # Decay
    for i in range(decay_samples):
        envelope.append(1.0 - (0.3 * i / decay_samples))
    
    # Sustain
    for i in range(sustain_samples):
        envelope.append(0.7)
    
    # Release
    for i in range(release_samples):
        envelope.append(0.7 * (1.0 - i / release_samples))
    
    return [s * e for s, e in zip(samples, envelope)]

def save_wave_file(filename, samples, sample_rate=44100):
    with wave.open(filename, 'w') as wave_file:
        n_channels = 1
        sample_width = 2
        
        wave_file.setnchannels(n_channels)
        wave_file.setsampwidth(sample_width)
        wave_file.setframerate(sample_rate)
        
        # Convert samples to bytes
        sample_data = []
        for sample in samples:
            # Convert to 16-bit integer
            sample = int(sample * 32767)
            sample_data.append(struct.pack('h', sample))
        
        wave_file.writeframes(b''.join(sample_data))

def generate_footstep():
    # Generate a short, low-frequency thud
    base_freq = 100
    duration = 0.15
    samples = generate_sine_wave(base_freq, duration)
    
    # Add some noise
    noise_samples = generate_sine_wave(base_freq * 4, duration, amplitude=0.2)
    samples = [s1 + s2 for s1, s2 in zip(samples, noise_samples)]
    
    # Apply envelope
    samples = apply_envelope(samples, attack=0.1, decay=0.3, sustain=0.4, release=0.2)
    
    return samples

def generate_ambient():
    # Generate a long, low-frequency drone
    duration = 5.0
    samples = []
    
    # Layer multiple frequencies
    freqs = [50, 100, 150, 200]
    amplitudes = [0.3, 0.2, 0.15, 0.1]
    
    for freq, amp in zip(freqs, amplitudes):
        wave_samples = generate_sine_wave(freq, duration, amplitude=amp)
        if not samples:
            samples = wave_samples
        else:
            samples = [s1 + s2 for s1, s2 in zip(samples, wave_samples)]
    
    # Normalize
    max_amplitude = max(abs(min(samples)), abs(max(samples)))
    samples = [s / max_amplitude * 0.7 for s in samples]
    
    return samples

def generate_interact():
    # Generate a short, high-frequency beep
    duration = 0.2
    samples = generate_sine_wave(440, duration, amplitude=0.3)  # A4 note
    
    # Add harmonic
    harmonic_samples = generate_sine_wave(880, duration, amplitude=0.15)  # A5 note
    samples = [s1 + s2 for s1, s2 in zip(samples, harmonic_samples)]
    
    # Apply envelope
    samples = apply_envelope(samples, attack=0.05, decay=0.1, sustain=0.7, release=0.15)
    
    return samples

def main():
    # Create output directory
    create_directory_if_not_exists("../resources/sounds")
    
    # Generate and save footstep sound
    footstep_samples = generate_footstep()
    save_wave_file("../resources/sounds/footstep.wav", footstep_samples)
    
    # Generate and save ambient sound
    ambient_samples = generate_ambient()
    save_wave_file("../resources/sounds/ambient.wav", ambient_samples)
    
    # Generate and save interact sound
    interact_samples = generate_interact()
    save_wave_file("../resources/sounds/interact.wav", interact_samples)

if __name__ == "__main__":
    main() 