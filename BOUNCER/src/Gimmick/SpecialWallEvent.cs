using System;
using UnityEngine;
using Random = UnityEngine.Random;

public class SpecialWallEvent : GimmickEvent
{
    [SerializeField]
    private WallNotify[] wallNotifies;

    [SerializeField, Header("ゲーム開始からの実行間隔")]
    private float executionInterval = 25.0f;
    
    [SerializeField, Header("破壊状態の壁を復活させるか")]
    private bool respawnDestroyedWalls = true;

    private float executePoint = 0.0f;

    protected override void Start()
    {
        base.Start();
        executePoint = executionInterval;
    }

    public override void ActivateGimmick(Action callback)
    {
        base.ActivateGimmick(callback);
        foreach (WallNotify wallNotify in wallNotifies)
        {
            wallNotify.ReplaceToSpecialWall(respawnDestroyedWalls: respawnDestroyedWalls);
        }
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
