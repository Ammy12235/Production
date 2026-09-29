using UnityEngine;
/// <summary>
/// メニューからスタッフロールへ
/// </summary>
public class EaseFunc_MtoSR : EaseFunc
{

    // Start is called once before the first execution of Update after the MonoBehaviour is created
    public void Start()
    {
        base.Start();
    }
    override public void Proceed()
    {
        base.Proceed();
        SettingManager.instance.SetSettingState(eSettingState.SettingState_StuffRoll);
        //GameManager.instance.SetGameTime(45);

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
    }
}

