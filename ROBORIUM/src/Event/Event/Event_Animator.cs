using UnityEngine;

public class Event_Animator : IEvent
{
    [SerializeField] private Animator targetAnimator;

    private void Awake()
    {
        targetAnimator.enabled = false;
    }
    public override void Init()
    {
        base.Init();
        targetAnimator.enabled = true;
       
    }

    public override void Run()
    {
        base.Run();

       

    }

    public override void Finish()
    {
        base.Finish();
    }
}
