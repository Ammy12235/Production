using UnityEngine;
//パッドアサインからプレイヤーカスタムへ
public class EaseFunc_PAtoPC : EaseFunc
{
    [SerializeField] private PadAssignManager padAssignManager;
    [SerializeField] private PlayerCustomManager playerCustomManager;
    // Start is called once before the first execution of Update after the MonoBehaviour is created
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
        playerCustomManager.AllChange();
        playerCustomManager.CPUCostumeChange();
        padAssignManager.OnAssignPlayerEnd();
    }

    public override void End()
    {
        base.End();

        int playerCount = GameManager.instance.GetPlayerNum();
        Debug.Log(playerCount);
        playerCustomManager.SetPlayerInput(playerCount);
        
    }
}
