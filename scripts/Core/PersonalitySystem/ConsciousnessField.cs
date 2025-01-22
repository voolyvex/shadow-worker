using UnityEngine;

namespace ShadowWorker.Core
{
    [RequireComponent(typeof(CircleCollider2D))]
    public class ConsciousnessField : MonoBehaviour, IConsciousnessField
    {
        [Header("Field Properties")]
        [SerializeField] private float radius = 5f;
        [SerializeField] private float baseIntensity = 1f;
        [SerializeField] private AnimationCurve falloffCurve = AnimationCurve.EaseInOut(0f, 1f, 1f, 0f);
        
        [Header("Influence Settings")]
        [SerializeField] private float consciousnessInfluence = 0.1f;
        [SerializeField] private float realityInfluence = 0.1f;
        
        private CircleCollider2D fieldCollider;
        private Vector2 Position => transform.position;
        private float currentIntensity;

        Vector2 IConsciousnessField.Position => Position;
        float IConsciousnessField.Radius => radius;
        float IConsciousnessField.Intensity => currentIntensity;

        private void Awake()
        {
            fieldCollider = GetComponent<CircleCollider2D>();
            fieldCollider.isTrigger = true;
            fieldCollider.radius = radius;
            currentIntensity = baseIntensity;
        }

        private void OnTriggerEnter2D(Collider2D other)
        {
            if (other.TryGetComponent<PersonalityProfile>(out var profile))
            {
                profile.AddConsciousnessField(this);
            }
        }

        private void OnTriggerExit2D(Collider2D other)
        {
            if (other.TryGetComponent<PersonalityProfile>(out var profile))
            {
                profile.RemoveConsciousnessField(this);
            }
        }

        public void AffectEntity(IPsychologicalEntity entity)
        {
            if (entity is PersonalityProfile profile)
            {
                float distance = Vector2.Distance(Position, profile.transform.position);
                float normalizedDistance = Mathf.Clamp01(distance / radius);
                float influence = falloffCurve.Evaluate(1f - normalizedDistance) * currentIntensity;

                // Apply influence based on distance and intensity
                profile.ModifyConsciousnessIntegration(consciousnessInfluence * influence * Time.deltaTime);
                profile.ModifyRealityIntegration(realityInfluence * influence * Time.deltaTime);

                // Modify DSM traits based on field properties
                ModifyEntityTraits(profile, influence);
            }
        }

        private void ModifyEntityTraits(PersonalityProfile profile, float influence)
        {
            // Example trait modifications based on field properties
            // These can be customized based on the specific field type
            float traitInfluence = influence * Time.deltaTime * 0.1f;
            
            profile.ModifyDSMTrait("dissociation", traitInfluence);
            profile.ModifyDSMTrait("reality_distortion", traitInfluence);
        }

        public void SetIntensity(float intensity)
        {
            currentIntensity = Mathf.Max(0f, intensity);
        }

        public void SetRadius(float newRadius)
        {
            radius = Mathf.Max(0.1f, newRadius);
            if (fieldCollider != null)
            {
                fieldCollider.radius = radius;
            }
        }

        private void OnDrawGizmos()
        {
            // Visualize the consciousness field in the editor
            Gizmos.color = new Color(0.5f, 0.8f, 1f, 0.2f);
            Gizmos.DrawWireSphere(transform.position, radius);
            
            // Draw influence direction
            Gizmos.color = new Color(0.5f, 0.8f, 1f, 0.4f);
            Vector3 forward = transform.forward * radius;
            Gizmos.DrawLine(transform.position, transform.position + forward);
        }
    }
} 