using UnityEngine;
using UnityEngine.UI;
using TMPro;
using ShadowWorker.Core;
using ShadowWorker.Player;

namespace ShadowWorker.UI
{
    public class PsychologicalStateUI : MonoBehaviour
    {
        [Header("References")]
        [SerializeField] private PersonalityProfile playerProfile;
        [SerializeField] private PlayerController playerController;
        
        [Header("UI Elements")]
        [SerializeField] private Slider staminaBar;
        [SerializeField] private Image resonanceIndicator;
        [SerializeField] private TextMeshProUGUI stateText;
        [SerializeField] private TextMeshProUGUI perceptionText;
        
        [Header("Visual Settings")]
        [SerializeField] private Color[] resonanceColors;
        [SerializeField] private float pulseSpeed = 1f;
        [SerializeField] private float minAlpha = 0.5f;
        [SerializeField] private float maxAlpha = 1f;
        
        private float pulseTime;

        private void Start()
        {
            if (playerProfile == null)
            {
                playerProfile = FindObjectOfType<PersonalityProfile>();
            }
            
            if (playerController == null)
            {
                playerController = FindObjectOfType<PlayerController>();
            }

            // Initialize UI
            UpdateUI();
        }

        private void Update()
        {
            UpdateUI();
            PulseResonanceIndicator();
        }

        private void UpdateUI()
        {
            if (playerProfile == null || playerController == null) return;

            // Update stamina bar
            if (staminaBar != null)
            {
                staminaBar.value = playerController.CurrentStamina;
            }

            // Update resonance indicator
            if (resonanceIndicator != null && resonanceColors.Length > 0)
            {
                int colorIndex = (int)playerProfile.CurrentResonanceState;
                colorIndex = Mathf.Clamp(colorIndex, 0, resonanceColors.Length - 1);
                resonanceIndicator.color = resonanceColors[colorIndex];
            }

            // Update state text
            if (stateText != null)
            {
                stateText.text = $"State: {playerProfile.CurrentResonanceState}";
            }

            // Update perception text
            if (perceptionText != null)
            {
                perceptionText.text = $"Perception: {playerProfile.CurrentPerceptionLayer}";
            }
        }

        private void PulseResonanceIndicator()
        {
            if (resonanceIndicator == null) return;

            pulseTime += Time.deltaTime * pulseSpeed;
            float alpha = Mathf.Lerp(minAlpha, maxAlpha, (Mathf.Sin(pulseTime) + 1f) * 0.5f);
            
            Color color = resonanceIndicator.color;
            color.a = alpha;
            resonanceIndicator.color = color;
        }

        // Editor-only: Validate resonance colors array
        private void OnValidate()
        {
            if (resonanceColors == null || resonanceColors.Length == 0)
            {
                resonanceColors = new Color[]
                {
                    new Color(0.2f, 0.2f, 0.2f), // Grounded
                    new Color(0.4f, 0.4f, 0.8f), // Resonating
                    new Color(0.6f, 0.6f, 1f),   // Synchronized
                    new Color(0.8f, 0.8f, 1f),   // Transcendent
                    new Color(1f, 1f, 1f)        // Unified
                };
            }
        }
    }
} 