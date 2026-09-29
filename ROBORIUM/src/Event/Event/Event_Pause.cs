using UnityEngine;

public class Event_Pause : IEvent
{
    [Header("PauseかResumeか")]
    [SerializeField] private bool isPause = false;
    // Start is called once before the first execution of Update after the MonoBehaviour is created
    public override void Init()
    {
        base.Init();
    }

    public override void Run()
    {
        base.Run();
        if (isPause)
        {
            Pauser.Pause();
        }
        else
        {
            Pauser.Resume();
        }
        Finish();

    }

    public override void Finish()
    {
        base.Finish();
    }
}
