using System;
using System.Collections;
using UnityEngine;

[RequireComponent(typeof(AudioSource))]
public abstract class GimmickEvent : MonoBehaviour
{
    [SerializeField, Header("UI用スプライト")]
    private Sprite eventSprite;

    [SerializeField, Header("UIの継続時間")]
    protected float uiDuration = 0.0f;
    
    private Action onGimmickEnd;
    
    private AudioSource audioSource;

    protected virtual void Start() {
        audioSource = GetComponent<AudioSource>();
        audioSource.loop = false;
    }
    
    public abstract bool CheckExecutionCondition();

    public virtual void ActivateGimmick(Action callback = null)
    {
        if (audioSource.clip != null) {
            audioSource.Play();
        }
        if (onGimmickEnd != null) {
            onGimmickEnd = callback;
        }
    }
    
    public virtual void EndGimmick()
    {
        if (onGimmickEnd != null) {
            onGimmickEnd?.Invoke();
        }
    }

    public Sprite GetEventSprite()
    {
        return eventSprite;
    }
    
    public float GetUIDuration()
    {
        return uiDuration;
    }
}
