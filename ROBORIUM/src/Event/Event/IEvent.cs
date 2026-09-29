using UnityEngine;

public class IEvent : MonoBehaviour
{
    bool isRunning = false;
    bool isFinished = false;

    EventActuator ownerActuator;

    EventLineDrawer debugLineDrawer;
    virtual public void Init()
    {
        isRunning = true;
        isFinished = false;
    }

    virtual public void Run()
    {

    }

    virtual public void Finish()
    {
        isRunning = false;
        isFinished = true;

        if (ownerActuator != null)
        {
            ownerActuator.NotifyEventFinished(this);
        }
    }

    public bool IsRunning()
    {
        return isRunning;
    }

    public bool IsFinished()
    {
        return isFinished;
    }

    public void SetOwnerActuator(EventActuator actuator)
    {
        ownerActuator = actuator;
    }

    public EventActuator GetOwnerActuator()
    {
        return ownerActuator;
    }
}


//コピペ用

/*
public override void Init()
{
    base.Init();
}

public override void Run()
{
    base.Run();

    Finish();

}

public override void Finish()
{
    base.Finish();
}
 */