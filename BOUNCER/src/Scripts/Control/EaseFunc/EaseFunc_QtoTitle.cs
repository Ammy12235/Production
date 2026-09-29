using UnityEngine;
using UnityEngine.Rendering.PostProcessing;

public class EaseFunc_QtoTitle : EaseFunc
{
    public void Start()
    {

        base.Start();
    }

    override public void Proceed()
    {
        base.Proceed();
       
    }

    public override void LateProceed()
    {
        base.LateProceed();
    }

    override public void Reverse()
    {
        base.Reverse();
    }

    public override void LateReverse()
    {
        base.LateReverse();
    }

    public override void Entry()
    {
        base.Entry();
        SettingManager.instance.SetSettingState(eSettingState.SettingState_Quit);
    }

    public override void End()
    {
        base.End();
        
    }
}
