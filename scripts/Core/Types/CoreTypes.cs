using UnityEngine;

namespace ShadowWorker.Core
{
    public enum EnneagramType
    {
        Reformer = 1,
        Helper = 2,
        Achiever = 3,
        Individualist = 4,
        Investigator = 5,
        Loyalist = 6,
        Enthusiast = 7,
        Challenger = 8,
        Peacemaker = 9
    }

    public enum ResonanceState
    {
        Grounded,
        Resonating,
        Synchronized,
        Transcendent,
        Unified
    }

    public enum PerceptionLayer
    {
        Surface,
        Subconscious,
        Collective,
        Integrated,
        Transformative
    }

    [System.Serializable]
    public struct DSMDimensions
    {
        [Range(0, 1)] public float anxiety;
        [Range(0, 1)] public float depression;
        [Range(0, 1)] public float dissociation;
        [Range(0, 1)] public float reality_distortion;
        [Range(0, 1)] public float emotional_regulation;
    }

    public interface IPsychologicalEntity
    {
        ResonanceState CurrentResonanceState { get; }
        PerceptionLayer CurrentPerceptionLayer { get; }
        void UpdatePsychologicalState(float deltaTime);
        float GetResonanceIntensity();
    }

    public interface IConsciousnessField
    {
        Vector2 Position { get; }
        float Radius { get; }
        float Intensity { get; }
        void AffectEntity(IPsychologicalEntity entity);
    }
} 