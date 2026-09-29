using UnityEngine;
using Cinemachine;
public class Event_CameraShake : IEvent
{
    CinemachineImpulseSource impulseSource;

    private void Awake()
    {
        impulseSource = GetComponent<CinemachineImpulseSource>();
        
    }
    public override void Init()
    {
        base.Init();
    }

    public override void Run()
    {
        base.Run();
        if(impulseSource!=null)
        impulseSource.GenerateImpulse();
        Finish();

    }

    public override void Finish()
    {
        base.Finish();
    }
}
