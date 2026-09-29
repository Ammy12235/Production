using System;
using UnityEngine;
using Random = UnityEngine.Random;

public class PlayerMultipleEvent : GimmickEvent
{
   
    [SerializeField, Header("ゲーム開始からの実行間隔")]
    private float executionInterval = 25.0f;

  
    private float executePoint = 0.0f;

    
    protected override void Start()
    {
        base.Start();
        executePoint = executionInterval;
    }

    public override void ActivateGimmick(Action callback)
    {
        base.ActivateGimmick(callback);
     
    }

    public override bool CheckExecutionCondition()
    {
        if (GameManager.instance.GetCurrentElapsedTime() >= executePoint)
        {
            executePoint += executionInterval;
            return true;
        }

        return false;
    }
}
