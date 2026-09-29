using System;
using System.Collections;
using UnityEngine;
using Random = UnityEngine.Random;

public class PointUPEvent : GimmickEvent
{
    [SerializeField, Header("ゲーム開始からの実行間隔")]
    private float executionInterval = 25.0f;
    
    [SerializeField, Header("撃墜時に追加されるポイント")]
    private int additionalPoints = 1;
    
    [SerializeField, Header("スペシャル壁破壊時に追加されるポイント")]
    private int specialWallAdditionalPoints = 0;

    [SerializeField, Header("継続時間")]
    private float eventDuration = 15.0f;
    
    private float executePoint = 0.0f;
    private Coroutine gimmickCoroutine;

    protected override void Start()
    {
        base.Start();
        executePoint = executionInterval;
    }

    public override void ActivateGimmick(Action callback)
    {
        base.ActivateGimmick(callback);
        if (GameManager.instance == null) return;
        foreach (GameObject playerObj in GameManager.instance.memberList)
        {
            PlayerEntity player = playerObj.GetComponent<PlayerEntity>();
            player.IncreaseBonusScore(additionalPoints);
            player.IncreaseBonusWallScore(specialWallAdditionalPoints);
        }
        if (gimmickCoroutine != null) {
            StopCoroutine(gimmickCoroutine);
            EndGimmick();
        }
        gimmickCoroutine = StartCoroutine(GimmickDurationCoroutine());
    }
    
    IEnumerator GimmickDurationCoroutine()
    {
        yield return new WaitForSeconds(eventDuration);
        
        EndGimmick();
        gimmickCoroutine = null;
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

    public override void EndGimmick() {
        if (GameManager.instance != null) {
            foreach (GameObject playerObj in GameManager.instance.memberList) {
                PlayerEntity player = playerObj.GetComponent<PlayerEntity>();
                player.DecreaseBonusScore(additionalPoints);
                player.DecreaseBonusWallScore(specialWallAdditionalPoints);
            }
        }

        base.EndGimmick();
    }

    private void OnDestroy() {
        if (gimmickCoroutine != null) {
            StopCoroutine(gimmickCoroutine);
            EndGimmick();
        }
    }
}
