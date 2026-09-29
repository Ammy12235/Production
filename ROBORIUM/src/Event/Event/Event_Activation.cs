using UnityEngine;
using System.Collections.Generic;
/// <summary>
/// オブジェクトのアクティベーションイベント
/// </summary>
public class Event_Activation : IEvent
{
    [System.Serializable]
    struct ActivationInfo
    {
        public GameObject target;
        public bool isActivated;
    }
    [SerializeField] private List<ActivationInfo> _activationInfo = new();//アクティベーションイベントの対象オブジェクト
    public override void Init()
    {
        base.Init();
    }

    public override void Run()
    {
        base.Run();
        if (_activationInfo != null)
        {
            for (int i = 0; i < _activationInfo.Count; i++)
            {
                _activationInfo[i].target.SetActive(_activationInfo[i].isActivated);
            }
        }
        Finish();
    }

    public override void Finish()
    {
        base.Finish();
    }
}
