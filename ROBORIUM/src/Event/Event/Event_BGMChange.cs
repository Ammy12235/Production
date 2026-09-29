using UnityEngine;
using UnityEngine.Audio;

public class Event_BGMChange : IEvent
{
    [SerializeField] private AudioClip bgmClip;
    [SerializeField] private AudioSource bgmEmitter;
    [SerializeField] private float fadeOutTime = 0.0f;

    float initialVolume = 0.0f;
    float timer = 0.0f;
    private void Awake()
    {

    }
    public override void Init()
    {
        base.Init();
        initialVolume = bgmEmitter.volume;
    }

    public override void Run()
    {
        base.Run();

        timer += Time.deltaTime;
        bgmEmitter.volume = initialVolume * (1 - timer / fadeOutTime);
        if (timer > fadeOutTime)
        {
            timer = 0.0f;
            bgmEmitter.Stop();
            bgmEmitter.clip = bgmClip;
            bgmEmitter.volume= initialVolume;
            bgmEmitter.Play();
            Finish();
        }

    }

    public override void Finish()
    {
        base.Finish();
    }
}
