using UnityEngine;
using UnityEngine.Audio;

public class Event_SE_Once :IEvent
{
    [SerializeField] private AudioClip _clip;
    AudioSource audioSource;

    private void Awake()
    {
        audioSource = GetComponent<AudioSource>();
    }
    public override void Init()
    {
        base.Init();
    }

    public override void Run()
    {
        base.Run();
        audioSource.PlayOneShot(_clip);
        Finish();

    }

    public override void Finish()
    {
        base.Finish();
    }
}
