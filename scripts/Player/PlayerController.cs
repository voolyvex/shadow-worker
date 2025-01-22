using UnityEngine;
using UnityEngine.InputSystem;
using ShadowWorker.Core;

namespace ShadowWorker.Player
{
    [RequireComponent(typeof(Rigidbody2D), typeof(PersonalityProfile))]
    public class PlayerController : MonoBehaviour
    {
        [Header("Movement")]
        [SerializeField] private float moveSpeed = 5f;
        [SerializeField] private float sprintMultiplier = 1.5f;
        [SerializeField] private float staminaDrain = 0.2f;
        [SerializeField] private float staminaRegenRate = 0.1f;
        
        [Header("Psychological Impact")]
        [SerializeField] private float movementStressRate = 0.1f;
        [SerializeField] private float sprintStressRate = 0.2f;
        [SerializeField] private float restRecoveryRate = 0.1f;
        
        private Rigidbody2D rb;
        private PersonalityProfile personalityProfile;
        private Vector2 moveInput;
        private bool isSprinting;
        private float currentStamina = 1f;
        
        // Properties
        public float CurrentStamina => currentStamina;
        public bool IsSprinting => isSprinting;
        public Vector2 MoveInput => moveInput;

        private void Awake()
        {
            rb = GetComponent<Rigidbody2D>();
            personalityProfile = GetComponent<PersonalityProfile>();
            
            // Configure rigidbody
            rb.gravityScale = 0f;
            rb.drag = 5f;
            rb.constraints = RigidbodyConstraints2D.FreezeRotation;
        }

        private void FixedUpdate()
        {
            HandleMovement();
            UpdatePsychologicalState();
            RegenerateStamina();
        }

        public void OnMove(InputValue value)
        {
            moveInput = value.Get<Vector2>();
        }

        public void OnSprint(InputValue value)
        {
            isSprinting = value.isPressed && currentStamina > 0f;
        }

        private void HandleMovement()
        {
            float currentSpeed = moveSpeed;
            
            if (isSprinting && currentStamina > 0f)
            {
                currentSpeed *= sprintMultiplier;
                currentStamina = Mathf.Max(0f, currentStamina - (staminaDrain * Time.fixedDeltaTime));
                
                if (currentStamina <= 0f)
                {
                    isSprinting = false;
                }
            }

            Vector2 movement = moveInput.normalized * currentSpeed;
            rb.velocity = movement;
        }

        private void UpdatePsychologicalState()
        {
            float stressRate = 0f;
            
            // Movement-based stress
            if (moveInput.magnitude > 0f)
            {
                stressRate += movementStressRate;
                if (isSprinting)
                {
                    stressRate += sprintStressRate;
                }
            }
            else
            {
                // Recovery when stationary
                stressRate -= restRecoveryRate;
            }

            // Apply psychological effects
            if (stressRate != 0f)
            {
                personalityProfile.ModifyDSMTrait("anxiety", stressRate * Time.fixedDeltaTime);
                
                // Reality distortion increases with high anxiety and low stamina
                float realityDistortion = (1f - currentStamina) * 0.1f;
                personalityProfile.ModifyDSMTrait("reality_distortion", realityDistortion * Time.fixedDeltaTime);
            }

            // Update psychological state
            personalityProfile.UpdatePsychologicalState(Time.fixedDeltaTime);
        }

        private void RegenerateStamina()
        {
            if (!isSprinting && currentStamina < 1f)
            {
                currentStamina = Mathf.Min(1f, currentStamina + (staminaRegenRate * Time.fixedDeltaTime));
            }
        }

        // Visual feedback
        private void OnDrawGizmos()
        {
            if (Application.isPlaying)
            {
                // Draw movement direction
                Gizmos.color = Color.green;
                Vector3 moveDirection = transform.position + new Vector3(moveInput.x, moveInput.y, 0f);
                Gizmos.DrawLine(transform.position, moveDirection);

                // Draw stamina indicator
                Gizmos.color = Color.yellow;
                float staminaRadius = 0.5f * currentStamina;
                Gizmos.DrawWireSphere(transform.position, staminaRadius);
            }
        }
    }
} 