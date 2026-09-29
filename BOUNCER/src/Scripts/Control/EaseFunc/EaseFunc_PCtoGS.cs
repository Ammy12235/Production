using UnityEngine;
using System.Collections.Generic;
/// <summary>
/// プレイヤーカスタムからゲームスタートへ
/// </summary>
public class EaseFunc_PCtoGS : EaseFunc
{

    [SerializeField] private PlayerCustomManager playerCustomManager;
    [SerializeField] private List<Animator> anims;
    [SerializeField] private GameObject soundStartCallObj;
    [SerializeField] private GameObject soundStartCallObj1;
    [SerializeField] private GameObject soundStartCallObj2;

    // Start is called once before the first execution of Update after the MonoBehaviour is created
    public void Start()
    {
        base.Start();
    }
    override public void Proceed()
    {
        base.Proceed();
        SettingManager.instance.SetSettingState(eSettingState.SettingState_End);
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
        GameManager.instance.SetMemberCostume(playerCustomManager.GetCostumeIndices());
        for (int i = 0; i < anims.Count; i++) anims[i].SetBool("isStart", true);
        Instantiate(soundStartCallObj, transform.position,Quaternion.identity);
        Instantiate(soundStartCallObj1, transform.position,Quaternion.identity);
        Instantiate(soundStartCallObj2, transform.position,Quaternion.identity);


    }
}

