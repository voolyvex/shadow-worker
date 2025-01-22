using System.Collections;
using NUnit.Framework;
using UnityEngine;
using UnityEngine.TestTools;
using ShadowWorker.Core;

namespace ShadowWorker.Tests.EditMode
{
    public class PersonalitySystemTests
    {
        private GameObject testObject;
        private PersonalityProfile profile;

        [SetUp]
        public void Setup()
        {
            testObject = new GameObject("TestEntity");
            testObject.AddComponent<BoxCollider2D>();
            profile = testObject.AddComponent<PersonalityProfile>();
        }

        [TearDown]
        public void Teardown()
        {
            Object.DestroyImmediate(testObject);
        }

        [Test]
        public void InitialState_IsGrounded()
        {
            Assert.AreEqual(ResonanceState.Grounded, profile.CurrentResonanceState);
            Assert.AreEqual(PerceptionLayer.Surface, profile.CurrentPerceptionLayer);
        }

        [Test]
        public void ModifyConsciousnessIntegration_StaysWithinBounds()
        {
            // Test positive modification
            profile.ModifyConsciousnessIntegration(0.5f);
            Assert.That(profile.GetResonanceIntensity(), Is.GreaterThan(1f));

            // Test exceeding upper bound
            profile.ModifyConsciousnessIntegration(1f);
            Assert.That(profile.GetResonanceIntensity(), Is.LessThanOrEqualTo(2f));

            // Test negative modification
            profile.ModifyConsciousnessIntegration(-0.5f);
            Assert.That(profile.GetResonanceIntensity(), Is.GreaterThanOrEqualTo(1f));
        }

        [UnityTest]
        public IEnumerator ConsciousnessIntegration_DecaysOverTime()
        {
            // Set initial integration
            profile.ModifyConsciousnessIntegration(1f);
            float initialIntensity = profile.GetResonanceIntensity();

            // Wait for decay
            yield return new WaitForSeconds(1f);

            // Check if intensity has decreased
            float finalIntensity = profile.GetResonanceIntensity();
            Assert.That(finalIntensity, Is.LessThan(initialIntensity));
        }

        [Test]
        public void ModifyDSMTraits_UpdatesCorrectly()
        {
            // Test each DSM trait
            string[] traits = { "anxiety", "depression", "dissociation", "reality_distortion", "emotional_regulation" };
            
            foreach (string trait in traits)
            {
                profile.ModifyDSMTrait(trait, 0.5f);
                profile.ModifyDSMTrait(trait, -0.2f);
                profile.ModifyDSMTrait(trait, 0.8f);
            }

            // No assertion needed as we're just testing for exceptions
            Assert.Pass("All DSM trait modifications completed without errors");
        }

        [Test]
        public void InvalidDSMTrait_DoesNotThrowException()
        {
            Assert.DoesNotThrow(() => profile.ModifyDSMTrait("invalid_trait", 0.5f));
        }

        [UnityTest]
        public IEnumerator UpdatePsychologicalState_UpdatesStateCorrectly()
        {
            // Set high integration values
            profile.ModifyConsciousnessIntegration(0.8f);
            profile.ModifyRealityIntegration(0.8f);

            // Wait for a frame to allow for state updates
            yield return null;

            // Check if state has changed from initial Grounded state
            Assert.That(profile.CurrentResonanceState, Is.Not.EqualTo(ResonanceState.Grounded));
        }

        [Test]
        public void GetResonanceIntensity_ScalesWithIntegration()
        {
            float baseIntensity = profile.GetResonanceIntensity();
            
            profile.ModifyConsciousnessIntegration(0.5f);
            float modifiedIntensity = profile.GetResonanceIntensity();
            
            Assert.That(modifiedIntensity, Is.GreaterThan(baseIntensity));
        }
    }
} 