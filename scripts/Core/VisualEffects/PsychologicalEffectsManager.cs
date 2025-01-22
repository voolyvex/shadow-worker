using UnityEngine;
using UnityEngine.Rendering;
using UnityEngine.Rendering.Universal;
using System.Collections.Generic;
using ShadowWorker.Core;

namespace ShadowWorker.Effects
{
    [RequireComponent(typeof(Volume))]
    public class PsychologicalEffectsManager : MonoBehaviour
    {
        [Header("References")]
        [SerializeField] private PersonalityProfile targetProfile;
        [SerializeField] private Volume postProcessVolume;
        
        [Header("Effect Settings")]
        [SerializeField] private float transitionSpeed = 2f;
        [SerializeField] private float maxDistortion = 1f;
        [SerializeField] private float maxChromaticAberration = 1f;
        [SerializeField] private float maxVignette = 0.5f;
        
        // Post-processing effect components
        private LensDistortion lensDistortion;
        private ChromaticAberration chromaticAberration;
        private Vignette vignette;
        private ColorAdjustments colorAdjustments;
        
        // Effect parameters for each resonance state
        private Dictionary<ResonanceState, EffectParameters> stateEffects;
        
        private void Awake()
        {
            InitializeComponents();
            SetupStateEffects();
        }

        private void Start()
        {
            if (targetProfile == null)
            {
                targetProfile = FindObjectOfType<PersonalityProfile>();
            }
        }

        private void Update()
        {
            if (targetProfile == null) return;
            UpdateVisualEffects();
        }

        private void InitializeComponents()
        {
            if (postProcessVolume == null)
            {
                postProcessVolume = GetComponent<Volume>();
            }

            // Get or add post-processing components
            postProcessVolume.profile.TryGet(out lensDistortion);
            postProcessVolume.profile.TryGet(out chromaticAberration);
            postProcessVolume.profile.TryGet(out vignette);
            postProcessVolume.profile.TryGet(out colorAdjustments);

            // Create components if they don't exist
            if (lensDistortion == null)
                postProcessVolume.profile.Add<LensDistortion>();
            if (chromaticAberration == null)
                postProcessVolume.profile.Add<ChromaticAberration>();
            if (vignette == null)
                postProcessVolume.profile.Add<Vignette>();
            if (colorAdjustments == null)
                postProcessVolume.profile.Add<ColorAdjustments>();
        }

        private void SetupStateEffects()
        {
            stateEffects = new Dictionary<ResonanceState, EffectParameters>
            {
                {
                    ResonanceState.Grounded,
                    new EffectParameters
                    {
                        distortion = 0f,
                        chromaticAberration = 0f,
                        vignette = 0f,
                        saturation = 0f,
                        colorTint = Color.white
                    }
                },
                {
                    ResonanceState.Resonating,
                    new EffectParameters
                    {
                        distortion = 0.2f,
                        chromaticAberration = 0.2f,
                        vignette = 0.2f,
                        saturation = 0.2f,
                        colorTint = new Color(1f, 0.9f, 0.9f)
                    }
                },
                {
                    ResonanceState.Synchronized,
                    new EffectParameters
                    {
                        distortion = 0.4f,
                        chromaticAberration = 0.4f,
                        vignette = 0.3f,
                        saturation = 0.4f,
                        colorTint = new Color(0.9f, 0.9f, 1f)
                    }
                },
                {
                    ResonanceState.Transcendent,
                    new EffectParameters
                    {
                        distortion = 0.6f,
                        chromaticAberration = 0.6f,
                        vignette = 0.4f,
                        saturation = 0.6f,
                        colorTint = new Color(0.8f, 0.9f, 1f)
                    }
                },
                {
                    ResonanceState.Unified,
                    new EffectParameters
                    {
                        distortion = 0.8f,
                        chromaticAberration = 0.8f,
                        vignette = 0.5f,
                        saturation = 0.8f,
                        colorTint = new Color(0.7f, 0.8f, 1f)
                    }
                }
            };
        }

        private void UpdateVisualEffects()
        {
            if (!stateEffects.TryGetValue(targetProfile.CurrentResonanceState, out var targetEffects))
                return;

            float resonanceIntensity = targetProfile.GetResonanceIntensity();
            
            // Update lens distortion
            if (lensDistortion != null)
            {
                lensDistortion.intensity.value = Mathf.Lerp(
                    lensDistortion.intensity.value,
                    targetEffects.distortion * maxDistortion * resonanceIntensity,
                    Time.deltaTime * transitionSpeed
                );
            }

            // Update chromatic aberration
            if (chromaticAberration != null)
            {
                chromaticAberration.intensity.value = Mathf.Lerp(
                    chromaticAberration.intensity.value,
                    targetEffects.chromaticAberration * maxChromaticAberration * resonanceIntensity,
                    Time.deltaTime * transitionSpeed
                );
            }

            // Update vignette
            if (vignette != null)
            {
                vignette.intensity.value = Mathf.Lerp(
                    vignette.intensity.value,
                    targetEffects.vignette * maxVignette * resonanceIntensity,
                    Time.deltaTime * transitionSpeed
                );
            }

            // Update color adjustments
            if (colorAdjustments != null)
            {
                colorAdjustments.saturation.value = Mathf.Lerp(
                    colorAdjustments.saturation.value,
                    targetEffects.saturation * 100f * resonanceIntensity,
                    Time.deltaTime * transitionSpeed
                );
            }
        }

        private struct EffectParameters
        {
            public float distortion;
            public float chromaticAberration;
            public float vignette;
            public float saturation;
            public Color colorTint;
        }
    }
} 