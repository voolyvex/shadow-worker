using UnityEngine;
using System;
using System.Collections.Generic;

namespace ShadowWorker.Core
{
    [RequireComponent(typeof(Collider2D))]
    public class PersonalityProfile : MonoBehaviour, IPsychologicalEntity
    {
        [Header("Core Traits")]
        [SerializeField] private EnneagramType primaryType;
        [SerializeField] private DSMDimensions dsmTraits;
        [SerializeField] private float baseResonanceIntensity = 1f;
        
        [Header("State Management")]
        [SerializeField] private ResonanceState currentResonanceState;
        [SerializeField] private PerceptionLayer currentPerceptionLayer;
        
        [Header("Integration")]
        [SerializeField, Range(0, 1)] private float consciousnessIntegration;
        [SerializeField, Range(0, 1)] private float realityIntegration;

        private Dictionary<ResonanceState, float> resonanceThresholds;
        private List<IConsciousnessField> activeFields;

        public ResonanceState CurrentResonanceState => currentResonanceState;
        public PerceptionLayer CurrentPerceptionLayer => currentPerceptionLayer;

        private void Awake()
        {
            InitializePersonality();
            activeFields = new List<IConsciousnessField>();
            SetupResonanceThresholds();
        }

        private void InitializePersonality()
        {
            // Initialize with default values
            currentResonanceState = ResonanceState.Grounded;
            currentPerceptionLayer = PerceptionLayer.Surface;
            consciousnessIntegration = 0f;
            realityIntegration = 0f;
        }

        private void SetupResonanceThresholds()
        {
            resonanceThresholds = new Dictionary<ResonanceState, float>
            {
                { ResonanceState.Grounded, 0.0f },
                { ResonanceState.Resonating, 0.25f },
                { ResonanceState.Synchronized, 0.5f },
                { ResonanceState.Transcendent, 0.75f },
                { ResonanceState.Unified, 0.95f }
            };
        }

        public void UpdatePsychologicalState(float deltaTime)
        {
            UpdateIntegrationLevels(deltaTime);
            UpdateResonanceState();
            UpdatePerceptionLayer();
            ProcessActiveFields();
        }

        private void UpdateIntegrationLevels(float deltaTime)
        {
            // Integration levels naturally decay over time
            consciousnessIntegration = Mathf.Max(0, consciousnessIntegration - (deltaTime * 0.1f));
            realityIntegration = Mathf.Max(0, realityIntegration - (deltaTime * 0.05f));
        }

        private void UpdateResonanceState()
        {
            float totalIntegration = (consciousnessIntegration + realityIntegration) / 2f;
            
            foreach (var threshold in resonanceThresholds)
            {
                if (totalIntegration >= threshold.Value)
                {
                    currentResonanceState = threshold.Key;
                }
                else break;
            }
        }

        private void UpdatePerceptionLayer()
        {
            // Perception layer is influenced by DSM traits and resonance state
            float perceptionScore = (dsmTraits.dissociation + dsmTraits.reality_distortion) / 2f;
            perceptionScore = Mathf.Lerp(perceptionScore, 1f, consciousnessIntegration);

            currentPerceptionLayer = perceptionScore switch
            {
                float p when p >= 0.8f => PerceptionLayer.Transformative,
                float p when p >= 0.6f => PerceptionLayer.Integrated,
                float p when p >= 0.4f => PerceptionLayer.Collective,
                float p when p >= 0.2f => PerceptionLayer.Subconscious,
                _ => PerceptionLayer.Surface
            };
        }

        private void ProcessActiveFields()
        {
            foreach (var field in activeFields)
            {
                field.AffectEntity(this);
            }
        }

        public float GetResonanceIntensity()
        {
            return baseResonanceIntensity * (1f + consciousnessIntegration);
        }

        public void AddConsciousnessField(IConsciousnessField field)
        {
            if (!activeFields.Contains(field))
            {
                activeFields.Add(field);
            }
        }

        public void RemoveConsciousnessField(IConsciousnessField field)
        {
            activeFields.Remove(field);
        }

        // Integration methods
        public void ModifyConsciousnessIntegration(float amount)
        {
            consciousnessIntegration = Mathf.Clamp01(consciousnessIntegration + amount);
        }

        public void ModifyRealityIntegration(float amount)
        {
            realityIntegration = Mathf.Clamp01(realityIntegration + amount);
        }

        // DSM trait modification
        public void ModifyDSMTrait(string traitName, float amount)
        {
            switch (traitName.ToLower())
            {
                case "anxiety":
                    dsmTraits.anxiety = Mathf.Clamp01(dsmTraits.anxiety + amount);
                    break;
                case "depression":
                    dsmTraits.depression = Mathf.Clamp01(dsmTraits.depression + amount);
                    break;
                case "dissociation":
                    dsmTraits.dissociation = Mathf.Clamp01(dsmTraits.dissociation + amount);
                    break;
                case "reality_distortion":
                    dsmTraits.reality_distortion = Mathf.Clamp01(dsmTraits.reality_distortion + amount);
                    break;
                case "emotional_regulation":
                    dsmTraits.emotional_regulation = Mathf.Clamp01(dsmTraits.emotional_regulation + amount);
                    break;
            }
        }
    }
} 