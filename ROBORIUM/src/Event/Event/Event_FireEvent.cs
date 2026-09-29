using System.Threading;
using UnityEngine;

public class Event_FireEvent : IEvent
{
    [SerializeField] private EventEmitter emitter;
    [SerializeField] private float timeOutLimit = 99999;

    float timer=0.0f;
    public override void Init()
    {
        base.Init();
        if (emitter.GetCanFire())
        {

            emitter.SetOwnerEvent(this);
            emitter.Fire();
        }
    }

    public override void Run()
    {
        timer += Time.deltaTime;
        if (timer>timeOutLimit)
        {
            Finish();
        }
        base.Run();

    }

    public override void Finish()
    {
        base.Finish();
    }
}
