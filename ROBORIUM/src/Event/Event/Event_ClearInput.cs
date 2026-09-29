using UnityEngine;

public class Event_ClearInput : IEvent
{
    [SerializeField] private Interact interactComponent;
    
    public override void Init()
    {
        base.Init();

    }

    public override void Run()
    {
        base.Run();
        if (interactComponent != null)
        {
            interactComponent.OnFinishedInteract();
            Debug.Log("End");
        }

        Finish();
    }

    public override void Finish()
    {
        base.Finish();
    }
}
